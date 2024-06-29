

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    QueryFullProcessImageNameW,
        _In_ HANDLE hProcess,
        _In_ DWORD dwFlags,
        _Out_writes_to_(*lpdwSize, *lpdwSize) LPWSTR lpExeName,
        _Inout_ PDWORD lpdwSize
        )
    {
        if (auto pQueryFullProcessImageNameW = try_get_QueryFullProcessImageNameW())
        {
            return pQueryFullProcessImageNameW(hProcess, dwFlags, lpExeName, lpdwSize);
        }

        auto dwSize = *lpdwSize;

        if (dwFlags & PROCESS_NAME_NATIVE)
        {
            dwSize = GetProcessImageFileNameW(hProcess, lpExeName, dwSize);
        }
        else
        {
            dwSize = GetModuleFileNameExW(hProcess, nullptr, lpExeName, dwSize);
        }

        if (dwSize)
        {
            *lpdwSize = dwSize;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    QueryFullProcessImageNameA,
        _In_ HANDLE hProcess,
        _In_ DWORD dwFlags,
        _Out_writes_to_(*lpdwSize, *lpdwSize) LPSTR lpExeName,
        _Inout_ PDWORD lpdwSize
        )
    {
        if (auto pQueryFullProcessImageNameA = try_get_QueryFullProcessImageNameA())
        {
            return pQueryFullProcessImageNameA(hProcess, dwFlags, lpExeName, lpdwSize);
        }

        auto dwSize = *lpdwSize;

        if (dwFlags & PROCESS_NAME_NATIVE)
        {
            dwSize = GetProcessImageFileNameA(hProcess, lpExeName, dwSize);
        }
        else
        {
            dwSize = GetModuleFileNameExA(hProcess, nullptr, lpExeName, dwSize);
        }

        if (dwSize)
        {
            *lpdwSize = dwSize;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
#endif
} //namespace YY::Thunks
