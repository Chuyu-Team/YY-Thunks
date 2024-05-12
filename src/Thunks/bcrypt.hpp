#include <bcrypt.h>
#include <wincrypt.h>

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
#pragma comment(lib, "Advapi32.lib")
#endif

namespace YY
{
	namespace Thunks
	{
		

#if defined(YY_Thunks_Implemented) && YY_Thunks_Support_Version < NTDDI_WIN6
        struct BCryptHash;
        struct BCryptAlgorithm;
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

        class BCryptObject
        {
            static constexpr auto kBCryptObjectMagic = 0x998u;

            DWORD uMagic = kBCryptObjectMagic;
            ULONG uRef = 1u;

        public:
            DWORD uAlgId = 0;
            bool bCanFree = true;

            BCryptObject(DWORD _uAlgId)
                : uAlgId(_uAlgId)
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

            DWORD GetClass() const
            {
                return GET_ALG_CLASS(uAlgId);
            }

            bool IsHash()
            {
                return GetClass() == ALG_CLASS_HASH;
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
        };

        struct BCryptAlgorithm : public BCryptObject
        {
            const BCryptMapItem* pMapItem = nullptr;
            ULONG fOpenAlgorithmFlags = 0;

            BCryptAlgorithm()
                : BCryptObject(0)
            {
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


            NTSTATUS WINAPI GetProperty(
                _In_z_                                      LPCWSTR pszProperty,
                _Out_writes_bytes_to_opt_(cbOutput, *pcbResult) PUCHAR   pbOutput,
                _In_                                        ULONG   cbOutput,
                _Out_                                       ULONG* pcbResult,
                _In_                                        ULONG   dwFlags
                ) override
            {
                if (__wcsnicmp_ascii(BCRYPT_ALGORITHM_NAME, pszProperty, -1) == 0)
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
                else if (__wcsnicmp_ascii(BCRYPT_PROVIDER_HANDLE, pszProperty, -1) == 0)
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

            virtual
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
                return STATUS_NOT_SUPPORTED;
            }
        };

        struct BCryptAlgorithmByCryptoAPI : BCryptAlgorithm
        {
            HCRYPTPROV hProv = NULL;

            ~BCryptAlgorithmByCryptoAPI()
            {
                if (hProv)
                    CryptReleaseContext(hProv, 0);
            }

            template<typename BCryptAlgorithmT>
            static NTSTATUS __fastcall Create(_In_ const BCryptMapItem* _pMapItem, _In_ ULONG _fOpenAlgorithmFlags, _Outptr_ BCryptAlgorithm** _ppAlgorithm)
            {
                *_ppAlgorithm = nullptr;

                HCRYPTPROV _hProv;
                if (!CryptAcquireContextW(&_hProv, nullptr, _pMapItem->szProvider, _pMapItem->uProvType, CRYPT_VERIFYCONTEXT))
                    return STATUS_INVALID_PARAMETER;

                const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
                auto _pBCryptAlgorithm = (BCryptAlgorithmT*)HeapAlloc(_hProcessHeap, 0, sizeof(BCryptAlgorithmT));
                if (!_pBCryptAlgorithm)
                {
                    CryptReleaseContext(_hProv, 0);
                    return STATUS_NO_MEMORY;
                }

                new (_pBCryptAlgorithm) BCryptAlgorithmT();
                _pBCryptAlgorithm->pMapItem = _pMapItem;
                _pBCryptAlgorithm->hProv = _hProv;
                _pBCryptAlgorithm->fOpenAlgorithmFlags = _fOpenAlgorithmFlags;
                *_ppAlgorithm = _pBCryptAlgorithm;
                return STATUS_SUCCESS;
            }
        };

        struct BCryptHash : public BCryptObject
        {
            BCryptAlgorithmByCryptoAPI* pAlgorithm = nullptr;
            ULONG dwFlags = 0;
            HCRYPTKEY hPubKey = NULL;
            HCRYPTHASH hHash = NULL;

            BCryptHash(_In_ BCryptAlgorithmByCryptoAPI* _pAlgorithm)
                : BCryptObject(_pAlgorithm->pMapItem->uAlgId)
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

                    const auto _cbKeyBlob = sizeof(_PLAINTEXTKEYBLOB) + _cbSecret;
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
                if (__wcsnicmp_ascii(BCRYPT_HASH_LENGTH, pszProperty, -1) == 0)
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
                else if (__wcsnicmp_ascii(BCRYPT_ALGORITHM_NAME, pszProperty, -1) == 0)
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
                else if (__wcsnicmp_ascii(BCRYPT_PROVIDER_HANDLE, pszProperty, -1) == 0)
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
        };

