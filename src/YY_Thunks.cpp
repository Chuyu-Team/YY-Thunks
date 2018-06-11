




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
    _APPLY(IsWow64Process2,                              kernel32                                      ) \
    _APPLY(IsWow64GuestMachineSupported,                 kernel32                                      ) \
    _APPLY(GetTickCount64,                               kernel32                                      ) \
    _APPLY(RegDeleteKeyExW,                              advapi32                                      ) \
    _APPLY(RegDeleteKeyExA,                              advapi32                                      ) \
    _APPLY(IsWow64Message,                               user32                                        ) 

#ifndef YY_Thunks_Support_Version
#define YY_Thunks_Support_Version WDK_NTDDI_VERSION
#endif


#include "YY_Thunks.h"
#include <Shlwapi.h>

//dll 链接不一致
#pragma warning(disable:4273)

EXTERN_C_START

#if (YY_Thunks_Support_Version < NTDDI_WS03SP1)
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

#endif

#if (YY_Thunks_Support_Version < NTDDI_WS03SP1)
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

#endif

#if (YY_Thunks_Support_Version < NTDDI_WS03SP1) && defined _X86_
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

#if (YY_Thunks_Support_Version < NTDDI_WS03SP1) && defined _X86_
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

#if (YY_Thunks_Support_Version < NTDDI_WS03) && defined _X86_
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

#if (YY_Thunks_Support_Version < NTDDI_WS03) && defined _X86_
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

#if (YY_Thunks_Support_Version < NTDDI_WS03) && defined _X86_
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

#if (YY_Thunks_Support_Version < NTDDI_WS03SP1) && defined _X86_
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

#if (YY_Thunks_Support_Version < NTDDI_WIN10_TH2) && (defined _X86_ || defined _AMD64_)
//Windows 10, version 1511
BOOL
WINAPI
IsWow64Process2(
	_In_      HANDLE  hProcess,
	_Out_     USHORT* pProcessMachine,
	_Out_opt_ USHORT* pNativeMachine
	)
{
	if (auto const pIsWow64Process2 = try_get_IsWow64Process2())
	{
		return pIsWow64Process2(hProcess, pProcessMachine, pNativeMachine);
	}


	//判断是否运行在Wow6432虚拟机
	BOOL bWow64Process;
	auto bRet = IsWow64Process(hProcess, &bWow64Process);

	if (bRet)
	{
		if (bWow64Process)
		{
			*pProcessMachine = IMAGE_FILE_MACHINE_I386;

			if (pNativeMachine)
				*pNativeMachine = IMAGE_FILE_MACHINE_AMD64;
		}
		else
		{
			*pProcessMachine = IMAGE_FILE_MACHINE_UNKNOWN;

			if (pNativeMachine)
				*pNativeMachine = IMAGE_FILE_MACHINE_UNKNOWN;
		}
	}

	return bRet;
}

_LCRT_DEFINE_IAT_SYMBOL(IsWow64Process2, _12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS3) && (defined _X86_ || defined _AMD64_)
//Windows 10, version 1709
_Must_inspect_result_
HRESULT
WINAPI
IsWow64GuestMachineSupported(
	_In_ USHORT WowGuestMachine,
	_Out_ BOOL* MachineIsSupported
    )
{
	if (auto const pIsWow64GuestMachineSupported = try_get_IsWow64GuestMachineSupported())
	{
		return pIsWow64GuestMachineSupported(WowGuestMachine, MachineIsSupported);
	}

	if (IMAGE_FILE_MACHINE_I386 == WowGuestMachine)
	{
#ifdef _AMD64_
		*MachineIsSupported = TRUE;
#else
		SYSTEM_INFO SystemInfo;
		GetNativeSystemInfo(&SystemInfo);

		*MachineIsSupported = SystemInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL;
#endif

	}
	else
	{
		*MachineIsSupported = FALSE;
	}

	return S_OK;
}

_LCRT_DEFINE_IAT_SYMBOL(IsWow64GuestMachineSupported, _8);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista, Windows Server 2008
ULONGLONG
WINAPI
GetTickCount64(
    VOID
    )
{
	if (auto const pGetTickCount64 = try_get_GetTickCount64())
	{
		return pGetTickCount64();
	}

	return GetTickCount();
}

