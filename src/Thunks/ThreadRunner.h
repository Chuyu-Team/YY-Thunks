#pragma once

namespace YY::Thunks::internal
{
	namespace 
	{
		typedef BOOL(WINAPI TaskCallback)(
			struct TaskItem* _pWork,
			UINT   _uMsg,
			WPARAM _wParam,
			LPARAM _lParam
			);

		struct TaskItem
		{
			TaskItem* pNext;

			TaskCallback* pfnCallback;
		};

		constexpr auto WM_RUNNER_CALL = WM_USER + 100;

		struct ThreadRunner
		{
			TaskItem* pCallbackList;
			volatile TaskItem* pPendingCallbackList;
			volatile HWND hWnd;
			volatile ULONG uRef;
				
			static LRESULT WINAPI WindowProcW(
				HWND   _hWnd,
				UINT   _uMsg,
				WPARAM _wParam,
				LPARAM _lParam
				)
			{
				if (_uMsg == WM_CLOSE)
				{
					auto _pData = (ThreadRunner*)GetWindowLongPtrW(_hWnd, GWLP_USERDATA);
					if (_pData)
					{
						if (!_pData->TryClean())
						{
							if (_wParam == 0)
							{
								return 0;
							}
							else
							{
								// 表示强制清理，这时虽然上面释放失败，但是为了避免崩溃
								// 我们仅仅停止窗口。
								_pData->hWnd = NULL;
							}
						}
					}
					DestroyWindow(_hWnd);
					return 0;
				}
				else if (_uMsg == WM_DESTROY)
				{
					PostQuitMessage(0);
				}
				else
				{
					auto _pData = (ThreadRunner*)GetWindowLongPtrW(_hWnd, GWLP_USERDATA);
					if (_pData && (_pData->pCallbackList || _pData->pPendingCallbackList))
					{
						const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;

						// 进行CallBack
						TaskItem* _pLastCallbackPrev = (TaskItem*)&_pData->pCallbackList;
						for (auto _pCallbackPrev = _pLastCallbackPrev; _pCallbackPrev;)
						{
							if (!_pCallbackPrev->pNext)
								break;

							auto _pNext = _pCallbackPrev->pNext;
							if (!_pNext)
								break;

							if (auto _pfnCallback = _pNext->pfnCallback)
							{
								_pLastCallbackPrev = _pNext;
								_pfnCallback(_pNext, _uMsg, _wParam, _lParam);
							}
							else
							{
								_pCallbackPrev->pNext = _pNext->pNext;
								// 需要移除
								HeapFree(_hProcessHeap, 0, _pNext);
							}								
						}

						// 合并 pPendingCallbackList 至 pCallbackList
						auto _pPendingCallbackList = (TaskItem*)InterlockedExchangePointer((PVOID*)&_pData->pPendingCallbackList, NULL);
						if (_pPendingCallbackList)
						{
							// 将顺序颠倒处理，因为插入后顺序就是倒的
							auto _pPendingCallbackListFirst = _pPendingCallbackList;
							for (; _pPendingCallbackList->pNext; _pPendingCallbackList = _pPendingCallbackList->pNext)
							{
								_pPendingCallbackListFirst->pNext = _pPendingCallbackList->pNext;
								_pPendingCallbackListFirst = _pPendingCallbackList->pNext;
							}
							_pPendingCallbackList->pNext = nullptr;

							_pLastCallbackPrev->pNext = _pPendingCallbackListFirst;

							// 开始便利 新增的Callback元素
							for (auto _pCallbackPrev = _pLastCallbackPrev; _pCallbackPrev;)
							{
								if (!_pCallbackPrev->pNext)
									break;

								auto _pNext = _pCallbackPrev->pNext;
								if (!_pNext)
									break;

								if (auto _pfnCallback = _pNext->pfnCallback)
								{
									_pLastCallbackPrev = _pNext;
									_pfnCallback(_pNext, _uMsg, _wParam, _lParam);
								}
								else
								{
									_pCallbackPrev->pNext = _pNext->pNext;
									// 需要移除
									HeapFree(_hProcessHeap, 0, _pNext);
								}
							}								
						}


					}
				}
				return ::DefWindowProcW(_hWnd, _uMsg, _wParam, _lParam);
			}

