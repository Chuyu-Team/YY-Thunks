#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#include <powrprof.h>
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#pragma comment(lib, "PowrProf.lib")
#endif

namespace YY
{
	namespace Thunks
	{
#if (YY_Thunks_Support_Version < NTDDI_WIN8) && defined(YY_Thunks_Implemented)

		static
		DWORD
		WINAPI
		PowerRegisterSuspendResumeNotificationDownlevel(
			_In_ DWORD _fFlags,
			_In_ HANDLE _hRecipient,
			_Out_ PHPOWERNOTIFY _phRegistrationHandle
			)
		{
			if (!_phRegistrationHandle)
				return ERROR_INVALID_PARAMETER;
			*_phRegistrationHandle = NULL;

			const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;

					
			if (DEVICE_NOTIFY_CALLBACK == _fFlags)
			{
				auto _pDeviceNotifyInfo = (DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS*)_hRecipient;
				if(!_pDeviceNotifyInfo)
					return ERROR_INVALID_PARAMETER;

				struct DeviceNotifyTaskItem : public internal::TaskItem
				{
					DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS Parameters;
				};

				auto _pTask = (DeviceNotifyTaskItem*)HeapAlloc(_hProcessHeap, 0, sizeof(DeviceNotifyTaskItem));
				if (!_pTask)
					return ERROR_OUTOFMEMORY;

				_pTask->Parameters = *_pDeviceNotifyInfo;
				_pTask->pfnCallback = [](
					internal::TaskItem* _pWork,
					UINT   _uMsg,
					WPARAM _wParam,
					LPARAM _lParam) -> BOOL
				{
					auto _pWork2 = (DeviceNotifyTaskItem*)_pWork;

					if (WM_POWERBROADCAST == _uMsg)
					{
						_pWork2->Parameters.Callback(_pWork2->Parameters.Context, _wParam, (PVOID)_lParam);
					}

					return TRUE;
				};

				if (internal::GetGlobalThreadRunner()->AddTask(_pTask))
				{
					*_phRegistrationHandle = _pTask;
					return ERROR_SUCCESS;
				}

				HeapFree(_hProcessHeap, 0, _pTask);
				return ERROR_OUTOFMEMORY;
			}
			else
			{
				return ERROR_INVALID_PARAMETER;
			}
		}

		static
		DWORD
		WINAPI
		PowerUnregisterSuspendResumeNotificationDownlevel(
			_Inout_ HPOWERNOTIFY _hRegistrationHandle
			)
		{
			if (!_hRegistrationHandle)
				return ERROR_INVALID_PARAMETER;

			return internal::GetGlobalThreadRunner()->RemoveTask((internal::TaskItem*)_hRegistrationHandle);
		}

#endif // (YY_Thunks_Support_Version < NTDDI_WIN8) && defined(YY_Thunks_Implemented)


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		// 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
		// 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		powrprof,
		4,
		POWER_PLATFORM_ROLE,
		WINAPI,
		PowerDeterminePlatformRoleEx,
			_In_ ULONG _uVersion
			)
		{
			if (auto const _pfnPowerDeterminePlatformRoleEx = try_get_PowerDeterminePlatformRoleEx())
			{
				return _pfnPowerDeterminePlatformRoleEx(_uVersion);
			}

			if (_uVersion == POWER_PLATFORM_ROLE_V1)
			{
				return PowerDeterminePlatformRole();
			}
			else
			{
				return PlatformRoleUnspecified;
			}
		}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		// 最低受支持的客户端	Windows 8 [仅限桌面应用]
		// 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
		__DEFINE_THUNK(
		powrprof,
		12,
		DWORD,
		WINAPI,
		PowerRegisterSuspendResumeNotification,
			_In_ DWORD _fFlags,
			_In_ HANDLE _hRecipient,
			_Out_ PHPOWERNOTIFY _phRegistrationHandle
			)
		{
			if (auto const _pfnPowerRegisterSuspendResumeNotification = try_get_PowerRegisterSuspendResumeNotification())
			{
				return _pfnPowerRegisterSuspendResumeNotification(_fFlags, _hRecipient, _phRegistrationHandle);
			}
			
			return PowerRegisterSuspendResumeNotificationDownlevel(_fFlags, _hRecipient, _phRegistrationHandle);
		}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		// 最低受支持的客户端	Windows 8 [仅限桌面应用]
		// 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
		__DEFINE_THUNK(
		powrprof,
		4,
		DWORD,
		WINAPI,
		PowerUnregisterSuspendResumeNotification,
			_Inout_ HPOWERNOTIFY _hRegistrationHandle
			)
		{
			if (auto const _pfnPowerUnregisterSuspendResumeNotification = try_get_PowerUnregisterSuspendResumeNotification())
			{
				return _pfnPowerUnregisterSuspendResumeNotification(_hRegistrationHandle);
			}

			return PowerUnregisterSuspendResumeNotificationDownlevel(_hRegistrationHandle);
		}
#endif
	}
}
