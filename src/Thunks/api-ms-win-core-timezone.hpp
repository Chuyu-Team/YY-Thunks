
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

						if (wcsicmp(StandardName, pTimeZoneInformation->StandardName) == 0)
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
	}
}