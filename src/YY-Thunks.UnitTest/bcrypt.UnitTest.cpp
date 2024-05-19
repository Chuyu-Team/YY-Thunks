#include "pch.h"
#include "Thunks/bcrypt.hpp"

#include <vector>

#pragma comment(lib, "bcrypt.lib")

CStringW GetStringHex(const BYTE* _pData, DWORD _cbData)
{
    CStringW _szResult;

    _szResult += L'{';

    for (DWORD i = 0; i != _cbData; ++i)
    {
        if ((i % 8) == 0)
        {
            _szResult += L'\r';
            _szResult += L'\n';
            _szResult += L' ';
            _szResult += L' ';
            _szResult += L' ';
            _szResult += L' ';
        }
        _szResult.AppendFormat(L"0x%.2X, ", _pData[i]);
    }

    _szResult += L'\r';
    _szResult += L'\n';
    _szResult += L'}';

    return _szResult;
}


namespace bcrypt
{
	TEST_CLASS(BCryptGenRandom)
	{
        AwaysNullGuard Guard;

	public:
		BCryptGenRandom()
		{
            Guard |= YY::Thunks::aways_null_try_get_BCryptOpenAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptCloseAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptGenRandom;
		}

		TEST_METHOD(BCRYPT_USE_SYSTEM_PREFERRED_RNG模式)
		{
			UCHAR _Temp[15] = {};
			long _Status = ::BCryptGenRandom(nullptr, _Temp, sizeof(_Temp), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
			Assert::IsTrue(_Status >= 0);
			
			const UCHAR _Temp2[15] = {};
			Assert::IsFalse(memcpy(_Temp, _Temp2, sizeof(_Temp2)) == 0);
		}

		TEST_METHOD(BCryptOpen在关闭)
		{
			BCRYPT_ALG_HANDLE _hAlg = nullptr;
			auto _Status = ::BCryptOpenAlgorithmProvider(&_hAlg, BCRYPT_RNG_ALGORITHM, nullptr, 0);
			Assert::IsTrue(_Status >= 0);
			Assert::IsNotNull(_hAlg);

			UCHAR _Temp[15] = {};
			_Status = ::BCryptGenRandom(_hAlg, _Temp, sizeof(_Temp), 0);
			Assert::IsTrue(_Status >= 0);

			const UCHAR _Temp2[15] = {};
			Assert::IsFalse(memcpy(_Temp, _Temp2, sizeof(_Temp2)) == 0);

			_Status = ::BCryptCloseAlgorithmProvider(_hAlg, 0);
			Assert::IsTrue(_Status >= 0);
		}
	};

    TEST_CLASS(BCryptHash)
    {
        AwaysNullGuard Guard;
    public:
        BCryptHash()
        {
            Guard |= YY::Thunks::aways_null_try_get_BCryptOpenAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptCloseAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptGenRandom;
            Guard |= YY::Thunks::aways_null_try_get_BCryptCreateHash;
            Guard |= YY::Thunks::aways_null_try_get_BCryptDestroyHash;
            Guard |= YY::Thunks::aways_null_try_get_BCryptHashData;
            Guard |= YY::Thunks::aways_null_try_get_BCryptFinishHash;
        }

        template<size_t kTargetShaLength>
        static void TestHash(_In_z_ LPCWSTR _szAlgId, _In_ ULONG _fFlags, _In_ const byte (&_pTargetSha)[kTargetShaLength], const char* _szKey = nullptr)
        {
            BCRYPT_ALG_HANDLE _hAlg = nullptr;
            int _Status = ::BCryptOpenAlgorithmProvider(&_hAlg, _szAlgId, nullptr, _fFlags);
            Assert::IsTrue(_Status >= 0);
            Assert::IsNotNull(_hAlg);
            BCRYPT_HASH_HANDLE _hHash = nullptr;
            _Status = ::BCryptCreateHash(_hAlg, &_hHash, nullptr, 0, (PUCHAR)_szKey, _szKey ? (ULONG)strlen(_szKey) : 0, 0);
            Assert::IsTrue(_Status >= 0);
            Assert::IsNotNull(_hHash);

            _Status = ::BCryptHashData(_hHash, (PUCHAR)"123", 3, 0);
            Assert::IsTrue(_Status >= 0);

            byte _ShaCurrent[kTargetShaLength] = {};
            _Status = ::BCryptFinishHash(_hHash, _ShaCurrent, sizeof(_ShaCurrent), 0);
            Assert::IsTrue(_Status >= 0);

            Assert::AreEqual(ToHexString(_ShaCurrent), ToHexString(_pTargetSha));

            _Status = ::BCryptDestroyHash(_hHash);
            Assert::IsTrue(_Status >= 0);
            _Status = ::BCryptCloseAlgorithmProvider(_hAlg, 0);
            Assert::IsTrue(_Status >= 0);
        }

        TEST_METHOD(MD5)
        {
            static const byte kTargetSha[16] = { 0x20, 0x2c, 0xb9, 0x62, 0xac, 0x59, 0x07, 0x5b, 0x96, 0x4b, 0x07, 0x15, 0x2d, 0x23, 0x4b, 0x70 };

            TestHash(BCRYPT_MD5_ALGORITHM, 0, kTargetSha);
        }

        TEST_METHOD(Sha1)
        {
            static const byte kTargetSha[20] = { 0x40, 0xbd, 0x00, 0x15, 0x63, 0x08, 0x5f, 0xc3, 0x51, 0x65, 0x32, 0x9e, 0xa1, 0xff, 0x5c, 0x5e, 0xcb, 0xdb, 0xbe, 0xef };

            TestHash(BCRYPT_SHA1_ALGORITHM, 0, kTargetSha);
        }

        TEST_METHOD(Sha256)
        {
            static const byte kTargetSha[32] = { 0xa6, 0x65, 0xa4, 0x59, 0x20, 0x42, 0x2f, 0x9d, 0x41, 0x7e, 0x48, 0x67, 0xef, 0xdc, 0x4f, 0xb8, 0xa0, 0x4a, 0x1f, 0x3f, 0xff, 0x1f, 0xa0, 0x7e, 0x99, 0x8e, 0x86, 0xf7, 0xf7, 0xa2, 0x7a, 0xe3 };

            TestHash(BCRYPT_SHA256_ALGORITHM, 0, kTargetSha);
        }

        TEST_METHOD(HMAC_Sha1)
        {
            struct TestInfo
            {
                const char* pKey;
                byte TargetSha1[20];
            };

            static const TestInfo s_Info[] =
            {
                {"", { 0x65, 0x8a, 0x09, 0x01, 0x62, 0x35, 0x68, 0xea, 0x5c, 0x36, 0x31, 0xcf, 0x61, 0x93, 0xa0, 0x23, 0xd6, 0x57, 0xae, 0x4f }},
                {"1", { 0x9e, 0x72, 0xd6, 0x6a, 0x7e, 0xe6, 0xf0, 0x1a, 0xbd, 0x7e, 0xd7, 0x6c, 0xcf, 0x94, 0xfb, 0x3e, 0x70, 0xf4, 0x0e, 0x45 }},
                {"456", { 0xc9, 0x4a, 0x24, 0x9b, 0x57, 0xc0, 0x62, 0xba, 0x17, 0x5d, 0xb6, 0x43, 0x41, 0x24, 0x22, 0xe8, 0xc3, 0x91, 0x63, 0x54 }},
            };

            for (auto& _Item : s_Info)
            {
                TestHash(BCRYPT_SHA1_ALGORITHM, BCRYPT_ALG_HANDLE_HMAC_FLAG, _Item.TargetSha1, _Item.pKey);
            }
        }
    };

    TEST_CLASS(BCryptDeriveKey)
    {
        AwaysNullGuard Guard;
    public:
        BCryptDeriveKey()
        {
            Guard |= YY::Thunks::aways_null_try_get_BCryptDeriveKeyPBKDF2;
            Guard |= YY::Thunks::aways_null_try_get_BCryptDeriveKeyCapi;
        }

        TEST_METHOD(PBKDF2)
        {
            {
                BCRYPT_ALG_HANDLE hAlgorithm = NULL;
                auto _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_SHA1_ALGORITHM, nullptr, BCRYPT_ALG_HANDLE_HMAC_FLAG);
                Assert::IsTrue(_Status >= 0);

                UCHAR _DerivedKey[20] = {};
                _Status = ::BCryptDeriveKeyPBKDF2(hAlgorithm, (PUCHAR)"123", 3, (PUCHAR)"123456789", 9, 4, _DerivedKey, sizeof(_DerivedKey), 0);
                Assert::IsTrue(_Status >= 0);

                static const UCHAR kTargetDerivedKey[] = { 0x04, 0x11, 0xaf, 0x2e, 0x9e, 0x1e, 0xfd, 0x17, 0x7c, 0xc2, 0x06, 0x6a, 0xa0, 0x74, 0xf5, 0x79, 0x44, 0x32, 0x8b, 0x3f };
                Assert::AreEqual(ToHexString(_DerivedKey), ToHexString(kTargetDerivedKey));

            }

            {
                BCRYPT_ALG_HANDLE hAlgorithm = NULL;
                auto _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_SHA1_ALGORITHM, nullptr, BCRYPT_ALG_HANDLE_HMAC_FLAG);
                Assert::IsTrue(_Status >= 0);

                UCHAR _DerivedKey[30] = {};
                _Status = ::BCryptDeriveKeyPBKDF2(hAlgorithm, (PUCHAR)"123", 3, (PUCHAR)"123456789", 9, 4, _DerivedKey, sizeof(_DerivedKey), 0);
                Assert::IsTrue(_Status >= 0);

                static const UCHAR kTargetDerivedKey[] = { 0x04, 0x11, 0xaf, 0x2e, 0x9e, 0x1e, 0xfd, 0x17, 0x7c, 0xc2, 0x06, 0x6a, 0xa0, 0x74, 0xf5, 0x79, 0x44, 0x32, 0x8b, 0x3f, 0x3e, 0xe7, 0x6a, 0xbd, 0x09, 0x02, 0xd0, 0xed, 0x9c, 0xa0 };
                Assert::AreEqual(ToHexString(_DerivedKey), ToHexString(kTargetDerivedKey));
            }
        }

