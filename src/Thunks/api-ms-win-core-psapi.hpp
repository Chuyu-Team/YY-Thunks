

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
BOOL
WINAPI
QueryFullProcessImageNameW(
    _In_ HANDLE hProcess,
    _In_ DWORD dwFlags,
    _Out_writes_to_(*lpdwSize, *lpdwSize) LPWSTR lpExeName,
    _Inout_ PDWORD lpdwSize
    )
#ifdef YY_Thunks_Defined
	;
#else
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

__YY_Thunks_Expand_Function(kernel32, QueryFullProcessImageNameW, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
BOOL
WINAPI
QueryFullProcessImageNameA(
    _In_ HANDLE hProcess,
    _In_ DWORD dwFlags,
    _Out_writes_to_(*lpdwSize, *lpdwSize) LPSTR lpExeName,
    _Inout_ PDWORD lpdwSize
    )
#ifdef YY_Thunks_Defined
	;
#else
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

__YY_Thunks_Expand_Function(kernel32, QueryFullProcessImageNameA, 16);

#endif

	}//namespace Thunks

} //namespace YY