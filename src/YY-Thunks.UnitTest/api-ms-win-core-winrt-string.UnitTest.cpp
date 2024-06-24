#include "pch.h"
#include "Thunks/api-ms-win-core-winrt-string.hpp"

namespace api_ms_win_core_winrt_string
{
    TEST_CLASS(WindowsCreateString)
    {
        AwaysNullGuard Guard;

    public:
        WindowsCreateString()
        {
            Guard |= YY::Thunks::aways_null_try_get_WindowsCreateString;
            Guard |= YY::Thunks::aways_null_try_get_WindowsDeleteString;
            Guard |= YY::Thunks::aways_null_try_get_WindowsGetStringRawBuffer;
        }

        TEST_METHOD(标准C字符串)
        {
            const wchar_t _szInput[] = L"TestString";
            const auto _cInput = _countof(_szInput) - 1;

            HSTRING _hString = nullptr;
            HRESULT hr = ::WindowsCreateString(_szInput, _cInput, &_hString);
            Assert::AreEqual(hr, (HRESULT)S_OK);

            UINT32 _cWindowsStringLength;
            auto _pszBuffer = ::WindowsGetStringRawBuffer(_hString, &_cWindowsStringLength);
            Assert::IsNotNull(_pszBuffer);
            Assert::AreEqual((size_t)_cWindowsStringLength, _cInput);

            Assert::IsTrue(memcmp(_pszBuffer, _szInput, sizeof(_szInput)) == 0);

            ::WindowsDeleteString(_hString);
        }

        TEST_METHOD(中途带零的字符串)
        {
            const wchar_t _szInput[] = L"Test\0String";
            const auto _cInput = _countof(_szInput) - 1;

            HSTRING _hString = nullptr;
            HRESULT hr = ::WindowsCreateString(_szInput, _cInput, &_hString);
            Assert::AreEqual(hr, (HRESULT)S_OK);

            UINT32 _cWindowsStringLength;
            auto _pszBuffer = ::WindowsGetStringRawBuffer(_hString, &_cWindowsStringLength);
            Assert::IsNotNull(_pszBuffer);
            Assert::AreEqual((size_t)_cWindowsStringLength, _cInput);

            Assert::IsTrue(memcmp(_pszBuffer, _szInput, sizeof(_szInput)) == 0);

            ::WindowsDeleteString(_hString);
        }

        TEST_METHOD(结尾多一个零的字符串)
        {
            const wchar_t _szInput[] = L"TestString\0";
            const auto _cInput = _countof(_szInput) - 1;

            HSTRING _hString = nullptr;
            HRESULT hr = ::WindowsCreateString(_szInput, _cInput, &_hString);
            Assert::AreEqual(hr, (HRESULT)S_OK);

            UINT32 _cWindowsStringLength;
            auto _pszBuffer = ::WindowsGetStringRawBuffer(_hString, &_cWindowsStringLength);
            Assert::IsNotNull(_pszBuffer);
            Assert::AreEqual((size_t)_cWindowsStringLength, _cInput);

            Assert::IsTrue(memcmp(_pszBuffer, _szInput, sizeof(_szInput)) == 0);

            ::WindowsDeleteString(_hString);
        }

        TEST_METHOD(中途截断的字符串)
        {
            const wchar_t _szInput[] = L"TestString";
            const wchar_t _szExpect[] = L"Test";
            const size_t _cInput = _countof(_szExpect) - 1;

            HSTRING _hString = nullptr;
            HRESULT hr = ::WindowsCreateString(_szInput, _cInput, &_hString);
            Assert::AreEqual(hr, (HRESULT)S_OK);

            UINT32 _cWindowsStringLength;
            auto _pszBuffer = ::WindowsGetStringRawBuffer(_hString, &_cWindowsStringLength);
            Assert::IsNotNull(_pszBuffer);
            Assert::AreEqual((size_t)_cWindowsStringLength, _cInput);

            Assert::IsTrue(memcmp(_pszBuffer, _szExpect, sizeof(_szExpect)) == 0);

            ::WindowsDeleteString(_hString);
        }
    };

