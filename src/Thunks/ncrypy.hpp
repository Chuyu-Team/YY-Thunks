#if YY_Thunks_Target < __WindowsNT6_2
#include <ncrypt.h>
#include <ncryptprotect.h>
#include <wincrypt.h>
#include <dpapi.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_2) && !defined(__Comment_Lib_crypt32)
#define __Comment_Lib_crypt32
#pragma comment(lib, "crypt32.lib")
#endif

namespace YY::Thunks {
#if defined(YY_Thunks_Implemented) && YY_Thunks_Target < __WindowsNT6_2
namespace
{
    enum class NcryptObjectType : DWORD
    {
        Provider = 1,
        Key = 2,
        Secret = 3,
        Descriptor = 4,
    };

    constexpr DWORD kNcryptObjectSignature = 0x594E4352; // 'YNCR'
    constexpr DWORD kNcryptProtectedBlobMagic = 0x53504E59; // 'YNPS'

#pragma pack(push, 1)
    struct NcryptProtectedBlobHeader
    {
        DWORD Magic;
        DWORD cbDescriptorChars;
        DWORD cbProtected;
    };
#pragma pack(pop)

    struct NcryptObjectHeader
    {
        DWORD Signature = kNcryptObjectSignature;
        uint32_t uRefCount = 1;
        NcryptObjectType Type;

        constexpr NcryptObjectHeader(NcryptObjectType _eType)
            : Type(_eType)
        {
        }

        void __fastcall AddRef()
        {
            InterlockedIncrement(&uRefCount);
        }

        void __fastcall Release()
        {
            if (InterlockedDecrement(&uRefCount) == 0)
            {
                internal::Delete(this);
            }
        }

        virtual ~NcryptObjectHeader() = default;
    };

    template<typename T>
    inline T* __fastcall NcryptObjectFromHandle(_In_opt_ NCRYPT_HANDLE _hObject, _In_ NcryptObjectType _eType)
    {
        if (_hObject == 0)
            return nullptr;

        auto _pObj = reinterpret_cast<T*>(_hObject);
        if (_pObj->Signature != kNcryptObjectSignature || _pObj->Type != _eType)
            return nullptr;

        return _pObj;
    }

    struct NcryptDescriptorObject : NcryptObjectHeader
    {
        DWORD cchDescriptor = 0;
        // wchar_t szDescriptor[0]

        constexpr NcryptDescriptorObject()
            : NcryptObjectHeader(NcryptObjectType::Descriptor)
        {
        }

        wchar_t* __fastcall GetDescriptorString()
        {
            if (cchDescriptor == 0)
                return nullptr;

            return reinterpret_cast<wchar_t*>(&cchDescriptor + 1);
        }
    };

    struct NcryptSecretObject : NcryptObjectHeader
    {
        DWORD cbSecret = 0;
        // BYTE SecretData[0] = {};

        constexpr NcryptSecretObject()
            : NcryptObjectHeader(NcryptObjectType::Secret)
        {}

        BYTE* __fastcall GetSecretData()
        {
            if (cbSecret == 0)
                return nullptr;

            return reinterpret_cast<BYTE*>(&cbSecret + 1);
        }
    };

#if YY_Thunks_Target < __WindowsNT6
    struct NcryptProviderObject : NcryptObjectHeader
    {
        HCRYPTPROV hProv = NULL;
        LPCWSTR szCryptProviderName = nullptr;
        DWORD dwProvType = 0;
        DWORD cchProviderName = 0;

        constexpr NcryptProviderObject()
            : NcryptObjectHeader(NcryptObjectType::Provider)
        {
        }

        LPWSTR __fastcall GetProviderName()
        {
            if (cchProviderName == 0)
                return nullptr;

            return reinterpret_cast<LPWSTR>(&cchProviderName + 1);
        }

        virtual ~NcryptProviderObject()
        {
            if (hProv)
            {
                ::CryptReleaseContext(hProv, 0);
                hProv = NULL;
            }
        }
    };

    struct NcryptKeyObject : NcryptObjectHeader
    {
        NcryptProviderObject* pNcryptProvider = nullptr;
        HCRYPTPROV hProv = NULL;
        HCRYPTKEY hKey = NULL;
        DWORD cbKey = 0;
        DWORD dwLegacyKeySpec = 0;
        bool bPersisted = false;
        wchar_t szAlgId[64] = {};
        wchar_t szKeyName[128] = {};

        constexpr NcryptKeyObject()
            : NcryptObjectHeader(NcryptObjectType::Key)
        {
        }

        virtual ~NcryptKeyObject()
        {
            // NcryptPersistKeyByName(this);

            if (hKey)
            {
                ::CryptDestroyKey(hKey);
                hKey = NULL;
            }

            if (hProv)
            {
                ::CryptReleaseContext(hProv, 0);
                hProv = NULL;
            }

            if (pNcryptProvider)
            {
                pNcryptProvider->Release();
                pNcryptProvider = nullptr;
            }
        }

        DWORD __fastcall GetKeySize() const
        {
            if (cbKey != 0)
                return cbKey;

            return 1024;
        }

        HCRYPTPROV GetProviderHandle() const
        {
            if (hProv)
                return hProv;

            if(pNcryptProvider)
                return pNcryptProvider->hProv;

            return NULL;
        }
    };

    static bool __fastcall NcryptAcquireContextByProvider(
        _In_ LPCWSTR _szProviderName,
        _In_opt_z_ LPCWSTR _szContainer,
        _In_ DWORD _dwFlags,
        _Out_ HCRYPTPROV* _phProv,
        _Outptr_opt_ LPCWSTR* _pszCryptProviderName = nullptr,
        _Out_opt_ DWORD* pdwProvType = nullptr)
    {
        if (_szProviderName == 0
            || _szProviderName[0] == L'\0'
            || StringCompareIgnoreCaseByAscii(_szProviderName, MS_KEY_STORAGE_PROVIDER, -1) == 0
            || StringCompareIgnoreCaseByAscii(_szProviderName, MS_PLATFORM_CRYPTO_PROVIDER, -1) == 0)
        {
            __WarningMessage__(MS_PLATFORM_CRYPTO_PROVIDER has no direct legacy CSP equivalent, fallback uses default CryptAPI RSA provider);
            
            const auto _szEnhRsaAesProvider = internal::GetSystemVersion() < internal::MakeVersion(6, 0) ? MS_ENH_RSA_AES_PROV_XP_W : MS_ENH_RSA_AES_PROV_W;
            if (CryptAcquireContextW(_phProv, _szContainer, _szEnhRsaAesProvider, PROV_RSA_AES, _dwFlags))
            {
                if (_pszCryptProviderName)
                {
                    *_pszCryptProviderName = _szEnhRsaAesProvider;
                }

                if (pdwProvType)
                {
                    *pdwProvType = PROV_RSA_AES;
                }

                return true;
            }

            if (CryptAcquireContextW(_phProv, _szContainer, nullptr, PROV_RSA_FULL, _dwFlags))
            {
                if (_pszCryptProviderName)
                {
                    *_pszCryptProviderName = nullptr;
                }

                if (pdwProvType)
                {
                    *pdwProvType = PROV_RSA_FULL;
                }

                return true;
            }
        }
        else if (StringCompareIgnoreCaseByAscii(_szProviderName, MS_SMART_CARD_KEY_STORAGE_PROVIDER, -1) == 0)
        {
            if (::CryptAcquireContextW(_phProv, _szContainer, MS_SCARD_PROV_W, PROV_RSA_FULL, _dwFlags))
            {
                if (_pszCryptProviderName)
                {
                    *_pszCryptProviderName = MS_SCARD_PROV_W;
                }

                if (pdwProvType)
                {
                    *pdwProvType = PROV_RSA_FULL;
                }
                return true;
            }
        }
        else
        {
            if (::CryptAcquireContextW(_phProv, _szContainer, _szProviderName, PROV_RSA_AES, _dwFlags))
            {
                if (_pszCryptProviderName)
                {
                    *_pszCryptProviderName = _szProviderName;
                }

                if (pdwProvType)
                {
                    *pdwProvType = PROV_RSA_AES;
                }
                return true;
            }

            if (::CryptAcquireContextW(_phProv, _szContainer, _szProviderName, PROV_RSA_FULL, _dwFlags))
            {
                if (_pszCryptProviderName)
                {
                    *_pszCryptProviderName = _szProviderName;
                }

                if (pdwProvType)
                {
                    *pdwProvType = PROV_RSA_FULL;
                }
                return true;
            }
        }

        return false;
    }

    static SECURITY_STATUS __fastcall NcryptPersistKeyByName(_In_ NcryptKeyObject* _pKey)
    {
        UNREFERENCED_PARAMETER(_pKey);
        return ERROR_SUCCESS;
    }

    static SECURITY_STATUS __fastcall NcryptCreateKeyObject(
        _In_ LPCWSTR _szAlgId,
        _In_opt_ LPCWSTR _szKeyName,
        _In_ NcryptProviderObject* _pProvider,
        _In_opt_ HCRYPTPROV _hProv,
        _In_opt_ HCRYPTKEY _hKey,
        _In_ DWORD _cbKey,
        _In_ DWORD _dwLegacyKeySpec,
        _In_ bool _bPersisted,
        _Out_ NCRYPT_KEY_HANDLE* _phKey)
    {
        auto _pKeyObject = internal::New<NcryptKeyObject>();
        if (_pKeyObject == nullptr)
            return NTE_NO_MEMORY;

        _pKeyObject->pNcryptProvider = _pProvider;
        if(_pProvider)
            _pProvider->AddRef();

        _pKeyObject->hProv = _hProv;
        _pKeyObject->hKey = _hKey;
        _pKeyObject->cbKey = _cbKey;
        _pKeyObject->dwLegacyKeySpec = _dwLegacyKeySpec;
        _pKeyObject->bPersisted = _bPersisted;
        if (_szAlgId)
        {
            internal::StringCopy(_pKeyObject->szAlgId, _countof(_pKeyObject->szAlgId), _szAlgId, -1);
        }
        if (_szKeyName)
        {
            internal::StringCopy(_pKeyObject->szKeyName, _countof(_pKeyObject->szKeyName), _szKeyName, -1);
        }

        *_phKey = reinterpret_cast<NCRYPT_KEY_HANDLE>(_pKeyObject);
        return ERROR_SUCCESS;
    }

