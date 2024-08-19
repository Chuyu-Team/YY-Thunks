﻿namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    RegisterPointerDeviceNotifications,
        _In_ HWND _hWindow,
        _In_ BOOL _bNotifyRange
        )
    {
        if (const auto _pfnRegisterPointerDeviceNotifications = try_get_RegisterPointerDeviceNotifications())
        {
            return _pfnRegisterPointerDeviceNotifications(_hWindow, _bNotifyRange);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    GetPointerDevices,
        _Inout_ UINT32* _uDeviceCount,
        _Out_writes_opt_(*_uDeviceCount) POINTER_DEVICE_INFO* _pPointerDevices
        )
    {
        if (const auto _pfnRGetPointerDevices = try_get_GetPointerDevices())
        {
            return _pfnRGetPointerDevices(_uDeviceCount, _pPointerDevices);
        }

        SetLastError(ERROR_SYSTEM_DEVICE_NOT_FOUND);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    GetPointerDevice,
        _In_ HANDLE _hDevice,
        _Out_writes_(1) POINTER_DEVICE_INFO* _pPointerDevice
        )
    {
        if (const auto _pfnGetPointerDevice = try_get_GetPointerDevice())
        {
            return _pfnGetPointerDevice(_hDevice, _pPointerDevice);
        }
        
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    GetPointerPenInfo,
        _In_ UINT32 _uPointerId,
        _Out_writes_(1) POINTER_PEN_INFO* _pPenInfo
        )
    {
        if (const auto _pfnGetPointerPenInfo = try_get_GetPointerPenInfo())
        {
            return _pfnGetPointerPenInfo(_uPointerId, _pPenInfo);
        }
        
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    GetPointerType,
        _In_ UINT32 _uPointerId,
        _Out_ POINTER_INPUT_TYPE* _pPointerType
        )
    {
        if (const auto _pfnGetPointerType = try_get_GetPointerType())
        {
            return _pfnGetPointerType(_uPointerId, _pPointerType);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-initializetouchinjection
    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    InitializeTouchInjection,
        _In_ UINT32 maxCount,
        _In_ DWORD  dwMode
        )
    {
        if (const auto _pfnInitializeTouchInjection = try_get_InitializeTouchInjection())
        {
            return _pfnInitializeTouchInjection(maxCount, dwMode);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-injecttouchinput
    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    InjectTouchInput,
        _In_ UINT32 count,
        _In_ const POINTER_TOUCH_INFO *contacts
        )
    {
        if (const auto _pfnInjectTouchInput = try_get_InjectTouchInput())
        {
            return _pfnInjectTouchInput(count, contacts);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


}
