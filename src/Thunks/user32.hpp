

namespace YY {
namespace Thunks {
#ifdef YY_Thunks_Defined
namespace internal {
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
} // namespace internal
#endif

#if (YY_Thunks_Support_Version < NTDDI_WS03SP1)
// Windows XP with SP2, Windows Server 2003 with SP1

EXTERN_C
BOOL WINAPI IsWow64Message(VOID)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pIsWow64Message = try_get_IsWow64Message())
    {
        return pIsWow64Message();
    }

    return FALSE;
}
#endif

__YY_Thunks_Expand_Function(user32, IsWow64Message, 0);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
// Windows Vista [desktop apps only]
// Windows Server 2008 [desktop apps only]

EXTERN_C
BOOL WINAPI SetProcessDPIAware(VOID)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pSetProcessDPIAware = try_get_SetProcessDPIAware())
    {
        return pSetProcessDPIAware();
    }

    /*
     * ������������ڣ�˵�������һ��XP���߸��Ͱ汾����������ƽ̨��
     * DPI��֪��ֱ�Ӵ򿪵ģ���������ֱ�ӷ���TRUE ���ɡ�
     */
    return TRUE;
}
#endif

__YY_Thunks_Expand_Function(user32, SetProcessDPIAware, 0);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS2)
// Windows 10, version 1703 [desktop apps only]
// Windows Server 2016 [desktop apps only]

EXTERN_C
BOOL WINAPI SetProcessDpiAwarenessContext(_In_ DPI_AWARENESS_CONTEXT value)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pSetProcessDpiAwarenessContext = try_get_SetProcessDpiAwarenessContext())
    {
        return pSetProcessDpiAwarenessContext(value);
    }

    PROCESS_DPI_AWARENESS DpiAwareness;

    if (DPI_AWARENESS_CONTEXT_UNAWARE == value || DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED == value)
    {
        DpiAwareness = PROCESS_DPI_UNAWARE;
    }
    else if (DPI_AWARENESS_CONTEXT_SYSTEM_AWARE == value)
    {
        DpiAwareness = PROCESS_SYSTEM_DPI_AWARE;
    }
    else if (
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE == value ||
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 == value)
    {
        DpiAwareness = PROCESS_PER_MONITOR_DPI_AWARE;
    }
    else
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    return SetProcessDpiAwareness(DpiAwareness);
}
#endif

__YY_Thunks_Expand_Function(user32, SetProcessDpiAwarenessContext, 4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1)
// Windows 10, version 1607 [desktop apps only]

EXTERN_C
UINT WINAPI GetDpiForSystem(VOID)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pGetDpiForSystem = try_get_GetDpiForSystem())
    {
        return pGetDpiForSystem();
    }

    return internal::GetDpiForSystemDownlevel();
}
#endif

__YY_Thunks_Expand_Function(user32, GetDpiForSystem, 0);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1)
// Windows 10, version 1607 [desktop apps only]

EXTERN_C
UINT WINAPI GetDpiForWindow(_In_ HWND hwnd)
#ifdef YY_Thunks_Defined
    ;
#else
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

__YY_Thunks_Expand_Function(user32, GetDpiForWindow, 4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1)
// Windows 10, version 1607 [desktop apps only]

EXTERN_C
int WINAPI GetSystemMetricsForDpi(_In_ int nIndex, _In_ UINT dpi)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pGetSystemMetricsForDpi = try_get_GetSystemMetricsForDpi())
    {
        return pGetSystemMetricsForDpi(nIndex, dpi);
    }

    auto nValue = GetSystemMetrics(nIndex);

    if (nValue != 0)
    {
        switch (nIndex)
        {
        case SM_CYSIZE:
        case SM_CXVSCROLL:
        case SM_CYSMICON:
        case SM_CYVSCROLL:
        case SM_CXPADDEDBORDER:
        case SM_CXSMICON:
        case SM_CYSMSIZE:
        case SM_CYICON:
        case SM_CYHSCROLL:
        case SM_CYMENUCHECK:
        case SM_CYCAPTION:
        case SM_CXHSCROLL:
        case SM_CXFRAME:
        case SM_CYMENUSIZE:
        case SM_CYFRAME:
        case SM_CYMENU:
        case SM_CXICON:
        case SM_CXICONSPACING:
        case SM_CYICONSPACING:
        case SM_CYVTHUMB:
        case SM_CXHTHUMB:
        case SM_CXCURSOR:
        case SM_CYCURSOR:
        case SM_CXMIN:
        case SM_CXMINTRACK:
        case SM_CYMIN:
        case SM_CYMINTRACK:
        case SM_CXSIZE:
        case SM_CYSMCAPTION:
        case SM_CXSMSIZE:
        case SM_CXMENUSIZE:
        case SM_CXMENUCHECK:
        {
            auto nDpiX = internal::GetDpiForSystemDownlevel();
            if (nDpiX != dpi)
            {
                nValue *= dpi;
                nValue += nDpiX / 2;
                nValue /= nDpiX;
            }

            break;
        }
        default:
            break;
        }
    }

    return nValue;
}
#endif

__YY_Thunks_Expand_Function(user32, GetSystemMetricsForDpi, 8);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1)
// Windows 10, version 1607 [desktop apps only]