    static SECURITY_STATUS __fastcall NcryptEnsureRsaKey(_Inout_ NcryptKeyObject* _pKeyObject)
    {
        if (_pKeyObject->hKey)
            return ERROR_SUCCESS;

        const DWORD _dwBits = _pKeyObject->GetKeySize();
        if (!::CryptGenKey(_pKeyObject->hProv, _pKeyObject->dwLegacyKeySpec, CRYPT_EXPORTABLE | (_dwBits << 16), &_pKeyObject->hKey))
            return NTE_FAIL;

        return ERROR_SUCCESS;
    }

    static SECURITY_STATUS __fastcall NcryptComputeHash(
        _In_ LPCWSTR _szHashAlg,
        _In_reads_bytes_(_cbData) const BYTE* _pData,
        _In_ DWORD _cbData,
        _Out_writes_bytes_(_cbOutput) BYTE* _pOutput,
        _In_ DWORD _cbOutput)
    {
        ALG_ID _AlgId = CALG_SHA_256;
        if (StringCompareIgnoreCaseByAscii(_szHashAlg, BCRYPT_SHA1_ALGORITHM, -1) == 0)
            _AlgId = CALG_SHA1;
        else if (StringCompareIgnoreCaseByAscii(_szHashAlg, BCRYPT_SHA256_ALGORITHM, -1) == 0)
            _AlgId = CALG_SHA_256;
        else
            return NTE_NOT_SUPPORTED;

        HCRYPTPROV _hProv = 0;
        if (!NcryptAcquireContextByProvider(nullptr, nullptr, CRYPT_VERIFYCONTEXT, &_hProv))
            return NTE_FAIL;

        HCRYPTHASH _hHash = 0;
        if (!::CryptCreateHash(_hProv, _AlgId, 0, 0, &_hHash)
            || !::CryptHashData(_hHash, _pData, _cbData, 0)
            || !::CryptGetHashParam(_hHash, HP_HASHVAL, _pOutput, &_cbOutput, 0))
        {
            if (_hHash)
                ::CryptDestroyHash(_hHash);
            ::CryptReleaseContext(_hProv, 0);
            return NTE_FAIL;
        }

        ::CryptDestroyHash(_hHash);
        ::CryptReleaseContext(_hProv, 0);
        return ERROR_SUCCESS;
    }
#endif // YY_Thunks_Target < __WindowsNT6
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
12,
SECURITY_STATUS,
WINAPI,
NCryptOpenStorageProvider,
    _Out_   NCRYPT_PROV_HANDLE* _phProvider,
    _In_opt_ LPCWSTR _szProviderName,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptOpenStorageProvider = try_get_NCryptOpenStorageProvider())
        return _pfnNCryptOpenStorageProvider(_phProvider, _szProviderName, _dwFlags);

    if (_phProvider == nullptr)
        return NTE_INVALID_PARAMETER;

    if (_dwFlags)
        return NTE_BAD_FLAGS;

    HCRYPTPROV _hProv = NULL;
    LPCWSTR _szCryptProviderName = nullptr;
    DWORD _dwProvType = 0;
    if (!NcryptAcquireContextByProvider(_szProviderName, nullptr, CRYPT_VERIFYCONTEXT, &_hProv, &_szCryptProviderName, &_dwProvType))
    {
        return NTE_BAD_PROVIDER;
    }

    const auto _cchProviderName = internal::StringLength(_szProviderName);
    const auto _cbProviderName = sizeof(wchar_t) * _cchProviderName;

    auto _pProvider = reinterpret_cast<NcryptProviderObject*>(internal::Alloc(sizeof(NcryptProviderObject) + _cbProviderName + sizeof(wchar_t)));
    if (_pProvider == nullptr)
        return NTE_NO_MEMORY;

    new(_pProvider) NcryptProviderObject();
    _pProvider->hProv = _hProv;
    _pProvider->dwProvType = _dwProvType;
    if (_szCryptProviderName)
    {
        _pProvider->szCryptProviderName = _szCryptProviderName == _szProviderName ? _pProvider->GetProviderName() : _szCryptProviderName;
    }

    _pProvider->cchProviderName = (DWORD)_cchProviderName;
    if (_cbProviderName)
    {
        memcpy(_pProvider->GetProviderName(), _szProviderName, _cbProviderName);
        _pProvider->GetProviderName()[_cchProviderName] = L'\0';
    }

    *_phProvider = reinterpret_cast<NCRYPT_PROV_HANDLE>(_pProvider);
    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
12,
SECURITY_STATUS,
WINAPI,
NCryptIsAlgSupported,
    _In_    NCRYPT_PROV_HANDLE _hProvider,
    _In_    LPCWSTR _pszAlgId,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptIsAlgSupported = try_get_NCryptIsAlgSupported())
        return _pfnNCryptIsAlgSupported(_hProvider, _pszAlgId, _dwFlags);

    if (!NcryptObjectFromHandle<NcryptProviderObject>(_hProvider, NcryptObjectType::Provider))
        return NTE_INVALID_HANDLE;

    if (_pszAlgId == nullptr)
        return NTE_INVALID_PARAMETER;

    if (_dwFlags)
        return NTE_BAD_FLAGS;

    if (StringCompareIgnoreCaseByAscii(_pszAlgId, BCRYPT_RSA_ALGORITHM, -1) == 0
        || StringCompareIgnoreCaseByAscii(_pszAlgId, BCRYPT_RSA_SIGN_ALGORITHM, -1) == 0)
        return ERROR_SUCCESS;

    return NTE_NOT_SUPPORTED;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
24,
SECURITY_STATUS,
WINAPI,
NCryptCreatePersistedKey,
    _In_    NCRYPT_PROV_HANDLE _hProvider,
    _Out_   NCRYPT_KEY_HANDLE* _phKey,
    _In_    LPCWSTR _pszAlgId,
    _In_opt_ LPCWSTR _pszKeyName,
    _In_    DWORD   _dwLegacyKeySpec,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptCreatePersistedKey = try_get_NCryptCreatePersistedKey())
        return _pfnNCryptCreatePersistedKey(_hProvider, _phKey, _pszAlgId, _pszKeyName, _dwLegacyKeySpec, _dwFlags);

    if (!NcryptObjectFromHandle<NcryptProviderObject>(_hProvider, NcryptObjectType::Provider))
        return NTE_INVALID_HANDLE;

    auto _pProvider = NcryptObjectFromHandle<NcryptProviderObject>(_hProvider, NcryptObjectType::Provider);

    if (_phKey == nullptr || _pszAlgId == nullptr)
        return NTE_INVALID_PARAMETER;

    DWORD _fCryptApiFlags = 0;
    if (_dwFlags & NCRYPT_MACHINE_KEY_FLAG)
    {
        _fCryptApiFlags |= CRYPT_MACHINE_KEYSET;
    }
   
    if (StringCompareIgnoreCaseByAscii(_pszAlgId, BCRYPT_RSA_ALGORITHM, -1) != 0
        && StringCompareIgnoreCaseByAscii(_pszAlgId, BCRYPT_RSA_SIGN_ALGORITHM, -1) != 0)
    {
#ifdef __WarningMessage__
        __WarningMessage__(NCryptCreatePersistedKey fallback currently only supports RSA persisted key via CryptAPI container);
#endif
        return NTE_NOT_SUPPORTED;
    }

    DWORD _dwKeySpec = _dwLegacyKeySpec;
    if (_dwKeySpec == 0)
    {
        _dwKeySpec = StringCompareIgnoreCaseByAscii(_pszAlgId, BCRYPT_RSA_SIGN_ALGORITHM, -1) == 0 ? AT_SIGNATURE : AT_KEYEXCHANGE;
    }
    if (_dwKeySpec != AT_SIGNATURE && _dwKeySpec != AT_KEYEXCHANGE)
        return NTE_BAD_ALGID;

    HCRYPTPROV _hProv = 0;
    const bool _bPersisted = (_pszKeyName && *_pszKeyName);
    if (_bPersisted)
    {
        if (!NcryptAcquireContextByProvider(_pProvider->GetProviderName(), _pszKeyName, _fCryptApiFlags | CRYPT_NEWKEYSET, &_hProv))
        {
            if (GetLastError() == NTE_EXISTS)
            {
                if (_dwFlags & NCRYPT_OVERWRITE_KEY_FLAG)
                {
                    if (!NcryptAcquireContextByProvider(_pProvider->GetProviderName(), _pszKeyName, _fCryptApiFlags, &_hProv))
                    {
                        return NTE_FAIL;
                    }
                } 
            }
            else
            {
                return NTE_FAIL;
            }
        }
    }

    HCRYPTKEY _hKey = 0;
    if (!::CryptGetUserKey(_hProv ? _hProv : _pProvider->hProv, _dwKeySpec, &_hKey))
    {
        if (!::CryptGenKey(_hProv ? _hProv : _pProvider->hProv, _dwKeySpec, CRYPT_EXPORTABLE | (1024 << 16), &_hKey))
        {
            if (_hProv)
            {
                ::CryptReleaseContext(_hProv, 0);
            }
            return NTE_FAIL;
        }
    }

    auto _Status = NcryptCreateKeyObject(_pszAlgId, _pszKeyName, _pProvider, _hProv, _hKey, 1024, _dwKeySpec, _bPersisted, _phKey);
    if (_Status != ERROR_SUCCESS)
    {
        ::CryptDestroyKey(_hKey);
        if (_hProv)
        {
            ::CryptReleaseContext(_hProv, 0);
        }
    }

    return _Status;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
