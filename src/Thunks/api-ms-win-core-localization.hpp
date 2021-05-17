


namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
LCID
WINAPI
LocaleNameToLCID(
	_In_ LPCWSTR lpName,
	_In_ DWORD dwFlags
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pLocaleNameToLCID = try_get_LocaleNameToLCID())
	{
		return pLocaleNameToLCID(lpName, dwFlags);
	}

	//UNREFRENCED_PARAMETER(dwFlags);

	if (lpName == nullptr)
	{
		return GetUserDefaultLCID();
	}
	else if (__wcsnicmp_ascii(lpName, LOCALE_NAME_SYSTEM_DEFAULT, _countof(LOCALE_NAME_SYSTEM_DEFAULT)) == 0)
	{
		return GetSystemDefaultLCID();
	}


	// Map of locale name to LCID.
	struct LocaleNameToLcid
	{
		const wchar_t*  localeName;
		LANGID          lcid;
	};

	// clang-format off
	// Map of locale name to an index in LcidToLocaleNameTable, for Windows XP.
	// Data in this table has been obtained from National Language Support (NLS) API Reference at
	// http://msdn.microsoft.com/en-us/goglobal/bb896001.aspx
	// The table is sorted to improve search performance.
	static const LocaleNameToLcid LocaleNameToLcidTable[] =
	{
		{ L""           , 0x007f },
		{ L"af"         , 0x0036 },
		{ L"af-ZA"      , 0x0436 },
		{ L"ar"         , 0x0001 },
		{ L"ar-AE"      , 0x3801 },
		{ L"ar-BH"      , 0x3C01 },
		{ L"ar-DZ"      , 0x1401 },
		{ L"ar-EG"      , 0x0C01 },
		{ L"ar-IQ"      , 0x0801 },
		{ L"ar-JO"      , 0x2C01 },
		{ L"ar-KW"      , 0x3401 },
		{ L"ar-LB"      , 0x3001 },
		{ L"ar-LY"      , 0x1001 },
		{ L"ar-MA"      , 0x1801 },
		{ L"ar-OM"      , 0x2001 },
		{ L"ar-QA"      , 0x4001 },
		{ L"ar-SA"      , 0x0401 },
		{ L"ar-SY"      , 0x2801 },
		{ L"ar-TN"      , 0x1C01 },
		{ L"ar-YE"      , 0x2401 },
		{ L"az"         , 0x002C },
		{ L"az-AZ-Cyrl" , 0x082C },
		{ L"az-AZ-Latn" , 0x042C },
		{ L"be"         , 0x0023 },
		{ L"be-BY"      , 0x0423 },
		{ L"bg"         , 0x0002 },
		{ L"bg-BG"      , 0x0402 },
		{ L"bn-IN"      , 0x0445 },
		{ L"bs-BA-Latn" , 0x141A },
		{ L"ca"         , 0x0003 },
		{ L"ca-ES"      , 0x0403 },
		{ L"cs"         , 0x0005 },
		{ L"cs-CZ"      , 0x0405 },
		{ L"cy-GB"      , 0x0452 },
		{ L"da"         , 0x0006 },
		{ L"da-DK"      , 0x0406 },
		{ L"de"         , 0x0007 },
		{ L"de-AT"      , 0x0C07 },
		{ L"de-CH"      , 0x0807 },
		{ L"de-DE"      , 0x0407 },
		{ L"de-LI"      , 0x1407 },
		{ L"de-LU"      , 0x1007 },
		{ L"div"        , 0x0065 },
		{ L"div-MV"     , 0x0465 },
		{ L"el"         , 0x0008 },
		{ L"el-GR"      , 0x0408 },
		{ L"en"         , 0x0009 },
		{ L"en-AU"      , 0x0C09 },
		{ L"en-BZ"      , 0x2809 },
		{ L"en-CA"      , 0x1009 },
		{ L"en-CB"      , 0x2409 },
		{ L"en-GB"      , 0x0809 },
		{ L"en-IE"      , 0x1809 },
		{ L"en-JM"      , 0x2009 },
		{ L"en-NZ"      , 0x1409 },
		{ L"en-PH"      , 0x3409 },
		{ L"en-TT"      , 0x2C09 },
		{ L"en-US"      , 0x0409 },
		{ L"en-ZA"      , 0x1C09 },
		{ L"en-ZW"      , 0x3009 },
		{ L"es"         , 0x000A },
		{ L"es-AR"      , 0x2C0A },
		{ L"es-BO"      , 0x400A },
		{ L"es-CL"      , 0x340A },
		{ L"es-CO"      , 0x240A },
		{ L"es-CR"      , 0x140A },
		{ L"es-DO"      , 0x1C0A },
		{ L"es-EC"      , 0x300A },
		{ L"es-ES"      , 0x0C0A },
		{ L"es-GT"      , 0x100A },
		{ L"es-HN"      , 0x480A },
		{ L"es-MX"      , 0x080A },
		{ L"es-NI"      , 0x4C0A },
		{ L"es-PA"      , 0x180A },
		{ L"es-PE"      , 0x280A },
		{ L"es-PR"      , 0x500A },
		{ L"es-PY"      , 0x3C0A },
		{ L"es-SV"      , 0x440A },
		{ L"es-UY"      , 0x380A },
		{ L"es-VE"      , 0x200A },
		{ L"et"         , 0x0025 },
		{ L"et-EE"      , 0x0425 },
		{ L"eu"         , 0x002D },
		{ L"eu-ES"      , 0x042D },
		{ L"fa"         , 0x0029 },
		{ L"fa-IR"      , 0x0429 },
		{ L"fi"         , 0x000B },
		{ L"fi-FI"      , 0x040B },
		{ L"fo"         , 0x0038 },
		{ L"fo-FO"      , 0x0438 },
		{ L"fr"         , 0x000C },
		{ L"fr-BE"      , 0x080C },
		{ L"fr-CA"      , 0x0C0C },
		{ L"fr-CH"      , 0x100C },
		{ L"fr-FR"      , 0x040C },
		{ L"fr-LU"      , 0x140C },
		{ L"fr-MC"      , 0x180C },
		{ L"gl"         , 0x0056 },
		{ L"gl-ES"      , 0x0456 },
		{ L"gu"         , 0x0047 },
		{ L"gu-IN"      , 0x0447 },
		{ L"he"         , 0x000D },
		{ L"he-IL"      , 0x040D },
		{ L"hi"         , 0x0039 },
		{ L"hi-IN"      , 0x0439 },
		{ L"hr"         , 0x001A },
		{ L"hr-BA"      , 0x101A },
		{ L"hr-HR"      , 0x041A },
		{ L"hu"         , 0x000E },
		{ L"hu-HU"      , 0x040E },
		{ L"hy"         , 0x002B },
		{ L"hy-AM"      , 0x042B },
		{ L"id"         , 0x0021 },
		{ L"id-ID"      , 0x0421 },
		{ L"is"         , 0x000F },
		{ L"is-IS"      , 0x040F },
		{ L"it"         , 0x0010 },
		{ L"it-CH"      , 0x0810 },
		{ L"it-IT"      , 0x0410 },
		{ L"ja"         , 0x0011 },
		{ L"ja-JP"      , 0x0411 },
		{ L"ka"         , 0x0037 },
		{ L"ka-GE"      , 0x0437 },
		{ L"kk"         , 0x003F },
		{ L"kk-KZ"      , 0x043F },
		{ L"kn"         , 0x004B },
		{ L"kn-IN"      , 0x044B },
		{ L"ko"         , 0x0012 },
		{ L"ko-KR"      , 0x0412 },
		{ L"kok"        , 0x0057 },
		{ L"kok-IN"     , 0x0457 },
		{ L"ky"         , 0x0040 },
		{ L"ky-KG"      , 0x0440 },
		{ L"lt"         , 0x0027 },
		{ L"lt-LT"      , 0x0427 },
		{ L"lv"         , 0x0026 },
		{ L"lv-LV"      , 0x0426 },
		{ L"mi-NZ"      , 0x0481 },
		{ L"mk"         , 0x002F },
		{ L"mk-MK"      , 0x042F },
		{ L"ml-IN"      , 0x044C },
		{ L"mn"         , 0x0050 },
		{ L"mn-MN"      , 0x0450 },
		{ L"mr"         , 0x004E },
		{ L"mr-IN"      , 0x044E },
		{ L"ms"         , 0x003E },
		{ L"ms-BN"      , 0x083E },
		{ L"ms-MY"      , 0x043E },
		{ L"mt-MT"      , 0x043A },
		{ L"nb-NO"      , 0x0414 },
		{ L"nl"         , 0x0013 },
		{ L"nl-BE"      , 0x0813 },
		{ L"nl-NL"      , 0x0413 },
		{ L"nn-NO"      , 0x0814 },
		{ L"no"         , 0x0014 },
		{ L"ns-ZA"      , 0x046C },
		{ L"pa"         , 0x0046 },
		{ L"pa-IN"      , 0x0446 },
		{ L"pl"         , 0x0015 },
		{ L"pl-PL"      , 0x0415 },
		{ L"pt"         , 0x0016 },
		{ L"pt-BR"      , 0x0416 },
		{ L"pt-PT"      , 0x0816 },
		{ L"quz-BO"     , 0x046B },
		{ L"quz-EC"     , 0x086B },
		{ L"quz-PE"     , 0x0C6B },
		{ L"ro"         , 0x0018 },
		{ L"ro-RO"      , 0x0418 },
		{ L"ru"         , 0x0019 },
		{ L"ru-RU"      , 0x0419 },
		{ L"sa"         , 0x004F },
		{ L"sa-IN"      , 0x044F },
		{ L"se-FI"      , 0x0C3B },
		{ L"se-NO"      , 0x043B },
		{ L"se-SE"      , 0x083B },
		{ L"sk"         , 0x001B },
		{ L"sk-SK"      , 0x041B },
		{ L"sl"         , 0x0024 },
		{ L"sl-SI"      , 0x0424 },
		{ L"sma-NO"     , 0x183B },
		{ L"sma-SE"     , 0x1C3B },
		{ L"smj-NO"     , 0x103B },
		{ L"smj-SE"     , 0x143B },
		{ L"smn-FI"     , 0x243B },
		{ L"sms-FI"     , 0x203B },
		{ L"sq"         , 0x001C },
		{ L"sq-AL"      , 0x041C },
		{ L"sr"         , 0x7C1A },
		{ L"sr-BA-Cyrl" , 0x1C1A },
		{ L"sr-BA-Latn" , 0x181A },
		{ L"sr-SP-Cyrl" , 0x0C1A },
		{ L"sr-SP-Latn" , 0x081A },
		{ L"sv"         , 0x001D },
		{ L"sv-FI"      , 0x081D },
		{ L"sv-SE"      , 0x041D },
		{ L"sw"         , 0x0041 },
		{ L"sw-KE"      , 0x0441 },
		{ L"syr"        , 0x005A },
		{ L"syr-SY"     , 0x045A },
		{ L"ta"         , 0x0049 },
		{ L"ta-IN"      , 0x0449 },
		{ L"te"         , 0x004A },
		{ L"te-IN"      , 0x044A },
		{ L"th"         , 0x001E },
		{ L"th-TH"      , 0x041E },
		{ L"tn-ZA"      , 0x0432 },
		{ L"tr"         , 0x001F },
		{ L"tr-TR"      , 0x041F },
		{ L"tt"         , 0x0044 },
		{ L"tt-RU"      , 0x0444 },
		{ L"uk"         , 0x0022 },
		{ L"uk-UA"      , 0x0422 },
		{ L"ur"         , 0x0020 },
		{ L"ur-PK"      , 0x0420 },
		{ L"uz"         , 0x0043 },
		{ L"uz-UZ-Cyrl" , 0x0843 },
		{ L"uz-UZ-Latn" , 0x0443 },
		{ L"vi"         , 0x002A },
		{ L"vi-VN"      , 0x042A },
		{ L"xh-ZA"      , 0x0434 },
		{ L"zh-CHS"     , 0x0004 },
		{ L"zh-CHT"     , 0x7C04 },
		{ L"zh-CN"      , 0x0804 },
		{ L"zh-HK"      , 0x0C04 },
		{ L"zh-MO"      , 0x1404 },
		{ L"zh-SG"      , 0x1004 },
		{ L"zh-TW"      , 0x0404 },
		{ L"zu-ZA"      , 0x0435 }
	};
	// clang-format on



	int bottom = 0;
	int top = _countof(LocaleNameToLcidTable) - 1;

	while (bottom <= top)
	{
		int middle = (bottom + top) / 2;
		int testIndex = __wcsnicmp_ascii(lpName, LocaleNameToLcidTable[middle].localeName, LOCALE_NAME_MAX_LENGTH);

		if (testIndex == 0)
			return LocaleNameToLcidTable[middle].lcid;

		if (testIndex < 0)
			top = middle - 1;
		else
			bottom = middle + 1;
	}

	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
}
#endif

