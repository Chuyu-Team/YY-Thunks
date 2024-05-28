
#include <ncrypt.h>

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
		//NCryptSignHash
		//Minimum supported client	Windows Vista
		//Minimum supported server	Windows Server 2008
		__DEFINE_THUNK(
			ncrypt,
			32,
			SECURITY_STATUS,
			NTAPI,
			NCryptSignHash,
			NCRYPT_KEY_HANDLE hKey,
			VOID* pPaddingInfo,
			PBYTE pbHashValue,
			DWORD cbHashValue,
			PBYTE pbSignature,
			DWORD cbSignature,
			DWORD* pcbResult,
			DWORD dwFlags
		)
		{
			if (auto const pNCryptSignHash = try_get_NCryptSignHash())
			{
				return pNCryptSignHash(hKey, pPaddingInfo, pbHashValue, cbHashValue, pbSignature, cbSignature, pcbResult, dwFlags);
			}

			return  NTE_BAD_FLAGS;
		}
	
#endif

		

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
		//NCryptGetProperty
		//Minimum supported client	Windows Vista
		//Minimum supported server	Windows Server 2008
		__DEFINE_THUNK(
			ncrypt,
			24,
			SECURITY_STATUS,
			NTAPI,
			NCryptGetProperty,
			NCRYPT_HANDLE hObject,
			LPCWSTR pszProperty,
			PBYTE pbOutput,
			DWORD cbOutput,
			DWORD* pcbResult,
			DWORD dwFlags
		)
		{
			if (auto const pNCryptGetProperty = try_get_NCryptGetProperty())
			{
				return pNCryptGetProperty(hObject, pszProperty, pbOutput, cbOutput, pcbResult, dwFlags);
			}

			return  NTE_BAD_FLAGS;
		}
		
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
		//NCryptFreeObject
		//Minimum supported client	Windows Vista
		//Minimum supported server	Windows Server 2008
		__DEFINE_THUNK(
			ncrypt,
			4,
			SECURITY_STATUS,
			NTAPI,
			NCryptFreeObject,
			NCRYPT_HANDLE hObject
		)
		{
			if (auto const pNCryptFreeObject = try_get_NCryptFreeObject())
			{
				return pNCryptFreeObject(hObject);
			}

			return  NTE_BAD_FLAGS;
		}

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //NCryptCreatePersistedKey
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
        __DEFINE_THUNK(
            ncrypt,
            24,
            SECURITY_STATUS,
            NTAPI,
            NCryptCreatePersistedKey,
            _In_    NCRYPT_PROV_HANDLE hProvider,
            _Out_   NCRYPT_KEY_HANDLE* phKey,
            _In_    LPCWSTR pszAlgId,
            _In_opt_ LPCWSTR pszKeyName,
            _In_    DWORD   dwLegacyKeySpec,
            _In_    DWORD   dwFlags
        )
        {
            if (auto const pNCryptCreatePersistedKey = try_get_NCryptCreatePersistedKey())
            {
                return pNCryptCreatePersistedKey(hProvider, phKey, pszAlgId, pszKeyName, dwLegacyKeySpec, dwFlags);
            }

            return  NTE_BAD_FLAGS;
        }

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //NCryptExportKey
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
        __DEFINE_THUNK(
            ncrypt,
            32,
            SECURITY_STATUS,
            NTAPI,
            NCryptExportKey,
            _In_    NCRYPT_KEY_HANDLE hKey,
            _In_opt_ NCRYPT_KEY_HANDLE hExportKey,
            _In_    LPCWSTR pszBlobType,
            _In_opt_ NCryptBufferDesc* pParameterList,
            _Out_writes_bytes_to_opt_(cbOutput, *pcbResult) PBYTE pbOutput,
            _In_    DWORD   cbOutput,
            _Out_   DWORD* pcbResult,
            _In_    DWORD   dwFlags
        )
        {
            if (auto const pNCryptExportKey = try_get_NCryptExportKey())
            {
                return pNCryptExportKey(hKey, hExportKey, pszBlobType, pParameterList, pbOutput, cbOutput, pcbResult, dwFlags);
            }

            return  NTE_BAD_FLAGS;
        }

#endif




#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //NCryptFinalizeKey
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
        __DEFINE_THUNK(
            ncrypt,
            8,
            SECURITY_STATUS,
            NTAPI,
            NCryptFinalizeKey,
            _In_    NCRYPT_KEY_HANDLE hKey,
            _In_    DWORD   dwFlags
        )
        {
            if (auto const pNCryptFinalizeKey = try_get_NCryptFinalizeKey())
            {
                return pNCryptFinalizeKey(hKey, dwFlags);
            }

            return  NTE_BAD_FLAGS;
        }

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //NCryptImportKey
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
        __DEFINE_THUNK(
            ncrypt,
            32,
            SECURITY_STATUS,
            NTAPI,
            NCryptImportKey,
            _In_    NCRYPT_PROV_HANDLE hProvider,
            _In_opt_ NCRYPT_KEY_HANDLE hImportKey,
            _In_    LPCWSTR pszBlobType,
            _In_opt_ NCryptBufferDesc* pParameterList,
            _Out_   NCRYPT_KEY_HANDLE* phKey,
            _In_reads_bytes_(cbData) PBYTE pbData,
            _In_    DWORD   cbData,
            _In_    DWORD   dwFlags
        )
        {
            if (auto const pNCryptImportKey = try_get_NCryptImportKey())
            {
                return pNCryptImportKey(hProvider, hImportKey, pszBlobType, pParameterList, phKey, pbData, cbData, dwFlags);
            }

            return  NTE_BAD_FLAGS;
        }

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //NCryptImportKey
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
        __DEFINE_THUNK(
            ncrypt,
            12,
            SECURITY_STATUS,
            NTAPI,
            NCryptIsAlgSupported,
            _In_    NCRYPT_PROV_HANDLE hProvider,
            _In_    LPCWSTR pszAlgId,
            _In_    DWORD   dwFlags
        )
        {
            if (auto const pNCryptIsAlgSupported = try_get_NCryptIsAlgSupported())
            {
                return pNCryptIsAlgSupported(hProvider, pszAlgId,  dwFlags);
            }

            return  NTE_BAD_FLAGS;
        }

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //NCryptImportKey
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
        __DEFINE_THUNK(
            ncrypt,
            12,
            SECURITY_STATUS,
            NTAPI,
            NCryptOpenStorageProvider,
            _Out_   NCRYPT_PROV_HANDLE* phProvider,
            _In_opt_ LPCWSTR pszProviderName,
            _In_    DWORD   dwFlags
        )
        {
            if (auto const pNCryptOpenStorageProvider = try_get_NCryptOpenStorageProvider())
            {
                return pNCryptOpenStorageProvider(phProvider, pszProviderName, dwFlags);
            }

            return  NTE_BAD_FLAGS;
        }

#endif
		

	}//namespace Thunks

} //namespace YY
