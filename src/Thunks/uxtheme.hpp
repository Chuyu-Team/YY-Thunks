#if (YY_Thunks_Support_Version < NTDDI_WIN6)
#include <uxtheme.h>
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6) && !defined(__Comment_Lib_uxtheme)
#define __Comment_Lib_uxtheme
#pragma comment(lib, "UxTheme.lib")
#endif

#if defined(YY_Thunks_Implemented) && defined(_X86_) && (YY_Thunks_Support_Version < NTDDI_WIN6)
namespace YY::Thunks::Fallback
{
    static void* __fastcall try_get_DrawThemeTextEx() noexcept
    {
        auto _pUxThemeBase = (const char*)try_get_module_uxtheme();
        if (!_pUxThemeBase)
            return nullptr;

        MEMORY_BASIC_INFORMATION _BaseInfo;

        // Windows XP SP3 其实已经实现了DrawThemeTextEx，只是没有导出。
        static const BYTE kAttributeCode_6_0_2900_5512[] =
        {
            0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x0C,
            0x83, 0x4D, 0xF8, 0xFF, 0x83, 0x65, 0xF4, 0x00,
            0x83, 0x7D, 0x0C, 0x00, 0x56, 
        };
        constexpr auto kAttributeCode_6_0_2900_5512_Offset = 0x5ADC2FF8 - 0x5ADC0000;
        auto _pTarget = _pUxThemeBase + kAttributeCode_6_0_2900_5512_Offset;
        if (VirtualQuery(_pTarget, &_BaseInfo, sizeof(_BaseInfo)) && _BaseInfo.AllocationBase == _pUxThemeBase)
        {
            if (memcmp(_pTarget, kAttributeCode_6_0_2900_5512, sizeof(kAttributeCode_6_0_2900_5512)) == 0)
            {
                return (void*)_pTarget;
            }
        }

        return nullptr;
    }
}
#endif


namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
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

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
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

        if (pdwDuration == nullptr || iStateIdFrom <= 0 || iStateIdTo <= 0)
        {
            return E_INVALIDARG;
        }

        *pdwDuration = 0;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
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
