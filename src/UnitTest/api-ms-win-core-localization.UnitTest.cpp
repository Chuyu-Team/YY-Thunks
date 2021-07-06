
#include "pch.h"
#include "src/Thunks/api-ms-win-core-localization.hpp"
#include <set>

namespace api_ms_win_core_localization
{
	TEST_CLASS(GetThreadPreferredUILanguages)
	{
	public:
		GetThreadPreferredUILanguages()
		{
			YY::Thunks::aways_null_try_get_GetThreadPreferredUILanguages = true;
		}

		TEST_METHOD(无效参数验证)
		{
			wchar_t szLanguagesBuffer[512];
			ULONG cchLanguagesBuffer = _countof(szLanguagesBuffer);
			ULONG ulNumLanguages = 0;

			//pulNumLanguages 不能为 nullptr
			auto bRet = ::GetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, nullptr, szLanguagesBuffer, &cchLanguagesBuffer);
			Assert::IsFalse(bRet);

			cchLanguagesBuffer = _countof(szLanguagesBuffer);
			//pwszLanguagesBuffer 为nullptr 时， pcchLanguagesBuffer 必须为 0
			bRet = ::GetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, &ulNumLanguages, nullptr, &cchLanguagesBuffer);
			Assert::IsFalse(bRet);


			//pcchLanguagesBuffer不能为 nullptr
			bRet = ::GetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, &ulNumLanguages, szLanguagesBuffer, nullptr);
			Assert::IsFalse(bRet);


			//MUI_LANGUAGE_NAME | MUI_LANGUAGE_ID不能同时使用
			cchLanguagesBuffer = _countof(szLanguagesBuffer);
			bRet = ::GetThreadPreferredUILanguages(MUI_LANGUAGE_NAME | MUI_LANGUAGE_ID, &ulNumLanguages, szLanguagesBuffer, &cchLanguagesBuffer);
			Assert::IsFalse(bRet);

			//MUI_MERGE_SYSTEM_FALLBACK | MUI_THREAD_LANGUAGES不能同时使用
			cchLanguagesBuffer = _countof(szLanguagesBuffer);
			bRet = ::GetThreadPreferredUILanguages(MUI_MERGE_SYSTEM_FALLBACK | MUI_THREAD_LANGUAGES, &ulNumLanguages, szLanguagesBuffer, &cchLanguagesBuffer);
			Assert::IsFalse(bRet);

			//MUI_MERGE_USER_FALLBACK | MUI_THREAD_LANGUAGES不能同时使用
			cchLanguagesBuffer = _countof(szLanguagesBuffer);
			bRet = ::GetThreadPreferredUILanguages(MUI_MERGE_USER_FALLBACK | MUI_THREAD_LANGUAGES, &ulNumLanguages, szLanguagesBuffer, &cchLanguagesBuffer);
			Assert::IsFalse(bRet);

			//不允许使用MUI_CONSOLE_FILTER
			cchLanguagesBuffer = _countof(szLanguagesBuffer);
			bRet = ::GetThreadPreferredUILanguages(MUI_CONSOLE_FILTER, &ulNumLanguages, szLanguagesBuffer, &cchLanguagesBuffer);
			Assert::IsFalse(bRet);

			//不允许使用MUI_COMPLEX_SCRIPT_FILTER
			cchLanguagesBuffer = _countof(szLanguagesBuffer);
			bRet = ::GetThreadPreferredUILanguages(MUI_COMPLEX_SCRIPT_FILTER, &ulNumLanguages, szLanguagesBuffer, &cchLanguagesBuffer);
			Assert::IsFalse(bRet);
		}

		TEST_METHOD(MUI_LANGUAGE_ID验证)
		{
			wchar_t szLanguagesBuffer[512];
			ULONG cchLanguagesBuffer = _countof(szLanguagesBuffer);
			ULONG ulNumLanguages = 0;

			//pulNumLanguages 不能为 nullptr
			auto bRet = ::GetThreadPreferredUILanguages(MUI_LANGUAGE_ID | MUI_THREAD_LANGUAGES, &ulNumLanguages, szLanguagesBuffer, &cchLanguagesBuffer);
			Assert::IsTrue(bRet);

			Assert::AreEqual(ulNumLanguages, 1ul);
			Assert::AreEqual(cchLanguagesBuffer, 6ul);

			Assert::AreEqual(szLanguagesBuffer[4], L'\0');
			Assert::AreEqual(szLanguagesBuffer[5], L'\0');

			CStringW ThreadHex;
			ThreadHex.Format(L"%.4x", GetThreadLocale());

			Assert::AreEqual(ThreadHex.GetString(), szLanguagesBuffer, true);
		}


		TEST_METHOD(MUI_LANGUAGE_NAME验证)
		{

			//zh-cn\0\0
			SetThreadLocale(0x0804);

			{
				wchar_t szLanguagesBuffer[512];
				ULONG cchLanguagesBuffer = _countof(szLanguagesBuffer);
				ULONG ulNumLanguages = 0;

				//pulNumLanguages 不能为 nullptr
				auto bRet = ::GetThreadPreferredUILanguages(MUI_LANGUAGE_NAME | MUI_THREAD_LANGUAGES, &ulNumLanguages, szLanguagesBuffer, &cchLanguagesBuffer);
				Assert::IsTrue(bRet);

				Assert::AreEqual(ulNumLanguages, 1ul);
				Assert::AreEqual(cchLanguagesBuffer, 7ul);

				Assert::AreEqual(szLanguagesBuffer[5], L'\0');
				Assert::AreEqual(szLanguagesBuffer[6], L'\0');

				Assert::AreEqual(L"zh-cn", szLanguagesBuffer, true);
			}

			//默认为 MUI_LANGUAGE_NAME
			{
				wchar_t szLanguagesBuffer[512];
				ULONG cchLanguagesBuffer = _countof(szLanguagesBuffer);
				ULONG ulNumLanguages = 0;

				//pulNumLanguages 不能为 nullptr
				auto bRet = ::GetThreadPreferredUILanguages(MUI_THREAD_LANGUAGES, &ulNumLanguages, szLanguagesBuffer, &cchLanguagesBuffer);
				Assert::IsTrue(bRet);

				Assert::AreEqual(ulNumLanguages, 1ul);
				Assert::AreEqual(cchLanguagesBuffer, 7ul);

				Assert::AreEqual(szLanguagesBuffer[5], L'\0');
				Assert::AreEqual(szLanguagesBuffer[6], L'\0');

				Assert::AreEqual(L"zh-cn", szLanguagesBuffer, true);
			}

		}

		TEST_METHOD(Languages消重验证)
		{

			//zh-cn\0\0
			SetThreadLocale(GetUserDefaultLangID());

			{
				wchar_t szLanguagesBuffer[512];
				ULONG cchLanguagesBuffer = _countof(szLanguagesBuffer);
				ULONG ulNumLanguages = 0;

				//pulNumLanguages 不能为 nullptr
				auto bRet = ::GetThreadPreferredUILanguages(MUI_LANGUAGE_ID | MUI_MERGE_USER_FALLBACK, &ulNumLanguages, szLanguagesBuffer, &cchLanguagesBuffer);
				Assert::IsTrue(bRet);

				std::set<CStringW> Map;

				for (auto Str = szLanguagesBuffer; *Str; Str = Str + wcslen(Str) + 1)
				{
					Assert::IsTrue(Map.insert(CStringW(Str).MakeLower()).second);
				}

				Assert::IsTrue(Map.size() >= 1);
			}

			//
			{
				auto UserId = GetUserDefaultLangID();
				LANGID ThreadId;

				if (UserId == 0x0804)
				{
					//en-us
					ThreadId = 0x0409;
				}
				else
				{
					ThreadId = 0x0804;
				}

				SetThreadLocale(ThreadId);

				wchar_t szLanguagesBuffer[512];
				ULONG cchLanguagesBuffer = _countof(szLanguagesBuffer);
				ULONG ulNumLanguages = 0;

				//pulNumLanguages 不能为 nullptr
				auto bRet = ::GetThreadPreferredUILanguages(MUI_LANGUAGE_ID | MUI_MERGE_USER_FALLBACK, &ulNumLanguages, szLanguagesBuffer, &cchLanguagesBuffer);
				Assert::IsTrue(bRet);

				std::set<CStringW> Map;

				for (auto Str = szLanguagesBuffer; *Str; Str = Str + wcslen(Str) + 1)
				{
					Assert::IsTrue(Map.insert(CStringW(Str).MakeLower()).second);
				}

				Assert::IsTrue(Map.size() >= 1);
			}

		}

	};


	TEST_CLASS(ResolveLocaleName)
	{
	public:
		ResolveLocaleName()
		{
			YY::Thunks::aways_null_try_get_ResolveLocaleName = true;
		}

		TEST_METHOD(中性语言验证)
		{
			wchar_t Buffer[LOCALE_NAME_MAX_LENGTH] = {};

			auto result = ::ResolveLocaleName(L"zh", Buffer, _countof(Buffer));

			Assert::AreEqual(result, (int)_countof(L"zh-CN"));

			Assert::AreEqual(Buffer, L"zh-CN", true);


			result = ::ResolveLocaleName(L"zh-Hant", Buffer, _countof(Buffer));

			Assert::AreEqual(result, (int)_countof(L"zh-HK"));

			Assert::AreEqual(Buffer, L"zh-HK", true);



			result = ::ResolveLocaleName(L"mn-Mong", Buffer, _countof(Buffer));

			Assert::AreEqual(result, (int)_countof(L"mn-Mong-CN"));

			Assert::AreEqual(Buffer, L"mn-Mong-CN", true);
		}

		TEST_METHOD(特定区域语言验证)
		{
			wchar_t Buffer[LOCALE_NAME_MAX_LENGTH] = {};

			auto result = ::ResolveLocaleName(L"zh-TW", Buffer, _countof(Buffer));

			Assert::AreEqual(result, (int)_countof(L"zh-TW"));

			Assert::AreEqual(Buffer, L"zh-TW", true);


			result = ::ResolveLocaleName(L"es-ES_tradnl", Buffer, _countof(Buffer));

			Assert::AreEqual(result, (int)_countof(L"es-ES_tradnl"));

			Assert::AreEqual(Buffer, L"es-ES_tradnl", true);
		}

		TEST_METHOD(特定区域排序语言验证)
		{
			wchar_t Buffer[LOCALE_NAME_MAX_LENGTH] = {};

			auto result = ::ResolveLocaleName(L"x-IV-mathan", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L""));
			Assert::AreEqual(Buffer, L"", true);

			result = ::ResolveLocaleName(L"de-DE_phoneb", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"de-DE"));
			Assert::AreEqual(Buffer, L"de-DE", true);


			result = ::ResolveLocaleName(L"hu-HU_technl", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"hu-HU"));
			Assert::AreEqual(Buffer, L"hu-HU", true);


			result = ::ResolveLocaleName(L"ka-GE_modern", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"ka-GE"));
			Assert::AreEqual(Buffer, L"ka-GE", true);


			result = ::ResolveLocaleName(L"zh-CN_stroke", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"zh-CN"));
			Assert::AreEqual(Buffer, L"zh-CN", true);

			result = ::ResolveLocaleName(L"zh-SG_stroke", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"zh-SG"));
			Assert::AreEqual(Buffer, L"zh-SG", true);

			result = ::ResolveLocaleName(L"zh-MO_stroke", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"zh-MO"));
			Assert::AreEqual(Buffer, L"zh-MO", true);


			result = ::ResolveLocaleName(L"zh-TW_pronun", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"zh-TW"));
			Assert::AreEqual(Buffer, L"zh-TW", true);


			result = ::ResolveLocaleName(L"zh-TW_radstr", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"zh-TW"));
			Assert::AreEqual(Buffer, L"zh-TW", true);

			result = ::ResolveLocaleName(L"ja-JP_radstr", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"ja-JP"));
			Assert::AreEqual(Buffer, L"ja-JP", true);

			result = ::ResolveLocaleName(L"zh-HK_radstr", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"zh-HK"));
			Assert::AreEqual(Buffer, L"zh-HK", true);


			result = ::ResolveLocaleName(L"zh-MO_radstr", Buffer, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"zh-MO"));
			Assert::AreEqual(Buffer, L"zh-MO", true);


		}

		TEST_METHOD(自动回退验证)
		{
			wchar_t Buffer[LOCALE_NAME_MAX_LENGTH] = {};

			auto result = ::ResolveLocaleName(L"zh-TW-XXXXXX", Buffer, _countof(Buffer));

			Assert::AreEqual(result, (int)_countof(L"zh-TW"));

			Assert::AreEqual(Buffer, L"zh-TW", true);



			result = ::ResolveLocaleName(L"zh-Hant-XXXXXX", Buffer, _countof(Buffer));

			Assert::AreEqual(result, (int)_countof(L"zh-HK"));

			Assert::AreEqual(Buffer, L"zh-HK", true);
		}


		TEST_METHOD(默认值验证)
		{
			wchar_t Buffer[LOCALE_NAME_MAX_LENGTH] = {};

			auto result = ::ResolveLocaleName(nullptr, Buffer, _countof(Buffer));

			Assert::AreNotEqual(result, 0);

			Assert::AreEqual(::LocaleNameToLCID(Buffer, 0), ::GetUserDefaultLCID());

			
			::ResolveLocaleName(LOCALE_NAME_SYSTEM_DEFAULT, Buffer, _countof(Buffer));

			Assert::AreNotEqual(result, 0);

			Assert::AreEqual(::LocaleNameToLCID(Buffer, 0), ::GetSystemDefaultLCID());

			result = ::ResolveLocaleName(L"wrwrwrwr", Buffer, _countof(Buffer));
			Assert::AreEqual(result, 1);

			Assert::AreEqual(Buffer, L"", true);
		}

		TEST_METHOD(仅返回长度验证)
		{
			wchar_t Buffer[LOCALE_NAME_MAX_LENGTH] = {};

			auto result = ::ResolveLocaleName(L"zh-CN", Buffer, 0);
			Assert::AreEqual(result, (int)_countof(L"zh-CN"));



			result = ::ResolveLocaleName(L"zh-CN", 0, _countof(Buffer));
			Assert::AreEqual(result, (int)_countof(L"zh-CN"));


			result = ::ResolveLocaleName(L"zh-CN", 0, 0);
			Assert::AreEqual(result, (int)_countof(L"zh-CN"));
		}
	};
}