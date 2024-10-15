#include "pch.h"
#include "Thunks/Crypt32.hpp"

#include <wincrypt.h>

#pragma comment(lib, "Crypt32.lib")

namespace Crypt32
{
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