20,
SECURITY_STATUS,
WINAPI,
NCryptSetProperty,
    _In_    NCRYPT_HANDLE _hObject,
    _In_    LPCWSTR _pszProperty,
    _In_reads_bytes_(_cbInput) PBYTE _pbInput,
    _In_    DWORD   _cbInput,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptSetProperty = try_get_NCryptSetProperty())
        return _pfnNCryptSetProperty(_hObject, _pszProperty, _pbInput, _cbInput, _dwFlags);

    if (_pszProperty == nullptr)
        return NTE_INVALID_PARAMETER;

    auto _pKey = NcryptObjectFromHandle<NcryptKeyObject>(_hObject, NcryptObjectType::Key);
    if (_pKey == nullptr)
        return NTE_INVALID_HANDLE;

    if (_dwFlags)
        return NTE_BAD_FLAGS;

    if(StringCompareIgnoreCaseByAscii(_pszProperty, NCRYPT_LENGTH_PROPERTY, -1) == 0)
    {
        if (_cbInput != sizeof(DWORD) || _pbInput == nullptr)
            return NTE_INVALID_PARAMETER;

        _pKey->cbKey = *reinterpret_cast<DWORD*>(_pbInput);
        return ERROR_SUCCESS;
    }

    return NTE_NOT_SUPPORTED;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
24,
SECURITY_STATUS,
WINAPI,
NCryptGetProperty,
    _In_    NCRYPT_HANDLE _hObject,
    _In_    LPCWSTR _pszProperty,
    _Out_writes_bytes_to_opt_(_cbOutput, *_pcbResult) PBYTE _pbOutput,
    _In_    DWORD   _cbOutput,
    _Out_   DWORD* _pcbResult,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptGetProperty = try_get_NCryptGetProperty())
        return _pfnNCryptGetProperty(_hObject, _pszProperty, _pbOutput, _cbOutput, _pcbResult, _dwFlags);

    if (_pszProperty == nullptr || _pcbResult == nullptr)
        return NTE_INVALID_PARAMETER;

    if (_dwFlags)
        return NTE_BAD_FLAGS;

    if (auto _pKey = NcryptObjectFromHandle<NcryptKeyObject>(_hObject, NcryptObjectType::Key))
    {
        if (StringCompareIgnoreCaseByAscii(_pszProperty, NCRYPT_LENGTH_PROPERTY, -1) == 0)
        {
            *_pcbResult = sizeof(DWORD);
            if (_pbOutput == nullptr)
                return ERROR_SUCCESS;

            if (_cbOutput < sizeof(DWORD))
                return NTE_BUFFER_TOO_SMALL;

            *reinterpret_cast<DWORD*>(_pbOutput) = _pKey->cbKey;
            return ERROR_SUCCESS;
        }
        else if (StringCompareIgnoreCaseByAscii(_pszProperty, NCRYPT_ALGORITHM_PROPERTY, -1) == 0)
        {
            const DWORD _cbData = (DWORD)((internal::StringLength(_pKey->szAlgId) + 1) * sizeof(wchar_t));
            *_pcbResult = _cbData;
            if (_pbOutput == nullptr)
                return ERROR_SUCCESS;

            if (_cbOutput < _cbData)
                return NTE_BUFFER_TOO_SMALL;

            memcpy(_pbOutput, _pKey->szAlgId, _cbData);
            return ERROR_SUCCESS;
        }

        return NTE_NOT_SUPPORTED;
    }

    return NTE_INVALID_HANDLE;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
8,
SECURITY_STATUS,
WINAPI,
NCryptFinalizeKey,
    _In_    NCRYPT_KEY_HANDLE _hKey,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptFinalizeKey = try_get_NCryptFinalizeKey())
        return _pfnNCryptFinalizeKey(_hKey, _dwFlags);

    auto _pKey = NcryptObjectFromHandle<NcryptKeyObject>(_hKey, NcryptObjectType::Key);
    if (_pKey == nullptr)
        return NTE_INVALID_HANDLE;

    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
20,
SECURITY_STATUS,
WINAPI,
NCryptOpenKey,
    _In_    NCRYPT_PROV_HANDLE _hProvider,
    _Out_   NCRYPT_KEY_HANDLE* _phKey,
    _In_    LPCWSTR _pszKeyName,
    _In_opt_ DWORD  _dwLegacyKeySpec,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptOpenKey = try_get_NCryptOpenKey())
        return _pfnNCryptOpenKey(_hProvider, _phKey, _pszKeyName, _dwLegacyKeySpec, _dwFlags);

    auto _pProvider = NcryptObjectFromHandle<NcryptProviderObject>(_hProvider, NcryptObjectType::Provider);
    if (!_pProvider)
        return NTE_INVALID_HANDLE;

    if (_phKey == nullptr || _pszKeyName == nullptr || *_pszKeyName == L'\0')
        return NTE_INVALID_PARAMETER;

    DWORD _fCryptApiFlags = 0;
    if (_dwFlags & NCRYPT_MACHINE_KEY_FLAG)
    {
        _fCryptApiFlags |= CRYPT_MACHINE_KEYSET;
    }

    if (_dwFlags & NCRYPT_SILENT_FLAG)
    {
        _fCryptApiFlags |= CRYPT_SILENT;
    }

    DWORD _dwKeySpec = _dwLegacyKeySpec;
    if (_dwKeySpec == 0)
        _dwKeySpec = AT_KEYEXCHANGE;
    if (_dwKeySpec != AT_SIGNATURE && _dwKeySpec != AT_KEYEXCHANGE)
        return NTE_BAD_ALGID;

    HCRYPTPROV _hProv = 0;
    if (!NcryptAcquireContextByProvider(_pProvider->GetProviderName(), _pszKeyName, _fCryptApiFlags, &_hProv))
    {
        if (GetLastError() == NTE_BAD_KEYSET)
            return NTE_NOT_FOUND;
        return NTE_FAIL;
    }

    HCRYPTKEY _hLegacyKey = 0;
    if (!::CryptGetUserKey(_hProv, _dwKeySpec, &_hLegacyKey))
    {
        if (_dwLegacyKeySpec == 0)
        {
            if (::CryptGetUserKey(_hProv, AT_SIGNATURE, &_hLegacyKey))
            {
                _dwKeySpec = AT_SIGNATURE;
            }
            else if (::CryptGetUserKey(_hProv, AT_KEYEXCHANGE, &_hLegacyKey))
            {
                _dwKeySpec = AT_KEYEXCHANGE;
            }
            else
            {
                ::CryptReleaseContext(_hProv, 0);
                return NTE_NOT_FOUND;
            }
        }
        else
        {
            ::CryptReleaseContext(_hProv, 0);
            return NTE_NOT_FOUND;
        }
    }

    auto _Status = NcryptCreateKeyObject(BCRYPT_RSA_ALGORITHM, _pszKeyName, _pProvider, _hProv, _hLegacyKey, 0, _dwKeySpec, true, _phKey);
    if (_Status != ERROR_SUCCESS)
    {
        ::CryptDestroyKey(_hLegacyKey);
        ::CryptReleaseContext(_hProv, 0);
    }
    return _Status;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
