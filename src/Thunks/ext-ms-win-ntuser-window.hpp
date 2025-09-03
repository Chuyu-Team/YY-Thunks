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
}
