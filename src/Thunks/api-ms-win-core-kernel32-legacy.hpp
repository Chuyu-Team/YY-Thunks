

namespace YY
{
	namespace Thunks
	{
#if (YY_Thunks_Support_Version < NTDDI_WIN7) && defined(YY_Thunks_Implemented)

		struct PowerRequestTaskItem : public internal::TaskItem
		{
			DWORD RequestTypeCount[4];
			DWORD uCount;
			// 当前实际生效的 Flags
			LONG fFlagsCurrent;
		};

#if defined(_WIN64)
		__declspec(align(16))
#endif
		union PowerRequestInfo
		{
			struct
			{
				// 引用计数
				size_t uRef;
				PowerRequestTaskItem* pTask;
			};
#if defined(_WIN64)
			long long uRaw[2];
#else
			long long uRaw;
#endif

			LSTATUS AddRef()
			{
				auto _pRunner = internal::GetGlobalThreadRunner();
				if (!_pRunner->AddRef())
				{
					return ERROR_FUNCTION_FAILED;
				}

				PowerRequestTaskItem* _pTask = nullptr;
				PowerRequestInfo _Current = *this;
				PowerRequestInfo _New;

				for (;;)
				{
					if (_Current.uRef == 0)
					{
						if (!_pTask)
						{
							const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
							_pTask = (PowerRequestTaskItem*)HeapAlloc(_hProcessHeap, 0, sizeof(PowerRequestTaskItem));
							if (!_pTask)
							{
								_pRunner->Release();
								return ERROR_OUTOFMEMORY;
							}
							memset(_pTask, 0, sizeof(*_pTask));

							_pTask->pfnCallback = &PowerRequestInfo::Callback;
						}

						_New.uRef = 1;
						_New.pTask = _pTask;
					}
					else
					{
						_New.uRef = _Current.uRef + 1;
						_New.pTask = _Current.pTask;
					}
#if defined(_WIN64)

					if (InterlockedCompareExchange128(this->uRaw, _New.uRaw[1], _New.uRaw[0], _Current.uRaw))
						break;
#else
					PowerRequestInfo _Last;
					_Last.uRaw = InterlockedCompareExchange64(&this->uRaw, _New.uRaw, _Current.uRaw);

					if (_Last.uRaw == _Current.uRaw)
						break;
					_Current.uRaw = _Last.uRaw;
#endif
				}

				// _pTask没有成功交换过去，所以删除。
				if (_pTask && _pTask != pTask)
				{
					const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
					HeapFree(_hProcessHeap, 0, _pTask);
				}

				LSTATUS _lStatus = ERROR_SUCCESS;

				if (_New.uRef == 1)
				{
					if (!_pRunner->AddTask(pTask))
					{
						_lStatus = ERROR_FUNCTION_FAILED;
					}
				}
				_pRunner->Release();

				return _lStatus;
			}

			LSTATUS Release()
			{
				PowerRequestInfo _Current = *this;
				PowerRequestInfo _New;

				for (;;)
				{
					// 当前没有引用
					if (_Current.uRef == 0)
						return ERROR_INVALID_PARAMETER;

					_New.uRef = _Current.uRef - 1;
					if (_New.uRef == 0)
					{
						_New.pTask = nullptr;
					}
					else
					{
						_New.pTask = _Current.pTask;
					}
#if defined(_WIN64)
					if (InterlockedCompareExchange128(this->uRaw, _New.uRaw[1], _New.uRaw[0], _Current.uRaw))
						break;
#else
					PowerRequestInfo _Last;
					_Last.uRaw = InterlockedCompareExchange64(&this->uRaw, _New.uRaw, _Current.uRaw);

					if (_Last.uRaw == _Current.uRaw)
						break;
					_Current.uRaw = _Last.uRaw;
#endif
				}

				if (_New.pTask == nullptr)
				{
					auto _pRunner = internal::GetGlobalThreadRunner();
					_pRunner->RemoveTask(_Current.pTask);
				}

				return ERROR_SUCCESS;
			}