			bool AddRef()
			{
				auto _uCurrentRef = uRef;

				for (;;)
				{
					if (_uCurrentRef == MAXUINT32)
					{
						SwitchToThread();
						_uCurrentRef = uRef;
						continue;
					}
					else if (_uCurrentRef == 0)
					{
						// 第一次，尝试加锁
						const auto _uLastRef = InterlockedCompareExchange(&uRef, MAXUINT32, 0);
						if (_uLastRef != 0)
						{
							_uCurrentRef = _uLastRef;
							continue;
						}

						struct ThreadInfo
						{
							HANDLE hEvent;
							ThreadRunner* pWork;
						};

						LSTATUS _lStatus = ERROR_SUCCESS;
						ThreadInfo _Info = { NULL, this };

						do
						{
							_Info.hEvent = CreateEventW(nullptr, FALSE, FALSE, NULL);
							if (!_Info.hEvent)
							{
								_lStatus = GetLastError();
								break;
							}

							// 成功锁定 开始创建创建
							auto _hThread = CreateThread(
								nullptr,
								0,
								[](LPVOID lpParameter) -> DWORD
								{
									auto& _Info = *(ThreadInfo*)lpParameter;

									static bool bRegister = false;
									if (!bRegister)
									{
										WNDCLASSW wc = {};

										wc.style = CS_HREDRAW | CS_VREDRAW;
										wc.lpfnWndProc = &DefWindowProcW;
										wc.cbClsExtra = 0;
										wc.cbWndExtra = 0;
										wc.hInstance = NULL;
										wc.hIcon = NULL;
										wc.hCursor = NULL;
										wc.hbrBackground = NULL;
										wc.lpszMenuName = NULL;
										wc.lpszClassName = L"YY_Thunks_Work_Wnd";

										if (RegisterClassW(&wc) == FALSE && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
											return FALSE;
										bRegister = true;
									}

									auto _hWnd = CreateWindowExW(WS_EX_NOACTIVATE, L"YY_Thunks_Work_Wnd", nullptr, WS_POPUP, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
									if (!_hWnd)
										return GetLastError();

									_Info.pWork->hWnd = _hWnd;

									SetWindowLongPtrW(_hWnd, GWLP_USERDATA, (LONG_PTR)_Info.pWork);
									SetWindowLongPtrW(_hWnd, GWLP_WNDPROC, (LONG_PTR)&ThreadRunner::WindowProcW);

									SetEvent(_Info.hEvent);

									MSG _Msg;

									for (;;)
									{
										auto _bRet = GetMessageW(&_Msg, NULL, 0, 0);
										if (_bRet == -1)
											continue;
										if (!_bRet)
											break;

										TranslateMessage(&_Msg);
										DispatchMessageW(&_Msg);
									}

									return 0;
								},
								&_Info,
								0,
								nullptr);

							if (!_hThread)
							{
								_lStatus = GetLastError();
								break;
							}

							HANDLE _Handles[] = { _Info.hEvent, _hThread };

							const auto _uResult = WaitForMultipleObjects(_countof(_Handles), _Handles, FALSE, 15 * 1000);

							switch (_uResult)
							{
							case WAIT_OBJECT_0:
								// 成功初始化完成
								break;
							case WAIT_OBJECT_0 + 1:
								// 失败
								_lStatus = ERROR_FUNCTION_FAILED;
								GetExitCodeThread(_hThread, (DWORD*)&_lStatus);
								break;
							default:
								_lStatus = ERROR_FUNCTION_FAILED;
								break;
							}

							if(_hThread)
								CloseHandle(_hThread);

						} while (false);

						if (_Info.hEvent)
						{
							CloseHandle(_Info.hEvent);
						}

						if (_lStatus == ERROR_SUCCESS)
						{
							// 成功处理！这里给窗口额外增加一次引用
							InterlockedExchange(&uRef, 2);
							break;
						}
						else
						{
							// 失败
							InterlockedExchange(&uRef, 0);
							return false;
						}
					}
					else
					{
						// 尝试增加一次引用
						const auto _uLastRef = InterlockedCompareExchange(&uRef, _uCurrentRef + 1, _uCurrentRef);
						if (_uLastRef != _uCurrentRef)
						{
							_uCurrentRef = _uLastRef;
							continue;
						}
						break;
					}
				}

				return true;
			}

			void Release()
			{
				// 尝试锁定
				auto _uCurrentRef = uRef;

				for (;;)
				{
					if (_uCurrentRef == MAXUINT32)
					{
						// 已经锁定，稍后再试
						SwitchToThread();
						_uCurrentRef = uRef;
					}
					else if (_uCurrentRef == 0 || _uCurrentRef == 1)
					{
						// 当前没有引用！！！
						break;
					}
					else
					{
						// 成功锁定，通知线程释放……
						if (InterlockedDecrement(&uRef) == 1)
						{
							// 引用归 1，异步通知窗口尝试退出。
							PostMessageW(hWnd, WM_CLOSE, 0, 0);
						}
						break;
					}
				}
			}

			bool AddTask(TaskItem* _pWork)
			{
				if (_pWork == nullptr || _pWork->pfnCallback == nullptr)
					return false;

				if (!AddRef())
					return false;

				// 先添加到 pPendingCallbackList，然后工作线程会将结果合并到 pCallbackList
				auto _pLast = (TaskItem*)pPendingCallbackList;

				for (;;)
				{
					_pWork->pNext = _pLast;

					auto _pLastLast = (TaskItem*)InterlockedCompareExchangePointer((PVOID*)&pPendingCallbackList, _pWork, _pLast);

					if (_pLastLast == _pLast)
					{
						break;
					}
					_pLast = _pLastLast;
				}

				return true;
			}

			LSTATUS RemoveTask(TaskItem* _pWork)
			{
				if(!_pWork)
					return ERROR_INVALID_PARAMETER;

				// Callback设置为 null 后再适合的时机会自动删除
				if (!InterlockedExchangePointer((PVOID*)&_pWork->pfnCallback, nullptr))
					return ERROR_INVALID_PARAMETER;

				Release();
				return ERROR_SUCCESS;
			}

			bool TryClean()
			{
				const auto _uLastRef = InterlockedCompareExchange(&uRef, MAXUINT32, 1);
				if (_uLastRef != 1)
				{
					// 引用已经重新增加，所以不需要删除了。
					return false;
				}
				// 成功锁定，将一些关键变量重置
				auto _hWnd = hWnd;
				hWnd = NULL;
				auto _pCallbackList = (TaskItem*)InterlockedExchangePointer((PVOID*)&pCallbackList, nullptr);
				auto _pPendingCallbackList = (TaskItem*)InterlockedExchangePointer((PVOID*)&pPendingCallbackList, nullptr);
				SetWindowLongPtrW(_hWnd, GWLP_USERDATA, NULL);
				// 解锁
				InterlockedExchange(&uRef, 0);

				const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;

				// 删除List内存
				for (; _pCallbackList; )
				{
					auto _pNext = _pCallbackList->pNext;
					HeapFree(_hProcessHeap, 0, _pCallbackList);
					_pCallbackList = _pNext;
				}

				for (; _pPendingCallbackList; )
				{
					auto _pNext = _pPendingCallbackList->pNext;
					HeapFree(_hProcessHeap, 0, _pPendingCallbackList);
					_pPendingCallbackList = _pNext;
				}

				return true;
			}
		};

		static ThreadRunner* __fastcall GetGlobalThreadRunner()
		{
			static ThreadRunner g_YYWork;
			static LONG g_RunOnce;

			if (g_RunOnce == 0)
			{
				if (!InterlockedBitTestAndSet(&g_RunOnce, 0))
				{
					atexit(
						[]()
						{
							if (auto _hWnd = GetGlobalThreadRunner()->hWnd)
							{
								SendMessageW(_hWnd, WM_CLOSE, 1, 0);
							}
						});
				}					
			}
			return &g_YYWork;
		}
	}
}
