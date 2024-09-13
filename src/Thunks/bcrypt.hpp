#if (YY_Thunks_Target < __WindowsNT6_1)
#include <bcrypt.h>
#include <wincrypt.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_advapi32)
#define __Comment_Lib_advapi32
#pragma comment(lib, "Advapi32.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6_1) && (YY_Thunks_Target >= __WindowsNT6 || __YY_Thunks_libs) && !defined(__Comment_Lib_bcrypt)
#define __Comment_Lib_bcrypt
#pragma comment(lib, "bcrypt.lib")
#endif

namespace YY::Thunks
{
#if defined(YY_Thunks_Implemented) && YY_Thunks_Target < __WindowsNT6
    namespace
    {
        struct BCryptHash;
        struct BCryptAlgorithm;
        struct BCryptKey;
        struct BCryptMapItem;
        typedef NTSTATUS(__fastcall* OpenAlgorithmProviderType)(
            _In_     const BCryptMapItem* _pCryptMapItem,
            _In_     ULONG              _fFlags,
            _Outptr_ BCryptAlgorithm** _ppAlgorithm);

        struct BCryptMapItem
        {
            LPCWSTR szProvider;
            LPCWSTR szAlgName;
            DWORD cbAlgId;
            DWORD uProvType;
            DWORD uAlgId;
            OpenAlgorithmProviderType pfnOpenAlgorithmProviderType;

            template<DWORD _cchAlgId>
            constexpr BCryptMapItem(const wchar_t (&_szAlgName)[_cchAlgId], LPCWSTR _szProvider, DWORD _uProvType, DWORD _uAlgId, OpenAlgorithmProviderType _pOpenAlgorithmProvider)
                : szProvider(_szProvider)
                , szAlgName(_szAlgName)
                , cbAlgId(sizeof(_szAlgName))
                , uProvType(_uProvType)
                , uAlgId(_uAlgId)
                , pfnOpenAlgorithmProviderType(_pOpenAlgorithmProvider)
            {
            }
        };

        enum class BCryptObjectType
        {
            None,
            Algorithm,
            Hash,
            Key,
        };

        class BCryptObject : public internal::CppAlloc
        {
            static constexpr auto kBCryptObjectMagic = 0x998u;

            DWORD uMagic = kBCryptObjectMagic;
            ULONG uRef = 1u;

        public:
            BCryptObjectType eType = BCryptObjectType::None;
            bool bCanFree = true;

            BCryptObject(BCryptObjectType _eType)
                : eType(_eType)
            {
            }

            virtual ~BCryptObject()
            {
                // 故意修改，便于 IsBCrypyAlgHandle 时判断有效性。
                uMagic = 0;
            }

            bool IsValid() const
            {
                return uMagic == kBCryptObjectMagic;
            }

            BCryptObjectType GetClass() const
            {
                return eType;
            }

            bool IsAlgorithm() const
            {
                return eType == BCryptObjectType::Algorithm;
            }

            bool IsHash() const
            {
                return eType == BCryptObjectType::Hash;
            }
            
            bool IsKey()
            {
                return eType == BCryptObjectType::Key;
            }

            void AddRef()
            {
                InterlockedIncrement(&uRef);
            }

            void Release()
            {
                if (InterlockedDecrement(&uRef) == 0)
                {
                    this->~BCryptObject();
                    if (bCanFree)
                    {
                        const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
                        HeapFree(_hProcessHeap, 0, this);
                    }
                }
            }

            virtual NTSTATUS WINAPI GetProperty(
                _In_z_                                      LPCWSTR pszProperty,
                _Out_writes_bytes_to_opt_(cbOutput, *pcbResult) PUCHAR   pbOutput,
                _In_                                        ULONG   cbOutput,
                _Out_                                       ULONG* pcbResult,
                _In_                                        ULONG   dwFlags
                ) = 0;

            virtual NTSTATUS WINAPI SetProperty(
                _In_z_                  LPCWSTR _szProperty,
                _In_reads_bytes_(_cbInput)    PUCHAR   _pInput,
                _In_                    ULONG   _cbInput,
                _In_                    ULONG   _fFlags) = 0;
        };

        struct BCryptAlgorithm : public BCryptObject
        {
            const BCryptMapItem* pMapItem = nullptr;
            HCRYPTPROV hProv = NULL;
            ULONG fOpenAlgorithmFlags = 0;
            DWORD uCryptMode = 0;
            DWORD uEffectiveKeyBitCount = 0;

            BCryptAlgorithm()
                : BCryptObject(BCryptObjectType::Algorithm)
            {
            }

            ~BCryptAlgorithm()
            {
                if (hProv)
                    CryptReleaseContext(hProv, 0);
            }

            bool IsRng() const
            {
                return pMapItem->uAlgId == 0;
            }

            bool CanCreateHash() const
            {
                return GET_ALG_CLASS(pMapItem->uAlgId) == ALG_CLASS_HASH;
            }

            DWORD __fastcall GetHashLength() const
            {
                constexpr auto kByteBits = 32 / sizeof(UINT32);

                switch (pMapItem->uAlgId)
                {
                case CALG_MD2:
                    return 32 / kByteBits;
                    break;
                case CALG_MD4:
                    return 128 / kByteBits;
                    break;
                case CALG_MD5:
                    return 128 / kByteBits;
                    break;
                case CALG_SHA1:
                    return 160 / kByteBits;
                    break;
                case CALG_SHA_256:
                    return 256 / kByteBits;
                    break;
                case CALG_SHA_384:
                    return 384 / kByteBits;
                    break;
                case CALG_SHA_512:
                    return 512 / kByteBits;
                    break;
                default:
                    return 0;
                    break;
                }
            }
            
            DWORD __fastcall GetHashBlockLength() const
            {
                constexpr auto kByteBits = 32 / sizeof(UINT32);

                switch (pMapItem->uAlgId)
                {
                case CALG_MD2:
                    return 128 / kByteBits;
                    break;
                case CALG_MD4:
                    return 256 / kByteBits;
                    break;
                case CALG_MD5:
                    return 512 / kByteBits;
                    break;
                case CALG_SHA1:
                    return 512 / kByteBits;
                    break;
                case CALG_SHA_256:
                    return 512 / kByteBits;
                    break;
                case CALG_SHA_384:
                    return 1088 / kByteBits;
                    break;
                case CALG_SHA_512:
                    return 1024 / kByteBits;
                    break;
                default:
                    return 0;
                    break;
                }
            }

            static NTSTATUS __fastcall CryptoErrorToNTStatus(LSTATUS _lCryptoError) noexcept
            {
                switch (_lCryptoError)
                {
                case NTE_BAD_DATA:
                    return STATUS_INVALID_BUFFER_SIZE;
                case ERROR_INVALID_HANDLE:
                case NTE_BAD_HASH:
                    return STATUS_INVALID_HANDLE;
                case NTE_BAD_ALGID:
                    return STATUS_NOT_SUPPORTED;
                case NTE_NO_MEMORY:
                    return STATUS_NO_MEMORY;
                case ERROR_MORE_DATA:
                    return STATUS_BUFFER_TOO_SMALL;
                default:
                    return STATUS_INVALID_PARAMETER;
                }
            }

            NTSTATUS WINAPI GetProperty(
                _In_z_                                      LPCWSTR pszProperty,
                _Out_writes_bytes_to_opt_(cbOutput, *pcbResult) PUCHAR   pbOutput,
                _In_                                        ULONG   cbOutput,
                _Out_                                       ULONG* pcbResult,
                _In_                                        ULONG   dwFlags
                ) override
            {
                if (StringCompareIgnoreCaseByAscii(BCRYPT_ALGORITHM_NAME, pszProperty, -1) == 0)
                {
                    *pcbResult = pMapItem->cbAlgId;
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < pMapItem->cbAlgId)
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    memcpy(pbOutput, pMapItem->szAlgName, pMapItem->cbAlgId);
                    return STATUS_SUCCESS;
                }
                else if (StringCompareIgnoreCaseByAscii(BCRYPT_PROVIDER_HANDLE, pszProperty, -1) == 0)
                {
                    *pcbResult = sizeof(BCRYPT_ALG_HANDLE);
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < sizeof(BCRYPT_ALG_HANDLE))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    *reinterpret_cast<BCRYPT_ALG_HANDLE*>(pbOutput) = this;
                    return STATUS_SUCCESS;
                }

                return STATUS_NOT_SUPPORTED;
            }

            NTSTATUS WINAPI SetProperty(
                _In_z_                  LPCWSTR _szProperty,
                _In_reads_bytes_(_cbInput)    PUCHAR   _pInput,
                _In_                    ULONG   _cbInput,
                _In_                    ULONG   _fFlags) override
            {
                return STATUS_NOT_SUPPORTED;
            }

            virtual NTSTATUS WINAPI CreateHash(
                _Outptr_                                 BCryptHash** ppHash,
                _Out_writes_bytes_all_opt_(cbHashObject) PUCHAR   pbHashObject,
                _In_                                     ULONG   cbHashObject,
                _In_reads_bytes_opt_(cbSecret)           PUCHAR   pbSecret,   // optional
                _In_                                     ULONG   cbSecret,   // optional
                _In_                                     ULONG   dwFlags)
            {
                return STATUS_NOT_SUPPORTED;
            }

            virtual NTSTATUS WINAPI GenerateSymmetricKey(
                _Out_                               BCryptKey**_ppKey,
                _Out_writes_bytes_all_opt_(_cbKeyObject)  PUCHAR   _pbKeyObject,
                _In_                                ULONG   _cbKeyObject,
                _In_reads_bytes_(_cbSecret)               PUCHAR   _pbSecret,
                _In_                                ULONG   _cbSecret,
                _In_                                ULONG   _fFlags)
            {
                return STATUS_NOT_SUPPORTED;
            }

