#include "pch.h"
#include "Thunks/api-ms-win-core-winrt-string.hpp"

namespace api_ms_win_core_winrt_string
{
	TEST_CLASS(WindowsStringHasEmbeddedNull)
	{
        AwaysNullGuard Guard;

	public:
        WindowsStringHasEmbeddedNull()
		{
            Guard |= YY::Thunks::aways_null_try_get_WindowsCreateString;
            Guard |= YY::Thunks::aways_null_try_get_WindowsStringHasEmbeddedNull;
		}

		TEST_METHOD(标准C字符串)
		{
            wchar_t RawString[] = L"TestString";
            const UINT32 Length = static_cast<UINT32>(sizeof(RawString) / sizeof(*RawString));

            HSTRING String = nullptr;
            HRESULT hr = ::WindowsCreateString(RawString, Length, &String);
            Assert::AreEqual(hr, (HRESULT)S_OK);
            BOOL Result = FALSE;
            hr = ::WindowsStringHasEmbeddedNull(String, &Result);
            Assert::AreEqual(hr, (HRESULT)S_OK);
            Assert::IsTrue(Result);
		}

        TEST_METHOD(中途带零的字符串)
        {
            wchar_t RawString[] = L"Test\0String";
            const UINT32 Length = static_cast<UINT32>(sizeof(RawString) / sizeof(*RawString));

            HSTRING String = nullptr;
            HRESULT hr = ::WindowsCreateString(RawString, Length, &String);
            Assert::AreEqual(hr, (HRESULT)S_OK);
            BOOL Result = FALSE;
            hr = ::WindowsStringHasEmbeddedNull(String, &Result);
            Assert::AreEqual(hr, (HRESULT)S_OK);
            Assert::IsTrue(Result);
        }

        TEST_METHOD(不带零的字符串)
        {
            wchar_t RawString[] = L"TestString";
            const UINT32 Length = static_cast<UINT32>(sizeof(RawString) / sizeof(*RawString)) -1;

            HSTRING String = nullptr;
            HRESULT hr = ::WindowsCreateString(RawString, Length, &String);
            Assert::AreEqual(hr, (HRESULT)S_OK);
            BOOL Result = FALSE;
            hr = ::WindowsStringHasEmbeddedNull(String, &Result);
            Assert::AreEqual(hr, (HRESULT)S_OK);
            Assert::IsFalse(Result);
        }
	};
}