        TEST_METHOD(Capi)
        {
            {
                BCRYPT_ALG_HANDLE hAlgorithm = NULL;
                auto _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_SHA1_ALGORITHM, nullptr, 0);
                Assert::IsTrue(_Status >= 0);

                BCRYPT_HASH_HANDLE _hHash = nullptr;
                _Status = ::BCryptCreateHash(hAlgorithm, &_hHash, nullptr, 0, nullptr, 0, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::IsNotNull(_hHash);

                _Status = ::BCryptHashData(_hHash, (PUCHAR)"123", 3, 0);
                Assert::IsTrue(_Status >= 0);

                UCHAR _DerivedKey[20] = {};
                _Status = ::BCryptDeriveKeyCapi(_hHash, nullptr, _DerivedKey, sizeof(_DerivedKey), 0);
                Assert::IsTrue(_Status >= 0);

                static const UCHAR kTargetDerivedKey[] = { 0x40, 0xbd, 0x00, 0x15, 0x63, 0x08, 0x5f, 0xc3, 0x51, 0x65, 0x32, 0x9e, 0xa1, 0xff, 0x5c, 0x5e, 0xcb, 0xdb, 0xbe, 0xef };
                Assert::AreEqual(ToHexString(_DerivedKey), ToHexString(kTargetDerivedKey));
            }

            {
                BCRYPT_ALG_HANDLE hAlgorithm = NULL;
                auto _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_SHA1_ALGORITHM, nullptr, 0);
                Assert::IsTrue(_Status >= 0);

                BCRYPT_HASH_HANDLE _hHash = nullptr;
                _Status = ::BCryptCreateHash(hAlgorithm, &_hHash, nullptr, 0, nullptr, 0, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::IsNotNull(_hHash);

                _Status = ::BCryptHashData(_hHash, (PUCHAR)"123", 3, 0);
                Assert::IsTrue(_Status >= 0);

                UCHAR _DerivedKey[22] = {};
                _Status = ::BCryptDeriveKeyCapi(_hHash, nullptr, _DerivedKey, sizeof(_DerivedKey), 0);
                Assert::IsTrue(_Status >= 0);

                static const UCHAR kTargetDerivedKey[22] = { 0xaa, 0x9f, 0x40, 0x19, 0x05, 0xa0, 0x89, 0x0a, 0x1d, 0xdc, 0xe2, 0xcf, 0x2c, 0x7d, 0x98, 0x58, 0x95, 0x50, 0xeb, 0xbd, 0x3d, 0x0a };
                Assert::AreEqual(ToHexString(_DerivedKey), ToHexString(kTargetDerivedKey));
            }
        }
    };

    TEST_CLASS(BCryptGetProperty)
    {
        AwaysNullGuard Guard;
    public:
        BCryptGetProperty()
        {
            Guard |= YY::Thunks::aways_null_try_get_BCryptGetProperty;
            Guard |= YY::Thunks::aways_null_try_get_BCryptOpenAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptCloseAlgorithmProvider;

            Guard |= YY::Thunks::aways_null_try_get_BCryptGenRandom;

            Guard |= YY::Thunks::aways_null_try_get_BCryptCreateHash;
            Guard |= YY::Thunks::aways_null_try_get_BCryptDestroyHash;
            Guard |= YY::Thunks::aways_null_try_get_BCryptHashData;
            Guard |= YY::Thunks::aways_null_try_get_BCryptFinishHash;

            Guard |= YY::Thunks::aways_null_try_get_BCryptGenerateSymmetricKey;
            Guard |= YY::Thunks::aways_null_try_get_BCryptDestroyKey;
        }

        TEST_METHOD(Random)
        {
            DWORD _cbGetPropertyResult;

            BCRYPT_ALG_HANDLE _hAlg = nullptr;
            int _Status = ::BCryptOpenAlgorithmProvider(&_hAlg, BCRYPT_RNG_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);
            Assert::IsNotNull(_hAlg);

            {
                wchar_t _szAlgorithmName[256] = {};
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hAlg, BCRYPT_ALGORITHM_NAME, (PUCHAR)_szAlgorithmName, sizeof(_szAlgorithmName), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::AreEqual(size_t(_cbGetPropertyResult), sizeof(BCRYPT_RNG_ALGORITHM));
                Assert::AreEqual(std::wstring(_szAlgorithmName), std::wstring(BCRYPT_RNG_ALGORITHM));
            }

            {
                BCRYPT_ALG_HANDLE _hHandle = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hAlg, BCRYPT_PROVIDER_HANDLE, (PUCHAR)&_hHandle, sizeof(_hHandle), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::AreEqual(size_t(_cbGetPropertyResult), sizeof(_hHandle));
                Assert::AreEqual((size_t)_hHandle, (size_t)_hAlg);
            }

            {
                DWORD _cbObjectLength = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&_cbObjectLength, sizeof(_cbObjectLength), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status < 0);
            }

            {
                DWORD _cbHashLength = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hAlg, BCRYPT_HASH_LENGTH, (PUCHAR)&_cbHashLength, sizeof(_cbHashLength), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status < 0);
            }

            {
                DWORD _cbBlockLength = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hAlg, BCRYPT_HASH_BLOCK_LENGTH, (PUCHAR)&_cbBlockLength, sizeof(_cbBlockLength), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status < 0);
            }
            
            _Status = ::BCryptCloseAlgorithmProvider(_hAlg, 0);
            Assert::IsTrue(_Status >= 0);
        }

        TEST_METHOD(Hash)
        {
            DWORD _cbGetPropertyResult;

            BCRYPT_ALG_HANDLE _hAlg = nullptr;
            int _Status = ::BCryptOpenAlgorithmProvider(&_hAlg, BCRYPT_SHA1_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);
            Assert::IsNotNull(_hAlg);

            {
                wchar_t _szAlgorithmName[256] = {};
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hAlg, BCRYPT_ALGORITHM_NAME, (PUCHAR)_szAlgorithmName, sizeof(_szAlgorithmName), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::AreEqual(size_t(_cbGetPropertyResult), sizeof(BCRYPT_SHA1_ALGORITHM));
                Assert::AreEqual(std::wstring(_szAlgorithmName), std::wstring(BCRYPT_SHA1_ALGORITHM));
            }

            {
                DWORD _cbObjectLength = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&_cbObjectLength, sizeof(_cbObjectLength), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::AreEqual(size_t(_cbGetPropertyResult), sizeof(_cbObjectLength));
                Assert::AreNotEqual(_cbObjectLength, 0ul);
            }

            {
                DWORD _cbHashLength = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hAlg, BCRYPT_HASH_LENGTH, (PUCHAR)&_cbHashLength, sizeof(_cbHashLength), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::AreEqual(size_t(_cbGetPropertyResult), sizeof(_cbHashLength));
                Assert::AreEqual(_cbHashLength, 20ul);
            }

            {
                BCRYPT_ALG_HANDLE _hHandle = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hAlg, BCRYPT_PROVIDER_HANDLE, (PUCHAR)&_hHandle, sizeof(_hHandle), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::AreEqual(size_t(_cbGetPropertyResult), sizeof(_hHandle));
                Assert::AreEqual((size_t)_hHandle, (size_t)_hAlg);
            }

            {
                DWORD _cbBlockLength = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hAlg, BCRYPT_HASH_BLOCK_LENGTH, (PUCHAR)&_cbBlockLength, sizeof(_cbBlockLength), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::AreEqual(size_t(_cbGetPropertyResult), sizeof(_cbBlockLength));
                Assert::AreEqual(_cbBlockLength, 64ul);
            }

            BCRYPT_HASH_HANDLE _hHash = nullptr;
            _Status = ::BCryptCreateHash(_hAlg, &_hHash, nullptr, 0, nullptr, 0, 0);
            Assert::IsTrue(_Status >= 0);
            Assert::IsNotNull(_hHash);

            {
                wchar_t _szAlgorithmName[256] = {};
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hHash, BCRYPT_ALGORITHM_NAME, (PUCHAR)_szAlgorithmName, sizeof(_szAlgorithmName), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::AreEqual(size_t(_cbGetPropertyResult), sizeof(BCRYPT_SHA1_ALGORITHM));
                Assert::AreEqual(std::wstring(_szAlgorithmName), std::wstring(BCRYPT_SHA1_ALGORITHM));
            }

            {
                DWORD _cbObjectLength = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hHash, BCRYPT_OBJECT_LENGTH, (PUCHAR)&_cbObjectLength, sizeof(_cbObjectLength), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status < 0);
            }

            {
                DWORD _cbHashLength = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hHash, BCRYPT_HASH_LENGTH, (PUCHAR)&_cbHashLength, sizeof(_cbHashLength), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::AreEqual(size_t(_cbGetPropertyResult), sizeof(_cbHashLength));
                Assert::AreEqual(_cbHashLength, 20ul);
            }

            {
                BCRYPT_ALG_HANDLE _hHandle = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hHash, BCRYPT_PROVIDER_HANDLE, (PUCHAR)&_hHandle, sizeof(_hHandle), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status >= 0);
                Assert::AreEqual(size_t(_cbGetPropertyResult), sizeof(_hHandle));
                Assert::AreEqual((size_t)_hHandle, (size_t)_hAlg);
            }

            {
                DWORD _cbBlockLength = 0;
                _cbGetPropertyResult = 0;
                _Status = ::BCryptGetProperty(_hHash, BCRYPT_HASH_BLOCK_LENGTH, (PUCHAR)&_cbBlockLength, sizeof(_cbBlockLength), &_cbGetPropertyResult, 0);
                Assert::IsTrue(_Status < 0);
            }
            _Status = ::BCryptDestroyHash(_hHash);
            Assert::IsTrue(_Status >= 0);
            _Status = ::BCryptCloseAlgorithmProvider(_hAlg, 0);
            Assert::IsTrue(_Status >= 0);
        }

        TEST_METHOD(AES加密算法相关属性)
        {
            BCRYPT_ALG_HANDLE hAlgorithm;
            long _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_AES_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);

            {
                wchar_t _szModeBuffer[256] = {};
                DWORD _cbMode = sizeof(_szModeBuffer);
                _Status = ::BCryptGetProperty(hAlgorithm, BCRYPT_CHAINING_MODE, (PBYTE)_szModeBuffer, sizeof(_szModeBuffer), &_cbMode, 0);
                Assert::IsTrue(_Status >= 0);

                Assert::AreEqual(BCRYPT_CHAIN_MODE_CBC, _szModeBuffer);
            }

            static constexpr const BYTE kAES128Key[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            BCRYPT_KEY_HANDLE hKey;
            _Status = ::BCryptGenerateSymmetricKey(
                hAlgorithm,
                &hKey,
                nullptr,
                0,
                (PBYTE)kAES128Key,
                sizeof(kAES128Key),
                0);
            Assert::IsTrue(_Status >= 0);

            {
                wchar_t _szModeBuffer[256] = {};
                DWORD _cbResult = sizeof(_szModeBuffer);
                _Status = ::BCryptGetProperty(hKey, BCRYPT_CHAINING_MODE, (PBYTE)_szModeBuffer, sizeof(_szModeBuffer), &_cbResult, 0);
                Assert::IsTrue(_Status >= 0);

                Assert::AreEqual(BCRYPT_CHAIN_MODE_CBC, _szModeBuffer);
            }

            {
                DWORD _cbBlock;
                DWORD _cbResult = sizeof(_cbBlock);
                _Status = ::BCryptGetProperty(hKey, BCRYPT_BLOCK_LENGTH, (PBYTE)&_cbBlock, sizeof(_cbBlock), &_cbResult, 0);
                Assert::IsTrue(_Status >= 0);

                Assert::AreEqual(_cbBlock, DWORD(128u / 8));
            }

            _Status = ::BCryptDestroyKey(hKey);
            Assert::IsTrue(_Status >= 0);

            _Status = ::BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            Assert::IsTrue(_Status >= 0);
        }
    };

    TEST_CLASS(BCryptSetProperty)
    {
        AwaysNullGuard Guard;
    public:
        BCryptSetProperty()
        {
            Guard |= YY::Thunks::aways_null_try_get_BCryptGetProperty;
            Guard |= YY::Thunks::aways_null_try_get_BCryptSetProperty;

            Guard |= YY::Thunks::aways_null_try_get_BCryptOpenAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptCloseAlgorithmProvider;

            Guard |= YY::Thunks::aways_null_try_get_BCryptGenRandom;

            Guard |= YY::Thunks::aways_null_try_get_BCryptCreateHash;
            Guard |= YY::Thunks::aways_null_try_get_BCryptDestroyHash;
            Guard |= YY::Thunks::aways_null_try_get_BCryptHashData;
            Guard |= YY::Thunks::aways_null_try_get_BCryptFinishHash;

            Guard |= YY::Thunks::aways_null_try_get_BCryptGenerateSymmetricKey;
            Guard |= YY::Thunks::aways_null_try_get_BCryptDestroyKey;
        }

        TEST_METHOD(AES加密算法相关属性)
        {
            BCRYPT_ALG_HANDLE hAlgorithm;
            long _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_AES_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);

            {
                _Status = ::BCryptSetProperty(hAlgorithm, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_ECB, sizeof(BCRYPT_CHAIN_MODE_ECB), 0);
                Assert::IsTrue(_Status >= 0);

                wchar_t _szModeBuffer[256] = {};
                DWORD _cbMode = sizeof(_szModeBuffer);
                _Status = ::BCryptGetProperty(hAlgorithm, BCRYPT_CHAINING_MODE, (PBYTE)_szModeBuffer, sizeof(_szModeBuffer), &_cbMode, 0);
                Assert::IsTrue(_Status >= 0);

                Assert::AreEqual(BCRYPT_CHAIN_MODE_ECB, _szModeBuffer);
            }

            static constexpr const BYTE kAES128Key[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            BCRYPT_KEY_HANDLE hKey;
            _Status = ::BCryptGenerateSymmetricKey(
                hAlgorithm,
                &hKey,
                nullptr,
                0,
                (PBYTE)kAES128Key,
                sizeof(kAES128Key),
                0);
            Assert::IsTrue(_Status >= 0);

            {
                wchar_t _szModeBuffer[256] = {};
                DWORD _cbResult = sizeof(_szModeBuffer);
                _Status = ::BCryptGetProperty(hKey, BCRYPT_CHAINING_MODE, (PBYTE)_szModeBuffer, sizeof(_szModeBuffer), &_cbResult, 0);
                Assert::IsTrue(_Status >= 0);

                Assert::AreEqual(BCRYPT_CHAIN_MODE_ECB, _szModeBuffer);

                _Status = ::BCryptSetProperty(hKey, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
                Assert::IsTrue(_Status >= 0);

                _Status = ::BCryptGetProperty(hKey, BCRYPT_CHAINING_MODE, (PBYTE)_szModeBuffer, sizeof(_szModeBuffer), &_cbResult, 0);
                Assert::IsTrue(_Status >= 0);

                Assert::AreEqual(BCRYPT_CHAIN_MODE_CBC, _szModeBuffer);
            }

            _Status = ::BCryptDestroyKey(hKey);
            Assert::IsTrue(_Status >= 0);

            _Status = ::BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            Assert::IsTrue(_Status >= 0);
        }
    };

    TEST_CLASS(BCryptEncrypt)
    {
        AwaysNullGuard Guard;

    public:
        BCryptEncrypt()
        {
            Guard |= YY::Thunks::aways_null_try_get_BCryptOpenAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptCloseAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptEncrypt;
            Guard |= YY::Thunks::aways_null_try_get_BCryptGenerateSymmetricKey;
            Guard |= YY::Thunks::aways_null_try_get_BCryptDestroyKey;
            Guard |= YY::Thunks::aways_null_try_get_BCryptSetProperty;
            Guard |= YY::Thunks::aways_null_try_get_BCryptGetProperty;
        }

        TEST_METHOD(AES加密测试)
        {
            static constexpr const BYTE kPlainText[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            BYTE rgbIV[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
            };

            static constexpr const BYTE kAES128Key[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            BCRYPT_ALG_HANDLE hAlgorithm;
            long _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_AES_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);

            _Status = ::BCryptSetProperty(hAlgorithm, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
            Assert::IsTrue(_Status >= 0);

            BCRYPT_KEY_HANDLE hKey;
            _Status = ::BCryptGenerateSymmetricKey(
                hAlgorithm,
                &hKey,
                nullptr,
                0,
                (PBYTE)kAES128Key,
                sizeof(kAES128Key),
                0);
            Assert::IsTrue(_Status >= 0);

            ULONG cbCipherText = 0;
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                (PUCHAR)rgbIV,
                sizeof(rgbIV),
                NULL,
                0,
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);
            Assert::IsTrue(_Status >= 0);

            std::vector<BYTE> vecCipherText;
            vecCipherText.resize(cbCipherText);
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                (PUCHAR)rgbIV,
                sizeof(rgbIV),
                vecCipherText.data(),
                (ULONG)vecCipherText.size(),
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);
            Assert::IsTrue(_Status >= 0);
            Assert::AreEqual(cbCipherText, (ULONG)vecCipherText.size());

            _Status = ::BCryptDestroyKey(hKey);
            Assert::IsTrue(_Status >= 0);

            static const BYTE kTargetEncryptData[] =
            { 
                0xC6, 0xA1, 0x3B, 0x37, 0x87, 0x8F, 0x5B, 0x82, 
                0x6F, 0x4F, 0x81, 0x62, 0xA1, 0xC8, 0xD8, 0x79, 
                0xB1, 0xA2, 0x92, 0x73, 0xBE, 0x2C, 0x42, 0x07, 
                0xA5, 0xAC, 0xE3, 0x93, 0x39, 0x8C, 0xB6, 0xFB, 
            };

            Assert::AreEqual(ToHexString(kTargetEncryptData), ToHexString(vecCipherText));

            _Status = ::BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            Assert::IsTrue(_Status >= 0);
        }

        TEST_METHOD(DES加密测试)
        {
            static constexpr const BYTE kPlainText[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            BYTE rgbIV[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            };

            static constexpr const BYTE kDESKey[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            };

            static const BYTE kTargetEncryptData[] =
            {
                0xA5, 0x17, 0x3A, 0xD5, 0x95, 0x7B, 0x43, 0x70,
                0x5A, 0x8C, 0xFC, 0x7E, 0xF9, 0x26, 0x6F, 0x57,
                0xD0, 0x69, 0x16, 0x84, 0x0B, 0x48, 0x4C, 0x2A,
            };

            BCRYPT_ALG_HANDLE hAlgorithm;
            long _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_DES_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);

            _Status = ::BCryptSetProperty(hAlgorithm, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
            Assert::IsTrue(_Status >= 0);

            BCRYPT_KEY_HANDLE hKey;
            _Status = ::BCryptGenerateSymmetricKey(
                hAlgorithm,
                &hKey,
                nullptr,
                0,
                (PBYTE)kDESKey,
                sizeof(kDESKey),
                0);
            Assert::IsTrue(_Status >= 0);

            ULONG cbCipherText = 0;
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                (PUCHAR)rgbIV,
                sizeof(rgbIV),
                NULL,
                0,
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);
            Assert::IsTrue(_Status >= 0);

            std::vector<BYTE> vecCipherText;
            vecCipherText.resize(cbCipherText);
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                (PUCHAR)rgbIV,
                sizeof(rgbIV),
                vecCipherText.data(),
                (ULONG)vecCipherText.size(),
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);

            Assert::IsTrue(_Status >= 0);
            vecCipherText.resize(cbCipherText);

            _Status = ::BCryptDestroyKey(hKey);
            Assert::IsTrue(_Status >= 0);

            Assert::AreEqual(ToHexString(kTargetEncryptData), ToHexString(vecCipherText));

            _Status = ::BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            Assert::IsTrue(_Status >= 0);
        }

        TEST_METHOD(RC2加密测试)
        {
            static constexpr const BYTE kPlainText[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            BYTE rgbIV[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            };

            static constexpr const BYTE kSecretKey[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            };

            static const BYTE kTargetEncryptData[] =
            {
                0xD3, 0x3B, 0x1D, 0x1C, 0x25, 0xA6, 0xED, 0x7A,
                0x4F, 0xF3, 0x01, 0xC4, 0x72, 0xB1, 0x6F, 0xA4,
                0x5A, 0xDA, 0x0B, 0xFE, 0x38, 0x33, 0x46, 0xC1,
            };

            BCRYPT_ALG_HANDLE hAlgorithm;
            long _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_RC2_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);

            _Status = ::BCryptSetProperty(hAlgorithm, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
            Assert::IsTrue(_Status >= 0);

            BCRYPT_KEY_HANDLE hKey;
            _Status = ::BCryptGenerateSymmetricKey(
                hAlgorithm,
                &hKey,
                nullptr,
                0,
                (PBYTE)kSecretKey,
                sizeof(kSecretKey),
                0);
            Assert::IsTrue(_Status >= 0);

            ULONG cbCipherText = 0;
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                (PUCHAR)rgbIV,
                sizeof(rgbIV),
                NULL,
                0,
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);
            Assert::IsTrue(_Status >= 0);

            std::vector<BYTE> vecCipherText;
            vecCipherText.resize(cbCipherText);
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                (PUCHAR)rgbIV,
                sizeof(rgbIV),
                vecCipherText.data(),
                (ULONG)vecCipherText.size(),
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);

            Assert::IsTrue(_Status >= 0);
            vecCipherText.resize(cbCipherText);

            _Status = ::BCryptDestroyKey(hKey);
            Assert::IsTrue(_Status >= 0);

            Assert::AreEqual(ToHexString(kTargetEncryptData), ToHexString(vecCipherText));

            _Status = ::BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            Assert::IsTrue(_Status >= 0);
        }

        TEST_METHOD(RC4加密测试)
        {
            static constexpr const BYTE kPlainText[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            static constexpr const BYTE kSecretKey[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            };

            static const BYTE kTargetEncryptData[] =
            {
                0xD5, 0xD1, 0xC6, 0xCE, 0xD5, 0xB3, 0xE6, 0x4D,
                0xD1, 0x50, 0xCC, 0x68, 0x39, 0x59, 0xFD, 0xDF,
            };

            BCRYPT_ALG_HANDLE hAlgorithm;
            long _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_RC4_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);

            BCRYPT_KEY_HANDLE hKey;
            _Status = ::BCryptGenerateSymmetricKey(
                hAlgorithm,
                &hKey,
                nullptr,
                0,
                (PBYTE)kSecretKey,
                sizeof(kSecretKey),
                0);
            Assert::IsTrue(_Status >= 0);

            ULONG cbCipherText = 0;
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                nullptr,
                0,
                NULL,
                0,
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);
            Assert::IsTrue(_Status >= 0);

            std::vector<BYTE> vecCipherText;
            vecCipherText.resize(cbCipherText);
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                nullptr,
                0,
                vecCipherText.data(),
                (ULONG)vecCipherText.size(),
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);

            Assert::IsTrue(_Status >= 0);
            vecCipherText.resize(cbCipherText);

            _Status = ::BCryptDestroyKey(hKey);
            Assert::IsTrue(_Status >= 0);

            Assert::AreEqual(ToHexString(kTargetEncryptData), ToHexString(vecCipherText));

            _Status = ::BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            Assert::IsTrue(_Status >= 0);
        }

        TEST_METHOD(_3DES加密测试)
        {
            static constexpr const BYTE kPlainText[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            static constexpr const BYTE kSecretKey[192/8] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            };

            static const BYTE kTargetEncryptData[] =
            {
                0xE1, 0xB2, 0x46, 0xE5, 0xA7, 0xC7, 0x4C, 0xBC,
                0x27, 0x3B, 0xF0, 0xB4, 0x0E, 0xDB, 0xF8, 0xF5,
                0xCA, 0xDD, 0xCA, 0x87, 0x87, 0x53, 0x6A, 0xD4,
            };

            BCRYPT_ALG_HANDLE hAlgorithm;
            long _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_3DES_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);

            /*{
                wchar_t _cbHashLength[100];
                DWORD _cbResult = 0;
                _Status = ::BCryptGetProperty(hAlgorithm, BCRYPT_CHAINING_MODE, (PUCHAR)&_cbHashLength, sizeof(_cbHashLength), &_cbResult, 0);
                Assert::IsTrue(_Status >= 0);
            }*/

            BCRYPT_KEY_HANDLE hKey;
            _Status = ::BCryptGenerateSymmetricKey(
                hAlgorithm,
                &hKey,
                nullptr,
                0,
                (PBYTE)kSecretKey,
                sizeof(kSecretKey),
                0);
            Assert::IsTrue(_Status >= 0);

            ULONG cbCipherText = 0;
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                nullptr,
                0,
                NULL,
                0,
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);
            Assert::IsTrue(_Status >= 0);

            std::vector<BYTE> vecCipherText;
            vecCipherText.resize(cbCipherText);
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                nullptr,
                0,
                vecCipherText.data(),
                (ULONG)vecCipherText.size(),
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);

            Assert::IsTrue(_Status >= 0);
            vecCipherText.resize(cbCipherText);

            _Status = ::BCryptDestroyKey(hKey);
            Assert::IsTrue(_Status >= 0);

            Assert::AreEqual(ToHexString(kTargetEncryptData), ToHexString(vecCipherText));

            _Status = ::BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            Assert::IsTrue(_Status >= 0);
        }
        
        TEST_METHOD(_3DES_112加密测试)
        {
            static constexpr const BYTE kPlainText[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            static constexpr const BYTE kSecretKey[128/8] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            };

            static const BYTE kTargetEncryptData[] =
            {
                0x33, 0xB2, 0xB0, 0x52, 0x19, 0x73, 0xD0, 0xAC,
                0x13, 0xF5, 0x42, 0x2E, 0x13, 0xC9, 0xE0, 0x68,
                0x04, 0x5A, 0xB4, 0x82, 0x82, 0x24, 0x6B, 0x06,
            };

            BCRYPT_ALG_HANDLE hAlgorithm;
            long _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_3DES_112_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);

            /*{
                DWORD _cbHashLength[100];
                DWORD _cbResult = 0;
                _Status = ::BCryptGetProperty(hAlgorithm, BCRYPT_BLOCK_SIZE_LIST, (PUCHAR)&_cbHashLength, sizeof(_cbHashLength), &_cbResult, 0);
                Assert::IsTrue(_Status >= 0);
            }*/

            BCRYPT_KEY_HANDLE hKey;
            _Status = ::BCryptGenerateSymmetricKey(
                hAlgorithm,
                &hKey,
                nullptr,
                0,
                (PBYTE)kSecretKey,
                sizeof(kSecretKey),
                0);
            Assert::IsTrue(_Status >= 0);

            ULONG cbCipherText = 0;
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                nullptr,
                0,
                NULL,
                0,
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);
            Assert::IsTrue(_Status >= 0);

            std::vector<BYTE> vecCipherText;
            vecCipherText.resize(cbCipherText);
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                nullptr,
                0,
                vecCipherText.data(),
                (ULONG)vecCipherText.size(),
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);

            Assert::IsTrue(_Status >= 0);
            vecCipherText.resize(cbCipherText);

            _Status = ::BCryptDestroyKey(hKey);
            Assert::IsTrue(_Status >= 0);

            Assert::AreEqual(ToHexString(kTargetEncryptData), ToHexString(vecCipherText));

            _Status = ::BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            Assert::IsTrue(_Status >= 0);
        }
    };

    TEST_CLASS(BCryptDecrypt)
    {
        AwaysNullGuard Guard;

    public:
        BCryptDecrypt()
        {
            Guard |= YY::Thunks::aways_null_try_get_BCryptOpenAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptCloseAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptDecrypt;
            Guard |= YY::Thunks::aways_null_try_get_BCryptGenerateSymmetricKey;
            Guard |= YY::Thunks::aways_null_try_get_BCryptDestroyKey;
            Guard |= YY::Thunks::aways_null_try_get_BCryptSetProperty;
        }

        TEST_METHOD(AES解密测试)
        {
            static constexpr const BYTE kPlainText[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            BYTE rgbIV[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
            };

            static constexpr const BYTE kAES128Key[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            static const BYTE kEncryptData[] =
            {
                0xC6, 0xA1, 0x3B, 0x37, 0x87, 0x8F, 0x5B, 0x82,
                0x6F, 0x4F, 0x81, 0x62, 0xA1, 0xC8, 0xD8, 0x79,
                0xB1, 0xA2, 0x92, 0x73, 0xBE, 0x2C, 0x42, 0x07,
                0xA5, 0xAC, 0xE3, 0x93, 0x39, 0x8C, 0xB6, 0xFB,
            };

            BCRYPT_ALG_HANDLE hAlgorithm;
            long _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_AES_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);

            _Status = ::BCryptSetProperty(hAlgorithm, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
            Assert::IsTrue(_Status >= 0);

            BCRYPT_KEY_HANDLE hKey;
            _Status = ::BCryptGenerateSymmetricKey(
                hAlgorithm,
                &hKey,
                nullptr,
                0,
                (PBYTE)kAES128Key,
                sizeof(kAES128Key),
                0);
            Assert::IsTrue(_Status >= 0);

            ULONG cbPlainText = 0;
            _Status = ::BCryptDecrypt(
                hKey,
                (PUCHAR)kEncryptData,
                sizeof(kEncryptData),
                NULL,
                (PUCHAR)rgbIV,
                sizeof(rgbIV),
                NULL,
                0,
                &cbPlainText,
                BCRYPT_BLOCK_PADDING);
            Assert::IsTrue(_Status >= 0);

            std::vector<BYTE> vecPlainText;
            vecPlainText.resize(cbPlainText);
            _Status = ::BCryptDecrypt(
                hKey,
                (PUCHAR)kEncryptData,
                sizeof(kEncryptData),
                NULL,
                (PUCHAR)rgbIV,
                sizeof(rgbIV),
                vecPlainText.data(),
                (ULONG)vecPlainText.size(),
                &cbPlainText,
                BCRYPT_BLOCK_PADDING);
            Assert::IsTrue(_Status >= 0);
            vecPlainText.resize(cbPlainText);

            _Status = ::BCryptDestroyKey(hKey);
            Assert::IsTrue(_Status >= 0);

            Assert::AreEqual(cbPlainText, (ULONG)sizeof(kPlainText));

            Assert::AreEqual(ToHexString(kPlainText), ToHexString(vecPlainText));

            _Status = ::BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            Assert::IsTrue(_Status >= 0);
        }
    };

    TEST_CLASS(BCryptExportKey)
    {
        AwaysNullGuard Guard;

    public:
        BCryptExportKey()
        {
            Guard |= YY::Thunks::aways_null_try_get_BCryptOpenAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptCloseAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptExportKey;
            Guard |= YY::Thunks::aways_null_try_get_BCryptGenerateSymmetricKey;
        }

        TEST_METHOD(AES密钥导出)
        {
            static constexpr const BYTE kSecret[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            static constexpr const BYTE kKeyDataBlob[] =
            {
                0x4B, 0x44, 0x42, 0x4D,
                0x01, 0x00, 0x00, 0x00,
                0x10, 0x00, 0x00, 0x00,
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            BCRYPT_ALG_HANDLE hAlgorithm;
            long _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_AES_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);

            BCRYPT_KEY_HANDLE hKey;
            _Status = ::BCryptGenerateSymmetricKey(
                hAlgorithm,
                &hKey,
                nullptr,
                0,
                (PBYTE)kSecret,
                sizeof(kSecret),
                0);
            Assert::IsTrue(_Status >= 0);
            
            ULONG _cbResult;
            _Status = ::BCryptExportKey(hKey, NULL, BCRYPT_KEY_DATA_BLOB, nullptr, 0, &_cbResult, 0);
            Assert::IsTrue(_Status >= 0);

            std::vector<BYTE> _KeyDataBlobBuffer(_cbResult);
            _Status = ::BCryptExportKey(hKey, NULL, BCRYPT_KEY_DATA_BLOB, _KeyDataBlobBuffer.data(), (ULONG)_KeyDataBlobBuffer.size(), &_cbResult, 0);
            Assert::IsTrue(_Status >= 0);
            _KeyDataBlobBuffer.resize(_cbResult);
            _Status = ::BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            Assert::IsTrue(_Status >= 0);

            Assert::AreEqual(ToHexString(_KeyDataBlobBuffer), ToHexString(kKeyDataBlob));
        }
    };

    TEST_CLASS(BCryptImportKey)
    {
        AwaysNullGuard Guard;

    public:
        BCryptImportKey()
        {
            Guard |= YY::Thunks::aways_null_try_get_BCryptOpenAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptCloseAlgorithmProvider;
            Guard |= YY::Thunks::aways_null_try_get_BCryptExportKey;
            Guard |= YY::Thunks::aways_null_try_get_BCryptImportKey;
            Guard |= YY::Thunks::aways_null_try_get_BCryptGenerateSymmetricKey;
            Guard |= YY::Thunks::aways_null_try_get_BCryptSetProperty;
            Guard |= YY::Thunks::aways_null_try_get_BCryptEncrypt;
            Guard |= YY::Thunks::aways_null_try_get_BCryptDestroyKey;
        }

        TEST_METHOD(AES密钥导入)
        {
            static constexpr const BYTE kPlainText[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            static constexpr const BYTE kKeyDataBlob[] =
            {
                0x4B, 0x44, 0x42, 0x4D,
                0x01, 0x00, 0x00, 0x00,
                0x10, 0x00, 0x00, 0x00,
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            };

            BYTE rgbIV[] =
            {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
            };

            // 生成的密文如果符合预期那么说明密钥导入成功。
            static const BYTE kTargetEncryptData[] =
            {
                0xC6, 0xA1, 0x3B, 0x37, 0x87, 0x8F, 0x5B, 0x82,
                0x6F, 0x4F, 0x81, 0x62, 0xA1, 0xC8, 0xD8, 0x79,
                0xB1, 0xA2, 0x92, 0x73, 0xBE, 0x2C, 0x42, 0x07,
                0xA5, 0xAC, 0xE3, 0x93, 0x39, 0x8C, 0xB6, 0xFB,
            };

            BCRYPT_ALG_HANDLE hAlgorithm;
            long _Status = ::BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_AES_ALGORITHM, nullptr, 0);
            Assert::IsTrue(_Status >= 0);

            BCRYPT_KEY_HANDLE hKey;
            _Status = ::BCryptImportKey(
                hAlgorithm,
                NULL,
                BCRYPT_KEY_DATA_BLOB,
                &hKey,
                nullptr,
                0,
                (PBYTE)kKeyDataBlob,
                sizeof(kKeyDataBlob),
                0);
            Assert::IsTrue(_Status >= 0);
            _Status = ::BCryptSetProperty(hAlgorithm, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
            Assert::IsTrue(_Status >= 0);

            ULONG cbCipherText = 0;
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                (PUCHAR)rgbIV,
                sizeof(rgbIV),
                NULL,
                0,
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);
            Assert::IsTrue(_Status >= 0);

            std::vector<BYTE> vecCipherText;
            vecCipherText.resize(cbCipherText);
            _Status = ::BCryptEncrypt(
                hKey,
                (PUCHAR)kPlainText,
                sizeof(kPlainText),
                NULL,
                (PUCHAR)rgbIV,
                sizeof(rgbIV),
                vecCipherText.data(),
                (ULONG)vecCipherText.size(),
                &cbCipherText,
                BCRYPT_BLOCK_PADDING);
            Assert::IsTrue(_Status >= 0);

            _Status = ::BCryptDestroyKey(hKey);
            Assert::IsTrue(_Status >= 0);

            _Status = ::BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            Assert::IsTrue(_Status >= 0);

            Assert::AreEqual(ToHexString(vecCipherText), ToHexString(kTargetEncryptData));
        }
    };
}