32,
SECURITY_STATUS,
WINAPI,
NCryptImportKey,
    _In_    NCRYPT_PROV_HANDLE _hProvider,
    _In_opt_ NCRYPT_KEY_HANDLE _hImportKey,
    _In_    LPCWSTR _szBlobType,
    _In_opt_ NCryptBufferDesc* _pParameterList,
    _Out_   NCRYPT_KEY_HANDLE* _phKey,
    _In_reads_bytes_(_cbData) PBYTE _pbData,
    _In_    DWORD   _cbData,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptImportKey = try_get_NCryptImportKey())
        return _pfnNCryptImportKey(_hProvider, _hImportKey, _szBlobType, _pParameterList, _phKey, _pbData, _cbData, _dwFlags);

    const auto _pProvider = NcryptObjectFromHandle<NcryptProviderObject>(_hProvider, NcryptObjectType::Provider);
    if (!_pProvider)
        return NTE_INVALID_HANDLE;

    NcryptKeyObject* _pImportKey = nullptr;
    if (_hImportKey)
    {
        _pImportKey = NcryptObjectFromHandle<NcryptKeyObject>(_hImportKey, NcryptObjectType::Key);
        if (!_pImportKey)
            return NTE_INVALID_HANDLE;
    }

    if (_phKey == nullptr || _szBlobType == nullptr || (_pbData == nullptr && _cbData))
        return NTE_INVALID_PARAMETER;

    UNREFERENCED_PARAMETER(_dwFlags);

    const HCRYPTKEY _hNativeImportKey = _hImportKey ? _pImportKey->hKey : NULL;
    HCRYPTKEY _hKey = 0;

    UNREFERENCED_PARAMETER(_pParameterList);

    if (StringCompareIgnoreCaseByAscii(LEGACY_RSAPRIVATE_BLOB, _szBlobType, -1) == 0
        || StringCompareIgnoreCaseByAscii(BCRYPT_PRIVATE_KEY_BLOB, _szBlobType, -1) == 0
        || StringCompareIgnoreCaseByAscii(LEGACY_RSAPUBLIC_BLOB, _szBlobType, -1) == 0)
    {
        if (!CryptImportKey(_pProvider->hProv, _pbData, _cbData, _hNativeImportKey, CRYPT_EXPORTABLE, &_hKey))
        {
            return NTE_NOT_SUPPORTED;
        }
    }
    else if (StringCompareIgnoreCaseByAscii(BCRYPT_RSAPUBLIC_BLOB, _szBlobType, -1) == 0
        || StringCompareIgnoreCaseByAscii(BCRYPT_PUBLIC_KEY_BLOB, _szBlobType, -1) == 0)
    {
        if (_cbData < sizeof(BCRYPT_RSAKEY_BLOB))
        {
            return NTE_INVALID_PARAMETER;
        }

        auto _pBCryptRsaBlob = reinterpret_cast<const BCRYPT_RSAKEY_BLOB*>(_pbData);
        if (_pBCryptRsaBlob->Magic != BCRYPT_RSAPUBLIC_MAGIC
            || _pBCryptRsaBlob->cbPublicExp == 0
            || _pBCryptRsaBlob->cbPublicExp > sizeof(DWORD)
            || _pBCryptRsaBlob->cbModulus == 0
            || _pBCryptRsaBlob->cbPrime1 != 0
            || _pBCryptRsaBlob->cbPrime2 != 0)
        {
            return NTE_INVALID_PARAMETER;
        }

        const auto _cbNeed = sizeof(BCRYPT_RSAKEY_BLOB) + _pBCryptRsaBlob->cbPublicExp + _pBCryptRsaBlob->cbModulus;
        if (_cbNeed > _cbData)
        {
            return NTE_INVALID_PARAMETER;
        }

        const auto _uBitLength = _pBCryptRsaBlob->cbModulus * 8;
        if (_pBCryptRsaBlob->BitLength != _uBitLength)
        {
            return NTE_INVALID_PARAMETER;
        }

        const BYTE* _pPublicExp = _pbData + sizeof(BCRYPT_RSAKEY_BLOB);
        DWORD _uPublicExp = 0;
        for (ULONG i = 0; i != _pBCryptRsaBlob->cbPublicExp; ++i)
        {
            _uPublicExp <<= 8;
            _uPublicExp |= _pPublicExp[i];
        }

        if (_uPublicExp == 0)
        {
            return NTE_INVALID_PARAMETER;
        }

        const auto _cbLegacyBlob = sizeof(_PUBLICKEYBLOB) + _pBCryptRsaBlob->cbModulus;
        auto _pLegacyBlob = reinterpret_cast<_PUBLICKEYBLOB*>(_malloca(_cbLegacyBlob));
        if (_pLegacyBlob == nullptr)
        {
            return NTE_NO_MEMORY;
        }

        _pLegacyBlob->bType = PUBLICKEYBLOB;
        _pLegacyBlob->bVersion = CUR_BLOB_VERSION;
        _pLegacyBlob->reserved = 0;
        _pLegacyBlob->aiKeyAlg = CALG_RSA_KEYX;
        _pLegacyBlob->magic = 0x31415352;
        _pLegacyBlob->bitlen = _uBitLength;
        _pLegacyBlob->pubexp = _uPublicExp;
        MoveAndReverse(reinterpret_cast<PBYTE>(_pLegacyBlob + 1), _pPublicExp + _pBCryptRsaBlob->cbPublicExp, _pBCryptRsaBlob->cbModulus);

        if (!CryptImportKey(_pProvider->hProv, reinterpret_cast<const BYTE*>(_pLegacyBlob), _cbLegacyBlob, _hNativeImportKey, CRYPT_EXPORTABLE, &_hKey))
        {
            _freea(_pLegacyBlob);
            return NTE_NOT_SUPPORTED;
        }

        _freea(_pLegacyBlob);
    }
    else if (StringCompareIgnoreCaseByAscii(BCRYPT_RSAFULLPRIVATE_BLOB, _szBlobType, -1) == 0)
    {
        if (_cbData < sizeof(BCRYPT_RSAKEY_BLOB))
        {
            return NTE_INVALID_PARAMETER;
        }

        auto _pBCryptRsaBlob = reinterpret_cast<const BCRYPT_RSAKEY_BLOB*>(_pbData);
        if (_pBCryptRsaBlob->Magic != BCRYPT_RSAFULLPRIVATE_MAGIC
            || _pBCryptRsaBlob->cbPublicExp == 0
            || _pBCryptRsaBlob->cbPublicExp > sizeof(DWORD)
            || _pBCryptRsaBlob->cbModulus == 0
            || _pBCryptRsaBlob->cbPrime1 == 0
            || _pBCryptRsaBlob->cbPrime2 == 0)
        {
            return NTE_INVALID_PARAMETER;
        }

        if (_pBCryptRsaBlob->BitLength != _pBCryptRsaBlob->cbModulus * 8
            || _pBCryptRsaBlob->cbPrime1 * 2 != _pBCryptRsaBlob->cbModulus
            || _pBCryptRsaBlob->cbPrime2 != _pBCryptRsaBlob->cbPrime1)
        {
            return NTE_INVALID_PARAMETER;
        }

        const auto _cbNeed = sizeof(BCRYPT_RSAKEY_BLOB)
            + _pBCryptRsaBlob->cbPublicExp
            + _pBCryptRsaBlob->cbModulus
            + _pBCryptRsaBlob->cbPrime1
            + _pBCryptRsaBlob->cbPrime2
            + _pBCryptRsaBlob->cbPrime1
            + _pBCryptRsaBlob->cbPrime2
            + _pBCryptRsaBlob->cbPrime1
            + _pBCryptRsaBlob->cbModulus;
        if (_cbNeed > _cbData)
        {
            return NTE_INVALID_PARAMETER;
        }

        const BYTE* _pPublicExp = _pbData + sizeof(BCRYPT_RSAKEY_BLOB);
        DWORD _uPublicExp = 0;
        for (ULONG i = 0; i != _pBCryptRsaBlob->cbPublicExp; ++i)
        {
            _uPublicExp <<= 8;
            _uPublicExp |= _pPublicExp[i];
        }

        if (_uPublicExp == 0)
        {
            return NTE_INVALID_PARAMETER;
        }

        const auto _cbLegacyBlob = sizeof(_PRIVATEKEYBLOB)
            + _pBCryptRsaBlob->cbModulus
            + _pBCryptRsaBlob->cbPrime1
            + _pBCryptRsaBlob->cbPrime2
            + _pBCryptRsaBlob->cbPrime1
            + _pBCryptRsaBlob->cbPrime2
            + _pBCryptRsaBlob->cbPrime1
            + _pBCryptRsaBlob->cbModulus;
        auto _pLegacyBlob = reinterpret_cast<_PRIVATEKEYBLOB*>(_malloca(_cbLegacyBlob));
        if (_pLegacyBlob == nullptr)
        {
            return NTE_NO_MEMORY;
        }

        _pLegacyBlob->bType = PRIVATEKEYBLOB;
        _pLegacyBlob->bVersion = CUR_BLOB_VERSION;
        _pLegacyBlob->reserved = 0;
        _pLegacyBlob->aiKeyAlg = CALG_RSA_KEYX;
        _pLegacyBlob->magic = 0x32415352;
        _pLegacyBlob->bitlen = _pBCryptRsaBlob->BitLength;
        _pLegacyBlob->pubexp = _uPublicExp;

        auto _pOut = reinterpret_cast<PBYTE>(_pLegacyBlob + 1);
        auto _pIn = _pPublicExp + _pBCryptRsaBlob->cbPublicExp;
        MoveAndReverse(_pOut, _pIn, _pBCryptRsaBlob->cbModulus);
        _pOut += _pBCryptRsaBlob->cbModulus;
        _pIn += _pBCryptRsaBlob->cbModulus;

        MoveAndReverse(_pOut, _pIn, _pBCryptRsaBlob->cbPrime1);
        _pOut += _pBCryptRsaBlob->cbPrime1;
        _pIn += _pBCryptRsaBlob->cbPrime1;

        MoveAndReverse(_pOut, _pIn, _pBCryptRsaBlob->cbPrime2);
        _pOut += _pBCryptRsaBlob->cbPrime2;
        _pIn += _pBCryptRsaBlob->cbPrime2;

        MoveAndReverse(_pOut, _pIn, _pBCryptRsaBlob->cbPrime1);
        _pOut += _pBCryptRsaBlob->cbPrime1;
        _pIn += _pBCryptRsaBlob->cbPrime1;

        MoveAndReverse(_pOut, _pIn, _pBCryptRsaBlob->cbPrime2);
        _pOut += _pBCryptRsaBlob->cbPrime2;
        _pIn += _pBCryptRsaBlob->cbPrime2;

        MoveAndReverse(_pOut, _pIn, _pBCryptRsaBlob->cbPrime1);
        _pOut += _pBCryptRsaBlob->cbPrime1;
        _pIn += _pBCryptRsaBlob->cbPrime1;

        MoveAndReverse(_pOut, _pIn, _pBCryptRsaBlob->cbModulus);

        if (!CryptImportKey(_pProvider->hProv, reinterpret_cast<const BYTE*>(_pLegacyBlob), _cbLegacyBlob, _hNativeImportKey, CRYPT_EXPORTABLE, &_hKey))
        {
            _freea(_pLegacyBlob);
            return NTE_NOT_SUPPORTED;
        }

        _freea(_pLegacyBlob);
    }
    else
    {
        return NTE_NOT_SUPPORTED;
    }

    auto _Status = NcryptCreateKeyObject(BCRYPT_RSA_ALGORITHM, nullptr, _pProvider, NULL, _hKey, 0, 0, false, _phKey);
    if (_Status != ERROR_SUCCESS)
    {
        ::CryptDestroyKey(_hKey);
    }

    return _Status;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