    TEST_CLASS(WindowsStringHasEmbeddedNull)
    {
        AwaysNullGuard Guard;

    public:
        WindowsStringHasEmbeddedNull()
        {
            Guard |= YY::Thunks::aways_null_try_get_WindowsCreateString;
            Guard |= YY::Thunks::aways_null_try_get_WindowsDeleteString;
            Guard |= YY::Thunks::aways_null_try_get_WindowsStringHasEmbeddedNull;
        }

        TEST_METHOD(标准C字符串)
        {
            const wchar_t _szInput[] = L"TestString";
            const auto _cInput = _countof(_szInput) - 1;

            HSTRING _hString = nullptr;
            HRESULT hr = ::WindowsCreateString(_szInput, _cInput, &_hString);
            Assert::AreEqual(hr, (HRESULT)S_OK);

            BOOL _bEmbeddedNull = FALSE;
            hr = ::WindowsStringHasEmbeddedNull(_hString, &_bEmbeddedNull);
            Assert::AreEqual(hr, (HRESULT)S_OK);
            Assert::IsFalse(_bEmbeddedNull);

            ::WindowsDeleteString(_hString);
        }

        TEST_METHOD(中途带零的字符串)
        {
            const wchar_t _szInput[] = L"Test\0String";
            const auto _cInput = _countof(_szInput) - 1;

            HSTRING _hString = nullptr;
            HRESULT hr = ::WindowsCreateString(_szInput, _cInput, &_hString);
            Assert::AreEqual(hr, (HRESULT)S_OK);

            BOOL _bEmbeddedNull = FALSE;
            hr = ::WindowsStringHasEmbeddedNull(_hString, &_bEmbeddedNull);
            Assert::AreEqual(hr, (HRESULT)S_OK);
            Assert::IsTrue(_bEmbeddedNull);

            ::WindowsDeleteString(_hString);
        }

        TEST_METHOD(结尾多一个零的字符串)
        {
            const wchar_t _szInput[] = L"TestString\0";
            const auto _cInput = _countof(_szInput) - 1;

            HSTRING _hString = nullptr;
            HRESULT hr = ::WindowsCreateString(_szInput, _cInput, &_hString);
            Assert::AreEqual(hr, (HRESULT)S_OK);

            BOOL _bEmbeddedNull = FALSE;
            hr = ::WindowsStringHasEmbeddedNull(_hString, &_bEmbeddedNull);
            Assert::AreEqual(hr, (HRESULT)S_OK);
            Assert::IsTrue(_bEmbeddedNull);

            ::WindowsDeleteString(_hString);
        }
    };

    TEST_CLASS(WindowsCompareStringOrdinal)
    {
        AwaysNullGuard Guard;

    public:
        WindowsCompareStringOrdinal()
        {
            Guard |= YY::Thunks::aways_null_try_get_WindowsCreateStringReference;
            Guard |= YY::Thunks::aways_null_try_get_WindowsDeleteString;
            Guard |= YY::Thunks::aways_null_try_get_WindowsCompareStringOrdinal;
        }

        TEST_METHOD(常规验证)
        {
            {
                HSTRING_HEADER _HeaderLeft;
                HSTRING _hStringLeft;
                auto hr = WindowsCreateStringReference(L"123", 3, &_HeaderLeft, &_hStringLeft);
                Assert::AreEqual(hr, (HRESULT)S_OK);

                HSTRING_HEADER _HeaderRigth;
                HSTRING _hStringRigth;
                hr = WindowsCreateStringReference(L"123", 3, &_HeaderRigth, &_hStringRigth);
                Assert::AreEqual(hr, (HRESULT)S_OK);

                INT32 _nResult = 0xCC;
                hr = ::WindowsCompareStringOrdinal(_hStringLeft, _hStringRigth, &_nResult);
                Assert::AreEqual(hr, (HRESULT)S_OK);
                Assert::AreEqual(_nResult, 0);

                WindowsDeleteString(_hStringLeft);
                WindowsDeleteString(_hStringRigth);
            }

            {
                HSTRING_HEADER _HeaderLeft;
                HSTRING _hStringLeft;
                auto hr = WindowsCreateStringReference(L"1234", 4, &_HeaderLeft, &_hStringLeft);
                Assert::AreEqual(hr, (HRESULT)S_OK);

                HSTRING_HEADER _HeaderRigth;
                HSTRING _hStringRigth;
                hr = WindowsCreateStringReference(L"123", 3, &_HeaderRigth, &_hStringRigth);
                Assert::AreEqual(hr, (HRESULT)S_OK);

                INT32 _nResult = 0xCC;
                hr = ::WindowsCompareStringOrdinal(_hStringLeft, _hStringRigth, &_nResult);
                Assert::AreEqual(hr, (HRESULT)S_OK);
                Assert::AreEqual(_nResult, 1);

                WindowsDeleteString(_hStringLeft);
                WindowsDeleteString(_hStringRigth);
            }

            {
                HSTRING_HEADER _HeaderLeft;
                HSTRING _hStringLeft;
                auto hr = WindowsCreateStringReference(L"123", 3, &_HeaderLeft, &_hStringLeft);
                Assert::AreEqual(hr, (HRESULT)S_OK);

                HSTRING_HEADER _HeaderRigth;
                HSTRING _hStringRigth;
                hr = WindowsCreateStringReference(L"1234", 4, &_HeaderRigth, &_hStringRigth);
                Assert::AreEqual(hr, (HRESULT)S_OK);

                INT32 _nResult = 0xCC;
                hr = ::WindowsCompareStringOrdinal(_hStringLeft, _hStringRigth, &_nResult);
                Assert::AreEqual(hr, (HRESULT)S_OK);
                Assert::AreEqual(_nResult, -1);

                WindowsDeleteString(_hStringLeft);
                WindowsDeleteString(_hStringRigth);
            }
        }

        TEST_METHOD(空值验证)
        {
            {
                HSTRING_HEADER _HeaderLeft;
                HSTRING _hStringLeft;
                auto hr = WindowsCreateStringReference(L"123", 3, &_HeaderLeft, &_hStringLeft);
                Assert::AreEqual(hr, (HRESULT)S_OK);

                INT32 _nResult = 0xCC;
                hr = ::WindowsCompareStringOrdinal(_hStringLeft, nullptr, &_nResult);
                Assert::AreEqual(hr, (HRESULT)S_OK);
                Assert::AreEqual(_nResult, 1);

                WindowsDeleteString(_hStringLeft);
            }

            {
                HSTRING_HEADER _HeaderRigth;
                HSTRING _hStringRigth;
                auto hr = WindowsCreateStringReference(L"123", 3, &_HeaderRigth, &_hStringRigth);
                Assert::AreEqual(hr, (HRESULT)S_OK);

                INT32 _nResult = 0xCC;
                hr = ::WindowsCompareStringOrdinal(nullptr, _hStringRigth, &_nResult);
                Assert::AreEqual(hr, (HRESULT)S_OK);
                Assert::AreEqual(_nResult, -1);

                WindowsDeleteString(_hStringRigth);
            }

            {
                INT32 _nResult = 0xCC;
                auto hr = ::WindowsCompareStringOrdinal(nullptr, nullptr, &_nResult);
                Assert::AreEqual(hr, (HRESULT)S_OK);
                Assert::AreEqual(_nResult, 0);
            }

            {
                HSTRING_HEADER _HeaderLeft;
                HSTRING _hStringLeft;
                auto hr = WindowsCreateStringReference(L"", 0, &_HeaderLeft, &_hStringLeft);
                Assert::AreEqual(hr, (HRESULT)S_OK);

                INT32 _nResult = 0xCC;
                hr = ::WindowsCompareStringOrdinal(_hStringLeft, nullptr, &_nResult);
                Assert::AreEqual(hr, (HRESULT)S_OK);
                Assert::AreEqual(_nResult, 0);

                WindowsDeleteString(_hStringLeft);
            }
        }
    };