        struct BCryptAlgorithmHash : public BCryptAlgorithmByCryptoAPI
        {
            NTSTATUS WINAPI GetProperty(
                _In_z_                                      LPCWSTR pszProperty,
                _Out_writes_bytes_to_opt_(cbOutput, *pcbResult) PUCHAR   pbOutput,
                _In_                                        ULONG   cbOutput,
                _Out_                                       ULONG* pcbResult,
                _In_                                        ULONG   dwFlags
                ) override
            {
                if (__wcsnicmp_ascii(BCRYPT_OBJECT_LENGTH, pszProperty, -1) == 0)
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
                else if (__wcsnicmp_ascii(BCRYPT_HASH_LENGTH, pszProperty, -1) == 0)
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
                else if (__wcsnicmp_ascii(BCRYPT_HASH_BLOCK_LENGTH, pszProperty, -1) == 0)
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

        struct BCryptAlgorithmRng : public BCryptAlgorithm
        {
            static NTSTATUS __fastcall Create(_In_ const BCryptMapItem* _pMapItem, _In_ ULONG _fOpenAlgorithmFlags, _Outptr_ BCryptAlgorithm** _ppAlgorithm)
            {
                *_ppAlgorithm = nullptr;

                const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
                auto _pBCryptAlgorithm = (BCryptAlgorithmRng*)HeapAlloc(_hProcessHeap, 0, sizeof(BCryptAlgorithmRng));
                if (!_pBCryptAlgorithm)
                {
                    return STATUS_NO_MEMORY;
                }

                new (_pBCryptAlgorithm) BCryptAlgorithmRng();
                _pBCryptAlgorithm->pMapItem = _pMapItem;
                _pBCryptAlgorithm->fOpenAlgorithmFlags = _fOpenAlgorithmFlags;
                *_ppAlgorithm = _pBCryptAlgorithm;
                return STATUS_SUCCESS;
            }
        };


        template<typename TargrtObject>
        bool __fastcall Is(void* _pSrc);

        template<>
        bool __fastcall Is<BCryptObject>(void* _pSrc)
        {
            auto _BCryptObject = reinterpret_cast<BCryptObject*>(_pSrc);
            return _BCryptObject != nullptr && _BCryptObject->IsValid();
        }

        template<>
        bool __fastcall Is<BCryptAlgorithm>(void* _pSrc)
        {
            if (!Is<BCryptObject>(_pSrc))
                return false;

            return reinterpret_cast<BCryptObject*>(_pSrc)->GetClass() == 0;
        }

        template<>
        bool __fastcall Is<BCryptAlgorithmRng>(void* _pSrc)
        {
            if (!Is<BCryptAlgorithm>(_pSrc))
                return false;

            return reinterpret_cast<BCryptAlgorithmRng*>(_pSrc)->IsRng();
        }

        template<>
        bool __fastcall Is<BCryptHash>(void* _pSrc)
        {
            if (!Is<BCryptObject>(_pSrc))
                return false;

            return reinterpret_cast<BCryptObject*>(_pSrc)->IsHash();
        }
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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
                // 加密算法
                // { L"AES", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_AES },
                // { L"DES", MS_DEF_DSS_PROV_W, PROV_DSS, CALG_DES },
                // { L"RC2", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_RC2 },
                // { L"RC4", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_RC4 },

                // 生成随机数算法
                { BCRYPT_RNG_ALGORITHM, nullptr, 0, 0, &BCryptAlgorithmRng::Create },
                { BCRYPT_RNG_FIPS186_DSA_ALGORITHM, nullptr, 0, 0, &BCryptAlgorithmRng::Create },
                { BCRYPT_RNG_DUAL_EC_ALGORITHM, nullptr, 0, 0, &BCryptAlgorithmRng::Create },

                // Hash算法
                { L"MD2", nullptr, PROV_RSA_AES, CALG_MD2, &BCryptAlgorithmByCryptoAPI::Create<BCryptAlgorithmHash> },
                { L"MD4", nullptr, PROV_RSA_AES, CALG_MD4, &BCryptAlgorithmByCryptoAPI::Create<BCryptAlgorithmHash> },
                { L"MD5", nullptr, PROV_RSA_AES, CALG_MD5, &BCryptAlgorithmByCryptoAPI::Create<BCryptAlgorithmHash> },
                { L"SHA1", nullptr, PROV_RSA_AES, CALG_SHA1, &BCryptAlgorithmByCryptoAPI::Create<BCryptAlgorithmHash> },
                { L"SHA256", nullptr, PROV_RSA_AES, CALG_SHA_256, &BCryptAlgorithmByCryptoAPI::Create<BCryptAlgorithmHash> },
                { L"SHA384", nullptr, PROV_RSA_AES, CALG_SHA_384, &BCryptAlgorithmByCryptoAPI::Create<BCryptAlgorithmHash> },
                { L"SHA512", nullptr, PROV_RSA_AES, CALG_SHA_512, &BCryptAlgorithmByCryptoAPI::Create<BCryptAlgorithmHash> },
            };

            for (auto& _Item : g_Map)
            {
                if (__wcsnicmp_ascii(_szAlgId, _Item.szAlgName, (size_t)-1) == 0)
                {
                    return _Item.pfnOpenAlgorithmProviderType(&_Item, _fFlags, reinterpret_cast<BCryptAlgorithm**>(_phAlgorithm));
                }
            }

            return STATUS_NOT_FOUND;
		}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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
                if (!Is<BCryptAlgorithmRng>(_hAlgorithm))
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


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

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


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		// 最低受支持的客户端	Windows 7 [桌面应用 |UWP 应用]
        // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		bcrypt,
		40,
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
                && __wcsnicmp_ascii(szAlgorithmNameBuffer, L"AES", -1) == 0)
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
	}
}
