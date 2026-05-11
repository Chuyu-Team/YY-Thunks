#include "pch.h"
#include "Thunks/ncrypy.hpp"

namespace ncrypy
{
    static void CleanupKeyContainer(_In_opt_z_ LPCWSTR _szKeyName)
    {
        if (_szKeyName == nullptr || *_szKeyName == L'\0')
            return;

        HCRYPTPROV _hProv = 0;
        ::CryptAcquireContextW(&_hProv, _szKeyName, nullptr, PROV_RSA_AES, CRYPT_DELETEKEYSET);
        ::CryptAcquireContextW(&_hProv, _szKeyName, nullptr, PROV_RSA_FULL, CRYPT_DELETEKEYSET);
    }

    static void BuildCaseKeyName(_In_z_ LPCWSTR _szPrefix, _Out_writes_(64) wchar_t _szKeyName[64])
    {
        wsprintfW(_szKeyName, L"YY_Thunks_ncrypy_%s_%08X", _szPrefix, GetTickCount());
    }

    TEST_CLASS(NCryptOpenStorageProvider)
    {
        AwaysNullGuard Guard;

    public:
        NCryptOpenStorageProvider()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptOpenStorageProvider;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFreeObject;
        }

        TEST_METHOD(基础打开与关闭)
        {
            NCRYPT_PROV_HANDLE _hProvider = 0;
            auto _Status = ::NCryptOpenStorageProvider(&_hProvider, nullptr, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            Assert::IsTrue(_hProvider != 0);

            _Status = ::NCryptFreeObject(_hProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
        }

        TEST_METHOD(参数校验)
        {
            auto _Status = ::NCryptOpenStorageProvider(nullptr, nullptr, 0);
            Assert::AreEqual(SECURITY_STATUS(NTE_INVALID_PARAMETER), _Status);
        }

        TEST_METHOD(智能卡与平台Provider映射)
        {
            NCRYPT_PROV_HANDLE _hSmartCardProvider = 0;
            auto _Status = ::NCryptOpenStorageProvider(&_hSmartCardProvider, MS_SMART_CARD_KEY_STORAGE_PROVIDER, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            Assert::IsTrue(_hSmartCardProvider != 0);
            _Status = ::NCryptFreeObject(_hSmartCardProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_PROV_HANDLE _hPlatformProvider = 0;
            _Status = ::NCryptOpenStorageProvider(&_hPlatformProvider, MS_PLATFORM_CRYPTO_PROVIDER, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            Assert::IsTrue(_hPlatformProvider != 0);
            _Status = ::NCryptFreeObject(_hPlatformProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_PROV_HANDLE _hUnknownProvider = 0;
            _Status = ::NCryptOpenStorageProvider(&_hUnknownProvider, L"YY.Unknown.KSP", 0);
            Assert::AreEqual(SECURITY_STATUS(NTE_BAD_PROVIDER), _Status);
        }
    };

    TEST_CLASS(NCryptImportKey)
    {
        AwaysNullGuard Guard;

    public:
        NCryptImportKey()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptOpenStorageProvider;
            Guard |= YY::Thunks::aways_null_try_get_NCryptCreatePersistedKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFinalizeKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptExportKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptImportKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptDeleteKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFreeObject;
        }

        TEST_METHOD(导出后再导入)
        {
            NCRYPT_PROV_HANDLE _hProvider = 0;
            auto _Status = ::NCryptOpenStorageProvider(&_hProvider, nullptr, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_KEY_HANDLE _hSourceKey = 0;
            _Status = ::NCryptCreatePersistedKey(_hProvider, &_hSourceKey, BCRYPT_RSA_ALGORITHM, nullptr, 0, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            _Status = ::NCryptFinalizeKey(_hSourceKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            DWORD _cbPrivateBlob = 0;
            _Status = ::NCryptExportKey(_hSourceKey, 0, LEGACY_RSAPRIVATE_BLOB, nullptr, nullptr, 0, &_cbPrivateBlob, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            Assert::IsTrue(_cbPrivateBlob > 0);

            std::vector<BYTE> _PrivateBlob(_cbPrivateBlob);
            _Status = ::NCryptExportKey(_hSourceKey, 0, LEGACY_RSAPRIVATE_BLOB, nullptr, _PrivateBlob.data(), (DWORD)_PrivateBlob.size(), &_cbPrivateBlob, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_KEY_HANDLE _hImportedKey = 0;
            _Status = ::NCryptImportKey(_hProvider, 0, LEGACY_RSAPRIVATE_BLOB, nullptr, &_hImportedKey, _PrivateBlob.data(), (DWORD)_PrivateBlob.size(), 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            Assert::IsTrue(_hImportedKey != 0);

            DWORD _cbPublicBlob = 0;
            _Status = ::NCryptExportKey(_hImportedKey, 0, BCRYPT_RSAPUBLIC_BLOB, nullptr, nullptr, 0, &_cbPublicBlob, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            Assert::IsTrue(_cbPublicBlob > 0);

            _Status = ::NCryptDeleteKey(_hImportedKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            _Status = ::NCryptDeleteKey(_hSourceKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            _Status = ::NCryptFreeObject(_hProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
        }
    };

    TEST_CLASS(NCryptEncrypt_NCryptDecrypt)
    {
        AwaysNullGuard Guard;

    public:
        NCryptEncrypt_NCryptDecrypt()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptOpenStorageProvider;
            Guard |= YY::Thunks::aways_null_try_get_NCryptCreatePersistedKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFinalizeKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptEncrypt;
            Guard |= YY::Thunks::aways_null_try_get_NCryptDecrypt;
            Guard |= YY::Thunks::aways_null_try_get_NCryptDeleteKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFreeObject;
        }

        TEST_METHOD(RSA加解密)
        {
            NCRYPT_PROV_HANDLE _hProvider = 0;
            auto _Status = ::NCryptOpenStorageProvider(&_hProvider, nullptr, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_KEY_HANDLE _hKey = 0;
            _Status = ::NCryptCreatePersistedKey(_hProvider, &_hKey, BCRYPT_RSA_ALGORITHM, nullptr, 0, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            _Status = ::NCryptFinalizeKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            static const BYTE kPlain[] = { 1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6 };
            DWORD _cbCipher = 0;
            _Status = ::NCryptEncrypt(_hKey, const_cast<PBYTE>(kPlain), sizeof(kPlain), nullptr, nullptr, 0, &_cbCipher, NCRYPT_PAD_PKCS1_FLAG);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            std::vector<BYTE> _Cipher(_cbCipher);
            _Status = ::NCryptEncrypt(_hKey, const_cast<PBYTE>(kPlain), sizeof(kPlain), nullptr, _Cipher.data(), (DWORD)_cbCipher, &_cbCipher, NCRYPT_PAD_PKCS1_FLAG);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            _Cipher.resize(_cbCipher);

            {
                DWORD _cbPlain = 0;
                _Status = ::NCryptDecrypt(_hKey, _Cipher.data(), (DWORD)_Cipher.size(), nullptr, nullptr, 0, &_cbPlain, NCRYPT_PAD_PKCS1_FLAG);
                Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

                std::vector<BYTE> _Plain(_cbPlain);
                _Status = ::NCryptDecrypt(_hKey, _Cipher.data(), (DWORD)_Cipher.size(), nullptr, _Plain.data(), (DWORD)_Plain.size(), &_cbPlain, NCRYPT_PAD_PKCS1_FLAG);
                Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
                _Plain.resize(_cbPlain);
                Assert::IsTrue(memcmp(_Plain.data(), kPlain, sizeof(kPlain)) == 0);
            }


            _Status = ::NCryptDeleteKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            _Status = ::NCryptFreeObject(_hProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
        }
    };

    TEST_CLASS(NCryptCreatePersistedKey)
    {
        AwaysNullGuard Guard;

    public:
        NCryptCreatePersistedKey()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptOpenStorageProvider;
            Guard |= YY::Thunks::aways_null_try_get_NCryptCreatePersistedKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFreeObject;
        }

        TEST_METHOD(创建持久化密钥)
        {
            wchar_t _szKeyName[64] = {};
            BuildCaseKeyName(L"CreatePersistedKey", _szKeyName);
            CleanupKeyContainer(_szKeyName);

            NCRYPT_PROV_HANDLE _hProvider = 0;
            auto _Status = ::NCryptOpenStorageProvider(&_hProvider, nullptr, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_KEY_HANDLE _hKey = 0;
            _Status = ::NCryptCreatePersistedKey(_hProvider, &_hKey, BCRYPT_RSA_ALGORITHM, _szKeyName, 0, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptDeleteKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptFreeObject(_hProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
        }
    };

    TEST_CLASS(NCryptSetProperty)
    {
        AwaysNullGuard Guard;

    public:
        NCryptSetProperty()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptOpenStorageProvider;
            Guard |= YY::Thunks::aways_null_try_get_NCryptCreatePersistedKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptSetProperty;
            Guard |= YY::Thunks::aways_null_try_get_NCryptDeleteKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFreeObject;
        }

        TEST_METHOD(设置密钥长度)
        {
            NCRYPT_PROV_HANDLE _hProvider = 0;
            auto _Status = ::NCryptOpenStorageProvider(&_hProvider, nullptr, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_KEY_HANDLE _hKey = 0;
            _Status = ::NCryptCreatePersistedKey(_hProvider, &_hKey, BCRYPT_RSA_ALGORITHM, nullptr, 0, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            DWORD _cbKey = 1024;
            _Status = ::NCryptSetProperty(_hKey, NCRYPT_LENGTH_PROPERTY, reinterpret_cast<PBYTE>(&_cbKey), sizeof(_cbKey), 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptDeleteKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptFreeObject(_hProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
        }
    };

    TEST_CLASS(NCryptFinalizeKey)
    {
        AwaysNullGuard Guard;

    public:
        NCryptFinalizeKey()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptOpenStorageProvider;
            Guard |= YY::Thunks::aways_null_try_get_NCryptCreatePersistedKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFinalizeKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptDeleteKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFreeObject;
        }

        TEST_METHOD(完成密钥生成)
        {
            NCRYPT_PROV_HANDLE _hProvider = 0;
            auto _Status = ::NCryptOpenStorageProvider(&_hProvider, nullptr, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_KEY_HANDLE _hKey = 0;
            _Status = ::NCryptCreatePersistedKey(_hProvider, &_hKey, BCRYPT_RSA_ALGORITHM, nullptr, 0, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptFinalizeKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptDeleteKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptFreeObject(_hProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
        }
    };

    TEST_CLASS(NCryptExportKey)
    {
        AwaysNullGuard Guard;

    public:
        NCryptExportKey()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptOpenStorageProvider;
            Guard |= YY::Thunks::aways_null_try_get_NCryptCreatePersistedKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFinalizeKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptExportKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptDeleteKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFreeObject;
        }

        TEST_METHOD(导出多种Blob)
        {
            NCRYPT_PROV_HANDLE _hProvider = 0;
            auto _Status = ::NCryptOpenStorageProvider(&_hProvider, nullptr, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_KEY_HANDLE _hKey = 0;
            _Status = ::NCryptCreatePersistedKey(_hProvider, &_hKey, BCRYPT_RSA_ALGORITHM, nullptr, 0, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            _Status = ::NCryptFinalizeKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            const LPCWSTR _szBlobTypes[] =
            {
                BCRYPT_RSAPUBLIC_BLOB,
                BCRYPT_PUBLIC_KEY_BLOB,
                LEGACY_RSAPUBLIC_BLOB,
                BCRYPT_RSAPRIVATE_BLOB,
                BCRYPT_PRIVATE_KEY_BLOB,
                LEGACY_RSAPRIVATE_BLOB,
            };

            for (auto _szBlobType : _szBlobTypes)
            {
                DWORD _cbBlob = 0;
                _Status = ::NCryptExportKey(_hKey, 0, _szBlobType, nullptr, nullptr, 0, &_cbBlob, 0);
                Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
                Assert::IsTrue(_cbBlob > 0);

                std::vector<BYTE> _Blob(_cbBlob);

                auto _cbActual = _cbBlob;
                _Status = ::NCryptExportKey(_hKey, 0, _szBlobType, nullptr, _Blob.data(), (DWORD)_Blob.size(), &_cbActual, 0);
                Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
                Assert::IsTrue(_cbActual > 0);
            }

            DWORD _cbUnsupported = 0;
            _Status = ::NCryptExportKey(_hKey, 0, BCRYPT_KEY_DATA_BLOB, nullptr, nullptr, 0, &_cbUnsupported, 0);
            Assert::AreEqual(SECURITY_STATUS(NTE_BAD_TYPE), _Status);

            _Status = ::NCryptDeleteKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptFreeObject(_hProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
        }
    };

    TEST_CLASS(NCryptSignHash)
    {
        AwaysNullGuard Guard;

    public:
        NCryptSignHash()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptOpenStorageProvider;
            Guard |= YY::Thunks::aways_null_try_get_NCryptCreatePersistedKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFinalizeKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptSignHash;
            Guard |= YY::Thunks::aways_null_try_get_NCryptDeleteKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFreeObject;
        }

        TEST_METHOD(签名)
        {
            NCRYPT_PROV_HANDLE _hProvider = 0;
            auto _Status = ::NCryptOpenStorageProvider(&_hProvider, nullptr, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_KEY_HANDLE _hKey = 0;
            _Status = ::NCryptCreatePersistedKey(_hProvider, &_hKey, BCRYPT_RSA_ALGORITHM, nullptr, 0, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            _Status = ::NCryptFinalizeKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            BCRYPT_PKCS1_PADDING_INFO _PaddingInfo = {};
            _PaddingInfo.pszAlgId = BCRYPT_SHA256_ALGORITHM;

            static const BYTE kHash[32] =
            {
                0xA6, 0x65, 0xA4, 0x59, 0x20, 0x42, 0x2F, 0x9D,
                0x41, 0x7E, 0x48, 0x67, 0xEF, 0xDC, 0x4F, 0xB8,
                0xA0, 0x4A, 0x1F, 0x3F, 0xFF, 0x1F, 0xA0, 0x7E,
                0x99, 0x8E, 0x86, 0xF7, 0xF7, 0xA2, 0x7A, 0xE3,
            };

            DWORD _cbSignature = 0;
            _Status = ::NCryptSignHash(_hKey, &_PaddingInfo, const_cast<PBYTE>(kHash), sizeof(kHash), nullptr, 0, &_cbSignature, BCRYPT_PAD_PKCS1);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptDeleteKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptFreeObject(_hProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
        }
    };

    TEST_CLASS(NCryptVerifySignature)
    {
        AwaysNullGuard Guard;

    public:
        NCryptVerifySignature()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptOpenStorageProvider;
            Guard |= YY::Thunks::aways_null_try_get_NCryptCreatePersistedKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFinalizeKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptSignHash;
            Guard |= YY::Thunks::aways_null_try_get_NCryptVerifySignature;
            Guard |= YY::Thunks::aways_null_try_get_NCryptDeleteKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFreeObject;
        }

        TEST_METHOD(验签)
        {
            NCRYPT_PROV_HANDLE _hProvider = 0;
            auto _Status = ::NCryptOpenStorageProvider(&_hProvider, nullptr, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_KEY_HANDLE _hKey = 0;
            _Status = ::NCryptCreatePersistedKey(_hProvider, &_hKey, BCRYPT_RSA_ALGORITHM, nullptr, 0, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            _Status = ::NCryptFinalizeKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            BCRYPT_PKCS1_PADDING_INFO _PaddingInfo = {};
            _PaddingInfo.pszAlgId = BCRYPT_SHA256_ALGORITHM;

            static const BYTE kHash[32] =
            {
                0xA6, 0x65, 0xA4, 0x59, 0x20, 0x42, 0x2F, 0x9D,
                0x41, 0x7E, 0x48, 0x67, 0xEF, 0xDC, 0x4F, 0xB8,
                0xA0, 0x4A, 0x1F, 0x3F, 0xFF, 0x1F, 0xA0, 0x7E,
                0x99, 0x8E, 0x86, 0xF7, 0xF7, 0xA2, 0x7A, 0xE3,
            };

            DWORD _cbSignature = 0;
            _Status = ::NCryptSignHash(_hKey, &_PaddingInfo, const_cast<PBYTE>(kHash), sizeof(kHash), nullptr, 0, &_cbSignature, BCRYPT_PAD_PKCS1);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            std::vector<BYTE> _Signature(_cbSignature);
            _Status = ::NCryptSignHash(_hKey, &_PaddingInfo, const_cast<PBYTE>(kHash), sizeof(kHash), _Signature.data(), (DWORD)_Signature.size(), &_cbSignature, BCRYPT_PAD_PKCS1);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptVerifySignature(_hKey, &_PaddingInfo, const_cast<PBYTE>(kHash), sizeof(kHash), _Signature.data(), _cbSignature, BCRYPT_PAD_PKCS1);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptDeleteKey(_hKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptFreeObject(_hProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
        }
    };

    TEST_CLASS(NCryptOpenKey)
    {
        AwaysNullGuard Guard;

    public:
        NCryptOpenKey()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptOpenStorageProvider;
            Guard |= YY::Thunks::aways_null_try_get_NCryptCreatePersistedKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFinalizeKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptOpenKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptDeleteKey;
            Guard |= YY::Thunks::aways_null_try_get_NCryptFreeObject;
        }

        TEST_METHOD(打开持久化密钥)
        {
            wchar_t _szKeyName[64] = {};
            BuildCaseKeyName(L"OpenKey", _szKeyName);
            CleanupKeyContainer(_szKeyName);

            NCRYPT_PROV_HANDLE _hProvider = 0;
            auto _Status = ::NCryptOpenStorageProvider(&_hProvider, nullptr, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_KEY_HANDLE _hCreatedKey = 0;
            _Status = ::NCryptCreatePersistedKey(_hProvider, &_hCreatedKey, BCRYPT_RSA_ALGORITHM, _szKeyName, 0, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            _Status = ::NCryptFinalizeKey(_hCreatedKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            _Status = ::NCryptFreeObject(_hCreatedKey);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            NCRYPT_KEY_HANDLE _hOpenedKey = 0;
            _Status = ::NCryptOpenKey(_hProvider, &_hOpenedKey, _szKeyName, 0, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptDeleteKey(_hOpenedKey, 0);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            _Status = ::NCryptFreeObject(_hProvider);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
        }
    };

    TEST_CLASS(NCryptProtectionDescriptor)
    {
        AwaysNullGuard Guard;

    public:
        NCryptProtectionDescriptor()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptCreateProtectionDescriptor;
            Guard |= YY::Thunks::aways_null_try_get_NCryptCloseProtectionDescriptor;
            Guard |= YY::Thunks::aways_null_try_get_NCryptGetProtectionDescriptorInfo;
        }

        TEST_METHOD(创建查询并关闭描述符)
        {
            NCRYPT_DESCRIPTOR_HANDLE _hDescriptor = 0;
            auto _Status = ::NCryptCreateProtectionDescriptor(L"LOCAL=user", 0, &_hDescriptor);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            void* _pInfo = nullptr;
            _Status = ::NCryptGetProtectionDescriptorInfo(_hDescriptor, nullptr, NCRYPT_PROTECTION_INFO_TYPE_DESCRIPTOR_STRING, &_pInfo);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            Assert::IsNotNull(_pInfo);

            Assert::AreEqual(0, wcscmp(L"LOCAL=user", static_cast<LPCWSTR>(_pInfo)));

            if (_pInfo)
                ::LocalFree(_pInfo);

            ::NCryptCloseProtectionDescriptor(_hDescriptor);
        }
    };

    TEST_CLASS(NCryptProtectUnprotectSecret)
    {
        AwaysNullGuard Guard;

    public:
        NCryptProtectUnprotectSecret()
        {
            Guard |= YY::Thunks::aways_null_try_get_NCryptCreateProtectionDescriptor;
            Guard |= YY::Thunks::aways_null_try_get_NCryptCloseProtectionDescriptor;
            Guard |= YY::Thunks::aways_null_try_get_NCryptProtectSecret;
            Guard |= YY::Thunks::aways_null_try_get_NCryptUnprotectSecret;
        }

        TEST_METHOD(加解密Secret)
        {
            NCRYPT_DESCRIPTOR_HANDLE _hDescriptor = 0;
            auto _Status = ::NCryptCreateProtectionDescriptor(L"LOCAL=user", 0, &_hDescriptor);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);

            static const BYTE kSecretData[] = { 1,2,3,4,5,6,7,8 };
            BYTE* _pProtected = nullptr;
            ULONG _cbProtected = 0;
            _Status = ::NCryptProtectSecret(_hDescriptor, 0, kSecretData, sizeof(kSecretData), nullptr, nullptr, &_pProtected, &_cbProtected);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            Assert::IsNotNull(_pProtected);
            Assert::IsTrue(_cbProtected > sizeof(kSecretData));

            BYTE* _pUnprotected = nullptr;
            ULONG _cbUnprotected = 0;
            NCRYPT_DESCRIPTOR_HANDLE _hOutDescriptor = 0;
            _Status = ::NCryptUnprotectSecret(&_hOutDescriptor, 0, _pProtected, _cbProtected, nullptr, nullptr, &_pUnprotected, &_cbUnprotected);
            Assert::AreEqual(SECURITY_STATUS(ERROR_SUCCESS), _Status);
            Assert::IsNotNull(_pUnprotected);
            Assert::AreEqual(DWORD(sizeof(kSecretData)), DWORD(_cbUnprotected));
            Assert::IsTrue(memcmp(kSecretData, _pUnprotected, sizeof(kSecretData)) == 0);

            if (_pProtected)
                ::LocalFree(_pProtected);
            if (_pUnprotected)
                ::LocalFree(_pUnprotected);
            if (_hOutDescriptor)
                ::NCryptCloseProtectionDescriptor(_hOutDescriptor);
            ::NCryptCloseProtectionDescriptor(_hDescriptor);
        }
    };
}