EXTERN_C
BOOL WINAPI AdjustWindowRectExForDpi(
    _Inout_ LPRECT lpRect,
    _In_ DWORD dwStyle,
    _In_ BOOL bMenu,
    _In_ DWORD dwExStyle,
    _In_ UINT dpi)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pAdjustWindowRectExForDpi = try_get_AdjustWindowRectExForDpi())
    {
        return pAdjustWindowRectExForDpi(lpRect, dwStyle, bMenu, dwExStyle, dpi);
    }

    RECT FrameRect = {};

    if (!AdjustWindowRectEx(&FrameRect, dwStyle, bMenu, dwExStyle))
    {
        return FALSE;
    }

    auto nDpiX = internal::GetDpiForSystemDownlevel();

    if (nDpiX != dpi)
    {
        FrameRect.left = MulDiv(FrameRect.left, dpi, nDpiX);
        FrameRect.top = MulDiv(FrameRect.top, dpi, nDpiX);
        FrameRect.right = MulDiv(FrameRect.right, dpi, nDpiX);
        FrameRect.bottom = MulDiv(FrameRect.bottom, dpi, nDpiX);
    }

    lpRect->left += FrameRect.left;
    lpRect->top += FrameRect.top;
    lpRect->right += FrameRect.right;
    lpRect->bottom += FrameRect.bottom;

    return TRUE;
}
#endif

__YY_Thunks_Expand_Function(user32, AdjustWindowRectExForDpi, 20);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1)
// Windows 10, version 1607 [desktop apps only]

EXTERN_C
BOOL WINAPI SystemParametersInfoForDpi(
    _In_ UINT uiAction,
    _In_ UINT uiParam,
    _Pre_maybenull_ _Post_valid_ PVOID pvParam,
    _In_ UINT fWinIni,
    _In_ UINT dpi)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pSystemParametersInfoForDpi = try_get_SystemParametersInfoForDpi())
    {
        return pSystemParametersInfoForDpi(uiAction, uiParam, pvParam, fWinIni, dpi);
    }

    if (!SystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni))
        return FALSE;

    if (SPI_GETICONTITLELOGFONT == uiAction || SPI_GETICONMETRICS == uiAction ||
        SPI_GETNONCLIENTMETRICS == uiAction)
    {
        auto nDpiX = internal::GetDpiForSystemDownlevel();

        if (nDpiX != dpi)
        {
            if (SPI_GETICONTITLELOGFONT == uiAction)
            {
                if (auto pInfo = (LOGFONTW *)pvParam)
                {
                    pInfo->lfHeight = MulDiv(pInfo->lfHeight, dpi, nDpiX);
                }
            }
            else if (SPI_GETICONMETRICS == uiAction)
            {
                if (auto pInfo = (ICONMETRICSW *)pvParam)
                {
                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iHorzSpacing))
                        pInfo->iHorzSpacing = MulDiv(pInfo->iHorzSpacing, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iVertSpacing))
                        pInfo->iVertSpacing = MulDiv(pInfo->iVertSpacing, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfFont))
                        pInfo->lfFont.lfHeight = MulDiv(pInfo->lfFont.lfHeight, dpi, nDpiX);
                }
            }
            else if (SPI_GETNONCLIENTMETRICS == uiAction)
            {
                if (auto pInfo = (NONCLIENTMETRICSW *)pvParam)
                {
                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iBorderWidth))
                        pInfo->iBorderWidth = MulDiv(pInfo->iBorderWidth, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iScrollWidth))
                        pInfo->iScrollWidth = MulDiv(pInfo->iScrollWidth, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iScrollHeight))
                        pInfo->iScrollHeight = MulDiv(pInfo->iScrollHeight, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iCaptionWidth))
                        pInfo->iCaptionWidth = MulDiv(pInfo->iCaptionWidth, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iCaptionHeight))
                        pInfo->iCaptionHeight = MulDiv(pInfo->iCaptionHeight, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfCaptionFont))
                        pInfo->lfCaptionFont.lfHeight =
                            MulDiv(pInfo->lfCaptionFont.lfHeight, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iSmCaptionWidth))
                        pInfo->iSmCaptionWidth = MulDiv(pInfo->iSmCaptionWidth, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iSmCaptionHeight))
                        pInfo->iSmCaptionHeight = MulDiv(pInfo->iSmCaptionHeight, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfSmCaptionFont))
                        pInfo->lfSmCaptionFont.lfHeight =
                            MulDiv(pInfo->lfSmCaptionFont.lfHeight, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iMenuWidth))
                        pInfo->iMenuWidth = MulDiv(pInfo->iMenuWidth, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iMenuHeight))
                        pInfo->iMenuHeight = MulDiv(pInfo->iMenuHeight, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfMenuFont))
                        pInfo->lfMenuFont.lfHeight = MulDiv(pInfo->lfMenuFont.lfHeight, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfStatusFont))
                        pInfo->lfStatusFont.lfHeight =
                            MulDiv(pInfo->lfStatusFont.lfHeight, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfMessageFont))
                        pInfo->lfMessageFont.lfHeight =
                            MulDiv(pInfo->lfMessageFont.lfHeight, dpi, nDpiX);

                    if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iPaddedBorderWidth))
                        pInfo->iPaddedBorderWidth = MulDiv(pInfo->iPaddedBorderWidth, dpi, nDpiX);
                }
            }
        }
    }

    return TRUE;
}
#endif

__YY_Thunks_Expand_Function(user32, SystemParametersInfoForDpi, 20);

#endif

} // namespace Thunks

} // namespace YY