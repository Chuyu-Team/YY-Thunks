




#define _YY_APPLY_TO_LATE_BOUND_MODULES(_APPLY)                                                          \
    _APPLY(kernel32,                                     "kernel32"                                    ) \
	_APPLY(advapi32,                                     "advapi32"                                    ) \
    _APPLY(user32,                                       "user32"                                      ) 


#define _YY_APPLY_TO_LATE_BOUND_FUNCTIONS(_APPLY)                                                        \
    _APPLY(DecodePointer,                                kernel32                                      ) \
    _APPLY(EncodePointer,                                kernel32                                      ) \
    _APPLY(Wow64DisableWow64FsRedirection,               kernel32                                      ) \
    _APPLY(Wow64RevertWow64FsRedirection,                kernel32                                      ) \
    _APPLY(Wow64EnableWow64FsRedirection,                kernel32                                      ) \
    _APPLY(IsWow64Process,                               kernel32                                      ) \
    _APPLY(RegDeleteKeyExW,                              advapi32                                      ) \
    _APPLY(RegDeleteKeyExA,                              advapi32                                      ) \
    _APPLY(IsWow64Message,                               user32                                        ) 



#include "YY_Thunks.h"

//dll 链接不一致
#pragma warning(disable:4273)

EXTERN_C_START

//Windows XP with SP2, Windows Server 2003 with SP1 
PVOID
WINAPI
DecodePointer(
	_In_opt_ PVOID Ptr
	)
{
	if (auto const pDecodePointer = try_get_DecodePointer())
	{
		return pDecodePointer(Ptr);
	}
	else
	{
		return Ptr;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(DecodePointer, _4);

//Windows XP with SP2, Windows Server 2003 with SP1 
PVOID
WINAPI
EncodePointer(
	_In_opt_ PVOID Ptr
	)
{
	if (auto const pEncodePointer = try_get_EncodePointer())
	{
		return pEncodePointer(Ptr);
	}
	else
	{
		return Ptr;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(EncodePointer, _4);


#ifdef _X86_
//Windows XP Professional x64 Edition, Windows Server 2003 with SP1
LSTATUS
APIENTRY
RegDeleteKeyExW(
	_In_ HKEY hKey,
	_In_ LPCWSTR lpSubKey,
	_In_ REGSAM samDesired,
	_Reserved_ DWORD Reserved
	)
{
	if(auto const pRegDeleteKeyExW = try_get_RegDeleteKeyExW())
	{
		return pRegDeleteKeyExW(hKey, lpSubKey, samDesired, Reserved);
	}


	return RegDeleteKeyW(hKey, lpSubKey);
}

_LCRT_DEFINE_IAT_SYMBOL(RegDeleteKeyExW, _16);
#endif

#ifdef _X86_
//Windows XP Professional x64 Edition, Windows Server 2003 with SP1
LSTATUS
APIENTRY
RegDeleteKeyExA(
	_In_ HKEY hKey,
	_In_ LPCSTR lpSubKey,
	_In_ REGSAM samDesired,
	_Reserved_ DWORD Reserved
	)
{
	if (auto const pRegDeleteKeyExA = try_get_RegDeleteKeyExA())
	{
		return pRegDeleteKeyExA(hKey, lpSubKey, samDesired, Reserved);
	}

	return RegDeleteKeyA(hKey, lpSubKey);
}

_LCRT_DEFINE_IAT_SYMBOL(RegDeleteKeyExA, _16);
#endif

#ifdef _X86_
//Windows XP Professional x64 Edition, Windows Server 2003
BOOL
WINAPI
Wow64DisableWow64FsRedirection(
	_Out_ PVOID* OldValue
)
{
	if (auto const pWow64DisableWow64FsRedirection = try_get_Wow64DisableWow64FsRedirection())
	{
		return pWow64DisableWow64FsRedirection(OldValue);
	}


	SetLastError(ERROR_INVALID_FUNCTION);

	return FALSE;
}

_LCRT_DEFINE_IAT_SYMBOL(Wow64DisableWow64FsRedirection, _4);
#endif

#ifdef _X86_
//Windows XP Professional x64 Edition, Windows Server 2003
BOOL
WINAPI
Wow64RevertWow64FsRedirection(
	_In_ PVOID OlValue
	)
{
	if (auto const pWow64RevertWow64FsRedirection = try_get_Wow64RevertWow64FsRedirection())
	{
		return pWow64RevertWow64FsRedirection(OlValue);
	}


	SetLastError(ERROR_INVALID_FUNCTION);

	return FALSE;
}

_LCRT_DEFINE_IAT_SYMBOL(Wow64RevertWow64FsRedirection, _4);
#endif

#ifdef _X86_
//Windows XP Professional x64 Edition, Windows Server 2003
BOOLEAN
WINAPI
Wow64EnableWow64FsRedirection(
	_In_ BOOLEAN Wow64FsEnableRedirection
	)
{
	if (auto const pWow64EnableWow64FsRedirection = try_get_Wow64EnableWow64FsRedirection())
	{
		return pWow64EnableWow64FsRedirection(Wow64FsEnableRedirection);
	}


	SetLastError(ERROR_INVALID_FUNCTION);

	return FALSE;

}

_LCRT_DEFINE_IAT_SYMBOL(Wow64EnableWow64FsRedirection, _4);
#endif

#ifdef _X86_
//Windows XP with SP2, Windows Server 2003 with SP1
BOOL
WINAPI
IsWow64Process(
	_In_ HANDLE hProcess,
	_Out_ PBOOL Wow64Process
	)
{
	if (auto const pIsWow64Process = try_get_IsWow64Process())
	{
		return pIsWow64Process(hProcess, Wow64Process);
	}


	*Wow64Process = FALSE;

	return TRUE;
}

_LCRT_DEFINE_IAT_SYMBOL(IsWow64Process, _8);

#endif


#ifdef _X86_
//Windows XP with SP2, Windows Server 2003 with SP1
BOOL
WINAPI
IsWow64Message(
	VOID)
{
	if (auto const pIsWow64Message = try_get_IsWow64Message())
	{
		return pIsWow64Message();
	}

	return FALSE;
}

_LCRT_DEFINE_IAT_SYMBOL(IsWow64Process, _0);

#endif

EXTERN_C_END