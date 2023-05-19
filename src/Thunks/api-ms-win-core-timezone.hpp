
#include <timezoneapi.h>

namespace YY
{
	namespace Thunks
	{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		_Success_(return != TIME_ZONE_ID_INVALID)
		DWORD,
		WINAPI,
		GetDynamicTimeZoneInformation,
			_Out_ PDYNAMIC_TIME_ZONE_INFORMATION pTimeZoneInformation
			)
		{
			if (const auto pGetDynamicTimeZoneInformation = try_get_GetDynamicTimeZoneInformation())
			{
				return pGetDynamicTimeZoneInformation(pTimeZoneInformation);
			}

			memset(pTimeZoneInformation, 0, sizeof(*pTimeZoneInformation));

			const auto Result = GetTimeZoneInformation((LPTIME_ZONE_INFORMATION)pTimeZoneInformation);

			if (Result == TIME_ZONE_ID_INVALID)
			{
				return Result;
			}

			
			pTimeZoneInformation->DynamicDaylightTimeDisabled = TIME_ZONE_ID_UNKNOWN == Result;

			//搜索 TimeZoneKeyName 名称

			do
			{
				HKEY hTimeZoneRootKey;
				if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion\Time Zones)", 0, KEY_READ, &hTimeZoneRootKey) != ERROR_SUCCESS)
				{
					break;
				}

				wchar_t szTimeZoneKeyName[128];

				for (unsigned i = 0; RegEnumKeyW(hTimeZoneRootKey, i, szTimeZoneKeyName, _countof(szTimeZoneKeyName)) == ERROR_SUCCESS;++i)
				{
					HKEY hTimeZoneKey;
					if (RegOpenKeyExW(hTimeZoneRootKey, szTimeZoneKeyName, 0, KEY_READ, &hTimeZoneKey) != ERROR_SUCCESS)
						continue;

					WCHAR StandardName[32] = {};
					//防止奇葩注册表，让
					DWORD cdStandardName = sizeof(StandardName);
					auto lStatus = RegQueryValueExW(hTimeZoneKey, L"Std", nullptr, nullptr, (LPBYTE)StandardName, &cdStandardName);

					RegCloseKey(hTimeZoneKey);

					if (lStatus == ERROR_SUCCESS)
					{
						//始终保持 0 截断
						StandardName[_countof(StandardName) - 1] = L'\0';

						if (__wcsnicmp_ascii(StandardName, pTimeZoneInformation->StandardName, _countof(pTimeZoneInformation->StandardName)) == 0)
						{
							memcpy(pTimeZoneInformation->TimeZoneKeyName, szTimeZoneKeyName, sizeof(szTimeZoneKeyName));

							break;
						}
					}
				}

				RegCloseKey(hTimeZoneRootKey);

			} while (false);

			
			return Result;

		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		4,
		BOOL,
		WINAPI,
		SetDynamicTimeZoneInformation,
			_In_ CONST DYNAMIC_TIME_ZONE_INFORMATION* lpTimeZoneInformation
			)
		{
			if (const auto pSetDynamicTimeZoneInformation = try_get_SetDynamicTimeZoneInformation())
			{
				return pSetDynamicTimeZoneInformation(lpTimeZoneInformation);
			}

			return SetTimeZoneInformation((TIME_ZONE_INFORMATION*)lpTimeZoneInformation);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		// 最低受支持的客户端	Windows 7 [桌面应用|UWP 应用]
		// 最低受支持的服务器	Windows Server 2012 [桌面应用|UWP 应用]
		__DEFINE_THUNK(
		kernel32,
		12,
		BOOL,
		WINAPI,
		TzSpecificLocalTimeToSystemTimeEx,
			_In_opt_ CONST DYNAMIC_TIME_ZONE_INFORMATION* _pTimeZoneInformation,
			_In_ CONST SYSTEMTIME* _pLocalTime,
			_Out_ LPSYSTEMTIME _pUniversalTime
			)
		{
			if (const auto _pfnTzSpecificLocalTimeToSystemTimeEx = try_get_TzSpecificLocalTimeToSystemTimeEx())
			{
				return _pfnTzSpecificLocalTimeToSystemTimeEx(_pTimeZoneInformation, _pLocalTime, _pUniversalTime);
			}

			return TzSpecificLocalTimeToSystemTime((LPTIME_ZONE_INFORMATION)_pTimeZoneInformation, _pLocalTime, _pUniversalTime);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WINXP)

		// 最低受支持的客户端	Windows XP [桌面应用|UWP 应用]
		// 最低受支持的服务器	Windows Server 2003 [桌面应用|UWP 应用]
		__DEFINE_THUNK(
		kernel32,
		12,
		BOOL,
		WINAPI,
		TzSpecificLocalTimeToSystemTime,
			_In_opt_ CONST TIME_ZONE_INFORMATION* _pTimeZoneInformation,
			_In_ CONST SYSTEMTIME* _pLocalTime,
			_Out_ LPSYSTEMTIME _pUniversalTime
			)
		{
			if (const auto _pfnTzSpecificLocalTimeToSystemTime = try_get_TzSpecificLocalTimeToSystemTime())
			{
				return _pfnTzSpecificLocalTimeToSystemTime(_pTimeZoneInformation, _pLocalTime, _pUniversalTime);
			}

			// 下面实现来自XP系统。

			// Win2K存在此函数
			const auto _pfnRtlCutoverTimeToSystemTime = try_get_RtlCutoverTimeToSystemTime();
			if (!_pfnRtlCutoverTimeToSystemTime)
			{
				internal::RaiseStatus(STATUS_NOT_FOUND);
			}

			// Get the timezone information into a useful format
			TIME_ZONE_INFORMATION _TmpTimeZoneInformation;
			if (!_pTimeZoneInformation)
			{
				// Convert universal time to local time using current timezone info
				if (GetTimeZoneInformation(&_TmpTimeZoneInformation) == TIME_ZONE_ID_INVALID)
				{
					return FALSE;
				}

				_pTimeZoneInformation = &_TmpTimeZoneInformation;
			}

			// convert the input local time to NT style time
			LARGE_INTEGER _CurrentLocalTime;
			if (!SystemTimeToFileTime(_pLocalTime, (LPFILETIME)&_CurrentLocalTime))
			{
				return FALSE;
			}

			// Get the new timezone bias
			auto _iTimeZoneBias = _pTimeZoneInformation->Bias;

			// Now see if we have stored cutover times
			if (_pTimeZoneInformation->StandardDate.wMonth && _pTimeZoneInformation->DaylightDate.wMonth)
			{
				LARGE_INTEGER _StandardTime;
				TIME_FIELDS _StandardStart;
				_StandardStart.Year = _pTimeZoneInformation->StandardDate.wYear;
				_StandardStart.Month = _pTimeZoneInformation->StandardDate.wMonth;
				_StandardStart.Weekday = _pTimeZoneInformation->StandardDate.wDayOfWeek;
				_StandardStart.Day = _pTimeZoneInformation->StandardDate.wDay;
				_StandardStart.Hour = _pTimeZoneInformation->StandardDate.wHour;
				_StandardStart.Minute = _pTimeZoneInformation->StandardDate.wMinute;
				_StandardStart.Second = _pTimeZoneInformation->StandardDate.wSecond;
				_StandardStart.Milliseconds = _pTimeZoneInformation->StandardDate.wMilliseconds;

				// We have timezone cutover information. Compute the
				// cutover dates and compute what our current bias
				// is
				if (!_pfnRtlCutoverTimeToSystemTime(&_StandardStart, &_StandardTime, &_CurrentLocalTime, TRUE))
				{
					SetLastError(ERROR_INVALID_PARAMETER);
					return FALSE;
				}

				LARGE_INTEGER _DaylightTime;
				TIME_FIELDS _DaylightStart;
				_DaylightStart.Year = _pTimeZoneInformation->DaylightDate.wYear;
				_DaylightStart.Month = _pTimeZoneInformation->DaylightDate.wMonth;
				_DaylightStart.Weekday = _pTimeZoneInformation->DaylightDate.wDayOfWeek;
				_DaylightStart.Day = _pTimeZoneInformation->DaylightDate.wDay;
				_DaylightStart.Hour = _pTimeZoneInformation->DaylightDate.wHour;
				_DaylightStart.Minute = _pTimeZoneInformation->DaylightDate.wMinute;
				_DaylightStart.Second = _pTimeZoneInformation->DaylightDate.wSecond;
				_DaylightStart.Milliseconds = _pTimeZoneInformation->DaylightDate.wMilliseconds;

				if (!_pfnRtlCutoverTimeToSystemTime(&_DaylightStart, &_DaylightTime, &_CurrentLocalTime, TRUE))
				{
					SetLastError(ERROR_INVALID_PARAMETER);
					return FALSE;
				}

				ULONG _uCurrentTimeZoneId = 0xffffffff;

				// If daylight < standard, then time >= daylight and
				// less than standard is daylight
				if (_DaylightTime.QuadPart < _StandardTime.QuadPart)
				{
					// If today is >= DaylightTime and < StandardTime, then
					// We are in daylight savings time
					if ((_CurrentLocalTime.QuadPart >= _DaylightTime.QuadPart) &&
						(_CurrentLocalTime.QuadPart < _StandardTime.QuadPart))
					{
						_uCurrentTimeZoneId = TIME_ZONE_ID_DAYLIGHT;
					}
					else
					{
						_uCurrentTimeZoneId = TIME_ZONE_ID_STANDARD;
					}
				}
				else
				{
					// If today is >= StandardTime and < DaylightTime, then
					// We are in standard time
					if ((_CurrentLocalTime.QuadPart >= _StandardTime.QuadPart) &&
						(_CurrentLocalTime.QuadPart < _DaylightTime.QuadPart))
					{
						_uCurrentTimeZoneId = TIME_ZONE_ID_STANDARD;
					}
					else
					{
						_uCurrentTimeZoneId = TIME_ZONE_ID_DAYLIGHT;
					}
				}

				// At this point, we know our current timezone and the
				// local time of the next cutover.
				if (_uCurrentTimeZoneId == TIME_ZONE_ID_DAYLIGHT)
				{
					_iTimeZoneBias += _pTimeZoneInformation->DaylightBias;
				}
				else
				{
					_iTimeZoneBias += _pTimeZoneInformation->StandardBias;
				}
			}

			// _iBias 单位是分钟，FileTime是 100ns 为步进
			_CurrentLocalTime.QuadPart += Int32x32To64(_iTimeZoneBias * 60, 10'000'000);
			return FileTimeToSystemTime((LPFILETIME)&_CurrentLocalTime, _pUniversalTime);
		}
#endif
	}
}