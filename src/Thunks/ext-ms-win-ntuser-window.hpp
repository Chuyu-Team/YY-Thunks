#if (YY_Thunks_Target < __WindowsNT6_2)
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
}
