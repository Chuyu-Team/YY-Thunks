#pragma once

namespace YY
{
	namespace Thunks
	{
		namespace internal
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
									// ��ʾǿ��������ʱ��Ȼ�����ͷ�ʧ�ܣ�����Ϊ�˱������
									// ���ǽ���ֹͣ���ڡ�
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

							// ����CallBack
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
									// ��Ҫ�Ƴ�
									HeapFree(_hProcessHeap, 0, _pNext);
								}								
							}

							// �ϲ� pPendingCallbackList �� pCallbackList
							auto _pPendingCallbackList = (TaskItem*)InterlockedExchangePointer((PVOID*)&_pData->pPendingCallbackList, NULL);
							if (_pPendingCallbackList)
							{
								// ��˳��ߵ�������Ϊ�����˳����ǵ���
								auto _pPendingCallbackListFirst = _pPendingCallbackList;
								for (; _pPendingCallbackList->pNext; _pPendingCallbackList = _pPendingCallbackList->pNext)
								{
									_pPendingCallbackListFirst->pNext = _pPendingCallbackList->pNext;
									_pPendingCallbackListFirst = _pPendingCallbackList->pNext;
								}
								_pPendingCallbackList->pNext = nullptr;

								_pLastCallbackPrev->pNext = _pPendingCallbackListFirst;

								// ��ʼ���� ������CallbackԪ��
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
										// ��Ҫ�Ƴ�
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
							// ��һ�Σ����Լ���
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

								// �ɹ����� ��ʼ��������
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
									// �ɹ���ʼ�����
									break;
								case WAIT_OBJECT_0 + 1:
									// ʧ��
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
								// �ɹ�������������ڶ�������һ������
								InterlockedExchange(&uRef, 2);
								break;
							}
							else
							{
								// ʧ��
								InterlockedExchange(&uRef, 0);
								return false;
							}
						}
						else
						{
							// ��������һ������
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
					// ��������
					auto _uCurrentRef = uRef;

					for (;;)
					{
						if (_uCurrentRef == MAXUINT32)
						{
							// �Ѿ��������Ժ�����
							SwitchToThread();
							_uCurrentRef = uRef;
						}
						else if (_uCurrentRef == 0 || _uCurrentRef == 1)
						{
							// ��ǰû�����ã�����
							break;
						}
						else
						{
							// �ɹ�������֪ͨ�߳��ͷš���
							if (InterlockedDecrement(&uRef) == 1)
							{
								// ���ù� 1���첽֪ͨ���ڳ����˳���
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

					// ����ӵ� pPendingCallbackList��Ȼ�����̻߳Ὣ����ϲ��� pCallbackList
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

					// Callback����Ϊ null �����ʺϵ�ʱ�����Զ�ɾ��
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
						// �����Ѿ��������ӣ����Բ���Ҫɾ���ˡ�
						return false;
					}
					// �ɹ���������һЩ�ؼ���������
					auto _hWnd = hWnd;
					hWnd = NULL;
					auto _pCallbackList = (TaskItem*)InterlockedExchangePointer((PVOID*)&pCallbackList, nullptr);
					auto _pPendingCallbackList = (TaskItem*)InterlockedExchangePointer((PVOID*)&pPendingCallbackList, nullptr);
					SetWindowLongPtrW(_hWnd, GWLP_USERDATA, NULL);
					// ����
					InterlockedExchange(&uRef, 0);

					const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;

					// ɾ��List�ڴ�
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
}
