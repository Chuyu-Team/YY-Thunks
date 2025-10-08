#if (YY_Thunks_Target < __WindowsNT6_3)
#include <winuser.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)
    // Windows 8 [仅限桌面应用]，Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    20,
    UINT_PTR,
    WINAPI,
    SetCoalescableTimer,
        _In_opt_ HWND _hWnd,
        _In_ UINT_PTR _nIDEvent,
        _In_ UINT _uElapse,
        _In_opt_ TIMERPROC _lpTimerFunc,
        _In_ ULONG _uToleranceDelay
        )
    {
        auto _pfnSetCoalescableTimer = try_get_SetCoalescableTimer();
        if (_pfnSetCoalescableTimer)
        {
            return _pfnSetCoalescableTimer(_hWnd, _nIDEvent, _uElapse, _lpTimerFunc, _uToleranceDelay);
        }

        return SetTimer(_hWnd, _nIDEvent, _uElapse, _lpTimerFunc);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    PhysicalToLogicalPoint,
        _In_ HWND _hWnd,
        _Inout_ LPPOINT _pPoint
        )
    {
        if (auto const _pfnPhysicalToLogicalPoint = try_get_PhysicalToLogicalPoint())
        {
            return _pfnPhysicalToLogicalPoint(_hWnd, _pPoint);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    LogicalToPhysicalPoint,
        _In_ HWND _hWnd,
        _Inout_ LPPOINT _pPoint
        )
    {
        if (auto const _pfnLogicalToPhysicalPoint = try_get_LogicalToPhysicalPoint())
        {
            return _pfnLogicalToPhysicalPoint(_hWnd, _pPoint);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_3)

    // 最低受支持的客户端	Windows 8.1 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    PhysicalToLogicalPointForPerMonitorDPI,
        _In_ HWND _hWnd,
        _Inout_ LPPOINT _pPoint
        )
    {
        if (auto const _pfnPhysicalToLogicalPointForPerMonitorDPI = try_get_PhysicalToLogicalPointForPerMonitorDPI())
        {
            return _pfnPhysicalToLogicalPointForPerMonitorDPI(_hWnd, _pPoint);
        }

        return PhysicalToLogicalPoint(_hWnd, _pPoint);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_3)

    // 最低受支持的客户端	Windows 8.1 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    LogicalToPhysicalPointForPerMonitorDPI,
        _In_ HWND _hWnd,
        _Inout_ LPPOINT _pPoint
        )
    {
        if (auto const _pfnLogicalToPhysicalPointForPerMonitorDPI = try_get_LogicalToPhysicalPointForPerMonitorDPI())
        {
            return _pfnLogicalToPhysicalPointForPerMonitorDPI(_hWnd, _pPoint);
        }

        return LogicalToPhysicalPoint(_hWnd, _pPoint);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    HWND,
    WINAPI,
    WindowFromPhysicalPoint,
        _In_ POINT _oPoint
        )
    {
        if (auto const _pfnWindowFromPhysicalPoint = try_get_WindowFromPhysicalPoint())
        {
            return _pfnWindowFromPhysicalPoint(_oPoint);
        }

        return WindowFromPoint(_oPoint);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 6.0开始：WH_KEYBOARD_LL/WH_MOUSE_LL且dwThreadId为0时，hMod可以为NULL
    __DEFINE_THUNK(
    user32,
    16,
    HHOOK,
    WINAPI,
    SetWindowsHookExW,
        _In_ int _idHook,
        _In_ HOOKPROC _pfn,
        _In_opt_ HINSTANCE _hMod,
        _In_ DWORD _uThreadId
        )
    {
        if (auto const _pfnSetWindowsHookExW = try_get_SetWindowsHookExW())
        {
            if(_hMod == nullptr && _uThreadId == 0 && (_idHook == WH_KEYBOARD_LL || _idHook == WH_MOUSE_LL))
            {
                _hMod = reinterpret_cast<HINSTANCE>(&__ImageBase);
            }

            return _pfnSetWindowsHookExW(_idHook, _pfn, _hMod, _uThreadId);
        }

        SetLastError(ERROR_INVALID_FUNCTION);
        return nullptr;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 6.0开始：WH_KEYBOARD_LL/WH_MOUSE_LL且dwThreadId为0时，hMod可以为NULL
    __DEFINE_THUNK(
    user32,
    16,
    HHOOK,
    WINAPI,
    SetWindowsHookExA,
        _In_ int _idHook,
        _In_ HOOKPROC _pfn,
        _In_opt_ HINSTANCE _hMod,
        _In_ DWORD _uThreadId
        )
    {
        if (auto const _pfnSetWindowsHookExA = try_get_SetWindowsHookExA())
        {
            if(_hMod == nullptr && _uThreadId == 0 && (_idHook == WH_KEYBOARD_LL || _idHook == WH_MOUSE_LL))
            {
                _hMod = reinterpret_cast<HINSTANCE>(&__ImageBase);
            }

            return _pfnSetWindowsHookExA(_idHook, _pfn, _hMod, _uThreadId);
        }

        SetLastError(ERROR_INVALID_FUNCTION);
        return nullptr;
    }
#endif
}