            virtual NTSTATUS WINAPI ImportKey(
                _In_opt_                            BCryptKey* _pImportKey,
                _In_z_                              LPCWSTR _szBlobType,
                _Out_                               BCryptKey** _ppKey,
                _Out_writes_bytes_all_opt_(_cbKeyObject)  PUCHAR   _pKeyObject,
                _In_                                ULONG   _cbKeyObject,
                _In_reads_bytes_(_cbInput)                PUCHAR   _pInput,
                _In_                                ULONG   _cbInput,
                _In_                                ULONG   _fFlags
                )
            {
                return STATUS_NOT_SUPPORTED;
            }
        };

        template<typename BCryptAlgorithmType>
        struct BCryptAlgorithmByCryptoAPI : BCryptAlgorithm
        {
            static NTSTATUS __fastcall Create(_In_ const BCryptMapItem* _pMapItem, _In_ ULONG _fOpenAlgorithmFlags, _Outptr_ BCryptAlgorithm** _ppAlgorithm)
            {
                *_ppAlgorithm = nullptr;

                HCRYPTPROV _hProv;
                if (!CryptAcquireContextW(&_hProv, nullptr, _pMapItem->szProvider, _pMapItem->uProvType, CRYPT_VERIFYCONTEXT))
                    return STATUS_INVALID_PARAMETER;

                const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
                auto _pBCryptAlgorithm = (BCryptAlgorithmType*)HeapAlloc(_hProcessHeap, 0, sizeof(BCryptAlgorithmType));
                if (!_pBCryptAlgorithm)
                {
                    CryptReleaseContext(_hProv, 0);
                    return STATUS_NO_MEMORY;
                }

                new (_pBCryptAlgorithm) BCryptAlgorithmType();
                _pBCryptAlgorithm->pMapItem = _pMapItem;
                _pBCryptAlgorithm->hProv = _hProv;
                _pBCryptAlgorithm->fOpenAlgorithmFlags = _fOpenAlgorithmFlags;
                *_ppAlgorithm = _pBCryptAlgorithm;
                return STATUS_SUCCESS;
            }
        };

        struct BCryptHash : public BCryptObject
        {
            BCryptAlgorithm* pAlgorithm = nullptr;
            ULONG dwFlags = 0;
            HCRYPTKEY hPubKey = NULL;
            HCRYPTHASH hHash = NULL;

            BCryptHash(_In_ BCryptAlgorithm* _pAlgorithm)
                : BCryptObject(BCryptObjectType::Hash)
                , pAlgorithm(_pAlgorithm)
            {
                pAlgorithm->AddRef();
            }

            ~BCryptHash()
            {
                if(hPubKey)
                    CryptDestroyKey(hPubKey);

                if(hHash)
                    CryptDestroyHash(hHash);

                if (pAlgorithm)
                    pAlgorithm->Release();
            }

            NTSTATUS WINAPI Init(
                _In_reads_bytes_opt_(_cbSecret) PUCHAR _pbSecret,   // optional
                _In_                            ULONG  _cbSecret,   // optional
                _In_                            ULONG  _dwFlags)
            {
                if (pAlgorithm->fOpenAlgorithmFlags & BCRYPT_ALG_HANDLE_HMAC_FLAG)
                {
                    // https://learn.microsoft.com/zh-cn/windows/win32/seccrypto/example-c-program--importing-a-plaintext-key
                    struct _PLAINTEXTKEYBLOB : public BLOBHEADER
                    {
                        DWORD dwKeySize;
                        BYTE rgbKeyData[2];
                    };

                    const DWORD _cbKeyBlob = sizeof(_PLAINTEXTKEYBLOB) + _cbSecret;
                    auto _pKeyBlob = static_cast<_PLAINTEXTKEYBLOB*>(_malloca(_cbKeyBlob));
                    if (!_pKeyBlob)
                    {
                        return STATUS_NO_MEMORY;
                    }
                    _pKeyBlob->bType = PLAINTEXTKEYBLOB;
                    _pKeyBlob->bVersion = CUR_BLOB_VERSION;
                    _pKeyBlob->reserved = 0;
                    _pKeyBlob->aiKeyAlg = CALG_RC2;
                    memcpy(_pKeyBlob->rgbKeyData, _pbSecret, _cbSecret);
                    if (_cbSecret >= 2)
                    {
                        _pKeyBlob->dwKeySize = _cbSecret;
                    }
                    else
                    {
                        // 长度小于 2字节时 CryptImportKey 会失败，特殊处理一下
                        _pKeyBlob->dwKeySize = 2;
                        if (_cbSecret == 0)
                        {
                            _pKeyBlob->rgbKeyData[0] = 0;
                        }
                        _pKeyBlob->rgbKeyData[1] = 0;
                    }

                    auto _bResult = CryptImportKey(pAlgorithm->hProv, reinterpret_cast<BYTE*>(_pKeyBlob), _cbKeyBlob, NULL, CRYPT_IPSEC_HMAC_KEY, &hPubKey);
                    // 避免密钥泄漏，所以立即将内存值清空！！！
                    memset(_pKeyBlob, 0, _cbKeyBlob);
                    _freea(_pKeyBlob);

                    if (!_bResult)
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    if (!CryptCreateHash(pAlgorithm->hProv, CALG_HMAC, hPubKey, 0, &hHash))
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    HMAC_INFO _HMacInfo = { pAlgorithm->pMapItem->uAlgId };
                    if (!CryptSetHashParam(hHash, HP_HMAC_INFO, reinterpret_cast<BYTE*>(&_HMacInfo), 0))
                    {
                        return STATUS_INVALID_PARAMETER;
                    }
                }
                else
                {
                    if (!CryptCreateHash(pAlgorithm->hProv, pAlgorithm->pMapItem->uAlgId, NULL, 0, &hHash))
                    {
                        return STATUS_INVALID_PARAMETER;
                    }
                }

                dwFlags = _dwFlags;
                return STATUS_SUCCESS;
            }

            NTSTATUS
            WINAPI
            HashData(
                _In_reads_bytes_(cbInput)    PUCHAR   pbInput,
                _In_                    ULONG   cbInput,
                _In_                    ULONG   dwFlags)
            {
                if (!CryptHashData(hHash, pbInput, cbInput, 0))
                    return STATUS_INVALID_PARAMETER;
                return STATUS_SUCCESS;
            }

            NTSTATUS
            WINAPI
            FinishHash(
                _Out_writes_bytes_all_(cbOutput) PUCHAR   pbOutput,
                _In_                        ULONG   cbOutput,
                _In_                        ULONG   dwFlags)
            {
                const auto _cbTargetHashLength = pAlgorithm->GetHashLength();
                if (cbOutput < _cbTargetHashLength)
                {
                    return STATUS_BUFFER_TOO_SMALL;
                }
                else if (cbOutput != _cbTargetHashLength)
                {
                    return STATUS_INVALID_PARAMETER;
                }

                if (!CryptGetHashParam(hHash, HP_HASHVAL, pbOutput, &cbOutput, 0))
                    return STATUS_INVALID_PARAMETER;

                return STATUS_SUCCESS;
            }

            NTSTATUS WINAPI GetProperty(
                _In_z_                                      LPCWSTR pszProperty,
                _Out_writes_bytes_to_opt_(cbOutput, *pcbResult) PUCHAR   pbOutput,
                _In_                                        ULONG   cbOutput,
                _Out_                                       ULONG* pcbResult,
                _In_                                        ULONG   dwFlags
                ) override
            {
                if (StringCompareIgnoreCaseByAscii(BCRYPT_HASH_LENGTH, pszProperty, -1) == 0)
                {
                    *pcbResult = sizeof(DWORD);
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < sizeof(DWORD))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    *reinterpret_cast<DWORD*>(pbOutput) = pAlgorithm->GetHashLength();
                    return STATUS_SUCCESS;
                }
                else if (StringCompareIgnoreCaseByAscii(BCRYPT_ALGORITHM_NAME, pszProperty, -1) == 0)
                {
                    *pcbResult = pAlgorithm->pMapItem->cbAlgId;
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < pAlgorithm->pMapItem->cbAlgId)
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    memcpy(pbOutput, pAlgorithm->pMapItem->szAlgName, pAlgorithm->pMapItem->cbAlgId);
                    return STATUS_SUCCESS;
                }
                else if (StringCompareIgnoreCaseByAscii(BCRYPT_PROVIDER_HANDLE, pszProperty, -1) == 0)
                {
                    *pcbResult = sizeof(BCRYPT_ALG_HANDLE);
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < sizeof(BCRYPT_ALG_HANDLE))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    *reinterpret_cast<BCRYPT_ALG_HANDLE*>(pbOutput) = pAlgorithm;
                    return STATUS_SUCCESS;
                }

                return STATUS_NOT_SUPPORTED;
            }

            NTSTATUS WINAPI SetProperty(
                _In_z_                  LPCWSTR _szProperty,
                _In_reads_bytes_(_cbInput)    PUCHAR   _pInput,
                _In_                    ULONG   _cbInput,
                _In_                    ULONG   _fFlags) override
            {
                return STATUS_NOT_SUPPORTED;
            }
        };

        struct BCryptHashAlgorithm : public BCryptAlgorithmByCryptoAPI<BCryptHashAlgorithm>
        {
            NTSTATUS WINAPI GetProperty(
                _In_z_                                      LPCWSTR pszProperty,
                _Out_writes_bytes_to_opt_(cbOutput, *pcbResult) PUCHAR   pbOutput,
                _In_                                        ULONG   cbOutput,
                _Out_                                       ULONG* pcbResult,
                _In_                                        ULONG   dwFlags
                ) override
            {
                if (StringCompareIgnoreCaseByAscii(BCRYPT_OBJECT_LENGTH, pszProperty, -1) == 0)
                {
                    *pcbResult = sizeof(DWORD);
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < sizeof(DWORD))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    *reinterpret_cast<DWORD*>(pbOutput) = sizeof(BCryptHash);
                    return STATUS_SUCCESS;
                }
                else if (StringCompareIgnoreCaseByAscii(BCRYPT_HASH_LENGTH, pszProperty, -1) == 0)
                {
                    *pcbResult = sizeof(DWORD);
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < sizeof(DWORD))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    *reinterpret_cast<DWORD*>(pbOutput) = GetHashLength();
                    return STATUS_SUCCESS;
                }
                else if (StringCompareIgnoreCaseByAscii(BCRYPT_HASH_BLOCK_LENGTH, pszProperty, -1) == 0)
                {
                    *pcbResult = sizeof(DWORD);
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < sizeof(DWORD))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    *reinterpret_cast<DWORD*>(pbOutput) = GetHashBlockLength();
                    return STATUS_SUCCESS;
                }

                return BCryptAlgorithm::GetProperty(pszProperty, pbOutput, cbOutput, pcbResult, dwFlags); 
            }


            NTSTATUS
            WINAPI
            CreateHash(
                _Outptr_                                 BCryptHash** ppHash,
                _Out_writes_bytes_all_opt_(cbHashObject) PUCHAR   pbHashObject,
                _In_                                     ULONG   cbHashObject,
                _In_reads_bytes_opt_(cbSecret)           PUCHAR   pbSecret,   // optional
                _In_                                     ULONG   cbSecret,   // optional
                _In_                                     ULONG   dwFlags)
            {
                BCryptHash* _pBCryptHash = reinterpret_cast<BCryptHash*>(pbHashObject);
                if (_pBCryptHash)
                {
                    if (cbHashObject < sizeof(BCryptHash))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    new (_pBCryptHash) BCryptHash(this);
                    _pBCryptHash->bCanFree = false;
                }
                else
                {
                    if (cbHashObject != 0)
                    {
                        return STATUS_INVALID_PARAMETER;
                    }
                    const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
                    _pBCryptHash = (BCryptHash*)HeapAlloc(_hProcessHeap, 0, sizeof(BCryptHash));
                    if (!_pBCryptHash)
                    {
                        return STATUS_NO_MEMORY;
                    }

                    new (_pBCryptHash) BCryptHash(this);
                }

                auto _Status = _pBCryptHash->Init(pbSecret, cbSecret, dwFlags);
                if (_Status)
                {
                    _pBCryptHash->Release();
                    return _Status;
                }
                *ppHash = _pBCryptHash;
                return STATUS_SUCCESS;
            }
        };

        struct BCryptRngAlgorithm : public BCryptAlgorithm
        {
            static NTSTATUS __fastcall Create(_In_ const BCryptMapItem* _pMapItem, _In_ ULONG _fOpenAlgorithmFlags, _Outptr_ BCryptAlgorithm** _ppAlgorithm)
            {
                *_ppAlgorithm = nullptr;

                const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
                auto _pBCryptAlgorithm = (BCryptRngAlgorithm*)HeapAlloc(_hProcessHeap, 0, sizeof(BCryptRngAlgorithm));
                if (!_pBCryptAlgorithm)
                {
                    return STATUS_NO_MEMORY;
                }

                new (_pBCryptAlgorithm) BCryptRngAlgorithm();
                _pBCryptAlgorithm->pMapItem = _pMapItem;
                _pBCryptAlgorithm->fOpenAlgorithmFlags = _fOpenAlgorithmFlags;
                *_ppAlgorithm = _pBCryptAlgorithm;
                return STATUS_SUCCESS;
            }
        };
        
        struct BCryptKey : public BCryptObject
        {
            static constexpr unsigned kMaxBlockLength = 16'384 / 8;

            BCryptAlgorithm* pAlgorithm = nullptr;
            HCRYPTKEY hKey = NULL;
            ULONG dwFlags = 0;

            BCryptKey(_In_ BCryptAlgorithm* _pAlgorithm)
                : BCryptObject(BCryptObjectType::Key)
                , pAlgorithm(_pAlgorithm)
            {
                pAlgorithm->AddRef();
            }

            ~BCryptKey()
            {
                if (hKey)
                    CryptDestroyKey(hKey);

                if (pAlgorithm)
                    pAlgorithm->Release();
            }

            NTSTATUS WINAPI Init(
                _In_reads_bytes_opt_(_cbSecret) PUCHAR _pbSecret,   // optional
                _In_                            ULONG  _cbSecret,   // optional
                _In_                            ULONG  _fFlags)
            {
                if (_pbSecret == nullptr || _fFlags)
                {
                    return STATUS_INVALID_PARAMETER;
                }

                dwFlags = _fFlags;
                // https://learn.microsoft.com/zh-cn/windows/win32/seccrypto/example-c-program--importing-a-plaintext-key
                struct _PLAINTEXTKEYBLOB : public BLOBHEADER
                {
                    DWORD dwKeySize;
                    BYTE rgbKeyData[2048];
                };

                _PLAINTEXTKEYBLOB _KeyBlob;
                _KeyBlob.bType = PLAINTEXTKEYBLOB;
                _KeyBlob.bVersion = CUR_BLOB_VERSION;
                _KeyBlob.reserved = 0;
                if (pAlgorithm->pMapItem->uAlgId == CALG_AES)
                {
                    switch (_cbSecret)
                    {
                    case 128 / 8:
                        _KeyBlob.aiKeyAlg = CALG_AES_128;
                        break;
                    case 192 / 8:
                        _KeyBlob.aiKeyAlg = CALG_AES_192;
                        break;
                    case 256 / 8:
                        _KeyBlob.aiKeyAlg = CALG_AES_256;
                        break;
                    default:
                        return STATUS_INVALID_PARAMETER;
                    }
                }
                else if (pAlgorithm->pMapItem->uAlgId == CALG_DES)
                {
                    // DES密钥长度只有64位
                    if (_cbSecret >= 64 / 8)
                    {
                        _cbSecret = 64 / 8;
                    }
                    else
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    _KeyBlob.aiKeyAlg = CALG_DES;
                }
                else if (pAlgorithm->pMapItem->uAlgId == CALG_3DES)
                {
                    if (_cbSecret >= 192 / 8)
                    {
                        _cbSecret = 192 / 8;
                    }
                    else
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    _KeyBlob.aiKeyAlg = CALG_3DES;
                }
                else if (pAlgorithm->pMapItem->uAlgId == CALG_3DES_112)
                {
                    if (_cbSecret >= 128 / 8)
                    {
                        _cbSecret = 128 / 8;
                    }
                    else
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    _KeyBlob.aiKeyAlg = CALG_3DES_112;
                }
                else if (pAlgorithm->pMapItem->uAlgId == CALG_RC2)
                {
                    // RC2密钥长度最大128bit
                    if (_cbSecret < 1)
                    {
                        return STATUS_INVALID_PARAMETER;
                    }
                    else if (_cbSecret > 16)
                    {
                        _cbSecret = 16;
                    }

                    _KeyBlob.aiKeyAlg = CALG_RC2;
                }
                else if (pAlgorithm->pMapItem->uAlgId == CALG_RC4)
                {
                    if (_cbSecret < 1)
                    {
                        return STATUS_INVALID_PARAMETER;
                    }
                    else if (_cbSecret > 16)
                    {
                        _cbSecret = 16;
                    }
                    _KeyBlob.aiKeyAlg = CALG_RC4;
                }
                else
                {
                    return STATUS_NOT_SUPPORTED;
                }
                _KeyBlob.dwKeySize = _cbSecret;
                memcpy(_KeyBlob.rgbKeyData, _pbSecret, _cbSecret);

                auto _bResult = CryptImportKey(pAlgorithm->hProv, reinterpret_cast<BYTE*>(&_KeyBlob), sizeof(BLOBHEADER) + sizeof(DWORD) + _cbSecret, NULL, CRYPT_EXPORTABLE, &hKey);
                // 避免密钥泄漏，所以立即将内存值清空！！！
                memset(&_KeyBlob, 0, sizeof(_KeyBlob));

                if (!_bResult)
                {
                    return STATUS_INVALID_PARAMETER;
                }

                if (auto _uCryptMode = pAlgorithm->uCryptMode)
                {
                    if (!CryptSetKeyParam(hKey, KP_MODE, (const BYTE*)&_uCryptMode, 0))
                    {
                        return STATUS_INVALID_PARAMETER;
                    }
                }

                if (auto _uEffectiveKeyBitCount = pAlgorithm->uEffectiveKeyBitCount)
                {
                    if (!CryptSetKeyParam(hKey, KP_EFFECTIVE_KEYLEN, (const BYTE*)&_uEffectiveKeyBitCount, 0))
                    {
                        return STATUS_INVALID_PARAMETER;
                    }
                }

                return STATUS_SUCCESS;
            }


            DWORD __fastcall GetBlockLength() const
            {
                DWORD _cbBlockBitLength = 0;
                DWORD _cbData = sizeof(_cbBlockBitLength);
                if (!CryptGetKeyParam(hKey, KP_BLOCKLEN, (BYTE*)&_cbBlockBitLength, &_cbData, 0))
                {
                    return 0;
                }

                return max(_cbBlockBitLength / 8, 1);
            }
            
            NTSTATUS WINAPI GetProperty(
                _In_z_                                      LPCWSTR pszProperty,
                _Out_writes_bytes_to_opt_(cbOutput, *pcbResult) PUCHAR   pbOutput,
                _In_                                        ULONG   cbOutput,
                _Out_                                       ULONG* pcbResult,
                _In_                                        ULONG   dwFlags
                ) override
            {
                if (StringCompareIgnoreCaseByAscii(BCRYPT_BLOCK_LENGTH, pszProperty, -1) == 0)
                {
                    *pcbResult = sizeof(DWORD);
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < sizeof(DWORD))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    const auto _cbBlockLength = GetBlockLength();
                    if (!_cbBlockLength)
                    {
                        return STATUS_NOT_SUPPORTED;
                    }
                    *reinterpret_cast<DWORD*>(pbOutput) = _cbBlockLength;
                    return STATUS_SUCCESS;
                }
                else if (StringCompareIgnoreCaseByAscii(BCRYPT_ALGORITHM_NAME, pszProperty, -1) == 0)
                {
                    *pcbResult = pAlgorithm->pMapItem->cbAlgId;
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < pAlgorithm->pMapItem->cbAlgId)
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    memcpy(pbOutput, pAlgorithm->pMapItem->szAlgName, pAlgorithm->pMapItem->cbAlgId);
                    return STATUS_SUCCESS;
                }
                else if (StringCompareIgnoreCaseByAscii(BCRYPT_PROVIDER_HANDLE, pszProperty, -1) == 0)
                {
                    *pcbResult = sizeof(BCRYPT_ALG_HANDLE);
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < sizeof(BCRYPT_ALG_HANDLE))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    *reinterpret_cast<BCRYPT_ALG_HANDLE*>(pbOutput) = pAlgorithm;
                    return STATUS_SUCCESS;
                }
                else if (StringCompareIgnoreCaseByAscii(BCRYPT_CHAINING_MODE, pszProperty, -1) == 0)
                {
                    DWORD _uCryptMode = 0;
                    DWORD _cbResult = sizeof(_uCryptMode);
                    if (!CryptGetKeyParam(hKey, KP_MODE, (BYTE*)&_uCryptMode, &_cbResult, 0))
                    {
                        return STATUS_NOT_SUPPORTED;
                    }

                    switch (_uCryptMode)
                    {
                    case CRYPT_MODE_CBC:
                        *pcbResult = sizeof(BCRYPT_CHAIN_MODE_CBC);
                        if (!pbOutput)
                        {
                            return STATUS_SUCCESS;
                        }
                        if (cbOutput < sizeof(BCRYPT_CHAIN_MODE_CBC))
                        {
                            return STATUS_BUFFER_TOO_SMALL;
                        }
                        memcpy(pbOutput, BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC));
                        return STATUS_SUCCESS;
                    case CRYPT_MODE_ECB:
                        *pcbResult = sizeof(BCRYPT_CHAIN_MODE_ECB);
                        if (!pbOutput)
                        {
                            return STATUS_SUCCESS;
                        }
                        if (cbOutput < sizeof(BCRYPT_CHAIN_MODE_ECB))
                        {
                            return STATUS_BUFFER_TOO_SMALL;
                        }
                        memcpy(pbOutput, BCRYPT_CHAIN_MODE_ECB, sizeof(BCRYPT_CHAIN_MODE_ECB));
                        return STATUS_SUCCESS;
                    case CRYPT_MODE_CFB:
                        *pcbResult = sizeof(BCRYPT_CHAIN_MODE_CFB);
                        if (!pbOutput)
                        {
                            return STATUS_SUCCESS;
                        }
                        if (cbOutput < sizeof(BCRYPT_CHAIN_MODE_CFB))
                        {
                            return STATUS_BUFFER_TOO_SMALL;
                        }
                        memcpy(pbOutput, BCRYPT_CHAIN_MODE_CFB, sizeof(BCRYPT_CHAIN_MODE_CFB));
                        return STATUS_SUCCESS;
                    default:
                        return STATUS_NOT_SUPPORTED;
                    }
                }
                return STATUS_NOT_SUPPORTED;
            }
            
            NTSTATUS
            WINAPI
            SetProperty(
                _In_z_                  LPCWSTR _szProperty,
                _In_reads_bytes_(_cbInput)    PUCHAR   _pInput,
                _In_                    ULONG   _cbInput,
                _In_                    ULONG   _fFlags) override
            {
                if (StringCompareIgnoreCaseByAscii(BCRYPT_CHAINING_MODE, _szProperty, -1) == 0)
                {
                    DWORD _uCryptMode;
                    if (_cbInput == sizeof(BCRYPT_CHAIN_MODE_CBC) && StringCompareIgnoreCaseByAscii((const wchar_t*)_pInput, BCRYPT_CHAIN_MODE_CBC, _countof(BCRYPT_CHAIN_MODE_CBC)) == 0)
                    {
                        _uCryptMode = CRYPT_MODE_CBC;
                    }
                    else if (_cbInput == sizeof(BCRYPT_CHAIN_MODE_ECB) && StringCompareIgnoreCaseByAscii((const wchar_t*)_pInput, BCRYPT_CHAIN_MODE_ECB, _countof(BCRYPT_CHAIN_MODE_ECB)) == 0)
                    {
                        _uCryptMode = CRYPT_MODE_ECB;
                    }
                    else if (_cbInput == sizeof(BCRYPT_CHAIN_MODE_CFB) && StringCompareIgnoreCaseByAscii((const wchar_t*)_pInput, BCRYPT_CHAIN_MODE_CFB, _countof(BCRYPT_CHAIN_MODE_CFB)) == 0)
                    {
                        _uCryptMode = CRYPT_MODE_CFB;
                    }
                    else
                    {
                        /*if (_cbInput == sizeof(BCRYPT_CHAIN_MODE_CCM) && StringCompareIgnoreCaseByAscii((const wchar_t*)_pInput, BCRYPT_CHAIN_MODE_CCM, _countof(BCRYPT_CHAIN_MODE_CCM)) == 0)
                        {
                            _uCryptMode = ;
                        }
                        else if (_cbInput == sizeof(BCRYPT_CHAIN_MODE_GCM) && StringCompareIgnoreCaseByAscii((const wchar_t*)_pInput, BCRYPT_CHAIN_MODE_GCM, _countof(BCRYPT_CHAIN_MODE_GCM)) == 0)
                        {
                            _uCryptMode = ;
                        }*/
                        return STATUS_NOT_SUPPORTED;
                    }

                    if (!CryptSetKeyParam(hKey, KP_MODE, (const BYTE*)&_uCryptMode, 0))
                    {
                        return STATUS_NOT_SUPPORTED;
                    }
                    return STATUS_SUCCESS;
                }
                return STATUS_NOT_SUPPORTED;
            }
            
            NTSTATUS WINAPI Encrypt(
                _In_reads_bytes_opt_(_cbInput)                    PUCHAR   _pInput,
                _In_                                        ULONG   _cbInput,
                _In_opt_                                    VOID* _pPaddingInfo,
                _Inout_updates_bytes_opt_(_cbIV)                    PUCHAR   _pIV,
                _In_                                        ULONG   _cbIV,
                _Out_writes_bytes_to_opt_(_cbOutput, *_pcbResult) PUCHAR   _pOutput,
                _In_                                        ULONG   _cbOutput,
                _Out_                                       ULONG* _pcbResult,
                _In_                                        ULONG   _fFlags)
            {
                BOOL _bFinal = FALSE;
                if (_fFlags & BCRYPT_BLOCK_PADDING)
                {
                    _bFinal = TRUE;
                }

                // 检测所需的缓冲区
                auto _cbOutputBufferNeed = _cbInput;
                if (!CryptEncrypt(hKey, NULL, _bFinal, 0, nullptr, &_cbOutputBufferNeed, 0))
                {
                    return BCryptAlgorithm::CryptoErrorToNTStatus(GetLastError());
                }

                if (_pOutput)
                {
                    // 检测缓冲区是否充足
                    if (_cbOutput < _cbOutputBufferNeed)
                    {
                        *_pcbResult = _cbOutputBufferNeed;
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    if (!_pInput)
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    if (_cbIV)
                    {
                        if (_pIV == nullptr)
                        {
                            return STATUS_INVALID_PARAMETER;
                        }

                        if (GetBlockLength() != _cbIV)
                        {
                            return STATUS_INVALID_PARAMETER;
                        }

                        if (!CryptSetKeyParam(hKey, KP_IV, _pIV, 0))
                        {
                            return STATUS_NOT_SUPPORTED;
                        }
                    }

                    if (_pOutput != _pInput)
                    {
                        memcpy(_pOutput, _pInput, _cbInput);
                    }

                    if (!CryptEncrypt(hKey, NULL, _bFinal, 0, _pOutput, &_cbInput, _cbOutput))
                    {
                        return BCryptAlgorithm::CryptoErrorToNTStatus(GetLastError());
                    }
                    *_pcbResult = _cbInput;
                }
                else
                {
                    // 单纯计算返回块大小

                    *_pcbResult = _cbOutputBufferNeed;
                }                

                return STATUS_SUCCESS;
            }

            NTSTATUS WINAPI Decrypt(
                _In_reads_bytes_opt_(_cbInput)              PUCHAR _pInput,
                _In_                                        ULONG _cbInput,
                _In_opt_                                    VOID* _pPaddingInfo,
                _Inout_updates_bytes_opt_(_cbIV)            PUCHAR _pIV,
                _In_                                        ULONG _cbIV,
                _Out_writes_bytes_to_opt_(_cbOutput, *_pcbResult) PUCHAR _pOutput,
                _In_                                        ULONG _cbOutput,
                _Out_                                       ULONG* _pcbResult,
                _In_                                        ULONG _fFlags)
            {
                BOOL _bFinal = FALSE;
                if (_fFlags & BCRYPT_BLOCK_PADDING)
                {
                    _bFinal = TRUE;
                }

                // 检测所需的缓冲区
                auto _cbOutputBufferNeed = _cbInput;
                if (!CryptDecrypt(hKey, NULL, _bFinal, 0, nullptr, &_cbOutputBufferNeed))
                {
                    return BCryptAlgorithm::CryptoErrorToNTStatus(GetLastError());
                }

                if (_pOutput)
                {
                    // 检测缓冲区
                    if (_cbOutput < _cbOutputBufferNeed)
                    {
                        *_pcbResult = _cbOutputBufferNeed;
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    if (!_pInput)
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    const auto _uBlockLength = GetBlockLength();
                    if (_uBlockLength == 0)
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    if (_cbIV)
                    {
                        if (_pIV == nullptr)
                        {
                            return STATUS_INVALID_PARAMETER;
                        }

                        if (_uBlockLength != _cbIV)
                        {
                            return STATUS_INVALID_PARAMETER;
                        }

                        if (!CryptSetKeyParam(hKey, KP_IV, _pIV, 0))
                        {
                            return STATUS_NOT_SUPPORTED;
                        }
                    }

                    if (_cbInput <= _cbOutput)
                    {
                        // 如果现有输出缓冲区足以容纳，那么我们直接干即可
                        if (_pInput != _pOutput)
                        {
                            memcpy(_pOutput, _pInput, _cbInput);
                        }
                        if (!CryptDecrypt(hKey, NULL, _bFinal, 0, _pOutput, &_cbInput))
                        {
                            return BCryptAlgorithm::CryptoErrorToNTStatus(GetLastError());
                        }

                        *_pcbResult = _cbInput;
                        return STATUS_SUCCESS;
                    }
                    else
                    {
                        // 这时一定 _bFinal == TRUE
                        
                        // 现有输出缓冲区无法容纳，为了减少中间内存开销，我们分成2步进行
                        // 第一步，先把头部的数据先解密
                        DWORD _cbResult1 = 0;
                        if (_cbInput > _uBlockLength)
                        {
                            _cbResult1 = _cbInput - _uBlockLength;
                            memcpy(_pOutput, _pInput, _cbResult1);
                            _pInput += _cbResult1;
                            _cbInput = _uBlockLength;
                            if (!CryptDecrypt(hKey, NULL, FALSE, 0, _pOutput, &_cbResult1))
                            {
                                return BCryptAlgorithm::CryptoErrorToNTStatus(GetLastError());
                            }

                            _pOutput += _cbResult1;
                        }

                        // 第二步：解密最后一块
                        BYTE _TmpOutputBuffer[kMaxBlockLength];
                        memcpy(_TmpOutputBuffer, _pInput, _cbInput);
                        if (!CryptDecrypt(hKey, NULL, _bFinal, 0, _TmpOutputBuffer, &_cbInput))
                        {
                            return BCryptAlgorithm::CryptoErrorToNTStatus(GetLastError());
                        }

                        memcpy(_pOutput, _TmpOutputBuffer, _cbInput);
                        *_pcbResult = _cbResult1 + _cbInput;
                        return STATUS_SUCCESS;
                    }
                }
                else
                {
                    // 返回长度
                    *_pcbResult = _cbOutputBufferNeed;
                    return STATUS_SUCCESS;
                }
            }

            NTSTATUS WINAPI ExportKey(
                _In_opt_                                    BCryptKey*   _pExportKey,
                _In_z_                                      LPCWSTR _szBlobType,
                _Out_writes_bytes_to_opt_(_cbOutput, *_pcbResult) PUCHAR   _pOutput,
                _In_                                        ULONG   _cbOutput,
                _Out_                                       ULONG*  _pcbResult,
                _In_                                        ULONG   _fFlags
                )
            {
                if (_pExportKey || _fFlags)
                {
                    return STATUS_INVALID_PARAMETER;
                }

                if (StringCompareIgnoreCaseByAscii(_szBlobType, BCRYPT_KEY_DATA_BLOB, -1) == 0 || StringCompareIgnoreCaseByAscii(_szBlobType, BCRYPT_OPAQUE_KEY_BLOB, -1) == 0)
                {
                    struct _PLAINTEXTKEYBLOB : public BLOBHEADER
                    {
                        DWORD dwKeySize;
                        BYTE rgbKeyData[0];
                    };

                    // 内部布局恰好类似，做一些特殊优化
                    static_assert(sizeof(_BCRYPT_KEY_DATA_BLOB_HEADER) == sizeof(_PLAINTEXTKEYBLOB), "");

                    if (!CryptExportKey(hKey, NULL, PLAINTEXTKEYBLOB, 0, (BYTE*)_pOutput, &_cbOutput))
                    {
                        auto _lStatus = GetLastError();
                        if (_lStatus == ERROR_MORE_DATA)
                        {
                            *_pcbResult = _cbOutput;
                        }
                        return BCryptAlgorithm::CryptoErrorToNTStatus(_lStatus);
                    }

                    if (_pOutput)
                    {
                        auto _pBCryptKeyDataBlobHerder = reinterpret_cast<_BCRYPT_KEY_DATA_BLOB_HEADER*>(_pOutput);
                        _pBCryptKeyDataBlobHerder->dwMagic = BCRYPT_KEY_DATA_BLOB_MAGIC;
                        _pBCryptKeyDataBlobHerder->dwVersion = BCRYPT_KEY_DATA_BLOB_VERSION1;
                    }

                    *_pcbResult = _cbOutput;

                    return STATUS_SUCCESS;
                }
                else
                {
                    return STATUS_NOT_SUPPORTED;
                }
            }
        };

        template<typename BCryptAlgorithmType, typename BCryptKeyType, DWORD kDefaultCryptMode, DWORD kDefaultBlockSize>
        struct BCryptKeyAlgorithm : public BCryptAlgorithmByCryptoAPI<BCryptAlgorithmType>
        {
            using BCryptAlgorithmByCryptoAPI<BCryptAlgorithmType>::uCryptMode;
            using BCryptAlgorithmByCryptoAPI<BCryptAlgorithmType>::uEffectiveKeyBitCount;

            BCryptKeyAlgorithm()
            {
                uCryptMode = kDefaultCryptMode;
            }

            NTSTATUS WINAPI GetProperty(
                _In_z_                                      LPCWSTR pszProperty,
                _Out_writes_bytes_to_opt_(cbOutput, *pcbResult) PUCHAR   pbOutput,
                _In_                                        ULONG   cbOutput,
                _Out_                                       ULONG* pcbResult,
                _In_                                        ULONG   dwFlags
                ) override
            {
                if (StringCompareIgnoreCaseByAscii(BCRYPT_OBJECT_LENGTH, pszProperty, -1) == 0)
                {
                    *pcbResult = sizeof(DWORD);
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < sizeof(DWORD))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    *reinterpret_cast<DWORD*>(pbOutput) = sizeof(BCryptKeyType);
                    return STATUS_SUCCESS;
                }
                else if (StringCompareIgnoreCaseByAscii(BCRYPT_CHAINING_MODE, pszProperty, -1) == 0)
                {
                    if (uCryptMode == 0)
                    {
                        return STATUS_NOT_SUPPORTED;
                    }

                    switch (uCryptMode)
                    {
                    case CRYPT_MODE_CBC:
                        *pcbResult = sizeof(BCRYPT_CHAIN_MODE_CBC);
                        if (!pbOutput)
                        {
                            return STATUS_SUCCESS;
                        }
                        if (cbOutput < sizeof(BCRYPT_CHAIN_MODE_CBC))
                        {
                            return STATUS_BUFFER_TOO_SMALL;
                        }
                        memcpy(pbOutput, BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC));
                        return STATUS_SUCCESS;
                    case CRYPT_MODE_ECB:
                        *pcbResult = sizeof(BCRYPT_CHAIN_MODE_ECB);
                        if (!pbOutput)
                        {
                            return STATUS_SUCCESS;
                        }
                        if (cbOutput < sizeof(BCRYPT_CHAIN_MODE_ECB))
                        {
                            return STATUS_BUFFER_TOO_SMALL;
                        }
                        memcpy(pbOutput, BCRYPT_CHAIN_MODE_ECB, sizeof(BCRYPT_CHAIN_MODE_ECB));
                        return STATUS_SUCCESS;
                    case CRYPT_MODE_CFB:
                        *pcbResult = sizeof(BCRYPT_CHAIN_MODE_CFB);
                        if (!pbOutput)
                        {
                            return STATUS_SUCCESS;
                        }
                        if (cbOutput < sizeof(BCRYPT_CHAIN_MODE_CFB))
                        {
                            return STATUS_BUFFER_TOO_SMALL;
                        }
                        memcpy(pbOutput, BCRYPT_CHAIN_MODE_CFB, sizeof(BCRYPT_CHAIN_MODE_CFB));
                        return STATUS_SUCCESS;
                    default:
                        return STATUS_NOT_SUPPORTED;
                    }
                }
                else if(StringCompareIgnoreCaseByAscii(BCRYPT_BLOCK_SIZE_LIST, pszProperty, -1) == 0)
                {
                    // 我也觉得有点奇怪，为什么AES这样的没有返回一个列表，仅仅返回一个值。
                    // 但是实际上微软的BCrypt也是如此。
                    if (kDefaultBlockSize == 0)
                    {
                        return STATUS_NOT_SUPPORTED;
                    }

                    *pcbResult = sizeof(DWORD);
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < sizeof(DWORD))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    *reinterpret_cast<DWORD*>(pbOutput) = kDefaultBlockSize;
                    return STATUS_SUCCESS;
                }
                else if (StringCompareIgnoreCaseByAscii(BCRYPT_EFFECTIVE_KEY_LENGTH, pszProperty, -1) == 0)
                {
                    if (uEffectiveKeyBitCount == 0)
                    {
                        return STATUS_NOT_SUPPORTED;
                    }

                    *pcbResult = sizeof(DWORD);
                    if (!pbOutput)
                    {
                        return STATUS_SUCCESS;
                    }

                    if (cbOutput < sizeof(DWORD))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    *reinterpret_cast<DWORD*>(pbOutput) = uEffectiveKeyBitCount;
                    return STATUS_SUCCESS;
                }
                return BCryptAlgorithmByCryptoAPI<BCryptAlgorithmType>::GetProperty(pszProperty, pbOutput, cbOutput, pcbResult, dwFlags);
            }

            NTSTATUS
            WINAPI
            SetProperty(
                _In_z_                  LPCWSTR _szProperty,
                _In_reads_bytes_(_cbInput)    PUCHAR   _pInput,
                _In_                    ULONG   _cbInput,
                _In_                    ULONG   _fFlags) override
            {
                if (StringCompareIgnoreCaseByAscii(BCRYPT_CHAINING_MODE, _szProperty, -1) == 0)
                {
                    if (uCryptMode == 0)
                    {
                        return STATUS_NOT_SUPPORTED;
                    }
                    else if (_cbInput == sizeof(BCRYPT_CHAIN_MODE_CBC) && StringCompareIgnoreCaseByAscii((const wchar_t*)_pInput, BCRYPT_CHAIN_MODE_CBC, _countof(BCRYPT_CHAIN_MODE_CBC)) == 0)
                    {
                        uCryptMode = CRYPT_MODE_CBC;
                    }
                    else if (_cbInput == sizeof(BCRYPT_CHAIN_MODE_ECB) && StringCompareIgnoreCaseByAscii((const wchar_t*)_pInput, BCRYPT_CHAIN_MODE_ECB, _countof(BCRYPT_CHAIN_MODE_ECB)) == 0)
                    {
                        uCryptMode = CRYPT_MODE_ECB;
                    }
                    else if (_cbInput == sizeof(BCRYPT_CHAIN_MODE_CFB) && StringCompareIgnoreCaseByAscii((const wchar_t*)_pInput, BCRYPT_CHAIN_MODE_CFB, _countof(BCRYPT_CHAIN_MODE_CFB)) == 0)
                    {
                        uCryptMode = CRYPT_MODE_CFB;
                    }
                    else
                    {
                        /*if (_cbInput == sizeof(BCRYPT_CHAIN_MODE_CCM) && StringCompareIgnoreCaseByAscii((const wchar_t*)_pInput, BCRYPT_CHAIN_MODE_CCM, _countof(BCRYPT_CHAIN_MODE_CCM)) == 0)
                        {
                            uCryptMode = ;
                        }
                        else if (_cbInput == sizeof(BCRYPT_CHAIN_MODE_GCM) && StringCompareIgnoreCaseByAscii((const wchar_t*)_pInput, BCRYPT_CHAIN_MODE_GCM, _countof(BCRYPT_CHAIN_MODE_GCM)) == 0)
                        {
                            uCryptMode = ;
                        }*/
                        return STATUS_NOT_SUPPORTED;
                    }
                    
                    return STATUS_SUCCESS;
                }

                return BCryptAlgorithmByCryptoAPI<BCryptAlgorithmType>::SetProperty(_szProperty, _pInput, _cbInput, _fFlags);
            }

            NTSTATUS WINAPI GenerateSymmetricKey(
                _Out_                               BCryptKey**_ppKey,
                _Out_writes_bytes_all_opt_(_cbKeyObject)  PUCHAR   _pbKeyObject,
                _In_                                ULONG   _cbKeyObject,
                _In_reads_bytes_(_cbSecret)               PUCHAR   _pbSecret,
                _In_                                ULONG   _cbSecret,
                _In_                                ULONG   _fFlags) override
            {
                *_ppKey = nullptr;
                BCryptKeyType* _pKey = nullptr;
                if (_cbKeyObject == 0)
                {
                    _pKey = (BCryptKeyType*)internal::Alloc(sizeof(BCryptKeyType));
                    new(_pKey) BCryptKeyType(this);
                }
                else
                {
                    if (_pbKeyObject == nullptr)
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    if (_cbKeyObject < sizeof(BCryptKeyType))
                    {
                        return STATUS_BUFFER_TOO_SMALL;
                    }

                    _pKey = reinterpret_cast<BCryptKeyType*>(_pbKeyObject);
                    new(_pKey) BCryptKeyType(this);
                    _pKey->bCanFree = false;
                }

                long _Status = _pKey->Init(_pbSecret, _cbSecret, _fFlags);
                if (_Status < 0)
                {
                    _pKey->Release();
                    return _Status;
                }
                *_ppKey = _pKey;
                return STATUS_SUCCESS;
            }


            NTSTATUS WINAPI ImportKey(
                _In_opt_                            BCryptKey* _pImportKey,
                _In_z_                              LPCWSTR _szBlobType,
                _Out_                               BCryptKey** _ppKey,
                _Out_writes_bytes_all_opt_(_cbKeyObject)  PUCHAR   _pKeyObject,
                _In_                                ULONG   _cbKeyObject,
                _In_reads_bytes_(_cbInput)                PUCHAR   _pInput,
                _In_                                ULONG   _cbInput,
                _In_                                ULONG   _fFlags
                ) override
            {
                if (_pImportKey || _fFlags)
                {
                    return STATUS_INVALID_PARAMETER;
                }
                
                if (StringCompareIgnoreCaseByAscii(_szBlobType, BCRYPT_KEY_DATA_BLOB, -1) == 0 || StringCompareIgnoreCaseByAscii(_szBlobType, BCRYPT_OPAQUE_KEY_BLOB, -1) == 0)
                {
                    auto _pHerder = (_BCRYPT_KEY_DATA_BLOB_HEADER*)_pInput;
                    if (_pHerder == nullptr || _pHerder->dwMagic != BCRYPT_KEY_DATA_BLOB_MAGIC || _pHerder->dwVersion != BCRYPT_KEY_DATA_BLOB_VERSION1)
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    // _pInput 读取缓冲区溢出
                    if (_pHerder->cbKeyData + sizeof(_BCRYPT_KEY_DATA_BLOB_HEADER) > _cbInput)
                    {
                        return STATUS_INVALID_PARAMETER;
                    }

                    return GenerateSymmetricKey(_ppKey, _pKeyObject, _cbKeyObject, (PUCHAR)&_pHerder[1], _pHerder->cbKeyData, 0);
                }

                return STATUS_NOT_SUPPORTED;
            }
        };

        struct BCryptAESAlgorithm : public BCryptKeyAlgorithm<BCryptAESAlgorithm, BCryptKey, CRYPT_MODE_CBC, 16>
        {
        };

        struct BCryptDESAlgorithm : public BCryptKeyAlgorithm<BCryptDESAlgorithm, BCryptKey, CRYPT_MODE_CBC, 8>
        {
        };

        struct BCrypt3DESAlgorithm : public BCryptKeyAlgorithm<BCrypt3DESAlgorithm, BCryptKey, CRYPT_MODE_CBC, 8>
        {
        };

        struct BCrypt3DES_112Algorithm : public BCryptKeyAlgorithm<BCrypt3DES_112Algorithm, BCryptKey, CRYPT_MODE_CBC, 8>
        {
        };

        struct BCryptRC2Algorithm : public BCryptKeyAlgorithm<BCryptRC2Algorithm, BCryptKey, CRYPT_MODE_CBC, 8>
        {
            BCryptRC2Algorithm()
            {
                // 微软BCrypt的RC2默认为128bits
                uEffectiveKeyBitCount = 128;
            }
        };
        
        struct BCryptRC4Algorithm : public BCryptKeyAlgorithm<BCryptRC4Algorithm, BCryptKey, 0, 1>
        {

        };

        template<typename TargrtObject>
        bool __fastcall Is(void* _pSrc);

        template<>
        bool __fastcall Is<BCryptObject>(void* _pSrc)
        {
            auto _pBCryptObject = reinterpret_cast<BCryptObject*>(_pSrc);
            return _pBCryptObject != nullptr && _pBCryptObject->IsValid();
        }

        template<>
        bool __fastcall Is<BCryptAlgorithm>(void* _pSrc)
        {
            if (!Is<BCryptObject>(_pSrc))
                return false;

            return reinterpret_cast<BCryptObject*>(_pSrc)->IsAlgorithm();
        }

        template<>
        bool __fastcall Is<BCryptRngAlgorithm>(void* _pSrc)
        {
            if (!Is<BCryptAlgorithm>(_pSrc))
                return false;

            return reinterpret_cast<BCryptRngAlgorithm*>(_pSrc)->IsRng();
        }

        template<>
        bool __fastcall Is<BCryptHash>(void* _pSrc)
        {
            if (!Is<BCryptObject>(_pSrc))
                return false;

            return reinterpret_cast<BCryptObject*>(_pSrc)->IsHash();
        }

        template<>
        bool __fastcall Is<BCryptKey>(void* _pSrc)
        {
            if (!Is<BCryptObject>(_pSrc))
                return false;

            return reinterpret_cast<BCryptObject*>(_pSrc)->IsKey();
        }
    }
