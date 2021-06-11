


namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
int
WINAPI
GetDateFormatEx(
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ DWORD dwFlags,
	_In_opt_ CONST SYSTEMTIME* lpDate,
	_In_opt_ LPCWSTR lpFormat,
	_Out_writes_opt_(cchDate) LPWSTR lpDateStr,
	_In_ int cchDate,
	_In_opt_ LPCWSTR lpCalendar
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetDateFormatEx = try_get_GetDateFormatEx())
	{
		return pGetDateFormatEx(lpLocaleName, dwFlags, lpDate, lpFormat, lpDateStr, cchDate, lpCalendar);
	}

	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	return GetDateFormatW(Locale, dwFlags, lpDate, lpFormat, lpDateStr, cchDate);
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetDateFormatEx, 28);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
int
WINAPI
GetTimeFormatEx(
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ DWORD dwFlags,
	_In_opt_ CONST SYSTEMTIME* lpTime,
	_In_opt_ LPCWSTR lpFormat,
	_Out_writes_opt_(cchTime) LPWSTR lpTimeStr,
	_In_ int cchTime
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetTimeFormatEx = try_get_GetTimeFormatEx())
	{
		return pGetTimeFormatEx(lpLocaleName, dwFlags, lpTime, lpFormat, lpTimeStr, cchTime);
	}

	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	return GetTimeFormatW(Locale, dwFlags, lpTime, lpFormat, lpTimeStr, cchTime);
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetTimeFormatEx, 24);

#endif

	}//namespace Thunks

} //namespace YY