32,
SECURITY_STATUS,
WINAPI,
NCryptExportKey,
    _In_    NCRYPT_KEY_HANDLE _hKey,
    _In_opt_ NCRYPT_KEY_HANDLE _hExportKey,
    _In_    LPCWSTR _szBlobType,
    _In_opt_ NCryptBufferDesc* _pParameterList,
    _Out_writes_bytes_to_opt_(_cbOutput, *_pcbResult) PBYTE _pOutput,
    _In_    DWORD   _cbOutput,
    _Out_   DWORD* _pcbResult,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptExportKey = try_get_NCryptExportKey())
        return _pfnNCryptExportKey(_hKey, _hExportKey, _szBlobType, _pParameterList, _pOutput, _cbOutput, _pcbResult, _dwFlags);

    auto _pKey = NcryptObjectFromHandle<NcryptKeyObject>(_hKey, NcryptObjectType::Key);
    if (_pKey == nullptr)
        return NTE_INVALID_HANDLE;

    HCRYPTKEY _hCryptExpKey = NULL;
    if (_hExportKey)
    {
        auto _pExportKey = NcryptObjectFromHandle<NcryptKeyObject>(_hExportKey, NcryptObjectType::Key);
        if (_pExportKey == nullptr)
            return NTE_INVALID_HANDLE;

        _hCryptExpKey = _pExportKey->hKey;
    }

    if (_szBlobType == nullptr || _pcbResult == nullptr)
        return NTE_INVALID_PARAMETER;

    if (_dwFlags & (~NCRYPT_SILENT_FLAG))
        return NTE_BAD_FLAGS;

    UNREFERENCED_PARAMETER(_pParameterList);

    if (StringCompareIgnoreCaseByAscii(BCRYPT_RSAFULLPRIVATE_BLOB, _szBlobType, -1) == 0)
    {
        DWORD _cbResult = _cbOutput;
        if (!CryptExportKey(_pKey->hKey, _hCryptExpKey, PRIVATEKEYBLOB, 0, (BYTE*)_pOutput, &_cbResult))
        {
            return NTE_NOT_SUPPORTED;
        }

        if (!_pOutput)
        {
            // _BCRYPT_RSAFULLPRIVATE_BLOB 比 _PRIVATEKEYBLOB最多多8个字节
            *_pcbResult = _cbResult + 8;
            return ERROR_SUCCESS;
        }
        *_pcbResult = _cbOutput;
        // _BCRYPT_RSAPRIVATE_BLOB 大小必定小于 _PRIVATEKEYBLOB，所以转换必然成功。
        if (!((_PRIVATEKEYBLOB*)_pOutput)->To<_BCRYPT_RSAFULLPRIVATE_BLOB>(_pcbResult))
        {
            return NTE_BUFFER_TOO_SMALL;
        }

        return ERROR_SUCCESS;
    }
    else if (StringCompareIgnoreCaseByAscii(BCRYPT_RSAPRIVATE_BLOB, _szBlobType, -1) == 0)
    {
        if (!CryptExportKey(_pKey->hKey, _hCryptExpKey, PRIVATEKEYBLOB, 0, (BYTE*)_pOutput, &_cbOutput))
        {
            return NTE_NOT_SUPPORTED;
        }

        *_pcbResult = _cbOutput;
        if (!_pOutput)
        {
            return ERROR_SUCCESS;
        }

        // _BCRYPT_RSAPRIVATE_BLOB 大小必定小于 _PRIVATEKEYBLOB，所以转换必然成功。
        ((_PRIVATEKEYBLOB*)_pOutput)->To<_BCRYPT_RSAPRIVATE_BLOB>(_pcbResult);
        return ERROR_SUCCESS;
    }
    else if (StringCompareIgnoreCaseByAscii(BCRYPT_RSAPUBLIC_BLOB, _szBlobType, -1) == 0
        || StringCompareIgnoreCaseByAscii(BCRYPT_PUBLIC_KEY_BLOB, _szBlobType, -1) == 0)
    {
        DWORD _cbResult = _cbOutput;
        if (!CryptExportKey(_pKey->hKey, _hCryptExpKey, PUBLICKEYBLOB, 0, (BYTE*)_pOutput, &_cbResult))
        {
            return NTE_NOT_SUPPORTED;
        }

        if (!_pOutput)
        {
            //  比 _PUBLICKEYBLOB 最多大 8个字节
            *_pcbResult = _cbResult + 8;
            return ERROR_SUCCESS;
        }

        *_pcbResult = _cbOutput;
        if (!((_PUBLICKEYBLOB*)_pOutput)->To<_BCRYPT_RSAPUBLIC_BLOB>(_pcbResult))
        {
            return NTE_BUFFER_TOO_SMALL;
        }

        return ERROR_SUCCESS;
    }
    else if (StringCompareIgnoreCaseByAscii(LEGACY_RSAPRIVATE_BLOB, _szBlobType, -1) == 0
        || StringCompareIgnoreCaseByAscii(BCRYPT_PRIVATE_KEY_BLOB, _szBlobType, -1) == 0)
    {
        if (!CryptExportKey(_pKey->hKey, _hCryptExpKey, PRIVATEKEYBLOB, 0, (BYTE*)_pOutput, &_cbOutput))
        {
            return NTE_NOT_SUPPORTED;
        }
        *_pcbResult = _cbOutput;
        return ERROR_SUCCESS;
    }
    else if (StringCompareIgnoreCaseByAscii(LEGACY_RSAPUBLIC_BLOB, _szBlobType, -1) == 0)
    {
        if (!CryptExportKey(_pKey->hKey, _hCryptExpKey, PUBLICKEYBLOB, 0, (BYTE*)_pOutput, &_cbOutput))
        {
            return NTE_NOT_SUPPORTED;
        }
        *_pcbResult = _cbOutput;
        return ERROR_SUCCESS;
    }
    else
    {
        return NTE_BAD_TYPE;
    }
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
32,
SECURITY_STATUS,
WINAPI,
NCryptSignHash,
    _In_    NCRYPT_KEY_HANDLE _hKey,
    _In_opt_    VOID* _pPaddingInfo,
    _In_reads_bytes_(_cbHashValue) PBYTE _pbHashValue,
    _In_    DWORD   _cbHashValue,
    _Out_writes_bytes_to_opt_(_cbSignature, *_pcbResult) PBYTE _pbSignature,
    _In_    DWORD   _cbSignature,
    _Out_   DWORD* _pcbResult,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptSignHash = try_get_NCryptSignHash())
        return _pfnNCryptSignHash(_hKey, _pPaddingInfo, _pbHashValue, _cbHashValue, _pbSignature, _cbSignature, _pcbResult, _dwFlags);

    auto _pKey = NcryptObjectFromHandle<NcryptKeyObject>(_hKey, NcryptObjectType::Key);
    if (_pKey == nullptr)
        return NTE_INVALID_HANDLE;

    if (_pcbResult == nullptr || (_pbHashValue == nullptr && _cbHashValue))
        return NTE_INVALID_PARAMETER;

    if ((_dwFlags & ~BCRYPT_PAD_PKCS1) != 0)
        return NTE_BAD_FLAGS;

    ALG_ID _AlgId = CALG_SHA1;
    if (_dwFlags & BCRYPT_PAD_PKCS1)
    {
        auto _pInfo = reinterpret_cast<BCRYPT_PKCS1_PADDING_INFO*>(_pPaddingInfo);
        if (_pInfo == nullptr || _pInfo->pszAlgId == nullptr)
            return NTE_INVALID_PARAMETER;

        if (StringCompareIgnoreCaseByAscii(_pInfo->pszAlgId, BCRYPT_SHA256_ALGORITHM, -1) == 0)
            _AlgId = CALG_SHA_256;
        else if (StringCompareIgnoreCaseByAscii(_pInfo->pszAlgId, BCRYPT_SHA1_ALGORITHM, -1) == 0)
            _AlgId = CALG_SHA1;
        else
            return NTE_NOT_SUPPORTED;
    }

    HCRYPTHASH _hHash = 0;
    if (!::CryptCreateHash(_pKey->GetProviderHandle(), _AlgId, 0, 0, &_hHash)
        || !::CryptHashData(_hHash, _pbHashValue, _cbHashValue, 0))
    {
        if (_hHash)
            ::CryptDestroyHash(_hHash);
        return NTE_FAIL;
    }

    DWORD _cbSig = _cbSignature;
    if (!::CryptSignHashW(_hHash, _pKey->dwLegacyKeySpec, nullptr, 0, _pbSignature, &_cbSig))
    {
        ::CryptDestroyHash(_hHash);
        return NTE_NOT_SUPPORTED;
    }

    ::CryptDestroyHash(_hHash);
    *_pcbResult = _cbSig;

    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
28,
SECURITY_STATUS,
WINAPI,
NCryptVerifySignature,
    _In_    NCRYPT_KEY_HANDLE _hKey,
    _In_opt_    VOID* _pPaddingInfo,
    _In_reads_bytes_(_cbHashValue) PBYTE _pbHashValue,
    _In_    DWORD   _cbHashValue,
    _In_reads_bytes_(_cbSignature) PBYTE _pbSignature,
    _In_    DWORD   _cbSignature,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptVerifySignature = try_get_NCryptVerifySignature())
        return _pfnNCryptVerifySignature(_hKey, _pPaddingInfo, _pbHashValue, _cbHashValue, _pbSignature, _cbSignature, _dwFlags);

    auto _pKey = NcryptObjectFromHandle<NcryptKeyObject>(_hKey, NcryptObjectType::Key);
    if (_pKey == nullptr)
        return NTE_INVALID_HANDLE;

    if ((_dwFlags & ~BCRYPT_PAD_PKCS1) != 0)
        return NTE_BAD_FLAGS;

    ALG_ID _AlgId = CALG_SHA1;
    if (_dwFlags & BCRYPT_PAD_PKCS1)
    {
        auto _pInfo = reinterpret_cast<BCRYPT_PKCS1_PADDING_INFO*>(_pPaddingInfo);
        if (_pInfo == nullptr || _pInfo->pszAlgId == nullptr)
            return NTE_INVALID_PARAMETER;

        if (StringCompareIgnoreCaseByAscii(_pInfo->pszAlgId, BCRYPT_SHA256_ALGORITHM, -1) == 0)
            _AlgId = CALG_SHA_256;
        else if (StringCompareIgnoreCaseByAscii(_pInfo->pszAlgId, BCRYPT_SHA1_ALGORITHM, -1) == 0)
            _AlgId = CALG_SHA1;
        else
            return NTE_NOT_SUPPORTED;
    }

    HCRYPTHASH _hHash = 0;
    if (!::CryptCreateHash(_pKey->GetProviderHandle(), _AlgId, 0, 0, &_hHash)
        || !::CryptHashData(_hHash, _pbHashValue, _cbHashValue, 0))
    {
        if (_hHash)
            ::CryptDestroyHash(_hHash);
        return NTE_FAIL;
    }

    if (!::CryptVerifySignatureW(_hHash, _pbSignature, _cbSignature, _pKey->hKey, nullptr, 0))
    {
        ::CryptDestroyHash(_hHash);
        return NTE_BAD_SIGNATURE;
    }

    ::CryptDestroyHash(_hHash);

    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