			static BOOL WINAPI Callback(
				internal::TaskItem* _pWork,
				UINT   _uMsg,
				WPARAM _wParam,
				LPARAM _lParam)
			{
				if (_uMsg == internal::WM_RUNNER_CALL && (UINT_PTR)_pWork == (UINT_PTR)_wParam)
				{
					auto _pWork2 = (PowerRequestTaskItem*)_pWork;

					LONG _uNewFlags = ES_CONTINUOUS;

					if (_pWork2->RequestTypeCount[PowerRequestDisplayRequired])
					{
						_uNewFlags |= ES_DISPLAY_REQUIRED;
					}

					if (_pWork2->RequestTypeCount[PowerRequestSystemRequired])
					{
						_uNewFlags |= ES_SYSTEM_REQUIRED;
					}

					if (_pWork2->RequestTypeCount[PowerRequestAwayModeRequired])
					{
						_uNewFlags |= ES_AWAYMODE_REQUIRED;
					}

					if (_pWork2->RequestTypeCount[PowerRequestExecutionRequired])
					{
						// https://learn.microsoft.com/zh-cn/windows/win32/api/winbase/nf-winbase-powersetrequest
						// 文档显示对于传统 S3系统等效于 PowerRequestSystemRequired
						// 所以我们等效 ES_SYSTEM_REQUIRED 处理
						_uNewFlags |= ES_SYSTEM_REQUIRED;
					}

					if (_uNewFlags != _pWork2->fFlagsCurrent)
					{
						_pWork2->fFlagsCurrent = _uNewFlags;

						SetThreadExecutionState(_uNewFlags);
					}

					// 参考 PowerClearRequest，为了保证总是刷新到系统
					if(_lParam)
						((PowerRequestInfo*)_lParam)->Release();
				}
				return TRUE;
			}
		};

		static PowerRequestInfo* GetPowerRequestCache()
		{
			static PowerRequestInfo g_PowerRequestCache;
			return &g_PowerRequestCache;
		}

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		// 最低受支持的客户端	Windows 7 [仅限桌面应用]
		// 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
		__DEFINE_THUNK(
		kernel32,
		4,
		HANDLE,
		WINAPI,
		PowerCreateRequest,
			_In_ PREASON_CONTEXT _pContext
			)
		{
			if (const auto _pfnPowerCreateRequest = try_get_PowerCreateRequest())
			{
				return _pfnPowerCreateRequest(_pContext);
			}
			
			if (_pContext == nullptr || _pContext->Version != POWER_REQUEST_CONTEXT_VERSION)
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return INVALID_HANDLE_VALUE;
			}

			// 因为 PowerCreateRequest创建的句柄需要使用 CloseHandle关闭
			// 所以我们随便复制一个句柄给它……
			HANDLE hTargetHandle;
			if (DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(), &hTargetHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
			{
				return hTargetHandle;
			}

			return INVALID_HANDLE_VALUE;
		}
#endif
		
#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		// 最低受支持的客户端	Windows 7 [仅限桌面应用]
		// 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
		__DEFINE_THUNK(
		kernel32,
		8,
		BOOL,
		WINAPI,
		PowerSetRequest,
			_In_ HANDLE _hPowerRequest,
			_In_ POWER_REQUEST_TYPE _eRequestType
			)
		{
			if (const auto _pfnPowerSetRequest = try_get_PowerSetRequest())
			{
				return _pfnPowerSetRequest(_hPowerRequest, _eRequestType);
			}

			if ((DWORD)_eRequestType >= (DWORD)_countof(PowerRequestTaskItem::RequestTypeCount))
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}

			const auto _uProcessId = GetProcessId(_hPowerRequest);
			if (_uProcessId == 0)
			{
				return FALSE;
			}