__YY_Thunks_Expand_Function(kernel32, LocaleNameToLCID, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
int
WINAPI
LCIDToLocaleName(
	_In_ LCID     Locale,
	_Out_writes_opt_(cchName) LPWSTR  lpName,
	_In_ int      cchName,
	_In_ DWORD    dwFlags
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pLCIDToLocaleName = try_get_LCIDToLocaleName())
	{
		return pLCIDToLocaleName(Locale, lpName, cchName, dwFlags);
	}

	if (Locale == 0 || (lpName == nullptr && cchName > 0) || cchName < 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	if (Locale == LOCALE_USER_DEFAULT)
		Locale = GetUserDefaultLCID();
	else if (Locale == LOCALE_SYSTEM_DEFAULT)
		Locale = GetSystemDefaultLCID();

	// Map of LCID to locale name.
	struct LcidToLocaleName
	{
		LANGID          lcid;
		const wchar_t*  localeName;
	};

	// clang-format off
	// Map of LCID to locale name for Windows XP.
	// Data in this table has been obtained from National Language Support (NLS) API Reference at
	// http://msdn.microsoft.com/en-us/goglobal/bb896001.aspx
	// The table is sorted to improve search performance.
	static const LcidToLocaleName LcidToLocaleNameTable[] =
	{
		{ 0x0001, L"ar"         },
		{ 0x0002, L"bg"         },
		{ 0x0003, L"ca"         },
		{ 0x0004, L"zh-CHS"     },
		{ 0x0005, L"cs"         },
		{ 0x0006, L"da"         },
		{ 0x0007, L"de"         },
		{ 0x0008, L"el"         },
		{ 0x0009, L"en"         },
		{ 0x000A, L"es"         },
		{ 0x000B, L"fi"         },
		{ 0x000C, L"fr"         },
		{ 0x000D, L"he"         },
		{ 0x000E, L"hu"         },
		{ 0x000F, L"is"         },
		{ 0x0010, L"it"         },
		{ 0x0011, L"ja"         },
		{ 0x0012, L"ko"         },
		{ 0x0013, L"nl"         },
		{ 0x0014, L"no"         },
		{ 0x0015, L"pl"         },
		{ 0x0016, L"pt"         },
		{ 0x0018, L"ro"         },
		{ 0x0019, L"ru"         },
		{ 0x001A, L"hr"         },
		{ 0x001B, L"sk"         },
		{ 0x001C, L"sq"         },
		{ 0x001D, L"sv"         },
		{ 0x001E, L"th"         },
		{ 0x001F, L"tr"         },
		{ 0x0020, L"ur"         },
		{ 0x0021, L"id"         },
		{ 0x0022, L"uk"         },
		{ 0x0023, L"be"         },
		{ 0x0024, L"sl"         },
		{ 0x0025, L"et"         },
		{ 0x0026, L"lv"         },
		{ 0x0027, L"lt"         },
		{ 0x0029, L"fa"         },
		{ 0x002A, L"vi"         },
		{ 0x002B, L"hy"         },
		{ 0x002C, L"az"         },
		{ 0x002D, L"eu"         },
		{ 0x002F, L"mk"         },
		{ 0x0036, L"af"         },
		{ 0x0037, L"ka"         },
		{ 0x0038, L"fo"         },
		{ 0x0039, L"hi"         },
		{ 0x003E, L"ms"         },
		{ 0x003F, L"kk"         },
		{ 0x0040, L"ky"         },
		{ 0x0041, L"sw"         },
		{ 0x0043, L"uz"         },
		{ 0x0044, L"tt"         },
		{ 0x0046, L"pa"         },
		{ 0x0047, L"gu"         },
		{ 0x0049, L"ta"         },
		{ 0x004A, L"te"         },
		{ 0x004B, L"kn"         },
		{ 0x004E, L"mr"         },
		{ 0x004F, L"sa"         },
		{ 0x0050, L"mn"         },
		{ 0x0056, L"gl"         },
		{ 0x0057, L"kok"        },
		{ 0x005A, L"syr"        },
		{ 0x0065, L"div"        },
		{ 0x007f, L""           },
		{ 0x0401, L"ar-SA"      },
		{ 0x0402, L"bg-BG"      },
		{ 0x0403, L"ca-ES"      },
		{ 0x0404, L"zh-TW"      },
		{ 0x0405, L"cs-CZ"      },
		{ 0x0406, L"da-DK"      },
		{ 0x0407, L"de-DE"      },
		{ 0x0408, L"el-GR"      },
		{ 0x0409, L"en-US"      },
		{ 0x040B, L"fi-FI"      },
		{ 0x040C, L"fr-FR"      },
		{ 0x040D, L"he-IL"      },
		{ 0x040E, L"hu-HU"      },
		{ 0x040F, L"is-IS"      },
		{ 0x0410, L"it-IT"      },
		{ 0x0411, L"ja-JP"      },
		{ 0x0412, L"ko-KR"      },
		{ 0x0413, L"nl-NL"      },
		{ 0x0414, L"nb-NO"      },
		{ 0x0415, L"pl-PL"      },
		{ 0x0416, L"pt-BR"      },
		{ 0x0418, L"ro-RO"      },
		{ 0x0419, L"ru-RU"      },
		{ 0x041A, L"hr-HR"      },
		{ 0x041B, L"sk-SK"      },
		{ 0x041C, L"sq-AL"      },
		{ 0x041D, L"sv-SE"      },
		{ 0x041E, L"th-TH"      },
		{ 0x041F, L"tr-TR"      },
		{ 0x0420, L"ur-PK"      },
		{ 0x0421, L"id-ID"      },
		{ 0x0422, L"uk-UA"      },
		{ 0x0423, L"be-BY"      },
		{ 0x0424, L"sl-SI"      },
		{ 0x0425, L"et-EE"      },
		{ 0x0426, L"lv-LV"      },
		{ 0x0427, L"lt-LT"      },
		{ 0x0429, L"fa-IR"      },
		{ 0x042A, L"vi-VN"      },
		{ 0x042B, L"hy-AM"      },
		{ 0x042C, L"az-AZ-Latn" },
		{ 0x042D, L"eu-ES"      },
		{ 0x042F, L"mk-MK"      },
		{ 0x0432, L"tn-ZA"      },
		{ 0x0434, L"xh-ZA"      },
		{ 0x0435, L"zu-ZA"      },
		{ 0x0436, L"af-ZA"      },
		{ 0x0437, L"ka-GE"      },
		{ 0x0438, L"fo-FO"      },
		{ 0x0439, L"hi-IN"      },
		{ 0x043A, L"mt-MT"      },
		{ 0x043B, L"se-NO"      },
		{ 0x043E, L"ms-MY"      },
		{ 0x043F, L"kk-KZ"      },
		{ 0x0440, L"ky-KG"      },
		{ 0x0441, L"sw-KE"      },
		{ 0x0443, L"uz-UZ-Latn" },
		{ 0x0444, L"tt-RU"      },
		{ 0x0445, L"bn-IN"      },
		{ 0x0446, L"pa-IN"      },
		{ 0x0447, L"gu-IN"      },
		{ 0x0449, L"ta-IN"      },
		{ 0x044A, L"te-IN"      },
		{ 0x044B, L"kn-IN"      },
		{ 0x044C, L"ml-IN"      },
		{ 0x044E, L"mr-IN"      },
		{ 0x044F, L"sa-IN"      },
		{ 0x0450, L"mn-MN"      },
		{ 0x0452, L"cy-GB"      },
		{ 0x0456, L"gl-ES"      },
		{ 0x0457, L"kok-IN"     },
		{ 0x045A, L"syr-SY"     },
		{ 0x0465, L"div-MV"     },
		{ 0x046B, L"quz-BO"     },
		{ 0x046C, L"ns-ZA"      },
		{ 0x0481, L"mi-NZ"      },
		{ 0x0801, L"ar-IQ"      },
		{ 0x0804, L"zh-CN"      },
		{ 0x0807, L"de-CH"      },
		{ 0x0809, L"en-GB"      },
		{ 0x080A, L"es-MX"      },
		{ 0x080C, L"fr-BE"      },
		{ 0x0810, L"it-CH"      },
		{ 0x0813, L"nl-BE"      },
		{ 0x0814, L"nn-NO"      },
		{ 0x0816, L"pt-PT"      },
		{ 0x081A, L"sr-SP-Latn" },
		{ 0x081D, L"sv-FI"      },
		{ 0x082C, L"az-AZ-Cyrl" },
		{ 0x083B, L"se-SE"      },
		{ 0x083E, L"ms-BN"      },
		{ 0x0843, L"uz-UZ-Cyrl" },
		{ 0x086B, L"quz-EC"     },
		{ 0x0C01, L"ar-EG"      },
		{ 0x0C04, L"zh-HK"      },
		{ 0x0C07, L"de-AT"      },
		{ 0x0C09, L"en-AU"      },
		{ 0x0C0A, L"es-ES"      },
		{ 0x0C0C, L"fr-CA"      },
		{ 0x0C1A, L"sr-SP-Cyrl" },
		{ 0x0C3B, L"se-FI"      },
		{ 0x0C6B, L"quz-PE"     },
		{ 0x1001, L"ar-LY"      },
		{ 0x1004, L"zh-SG"      },
		{ 0x1007, L"de-LU"      },
		{ 0x1009, L"en-CA"      },
		{ 0x100A, L"es-GT"      },
		{ 0x100C, L"fr-CH"      },
		{ 0x101A, L"hr-BA"      },
		{ 0x103B, L"smj-NO"     },
		{ 0x1401, L"ar-DZ"      },
		{ 0x1404, L"zh-MO"      },
		{ 0x1407, L"de-LI"      },
		{ 0x1409, L"en-NZ"      },
		{ 0x140A, L"es-CR"      },
		{ 0x140C, L"fr-LU"      },
		{ 0x141A, L"bs-BA-Latn" },
		{ 0x143B, L"smj-SE"     },
		{ 0x1801, L"ar-MA"      },
		{ 0x1809, L"en-IE"      },
		{ 0x180A, L"es-PA"      },
		{ 0x180C, L"fr-MC"      },
		{ 0x181A, L"sr-BA-Latn" },
		{ 0x183B, L"sma-NO"     },
		{ 0x1C01, L"ar-TN"      },
		{ 0x1C09, L"en-ZA"      },
		{ 0x1C0A, L"es-DO"      },
		{ 0x1C1A, L"sr-BA-Cyrl" },
		{ 0x1C3B, L"sma-SE"     },
		{ 0x2001, L"ar-OM"      },
		{ 0x2009, L"en-JM"      },
		{ 0x200A, L"es-VE"      },
		{ 0x203B, L"sms-FI"     },
		{ 0x2401, L"ar-YE"      },
		{ 0x2409, L"en-CB"      },
		{ 0x240A, L"es-CO"      },
		{ 0x243B, L"smn-FI"     },
		{ 0x2801, L"ar-SY"      },
		{ 0x2809, L"en-BZ"      },
		{ 0x280A, L"es-PE"      },
		{ 0x2C01, L"ar-JO"      },
		{ 0x2C09, L"en-TT"      },
		{ 0x2C0A, L"es-AR"      },
		{ 0x3001, L"ar-LB"      },
		{ 0x3009, L"en-ZW"      },
		{ 0x300A, L"es-EC"      },
		{ 0x3401, L"ar-KW"      },
		{ 0x3409, L"en-PH"      },
		{ 0x340A, L"es-CL"      },
		{ 0x3801, L"ar-AE"      },
		{ 0x380A, L"es-UY"      },
		{ 0x3C01, L"ar-BH"      },
		{ 0x3C0A, L"es-PY"      },
		{ 0x4001, L"ar-QA"      },
		{ 0x400A, L"es-BO"      },
		{ 0x440A, L"es-SV"      },
		{ 0x480A, L"es-HN"      },
		{ 0x4C0A, L"es-NI"      },
		{ 0x500A, L"es-PR"      },
		{ 0x7C04, L"zh-CHT"     },
		{ 0x7C1A, L"sr"         }
	};
	// clang-format on

	int bottom = 0;
	int top = _countof(LcidToLocaleNameTable) - 1;

	while (bottom <= top)
	{
		int middle = (bottom + top) / 2;
		int testIndex = Locale - LcidToLocaleNameTable[middle].lcid;

		if (testIndex == 0)
		{
			auto buffer = LcidToLocaleNameTable[middle].localeName;
			auto count = wcslen(buffer);

			if (cchName > 0)
			{
				if ((int)count >= cchName)
				{
					SetLastError(ERROR_INSUFFICIENT_BUFFER);
					return 0;
				}

				memcpy(lpName, buffer, count * sizeof(*buffer));

				lpName[count] = L'\0';
			}

			return (int)count + 1;
		}

		if (testIndex < 0)
			top = middle - 1;
		else
			bottom = middle + 1;
	}

	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
}
#endif

__YY_Thunks_Expand_Function(kernel32, LCIDToLocaleName, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
int
WINAPI
GetLocaleInfoEx(
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ LCTYPE LCType,
	_Out_writes_to_opt_(cchData, return) LPWSTR lpLCData,
	_In_ int cchData
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetLocaleInfoEx = try_get_GetLocaleInfoEx())
	{
		return pGetLocaleInfoEx(lpLocaleName, LCType, lpLCData, cchData);
	}

	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	return GetLocaleInfoW(Locale, LCType, lpLCData, cchData);
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetLocaleInfoEx, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
int
WINAPI
GetNumberFormatEx(
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ DWORD dwFlags,
	_In_ LPCWSTR lpValue,
	_In_opt_ CONST NUMBERFMTW *lpFormat,
	_Out_writes_opt_(cchNumber) LPWSTR lpNumberStr,
	_In_ int cchNumber
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetNumberFormatEx = try_get_GetNumberFormatEx())
	{
		return pGetNumberFormatEx(lpLocaleName, dwFlags, lpValue, lpFormat, lpNumberStr, cchNumber);
	}


	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	return GetNumberFormatW(Locale, dwFlags, lpValue, lpFormat, lpNumberStr, cchNumber);
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetNumberFormatEx, 24);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
int
WINAPI
GetCurrencyFormatEx(
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ DWORD dwFlags,
	_In_ LPCWSTR lpValue,
	_In_opt_ CONST CURRENCYFMTW *lpFormat,
	_Out_writes_opt_(cchCurrency) LPWSTR lpCurrencyStr,
	_In_ int cchCurrency
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetCurrencyFormatEx = try_get_GetCurrencyFormatEx())
	{
		return pGetCurrencyFormatEx(lpLocaleName, dwFlags, lpValue, lpFormat, lpCurrencyStr, cchCurrency);
	}


	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	return GetCurrencyFormatW(Locale, dwFlags, lpValue, lpFormat, lpCurrencyStr, cchCurrency);
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetCurrencyFormatEx, 24);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
int
WINAPI
GetUserDefaultLocaleName(
	_Out_writes_(cchLocaleName) LPWSTR lpLocaleName,
	_In_ int cchLocaleName
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetUserDefaultLocaleName = try_get_GetUserDefaultLocaleName())
	{
		return pGetUserDefaultLocaleName(lpLocaleName, cchLocaleName);
	}


	return LCIDToLocaleName(LOCALE_USER_DEFAULT, lpLocaleName, cchLocaleName, 0);
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetUserDefaultLocaleName, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
int
WINAPI
GetSystemDefaultLocaleName(
	_Out_writes_(cchLocaleName) LPWSTR lpLocaleName,
	_In_ int cchLocaleName
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetSystemDefaultLocaleName = try_get_GetSystemDefaultLocaleName())
	{
		return pGetSystemDefaultLocaleName(lpLocaleName, cchLocaleName);
	}


	return LCIDToLocaleName(LOCALE_SYSTEM_DEFAULT, lpLocaleName, cchLocaleName, 0);
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetSystemDefaultLocaleName, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
BOOL
WINAPI
EnumCalendarInfoExEx(
	_In_ CALINFO_ENUMPROCEXEX pCalInfoEnumProcExEx,
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ CALID Calendar,
	_In_opt_ LPCWSTR lpReserved,
	_In_ CALTYPE CalType,
	_In_ LPARAM lParam
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pEnumCalendarInfoExEx = try_get_EnumCalendarInfoExEx())
	{
		return pEnumCalendarInfoExEx(pCalInfoEnumProcExEx, lpLocaleName, Calendar, lpReserved, CalType, lParam);
	}

	LSTATUS lStatus;

	do
	{
		if (pCalInfoEnumProcExEx == nullptr)
		{
			lStatus = ERROR_INVALID_PARAMETER;
			break;
		}

		auto Locale = LocaleNameToLCID(lpLocaleName, 0);

		if (Locale == 0)
		{
			lStatus = ERROR_INVALID_PARAMETER;
			break;
		}

		// clang-format off
#if defined(_X86_)
		constexpr const auto lParamOffset = 1;
		constexpr const auto pCallBackOffset = 16;

		static constexpr const byte ThunkData[] =
		{
			//lpCalendarInfoString = 0Ch + 4h
			//Calendar             = 08h + 8h
			0x68, 0x00, 0x00, 0x00, 0x00,       // push    lParam                              ; lParam
			0x6A, 0x00,                         // push    0                                   ; lpReserved
			0xFF, 0x74, 0x24, 0x10,             // push    dword ptr Calendar[esp]             ; Calendar
			0xFF, 0x74, 0x24, 0x10,             // push    dword ptr lpCalendarInfoString[esp] ; lpCalendarInfoString

			0xB8, 0x00, 0x00, 0x00, 0x00,       // mov     eax, pCalInfoEnumProcExEx
			0xFF, 0xD0,                         // call    eax
			0xC2, 0x08, 0x00,                   // retn    8
		};
#elif defined(_AMD64_)
		constexpr const auto lParamOffset = 2;
		constexpr const auto pCallBackOffset = 15;

		static constexpr const byte ThunkData[] =
		{
			//lpCalendarInfoString = rcx
			//Calendar             = edx
			0x49, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     r9, lParam
			0x45, 0x33, 0xC0,                                           // xor     r8d, r8d                 ; lpReserved

			0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     rax, pCalInfoEnumProcExEx
			0xFF, 0xE0,                                                 // jmp     rax
		};
#endif
		// clang-format on

		auto pFun = (byte*)VirtualAlloc(nullptr, sizeof(ThunkData), MEM_COMMIT, PAGE_READWRITE);

		if (!pFun)
		{
			lStatus = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}

		memcpy(pFun, ThunkData, sizeof(ThunkData));
		*(size_t*)(pFun + lParamOffset) = lParam;
		*(size_t*)(pFun + pCallBackOffset) = (size_t)pCalInfoEnumProcExEx;

		DWORD flOldProtect;
		VirtualProtect(pFun, sizeof(ThunkData), PAGE_EXECUTE_READ, &flOldProtect);

		auto bSuccess = EnumCalendarInfoExW(
			(CALINFO_ENUMPROCEXW)pFun,
			Locale,
			Calendar,
			CalType);

		lStatus = bSuccess ? ERROR_SUCCESS : GetLastError();

		VirtualFree(pFun, 0, MEM_RELEASE);
	} while (false);

	if (lStatus == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		SetLastError(lStatus);
		return FALSE;
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, EnumCalendarInfoExEx, 24);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
BOOL
WINAPI
EnumDateFormatsExEx(
    _In_ DATEFMT_ENUMPROCEXEX lpDateFmtEnumProcExEx,
    _In_opt_ LPCWSTR lpLocaleName,
    _In_ DWORD dwFlags,
    _In_ LPARAM lParam
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pEnumDateFormatsExEx = try_get_EnumDateFormatsExEx())
	{
		return pEnumDateFormatsExEx(lpDateFmtEnumProcExEx, lpLocaleName, dwFlags, lParam);
	}

	LSTATUS lStatus;

	do
	{
		if (lpDateFmtEnumProcExEx == nullptr)
		{
			lStatus = ERROR_INVALID_PARAMETER;
			break;
		}

		auto Locale = LocaleNameToLCID(lpLocaleName, 0);

		if (Locale == 0)
		{
			lStatus = ERROR_INVALID_PARAMETER;
			break;
		}

		// clang-format off
#if defined(_X86_)
		constexpr const auto lParamOffset = 1;
		constexpr const auto pCallBackOffset = 14;

		static constexpr const byte ThunkData[] =
		{
			//lpDateFormatString = 08h + 4h
			//CalendarID         = 04h + 8h
			0x68, 0x00, 0x00, 0x00, 0x00,       // push    lParam                              ; lParam
			0xFF, 0x74, 0x24, 0x0C,             // push    dword ptr CalendarID[esp]           ; CalendarID
			0xFF, 0x74, 0x24, 0x0C,             // push    dword ptr lpDateFormatString[esp]   ; lpDateFormatString

			0xB8, 0x00, 0x00, 0x00, 0x00,       // mov     eax, lpDateFmtEnumProcExEx
			0xFF, 0xD0,                         // call    eax
			0xC2, 0x08, 0x00,                   // retn    8
		};
#elif defined(_AMD64_)
		constexpr const auto lParamOffset = 2;
		constexpr const auto pCallBackOffset = 12;

		static constexpr const byte ThunkData[] =
		{
			//lpDateFormatString = rcx
			//CalendarID         = edx
			0x49, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     r8, lParam

			0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     rax, lpDateFmtEnumProcExEx
			0xFF, 0xE0,                                                 // jmp     rax
		};
#endif
		// clang-format on

		auto pFun = (byte*)VirtualAlloc(nullptr, sizeof(ThunkData), MEM_COMMIT, PAGE_READWRITE);

		if (!pFun)
		{
			lStatus = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}

		memcpy(pFun, ThunkData, sizeof(ThunkData));
		*(size_t*)(pFun + lParamOffset) = lParam;
		*(size_t*)(pFun + pCallBackOffset) = (size_t)lpDateFmtEnumProcExEx;

		DWORD flOldProtect;
		VirtualProtect(pFun, sizeof(ThunkData), PAGE_EXECUTE_READ, &flOldProtect);

		auto bSuccess = EnumDateFormatsExW(
			(DATEFMT_ENUMPROCEXW)pFun,
			Locale,
			dwFlags);

		lStatus = bSuccess ? ERROR_SUCCESS : GetLastError();

		VirtualFree(pFun, 0, MEM_RELEASE);
	} while (false);

	if (lStatus == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		SetLastError(lStatus);
		return FALSE;
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, EnumDateFormatsExEx, 16);

#endif


	}//namespace Thunks

} //namespace YY