#endif
}

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    16,
    NTSTATUS,
    WINAPI,
    BCryptOpenAlgorithmProvider,
        _Out_       BCRYPT_ALG_HANDLE* _phAlgorithm,
        _In_        LPCWSTR            _szAlgId,
        _In_opt_    LPCWSTR            _szImplementation,
        _In_        ULONG              _fFlags
        )
    {
        if (const auto _pfnBCryptOpenAlgorithmProvider = try_get_BCryptOpenAlgorithmProvider())
        {
            return _pfnBCryptOpenAlgorithmProvider(_phAlgorithm, _szAlgId, _szImplementation, _fFlags);
        }

        UNREFERENCED_PARAMETER(_szImplementation);

        if (_phAlgorithm == nullptr || _szAlgId == nullptr || (_fFlags & ~BCRYPT_ALG_HANDLE_HMAC_FLAG))
        {
            return STATUS_INVALID_PARAMETER;
        }

        static const BCryptMapItem g_Map[] =
        {
            // { BCRYPT_RSA_ALGORITHM, nullptr, PROV_RSA_AES, CALG_RC4, &BCryptRSAAlgorithm::Create },
            // BCRYPT_RSA_SIGN_ALGORITHM
            // BCRYPT_DH_ALGORITHM
            // BCRYPT_DSA_ALGORITHM
            { BCRYPT_RC2_ALGORITHM, nullptr, PROV_RSA_AES, CALG_RC2, &BCryptRC2Algorithm::Create },
            { BCRYPT_RC4_ALGORITHM, nullptr, PROV_RSA_AES, CALG_RC4, &BCryptRC4Algorithm::Create },
            { BCRYPT_AES_ALGORITHM, nullptr, PROV_RSA_AES, CALG_AES, &BCryptAESAlgorithm::Create },
            { BCRYPT_DES_ALGORITHM, nullptr, PROV_RSA_FULL, CALG_DES, &BCryptDESAlgorithm::Create },
            // BCRYPT_DESX_ALGORITHM
            { BCRYPT_3DES_ALGORITHM, nullptr, PROV_RSA_FULL, CALG_3DES, &BCrypt3DESAlgorithm::Create },
            { BCRYPT_3DES_112_ALGORITHM, nullptr, PROV_RSA_FULL, CALG_3DES_112, &BCrypt3DES_112Algorithm::Create },
            { BCRYPT_MD2_ALGORITHM, nullptr, PROV_RSA_FULL, CALG_MD2, &BCryptHashAlgorithm::Create },
            { BCRYPT_MD4_ALGORITHM, nullptr, PROV_RSA_FULL, CALG_MD4, &BCryptHashAlgorithm::Create },
            { BCRYPT_MD5_ALGORITHM, nullptr, PROV_RSA_FULL, CALG_MD5, &BCryptHashAlgorithm::Create },
            { BCRYPT_SHA1_ALGORITHM, nullptr, PROV_RSA_FULL, CALG_SHA1, &BCryptHashAlgorithm::Create },
            { BCRYPT_SHA256_ALGORITHM, nullptr, PROV_RSA_AES, CALG_SHA_256, &BCryptHashAlgorithm::Create },
            { BCRYPT_SHA384_ALGORITHM, nullptr, PROV_RSA_AES, CALG_SHA_384, &BCryptHashAlgorithm::Create },
            { BCRYPT_SHA512_ALGORITHM, nullptr, PROV_RSA_AES, CALG_SHA_512, &BCryptHashAlgorithm::Create },
            //#define BCRYPT_AES_GMAC_ALGORITHM               L"AES-GMAC"
            //#define BCRYPT_AES_CMAC_ALGORITHM               L"AES-CMAC"
            //#define BCRYPT_ECDSA_P256_ALGORITHM             L"ECDSA_P256"
            //#define BCRYPT_ECDSA_P384_ALGORITHM             L"ECDSA_P384"
            //#define BCRYPT_ECDSA_P521_ALGORITHM             L"ECDSA_P521"
            //#define BCRYPT_ECDH_P256_ALGORITHM              L"ECDH_P256"
            //#define BCRYPT_ECDH_P384_ALGORITHM              L"ECDH_P384"
            //#define BCRYPT_ECDH_P521_ALGORITHM              L"ECDH_P521"
            { BCRYPT_RNG_ALGORITHM, nullptr, 0, 0, &BCryptRngAlgorithm::Create },
            { BCRYPT_RNG_FIPS186_DSA_ALGORITHM, nullptr, 0, 0, &BCryptRngAlgorithm::Create },
            { BCRYPT_RNG_DUAL_EC_ALGORITHM, nullptr, 0, 0, &BCryptRngAlgorithm::Create },
        };

#if (YY_Thunks_Target < __WindowsNT5_1_SP3)
        __WarningMessage__("Windows XP SP3的Crypt开始才支持 CALG_SHA_256、CALG_SHA_384、CALG_SHA_512");
#endif

        for (auto& _Item : g_Map)
        {
            if (StringCompareIgnoreCaseByAscii(_szAlgId, _Item.szAlgName, (size_t)-1) == 0)
            {
                return _Item.pfnOpenAlgorithmProviderType(&_Item, _fFlags, reinterpret_cast<BCryptAlgorithm**>(_phAlgorithm));
            }
        }

        return STATUS_NOT_FOUND;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    8,
    NTSTATUS,
    WINAPI,
    BCryptCloseAlgorithmProvider,
        _Inout_ BCRYPT_ALG_HANDLE _hAlgorithm,
        _In_    ULONG _fFlags)
    {
        if (auto _pfnBCryptCloseAlgorithmProvider = try_get_BCryptCloseAlgorithmProvider())
        {
            return _pfnBCryptCloseAlgorithmProvider(_hAlgorithm, _fFlags);
        }

        if (_fFlags)
            return STATUS_INVALID_PARAMETER;

        if (!Is<BCryptAlgorithm>(_hAlgorithm))
            return STATUS_INVALID_HANDLE;

        reinterpret_cast<BCryptObject*>(_hAlgorithm)->Release();
        return STATUS_SUCCESS;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    16,
    NTSTATUS,
    WINAPI,
    BCryptGenRandom,
        _In_opt_                        BCRYPT_ALG_HANDLE _hAlgorithm,
        _Out_writes_bytes_(_cbBuffer)   PUCHAR _pbBuffer,
        _In_                            ULONG _cbBuffer,
        _In_                            ULONG _fFlags
        )
    {
        if (auto _pfnBCryptGenRandom = try_get_BCryptGenRandom())
        {
            return _pfnBCryptGenRandom(_hAlgorithm, _pbBuffer, _cbBuffer, _fFlags);
        }
            
        if (_pbBuffer == nullptr)
            return STATUS_INVALID_PARAMETER;
        if (_cbBuffer == 0)
            return STATUS_SUCCESS;

        if (_fFlags & BCRYPT_USE_SYSTEM_PREFERRED_RNG)
        {
            if(_hAlgorithm != NULL)
                return STATUS_INVALID_PARAMETER;
        }
        else
        {
            if (!Is<BCryptRngAlgorithm>(_hAlgorithm))
            {
                return STATUS_INVALID_HANDLE;
            }
        }

        // 此函数内部其实就是用了Crypt API，所以针对Windows XP就直接使用它了。
        const auto _pfnRtlGenRandom = try_get_SystemFunction036();
        if (!_pfnRtlGenRandom)
        {
            internal::RaiseStatus(STATUS_NOT_IMPLEMENTED);
            return STATUS_NOT_IMPLEMENTED;
        }
            
        if (_pfnRtlGenRandom(_pbBuffer, _cbBuffer))
            return STATUS_SUCCESS;
        else
            return STATUS_UNSUCCESSFUL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    24,
    NTSTATUS,
    WINAPI,
    BCryptGetProperty,
        _In_                                        BCRYPT_HANDLE   hObject,
        _In_z_                                      LPCWSTR pszProperty,
        _Out_writes_bytes_to_opt_(cbOutput, *pcbResult) PUCHAR   pbOutput,
        _In_                                        ULONG   cbOutput,
        _Out_                                       ULONG   *pcbResult,
        _In_                                        ULONG   dwFlags
        )
    {
        if (const auto _pfnBCryptGetProperty = try_get_BCryptGetProperty())
        {
            return _pfnBCryptGetProperty(hObject, pszProperty, pbOutput, cbOutput, pcbResult, dwFlags);
        }
            
        if(pszProperty == nullptr || dwFlags)
            return STATUS_INVALID_PARAMETER;

        if (!Is<BCryptObject>(hObject))
        {
            return STATUS_INVALID_HANDLE;
        }
            
        return reinterpret_cast<BCryptObject*>(hObject)->GetProperty(pszProperty, pbOutput, cbOutput, pcbResult, dwFlags);
    }