32,
SECURITY_STATUS,
WINAPI,
NCryptEncrypt,
    _In_    NCRYPT_KEY_HANDLE _hKey,
    _In_reads_bytes_opt_(_cbInput) PBYTE _pInput,
    _In_    DWORD   _cbInput,
    _In_opt_    VOID* _pPaddingInfo,
    _Out_writes_bytes_to_opt_(_cbOutput, *_pcbResult) PBYTE _pOutput,
    _In_    DWORD   _cbOutput,
    _Out_   DWORD* _pcbResult,
    _In_    DWORD   _fFlags)
{
    if (const auto _pfnNCryptEncrypt = try_get_NCryptEncrypt())
        return _pfnNCryptEncrypt(_hKey, _pInput, _cbInput, _pPaddingInfo, _pOutput, _cbOutput, _pcbResult, _fFlags);

    auto _pKey = NcryptObjectFromHandle<NcryptKeyObject>(_hKey, NcryptObjectType::Key);
    if (_pKey == nullptr)
        return NTE_INVALID_HANDLE;

    UNREFERENCED_PARAMETER(_pPaddingInfo);

    if (_pcbResult == nullptr)
        return NTE_INVALID_PARAMETER;

    BOOL _bFinal = FALSE;
    if (_fFlags & NCRYPT_NO_PADDING_FLAG)
    {

    }
    else if (_fFlags & NCRYPT_PAD_PKCS1_FLAG)
    {
        _bFinal = TRUE;
    }
    else
    {
        return NTE_NOT_SUPPORTED;
    }

    auto _cbOutputBufferNeed = _cbInput;
    if (!CryptEncrypt(_pKey->hKey, NULL, _bFinal, 0, nullptr, &_cbOutputBufferNeed, 0))
    {
        const auto _lStatus = GetLastError();
        if (_pOutput == nullptr && NTE_BAD_LEN == _lStatus)
        {
            *_pcbResult = _pKey->GetKeySize() / 8;
            return ERROR_SUCCESS;
        }

        return NTE_NOT_SUPPORTED;
    }

    if (_pOutput == nullptr)
    {
        *_pcbResult = _cbOutputBufferNeed;
        return ERROR_SUCCESS;
    }

    if (_pInput == nullptr)
    {
        return NTE_INVALID_PARAMETER;
    }

    if (_cbOutput < _cbOutputBufferNeed)
    {
        *_pcbResult = _cbOutputBufferNeed;
        return NTE_BUFFER_TOO_SMALL;
    }

    if (_pOutput != _pInput)
    {
        memcpy(_pOutput, _pInput, _cbInput);
    }

    if (!CryptEncrypt(_pKey->hKey, NULL, _bFinal, 0, _pOutput, &_cbInput, _cbOutput))
        return NTE_NOT_SUPPORTED;

    *_pcbResult = _cbInput;
    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
32,
SECURITY_STATUS,
WINAPI,
NCryptDecrypt,
    _In_    NCRYPT_KEY_HANDLE _hKey,
    _In_reads_bytes_opt_(_cbInput) PBYTE _pInput,
    _In_    DWORD   _cbInput,
    _In_opt_    VOID* _pPaddingInfo,
    _Out_writes_bytes_to_opt_(_cbOutput, *_pcbResult) PBYTE _pOutput,
    _In_    DWORD   _cbOutput,
    _Out_   DWORD* _pcbResult,
    _In_    DWORD   _fFlags)
{
    if (const auto _pfnNCryptDecrypt = try_get_NCryptDecrypt())
        return _pfnNCryptDecrypt(_hKey, _pInput, _cbInput, _pPaddingInfo, _pOutput, _cbOutput, _pcbResult, _fFlags);

    auto _pKey = NcryptObjectFromHandle<NcryptKeyObject>(_hKey, NcryptObjectType::Key);
    if (_pKey == nullptr)
        return NTE_INVALID_HANDLE;

    UNREFERENCED_PARAMETER(_pPaddingInfo);

    if (_pcbResult == nullptr)
        return NTE_INVALID_PARAMETER;

    BOOL _bFinal = FALSE;
    if (_fFlags & NCRYPT_NO_PADDING_FLAG)
    {

    }
    else if (_fFlags & NCRYPT_PAD_PKCS1_FLAG)
    {
        _bFinal = TRUE;
    }
    else
    {
        return NTE_NOT_SUPPORTED;
    }

    if (_pInput == nullptr)
    {
        return NTE_INVALID_PARAMETER;
    }

    if (_pOutput == nullptr)
    {
        *_pcbResult = _cbInput;
        return ERROR_SUCCESS;
    }

    if (_cbOutput < _cbInput)
    {
        *_pcbResult = _cbInput;
        return NTE_BUFFER_TOO_SMALL;
    }

    memcpy(_pOutput, _pInput, _cbInput);
    if (!CryptDecrypt(_pKey->hKey, NULL, _bFinal, 0, _pOutput, &_cbInput))
        return NTE_NOT_SUPPORTED;

    *_pcbResult = _cbInput;
    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
8,
SECURITY_STATUS,
WINAPI,
NCryptDeleteKey,
    _In_    NCRYPT_KEY_HANDLE _hKey,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptDeleteKey = try_get_NCryptDeleteKey())
        return _pfnNCryptDeleteKey(_hKey, _dwFlags);

    if (_dwFlags &(~NCRYPT_SILENT_FLAG))
        return NTE_BAD_FLAGS;

    auto _pKey = NcryptObjectFromHandle<NcryptKeyObject>(_hKey, NcryptObjectType::Key);
    if (_pKey == nullptr)
        return NTE_INVALID_HANDLE;

    if (_pKey->bPersisted && _pKey->szKeyName[0] != L'\0')
    {
        HCRYPTPROV _hProv = 0;
        auto _pNcryptProvider = _pKey->pNcryptProvider;
        DWORD _fCryptFlags = CRYPT_DELETEKEYSET;
        if (_dwFlags & NCRYPT_SILENT_FLAG)
            _fCryptFlags |= CRYPT_SILENT;

        ::CryptAcquireContextW(&_hProv, _pKey->szKeyName, _pNcryptProvider->szCryptProviderName, _pNcryptProvider->dwProvType, CRYPT_DELETEKEYSET);
    }

    _pKey->Release();
    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
4,
SECURITY_STATUS,
WINAPI,
NCryptFreeObject,
    _In_    NCRYPT_HANDLE _hObject)
{
    if (const auto _pfnNCryptFreeObject = try_get_NCryptFreeObject())
        return _pfnNCryptFreeObject(_hObject);

    if (_hObject == 0)
        return NTE_INVALID_HANDLE;

    auto _pObject = reinterpret_cast<NcryptObjectHeader*>(_hObject);
    if (_pObject->Signature != kNcryptObjectSignature)
        return NTE_INVALID_HANDLE;

    _pObject->Release();
    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
16,
SECURITY_STATUS,
WINAPI,
NCryptSecretAgreement,
    _In_    NCRYPT_KEY_HANDLE _hPrivKey,
    _In_    NCRYPT_KEY_HANDLE _hPubKey,
    _Out_   NCRYPT_SECRET_HANDLE* _phAgreedSecret,
    _In_    DWORD   _dwFlags)
{
    if (const auto _pfnNCryptSecretAgreement = try_get_NCryptSecretAgreement())
        return _pfnNCryptSecretAgreement(_hPrivKey, _hPubKey, _phAgreedSecret, _dwFlags);

    auto _pPriv = NcryptObjectFromHandle<NcryptKeyObject>(_hPrivKey, NcryptObjectType::Key);
    auto _pPub = NcryptObjectFromHandle<NcryptKeyObject>(_hPubKey, NcryptObjectType::Key);
    if (_pPriv == nullptr || _pPub == nullptr || _phAgreedSecret == nullptr)
        return NTE_INVALID_PARAMETER;

    if (_dwFlags & (~NCRYPT_SILENT_FLAG))
        return NTE_BAD_FLAGS;

    DWORD _cbPriv = 0;
    DWORD _cbPub = 0;
    if (!::CryptExportKey(_pPriv->hKey, 0, PUBLICKEYBLOB, 0, nullptr, &_cbPriv)
        || !::CryptExportKey(_pPub->hKey, 0, PUBLICKEYBLOB, 0, nullptr, &_cbPub))
        return NTE_NOT_SUPPORTED;

    const auto _cbMix = _cbPriv + _cbPub;
    internal::StringBuffer<BYTE> _oBuffer;
    auto _pData = _oBuffer.GetBuffer(_cbMix);
    if (!_pData)
    {
        return NTE_NO_MEMORY;
    }

    if (!::CryptExportKey(_pPriv->hKey, 0, PUBLICKEYBLOB, 0, _pData, &_cbPriv)
        || !::CryptExportKey(_pPub->hKey, 0, PUBLICKEYBLOB, 0, _pData + _cbPriv, &_cbPub))
    {
        return NTE_NOT_SUPPORTED;
    }

    constexpr auto kSecretDataSize = 32;
    auto _pSecret = reinterpret_cast<NcryptSecretObject*>(::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NcryptSecretObject) + kSecretDataSize));
    if (_pSecret == nullptr)
        return NTE_NO_MEMORY;

    _pSecret->Signature = kNcryptObjectSignature;
    _pSecret->Type = NcryptObjectType::Secret;
    _pSecret->cbSecret = kSecretDataSize;

    auto _Status = NcryptComputeHash(BCRYPT_SHA256_ALGORITHM, _pData, _cbMix, _pSecret->GetSecretData(), kSecretDataSize);
    if (_Status != ERROR_SUCCESS)
    {
        SecureZeroMemory(_pSecret->GetSecretData(), kSecretDataSize);
        ::HeapFree(::GetProcessHeap(), 0, _pSecret);
        return _Status;
    }

    *_phAgreedSecret = reinterpret_cast<NCRYPT_SECRET_HANDLE>(_pSecret);
    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 支持的最低服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
