#include "pch.h"
#include "Thunks/credui.hpp"

namespace Credui
{
    TEST_CLASS(CredPackAuthenticationBufferW)
    {
        AwaysNullGuard Guard;

    public:
        CredPackAuthenticationBufferW()
        {
            Guard |= YY::Thunks::aways_null_try_get_CredPackAuthenticationBufferW;
            Guard |= YY::Thunks::aways_null_try_get_CredUnPackAuthenticationBufferW;
        }

        TEST_METHOD(打包并解包)
        {
            WCHAR _szUser[] = L"Domain\\User";
            WCHAR _szPassword[] = L"P@ssw0rd";

            DWORD _cbPacked = 0;
            Assert::IsTrue(::CredPackAuthenticationBufferW(0, _szUser, _szPassword, nullptr, &_cbPacked));
            Assert::IsTrue(_cbPacked > 0);

            std::vector<BYTE> _Packed(_cbPacked);
            Assert::IsTrue(::CredPackAuthenticationBufferW(0, _szUser, _szPassword, _Packed.data(), &_cbPacked));

            WCHAR _szUnpackUser[CREDUI_MAX_USERNAME_LENGTH + 1] = {};
            WCHAR _szUnpackDomain[CREDUI_MAX_DOMAIN_TARGET_LENGTH + 1] = {};
            WCHAR _szUnpackPassword[CREDUI_MAX_PASSWORD_LENGTH + 1] = {};

            DWORD _cchUser = _countof(_szUnpackUser);
            DWORD _cchDomain = _countof(_szUnpackDomain);
            DWORD _cchPassword = _countof(_szUnpackPassword);

            Assert::IsTrue(::CredUnPackAuthenticationBufferW(
                0,
                _Packed.data(),
                _cbPacked,
                _szUnpackUser,
                &_cchUser,
                _szUnpackDomain,
                &_cchDomain,
                _szUnpackPassword,
                &_cchPassword));

            Assert::IsTrue(wcscmp(_szUnpackUser, _szUser) == 0);
            Assert::IsTrue(wcscmp(_szUnpackDomain, L"Domain") == 0);
            Assert::IsTrue(wcscmp(_szUnpackPassword, _szPassword) == 0);
        }

        TEST_METHOD(缓冲区不足)
        {
            WCHAR _szUser[] = L"A";
            WCHAR _szPassword[] = L"B";

            BYTE _Packed[8] = {};
            DWORD _cbPacked = _countof(_Packed);

            Assert::IsFalse(::CredPackAuthenticationBufferW(0, _szUser, _szPassword, _Packed, &_cbPacked));
            Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), GetLastError());
            Assert::IsTrue(_cbPacked > _countof(_Packed));
        }
    };

    TEST_CLASS(CredPackAuthenticationBufferA)
    {
        AwaysNullGuard Guard;

    public:
        CredPackAuthenticationBufferA()
        {
            Guard |= YY::Thunks::aways_null_try_get_CredPackAuthenticationBufferA;
            Guard |= YY::Thunks::aways_null_try_get_CredUnPackAuthenticationBufferA;
        }

        TEST_METHOD(打包并解包)
        {
            CHAR _szUser[] = "Domain\\User";
            CHAR _szPassword[] = "P@ssw0rd";

            DWORD _cbPacked = 0;
            Assert::IsTrue(::CredPackAuthenticationBufferA(0, _szUser, _szPassword, nullptr, &_cbPacked));
            Assert::IsTrue(_cbPacked > 0);

            std::vector<BYTE> _Packed(_cbPacked);
            Assert::IsTrue(::CredPackAuthenticationBufferA(0, _szUser, _szPassword, _Packed.data(), &_cbPacked));

            CHAR _szUnpackUser[CREDUI_MAX_USERNAME_LENGTH + 1] = {};
            CHAR _szUnpackDomain[CREDUI_MAX_DOMAIN_TARGET_LENGTH + 1] = {};
            CHAR _szUnpackPassword[CREDUI_MAX_PASSWORD_LENGTH + 1] = {};

            DWORD _cchUser = _countof(_szUnpackUser);
            DWORD _cchDomain = _countof(_szUnpackDomain);
            DWORD _cchPassword = _countof(_szUnpackPassword);

            Assert::IsTrue(::CredUnPackAuthenticationBufferA(
                0,
                _Packed.data(),
                _cbPacked,
                _szUnpackUser,
                &_cchUser,
                _szUnpackDomain,
                &_cchDomain,
                _szUnpackPassword,
                &_cchPassword));

            Assert::IsTrue(strcmp(_szUnpackUser, _szUser) == 0);
            Assert::IsTrue(strcmp(_szUnpackDomain, "Domain") == 0);
            Assert::IsTrue(strcmp(_szUnpackPassword, _szPassword) == 0);
        }

        TEST_METHOD(缓冲区不足)
        {
            CHAR _szUser[] = "A";
            CHAR _szPassword[] = "B";

            BYTE _Packed[8] = {};
            DWORD _cbPacked = _countof(_Packed);

            Assert::IsFalse(::CredPackAuthenticationBufferA(0, _szUser, _szPassword, _Packed, &_cbPacked));
            Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), GetLastError());
            Assert::IsTrue(_cbPacked > _countof(_Packed));
        }
    };

    TEST_CLASS(CredUIPromptForWindowsCredentialsW)
    {
        AwaysNullGuard Guard;

    public:
        CredUIPromptForWindowsCredentialsW()
        {
            Guard |= YY::Thunks::aways_null_try_get_CredUIPromptForWindowsCredentialsW;
        }

        TEST_METHOD(参数校验)
        {
            ULONG _uAuthPackage = 0;
            ULONG _cbOutAuthBuffer = 0;
            BOOL _bSave = FALSE;
            LPVOID _pOutAuthBuffer = reinterpret_cast<LPVOID>(1);

            auto _uResult = ::CredUIPromptForWindowsCredentialsW(
                nullptr,
                0,
                &_uAuthPackage,
                nullptr,
                0,
                nullptr,
                &_cbOutAuthBuffer,
                &_bSave,
                CREDUIWIN_GENERIC);
            Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), _uResult);

            _uResult = ::CredUIPromptForWindowsCredentialsW(
                nullptr,
                0,
                &_uAuthPackage,
                nullptr,
                0,
                &_pOutAuthBuffer,
                nullptr,
                &_bSave,
                CREDUIWIN_GENERIC);
            Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), _uResult);
        }
    };
}
