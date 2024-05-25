#include <uxtheme.h>

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
#pragma comment(lib, "UxTheme.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // 最低受支持的客户端    Windows Vista [仅限桌面应用]
    // 最低受支持的服务器    Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    uxtheme,
    36,
    HRESULT,
    WINAPI,
    DrawThemeTextEx,
        _In_ HTHEME hTheme,
        _In_ HDC hdc,
        _In_ int iPartId,
        _In_ int iStateId,
        _In_reads_(cchText) LPCWSTR pszText,
        _In_ int cchText,
        _In_ DWORD dwTextFlags,
        _Inout_ LPRECT pRect,
        _In_opt_ const DTTOPTS *pOptions
        )
    {
        if (const auto _pfnDrawThemeTextEx = try_get_DrawThemeTextEx())
        {
            return _pfnDrawThemeTextEx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, pOptions);
        }

        return DrawThemeText(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, 0, pRect);
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // 最低受支持的客户端    Windows Vista [仅限桌面应用]
    // 最低受支持的服务器    Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    uxtheme,
    24,
    HRESULT,
    WINAPI,
    GetThemeTransitionDuration,
        _In_ HTHEME hTheme,
        _In_ int iPartId,
        _In_ int iStateIdFrom,
        _In_ int iStateIdTo,
        _In_ int iPropId,
        _Out_ DWORD *pdwDuration
        )
    {
        if (const auto _pfnGetThemeTransitionDuration = try_get_GetThemeTransitionDuration())
        {
            return _pfnGetThemeTransitionDuration(hTheme, iPartId, iStateIdFrom, iStateIdTo, iPropId, pdwDuration);
        }

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // 最低受支持的客户端    Windows Vista [仅限桌面应用]
    // 最低受支持的服务器    Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    uxtheme,
    16,
    HRESULT,
    WINAPI,
    SetWindowThemeAttribute,
        _In_ HWND hwnd,
        _In_ enum WINDOWTHEMEATTRIBUTETYPE eAttribute,
        _In_reads_bytes_(cbAttribute) PVOID pvAttribute,
        _In_ DWORD cbAttribute
        )
    {
        if (const auto _pfnSetWindowThemeAttribute = try_get_SetWindowThemeAttribute())
        {
            return _pfnSetWindowThemeAttribute(hwnd, eAttribute, pvAttribute, cbAttribute);
        }

        return E_NOTIMPL;
    }
#endif
}