			if (_uProcessId != GetCurrentProcessId())
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}

			auto _lStatus = GetPowerRequestCache()->AddRef();
			if (_lStatus == ERROR_SUCCESS)
			{
				auto _pTask = GetPowerRequestCache()->pTask;
				InterlockedIncrement(&_pTask->uCount);

				if (InterlockedIncrement(&_pTask->RequestTypeCount[_eRequestType]) == 0)
				{
					// 状态发生变化，通知进行更改
					PostMessageW(
						internal::GetGlobalThreadRunner()->hWnd,
						internal::WM_RUNNER_CALL,
						(WPARAM)_pTask,
						0);
				}

				return TRUE;
			}

			SetLastError(_lStatus);
			return FALSE;
		}
#endif
		
#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		// 最低受支持的客户端	Windows 7 [仅限桌面应用]
		// 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
		__DEFINE_THUNK(
		kernel32,
		8,
		BOOL,
		WINAPI,
		PowerClearRequest,
			_In_ HANDLE _hPowerRequest,
			_In_ POWER_REQUEST_TYPE _eRequestType
			)
		{
			if (const auto _pfnPowerClearRequest = try_get_PowerClearRequest())
			{
				return _pfnPowerClearRequest(_hPowerRequest, _eRequestType);
			}

			if ((DWORD)_eRequestType >= (DWORD)_countof(PowerRequestTaskItem::RequestTypeCount))
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}

			const auto _uProcessId = GetProcessId(_hPowerRequest);
			if (_uProcessId == 0)
			{
				return FALSE;
			}

			if (_uProcessId != GetCurrentProcessId())
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}

			auto _pTask = GetPowerRequestCache()->pTask;
			if (!_pTask)
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}

			auto _uCurrent = _pTask->RequestTypeCount[_eRequestType];
			for (;;)
			{
				if (_uCurrent == 0)
				{
					SetLastError(ERROR_INVALID_PARAMETER);
					return FALSE;
				}

				const auto _uLast = InterlockedCompareExchange(&_pTask->RequestTypeCount[_eRequestType], _uCurrent - 1, _uCurrent);
				if (_uLast != _uCurrent)
				{
					_uCurrent = _uLast;
					continue;
				}

				if (_uCurrent == 1)
				{
					// 状态发生变化，通知进行更改
					if (InterlockedDecrement(&_pTask->uCount) == 0)
					{
						// 引用归 0，必须保证状态刷新到系统，所以我们不在这里释放引用。
						PostMessageW(
							internal::GetGlobalThreadRunner()->hWnd,
							internal::WM_RUNNER_CALL,
							(WPARAM)_pTask,
							(LPARAM)GetPowerRequestCache());

						return TRUE;
					}
					else
					{
						PostMessageW(
							internal::GetGlobalThreadRunner()->hWnd,
							internal::WM_RUNNER_CALL,
							(WPARAM)_pTask,
							0);
					}
				}

				break;
			}

			GetPowerRequestCache()->Release();
			return TRUE;
		}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 支持的最低客户端	Windows Vista [桌面应用程序 |UWP 应用]
		// 支持的最低服务器	Windows Server 2008 [桌面应用程序 |UWP 应用]
		__DEFINE_THUNK(
		kernel32,
		8,
		BOOL,
		WINAPI,
		SetFileCompletionNotificationModes,
			_In_ HANDLE FileHandle,
			_In_ UCHAR Flags
			)
		{
			if (const auto _pfnSetFileCompletionNotificationModes = try_get_SetFileCompletionNotificationModes())
			{
				return _pfnSetFileCompletionNotificationModes(FileHandle, Flags);
			}

			// 初步看起来没有什么的，只是会降低完成端口的效率。
			// 至少需要 Vista才支持 FileIoCompletionNotificationInformation
			// 只能假定先返回成功。
			return TRUE;
		}
#endif
	}//namespace Thunks

} //namespace YY