ncrypy,
28,
SECURITY_STATUS,
WINAPI,
NCryptDeriveKey,
    _In_        NCRYPT_SECRET_HANDLE _hSharedSecret,
    _In_        LPCWSTR              _pwszKDF,
    _In_opt_    NCryptBufferDesc* _pParameterList,
    _Out_writes_bytes_to_opt_(_cbDerivedKey, *_pcbResult) PBYTE _pbDerivedKey,
    _In_        DWORD                _cbDerivedKey,
    _Out_       DWORD* _pcbResult,
    _In_        ULONG                _dwFlags)
{
    if (const auto _pfnNCryptDeriveKey = try_get_NCryptDeriveKey())
        return _pfnNCryptDeriveKey(_hSharedSecret, _pwszKDF, _pParameterList, _pbDerivedKey, _cbDerivedKey, _pcbResult, _dwFlags);

    auto _pSecret = NcryptObjectFromHandle<NcryptSecretObject>(_hSharedSecret, NcryptObjectType::Secret);
    if (_pSecret == nullptr || _pcbResult == nullptr)
        return NTE_INVALID_PARAMETER;

    if (_dwFlags)
        return NTE_BAD_FLAGS;

    DWORD _cbHash = 32;
    const BYTE* _pLabel = nullptr;
    DWORD _cbLabel = 0;
    const BYTE* _pContext = nullptr;
    DWORD _cbContext = 0;

    if (_pParameterList)
    {
        for (ULONG i = 0; i != _pParameterList->cBuffers; ++i)
        {
            const auto& _Item = _pParameterList->pBuffers[i];
            if (_Item.BufferType == KDF_HASH_ALGORITHM)
            {
                if (_Item.pvBuffer && StringCompareIgnoreCaseByAscii(reinterpret_cast<LPCWSTR>(_Item.pvBuffer), BCRYPT_SHA1_ALGORITHM, -1) == 0)
                    _cbHash = 20;
                else
                    _cbHash = 32;
            }
            else if (_Item.BufferType == KDF_LABEL)
            {
                _pLabel = reinterpret_cast<const BYTE*>(_Item.pvBuffer);
                _cbLabel = _Item.cbBuffer;
            }
            else if (_Item.BufferType == KDF_CONTEXT)
            {
                _pContext = reinterpret_cast<const BYTE*>(_Item.pvBuffer);
                _cbContext = _Item.cbBuffer;
            }
        }
    }

    const DWORD _cbMaterialBase = _pSecret->cbSecret + _cbLabel + _cbContext;
    internal::StringBuffer<BYTE> _oBuffer;
    auto _pMaterial = _oBuffer.GetBuffer(_cbMaterialBase + 4);
    if (_pMaterial == nullptr)
        return NTE_NO_MEMORY;

    BYTE* _pWrite = _pMaterial;
    memcpy(_pWrite, _pSecret->GetSecretData(), _pSecret->cbSecret);
    _pWrite += _pSecret->cbSecret;
    if (_pLabel && _cbLabel)
    {
        memcpy(_pWrite, _pLabel, _cbLabel);
        _pWrite += _cbLabel;
    }
    if (_pContext && _cbContext)
    {
        memcpy(_pWrite, _pContext, _cbContext);
    }

    if (_pbDerivedKey == nullptr || _cbDerivedKey == 0)
    {
        *_pcbResult = _cbHash;
        return ERROR_SUCCESS;
    }

    const auto _cbNeed = _cbDerivedKey;
    *_pcbResult = _cbNeed;

    DWORD _uCounter = 1;
    DWORD _cbGenerated = 0;
    while (_cbGenerated < _cbNeed)
    {
        BYTE _Digest[32] = {};
        _pMaterial[_cbMaterialBase + 0] = (BYTE)(_uCounter >> 24);
        _pMaterial[_cbMaterialBase + 1] = (BYTE)(_uCounter >> 16);
        _pMaterial[_cbMaterialBase + 2] = (BYTE)(_uCounter >> 8);
        _pMaterial[_cbMaterialBase + 3] = (BYTE)(_uCounter);

        auto _Status = NcryptComputeHash(
            _cbHash == 20 ? BCRYPT_SHA1_ALGORITHM : BCRYPT_SHA256_ALGORITHM,
            _pMaterial,
            _cbMaterialBase + 4,
            _Digest,
            _cbHash);
        if (_Status != ERROR_SUCCESS)
        {
            return _Status;
        }

        auto _cbCopy = (_cbHash < (_cbNeed - _cbGenerated)) ? _cbHash : (_cbNeed - _cbGenerated);
        memcpy(_pbDerivedKey + _cbGenerated, _Digest, _cbCopy);
        _cbGenerated += _cbCopy;
        ++_uCounter;
    }

    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

#if defined(YY_Thunks_Implemented)
static SECURITY_STATUS WINAPI NCryptCreateProtectionDescriptorDownlevel(
    _In_            LPCWSTR _pwszDescriptorString,
    DWORD   _dwFlags,
    _Out_           NCRYPT_DESCRIPTOR_HANDLE* _phDescriptor
    )
{
    if (_phDescriptor == nullptr || _pwszDescriptorString == nullptr)
        return NTE_INVALID_PARAMETER;

    if (_dwFlags & ~NCRYPT_NAMED_DESCRIPTOR_FLAG)
        return NTE_BAD_FLAGS;

    const auto _cchDescriptorString = internal::StringLength(_pwszDescriptorString);
    const auto _cbDescriptorString = (_cchDescriptorString + 1) * sizeof(wchar_t);

    auto _pDesc = reinterpret_cast<NcryptDescriptorObject*>(internal::Alloc(sizeof(NcryptDescriptorObject) + _cbDescriptorString));
    if (_pDesc == nullptr)
        return NTE_NO_MEMORY;

    new(_pDesc) NcryptDescriptorObject();

    _pDesc->cchDescriptor = _cchDescriptorString;
    memcpy(_pDesc->GetDescriptorString(), _pwszDescriptorString, _cbDescriptorString);
    _pDesc->GetDescriptorString()[_cchDescriptorString] = L'\0';
    *_phDescriptor = reinterpret_cast<NCRYPT_DESCRIPTOR_HANDLE>(_pDesc);
    return ERROR_SUCCESS;
}
#endif // defined(YY_Thunks_Implemented)

// 最小支持客户端	Windows 8 [仅桌面应用]
// 最小支持服务器	Windows Server 2012[仅桌面应用]
__DEFINE_THUNK(
ncrypy,
12,
SECURITY_STATUS,
WINAPI,
NCryptCreateProtectionDescriptor,
    _In_            LPCWSTR _pwszDescriptorString,
                    DWORD   _dwFlags,
    _Out_           NCRYPT_DESCRIPTOR_HANDLE* _phDescriptor
    )
{
    if (const auto _pfnNCryptCreateProtectionDescriptor = try_get_NCryptCreateProtectionDescriptor())
        return _pfnNCryptCreateProtectionDescriptor(_pwszDescriptorString, _dwFlags, _phDescriptor);

    return NCryptCreateProtectionDescriptorDownlevel(_pwszDescriptorString, _dwFlags, _phDescriptor);
}
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

// 最小支持客户端	Windows 8 [仅桌面应用]
// 最小支持服务器	Windows Server 2012[仅桌面应用]
__DEFINE_THUNK(
ncrypy,
4,
SECURITY_STATUS,
WINAPI,
NCryptCloseProtectionDescriptor,
    _In_            NCRYPT_DESCRIPTOR_HANDLE _hDescriptor)
{
    if (const auto _pfnNCryptCloseProtectionDescriptor = try_get_NCryptCloseProtectionDescriptor())
        return _pfnNCryptCloseProtectionDescriptor(_hDescriptor);

    if (!_hDescriptor)
        return NTE_INVALID_HANDLE;

    auto _pObject = reinterpret_cast<NcryptObjectHeader*>(_hDescriptor);
    if (_pObject->Signature != kNcryptObjectSignature)
        return NTE_INVALID_HANDLE;

    _pObject->Release();
    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

// 最小支持客户端	Windows 8 [仅桌面应用]
// 最小支持服务器	Windows Server 2012[仅桌面应用]
__DEFINE_THUNK(
ncrypy,
16,
SECURITY_STATUS,
WINAPI,
NCryptGetProtectionDescriptorInfo,
    _In_            NCRYPT_DESCRIPTOR_HANDLE _hDescriptor,
    _In_opt_        const NCRYPT_ALLOC_PARA* _pMemPara,
                    DWORD _dwInfoType,
    _Outptr_        void** _ppvInfo)
{
    if (const auto _pfnNCryptGetProtectionDescriptorInfo = try_get_NCryptGetProtectionDescriptorInfo())
        return _pfnNCryptGetProtectionDescriptorInfo(_hDescriptor, _pMemPara, _dwInfoType, _ppvInfo);

    auto _pDesc = NcryptObjectFromHandle<NcryptDescriptorObject>(reinterpret_cast<NCRYPT_HANDLE>(_hDescriptor), NcryptObjectType::Descriptor);
    if (_pDesc == nullptr || _ppvInfo == nullptr)
        return NTE_INVALID_PARAMETER;

    if (_dwInfoType != NCRYPT_PROTECTION_INFO_TYPE_DESCRIPTOR_STRING)
        return NTE_BAD_TYPE;

    const auto _cbData = (_pDesc->cchDescriptor + 1) * sizeof(wchar_t);
    auto _szOut = reinterpret_cast<wchar_t*>(_pMemPara ? _pMemPara->pfnAlloc(_cbData) : ::LocalAlloc(LMEM_FIXED, _cbData));
    if (_szOut == nullptr)
        return NTE_NO_MEMORY;

    memcpy(_szOut, _pDesc->GetDescriptorString(), _pDesc->cchDescriptor * sizeof(wchar_t));
    _szOut[_pDesc->cchDescriptor] = L'\0';
    *_ppvInfo = _szOut;
    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

// 最小支持客户端	Windows 8 [仅桌面应用]
// 最小支持服务器	Windows Server 2012[仅桌面应用]
__DEFINE_THUNK(
ncrypy,
32,
SECURITY_STATUS,
WINAPI,
NCryptProtectSecret,
    _In_            NCRYPT_DESCRIPTOR_HANDLE _hDescriptor,
                    DWORD _dwFlags,
    _In_reads_bytes_(_cbData) const BYTE* _pbData,
                    ULONG _cbData,
    _In_opt_        const NCRYPT_ALLOC_PARA* _pMemPara,
    _In_opt_        HWND _hWnd,
    _Outptr_result_bytebuffer_(*_pcbProtectedBlob) BYTE** _ppbProtectedBlob,
    _Out_           ULONG* _pcbProtectedBlob)
{
    if (const auto _pfnNCryptProtectSecret = try_get_NCryptProtectSecret())
        return _pfnNCryptProtectSecret(_hDescriptor, _dwFlags, _pbData, _cbData, _pMemPara, _hWnd, _ppbProtectedBlob, _pcbProtectedBlob);

    UNREFERENCED_PARAMETER(_hWnd);

    auto _pDesc = NcryptObjectFromHandle<NcryptDescriptorObject>(reinterpret_cast<NCRYPT_HANDLE>(_hDescriptor), NcryptObjectType::Descriptor);
    if (_pDesc == nullptr || _ppbProtectedBlob == nullptr || _pcbProtectedBlob == nullptr || (_pbData == nullptr && _cbData))
        return NTE_INVALID_PARAMETER;

    DATA_BLOB _oIn = {};
    _oIn.pbData = const_cast<BYTE*>(_pbData);
    _oIn.cbData = _cbData;

    DWORD _fCryptProtectFlags = 0;
    if (StringCompareIgnoreCaseByAscii(_pDesc->GetDescriptorString(), L"LOCAL=user", -1) == 0)
    {
        // 保护数据仅限于当前用户。
    }
    else
    {
        // 其他情况，数据将对同一台计算机上的所有用户可用，凑合吧。
        // LOCAL=machine
        // SID=S-1-5-21-4392301 AND SID=S-1-5-21-3101812
        // SDDL=O:S-1-5-5-0-290724G:SYD:(A;;CCDC;;;S-1-5-5-0-290724)(A;;DC;;;WD)
        // WEBCREDENTIALS=MyPasswordName
        // WEBCREDENTIALS=MyPasswordName,myweb.com
        _fCryptProtectFlags |= CRYPTPROTECT_LOCAL_MACHINE;
    }

    if (_dwFlags & NCRYPT_SILENT_FLAG)
        _fCryptProtectFlags |= CRYPTPROTECT_UI_FORBIDDEN;

    DATA_BLOB _oDpapi = {};
    if (!::CryptProtectData(&_oIn, nullptr, nullptr, nullptr, nullptr, _fCryptProtectFlags, &_oDpapi))
        return NTE_FAIL;

    const auto _cchDesc = _pDesc->cchDescriptor + 1;
    const auto _cbDesc = _cchDesc * sizeof(wchar_t);
    const auto _cbTotal = sizeof(NcryptProtectedBlobHeader) + _cbDesc + _oDpapi.cbData;

    auto _pbOut = reinterpret_cast<BYTE*>(_pMemPara ? _pMemPara->pfnAlloc(_cbTotal) : ::LocalAlloc(LMEM_FIXED, _cbTotal));
    if (_pbOut == nullptr)
    {
        ::LocalFree(_oDpapi.pbData);
        return NTE_NO_MEMORY;
    }

    auto _pHeader = reinterpret_cast<NcryptProtectedBlobHeader*>(_pbOut);
    _pHeader->Magic = kNcryptProtectedBlobMagic;
    _pHeader->cbDescriptorChars = _cchDesc;
    _pHeader->cbProtected = _oDpapi.cbData;

    memcpy(_pbOut + sizeof(NcryptProtectedBlobHeader), _pDesc->GetDescriptorString(), _cbDesc);
    memcpy(_pbOut + sizeof(NcryptProtectedBlobHeader) + _cbDesc, _oDpapi.pbData, _oDpapi.cbData);
    ::LocalFree(_oDpapi.pbData);

    *_ppbProtectedBlob = _pbOut;
    *_pcbProtectedBlob = _cbTotal;
    return ERROR_SUCCESS;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

// 最小支持客户端	Windows 8 [仅桌面应用]
// 最小支持服务器	Windows Server 2012[仅桌面应用]
__DEFINE_THUNK(
ncrypy,
32,
SECURITY_STATUS,
WINAPI,
NCryptUnprotectSecret,
    _Out_opt_       NCRYPT_DESCRIPTOR_HANDLE* _phDescriptor,
    _In_            DWORD _dwFlags,
    _In_reads_bytes_(_cbProtectedBlob) const BYTE* _pbProtectedBlob,
                    ULONG _cbProtectedBlob,
    _In_opt_        const NCRYPT_ALLOC_PARA* _pMemPara,
    _In_opt_        HWND _hWnd,
    _Outptr_result_bytebuffer_(*_pcbData) BYTE** _ppbData,
    _Out_           ULONG* _pcbData)
{
    if (const auto _pfnNCryptUnprotectSecret = try_get_NCryptUnprotectSecret())
        return _pfnNCryptUnprotectSecret(_phDescriptor, _dwFlags, _pbProtectedBlob, _cbProtectedBlob, _pMemPara, _hWnd, _ppbData, _pcbData);

    UNREFERENCED_PARAMETER(_hWnd);

    if (_ppbData == nullptr || _pcbData == nullptr || _pbProtectedBlob == nullptr || _cbProtectedBlob < sizeof(NcryptProtectedBlobHeader))
        return NTE_INVALID_PARAMETER;

    auto _pHeader = reinterpret_cast<const NcryptProtectedBlobHeader*>(_pbProtectedBlob);
    if (_pHeader->Magic != kNcryptProtectedBlobMagic)
        return NTE_INVALID_PARAMETER;

    const auto _cbDesc = _pHeader->cbDescriptorChars;
    const auto _cbDescBytes = _cbDesc * sizeof(wchar_t);
    const auto _cbHead = sizeof(NcryptProtectedBlobHeader) + _cbDescBytes;
    if (_cbDesc == 0 || _cbProtectedBlob < _cbHead || (_cbProtectedBlob - _cbHead) != _pHeader->cbProtected)
        return NTE_INVALID_PARAMETER;

    if (_phDescriptor)
    {
        auto _szDesc = reinterpret_cast<LPCWSTR>(_pbProtectedBlob + sizeof(NcryptProtectedBlobHeader));
        auto _Status = NCryptCreateProtectionDescriptorDownlevel(_szDesc, 0, _phDescriptor);
        if (_Status != ERROR_SUCCESS)
            return _Status;
    }

    if (_dwFlags & NCRYPT_UNPROTECT_NO_DECRYPT)
    {
        *_ppbData = nullptr;
        *_pcbData = 0;
        return ERROR_SUCCESS;
    }

    DATA_BLOB _oIn = {};
    _oIn.pbData = const_cast<BYTE*>(_pbProtectedBlob + _cbHead);
    _oIn.cbData = _pHeader->cbProtected;

    DWORD _fCryptProtectFlags = 0;
    if (_dwFlags & NCRYPT_SILENT_FLAG)
        _fCryptProtectFlags |= CRYPTPROTECT_UI_FORBIDDEN;

    DATA_BLOB _oOut = {};
    if (!::CryptUnprotectData(&_oIn, nullptr, nullptr, nullptr, nullptr, _fCryptProtectFlags, &_oOut))
    {
        return NTE_FAIL;
    }

    if (_pMemPara)
    {
        auto _pbOut = reinterpret_cast<BYTE*>(_pMemPara->pfnAlloc(_oOut.cbData));
        if (!_pbOut)
        {
            ::LocalFree(_oOut.pbData);
            return NTE_NO_MEMORY;
        }

        memcpy(_pbOut, _oOut.pbData, _oOut.cbData);
        ::LocalFree(_oOut.pbData);

        *_ppbData = _pbOut;
        *_pcbData = _oOut.cbData;
    }
    else
    {
        *_ppbData = _oOut.pbData;
        *_pcbData = _oOut.cbData;
    }
    
    return ERROR_SUCCESS;
}
#endif
}
