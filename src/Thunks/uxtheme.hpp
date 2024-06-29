#if (YY_Thunks_Target < __WindowsNT6)
#include <uxtheme.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_uxtheme)
#define __Comment_Lib_uxtheme
#pragma comment(lib, "UxTheme.lib")
#endif

#if defined(YY_Thunks_Implemented) && (YY_Thunks_Target < __WindowsNT6)
namespace YY::Thunks::Fallback
{
    static void* __fastcall try_get_DrawThemeTextEx(const ProcInfo& _ProcInfo) noexcept
    {
        auto _pProc = try_get_proc_address_from_dll(_ProcInfo);
        if (_pProc)
            return _pProc;

        static constexpr const ProcOffsetInfo kProcInfo[] =
        {
#if defined(_X86_)
            { 0x3B901769ul, 0x5ADC5C2Ful - 0x5ADC0000ul }, // 6.0.2600.0 (Windows XP RTM)
            { 0x3D7D2609ul, 0x5ADC5AEDul - 0x5ADC0000ul }, // 6.0.2800.1106 (Windows XP SP1)
            { 0x4121457Aul, 0x5ADC645Cul - 0x5ADC0000ul }, // 6.0.2900.2180 (Windows XP SP2)
            { 0x4802BDC0ul, 0x5ADC2FF8ul - 0x5ADC0000ul }, // 6.0.2900.5512 (Windows XP SP3)
            { 0x3E8024BEul, 0x71B8ACA0ul - 0x71B70000ul }, // 6.0.3790.0 (Windows 2003)
            { 0x42437794ul, 0x7DF64466ul - 0x7DF50000ul }, // 6.0.3790.1830 (Windows 2003 SP1)
            { 0x45D70ACBul, 0x71B91D41ul - 0x71B70000ul }, // 6.0.3790.3959 (Windows 2003 SP2)
#elif defined(_AMD64_)
            { 0x42438B57ul, 0x7FF77069A60ull - 0x7FF77060000ull }, // 6.0.3790.1830 (Windows 2003 SP1)
            { 0x45D6CCAEul, 0x000007FF77239A70ull - 0x7FF77230000ull }, // 6.0.3790.3959 (Windows 2003 SP2)
#endif
        };

        __WarningMessage__("try_get_DrawThemeTextEx 可能遗漏某些XP/2003补丁中的uxtheme.dll，如果你知道详细可以提交PR。");
        return try_get_proc_address_from_offset(try_get_module_uxtheme(), kProcInfo);
    }
}
#endif


namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

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


#if (YY_Thunks_Target < __WindowsNT6)

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


#if (YY_Thunks_Target < __WindowsNT6)

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
