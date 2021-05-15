

namespace YY {
namespace Thunks {

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
// Windows Vista [desktop apps only]
// Windows Server 2008 [desktop apps only]

EXTERN_C
BOOL APIENTRY GetFileVersionInfoExW(
    _In_ DWORD dwFlags,
    _In_ LPCWSTR lpwstrFilename,
    _Reserved_ DWORD dwHandle,
    _In_ DWORD dwLen,
    _Out_writes_bytes_(dwLen) LPVOID lpData)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto pGetFileVersionInfoExW = try_get_GetFileVersionInfoExW())
    {
        return pGetFileVersionInfoExW(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
    }

    return GetFileVersionInfoW(lpwstrFilename, dwHandle, dwLen, lpData);
}
#endif

__YY_Thunks_Expand_Function(version, GetFileVersionInfoExW, 20);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
// Windows Vista [desktop apps only]
// Windows Server 2008 [desktop apps only]

EXTERN_C
BOOL APIENTRY GetFileVersionInfoExA(
    _In_ DWORD dwFlags,
    _In_ LPCSTR lpwstrFilename,
    _Reserved_ DWORD dwHandle,
    _In_ DWORD dwLen,
    _Out_writes_bytes_(dwLen) LPVOID lpData)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto pGetFileVersionInfoExA = try_get_GetFileVersionInfoExA())
    {
        return pGetFileVersionInfoExA(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
    }

    return GetFileVersionInfoA(lpwstrFilename, dwHandle, dwLen, lpData);
}
#endif

__YY_Thunks_Expand_Function(version, GetFileVersionInfoExA, 20);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
// Windows Vista [desktop apps only]
// Windows Server 2008 [desktop apps only]

EXTERN_C
DWORD
APIENTRY
GetFileVersionInfoSizeExW(_In_ DWORD dwFlags, _In_ LPCWSTR lpwstrFilename, _Out_ LPDWORD lpdwHandle)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto pGetFileVersionInfoSizeExW = try_get_GetFileVersionInfoSizeExW())
    {
        return pGetFileVersionInfoSizeExW(dwFlags, lpwstrFilename, lpdwHandle);
    }

    return GetFileVersionInfoSizeW(lpwstrFilename, lpdwHandle);
}
#endif

__YY_Thunks_Expand_Function(version, GetFileVersionInfoSizeExW, 12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
// Windows Vista [desktop apps only]
// Windows Server 2008 [desktop apps only]

EXTERN_C
DWORD
APIENTRY
GetFileVersionInfoSizeExA(_In_ DWORD dwFlags, _In_ LPCSTR lpwstrFilename, _Out_ LPDWORD lpdwHandle)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto pGetFileVersionInfoSizeExA = try_get_GetFileVersionInfoSizeExA())
    {
        return pGetFileVersionInfoSizeExA(dwFlags, lpwstrFilename, lpdwHandle);
    }

    return GetFileVersionInfoSizeA(lpwstrFilename, lpdwHandle);
}
#endif

__YY_Thunks_Expand_Function(version, GetFileVersionInfoSizeExA, 12);

#endif
} // namespace Thunks

} // namespace YY
