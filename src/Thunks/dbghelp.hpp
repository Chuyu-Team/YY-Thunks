#if (YY_Thunks_Support_Version < NTDDI_WIN6)
#include <dbghelp.h>
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6) && !defined(__Comment_Lib_dbghelp)
#define __Comment_Lib_dbghelp
#pragma comment(lib, "Dbghelp.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // XP SP3自带的没有W版
    __DEFINE_THUNK(
    dbghelp,
    8,
    BOOL,
    WINAPI,
    SymSetSearchPathW,
        _In_ HANDLE hProcess,
        _In_opt_ PCWSTR SearchPath
        )
    {
        if (const auto _pfnSymSetSearchPathW = try_get_SymSetSearchPathW())
        {
            return _pfnSymSetSearchPathW(hProcess, SearchPath);
        }

        PCSTR _szSearchPathANSI = nullptr;

        internal::StringBuffer<char> _szBuffer;
        if (SearchPath)
        {
            auto _lStatus = internal::Convert(SearchPath, -1, &_szBuffer);
            if (_lStatus)
            {
                SetLastError(_lStatus);
                return FALSE;
            }

            _szSearchPathANSI = _szBuffer.GetC_String();
        }
        return ::SymSetSearchPath(hProcess, _szSearchPathANSI);
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // XP SP3自带的没有W版
    __DEFINE_THUNK(
    dbghelp,
    12,
    BOOL,
    WINAPI,
    SymGetSearchPathW,
        _In_ HANDLE hProcess,
        _Out_writes_(SearchPathLength) PWSTR SearchPath,
        _In_ DWORD SearchPathLength
        )
    {
        if (const auto _pfnSymGetSearchPathW = try_get_SymGetSearchPathW())
        {
            return _pfnSymGetSearchPathW(hProcess, SearchPath, SearchPathLength);
        }

        if (SearchPath == nullptr || SearchPathLength == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        char _szANSISearchPathBuffer[2048];
        if (!SymGetSearchPath(hProcess, _szANSISearchPathBuffer, _countof(_szANSISearchPathBuffer)))
        {
            return FALSE;
        }

        if (*_szANSISearchPathBuffer == L'\0')
        {
            SearchPath[0] = L'\0';
            return TRUE;
        }

        return MultiByteToWideChar(CP_ACP, 0, _szANSISearchPathBuffer, -1, SearchPath, SearchPathLength) == 0;
    }
#endif
}