#endif
        

#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    20,
    NTSTATUS,
    WINAPI,
    BCryptSetProperty,
        _Inout_                 BCRYPT_HANDLE _hObject,
        _In_z_                  LPCWSTR _szProperty,
        _In_reads_bytes_(cbInput)    PUCHAR   _pInput,
        _In_                    ULONG   _cbInput,
        _In_                    ULONG   _fFlags
        )
    {
        if (const auto _pfnBCryptSetProperty = try_get_BCryptSetProperty())
        {
            return _pfnBCryptSetProperty(_hObject, _szProperty, _pInput, _cbInput, _fFlags);
        }

        if(_szProperty == nullptr || _fFlags)
            return STATUS_INVALID_PARAMETER;

        if (!Is<BCryptObject>(_hObject))
        {
            return STATUS_INVALID_HANDLE;
        }
            
        return reinterpret_cast<BCryptObject*>(_hObject)->SetProperty(_szProperty, _pInput, _cbInput, _fFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    28,
    NTSTATUS,
    WINAPI,
    BCryptCreateHash,
        _Inout_                             BCRYPT_ALG_HANDLE   hAlgorithm,
        _Out_                               BCRYPT_HASH_HANDLE  *phHash,
        _Out_writes_bytes_all_opt_(cbHashObject) PUCHAR   pbHashObject,
        _In_                                ULONG   cbHashObject,
        _In_reads_bytes_opt_(cbSecret)           PUCHAR   pbSecret,   // optional
        _In_                                ULONG   cbSecret,   // optional
        _In_                                ULONG   dwFlags
        )
    {
        if (const auto _pfnBCryptCreateHash = try_get_BCryptCreateHash())
        {
            return _pfnBCryptCreateHash(hAlgorithm, phHash, pbHashObject, cbHashObject, pbSecret, cbSecret, dwFlags);
        }

        if ((cbHashObject && pbHashObject == nullptr) || dwFlags || phHash == nullptr)
        {
            return STATUS_INVALID_PARAMETER;
        }

        if (!Is<BCryptAlgorithm>(hAlgorithm))
        {
            return STATUS_INVALID_HANDLE;
        }

        return reinterpret_cast<BCryptAlgorithm*>(hAlgorithm)->CreateHash(reinterpret_cast<BCryptHash**>(phHash), pbHashObject, cbHashObject, pbSecret, cbSecret, dwFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    16,
    NTSTATUS,
    WINAPI,
    BCryptHashData,
        _Inout_                 BCRYPT_HASH_HANDLE  hHash,
        _In_reads_bytes_(cbInput)    PUCHAR   pbInput,
        _In_                    ULONG   cbInput,
        _In_                    ULONG   dwFlags
        )
    {
        if (const auto _pfnBCryptHashData = try_get_BCryptHashData())
        {
            return _pfnBCryptHashData(hHash, pbInput, cbInput, dwFlags);
        }

        if(dwFlags)
            return STATUS_INVALID_PARAMETER;

        if (!Is<BCryptHash>(hHash))
            return STATUS_INVALID_HANDLE;

        return reinterpret_cast<BCryptHash*>(hHash)->HashData(pbInput, cbInput, dwFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    16,
    NTSTATUS,
    WINAPI,
    BCryptFinishHash,
        _Inout_                     BCRYPT_HASH_HANDLE hHash,
        _Out_writes_bytes_all_(cbOutput) PUCHAR   pbOutput,
        _In_                        ULONG   cbOutput,
        _In_                        ULONG   dwFlags
        )
    {
        if (const auto _pfnBCryptFinishHash = try_get_BCryptFinishHash())
        {
            return _pfnBCryptFinishHash(hHash, pbOutput, cbOutput, dwFlags);
        }

        if(dwFlags)
            return STATUS_INVALID_PARAMETER;

        if (!Is<BCryptHash>(hHash))
            return STATUS_INVALID_HANDLE;

        return reinterpret_cast<BCryptHash*>(hHash)->FinishHash(pbOutput, cbOutput, dwFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    4,
    NTSTATUS,
    WINAPI,
    BCryptDestroyHash,
        _Inout_ BCRYPT_HASH_HANDLE hHash)
    {
        if (const auto _pfnBCryptDestroyHash = try_get_BCryptDestroyHash())
        {
            return _pfnBCryptDestroyHash(hHash);
        }

        if (!Is<BCryptHash>(hHash))
            return STATUS_INVALID_PARAMETER;

        reinterpret_cast<BCryptHash*>(hHash)->Release();
        return STATUS_SUCCESS;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    40,
    NTSTATUS,
    WINAPI,
    BCryptDeriveKeyPBKDF2,
        _In_                                 BCRYPT_ALG_HANDLE   hPrf,
        _In_reads_bytes_opt_( cbPassword )   PUCHAR              pbPassword,
        _In_                                 ULONG               cbPassword,
        _In_reads_bytes_opt_( cbSalt )       PUCHAR              pbSalt,
        _In_                                 ULONG               cbSalt,
        _In_                                 ULONGLONG           cIterations,
        _Out_writes_bytes_( cbDerivedKey )   PUCHAR              pbDerivedKey,
        _In_                                 ULONG               cbDerivedKey,
        _In_                                 ULONG               dwFlags
        )
    {
        if (const auto _pfnBCryptDeriveKeyPBKDF2 = try_get_BCryptDeriveKeyPBKDF2())
        {
            return _pfnBCryptDeriveKeyPBKDF2(hPrf, pbPassword, cbPassword, pbSalt, cbSalt, cIterations, pbDerivedKey, cbDerivedKey, dwFlags);
        }
            
        // 实现参考微软 bcrypt.BCryptDeriveKeyPBKDF2 函数
        if ((pbPassword == nullptr && cbPassword) || (pbSalt == nullptr && cbSalt) || cIterations == 0 || pbDerivedKey == nullptr || cbDerivedKey == 0)
        {
            return STATUS_INVALID_PARAMETER;
        }

        --cIterations;

        ULONG _cbResult = 0;

        // Windows 7 开始才支持BCRYPT_IS_KEYED_HASH，因此这里就不做判断了，跟微软原版存在略微差异。

        DWORD _uHashLength = 0;
        int _Status = ::BCryptGetProperty(hPrf, BCRYPT_HASH_LENGTH, reinterpret_cast<PUCHAR>(&_uHashLength), sizeof(_uHashLength), &_cbResult, 0);
        if (_Status < 0)
        {
            return _Status;
        }
        constexpr auto kMaxHashLength = 512 / 8;
        if (kMaxHashLength < _uHashLength)
        {
            return STATUS_INVALID_PARAMETER;
        }

        DWORD _uObjectLength = 0;
        _Status = ::BCryptGetProperty(hPrf, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&_uObjectLength), sizeof(_uObjectLength), &_cbResult, 0);
        if (_Status < 0)
        {
            return _Status;
        }

        auto _pObjectHashBuffer = (PUCHAR)_malloca(_uObjectLength);
        if (!_pObjectHashBuffer)
        {
            return STATUS_NO_MEMORY;
        }

        // 这个循环等效于 微软 bcrypt.GetPBKDF2Block 函数
        BCRYPT_HASH_HANDLE hHash = nullptr;
        for (DWORD _uBlockCount = 1; cbDerivedKey; ++_uBlockCount)
        {
            _Status = ::BCryptCreateHash(hPrf, &hHash, _pObjectHashBuffer, _uObjectLength, pbPassword, cbPassword, 0);
            if (_Status < 0)
                goto __END;
            _Status = ::BCryptHashData(hHash, pbSalt, cbSalt, 0);
            if (_Status < 0)
                goto __END;

            // 将 _uBlockCount 的字节序颠倒（大端模式）
            DWORD _uReverseBytes = (_uBlockCount << 24);
            _uReverseBytes |= (_uBlockCount << 8) & 0x00FF0000;
            _uReverseBytes |= (_uBlockCount >> 8) & 0x0000FF00;
            _uReverseBytes |= (_uBlockCount >> 24);

            _Status = ::BCryptHashData(hHash, (PUCHAR)&_uReverseBytes, sizeof(_uReverseBytes), 0);
            if (_Status < 0)
                goto __END;

            BYTE _TempHash[kMaxHashLength];
            _Status = ::BCryptFinishHash(hHash, _TempHash, _uHashLength, 0);
            if (_Status < 0)
                goto __END;
            ::BCryptDestroyHash(hHash);
            hHash = NULL;

            BYTE _BlockHash[kMaxHashLength];
            memcpy(_BlockHash, _TempHash, _uHashLength);

            for (auto _uIterationCount = cIterations; _uIterationCount; --_uIterationCount)
            {
                _Status = ::BCryptCreateHash(hPrf, &hHash, _pObjectHashBuffer, _uObjectLength, pbPassword, cbPassword, 0);
                if (_Status < 0)
                    goto __END;

                _Status = ::BCryptHashData(hHash, _TempHash, _uHashLength, 0);
                if (_Status < 0)
                    goto __END;

                _Status = ::BCryptFinishHash(hHash, _TempHash, _uHashLength, 0);
                if (_Status < 0)
                    goto __END;

                ::BCryptDestroyHash(hHash);
                hHash = NULL;

                for (auto i = 0; i != _uHashLength; ++i)
                {
                    _BlockHash[i] ^= _TempHash[i];
                }
            }

            if (cbDerivedKey > _uHashLength)
            {
                memcpy(pbDerivedKey, _BlockHash, _uHashLength);
                pbDerivedKey += _uHashLength;
                cbDerivedKey -= _uHashLength;
            }
            else
            {
                memcpy(pbDerivedKey, _BlockHash, cbDerivedKey);
                break;
            }
        }

        _Status = STATUS_SUCCESS;

    __END:

        if(hHash)
            ::BCryptDestroyHash(hHash);

        if (_pObjectHashBuffer)
            _freea(_pObjectHashBuffer);
        return _Status;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    20,
    NTSTATUS,
    WINAPI,
    BCryptDeriveKeyCapi,
        _In_                            BCRYPT_HASH_HANDLE  hHash,
        _In_opt_                        BCRYPT_ALG_HANDLE   hTargetAlg,
        _Out_writes_bytes_( cbDerivedKey )    PUCHAR              pbDerivedKey,
        _In_                            ULONG               cbDerivedKey,
        _In_                            ULONG               dwFlags
        )
    {
        if (const auto _pfnBCryptDeriveKeyCapi = try_get_BCryptDeriveKeyCapi())
        {
            return _pfnBCryptDeriveKeyCapi(hHash, hTargetAlg, pbDerivedKey, cbDerivedKey, dwFlags);
        }

        if (dwFlags != 0 || (pbDerivedKey == nullptr && cbDerivedKey))
        {
            return STATUS_INVALID_PARAMETER;
        }

        ULONG _cbResult = 0;
        DWORD _uHashLength = 0;
        int _Status = ::BCryptGetProperty(hHash, BCRYPT_HASH_LENGTH, (PUCHAR)&_uHashLength, sizeof(_uHashLength), &_cbResult, 0);
        if (_Status < 0)
            return _Status;

        constexpr auto kMaxHashLength = 512 / 8;
        if (kMaxHashLength < _uHashLength || _uHashLength * 2 < cbDerivedKey)
        {
            return STATUS_INVALID_PARAMETER;
        }

        UCHAR _Hash[kMaxHashLength * 2];
        _Status = BCryptFinishHash(hHash, _Hash, _uHashLength, 0);
        if (_Status < 0)
            return _Status;

        auto v19 = _uHashLength < cbDerivedKey;

        wchar_t szAlgorithmNameBuffer[4];
        if (hTargetAlg && cbDerivedKey == 16 && _uHashLength < 32
            && BCryptGetProperty(hTargetAlg, BCRYPT_ALGORITHM_NAME, (PUCHAR)szAlgorithmNameBuffer, sizeof(szAlgorithmNameBuffer), &_cbResult, 0) >= 0
            && StringCompareIgnoreCaseByAscii(szAlgorithmNameBuffer, L"AES", -1) == 0)
        {
            v19 = true;
        }

        if (!v19)
        {
            memcpy(pbDerivedKey, _Hash, cbDerivedKey);
            return STATUS_SUCCESS;
        }

        UCHAR _FirstHash[kMaxHashLength];
        UCHAR _SecondHash[kMaxHashLength];

        memset(_FirstHash, 0x36, sizeof(_FirstHash));
        memset(_SecondHash, 0x5C, sizeof(_SecondHash));

        for (DWORD i = 0; i != _uHashLength; ++i)
        {
            _FirstHash[i] ^= _Hash[i];
            _SecondHash[i] ^= _Hash[i];
        }

        BCRYPT_ALG_HANDLE hProvider;
        _Status = BCryptGetProperty(hHash, BCRYPT_PROVIDER_HANDLE, (PUCHAR)&hProvider, sizeof(hProvider), &_cbResult, 0);
        if (_Status < 0)
            return _Status;

        DWORD _cbHashObjectLength = 0;
        _Status = BCryptGetProperty(hProvider, BCRYPT_OBJECT_LENGTH, (PUCHAR)&_cbHashObjectLength, sizeof(_cbHashObjectLength), &_cbResult, 0);
        if (_Status < 0)
            return _Status;

        auto _pHashObjectBuffer = (PUCHAR)_malloca(_cbHashObjectLength);
        if (!_pHashObjectBuffer)
            return STATUS_NO_MEMORY;

        BCRYPT_HASH_HANDLE hHash2 = NULL;
        do
        {
            _Status = BCryptCreateHash(hProvider, &hHash2, _pHashObjectBuffer, _cbHashObjectLength, nullptr, 0, 0);
            if (_Status < 0)
                break;

            _Status = BCryptHashData(hHash2, _FirstHash, sizeof(_FirstHash), 0);
            if (_Status < 0)
                break;

            _Status = BCryptFinishHash(hHash2, _Hash, _uHashLength, 0);
            if (_Status < 0)
                break;
            BCryptDestroyHash(hHash2);
            hHash2 = NULL;

            _Status = BCryptCreateHash(hProvider, &hHash2, _pHashObjectBuffer, _cbHashObjectLength, nullptr, 0, 0);
            if (_Status < 0)
                break;

            _Status = BCryptHashData(hHash2, _SecondHash, sizeof(_SecondHash), 0);
            if (_Status < 0)
                break;

            _Status = BCryptFinishHash(hHash2, _Hash + _uHashLength, _uHashLength, 0);
            if (_Status < 0)
                break;

            memcpy(pbDerivedKey, _Hash, cbDerivedKey);
            _Status = STATUS_SUCCESS;

        } while (false);

        if (hHash2)
            BCryptDestroyHash(hHash2);

        if (_pHashObjectBuffer)
            _freea(_pHashObjectBuffer);
        return _Status;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    40,
    NTSTATUS,
    WINAPI,
    BCryptEncrypt,
        _Inout_                                     BCRYPT_KEY_HANDLE _hKey,
        _In_reads_bytes_opt_(_cbInput)                    PUCHAR   _pInput,
        _In_                                        ULONG   _cbInput,
        _In_opt_                                    VOID    *_pPaddingInfo,
        _Inout_updates_bytes_opt_(_cbIV)                    PUCHAR   _pIV,
        _In_                                        ULONG   _cbIV,
        _Out_writes_bytes_to_opt_(_cbOutput, *_pcbResult) PUCHAR   _pOutput,
        _In_                                        ULONG   _cbOutput,
        _Out_                                       ULONG   *_pcbResult,
        _In_                                        ULONG   _fFlags)
    {
        if (const auto _pfnBCryptEncrypt = try_get_BCryptEncrypt())
        {
            return _pfnBCryptEncrypt(_hKey, _pInput, _cbInput, _pPaddingInfo, _pIV, _cbIV, _pOutput, _cbOutput, _pcbResult, _fFlags);
        }

        if (!Is<BCryptKey>(_hKey))
        {
            return STATUS_INVALID_HANDLE;
        }

        return reinterpret_cast<BCryptKey*>(_hKey)->Encrypt(_pInput, _cbInput, _pPaddingInfo, _pIV, _cbIV, _pOutput, _cbOutput, _pcbResult, _fFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    40,
    NTSTATUS,
    WINAPI,
    BCryptDecrypt,
        _Inout_                                     BCRYPT_KEY_HANDLE _hKey,
        _In_reads_bytes_opt_(_cbInput)              PUCHAR _pInput,
        _In_                                        ULONG _cbInput,
        _In_opt_                                    VOID* _pPaddingInfo,
        _Inout_updates_bytes_opt_(_cbIV)            PUCHAR _pIV,
        _In_                                        ULONG _cbIV,
        _Out_writes_bytes_to_opt_(_cbOutput, *_pcbResult) PUCHAR _pOutput,
        _In_                                        ULONG _cbOutput,
        _Out_                                       ULONG* _pcbResult,
        _In_                                        ULONG _fFlags
        )
    {
        if (const auto _pfnBCryptDecrypt = try_get_BCryptDecrypt())
        {
            return _pfnBCryptDecrypt(_hKey, _pInput, _cbInput, _pPaddingInfo, _pIV, _cbIV, _pOutput, _cbOutput, _pcbResult, _fFlags);
        }
            
        if (!Is<BCryptKey>(_hKey))
        {
            return STATUS_INVALID_HANDLE;
        }

        return reinterpret_cast<BCryptKey*>(_hKey)->Decrypt(_pInput, _cbInput, _pPaddingInfo, _pIV, _cbIV, _pOutput, _cbOutput, _pcbResult, _fFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    28,
    NTSTATUS,
    WINAPI,
    BCryptGenerateSymmetricKey,
        _Inout_                             BCRYPT_ALG_HANDLE _hAlgorithm,
        _Out_                               BCRYPT_KEY_HANDLE * _phKey,
        _Out_writes_bytes_all_opt_(cbKeyObject)  PUCHAR _pKeyObject,
        _In_                                ULONG _cbKeyObject,
        _In_reads_bytes_(cbSecret)               PUCHAR _pSecret,
        _In_                                ULONG _cbSecret,
        _In_                                ULONG _fFlags
        )
    {
        if (const auto _pfnBCryptGenerateSymmetricKey = try_get_BCryptGenerateSymmetricKey())
        {
            return _pfnBCryptGenerateSymmetricKey(_hAlgorithm, _phKey, _pKeyObject, _cbKeyObject, _pSecret, _cbSecret, _fFlags);
        }
            
        if (_fFlags)
        {
            return STATUS_INVALID_PARAMETER;
        }


        if (!Is<BCryptAlgorithm>(_hAlgorithm))
        {
            return STATUS_INVALID_HANDLE;
        }

        return reinterpret_cast<BCryptAlgorithm*>(_hAlgorithm)->GenerateSymmetricKey(reinterpret_cast<BCryptKey**>(_phKey), _pKeyObject, _cbKeyObject, _pSecret, _cbSecret, _fFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    4,
    NTSTATUS,
    WINAPI,
    BCryptDestroyKey,
        _Inout_ BCRYPT_KEY_HANDLE _hKey
        )
    {
        if (const auto _pfnBCryptDestroyKey = try_get_BCryptDestroyKey())
        {
            return _pfnBCryptDestroyKey(_hKey);
        }
            
        if (!Is<BCryptKey>(_hKey))
            return STATUS_INVALID_HANDLE;

        reinterpret_cast<BCryptKey*>(_hKey)->Release();
        return STATUS_SUCCESS;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    28,
    NTSTATUS,
    WINAPI,
    BCryptExportKey,
        _In_                                        BCRYPT_KEY_HANDLE   _hKey,
        _In_opt_                                    BCRYPT_KEY_HANDLE   _hExportKey,
        _In_z_                                      LPCWSTR _szBlobType,
        _Out_writes_bytes_to_opt_(_cbOutput, *_pcbResult) PUCHAR   _pOutput,
        _In_                                        ULONG   _cbOutput,
        _Out_                                       ULONG*  _pcbResult,
        _In_                                        ULONG   _fFlags
        )
    {
        if (const auto _pfnBCryptExportKey = try_get_BCryptExportKey())
        {
            return _pfnBCryptExportKey(_hKey, _hExportKey, _szBlobType, _pOutput, _cbOutput, _pcbResult, _fFlags);
        }
            
        if (!Is<BCryptKey>(_hKey))
            return STATUS_INVALID_HANDLE;

        if(_hExportKey && Is<BCryptKey>(_hExportKey) == false)
            return STATUS_INVALID_HANDLE;

        return reinterpret_cast<BCryptKey*>(_hKey)->ExportKey(reinterpret_cast<BCryptKey*>(_hExportKey), _szBlobType, _pOutput, _cbOutput, _pcbResult, _fFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    bcrypt,
    36,
    NTSTATUS,
    WINAPI,
    BCryptImportKey,
        _In_                                BCRYPT_ALG_HANDLE _hAlgorithm,
        _In_opt_                            BCRYPT_KEY_HANDLE _hImportKey,
        _In_z_                              LPCWSTR _szBlobType,
        _Out_                               BCRYPT_KEY_HANDLE* _phKey,
        _Out_writes_bytes_all_opt_(_cbKeyObject)  PUCHAR   _pKeyObject,
        _In_                                ULONG   _cbKeyObject,
        _In_reads_bytes_(_cbInput)                PUCHAR   _pInput,
        _In_                                ULONG   _cbInput,
        _In_                                ULONG   _fFlags
        )
    {
        if (const auto _pfnBCryptImportKey = try_get_BCryptImportKey())
        {
            return _pfnBCryptImportKey(_hAlgorithm, _hImportKey, _szBlobType, _phKey, _pKeyObject, _cbKeyObject, _pInput, _cbInput, _fFlags);
        }
            
        if (!Is<BCryptAlgorithm>(_hAlgorithm))
            return STATUS_INVALID_HANDLE;

        if(_hImportKey && Is<BCryptKey>(_hImportKey) == false)
            return STATUS_INVALID_HANDLE;

        return reinterpret_cast<BCryptAlgorithm*>(_hAlgorithm)->ImportKey(
            reinterpret_cast<BCryptKey*>(_hImportKey),
            _szBlobType,
            reinterpret_cast<BCryptKey**>(_phKey),
            _pKeyObject,
            _cbKeyObject,
            _pInput,
            _cbInput,
            _fFlags);
    }
#endif
}
