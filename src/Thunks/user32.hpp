#if (YY_Thunks_Target < __WindowsNT10_15063)
#include <shellscalingapi.h>
#endif

#if (YY_Thunks_Target < __WindowsNT10_14393) && !defined(__Comment_Lib_user32)
#define __Comment_Lib_user32
#pragma comment(lib, "User32.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT10_15063) && (YY_Thunks_Target >= __WindowsNT6_3 || __YY_Thunks_libs) && !defined(__Comment_Lib_shcore)
#define __Comment_Lib_shcore
#pragma comment(lib, "Shcore.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT10_14393) && !defined(__Comment_Lib_gdi32)
#define __Comment_Lib_gdi32
#pragma comment(lib, "Gdi32.lib")
#endif

#if defined(YY_Thunks_Implemented)
namespace YY::Thunks::internal
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT10_14393)
        static UINT __fastcall GetDpiForSystemDownlevel()
        {
            static int nDPICache = 0;

            if (nDPICache)
            {
                return nDPICache;
            }

            int nDpiX = USER_DEFAULT_SCREEN_DPI;

            if (HDC hdc = GetDC(NULL))
            {
                nDpiX = GetDeviceCaps(hdc, LOGPIXELSX);
                ReleaseDC(NULL, hdc);
            }

            return nDPICache = nDpiX;
        }
#endif
    }
}
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows XP with SP2, Windows Server 2003 with SP1
    __DEFINE_THUNK(
    user32,
    0,
    BOOL,
    WINAPI,
    IsWow64Message,
        VOID
        )
    {
        if (auto const pIsWow64Message = try_get_IsWow64Message())
        {
            return pIsWow64Message();
        }

        return FALSE;
    }
#endif
        