    TEST_CLASS(WindowsDuplicateString)
    {
        AwaysNullGuard Guard;

    public:
        WindowsDuplicateString()
        {
            Guard |= YY::Thunks::aways_null_try_get_WindowsCreateString;
            Guard |= YY::Thunks::aways_null_try_get_WindowsCreateStringReference;
            Guard |= YY::Thunks::aways_null_try_get_WindowsDuplicateString;
            Guard |= YY::Thunks::aways_null_try_get_WindowsGetStringRawBuffer;
        }

        TEST_METHOD(标准行为验证)
        {
            {
                const wchar_t _szInput[] = L"TestString";
                const auto _cInput = _countof(_szInput) - 1;

                HSTRING _hString = nullptr;
                HRESULT hr = ::WindowsCreateString(_szInput, _cInput, &_hString);
                Assert::AreEqual(hr, (HRESULT)S_OK);

                HSTRING _hNewString = nullptr;
                hr = ::WindowsDuplicateString(_hString, &_hNewString);
                Assert::AreEqual(hr, (HRESULT)S_OK);
                // WindowsCreateString 的字符串直接增加引用计数即可
                Assert::AreEqual((void*)_hString, (void*)_hNewString);

                UINT32 _cWindowsStringLength;
                auto _pszBuffer = ::WindowsGetStringRawBuffer(_hNewString, &_cWindowsStringLength);
                Assert::IsNotNull(_pszBuffer);
                Assert::AreEqual((size_t)_cWindowsStringLength, _cInput);

                Assert::IsTrue(memcmp(_pszBuffer, _szInput, sizeof(_szInput)) == 0);

                ::WindowsDeleteString(_hString);
                ::WindowsDeleteString(_hNewString);
            }

            {
                const wchar_t _szInput[] = L"TestString";
                const auto _cInput = _countof(_szInput) - 1;

                HSTRING _hString = nullptr;
                HSTRING_HEADER _Header;
                HRESULT hr = ::WindowsCreateStringReference(_szInput, _cInput, &_Header, &_hString);
                Assert::AreEqual(hr, (HRESULT)S_OK);

                HSTRING _hNewString = nullptr;
                hr = ::WindowsDuplicateString(_hString, &_hNewString);
                Assert::AreEqual(hr, (HRESULT)S_OK);
                // WindowsCreateString 的字符串直接增加引用计数即可
                Assert::AreNotEqual((void*)_hString, (void*)_hNewString);

                UINT32 _cWindowsStringLength;
                auto _pszBuffer = ::WindowsGetStringRawBuffer(_hNewString, &_cWindowsStringLength);
                Assert::IsNotNull(_pszBuffer);
                Assert::AreEqual((size_t)_cWindowsStringLength, _cInput);

                Assert::IsTrue(memcmp(_pszBuffer, _szInput, sizeof(_szInput)) == 0);

                ::WindowsDeleteString(_hString);
                ::WindowsDeleteString(_hNewString);
            }

            {
                HSTRING _hNewString = nullptr;
                auto hr = ::WindowsDuplicateString(nullptr, &_hNewString);
                Assert::AreEqual(hr, (HRESULT)S_OK);
                // WindowsCreateString 的字符串直接增加引用计数即可
                Assert::AreEqual((void*)nullptr, (void*)_hNewString);
            }
        }
    };
}
