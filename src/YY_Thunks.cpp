




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
    _APPLY(GetSystemTimePreciseAsFileTime,               kernel32                                      ) \
    _APPLY(InitializeCriticalSectionEx,                  kernel32                                      ) \
    _APPLY(InitOnceExecuteOnce,                          kernel32                                      ) \
    _APPLY(GetCurrentProcessorNumber,                    kernel32                                      ) \
    _APPLY(GetCurrentProcessorNumberEx,                  kernel32                                      ) \
    _APPLY(GetNumaNodeProcessorMask,                     kernel32                                      ) \
    _APPLY(GetNumaNodeProcessorMaskEx,                   kernel32                                      ) \
    _APPLY(SetThreadGroupAffinity,                       kernel32                                      ) \
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

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS3) && (defined _X86_ || defined _AMD64_)
//Windows 10, Version 1709，坑爹微软MSDN文档有误！
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

			//IA64已经哭晕在厕所
			if (pNativeMachine)
				*pNativeMachine = IMAGE_FILE_MACHINE_AMD64;
		}
		else
		{
			*pProcessMachine = IMAGE_FILE_MACHINE_UNKNOWN;

			if (pNativeMachine)
				*pNativeMachine = IMAGE_FILE_MACHINE_I386;
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

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8, Windows Server 2012
VOID
WINAPI
GetSystemTimePreciseAsFileTime(
	_Out_ LPFILETIME lpSystemTimeAsFileTime
    )
{
	if (auto const pGetSystemTimePreciseAsFileTime = try_get_GetSystemTimePreciseAsFileTime())
	{
		return pGetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
	}

	return GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
}

_LCRT_DEFINE_IAT_SYMBOL(GetSystemTimePreciseAsFileTime, _4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista, Windows Server 2008
BOOL
WINAPI
InitializeCriticalSectionEx(
    _Out_ LPCRITICAL_SECTION lpCriticalSection,
    _In_ DWORD dwSpinCount,
    _In_ DWORD Flags
    )
{
	if (auto const pInitializeCriticalSectionEx = try_get_InitializeCriticalSectionEx())
	{
		return pInitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags);
	}

	return InitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
}

_LCRT_DEFINE_IAT_SYMBOL(InitializeCriticalSectionEx, _12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista, Windows Server 2008
BOOL
WINAPI
InitOnceExecuteOnce(
    _Inout_ PINIT_ONCE InitOnce,
    _In_ __callback PINIT_ONCE_FN InitFn,
    _Inout_opt_ PVOID Parameter,
    _Outptr_opt_result_maybenull_ LPVOID* Context
    )
{
	if (auto const pInitOnceExecuteOnce = try_get_InitOnceExecuteOnce())
	{
		return pInitOnceExecuteOnce(InitOnce, InitFn, Parameter, Context);
	}


	//目标系统不支持，切换到XP兼容模式
	for (;;)
	{
		switch (InterlockedCompareExchange((volatile size_t*)InitOnce, 1, 0))
		{
		case 2:
			//同步完成，并且其他线程已经操作成功
			return TRUE;
			break;
		case 1:
			//尚未完成，继续等待
			Sleep(0);
			break;
		case 0:
			//同步完成，确认是处，调用指定函数
		{
			BOOL bRet = InitFn(InitOnce, Parameter, Context) == TRUE;
			//函数调用完成

			if (InterlockedExchange((volatile size_t*)InitOnce, bRet ? 2 : 0) == 1)
			{
				return bRet;
			}

		}
		default:
			//同步完成，但是发生错误
			SetLastError(ERROR_INVALID_DATA);
			return FALSE;
			break;
		}
	}
}

_LCRT_DEFINE_IAT_SYMBOL(InitOnceExecuteOnce, _16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WS03)
//Windows Vista, Windows Server 2003
DWORD
WINAPI
GetCurrentProcessorNumber(
	VOID
    )
{
	if (auto pGetCurrentProcessorNumber = try_get_GetCurrentProcessorNumber())
	{
		return pGetCurrentProcessorNumber();
	}
	else
	{
		//如果不支持此接口，那么假定是单核
		return 0;
	}

}

_LCRT_DEFINE_IAT_SYMBOL(GetCurrentProcessorNumber, _0);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7, Windows Server 2008 R2
VOID
WINAPI
GetCurrentProcessorNumberEx(
	_Out_ PPROCESSOR_NUMBER ProcNumber
    )
{
	if (auto pGetCurrentProcessorNumberEx = try_get_GetCurrentProcessorNumberEx())
	{
		pGetCurrentProcessorNumberEx(ProcNumber);
	}
	else
	{
		//不支持GetCurrentProcessorNumberEx时假定用户只有一组CPU
		ProcNumber->Group = 0;
		ProcNumber->Number = GetCurrentProcessorNumber();
		ProcNumber->Reserved = 0;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(GetCurrentProcessorNumberEx, _4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WINXPSP2) || defined _X86_
//Windows Vista, Windows XP Professional x64 Edition, Windows XP with SP2, Windows Server 2003
BOOL
WINAPI
GetNumaNodeProcessorMask(
	_In_  UCHAR Node,
	_Out_ PULONGLONG ProcessorMask
    )
{
	if (auto pGetNumaNodeProcessorMask = try_get_GetNumaNodeProcessorMask())
	{
		return pGetNumaNodeProcessorMask(Node, ProcessorMask);
	}
	else
	{
		//不支持此接口
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(GetNumaNodeProcessorMask, _8);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7, Windows Server 2008 R2
BOOL
WINAPI
GetNumaNodeProcessorMaskEx(
	_In_ USHORT Node,
	_Out_ PGROUP_AFFINITY ProcessorMask
)
{
	if (auto pGetNumaNodeProcessorMaskEx = try_get_GetNumaNodeProcessorMaskEx())
	{
		return pGetNumaNodeProcessorMaskEx(Node, ProcessorMask);
	}
	else
	{
		ULONGLONG ullProcessorMask;
		auto bRet = GetNumaNodeProcessorMask(Node, &ullProcessorMask);

		if (bRet)
		{
			ProcessorMask->Mask = ullProcessorMask;
			//假定只有一组CPU
			ProcessorMask->Group = 0;
			ProcessorMask->Reserved[0] = 0;
			ProcessorMask->Reserved[1] = 0;
			ProcessorMask->Reserved[2] = 0;
		}

		return bRet;
	}

}

_LCRT_DEFINE_IAT_SYMBOL(GetNumaNodeProcessorMaskEx, _8);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7, Windows Server 2008 R2
BOOL
WINAPI
SetThreadGroupAffinity(
	_In_ HANDLE hThread,
	_In_ CONST GROUP_AFFINITY* GroupAffinity,
	_Out_opt_ PGROUP_AFFINITY PreviousGroupAffinity
    )
{
	if (auto pSetThreadGroupAffinity = try_get_SetThreadGroupAffinity())
	{
		return pSetThreadGroupAffinity(hThread, GroupAffinity, PreviousGroupAffinity);
	}

	auto NewMask = SetThreadAffinityMask(hThread, GroupAffinity->Mask);

	if (NewMask==0)
	{
		return FALSE;
	}

	if (PreviousGroupAffinity)
	{
		PreviousGroupAffinity->Mask = NewMask;
		PreviousGroupAffinity->Group = 0;
		PreviousGroupAffinity->Reserved[0] = 0;
		PreviousGroupAffinity->Reserved[1] = 0;
		PreviousGroupAffinity->Reserved[2] = 0;
	}

	return TRUE;
}

_LCRT_DEFINE_IAT_SYMBOL(SetThreadGroupAffinity, _12);

#endif

EXTERN_C_END