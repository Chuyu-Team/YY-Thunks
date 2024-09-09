namespace YY::Thunks
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

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    InitializeTouchInjection,
        _In_ UINT32 _uMaxCount,
        _In_ DWORD  _udwMode
        )
    {
        if (const auto _pfnInitializeTouchInjection = try_get_InitializeTouchInjection())
        {
            return _pfnInitializeTouchInjection(_uMaxCount, _udwMode);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    InjectTouchInput,
        _In_ UINT32 _cCount,
        _In_ const POINTER_TOUCH_INFO* _pContacts
        )
    {
        if (const auto _pfnInjectTouchInput = try_get_InjectTouchInput())
        {
            return _pfnInjectTouchInput(_cCount, _pContacts);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    12,
    BOOL,
    WINAPI,
    GetPointerFrameTouchInfo,
        _In_ UINT32 _uPointerId,
        _Inout_ UINT32* _pcPointerCount,
        _Out_writes_opt_(*_pcPointerCount) POINTER_TOUCH_INFO* _pTouchInfo
        )
    {
        if (const auto _pfnGetPointerFrameTouchInfo = try_get_GetPointerFrameTouchInfo())
        {
            return _pfnGetPointerFrameTouchInfo(_uPointerId, _pcPointerCount, _pTouchInfo);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    16,
    BOOL,
    WINAPI,
    GetPointerFrameTouchInfoHistory,
        _In_ UINT32 _uPointerId,
        _Inout_ UINT32* _pcEntriesCount,
        _Inout_ UINT32* _pcPointerCount,
        _Out_writes_opt_(*_pcEntriesCount * *_pcPointerCount) POINTER_TOUCH_INFO* _pTouchInfo
        )
    {
        if (const auto _pfnGetPointerFrameTouchInfoHistory = try_get_GetPointerFrameTouchInfoHistory())
        {
            return _pfnGetPointerFrameTouchInfoHistory(_uPointerId, _pcEntriesCount, _pcPointerCount, _pTouchInfo);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    GetPointerInfo,
        _In_ UINT32 _uPointerId,
        _Out_writes_(1) POINTER_INFO* _pPointerInfo
        )
    {
        if (const auto _pfnGetPointerInfo = try_get_GetPointerInfo())
        {
            return _pfnGetPointerInfo(_uPointerId, _pPointerInfo);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    12,
    BOOL,
    WINAPI,
    GetPointerPenInfoHistory,
        _In_ UINT32 _uPointerId,
        _Inout_ UINT32* _pcEntriesCount,
        _Out_writes_opt_(*_pcEntriesCount) POINTER_PEN_INFO* _pPenInfo
        )
    {
        if (const auto _pfnGetPointerPenInfoHistory = try_get_GetPointerPenInfoHistory())
        {
            return _pfnGetPointerPenInfoHistory(_uPointerId, _pcEntriesCount, _pPenInfo);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    SkipPointerFrameMessages,
        _In_ UINT32 _uPointerId
        )
    {
        if (const auto _pfnSkipPointerFrameMessages = try_get_SkipPointerFrameMessages())
        {
            return _pfnSkipPointerFrameMessages(_uPointerId);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    12,
    BOOL,
    WINAPI,
    GetPointerInfoHistory,
        _In_ UINT32 _uPointerId,
        _Inout_ UINT32* _pcEntriesCount,
        _Out_writes_opt_(*_pcEntriesCount) POINTER_INFO* _pPointerInfo
        )
    {
        if (const auto _pfnGetPointerInfoHistory = try_get_GetPointerInfoHistory())
        {
            return _pfnGetPointerInfoHistory(_uPointerId, _pcEntriesCount, _pPointerInfo);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    GetPointerTouchInfo,
        _In_ UINT32 _uPointerId,
        _Out_writes_(1) POINTER_TOUCH_INFO* _pTouchInfo
        )
    {
        if (const auto _pfnGetPointerTouchInfo = try_get_GetPointerTouchInfo())
        {
            return _pfnGetPointerTouchInfo(_uPointerId, _pTouchInfo);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    12,
    BOOL,
    WINAPI,
    GetPointerTouchInfoHistory,
        _In_ UINT32 _uPointerId,
        _Inout_ UINT32* _pcEntriesCount,
        _Out_writes_opt_(*_pcEntriesCount) POINTER_TOUCH_INFO* _pTouchInfo
        )
    {
        if (const auto _pfnGetPointerTouchInfoHistory = try_get_GetPointerTouchInfoHistory())
        {
            return _pfnGetPointerTouchInfoHistory(_uPointerId, _pcEntriesCount, _pTouchInfo);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    0,
    BOOL,
    WINAPI,
    IsMouseInPointerEnabled
        )
    {
        if (const auto _pfnIsMouseInPointerEnabled = try_get_IsMouseInPointerEnabled())
        {
            return _pfnIsMouseInPointerEnabled();
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    EnableMouseInPointer,
        _In_ BOOL _bEnable
        )
    {
        if (const auto _pfnEnableMouseInPointer = try_get_EnableMouseInPointer())
        {
            return _pfnEnableMouseInPointer(_bEnable);
        }

        if (_bEnable == FALSE)
        {
            return TRUE;
        }

        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }
#endif
}
