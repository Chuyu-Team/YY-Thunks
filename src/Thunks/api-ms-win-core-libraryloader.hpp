
namespace YY
{
	namespace Thunks
	{
	#ifndef YY_Thunks_Defined
		namespace internal
		{
			/*LSTATUS __fastcall BasepGetModuleHandleExParameterValidation(
				_In_ DWORD dwFlags,
				_In_opt_ LPCSTR lpModuleName,
				_Out_ HMODULE* phModule
				)*/
#define BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule) \
				for (;;)                                                           \
				{                                                                  \
					if (phModule)                                                  \
					{                                                              \
						*phModule = nullptr;                                       \
						if ((dwFlags & ~(GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)) == 0                                         \
							|| (dwFlags & (GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)) != (GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)   \
							|| (lpModuleName || (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS) == 0))                                                                                                     \
						{                                                         \
							if (lpModuleName == nullptr)                          \
							{                                                     \
								*phModule = (HMODULE)((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ImageBaseAddress; \
								return TRUE;                                      \
							}                                                     \
							break;                                                \
						}                                                         \
					}                                                             \
					SetLastError(ERROR_INVALID_PARAMETER);                        \
					return FALSE;                                                 \
				}
		}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WINXP)
//Windows XP [desktop apps only]
//Windows Server 2003 [desktop apps only]

EXTERN_C
BOOL
WINAPI
GetModuleHandleExA(
	_In_ DWORD dwFlags,
	_In_opt_ LPCSTR lpModuleName,
	_Out_ HMODULE* phModule
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (const auto pGetModuleHandleExA = try_get_GetModuleHandleExA())
	{
		return pGetModuleHandleExA(dwFlags, lpModuleName, phModule);
	}

	BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule);

	if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
	{
		EnterCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
	}

	LSTATUS lStatus = ERROR_SUCCESS;

	for (;;)
	{
		HMODULE hModule;

		if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
		{
			auto pRtlPcToFileHeader = try_get_RtlPcToFileHeader();
			if (!pRtlPcToFileHeader)
			{
				lStatus = ERROR_NOT_SUPPORTED;
				break;
			}

			hModule = (HMODULE)pRtlPcToFileHeader((PVOID)lpModuleName, (PVOID*)&hModule);
		}
		else
		{
			hModule = GetModuleHandleA(lpModuleName);
		}

		if (hModule == nullptr)
		{
			lStatus = ERROR_DLL_NOT_FOUND;
			break;
		}

		if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)
		{

		}
		else
		{
			const auto pLdrAddRefDll = try_get_LdrAddRefDll();
			if (!pLdrAddRefDll)
			{
				lStatus = ERROR_NOT_SUPPORTED;
				break;
			}

			auto Status = pLdrAddRefDll(dwFlags& GET_MODULE_HANDLE_EX_FLAG_PIN, hModule);
			if (Status < 0)
			{
				lStatus = internal::BaseSetLastNTError(Status);
				break;
			}
		}

		*phModule = hModule;

		break;
	}

	if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
	{
		LeaveCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
	}

	if (lStatus)
	{
		SetLastError(lStatus);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetModuleHandleExA, 12);
#endif

#if (YY_Thunks_Support_Version < NTDDI_WINXP)
//Windows XP [desktop apps only]
//Windows Server 2003 [desktop apps only]

EXTERN_C
BOOL
WINAPI
GetModuleHandleExW(
	_In_ DWORD dwFlags,
	_In_opt_ LPCWSTR lpModuleName,
	_Out_ HMODULE* phModule
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (const auto pGetModuleHandleExW = try_get_GetModuleHandleExW())
	{
		return pGetModuleHandleExW(dwFlags, lpModuleName, phModule);
	}

	BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule);

	if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
	{
		EnterCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
	}

	LSTATUS lStatus = ERROR_SUCCESS;

	for (;;)
	{
		HMODULE hModule;

		if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
		{
			const auto pRtlPcToFileHeader = try_get_RtlPcToFileHeader();
			if (!pRtlPcToFileHeader)
			{
				lStatus = ERROR_NOT_SUPPORTED;
				break;
			}

			hModule = (HMODULE)pRtlPcToFileHeader((PVOID)lpModuleName, (PVOID*)&hModule);
		}
		else
		{
			hModule = GetModuleHandleW(lpModuleName);
		}

		if (hModule == nullptr)
		{
			lStatus = ERROR_DLL_NOT_FOUND;
			break;
		}

		if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)
		{

		}
		else
		{
			const auto pLdrAddRefDll = try_get_LdrAddRefDll();
			if (!pLdrAddRefDll)
			{
				lStatus = ERROR_NOT_SUPPORTED;
				break;
			}

			auto Status = pLdrAddRefDll(dwFlags & GET_MODULE_HANDLE_EX_FLAG_PIN, hModule);
			if (Status < 0)
			{
				lStatus = internal::BaseSetLastNTError(Status);
				break;
			}
		}

		*phModule = hModule;

		break;
	}

	if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
	{
		LeaveCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
	}

	if (lStatus)
	{
		SetLastError(lStatus);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetModuleHandleExW, 12);
#endif

	}
}