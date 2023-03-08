#include <bcrypt.h>
#include <wincrypt.h>

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
#pragma comment(lib, "Advapi32.lib")
#endif

namespace YY
{
	namespace Thunks
	{
		

#ifdef YY_Thunks_Implemented
		struct BCryptMapItem
		{
			LPCWSTR szAlgId;
			LPCWSTR szProvider;
			DWORD uProvType;
			DWORD uCalg;
		};

		const BCRYPT_ALG_HANDLE hRngHandle = reinterpret_cast<BCRYPT_ALG_HANDLE>(0xF0);

		static const BCryptMapItem* __fastcall BCryptAlgIdToCryptoAPIProvType(_In_ LPCWSTR _szAlgId)
		{
			if (!_szAlgId)
				return nullptr;

			static const BCryptMapItem g_Map[] =
			{
				// 加密算法
				// { L"AES", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_AES },
				// { L"DES", MS_DEF_DSS_PROV_W, PROV_DSS, CALG_DES },
				// { L"RC2", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_RC2 },
				// { L"RC4", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_RC4 },

				// 生成随机数算法
				{ BCRYPT_RNG_ALGORITHM, nullptr, 0 },
				{ BCRYPT_RNG_FIPS186_DSA_ALGORITHM, nullptr, 0 },
				{ BCRYPT_RNG_DUAL_EC_ALGORITHM, nullptr, 0 },

				// Hash算法
				// { L"MD2", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_MD2 },
				// { L"MD4", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_MD4 },
				// { L"MD5", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_MD5 },
				// { L"SHA1", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_SHA1 },
				// { L"SHA256", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_SHA_256 },
				// { L"SHA384", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_SHA_384 },
				// { L"SHA512", MS_ENH_RSA_AES_PROV_XP_W, PROV_RSA_AES, CALG_SHA_512 },
			};

			for (auto& _Item : g_Map)
			{
				if (wcsicmp(_szAlgId, _Item.szAlgId) == 0)
					return &_Item;
			}

			return nullptr;
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
			if (auto _pfnBCryptOpenAlgorithmProvider = try_get_BCryptOpenAlgorithmProvider())
			{
				return _pfnBCryptOpenAlgorithmProvider(_phAlgorithm, _szAlgId, _szImplementation, _fFlags);
			}

			UNREFERENCED_PARAMETER(_szImplementation);
			UNREFERENCED_PARAMETER(_fFlags);

			auto _pItem = BCryptAlgIdToCryptoAPIProvType(_szAlgId);
			if (!_pItem)
				return STATUS_NOT_FOUND;
			if (_pItem->uCalg == 0)
			{
				// 此外对于Windows XP内部其实只有一种随机算法，所以没必要真的开辟资源
				// 返回一个标识性的常量即可。
				*_phAlgorithm = hRngHandle;
				return STATUS_SUCCESS;
			}
			else
			{
				return STATUS_NOT_FOUND;
			}
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

			UNREFERENCED_PARAMETER(_fFlags);

			if(_hAlgorithm == hRngHandle)
				return STATUS_SUCCESS;
			else
				return STATUS_INVALID_HANDLE;
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
			_Out_writes_bytes_(cbBuffer)    PUCHAR _pbBuffer,
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
				if (_hAlgorithm != hRngHandle)
					return STATUS_INVALID_PARAMETER;
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
	}
}