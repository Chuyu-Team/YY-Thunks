#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#include <winuser.h>
#endif

namespace YY
{
	namespace Thunks
	{
#ifndef CONST_DEVICE_NOTIFY_WINDOW_HANDLE
#define CONST_DEVICE_NOTIFY_WINDOW_HANDLE (HPOWERNOTIFY)0x1001
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		// Windows 8 [仅限桌面应用]，Windows Server 2012 [仅限桌面应用]
		__DEFINE_THUNK(
		user32,
		8,
		HPOWERNOTIFY,
		WINAPI,
		RegisterSuspendResumeNotification,
			IN HANDLE _hRecipient,
			IN DWORD _fFlags
			)
		{
			if (const auto _pfnRegisterSuspendResumeNotification = try_get_RegisterSuspendResumeNotification())
			{
				return _pfnRegisterSuspendResumeNotification(_hRecipient, _fFlags);
			}

			if (DEVICE_NOTIFY_CALLBACK == _fFlags)
			{
				HPOWERNOTIFY _hRegistrationHandle = nullptr;

				auto _lStatus = PowerRegisterSuspendResumeNotificationDownlevel(DEVICE_NOTIFY_CALLBACK, _hRecipient, &_hRegistrationHandle);
				if (_lStatus == ERROR_SUCCESS)
				{
					return _hRegistrationHandle;
				}
				else
				{
					SetLastError(_lStatus);
					return nullptr;
				}
			}
			else if (DEVICE_NOTIFY_WINDOW_HANDLE == _fFlags)
			{
				// 这种情况需要向窗口发送电源通知，但是老版本系统不支持 现代化睡眠
				// 本身就是可以正确处理这种情况的。所以我们忽略参数，直接返回一个标记常量即可。
				UNREFERENCED_PARAMETER(_hRecipient);

				return CONST_DEVICE_NOTIFY_WINDOW_HANDLE;
			}
			else
			{
				// 仅支持这二个参数
				SetLastError(ERROR_INVALID_PARAMETER);
				return nullptr;
			}
		}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		// Windows 8 [仅限桌面应用]，Windows Server 2012 [仅限桌面应用]
		__DEFINE_THUNK(
		user32,
		4,
		BOOL,
		WINAPI,
		UnregisterSuspendResumeNotification,
			IN HPOWERNOTIFY _hHandle
			)
		{
			if (const auto _pfnUnregisterSuspendResumeNotification = try_get_UnregisterSuspendResumeNotification())
			{
				return _pfnUnregisterSuspendResumeNotification(_hHandle);
			}

			if (_hHandle == nullptr)
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}
			else if (_hHandle == CONST_DEVICE_NOTIFY_WINDOW_HANDLE)
			{
				// 忽略，这是 DEVICE_NOTIFY_WINDOW_HANDLE 的常量
				return TRUE;
			}
			else
			{
				// 来自 DEVICE_NOTIFY_CALLBACK的注册
				auto _lStatus = PowerUnregisterSuspendResumeNotificationDownlevel(_hHandle);
				if (_lStatus == ERROR_SUCCESS)
				{
					return TRUE;
				}
				else
				{
					SetLastError(_lStatus);
					return FALSE;
				}
			}
		}
#endif
	}
}