#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    0,
    BOOL,
    WINAPI,
    IsProcessDPIAware,
        void)
    {
        if (const auto _pfnIsProcessDPIAware = try_get_IsProcessDPIAware())
        {
            return _pfnIsProcessDPIAware();
        }

        // XP 无法感知DPI，返回的Rect始终被系统缩放了。
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    0,
    BOOL,
    WINAPI,
    SetProcessDPIAware,
        VOID
        )
    {
        if (auto const pSetProcessDPIAware = try_get_SetProcessDPIAware())
        {
            return pSetProcessDPIAware();
        }

        // 假装成功，其实我们都知道，我们没有成功……
        // XP系统无法感知 DPI
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_15063)

    //Windows 10, version 1703 [desktop apps only]
    //Windows Server 2016 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    SetProcessDpiAwarenessContext,
        _In_ DPI_AWARENESS_CONTEXT value
        )
    {
        if (auto const pSetProcessDpiAwarenessContext = try_get_SetProcessDpiAwarenessContext())
        {
            return pSetProcessDpiAwarenessContext(value);
        }

        LSTATUS lStatus;

        do
        {
            PROCESS_DPI_AWARENESS DpiAwareness;

            if (DPI_AWARENESS_CONTEXT_UNAWARE == value
                || DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED == value)
            {
                DpiAwareness = PROCESS_DPI_UNAWARE;
            }
            else if (DPI_AWARENESS_CONTEXT_SYSTEM_AWARE == value)
            {
                DpiAwareness = PROCESS_SYSTEM_DPI_AWARE;
            }
            else if (DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE == value
                || DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 == value)
            {
                DpiAwareness = PROCESS_PER_MONITOR_DPI_AWARE;
            }
            else
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            auto hr = SetProcessDpiAwareness(DpiAwareness);

            if (SUCCEEDED(hr))
            {
                return TRUE;
            }

            //将 HRESULT 错误代码转换到 LSTATUS
            if (hr & 0xFFFF0000)
            {
                if (HRESULT_FACILITY(hr) == FACILITY_WIN32)
                {
                    lStatus = HRESULT_CODE(hr);
                }
                else
                {
                    lStatus = ERROR_FUNCTION_FAILED;
                }
            }
            else
            {
                lStatus = hr;
            }

        } while (false);

            
        SetLastError(lStatus);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    //Windows 10, version 1607 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    0,
    UINT,
    WINAPI,
    GetDpiForSystem,
        VOID
        )
    {
        if (auto const pGetDpiForSystem = try_get_GetDpiForSystem())
        {
            return pGetDpiForSystem();
        }

        return internal::GetDpiForSystemDownlevel();
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    //Windows 10, version 1607 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    4,
    UINT,
    WINAPI,
    GetDpiForWindow,
        _In_ HWND hwnd
        )
    {
        if (auto const pGetDpiForWindow = try_get_GetDpiForWindow())
        {
            return pGetDpiForWindow(hwnd);
        }

        if (HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY))
        {
            UINT nDpiX, nDpiY;
            if (SUCCEEDED(GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &nDpiX, &nDpiY)))
            {
                return nDpiX;
            }
        }

        return internal::GetDpiForSystemDownlevel();
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    //Windows 10, version 1607 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    8,
    int,
    WINAPI,
    GetSystemMetricsForDpi,
        _In_ int _nIndex,
        _In_ UINT _uDpi
        )
    {
        if (auto const _pfnGetSystemMetricsForDpi = try_get_GetSystemMetricsForDpi())
        {
            return _pfnGetSystemMetricsForDpi(_nIndex, _uDpi);
        }

        if (_uDpi == 0ul)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }

        auto _nValue = GetSystemMetrics(_nIndex);
        if (_nValue != 0)
        {
            switch (_nIndex)
            {
            case SM_CXVSCROLL:
            case SM_CYHSCROLL:
            case SM_CYCAPTION: // 4

            case SM_CYVTHUMB: // 9
            case SM_CXHTHUMB:
            case SM_CXICON:
            case SM_CYICON:
            case SM_CXCURSOR:
            case SM_CYCURSOR:
            case SM_CYMENU: // 15

            case SM_CYVSCROLL: // 20
            case SM_CXHSCROLL:

            case SM_CXMIN: // 28
            case SM_CYMIN:
            case SM_CXSIZE:
            case SM_CYSIZE:
            case SM_CXMINTRACK:
            case SM_CYMINTRACK: //35

            case SM_CXICONSPACING: //38
            case SM_CYICONSPACING: // 39

            case SM_CXSMICON: // 49
            case SM_CYSMICON:
            case SM_CYSMCAPTION:
            case SM_CXSMSIZE:
            case SM_CYSMSIZE:
            case SM_CXMENUSIZE:
            case SM_CYMENUSIZE: // 55

            case SM_CXMENUCHECK: // 71
            case SM_CYMENUCHECK: // 72

            case SM_CXPADDEDBORDER: // 92
            {
                const auto _uBaseDpi = internal::GetDpiForSystemDownlevel();
                if (_uBaseDpi != _uDpi)
                {
                    int _nDelta = 0;
                    switch (_nIndex)
                    {
                    case SM_CYCAPTION: // 4
                    case SM_CYMENU: // 15
                    case SM_CYSMCAPTION: // 51
                        _nDelta = GetSystemMetrics(SM_CYBORDER);
                        break;
                    default:
                        __WarningMessage__("SM_CXMIN、SM_CYMIN、SM_CXMINTRACK、SM_CYMINTRACK、SM_CXMENUCHECK、SM_CYMENUCHECK 存在一定偏差。");
                        break;
                    }
                    _nValue = MulDiv(_nValue - _nDelta, _uDpi, _uBaseDpi) + _nDelta;
                }

                break;
            }
            case SM_CXFRAME:
            {
                const auto _nBorreder = GetSystemMetrics(SM_CXBORDER);
                _nValue -= 2 * _nBorreder;
                _nValue = MulDiv(_nValue, _uDpi, USER_DEFAULT_SCREEN_DPI) + _nBorreder;
                break;
            }
            case SM_CYFRAME:
            {
                const auto _nBorreder = GetSystemMetrics(SM_CYBORDER);
                _nValue -= 2 * _nBorreder;
                _nValue = MulDiv(_nValue, _uDpi, USER_DEFAULT_SCREEN_DPI) + _nBorreder;
                break;
            }
            }
        }

        return _nValue;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    //Windows 10, version 1607 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    20,
    BOOL,
    WINAPI,
    AdjustWindowRectExForDpi,
        _Inout_ LPRECT _pRect,
        _In_ DWORD _fStyle,
        _In_ BOOL _bMenu,
        _In_ DWORD _fExStyle,
        _In_ UINT _uDpi
        )
    {
        if (auto const _pfnAdjustWindowRectExForDpi = try_get_AdjustWindowRectExForDpi())
        {
            return _pfnAdjustWindowRectExForDpi(_pRect, _fStyle, _bMenu, _fExStyle, _uDpi);
        }

        if (_uDpi == 0ul)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        RECT _DeltaRect = {};
        if (!AdjustWindowRectEx(&_DeltaRect, _fStyle, _bMenu, _fExStyle))
        {
            return FALSE;
        }

        const auto _uBaseDpi = internal::GetDpiForSystemDownlevel();
        if (_uBaseDpi != _uDpi)
        {
            RECT _DlgFrameRect = {};
            if (_fStyle & (WS_THICKFRAME | WS_DLGFRAME))
            {
                AdjustWindowRectEx(&_DlgFrameRect, WS_DLGFRAME, FALSE, 0);
                if (_bMenu || (_fStyle & WS_CAPTION) == WS_CAPTION)
                {
                    const auto _nSystemBorderWidth = GetSystemMetrics(SM_CXBORDER);
                    _DlgFrameRect.top -= _nSystemBorderWidth;
                }
            }
            else if (_fStyle & WS_BORDER)
            {
                AdjustWindowRectEx(&_DlgFrameRect, WS_BORDER, FALSE, 0);
                if (_bMenu || (_fStyle & WS_CAPTION) == WS_CAPTION)
                {
                    // 特殊优化，不在调用 GetSystemMetrics(SM_CXBORDER)
                    // 毕竟理论上 _DlgFrameRect.top == -GetSystemMetrics(SM_CXBORDER)
                    _DlgFrameRect.top += _DlgFrameRect.top;
                }
            }

            _DeltaRect.left -= _DlgFrameRect.left;
            _DeltaRect.top -= _DlgFrameRect.top;
            _DeltaRect.right -= _DlgFrameRect.right;
            _DeltaRect.bottom -= _DlgFrameRect.bottom;

            _DeltaRect.left = MulDiv(_DeltaRect.left, _uDpi, _uBaseDpi) + _DlgFrameRect.left;
            _DeltaRect.top = MulDiv(_DeltaRect.top, _uDpi, _uBaseDpi) + _DlgFrameRect.top;
            _DeltaRect.right = MulDiv(_DeltaRect.right, _uDpi, _uBaseDpi) + _DlgFrameRect.right;
            _DeltaRect.bottom = MulDiv(_DeltaRect.bottom, _uDpi, _uBaseDpi) + _DlgFrameRect.bottom;
        }

        _pRect->left += _DeltaRect.left;
        _pRect->top += _DeltaRect.top;
        _pRect->right += _DeltaRect.right;
        _pRect->bottom += _DeltaRect.bottom;
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    //Windows 10, version 1607 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    20,
    BOOL,
    WINAPI,
    SystemParametersInfoForDpi,
        _In_ UINT _uAction,
        _In_ UINT _uParam,
        _Pre_maybenull_ _Post_valid_ PVOID _pParam,
        _In_ UINT _fWinIni,
        _In_ UINT _uDpi
        )
    {
        if (auto const _pfnSystemParametersInfoForDpi = try_get_SystemParametersInfoForDpi())
            return _pfnSystemParametersInfoForDpi(_uAction, _uParam, _pParam, _fWinIni, _uDpi);

        // SystemParametersInfoW 函数始终拿到的是 DPI = _uBaseDpi 的情况
        // 其结果与当前系统DPI值无关。
        // 我们可以通过缩放来模拟 SystemParametersInfoForDpi
        const auto _uBaseDpi = internal::GetDpiForSystemDownlevel();

        switch (_uAction)
        {
        case SPI_GETICONTITLELOGFONT:
        {
            if (!SystemParametersInfoW(_uAction, _uParam, _pParam, _fWinIni))
                return FALSE;

            if(_pParam == nullptr || _uDpi == 0 || _uDpi == _uBaseDpi)
                return TRUE;

            auto _pInfo = (LOGFONTW*)_pParam;
            _pInfo->lfHeight = MulDiv(_pInfo->lfHeight, _uDpi, _uBaseDpi);
            return TRUE;
        }
        case SPI_GETICONMETRICS:
        {
            if (!SystemParametersInfoW(_uAction, _uParam, _pParam, _fWinIni))
                return FALSE;

            if (_pParam == nullptr || _uDpi == 0 || _uDpi == _uBaseDpi)
                return TRUE;

            auto _pInfo = (ICONMETRICSW*)_pParam;
            _pInfo->iHorzSpacing = MulDiv(_pInfo->iHorzSpacing, _uDpi, _uBaseDpi);
            _pInfo->iVertSpacing = MulDiv(_pInfo->iVertSpacing, _uDpi, _uBaseDpi);
            _pInfo->lfFont.lfHeight = MulDiv(_pInfo->lfFont.lfHeight, _uDpi, _uBaseDpi);
            return TRUE;
        }
        case SPI_GETNONCLIENTMETRICS:
        {
            // 微软强制要求Size等于 sizeof(NONCLIENTMETRICSW)
            auto _pInfo = (NONCLIENTMETRICSW*)_pParam;
            if (_pInfo == nullptr || _pInfo->cbSize != sizeof(NONCLIENTMETRICSW))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            if (!SystemParametersInfoW(_uAction, _uParam, _pParam, _fWinIni))
                return FALSE;

            if (_pParam == nullptr || _uDpi == 0)
                return TRUE;

            // GetSystemMetrics(SM_CYBORDER) 的大小始终跟 Dpi无关
            const auto _nSystemBorderWidth = MulDiv(GetSystemMetrics(SM_CYBORDER), _uDpi, USER_DEFAULT_SCREEN_DPI);
            if (_uDpi == _uBaseDpi)
            {
                // 如果DPI恰好 == _uBaseDpi，这时可以优化，避免无意义的MulDiv调用。
                // 注意，主动传入DPI时，iBorderWidth默认为 GetSystemMetrics(SM_CYBORDER)。
                const auto _nBorderWidth = _pInfo->iBorderWidth + _pInfo->iPaddedBorderWidth;
                if (_nBorderWidth > 0)
                {
                    _pInfo->iPaddedBorderWidth = _nBorderWidth - _nSystemBorderWidth;
                    _pInfo->iBorderWidth = _nSystemBorderWidth;
                }
                return TRUE;
            }

            // 跳过 iBorderWidth，通过后续的iPaddedBorderWidth调整。
            // _pInfo->iBorderWidth = MulDiv(_pInfo->iBorderWidth, _uDpi, kBaseDpi);
            _pInfo->iScrollWidth = MulDiv(_pInfo->iScrollWidth, _uDpi, _uBaseDpi);
            _pInfo->iScrollHeight = MulDiv(_pInfo->iScrollHeight, _uDpi, _uBaseDpi);
            _pInfo->iCaptionWidth = MulDiv(_pInfo->iCaptionWidth, _uDpi, _uBaseDpi);
            _pInfo->iCaptionHeight = MulDiv(_pInfo->iCaptionHeight, _uDpi, _uBaseDpi);
            _pInfo->lfCaptionFont.lfHeight = MulDiv(_pInfo->lfCaptionFont.lfHeight, _uDpi, _uBaseDpi);
            _pInfo->iSmCaptionWidth = MulDiv(_pInfo->iSmCaptionWidth, _uDpi, _uBaseDpi);
            _pInfo->iSmCaptionHeight = MulDiv(_pInfo->iSmCaptionHeight, _uDpi, _uBaseDpi);
            _pInfo->lfSmCaptionFont.lfHeight = MulDiv(_pInfo->lfSmCaptionFont.lfHeight, _uDpi, _uBaseDpi);
            _pInfo->iMenuWidth = MulDiv(_pInfo->iMenuWidth, _uDpi, _uBaseDpi);
            _pInfo->iMenuHeight = MulDiv(_pInfo->iMenuHeight, _uDpi, _uBaseDpi);
            _pInfo->lfMenuFont.lfHeight = MulDiv(_pInfo->lfMenuFont.lfHeight, _uDpi, _uBaseDpi);
            _pInfo->lfStatusFont.lfHeight = MulDiv(_pInfo->lfStatusFont.lfHeight, _uDpi, _uBaseDpi);
            _pInfo->lfMessageFont.lfHeight = MulDiv(_pInfo->lfMessageFont.lfHeight, _uDpi, _uBaseDpi);

            const auto _nBorderWidth = _pInfo->iBorderWidth + _pInfo->iPaddedBorderWidth;
            if (_nBorderWidth > 0)
            {
                _pInfo->iPaddedBorderWidth = MulDiv(_nBorderWidth, _uDpi, _uBaseDpi) - _nSystemBorderWidth;
                _pInfo->iBorderWidth = _nSystemBorderWidth;
            }
            return TRUE;
        }
        }

        // 微软原版这时不会返回错误代码，但是总感觉是微软的Bug
        // 我们这里就随便返回一个错误代码吧。
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    GetWindowDisplayAffinity,
        _In_ HWND _hWnd,
        _Out_ DWORD* _pdwAffinity)
    {
        if (auto const _pfnGetWindowDisplayAffinity = try_get_GetWindowDisplayAffinity())
        {
            return _pfnGetWindowDisplayAffinity(_hWnd, _pdwAffinity);
        }

        if (!_pdwAffinity)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        // 系统不支持，假装自己不需要任何保护
        *_pdwAffinity = WDA_NONE;
        return TRUE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    SetWindowDisplayAffinity,
        _In_ HWND _hWnd,
        _In_ DWORD _dwAffinity)
    {
        if (auto const _pfnSetWindowDisplayAffinity = try_get_SetWindowDisplayAffinity())
        {
            return _pfnSetWindowDisplayAffinity(_hWnd, _dwAffinity);
        }

        // 系统不支持，假装自己成功……
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    RegisterTouchWindow,
        _In_ HWND _hWnd,
        _In_ ULONG _ulFlags)
    {
        if (auto const _pfnRegisterTouchWindow = try_get_RegisterTouchWindow())
        {
            return _pfnRegisterTouchWindow(_hWnd, _ulFlags);
        }

        return TRUE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    UnregisterTouchWindow,
        _In_ HWND _hWnd)
    {
        if (auto const _pfnUnregisterTouchWindow = try_get_UnregisterTouchWindow())
        {
            return _pfnUnregisterTouchWindow(_hWnd);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    IsTouchWindow,
        _In_ HWND _hWnd,
        _Out_opt_ PULONG _puFlags)
    {
        if (auto const _pfnIsTouchWindow = try_get_IsTouchWindow())
        {
            return _pfnIsTouchWindow(_hWnd, _puFlags);
        }

        if (_puFlags)
            *_puFlags = 0;

        return TRUE;
    }
#endif
        

#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    16,
    BOOL,
    WINAPI,
    GetTouchInputInfo,
        _In_ HTOUCHINPUT _hTouchInput,
        _In_ UINT _uInputs,
        _Out_writes_(_uInputs) PTOUCHINPUT _pInputs,
        _In_ int _cbSize)
    {
        if (auto const _pfnGetTouchInputInfo = try_get_GetTouchInputInfo())
        {
            return _pfnGetTouchInputInfo(_hTouchInput, _uInputs, _pInputs, _cbSize);
        }

        // 老版本系统没有触摸消息，_hTouchInput 必然无效
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    CloseTouchInputHandle,
        _In_ HTOUCHINPUT _hTouchInput)
    {
        if (auto const _pfnCloseTouchInputHandle = try_get_CloseTouchInputHandle())
        {
            return _pfnCloseTouchInputHandle(_hTouchInput);
        }

        // 老版本系统没有触摸消息，_hTouchInput 必然无效
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
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
    ChangeWindowMessageFilter,
        _In_ UINT _uMessage,
        _In_ DWORD _fFlag
        )
    {
        if (const auto _pfnChangeWindowMessageFilter = try_get_ChangeWindowMessageFilter())
        {
            return _pfnChangeWindowMessageFilter(_uMessage, _fFlag);
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
    UpdateLayeredWindowIndirect,
        _In_ HWND hWnd,
        _In_ const UPDATELAYEREDWINDOWINFO* pULWInfo
        )
    {
        if (const auto _pfnUpdateLayeredWindowIndirect = try_get_UpdateLayeredWindowIndirect())
        {
            return _pfnUpdateLayeredWindowIndirect(hWnd, pULWInfo);
        }

        return UpdateLayeredWindow(
            hWnd,
            pULWInfo->hdcDst,
            const_cast<POINT*>(pULWInfo->pptDst),
            const_cast<SIZE*>(pULWInfo->psize),
            pULWInfo->hdcSrc,
            const_cast<POINT*>(pULWInfo->pptSrc),
            pULWInfo->crKey,
            const_cast<BLENDFUNCTION*>(pULWInfo->pblend), pULWInfo->dwFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows 2000 Professional [仅限桌面应用]
    // 最低受支持的服务器	Windows 2000 Server[仅限桌面应用]
    // 虽然 2000就支持，但是 SPI_GETNONCLIENTMETRICS 需要特殊处理。
    __DEFINE_THUNK(
    user32,
    16,
    BOOL,
    WINAPI,
    SystemParametersInfoW,
        _In_ UINT _uAction,
        _In_ UINT _uParam,
        _Pre_maybenull_ _Post_valid_ PVOID _pParam,
        _In_ UINT _fWinIni
        )
    {
        const auto _pfnSystemParametersInfoW = try_get_SystemParametersInfoW();
        if (!_pfnSystemParametersInfoW)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return FALSE;
        }

        if (_pParam && internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            if (SPI_GETNONCLIENTMETRICS == _uAction)
            {
                // https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/ns-winuser-nonclientmetricsw#remarks
                // Windows XP等系统必须减去iPaddedBorderWidth大小。
                auto _pInfo = (NONCLIENTMETRICSW*)_pParam;
                if (_pInfo->cbSize == sizeof(NONCLIENTMETRICSW))
                {
                    _pInfo->cbSize = RTL_SIZEOF_THROUGH_FIELD(NONCLIENTMETRICSW, lfMessageFont);
                    // XP不支持iPaddedBorderWidth，暂时设置为 0，如果有更佳值可以PR。
                    _pInfo->iPaddedBorderWidth = 0;
                    auto _bRet = _pfnSystemParametersInfoW(_uAction, RTL_SIZEOF_THROUGH_FIELD(NONCLIENTMETRICSW, lfMessageFont), _pParam, _fWinIni);
                    // 重新恢复其大小
                    _pInfo->cbSize = sizeof(NONCLIENTMETRICSW);
                    return _bRet;
                }
            }
        }
        
        return _pfnSystemParametersInfoW(_uAction, _uParam, _pParam, _fWinIni);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows 2000 Professional [仅限桌面应用]
    // 最低受支持的服务器	Windows 2000 Server[仅限桌面应用]
    // 虽然 2000就支持，但是 SPI_GETNONCLIENTMETRICS 需要特殊处理。
    __DEFINE_THUNK(
    user32,
    16,
    BOOL,
    WINAPI,
    SystemParametersInfoA,
        _In_ UINT _uAction,
        _In_ UINT _uParam,
        _Pre_maybenull_ _Post_valid_ PVOID _pParam,
        _In_ UINT _fWinIni
        )
    {
        const auto _pfnSystemParametersInfoA = try_get_SystemParametersInfoA();
        if (!_pfnSystemParametersInfoA)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return FALSE;
        }

#if (YY_Thunks_Target < __WindowsNT6)
        if (_pParam && internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            if (SPI_GETNONCLIENTMETRICS == _uAction)
            {
                // https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/ns-winuser-nonclientmetricsw#remarks
                // Windows XP等系统必须减去iPaddedBorderWidth大小。
                auto _pInfo = (NONCLIENTMETRICSA*)_pParam;
                if (_pInfo->cbSize == sizeof(NONCLIENTMETRICSA))
                {
                    _pInfo->cbSize = RTL_SIZEOF_THROUGH_FIELD(NONCLIENTMETRICSA, lfMessageFont);
                    _pInfo->iPaddedBorderWidth = 0;
                    auto _bRet = _pfnSystemParametersInfoA(_uAction, RTL_SIZEOF_THROUGH_FIELD(NONCLIENTMETRICSA, lfMessageFont), _pParam, _fWinIni);
                    // 重新恢复其大小
                    _pInfo->cbSize = sizeof(NONCLIENTMETRICSA);
                    return _bRet;
                }
            }
        }
#endif
        return _pfnSystemParametersInfoA(_uAction, _uParam, _pParam, _fWinIni);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    DPI_AWARENESS,
    WINAPI,
    GetAwarenessFromDpiAwarenessContext,
        _In_ DPI_AWARENESS_CONTEXT _hValue
        )
    {
        if (auto const _pfnGetAwarenessFromDpiAwarenessContext = try_get_GetAwarenessFromDpiAwarenessContext())
        {
            return _pfnGetAwarenessFromDpiAwarenessContext(_hValue);
        }

        if (_hValue == DPI_AWARENESS_CONTEXT_UNAWARE || _hValue == DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED)
        {
            return DPI_AWARENESS_UNAWARE;
        }
        else if (_hValue == DPI_AWARENESS_CONTEXT_SYSTEM_AWARE)
        {
            return DPI_AWARENESS_SYSTEM_AWARE;
        }
        else if (_hValue == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE || _hValue == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)
        {
            return DPI_AWARENESS_PER_MONITOR_AWARE;
        }
        else
        {
            return DPI_AWARENESS_INVALID;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    AreDpiAwarenessContextsEqual,
        _In_ DPI_AWARENESS_CONTEXT _hDpiContextA,
        _In_ DPI_AWARENESS_CONTEXT _hDpiContextB
        )
    {
        if (auto const _pfnAreDpiAwarenessContextsEqual = try_get_AreDpiAwarenessContextsEqual())
        {
            return _pfnAreDpiAwarenessContextsEqual(_hDpiContextA, _hDpiContextB);
        }
        
        return _hDpiContextA == _hDpiContextB;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    EnableNonClientDpiScaling,
        _In_ HWND _hWnd
        )
    {
        if (auto const _pfnEnableNonClientDpiScaling = try_get_EnableNonClientDpiScaling())
        {
            return _pfnEnableNonClientDpiScaling(_hWnd);
        }
        
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    0,
    DPI_AWARENESS_CONTEXT,
    WINAPI,
    GetThreadDpiAwarenessContext
        )
    {
        if (auto const _pfnGetThreadDpiAwarenessContext = try_get_GetThreadDpiAwarenessContext())
        {
            return _pfnGetThreadDpiAwarenessContext();
        }
        
        PROCESS_DPI_AWARENESS _eValue = PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE;
        auto _hr = GetProcessDpiAwareness(NULL, &_eValue);
        if (SUCCEEDED(_hr))
        {
            switch (_eValue)
            {
            case PROCESS_DPI_UNAWARE:
                return DPI_AWARENESS_CONTEXT_UNAWARE;
            case PROCESS_SYSTEM_DPI_AWARE:
                return DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
            case PROCESS_PER_MONITOR_DPI_AWARE:
                return DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;
            default:
                break;
            }
        }

        return DPI_AWARENESS_CONTEXT_UNAWARE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    DPI_AWARENESS_CONTEXT,
    WINAPI,
    GetWindowDpiAwarenessContext,
        _In_ HWND _hWnd
        )
    {
        if (auto const _pfnGetWindowDpiAwarenessContext = try_get_GetWindowDpiAwarenessContext())
        {
            return _pfnGetWindowDpiAwarenessContext(_hWnd);
        }
        
        DWORD _uProcessId = 0;
        GetWindowThreadProcessId(_hWnd, &_uProcessId);
        if (_uProcessId == 0)
        {
            return NULL;
        }

        HANDLE _hProcess = NULL;
        if (_uProcessId != GetCurrentProcessId() && internal::GetSystemVersion() >= internal::MakeVersion(6, 3))
        {
            _hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, _uProcessId);
        }

        PROCESS_DPI_AWARENESS _eValue = PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE;
        auto _hr = GetProcessDpiAwareness(_hProcess, &_eValue);
        if (_hProcess)
        {
            CloseHandle(_hProcess);
        }

        if (SUCCEEDED(_hr))
        {
            switch (_eValue)
            {
            case PROCESS_DPI_UNAWARE:
                return DPI_AWARENESS_CONTEXT_UNAWARE;
            case PROCESS_SYSTEM_DPI_AWARE:
                return DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
            case PROCESS_PER_MONITOR_DPI_AWARE:
                return DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;
            default:
                break;
            }
        }

        return DPI_AWARENESS_CONTEXT_UNAWARE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    SetDisplayAutoRotationPreferences,
        _In_ ORIENTATION_PREFERENCE _eOrientation
        )
    {
        if (auto const _pfnSetDisplayAutoRotationPreferences = try_get_SetDisplayAutoRotationPreferences())
        {
            return _pfnSetDisplayAutoRotationPreferences(_eOrientation);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    GetDisplayAutoRotationPreferences,
        _Out_ ORIENTATION_PREFERENCE* _peOrientation
        )
    {
        if (auto const _pfnGetDisplayAutoRotationPreferences = try_get_GetDisplayAutoRotationPreferences())
        {
            return _pfnGetDisplayAutoRotationPreferences(_peOrientation);
        }

        *_peOrientation = ORIENTATION_PREFERENCE::ORIENTATION_PREFERENCE_NONE;
        return TRUE;
    }
#endif
} //namespace YY::Thunks