_LCRT_DEFINE_IAT_SYMBOL(GetTickCount64, _0);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WS03SP1) && defined _X86_
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

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista, Windows Server 2008
LSTATUS
APIENTRY
RegSetKeyValueW(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpSubKey,
	_In_opt_ LPCWSTR lpValueName,
	_In_ DWORD dwType,
	_In_reads_bytes_opt_(cbData) LPCVOID lpData,
	_In_ DWORD cbData
	)
{
	//Empty?
	if (lpSubKey == nullptr || *lpSubKey == L'\0')
	{
		return RegSetValueExW(hKey, lpValueName, 0, dwType, (const BYTE*)lpData, cbData);
	}
	else
	{
		HKEY hSubKey;
		auto lStatus = RegCreateKeyExW(hKey, lpSubKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hSubKey, nullptr);

		if (lStatus == ERROR_SUCCESS)
		{
			lStatus = RegSetValueExW(hSubKey, lpValueName, 0, dwType, (const BYTE*)lpData, cbData);

			RegCloseKey(hSubKey);
		}

		return lStatus;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(RegSetKeyValueW, _24);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista, Windows Server 2008
LSTATUS
APIENTRY
RegSetKeyValueA(
	_In_ HKEY hKey,
	_In_opt_ LPCSTR lpSubKey,
	_In_opt_ LPCSTR lpValueName,
	_In_ DWORD dwType,
	_In_reads_bytes_opt_(cbData) LPCVOID lpData,
	_In_ DWORD cbData
	)
{
	//Empty?
	if (lpSubKey == nullptr || *lpSubKey == '\0')
	{
		return RegSetValueExA(hKey, lpValueName, 0, dwType, (const BYTE*)lpData, cbData);
	}
	else
	{
		HKEY hSubKey;
		auto lStatus = RegCreateKeyExA(hKey, lpSubKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hSubKey, nullptr);

		if (lStatus == ERROR_SUCCESS)
		{
			lStatus = RegSetValueExA(hSubKey, lpValueName, 0, dwType, (const BYTE*)lpData, cbData);

			RegCloseKey(hSubKey);
		}

		return lStatus;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(RegSetKeyValueA, _24);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista, Windows Server 2008
LSTATUS
APIENTRY
RegDeleteKeyValueW(
    _In_ HKEY hKey,
    _In_opt_ LPCWSTR lpSubKey,
    _In_opt_ LPCWSTR lpValueName
    )
{
	HKEY hSubKey;
	auto lStatus = RegOpenKeyExW(hKey, lpSubKey, 0, KEY_SET_VALUE, &hSubKey);

	if (lStatus == ERROR_SUCCESS)
	{
		lStatus = RegDeleteValueW(hSubKey, lpValueName);
		RegCloseKey(hSubKey);
	}

	return lStatus;
}

_LCRT_DEFINE_IAT_SYMBOL(RegDeleteKeyValueW, _12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista, Windows Server 2008
LSTATUS
APIENTRY
RegDeleteKeyValueA(
	_In_ HKEY hKey,
	_In_opt_ LPCSTR lpSubKey,
	_In_opt_ LPCSTR lpValueName
	)
{
	HKEY hSubKey;
	auto lStatus = RegOpenKeyExA(hKey, lpSubKey, 0, KEY_SET_VALUE, &hSubKey);

	if (lStatus == ERROR_SUCCESS)
	{
		lStatus = RegDeleteValueA(hSubKey, lpValueName);
		RegCloseKey(hSubKey);
	}

	return lStatus;
}

_LCRT_DEFINE_IAT_SYMBOL(RegDeleteKeyValueA, _12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista, Windows Server 2008
LSTATUS
APIENTRY
RegDeleteTreeW(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpSubKey
	)
{
	return SHDeleteKeyW(hKey, lpSubKey);
}

_LCRT_DEFINE_IAT_SYMBOL(RegDeleteTreeW, _8);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista, Windows Server 2008
LSTATUS
APIENTRY
RegDeleteTreeA(
    _In_ HKEY hKey,
    _In_opt_ LPCSTR lpSubKey
    )
{
	return SHDeleteKeyA(hKey, lpSubKey);
}

_LCRT_DEFINE_IAT_SYMBOL(RegDeleteTreeA, _8);

#endif

EXTERN_C_END