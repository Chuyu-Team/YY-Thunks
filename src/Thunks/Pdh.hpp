#if (YY_Thunks_Support_Version < NTDDI_WIN6)
#include <pdh.h>
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6) && !defined(__Comment_Lib_pdh)
#define __Comment_Lib_pdh
#pragma comment(lib, "Pdh.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // Windows Vista [desktop apps only]
    // Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    pdh,
    16,
    PDH_STATUS,
    WINAPI,
    PdhAddEnglishCounterW,
        _In_  PDH_HQUERY     hQuery,
        _In_  LPCWSTR        szFullCounterPath,
        _In_  DWORD_PTR      dwUserData,
        _Out_ PDH_HCOUNTER * phCounter
        )
    {
        if (const auto pPdhAddEnglishCounterW = try_get_PdhAddEnglishCounterW())
        {
            return pPdhAddEnglishCounterW(hQuery, szFullCounterPath, dwUserData, phCounter);
        }

        return PdhAddCounterW(hQuery, szFullCounterPath, dwUserData, phCounter);
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // Windows Vista [desktop apps only]
    // Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    pdh,
    16,
    PDH_STATUS,
    WINAPI,
    PdhAddEnglishCounterA,
        _In_  PDH_HQUERY     hQuery,
        _In_  LPCSTR         szFullCounterPath,
        _In_  DWORD_PTR      dwUserData,
        _Out_ PDH_HCOUNTER * phCounter
        )
    {
        if (const auto pPdhAddEnglishCounterA = try_get_PdhAddEnglishCounterA())
        {
            return pPdhAddEnglishCounterA(hQuery, szFullCounterPath, dwUserData, phCounter);
        }

        return PdhAddCounterA(hQuery, szFullCounterPath, dwUserData, phCounter);
    }

#endif
}
