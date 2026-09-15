#include "pch.h"
#include "Thunks/Crypt32.hpp"

#include <bcrypt.h>
#include <wincrypt.h>

#pragma comment(lib, "Crypt32.lib")

namespace Crypt32
{
    TEST_CLASS(CryptHashCertificate2)
    {
        AwaysNullGuard Guard;
    public:
        CryptHashCertificate2()
        {
            Guard |= YY::Thunks::aways_null_try_get_CryptHashCertificate2;
        }

        TEST_METHOD(MD5)
        {
            static const BYTE kEncoded[] = { 1, 2, 3 };
            static const BYTE kEncodedHash[] = {0x52, 0x89, 0xdf, 0x73, 0x7d, 0xf5, 0x73, 0x26, 0xfc, 0xdd, 0x22, 0x59, 0x7a, 0xfb, 0x1f, 0xac };

            DWORD _cbFallbackHash = 0;
            Assert::IsTrue(::CryptHashCertificate2(BCRYPT_MD5_ALGORITHM, 0, nullptr, kEncoded, sizeof(kEncoded), nullptr, &_cbFallbackHash));

            std::vector<BYTE> _FallbackHash(_cbFallbackHash);
            Assert::IsTrue(::CryptHashCertificate2(BCRYPT_MD5_ALGORITHM, 0, nullptr, kEncoded, sizeof(kEncoded), _FallbackHash.data(), &_cbFallbackHash));

            Assert::AreEqual(ToHexString(kEncodedHash), ToHexString(_FallbackHash));
        }

        TEST_METHOD(SHA1)
        {
            static const BYTE kEncoded[] = { 1, 2, 3 };
            static const BYTE kEncodedHash[] = {0x70, 0x37, 0x80, 0x71, 0x98, 0xc2, 0x2a, 0x7d, 0x2b, 0x08, 0x07, 0x37, 0x1d, 0x76, 0x37, 0x79, 0xa8, 0x4f, 0xdf, 0xcf};

            DWORD _cbFallbackHash = 0;
            Assert::IsTrue(::CryptHashCertificate2(BCRYPT_SHA1_ALGORITHM, 0, nullptr, kEncoded, sizeof(kEncoded), nullptr, &_cbFallbackHash));

            std::vector<BYTE> _FallbackHash(_cbFallbackHash);
            Assert::IsTrue(::CryptHashCertificate2(BCRYPT_SHA1_ALGORITHM, 0, nullptr, kEncoded, sizeof(kEncoded), _FallbackHash.data(), &_cbFallbackHash));

            Assert::AreEqual(ToHexString(kEncodedHash), ToHexString(_FallbackHash));
        }

        TEST_METHOD(SHA256)
        {
            static const BYTE kEncoded[] = { 1, 2, 3 };
            static const BYTE kEncodedHash[] = { 0x03, 0x90, 0x58, 0xc6, 0xf2, 0xc0, 0xcb, 0x49, 0x2c, 0x53, 0x3b, 0x0a, 0x4d, 0x14, 0xef, 0x77, 0xcc, 0x0f, 0x78, 0xab, 0xcc, 0xce, 0xd5, 0x28, 0x7d, 0x84, 0xa1, 0xa2, 0x01, 0x1c, 0xfb, 0x81 };

            DWORD _cbFallbackHash = 0;
            Assert::IsTrue(::CryptHashCertificate2(BCRYPT_SHA256_ALGORITHM, 0, nullptr, kEncoded, sizeof(kEncoded), nullptr, &_cbFallbackHash));

            std::vector<BYTE> _FallbackHash(_cbFallbackHash);
            Assert::IsTrue(::CryptHashCertificate2(BCRYPT_SHA256_ALGORITHM, 0, nullptr, kEncoded, sizeof(kEncoded), _FallbackHash.data(), &_cbFallbackHash));

            Assert::AreEqual(ToHexString(kEncodedHash), ToHexString(_FallbackHash));
        }
    };

    TEST_CLASS(CryptBinaryToStringW)
    {
        AwaysNullGuard Guard;

    public:
        CryptBinaryToStringW()
        {
            g_uSystemVersion = MakeVersion(5, 1, 2600);
        }

        ~CryptBinaryToStringW()
        {
            g_uSystemVersion = 0;
        }

        TEST_METHOD(CRYPT_STRING_NOCRLF参数验证)
        {
            wchar_t _szUnitTestDll[512];
            GetModuleFileNameW((HMODULE)&__ImageBase, _szUnitTestDll, std::size(_szUnitTestDll));

            const auto _FileData = ReadFileData(_szUnitTestDll);
            Assert::AreNotEqual(_FileData.size(), size_t(0));

            {
                DWORD _cchOut = 0;
                Assert::IsTrue(::CryptBinaryToStringW((const BYTE*)_FileData.c_str(), _FileData.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &_cchOut));

                auto _szOutBuffer = new wchar_t[_cchOut];
                Assert::IsNotNull(_szOutBuffer);

                Assert::IsTrue(::CryptBinaryToStringW((const BYTE*)_FileData.c_str(), _FileData.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, _szOutBuffer, &_cchOut));

                Assert::AreEqual(size_t(_cchOut), wcslen(_szOutBuffer));

                Assert::IsNull(wcschr(_szOutBuffer, L'\r'));
                Assert::IsNull(wcschr(_szOutBuffer, L'\n'));

                DWORD _cbBinary = 0;
                Assert::IsTrue(::CryptStringToBinaryW(_szOutBuffer, _cchOut, CRYPT_STRING_BASE64, nullptr, &_cbBinary, nullptr, nullptr));
                Assert::AreEqual(size_t(_cbBinary), _FileData.size());

                std::string _Binary;
                _Binary.resize(_cbBinary);
                Assert::IsTrue(::CryptStringToBinaryW(_szOutBuffer, _cchOut, CRYPT_STRING_BASE64, (BYTE*)_Binary.data(), &_cbBinary, nullptr, nullptr));
                Assert::AreEqual(_FileData, _Binary);

                delete[] _szOutBuffer;
            }

            {
                DWORD _cchOut = 0;
                Assert::IsTrue(::CryptBinaryToStringA((const BYTE*)_FileData.c_str(), _FileData.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &_cchOut));

                auto _szOutBuffer = new char[_cchOut];
                Assert::IsNotNull(_szOutBuffer);

                Assert::IsTrue(::CryptBinaryToStringA((const BYTE*)_FileData.c_str(), _FileData.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, _szOutBuffer, &_cchOut));

                Assert::AreEqual(size_t(_cchOut), strlen(_szOutBuffer));

                Assert::IsNull(strchr(_szOutBuffer, '\r'));
                Assert::IsNull(strchr(_szOutBuffer, '\n'));

                DWORD _cbBinary = 0;
                Assert::IsTrue(::CryptStringToBinaryA(_szOutBuffer, _cchOut, CRYPT_STRING_BASE64, nullptr, &_cbBinary, nullptr, nullptr));
                Assert::AreEqual(size_t(_cbBinary), _FileData.size());

                std::string _Binary;
                _Binary.resize(_cbBinary);
                Assert::IsTrue(::CryptStringToBinaryA(_szOutBuffer, _cchOut, CRYPT_STRING_BASE64, (BYTE*)_Binary.data(), &_cbBinary, nullptr, nullptr));
                Assert::AreEqual(_FileData, _Binary);

                delete[] _szOutBuffer;
            }
        }
    };
}
