




#define _YY_APPLY_TO_LATE_BOUND_MODULES(_APPLY)                                                          \
    _APPLY(ntdll,                                        "ntdll"                                       ) \
    _APPLY(kernel32,                                     "kernel32"                                    ) \
	_APPLY(advapi32,                                     "advapi32"                                    ) \
    _APPLY(user32,                                       "user32"                                      ) \
    _APPLY(ws2_32,                                       "ws2_32"                                      ) 


#define _YY_APPLY_TO_LATE_BOUND_FUNCTIONS(_APPLY)                                                        \
    _APPLY(NtQueryDirectoryFile,                         ntdll                                         ) \
    _APPLY(NtQueryInformationFile,                       ntdll                                         ) \
    _APPLY(NtSetInformationFile,                         ntdll                                         ) \
    _APPLY(RtlNtStatusToDosError,                        ntdll                                         ) \
    _APPLY(RtlDosPathNameToNtPathName_U_WithStatus,      ntdll                                         ) \
    _APPLY(RtlFreeUnicodeString,                         ntdll                                         ) \
    _APPLY(NtQueryObject,                                ntdll                                         ) \
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
    _APPLY(LocaleNameToLCID,                             kernel32                                      ) \
    _APPLY(LCIDToLocaleName,                             kernel32                                      ) \
    _APPLY(GetLocaleInfoEx,                              kernel32                                      ) \
    _APPLY(GetDateFormatEx,                              kernel32                                      ) \
    _APPLY(GetTimeFormatEx,                              kernel32                                      ) \
    _APPLY(GetNumberFormatEx,                            kernel32                                      ) \
    _APPLY(GetCurrencyFormatEx,                          kernel32                                      ) \
    _APPLY(GetUserDefaultLocaleName,                     kernel32                                      ) \
    _APPLY(GetSystemDefaultLocaleName,                   kernel32                                      ) \
    _APPLY(EnumCalendarInfoExEx,                         kernel32                                      ) \
    _APPLY(EnumDateFormatsExEx,                          kernel32                                      ) \
    _APPLY(GetFileInformationByHandleEx,                 kernel32                                      ) \
    _APPLY(SetFileInformationByHandle,                   kernel32                                      ) \
    _APPLY(GetFinalPathNameByHandleW,                    kernel32                                      ) \
    _APPLY(GetFinalPathNameByHandleA,                    kernel32                                      ) \
    _APPLY(GetLogicalProcessorInformation,               kernel32                                      ) \
    _APPLY(GetLogicalProcessorInformationEx,             kernel32                                      ) \
    _APPLY(RegDeleteKeyExW,                              advapi32                                      ) \
    _APPLY(RegDeleteKeyExA,                              advapi32                                      ) \
    _APPLY(RegGetValueW,                                 advapi32                                      ) \
    _APPLY(RegGetValueA,                                 advapi32                                      ) \
    _APPLY(IsWow64Message,                               user32                                        ) \
    _APPLY(inet_pton,                                    ws2_32                                        ) \
    _APPLY(InetPtonW,                                    ws2_32                                        ) \
    _APPLY(inet_ntop,                                    ws2_32                                        ) \
    _APPLY(InetNtopW,                                    ws2_32                                        ) 

#ifndef YY_Thunks_Support_Version
#define YY_Thunks_Support_Version WDK_NTDDI_VERSION
#endif

#define _WINSOCKAPI_

#define _Disallow_YY_KM_Namespace
#include "km.h"
#include <Shlwapi.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "YY_Thunks.h"

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

#if defined _X86_
			if (pNativeMachine)
				*pNativeMachine = IMAGE_FILE_MACHINE_I386;
#elif defined _AMD64_
			if (pNativeMachine)
				*pNativeMachine = IMAGE_FILE_MACHINE_AMD64;
#else
#error 不支持此体系
#endif
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


#if (YY_Thunks_Support_Version < NTDDI_WS03SP1) || defined _X86_
//Windows Vista, Windows XP Professional x64 Edition, Windows Server 2008, Windows Server 2003 with SP1
LSTATUS
APIENTRY
RegGetValueW(
    HKEY    hkey,
    LPCWSTR lpSubKey,
    LPCWSTR lpValue,
    DWORD   dwFlags,
    LPDWORD pdwType,
    PVOID   pvData,
    LPDWORD pcbData
    )
{
#if (YY_Thunks_Support_Version >= NTDDI_WINXPSP2)
	return SHRegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
#else
	if (auto const pRegGetValueW = try_get_RegGetValueW())
	{
		return pRegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
	}

	if (pvData && pcbData == nullptr
		|| (dwFlags & RRF_RT_ANY) == 0)
	{
		return ERROR_INVALID_PARAMETER;
	}

	LSTATUS lStatus;

	if (lpSubKey&&*lpSubKey)
	{
		lStatus = RegOpenKeyExW(hkey, lpSubKey, 0, KEY_QUERY_VALUE, &hkey);
		if (lStatus != ERROR_SUCCESS)
			return lStatus;
	}

	DWORD dwType;
	DWORD cbData = pvData && pcbData ? *pcbData : 0;
	const auto cbDataBackup = cbData;


	lStatus = RegQueryValueExW(hkey, lpValue, nullptr, &dwType, (LPBYTE)pvData, &cbData);

	DWORD cbDataFix;
	if (lStatus == ERROR_SUCCESS
		|| lStatus == ERROR_MORE_DATA)
	{
		if (dwType == REG_MULTI_SZ)
		{
			//连续2个 null 结尾
			cbDataFix = cbData + sizeof(*lpValue) * 2;
		}
		else if (dwType == REG_SZ)
		{
			//一个null 结尾
			cbDataFix = cbData + sizeof(*lpValue);
		}
		else if (dwType == REG_EXPAND_SZ)
		{
			if (dwFlags & RRF_NOEXPAND)
			{
				//一个null 结尾
				cbDataFix = cbData + sizeof(*lpValue);
			}
			else
			{
				//需要展开
				cbDataFix = pvData && pcbData ? *pcbData : 0;
				lStatus = SHQueryValueExW(hkey, lpValue, nullptr, &dwType, pvData, &cbDataFix);

				if (lStatus == ERROR_SUCCESS
					|| lStatus == ERROR_MORE_DATA)
				{
					cbData = cbDataFix;
				}
			}
		}
		else
		{
			cbDataFix = cbData;
		}
	}

	if (lpSubKey&&*lpSubKey)
	{
		RegCloseKey(hkey);
	}


	if (lStatus == ERROR_SUCCESS
		|| lStatus == ERROR_MORE_DATA)
	{
		if (pdwType)
			*pdwType = dwType;

		//检测类型是否支持
		switch (dwType)
		{
		case REG_NONE:
		case REG_SZ:
		case REG_EXPAND_SZ:
		case REG_BINARY:
		case REG_DWORD:
			if ((dwFlags & (1 << dwType)) == 0)
			{
				lStatus = ERROR_UNSUPPORTED_TYPE;
				break;
			}
			else if (pvData && lStatus == ERROR_SUCCESS)
			{
				//当函数成功时，保证字符串正常 NULL 结尾
				if (REG_SZ == dwType || REG_EXPAND_SZ == dwType)
				{
					//保证1个 NULL 结尾


					//sizeof(wchar_t) 边界对齐
					if (cbData % sizeof(*lpValue))
					{
						if (cbData >= cbDataBackup)
						{
							lStatus = ERROR_MORE_DATA;
							break;
						}

						((byte*)pvData)[cbData] = 0;

						++cbData;
					}


					auto cStringMin = cbData / sizeof(*lpValue);


					auto pString = (wchar_t*)pvData;

					if (cStringMin == 0 || pString[cStringMin - 1])
					{
						if (cbData + sizeof(*lpValue) > cbDataBackup)
						{
							lStatus = ERROR_MORE_DATA;
							break;
						}

						pString[cStringMin++] = L'\0';
					}

					cbDataFix = cStringMin * sizeof(*lpValue);
				}
			}
			break;
		case REG_MULTI_SZ:
			if ((dwFlags & RRF_RT_REG_MULTI_SZ) == 0)
			{
				lStatus = ERROR_UNSUPPORTED_TYPE;
			}
			else if (pvData && lStatus == ERROR_SUCCESS)
			{
				//保证最后一个字符串 NULL 结尾 并且 包含 NULL 结束符

				//sizeof(wchar_t) 边界对齐
				if (cbData % sizeof(*lpValue))
				{
					if (cbData >= cbDataBackup)
					{
						lStatus = ERROR_MORE_DATA;
						break;
					}

					((byte*)pvData)[cbData] = 0;

					++cbData;
				}

				auto cStringMin = cbData / sizeof(*lpValue);

				auto pString = (wchar_t*)pvData;

				if (cStringMin == 0)
				{
					if (cbData + sizeof(*lpValue) > cbDataBackup)
					{
						lStatus = ERROR_MORE_DATA;
						break;
					}

					pString[cStringMin++] = L'\0';
				}
				else if (pString[cStringMin - 1])
				{
					if (cbData + sizeof(*lpValue) * 2 > cbDataBackup)
					{
						lStatus = ERROR_MORE_DATA;
						break;
					}

					pString[cStringMin++] = L'\0';

					pString[cStringMin++] = L'\0';
				}
				else if (cStringMin >= 2 && pString[cStringMin - 2])
				{
					if (cbData + sizeof(*lpValue) > cbDataBackup)
					{
						lStatus = ERROR_MORE_DATA;
						break;
					}

					pString[cStringMin++] = L'\0';
				}

				cbDataFix = cStringMin * sizeof(*lpValue);
			}
			break;
		case REG_QWORD:
			if ((dwFlags & RRF_RT_REG_QWORD) == 0)
			{
				lStatus = ERROR_UNSUPPORTED_TYPE;
			}
			break;
		default:
			if((dwFlags & RRF_RT_ANY) != RRF_RT_ANY)
				lStatus = ERROR_UNSUPPORTED_TYPE;
			break;
		}

		if (pcbData)
			*pcbData = cbDataFix;

	}


	if ((RRF_ZEROONFAILURE & dwFlags) != 0 && lStatus)
	{
		//全0填充
		if (pvData && cbDataBackup)
		{
			memset(pvData, 0, cbDataBackup);
		}
	}

	return lStatus;
#endif
}

_LCRT_DEFINE_IAT_SYMBOL(RegGetValueW, _28);


LSTATUS
APIENTRY
RegGetValueA(
    HKEY    hkey,
    LPCSTR  lpSubKey,
    LPCSTR  lpValue,
    DWORD   dwFlags,
    LPDWORD pdwType,
    PVOID   pvData,
    LPDWORD pcbData
    )
{
#if (YY_Thunks_Support_Version >= NTDDI_WINXPSP2)
	return SHRegGetValueA(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
#else
	if (auto const pRegGetValueA = try_get_RegGetValueA())
	{
		return pRegGetValueA(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
	}

	if (pvData && pcbData == nullptr
		|| (dwFlags & RRF_RT_ANY) == 0)
	{
		return ERROR_INVALID_PARAMETER;
	}

	LSTATUS lStatus;

	if (lpSubKey&&*lpSubKey)
	{
		lStatus = RegOpenKeyExA(hkey, lpSubKey, 0, KEY_QUERY_VALUE, &hkey);
		if (lStatus != ERROR_SUCCESS)
			return lStatus;
	}

	DWORD dwType;
	DWORD cbData = pvData && pcbData ? *pcbData : 0;
	const auto cbDataBackup = cbData;


	lStatus = RegQueryValueExA(hkey, lpValue, nullptr, &dwType, (LPBYTE)pvData, &cbData);

	DWORD cbDataFix;
	if (lStatus == ERROR_SUCCESS
		|| lStatus == ERROR_MORE_DATA)
	{
		if (dwType == REG_MULTI_SZ)
		{
			//连续2个 null 结尾
			cbDataFix = cbData + sizeof(*lpValue) * 2;
		}
		else if (dwType == REG_SZ)
		{
			//一个null 结尾
			cbDataFix = cbData + sizeof(*lpValue);
		}
		else if (dwType == REG_EXPAND_SZ)
		{
			if (dwFlags & RRF_NOEXPAND)
			{
				//一个null 结尾
				cbDataFix = cbData + sizeof(*lpValue);
			}
			else
			{
				//需要展开
				cbDataFix = pvData && pcbData ? *pcbData : 0;
				lStatus = SHQueryValueExA(hkey, lpValue, nullptr, &dwType, pvData, &cbDataFix);

				if (lStatus == ERROR_SUCCESS
					|| lStatus == ERROR_MORE_DATA)
				{
					cbData = cbDataFix;
				}
			}
		}
		else
		{
			cbDataFix = cbData;
		}
	}

	if (lpSubKey&&*lpSubKey)
	{
		RegCloseKey(hkey);
	}


	if (lStatus == ERROR_SUCCESS
		|| lStatus == ERROR_MORE_DATA)
	{
		if (pdwType)
			*pdwType = dwType;

		//检测类型是否支持
		switch (dwType)
		{
		case REG_NONE:
		case REG_SZ:
		case REG_EXPAND_SZ:
		case REG_BINARY:
		case REG_DWORD:
			if ((dwFlags & (1<< dwType)) == 0)
			{
				lStatus = ERROR_UNSUPPORTED_TYPE;
			}
			else if (pvData && lStatus == ERROR_SUCCESS)
			{
				//当函数成功时，保证字符串正常 NULL 结尾
				if (REG_SZ == dwType || REG_EXPAND_SZ == dwType)
				{
					//保证1个 NULL 结尾
					auto cStringMin = cbData;


					auto pString = (char*)pvData;

					if (cStringMin == 0 || pString[cStringMin - 1])
					{
						if (cbData + sizeof(*lpValue) > cbDataBackup)
						{
							lStatus = ERROR_MORE_DATA;
							break;
						}

						pString[cStringMin++] = '\0';
					}

					cbDataFix = cStringMin * sizeof(*lpValue);
				}				
			}
			break;
		case REG_MULTI_SZ:
			if ((dwFlags & RRF_RT_REG_MULTI_SZ) == 0)
			{
				lStatus = ERROR_UNSUPPORTED_TYPE;
			}
			else
			{
				//保证最后一个字符串 NULL 结尾 并且 包含 NULL 结束符

				auto cStringMin = cbData;

				auto pString = (char*)pvData;

				if (cStringMin == 0)
				{
					if (cbData + sizeof(*lpValue) > cbDataBackup)
					{
						lStatus = ERROR_MORE_DATA;
						break;
					}

					pString[cStringMin++] = '\0';
				}
				else if (pString[cStringMin - 1])
				{
					if (cbData + sizeof(*lpValue) * 2 > cbDataBackup)
					{
						lStatus = ERROR_MORE_DATA;
						break;
					}

					pString[cStringMin++] = '\0';

					pString[cStringMin++] = '\0';
				}
				else if (cStringMin >= 2 && pString[cStringMin - 2])
				{
					if (cbData + sizeof(*lpValue) > cbDataBackup)
					{
						lStatus = ERROR_MORE_DATA;
						break;
					}

					pString[cStringMin++] = '\0';
				}

				cbDataFix = cStringMin * sizeof(*lpValue);
			}
			break;
		case REG_QWORD:
			if ((dwFlags & RRF_RT_REG_QWORD) == 0)
			{
				lStatus = ERROR_UNSUPPORTED_TYPE;
			}
			break;
		default:
			if((dwFlags & RRF_RT_ANY) != RRF_RT_ANY)
				lStatus = ERROR_UNSUPPORTED_TYPE;
			break;
		}

		if (pcbData)
			*pcbData = cbDataFix;

	}


	if ((RRF_ZEROONFAILURE & dwFlags) != 0 && lStatus)
	{
		//全0填充
		if (pvData && cbDataBackup)
		{
			memset(pvData, 0, cbDataBackup);
		}
	}

	return lStatus;
#endif
}

_LCRT_DEFINE_IAT_SYMBOL(RegGetValueA, _28);
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

LCID
WINAPI
LocaleNameToLCID(
	_In_ LPCWSTR lpName,
	_In_ DWORD dwFlags)
{
	if (auto pLocaleNameToLCID = try_get_LocaleNameToLCID())
	{
		return pLocaleNameToLCID(lpName, dwFlags);
	}

	//UNREFRENCED_PARAMETER(dwFlags);

	if (lpName == nullptr)
	{
		return GetUserDefaultLCID();
	}
	else if (__wcsnicmp_ascii(lpName, LOCALE_NAME_SYSTEM_DEFAULT, _countof(LOCALE_NAME_SYSTEM_DEFAULT)) == 0)
	{
		return GetSystemDefaultLCID();
	}


	// Map of locale name to LCID.
	struct LocaleNameToLcid
	{
		const wchar_t*  localeName;
		LANGID          lcid;
	};


	// Map of locale name to an index in LcidToLocaleNameTable, for Windows XP.
	// Data in this table has been obtained from National Language Support (NLS) API Reference at
	// http://msdn.microsoft.com/en-us/goglobal/bb896001.aspx
	// The table is sorted to improve search performance.
	static const LocaleNameToLcid LocaleNameToLcidTable[] =
	{
		{ L""           , 0x007f },
		{ L"af"         , 0x0036 },
		{ L"af-ZA"      , 0x0436 },
		{ L"ar"         , 0x0001 },
		{ L"ar-AE"      , 0x3801 },
		{ L"ar-BH"      , 0x3C01 },
		{ L"ar-DZ"      , 0x1401 },
		{ L"ar-EG"      , 0x0C01 },
		{ L"ar-IQ"      , 0x0801 },
		{ L"ar-JO"      , 0x2C01 },
		{ L"ar-KW"      , 0x3401 },
		{ L"ar-LB"      , 0x3001 },
		{ L"ar-LY"      , 0x1001 },
		{ L"ar-MA"      , 0x1801 },
		{ L"ar-OM"      , 0x2001 },
		{ L"ar-QA"      , 0x4001 },
		{ L"ar-SA"      , 0x0401 },
		{ L"ar-SY"      , 0x2801 },
		{ L"ar-TN"      , 0x1C01 },
		{ L"ar-YE"      , 0x2401 },
		{ L"az"         , 0x002C },
		{ L"az-AZ-Cyrl" , 0x082C },
		{ L"az-AZ-Latn" , 0x042C },
		{ L"be"         , 0x0023 },
		{ L"be-BY"      , 0x0423 },
		{ L"bg"         , 0x0002 },
		{ L"bg-BG"      , 0x0402 },
		{ L"bn-IN"      , 0x0445 },
		{ L"bs-BA-Latn" , 0x141A },
		{ L"ca"         , 0x0003 },
		{ L"ca-ES"      , 0x0403 },
		{ L"cs"         , 0x0005 },
		{ L"cs-CZ"      , 0x0405 },
		{ L"cy-GB"      , 0x0452 },
		{ L"da"         , 0x0006 },
		{ L"da-DK"      , 0x0406 },
		{ L"de"         , 0x0007 },
		{ L"de-AT"      , 0x0C07 },
		{ L"de-CH"      , 0x0807 },
		{ L"de-DE"      , 0x0407 },
		{ L"de-LI"      , 0x1407 },
		{ L"de-LU"      , 0x1007 },
		{ L"div"        , 0x0065 },
		{ L"div-MV"     , 0x0465 },
		{ L"el"         , 0x0008 },
		{ L"el-GR"      , 0x0408 },
		{ L"en"         , 0x0009 },
		{ L"en-AU"      , 0x0C09 },
		{ L"en-BZ"      , 0x2809 },
		{ L"en-CA"      , 0x1009 },
		{ L"en-CB"      , 0x2409 },
		{ L"en-GB"      , 0x0809 },
		{ L"en-IE"      , 0x1809 },
		{ L"en-JM"      , 0x2009 },
		{ L"en-NZ"      , 0x1409 },
		{ L"en-PH"      , 0x3409 },
		{ L"en-TT"      , 0x2C09 },
		{ L"en-US"      , 0x0409 },
		{ L"en-ZA"      , 0x1C09 },
		{ L"en-ZW"      , 0x3009 },
		{ L"es"         , 0x000A },
		{ L"es-AR"      , 0x2C0A },
		{ L"es-BO"      , 0x400A },
		{ L"es-CL"      , 0x340A },
		{ L"es-CO"      , 0x240A },
		{ L"es-CR"      , 0x140A },
		{ L"es-DO"      , 0x1C0A },
		{ L"es-EC"      , 0x300A },
		{ L"es-ES"      , 0x0C0A },
		{ L"es-GT"      , 0x100A },
		{ L"es-HN"      , 0x480A },
		{ L"es-MX"      , 0x080A },
		{ L"es-NI"      , 0x4C0A },
		{ L"es-PA"      , 0x180A },
		{ L"es-PE"      , 0x280A },
		{ L"es-PR"      , 0x500A },
		{ L"es-PY"      , 0x3C0A },
		{ L"es-SV"      , 0x440A },
		{ L"es-UY"      , 0x380A },
		{ L"es-VE"      , 0x200A },
		{ L"et"         , 0x0025 },
		{ L"et-EE"      , 0x0425 },
		{ L"eu"         , 0x002D },
		{ L"eu-ES"      , 0x042D },
		{ L"fa"         , 0x0029 },
		{ L"fa-IR"      , 0x0429 },
		{ L"fi"         , 0x000B },
		{ L"fi-FI"      , 0x040B },
		{ L"fo"         , 0x0038 },
		{ L"fo-FO"      , 0x0438 },
		{ L"fr"         , 0x000C },
		{ L"fr-BE"      , 0x080C },
		{ L"fr-CA"      , 0x0C0C },
		{ L"fr-CH"      , 0x100C },
		{ L"fr-FR"      , 0x040C },
		{ L"fr-LU"      , 0x140C },
		{ L"fr-MC"      , 0x180C },
		{ L"gl"         , 0x0056 },
		{ L"gl-ES"      , 0x0456 },
		{ L"gu"         , 0x0047 },
		{ L"gu-IN"      , 0x0447 },
		{ L"he"         , 0x000D },
		{ L"he-IL"      , 0x040D },
		{ L"hi"         , 0x0039 },
		{ L"hi-IN"      , 0x0439 },
		{ L"hr"         , 0x001A },
		{ L"hr-BA"      , 0x101A },
		{ L"hr-HR"      , 0x041A },
		{ L"hu"         , 0x000E },
		{ L"hu-HU"      , 0x040E },
		{ L"hy"         , 0x002B },
		{ L"hy-AM"      , 0x042B },
		{ L"id"         , 0x0021 },
		{ L"id-ID"      , 0x0421 },
		{ L"is"         , 0x000F },
		{ L"is-IS"      , 0x040F },
		{ L"it"         , 0x0010 },
		{ L"it-CH"      , 0x0810 },
		{ L"it-IT"      , 0x0410 },
		{ L"ja"         , 0x0011 },
		{ L"ja-JP"      , 0x0411 },
		{ L"ka"         , 0x0037 },
		{ L"ka-GE"      , 0x0437 },
		{ L"kk"         , 0x003F },
		{ L"kk-KZ"      , 0x043F },
		{ L"kn"         , 0x004B },
		{ L"kn-IN"      , 0x044B },
		{ L"ko"         , 0x0012 },
		{ L"ko-KR"      , 0x0412 },
		{ L"kok"        , 0x0057 },
		{ L"kok-IN"     , 0x0457 },
		{ L"ky"         , 0x0040 },
		{ L"ky-KG"      , 0x0440 },
		{ L"lt"         , 0x0027 },
		{ L"lt-LT"      , 0x0427 },
		{ L"lv"         , 0x0026 },
		{ L"lv-LV"      , 0x0426 },
		{ L"mi-NZ"      , 0x0481 },
		{ L"mk"         , 0x002F },
		{ L"mk-MK"      , 0x042F },
		{ L"ml-IN"      , 0x044C },
		{ L"mn"         , 0x0050 },
		{ L"mn-MN"      , 0x0450 },
		{ L"mr"         , 0x004E },
		{ L"mr-IN"      , 0x044E },
		{ L"ms"         , 0x003E },
		{ L"ms-BN"      , 0x083E },
		{ L"ms-MY"      , 0x043E },
		{ L"mt-MT"      , 0x043A },
		{ L"nb-NO"      , 0x0414 },
		{ L"nl"         , 0x0013 },
		{ L"nl-BE"      , 0x0813 },
		{ L"nl-NL"      , 0x0413 },
		{ L"nn-NO"      , 0x0814 },
		{ L"no"         , 0x0014 },
		{ L"ns-ZA"      , 0x046C },
		{ L"pa"         , 0x0046 },
		{ L"pa-IN"      , 0x0446 },
		{ L"pl"         , 0x0015 },
		{ L"pl-PL"      , 0x0415 },
		{ L"pt"         , 0x0016 },
		{ L"pt-BR"      , 0x0416 },
		{ L"pt-PT"      , 0x0816 },
		{ L"quz-BO"     , 0x046B },
		{ L"quz-EC"     , 0x086B },
		{ L"quz-PE"     , 0x0C6B },
		{ L"ro"         , 0x0018 },
		{ L"ro-RO"      , 0x0418 },
		{ L"ru"         , 0x0019 },
		{ L"ru-RU"      , 0x0419 },
		{ L"sa"         , 0x004F },
		{ L"sa-IN"      , 0x044F },
		{ L"se-FI"      , 0x0C3B },
		{ L"se-NO"      , 0x043B },
		{ L"se-SE"      , 0x083B },
		{ L"sk"         , 0x001B },
		{ L"sk-SK"      , 0x041B },
		{ L"sl"         , 0x0024 },
		{ L"sl-SI"      , 0x0424 },
		{ L"sma-NO"     , 0x183B },
		{ L"sma-SE"     , 0x1C3B },
		{ L"smj-NO"     , 0x103B },
		{ L"smj-SE"     , 0x143B },
		{ L"smn-FI"     , 0x243B },
		{ L"sms-FI"     , 0x203B },
		{ L"sq"         , 0x001C },
		{ L"sq-AL"      , 0x041C },
		{ L"sr"         , 0x7C1A },
		{ L"sr-BA-Cyrl" , 0x1C1A },
		{ L"sr-BA-Latn" , 0x181A },
		{ L"sr-SP-Cyrl" , 0x0C1A },
		{ L"sr-SP-Latn" , 0x081A },
		{ L"sv"         , 0x001D },
		{ L"sv-FI"      , 0x081D },
		{ L"sv-SE"      , 0x041D },
		{ L"sw"         , 0x0041 },
		{ L"sw-KE"      , 0x0441 },
		{ L"syr"        , 0x005A },
		{ L"syr-SY"     , 0x045A },
		{ L"ta"         , 0x0049 },
		{ L"ta-IN"      , 0x0449 },
		{ L"te"         , 0x004A },
		{ L"te-IN"      , 0x044A },
		{ L"th"         , 0x001E },
		{ L"th-TH"      , 0x041E },
		{ L"tn-ZA"      , 0x0432 },
		{ L"tr"         , 0x001F },
		{ L"tr-TR"      , 0x041F },
		{ L"tt"         , 0x0044 },
		{ L"tt-RU"      , 0x0444 },
		{ L"uk"         , 0x0022 },
		{ L"uk-UA"      , 0x0422 },
		{ L"ur"         , 0x0020 },
		{ L"ur-PK"      , 0x0420 },
		{ L"uz"         , 0x0043 },
		{ L"uz-UZ-Cyrl" , 0x0843 },
		{ L"uz-UZ-Latn" , 0x0443 },
		{ L"vi"         , 0x002A },
		{ L"vi-VN"      , 0x042A },
		{ L"xh-ZA"      , 0x0434 },
		{ L"zh-CHS"     , 0x0004 },
		{ L"zh-CHT"     , 0x7C04 },
		{ L"zh-CN"      , 0x0804 },
		{ L"zh-HK"      , 0x0C04 },
		{ L"zh-MO"      , 0x1404 },
		{ L"zh-SG"      , 0x1004 },
		{ L"zh-TW"      , 0x0404 },
		{ L"zu-ZA"      , 0x0435 }
	};



	int bottom = 0;
	int top = _countof(LocaleNameToLcidTable) - 1;

	while (bottom <= top)
	{
		int middle = (bottom + top) / 2;
		int testIndex = __wcsnicmp_ascii(lpName, LocaleNameToLcidTable[middle].localeName, LOCALE_NAME_MAX_LENGTH);

		if (testIndex == 0)
			return LocaleNameToLcidTable[middle].lcid;

		if (testIndex < 0)
			top = middle - 1;
		else
			bottom = middle + 1;
	}

	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
}

_LCRT_DEFINE_IAT_SYMBOL(LocaleNameToLCID, _8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

int
WINAPI
LCIDToLocaleName(
	_In_ LCID     Locale,
	_Out_writes_opt_(cchName) LPWSTR  lpName,
	_In_ int      cchName,
	_In_ DWORD    dwFlags)
{
	if (auto pLCIDToLocaleName = try_get_LCIDToLocaleName())
	{
		return pLCIDToLocaleName(Locale, lpName, cchName, dwFlags);
	}

	if (Locale == 0 || (lpName == nullptr && cchName > 0) || cchName < 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	if (Locale == LOCALE_USER_DEFAULT)
		Locale = GetUserDefaultLCID();
	else if (Locale == LOCALE_SYSTEM_DEFAULT)
		Locale = GetSystemDefaultLCID();

	// Map of LCID to locale name.
	struct LcidToLocaleName
	{
		LANGID          lcid;
		const wchar_t*  localeName;
	};

	// Map of LCID to locale name for Windows XP.
	// Data in this table has been obtained from National Language Support (NLS) API Reference at
	// http://msdn.microsoft.com/en-us/goglobal/bb896001.aspx
	// The table is sorted to improve search performance.
	static const LcidToLocaleName LcidToLocaleNameTable[] =
	{
		{ 0x0001, L"ar"         },
		{ 0x0002, L"bg"         },
		{ 0x0003, L"ca"         },
		{ 0x0004, L"zh-CHS"     },
		{ 0x0005, L"cs"         },
		{ 0x0006, L"da"         },
		{ 0x0007, L"de"         },
		{ 0x0008, L"el"         },
		{ 0x0009, L"en"         },
		{ 0x000A, L"es"         },
		{ 0x000B, L"fi"         },
		{ 0x000C, L"fr"         },
		{ 0x000D, L"he"         },
		{ 0x000E, L"hu"         },
		{ 0x000F, L"is"         },
		{ 0x0010, L"it"         },
		{ 0x0011, L"ja"         },
		{ 0x0012, L"ko"         },
		{ 0x0013, L"nl"         },
		{ 0x0014, L"no"         },
		{ 0x0015, L"pl"         },
		{ 0x0016, L"pt"         },
		{ 0x0018, L"ro"         },
		{ 0x0019, L"ru"         },
		{ 0x001A, L"hr"         },
		{ 0x001B, L"sk"         },
		{ 0x001C, L"sq"         },
		{ 0x001D, L"sv"         },
		{ 0x001E, L"th"         },
		{ 0x001F, L"tr"         },
		{ 0x0020, L"ur"         },
		{ 0x0021, L"id"         },
		{ 0x0022, L"uk"         },
		{ 0x0023, L"be"         },
		{ 0x0024, L"sl"         },
		{ 0x0025, L"et"         },
		{ 0x0026, L"lv"         },
		{ 0x0027, L"lt"         },
		{ 0x0029, L"fa"         },
		{ 0x002A, L"vi"         },
		{ 0x002B, L"hy"         },
		{ 0x002C, L"az"         },
		{ 0x002D, L"eu"         },
		{ 0x002F, L"mk"         },
		{ 0x0036, L"af"         },
		{ 0x0037, L"ka"         },
		{ 0x0038, L"fo"         },
		{ 0x0039, L"hi"         },
		{ 0x003E, L"ms"         },
		{ 0x003F, L"kk"         },
		{ 0x0040, L"ky"         },
		{ 0x0041, L"sw"         },
		{ 0x0043, L"uz"         },
		{ 0x0044, L"tt"         },
		{ 0x0046, L"pa"         },
		{ 0x0047, L"gu"         },
		{ 0x0049, L"ta"         },
		{ 0x004A, L"te"         },
		{ 0x004B, L"kn"         },
		{ 0x004E, L"mr"         },
		{ 0x004F, L"sa"         },
		{ 0x0050, L"mn"         },
		{ 0x0056, L"gl"         },
		{ 0x0057, L"kok"        },
		{ 0x005A, L"syr"        },
		{ 0x0065, L"div"        },
		{ 0x007f, L""           },
		{ 0x0401, L"ar-SA"      },
		{ 0x0402, L"bg-BG"      },
		{ 0x0403, L"ca-ES"      },
		{ 0x0404, L"zh-TW"      },
		{ 0x0405, L"cs-CZ"      },
		{ 0x0406, L"da-DK"      },
		{ 0x0407, L"de-DE"      },
		{ 0x0408, L"el-GR"      },
		{ 0x0409, L"en-US"      },
		{ 0x040B, L"fi-FI"      },
		{ 0x040C, L"fr-FR"      },
		{ 0x040D, L"he-IL"      },
		{ 0x040E, L"hu-HU"      },
		{ 0x040F, L"is-IS"      },
		{ 0x0410, L"it-IT"      },
		{ 0x0411, L"ja-JP"      },
		{ 0x0412, L"ko-KR"      },
		{ 0x0413, L"nl-NL"      },
		{ 0x0414, L"nb-NO"      },
		{ 0x0415, L"pl-PL"      },
		{ 0x0416, L"pt-BR"      },
		{ 0x0418, L"ro-RO"      },
		{ 0x0419, L"ru-RU"      },
		{ 0x041A, L"hr-HR"      },
		{ 0x041B, L"sk-SK"      },
		{ 0x041C, L"sq-AL"      },
		{ 0x041D, L"sv-SE"      },
		{ 0x041E, L"th-TH"      },
		{ 0x041F, L"tr-TR"      },
		{ 0x0420, L"ur-PK"      },
		{ 0x0421, L"id-ID"      },
		{ 0x0422, L"uk-UA"      },
		{ 0x0423, L"be-BY"      },
		{ 0x0424, L"sl-SI"      },
		{ 0x0425, L"et-EE"      },
		{ 0x0426, L"lv-LV"      },
		{ 0x0427, L"lt-LT"      },
		{ 0x0429, L"fa-IR"      },
		{ 0x042A, L"vi-VN"      },
		{ 0x042B, L"hy-AM"      },
		{ 0x042C, L"az-AZ-Latn" },
		{ 0x042D, L"eu-ES"      },
		{ 0x042F, L"mk-MK"      },
		{ 0x0432, L"tn-ZA"      },
		{ 0x0434, L"xh-ZA"      },
		{ 0x0435, L"zu-ZA"      },
		{ 0x0436, L"af-ZA"      },
		{ 0x0437, L"ka-GE"      },
		{ 0x0438, L"fo-FO"      },
		{ 0x0439, L"hi-IN"      },
		{ 0x043A, L"mt-MT"      },
		{ 0x043B, L"se-NO"      },
		{ 0x043E, L"ms-MY"      },
		{ 0x043F, L"kk-KZ"      },
		{ 0x0440, L"ky-KG"      },
		{ 0x0441, L"sw-KE"      },
		{ 0x0443, L"uz-UZ-Latn" },
		{ 0x0444, L"tt-RU"      },
		{ 0x0445, L"bn-IN"      },
		{ 0x0446, L"pa-IN"      },
		{ 0x0447, L"gu-IN"      },
		{ 0x0449, L"ta-IN"      },
		{ 0x044A, L"te-IN"      },
		{ 0x044B, L"kn-IN"      },
		{ 0x044C, L"ml-IN"      },
		{ 0x044E, L"mr-IN"      },
		{ 0x044F, L"sa-IN"      },
		{ 0x0450, L"mn-MN"      },
		{ 0x0452, L"cy-GB"      },
		{ 0x0456, L"gl-ES"      },
		{ 0x0457, L"kok-IN"     },
		{ 0x045A, L"syr-SY"     },
		{ 0x0465, L"div-MV"     },
		{ 0x046B, L"quz-BO"     },
		{ 0x046C, L"ns-ZA"      },
		{ 0x0481, L"mi-NZ"      },
		{ 0x0801, L"ar-IQ"      },
		{ 0x0804, L"zh-CN"      },
		{ 0x0807, L"de-CH"      },
		{ 0x0809, L"en-GB"      },
		{ 0x080A, L"es-MX"      },
		{ 0x080C, L"fr-BE"      },
		{ 0x0810, L"it-CH"      },
		{ 0x0813, L"nl-BE"      },
		{ 0x0814, L"nn-NO"      },
		{ 0x0816, L"pt-PT"      },
		{ 0x081A, L"sr-SP-Latn" },
		{ 0x081D, L"sv-FI"      },
		{ 0x082C, L"az-AZ-Cyrl" },
		{ 0x083B, L"se-SE"      },
		{ 0x083E, L"ms-BN"      },
		{ 0x0843, L"uz-UZ-Cyrl" },
		{ 0x086B, L"quz-EC"     },
		{ 0x0C01, L"ar-EG"      },
		{ 0x0C04, L"zh-HK"      },
		{ 0x0C07, L"de-AT"      },
		{ 0x0C09, L"en-AU"      },
		{ 0x0C0A, L"es-ES"      },
		{ 0x0C0C, L"fr-CA"      },
		{ 0x0C1A, L"sr-SP-Cyrl" },
		{ 0x0C3B, L"se-FI"      },
		{ 0x0C6B, L"quz-PE"     },
		{ 0x1001, L"ar-LY"      },
		{ 0x1004, L"zh-SG"      },
		{ 0x1007, L"de-LU"      },
		{ 0x1009, L"en-CA"      },
		{ 0x100A, L"es-GT"      },
		{ 0x100C, L"fr-CH"      },
		{ 0x101A, L"hr-BA"      },
		{ 0x103B, L"smj-NO"     },
		{ 0x1401, L"ar-DZ"      },
		{ 0x1404, L"zh-MO"      },
		{ 0x1407, L"de-LI"      },
		{ 0x1409, L"en-NZ"      },
		{ 0x140A, L"es-CR"      },
		{ 0x140C, L"fr-LU"      },
		{ 0x141A, L"bs-BA-Latn" },
		{ 0x143B, L"smj-SE"     },
		{ 0x1801, L"ar-MA"      },
		{ 0x1809, L"en-IE"      },
		{ 0x180A, L"es-PA"      },
		{ 0x180C, L"fr-MC"      },
		{ 0x181A, L"sr-BA-Latn" },
		{ 0x183B, L"sma-NO"     },
		{ 0x1C01, L"ar-TN"      },
		{ 0x1C09, L"en-ZA"      },
		{ 0x1C0A, L"es-DO"      },
		{ 0x1C1A, L"sr-BA-Cyrl" },
		{ 0x1C3B, L"sma-SE"     },
		{ 0x2001, L"ar-OM"      },
		{ 0x2009, L"en-JM"      },
		{ 0x200A, L"es-VE"      },
		{ 0x203B, L"sms-FI"     },
		{ 0x2401, L"ar-YE"      },
		{ 0x2409, L"en-CB"      },
		{ 0x240A, L"es-CO"      },
		{ 0x243B, L"smn-FI"     },
		{ 0x2801, L"ar-SY"      },
		{ 0x2809, L"en-BZ"      },
		{ 0x280A, L"es-PE"      },
		{ 0x2C01, L"ar-JO"      },
		{ 0x2C09, L"en-TT"      },
		{ 0x2C0A, L"es-AR"      },
		{ 0x3001, L"ar-LB"      },
		{ 0x3009, L"en-ZW"      },
		{ 0x300A, L"es-EC"      },
		{ 0x3401, L"ar-KW"      },
		{ 0x3409, L"en-PH"      },
		{ 0x340A, L"es-CL"      },
		{ 0x3801, L"ar-AE"      },
		{ 0x380A, L"es-UY"      },
		{ 0x3C01, L"ar-BH"      },
		{ 0x3C0A, L"es-PY"      },
		{ 0x4001, L"ar-QA"      },
		{ 0x400A, L"es-BO"      },
		{ 0x440A, L"es-SV"      },
		{ 0x480A, L"es-HN"      },
		{ 0x4C0A, L"es-NI"      },
		{ 0x500A, L"es-PR"      },
		{ 0x7C04, L"zh-CHT"     },
		{ 0x7C1A, L"sr"         }
	};

	int bottom = 0;
	int top = _countof(LcidToLocaleNameTable) - 1;

	while (bottom <= top)
	{
		int middle = (bottom + top) / 2;
		int testIndex = Locale - LcidToLocaleNameTable[middle].lcid;

		if (testIndex == 0)
		{
			auto buffer = LcidToLocaleNameTable[middle].localeName;
			auto count = wcslen(buffer);

			if (cchName > 0)
			{
				if ((int)count >= cchName)
				{
					SetLastError(ERROR_INSUFFICIENT_BUFFER);
					return 0;
				}

				memcpy(lpName, buffer, count * sizeof(*buffer));

				lpName[count] = L'\0';
			}

			return (int)count + 1;
		}

		if (testIndex < 0)
			top = middle - 1;
		else
			bottom = middle + 1;
	}

	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
}

_LCRT_DEFINE_IAT_SYMBOL(LCIDToLocaleName, _16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

int
WINAPI
GetLocaleInfoEx(
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ LCTYPE LCType,
	_Out_writes_to_opt_(cchData, return) LPWSTR lpLCData,
	_In_ int cchData
	)
{
	if (auto pGetLocaleInfoEx = try_get_GetLocaleInfoEx())
	{
		return pGetLocaleInfoEx(lpLocaleName, LCType, lpLCData, cchData);
	}

	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	return GetLocaleInfoW(Locale, LCType, lpLCData, cchData);
}

_LCRT_DEFINE_IAT_SYMBOL(GetLocaleInfoEx, _16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

int
WINAPI
GetDateFormatEx(
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ DWORD dwFlags,
	_In_opt_ CONST SYSTEMTIME* lpDate,
	_In_opt_ LPCWSTR lpFormat,
	_Out_writes_opt_(cchDate) LPWSTR lpDateStr,
	_In_ int cchDate,
	_In_opt_ LPCWSTR lpCalendar
	)
{
	if (auto pGetDateFormatEx = try_get_GetDateFormatEx())
	{
		return pGetDateFormatEx(lpLocaleName, dwFlags, lpDate, lpFormat, lpDateStr, cchDate, lpCalendar);
	}

	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	return GetDateFormatW(Locale, dwFlags, lpDate, lpFormat, lpDateStr, cchDate);
}

_LCRT_DEFINE_IAT_SYMBOL(GetDateFormatEx, _28);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

int
WINAPI
GetTimeFormatEx(
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ DWORD dwFlags,
	_In_opt_ CONST SYSTEMTIME* lpTime,
	_In_opt_ LPCWSTR lpFormat,
	_Out_writes_opt_(cchTime) LPWSTR lpTimeStr,
	_In_ int cchTime
	)
{
	if (auto pGetTimeFormatEx = try_get_GetTimeFormatEx())
	{
		return pGetTimeFormatEx(lpLocaleName, dwFlags, lpTime, lpFormat, lpTimeStr, cchTime);
	}

	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	return GetTimeFormatW(Locale, dwFlags, lpTime, lpFormat, lpTimeStr, cchTime);
}

_LCRT_DEFINE_IAT_SYMBOL(GetTimeFormatEx, _24);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

int
WINAPI
GetNumberFormatEx(
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ DWORD dwFlags,
	_In_ LPCWSTR lpValue,
	_In_opt_ CONST NUMBERFMTW *lpFormat,
	_Out_writes_opt_(cchNumber) LPWSTR lpNumberStr,
	_In_ int cchNumber
	)
{
	if (auto pGetNumberFormatEx = try_get_GetNumberFormatEx())
	{
		return pGetNumberFormatEx(lpLocaleName, dwFlags, lpValue, lpFormat, lpNumberStr, cchNumber);
	}


	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	return GetNumberFormatW(Locale, dwFlags, lpValue, lpFormat, lpNumberStr, cchNumber);
}

_LCRT_DEFINE_IAT_SYMBOL(GetNumberFormatEx, _24);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

int
WINAPI
GetCurrencyFormatEx(
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ DWORD dwFlags,
	_In_ LPCWSTR lpValue,
	_In_opt_ CONST CURRENCYFMTW *lpFormat,
	_Out_writes_opt_(cchCurrency) LPWSTR lpCurrencyStr,
	_In_ int cchCurrency
	)
{
	if (auto pGetCurrencyFormatEx = try_get_GetCurrencyFormatEx())
	{
		return pGetCurrencyFormatEx(lpLocaleName, dwFlags, lpValue, lpFormat, lpCurrencyStr, cchCurrency);
	}


	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	return GetCurrencyFormatW(Locale, dwFlags, lpValue, lpFormat, lpCurrencyStr, cchCurrency);
}

_LCRT_DEFINE_IAT_SYMBOL(GetCurrencyFormatEx, _24);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

int
WINAPI
GetUserDefaultLocaleName(
	_Out_writes_(cchLocaleName) LPWSTR lpLocaleName,
	_In_ int cchLocaleName
	)
{
	if (auto pGetUserDefaultLocaleName = try_get_GetUserDefaultLocaleName())
	{
		return pGetUserDefaultLocaleName(lpLocaleName, cchLocaleName);
	}


	return LCIDToLocaleName(LOCALE_USER_DEFAULT, lpLocaleName, cchLocaleName, 0);
}

_LCRT_DEFINE_IAT_SYMBOL(GetUserDefaultLocaleName, _8);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

int
WINAPI
GetSystemDefaultLocaleName(
	_Out_writes_(cchLocaleName) LPWSTR lpLocaleName,
	_In_ int cchLocaleName
	)
{
	if (auto pGetSystemDefaultLocaleName = try_get_GetSystemDefaultLocaleName())
	{
		return pGetSystemDefaultLocaleName(lpLocaleName, cchLocaleName);
	}


	return LCIDToLocaleName(LOCALE_SYSTEM_DEFAULT, lpLocaleName, cchLocaleName, 0);
}

_LCRT_DEFINE_IAT_SYMBOL(GetSystemDefaultLocaleName, _8);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008
#include <winnls.h>

struct EnumCalendarInfoExExDataInfo
{
	CALINFO_ENUMPROCEXEX pCalInfoEnumProcExEx;
	LPARAM lParam;
};

static thread_local EnumCalendarInfoExExDataInfo __EnumCalendarInfoExExDataInfo;

BOOL
WINAPI
EnumCalendarInfoExEx(
	_In_ CALINFO_ENUMPROCEXEX pCalInfoEnumProcExEx,
	_In_opt_ LPCWSTR lpLocaleName,
	_In_ CALID Calendar,
	_In_opt_ LPCWSTR lpReserved,
	_In_ CALTYPE CalType,
	_In_ LPARAM lParam
	)
{
	if (auto pEnumCalendarInfoExEx = try_get_EnumCalendarInfoExEx())
	{
		return pEnumCalendarInfoExEx(pCalInfoEnumProcExEx, lpLocaleName, Calendar, lpReserved, CalType, lParam);
	}

	if (pCalInfoEnumProcExEx == nullptr)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	//保存上下文
	__EnumCalendarInfoExExDataInfo.pCalInfoEnumProcExEx = pCalInfoEnumProcExEx;
	__EnumCalendarInfoExExDataInfo.lParam = lParam;

	return EnumCalendarInfoExW(
			[](LPWSTR lpCalendarInfoString, CALID Calendar)->BOOL
			{
				return __EnumCalendarInfoExExDataInfo.pCalInfoEnumProcExEx(lpCalendarInfoString, Calendar, nullptr, __EnumCalendarInfoExExDataInfo.lParam);
			},
			Locale,
			Calendar,
			CalType);
}

_LCRT_DEFINE_IAT_SYMBOL(EnumCalendarInfoExEx, _24);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008
#include <winnls.h>

struct EnumDateFormatsExExDataInfo
{
	DATEFMT_ENUMPROCEXEX pCalInfoEnumProcExEx;
	LPARAM lParam;
};

static thread_local EnumDateFormatsExExDataInfo __EnumDateFormatsExExDataInfo;

BOOL
WINAPI
EnumDateFormatsExEx(
    _In_ DATEFMT_ENUMPROCEXEX lpDateFmtEnumProcExEx,
    _In_opt_ LPCWSTR lpLocaleName,
    _In_ DWORD dwFlags,
    _In_ LPARAM lParam
	)
{
	if (auto pEnumDateFormatsExEx = try_get_EnumDateFormatsExEx())
	{
		return pEnumDateFormatsExEx(lpDateFmtEnumProcExEx, lpLocaleName, dwFlags, lParam);
	}

	if (lpDateFmtEnumProcExEx == nullptr)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	auto Locale = LocaleNameToLCID(lpLocaleName, 0);

	if (Locale == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	//保存上下文
	__EnumDateFormatsExExDataInfo.pCalInfoEnumProcExEx = lpDateFmtEnumProcExEx;
	__EnumDateFormatsExExDataInfo.lParam = lParam;

	return EnumDateFormatsExW(
			[](LPWSTR lpDateFormatString, CALID CalendarID)->BOOL
			{
				return __EnumDateFormatsExExDataInfo.pCalInfoEnumProcExEx(lpDateFormatString, CalendarID, __EnumCalendarInfoExExDataInfo.lParam);
			},
			Locale,
			dwFlags);
}

_LCRT_DEFINE_IAT_SYMBOL(EnumDateFormatsExEx, _16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008
BOOL
WINAPI
GetFileInformationByHandleEx(
	_In_  HANDLE hFile,
	_In_  FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
	_Out_writes_bytes_(dwBufferSize) LPVOID lpFileInformation,
	_In_  DWORD dwBufferSize
	)
{
	if (auto pGetFileInformationByHandleEx = try_get_GetFileInformationByHandleEx())
	{
		return pGetFileInformationByHandleEx(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
	}


	FILE_INFORMATION_CLASS NtFileInformationClass;
	DWORD cbMinBufferSize;
	bool bNtQueryDirectoryFile = false;
	BOOLEAN RestartScan = false;

	switch (FileInformationClass)
	{
	case FileBasicInfo:
		NtFileInformationClass = FileBasicInformation;
		cbMinBufferSize = sizeof(FILE_BASIC_INFO);
		break;
	case FileStandardInfo:
		NtFileInformationClass = FileStandardInformation;
		cbMinBufferSize = sizeof(FILE_STANDARD_INFO);
		break;
	case FileNameInfo:
		NtFileInformationClass = FileNameInformation;
		cbMinBufferSize = sizeof(FILE_NAME_INFO);
		break;
	case FileStreamInfo:
		NtFileInformationClass = FileStreamInformation;
		cbMinBufferSize = sizeof(FILE_STREAM_INFO);
		break;
	case FileCompressionInfo:
		NtFileInformationClass = FileCompressionInformation;
		cbMinBufferSize = sizeof(FILE_COMPRESSION_INFO);
		break;
	case FileAttributeTagInfo:
		NtFileInformationClass = FileAttributeTagInformation;
		cbMinBufferSize = sizeof(FILE_ATTRIBUTE_TAG_INFO);
		break;
	case FileIdBothDirectoryRestartInfo:
		RestartScan = true;
	case FileIdBothDirectoryInfo:
		NtFileInformationClass = FileIdBothDirectoryInformation;
		cbMinBufferSize = sizeof(FILE_ID_BOTH_DIR_INFO);
		bNtQueryDirectoryFile = true;
		break;
	case FileRemoteProtocolInfo:
		NtFileInformationClass = FileRemoteProtocolInformation;
		cbMinBufferSize = sizeof(FILE_REMOTE_PROTOCOL_INFO);
		break;
	default:
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
		break;
	}


	if (cbMinBufferSize > dwBufferSize)
	{
		SetLastError(ERROR_BAD_LENGTH);
		return FALSE;
	}

	IO_STATUS_BLOCK IoStatusBlock;
	NTSTATUS Status;

	if (bNtQueryDirectoryFile)
	{
		auto pNtQueryDirectoryFile = try_get_NtQueryDirectoryFile();
		if (!pNtQueryDirectoryFile)
		{
			SetLastError(ERROR_INVALID_FUNCTION);
			return FALSE;
		}

		Status = pNtQueryDirectoryFile(
			hFile,
			nullptr,
			nullptr,
			nullptr,
			&IoStatusBlock,
			lpFileInformation,
			dwBufferSize,
			NtFileInformationClass,
			false,
			nullptr,
			RestartScan
			);

		if (STATUS_PENDING == Status)
		{
			if (WaitForSingleObjectEx(hFile, 0, FALSE) == WAIT_FAILED)
			{
				//WaitForSingleObjectEx会设置LastError
				return FALSE;
			}

			Status = IoStatusBlock.Status;
		}
	}
	else
	{
		auto pNtQueryInformationFile = try_get_NtQueryInformationFile();

		if (!pNtQueryInformationFile)
		{
			SetLastError(ERROR_INVALID_FUNCTION);
			return FALSE;
		}

		Status = pNtQueryInformationFile(hFile, &IoStatusBlock, lpFileInformation, dwBufferSize, NtFileInformationClass);
	}

	if (Status >= STATUS_SUCCESS)
	{
		if (FileStreamInfo == FileInformationClass && IoStatusBlock.Information == 0)
		{
			SetLastError(ERROR_HANDLE_EOF);
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		auto pRtlNtStatusToDosError = try_get_RtlNtStatusToDosError();

		//如果没有RtlNtStatusToDosError就直接设置Status代码吧，反正至少比没有错误代码强
		SetLastError(pRtlNtStatusToDosError ? pRtlNtStatusToDosError(Status) : Status);

		return FALSE;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(GetFileInformationByHandleEx, _16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008
BOOL
WINAPI
SetFileInformationByHandle(
	_In_ HANDLE hFile,
	_In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
	_In_reads_bytes_(dwBufferSize) LPVOID lpFileInformation,
	_In_ DWORD dwBufferSize
	)
{
	if (auto pSetFileInformationByHandle = try_get_SetFileInformationByHandle())
	{
		return pSetFileInformationByHandle(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
	}


	auto pNtSetInformationFile = try_get_NtSetInformationFile();
	if (!pNtSetInformationFile)
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		return FALSE;
	}


	FILE_INFORMATION_CLASS NtFileInformationClass;
	DWORD cbMinBufferSize;
	bool bFreeFileInformation = false;

	switch (FileInformationClass)
	{
	case FileBasicInfo:
		NtFileInformationClass = FileBasicInformation;
		cbMinBufferSize = sizeof(FILE_BASIC_INFO);
		break;
	case FileRenameInfo:
		NtFileInformationClass = FileRenameInformation;
		cbMinBufferSize = sizeof(FILE_RENAME_INFO);

		if (cbMinBufferSize > dwBufferSize)
		{
			SetLastError(ERROR_BAD_LENGTH);
			return FALSE;
		}

		if (lpFileInformation == nullptr)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}
		else
		{
			auto pRenameInfo = (FILE_RENAME_INFO*)lpFileInformation;

			if (pRenameInfo->FileNameLength < sizeof(wchar_t) || pRenameInfo->FileName[0] != L':')
			{
				auto pRtlDosPathNameToNtPathName_U_WithStatus = try_get_RtlDosPathNameToNtPathName_U_WithStatus();
				auto pRtlFreeUnicodeString = try_get_RtlFreeUnicodeString();

				if (pRtlDosPathNameToNtPathName_U_WithStatus == nullptr || pRtlFreeUnicodeString ==nullptr)
				{
					SetLastError(ERROR_INVALID_FUNCTION);
					return FALSE;
				}

				UNICODE_STRING NtName;

				auto Status = pRtlDosPathNameToNtPathName_U_WithStatus(pRenameInfo->FileName, &NtName, nullptr, nullptr);

				if (Status < STATUS_SUCCESS)
				{
					auto pRtlNtStatusToDosError = try_get_RtlNtStatusToDosError();

					SetLastError(pRtlNtStatusToDosError ? pRtlNtStatusToDosError(Status) : Status);

					return FALSE;
				}

				auto dwNewBufferSize = sizeof(FILE_RENAME_INFO) + NtName.Length;

				auto NewRenameInfo = (FILE_RENAME_INFO*)GlobalAlloc(GMEM_FIXED, dwNewBufferSize);
				if (!NewRenameInfo)
				{
					auto lStatus = GetLastError();

					pRtlFreeUnicodeString(&NtName);

					SetLastError(lStatus);
					return FALSE;
				}

				bFreeFileInformation = true;

				NewRenameInfo->ReplaceIfExists = pRenameInfo->ReplaceIfExists;
				NewRenameInfo->RootDirectory = pRenameInfo->RootDirectory;
				NewRenameInfo->FileNameLength = NtName.Length;

				memcpy(NewRenameInfo->FileName, NtName.Buffer, NtName.Length);

				*(wchar_t*)((byte*)NewRenameInfo->FileName + NtName.Length) = L'\0';


				lpFileInformation = NewRenameInfo;
				dwBufferSize = dwNewBufferSize;

				pRtlFreeUnicodeString(&NtName);
			}
		}
		break;
	case FileDispositionInfo:
		NtFileInformationClass = FileDispositionInformation;
		cbMinBufferSize = sizeof(FILE_DISPOSITION_INFO);
		break;
	case FileAllocationInfo:
		NtFileInformationClass = FileAllocationInformation;
		cbMinBufferSize = sizeof(FILE_ALLOCATION_INFO);
		break;
	case FileEndOfFileInfo:
		NtFileInformationClass = FileEndOfFileInformation;
		cbMinBufferSize = sizeof(FILE_END_OF_FILE_INFO);
		break;
	case FileIoPriorityHintInfo:
		NtFileInformationClass = FileIoPriorityHintInformation;
		cbMinBufferSize = sizeof(FILE_IO_PRIORITY_HINT_INFO);

		//长度检查，微软原版似乎没有该安全检查
		if (cbMinBufferSize > dwBufferSize)
		{
			SetLastError(ERROR_BAD_LENGTH);
			return FALSE;
		}

		if (lpFileInformation == nullptr || ((FILE_IO_PRIORITY_HINT_INFO*)lpFileInformation)->PriorityHint >= MaximumIoPriorityHintType)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		break;
	default:
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
		break;
	}

	if (cbMinBufferSize > dwBufferSize)
	{
		if (bFreeFileInformation)
		{
			GlobalFree((HGLOBAL)lpFileInformation);
		}

		SetLastError(ERROR_BAD_LENGTH);
		return FALSE;
	}

	IO_STATUS_BLOCK IoStatusBlock;

	auto Status = pNtSetInformationFile(hFile, &IoStatusBlock, lpFileInformation, dwBufferSize, NtFileInformationClass);

	if (bFreeFileInformation)
	{
		GlobalFree((HGLOBAL)lpFileInformation);
	}

	if (Status >= STATUS_SUCCESS)
		return TRUE;

	auto pRtlNtStatusToDosError = try_get_RtlNtStatusToDosError();

	//如果没有RtlNtStatusToDosError就直接设置Status代码吧，反正至少比没有错误代码强
	SetLastError(pRtlNtStatusToDosError ? pRtlNtStatusToDosError(Status) : Status);

	return FALSE;
}

_LCRT_DEFINE_IAT_SYMBOL(SetFileInformationByHandle, _16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008
DWORD
WINAPI
GetFinalPathNameByHandleW(
	_In_ HANDLE hFile,
	_Out_writes_(cchFilePath) LPWSTR lpszFilePath,
	_In_ DWORD cchFilePath,
	_In_ DWORD dwFlags
	)
{
	if (auto pGetFinalPathNameByHandleW = try_get_GetFinalPathNameByHandleW())
	{
		return pGetFinalPathNameByHandleW(hFile, lpszFilePath, cchFilePath, dwFlags);
	}

	//参数检查
	if (INVALID_HANDLE_VALUE == hFile)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return 0;
	}


	switch (dwFlags & (VOLUME_NAME_DOS | VOLUME_NAME_GUID | VOLUME_NAME_NONE | VOLUME_NAME_NT))
	{
	case VOLUME_NAME_DOS:
		break;
	case VOLUME_NAME_GUID:
		break;
	case VOLUME_NAME_NT:
		break;
	case VOLUME_NAME_NONE:
		break;
	default:
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
		break;
	}


	auto pNtQueryObject = try_get_NtQueryObject();
	auto pNtQueryInformationFile = try_get_NtQueryInformationFile();

	if (nullptr == pNtQueryObject
		|| nullptr == pNtQueryInformationFile)
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		return 0;
	}

	LSTATUS lStatus = ERROR_SUCCESS;
	DWORD   cchReturn = 0;

	OBJECT_NAME_INFORMATION* pObjectName = nullptr;
	ULONG cbObjectName = 528;

	FILE_NAME_INFORMATION* pFileNameInfo = nullptr;
	ULONG cbFileNameInfo = 528;

	for (;;)
	{
		if (pObjectName)
		{
			GlobalFree(pObjectName);
			pObjectName = nullptr;
		}

		pObjectName = (OBJECT_NAME_INFORMATION*)GlobalAlloc(GMEM_FIXED, cbObjectName);

		if (!pObjectName)
		{
			//内存不足？
			lStatus = ERROR_NOT_ENOUGH_MEMORY;
			goto __Exit;
		}

		auto Status = pNtQueryObject(hFile, ObjectNameInformation, pObjectName, cbObjectName, &cbObjectName);

		if (STATUS_BUFFER_OVERFLOW == Status)
		{
			continue;
		}
		else if (Status < 0)
		{
			auto pRtlNtStatusToDosError = try_get_RtlNtStatusToDosError();

			lStatus = pRtlNtStatusToDosError ? pRtlNtStatusToDosError(Status) : Status;

			goto __Exit;
		}
		else
		{
			break;
		}
	}

	for (;;)
	{
		if (pFileNameInfo)
		{
			GlobalFree(pFileNameInfo);
			pFileNameInfo = nullptr;
		}

		pFileNameInfo = (FILE_NAME_INFORMATION*)GlobalAlloc(GMEM_FIXED, cbFileNameInfo);

		if (!pFileNameInfo)
		{
			//内存不足？
			lStatus = ERROR_NOT_ENOUGH_MEMORY;
			goto __Exit;
		}

		IO_STATUS_BLOCK IoStatusBlock;

		auto Status = pNtQueryInformationFile(hFile, &IoStatusBlock, pFileNameInfo, cbFileNameInfo, FileNameInformation);

		if (STATUS_BUFFER_OVERFLOW == Status)
		{
			cbFileNameInfo = pFileNameInfo->FileNameLength + sizeof(FILE_NAME_INFORMATION);
			continue;
		}
		else if (Status < 0)
		{
			auto pRtlNtStatusToDosError = try_get_RtlNtStatusToDosError();

			lStatus = pRtlNtStatusToDosError ? pRtlNtStatusToDosError(Status) : Status;

			goto __Exit;
		}
		else
		{
			break;
		}
	}

	if (pFileNameInfo->FileName[0] != '\\')
	{
		lStatus = ERROR_ACCESS_DENIED;
		goto __Exit;
	}



	if (pFileNameInfo->FileNameLength >= pObjectName->Name.Length)
	{
		lStatus = ERROR_BAD_PATHNAME;
		goto __Exit;
	}

	//低于Vista平台无法支持 FILE_NAME_OPENED，因此忽略

	if (VOLUME_NAME_NT & dwFlags)
	{
		//返回NT路径
		cchReturn = pObjectName->Name.Length / sizeof(lpszFilePath[0]);

		if (cchFilePath <= cchReturn)
		{
			//长度不足……

			cchReturn += 1;
		}
		else
		{
			memcpy(lpszFilePath, pObjectName->Name.Buffer, cchReturn * sizeof(lpszFilePath[0]));

			lpszFilePath[cchReturn] = L'\0';
		}
	}
	else if (VOLUME_NAME_NONE & dwFlags)
	{
		//仅返回文件名
		cchReturn = pFileNameInfo->FileNameLength / sizeof(lpszFilePath[0]);

		if (cchFilePath <= cchReturn)
		{
			//长度不足……

			cchReturn += 1;
		}
		else
		{
			memcpy(lpszFilePath, pFileNameInfo->FileName, cchReturn * sizeof(lpszFilePath[0]));
			lpszFilePath[cchReturn] = L'\0';
		}
	}
	else
	{
		const wchar_t szGobal[] = L"\\\\?\\GLOBALROOT";

		const DWORD cbVolumeName = pObjectName->Name.Length - pFileNameInfo->FileNameLength + sizeof(lpszFilePath[0]);

		auto szVolumeMountPoint = (wchar_t*)GlobalAlloc(GMEM_FIXED, cbVolumeName + sizeof(szGobal));
		if (!szVolumeMountPoint)
		{
			lStatus = ERROR_NOT_ENOUGH_MEMORY;
			goto __Exit;
		}

		DWORD cbVolumeMountPoint = sizeof(szGobal) - sizeof(szGobal[0]);

		memcpy(szVolumeMountPoint, szGobal, cbVolumeMountPoint);

		memcpy((byte*)szVolumeMountPoint + cbVolumeMountPoint, pObjectName->Name.Buffer, cbVolumeName);
		cbVolumeMountPoint += cbVolumeName;

		szVolumeMountPoint[cbVolumeMountPoint / sizeof(szVolumeMountPoint[0])] = L'\0';

		wchar_t szVolumeName[MAX_PATH];

		if (!GetVolumeNameForVolumeMountPointW(szVolumeMountPoint, szVolumeName, _countof(szVolumeName)))
		{
			lStatus = GetLastError();
			GlobalFree(szVolumeMountPoint);

			goto __Exit;
		}

		GlobalFree(szVolumeMountPoint);


		if (VOLUME_NAME_GUID & dwFlags)
		{
			//返回分区GUID名称

			auto cchVolumeName = wcslen(szVolumeName);

			if (cchVolumeName == 0)
			{
				//逗我？
				lStatus = ERROR_INVALID_FUNCTION;
				goto __Exit;
			}

			--cchVolumeName;

			cchReturn = cchVolumeName + pFileNameInfo->FileNameLength / sizeof(pFileNameInfo->FileName[0]);

			if (cchFilePath <= cchReturn)
			{
				cchReturn += 1;
			}
			else
			{
				//复制VolumeName
				memcpy(lpszFilePath, szVolumeName, cchVolumeName * sizeof(szVolumeName[0]));

				//复制文件名
				memcpy(lpszFilePath + cchVolumeName, pFileNameInfo->FileName, pFileNameInfo->FileNameLength);

				//NULL边界
				lpszFilePath[cchReturn] = L'\0';
			}
		}
		else
		{
			//返回Dos路径
			DWORD cchVolumePathName = 0;

			wchar_t VolumePathName[MAX_PATH + 4] = L"\\\\?\\";

			if (!GetVolumePathNamesForVolumeNameW(szVolumeName, VolumePathName + 4, MAX_PATH, &cchVolumePathName))
			{
				lStatus = GetLastError();
				goto __Exit;
			}

			cchVolumePathName = wcslen(VolumePathName);

			if (cchVolumePathName == 0)
			{
				//逗我？
				lStatus = ERROR_INVALID_FUNCTION;
				goto __Exit;
			}

			--cchVolumePathName;

			cchReturn = cchVolumePathName + pFileNameInfo->FileNameLength / sizeof(pFileNameInfo->FileName[0]);

			if (cchFilePath <= cchReturn)
			{
				cchReturn += 1;
			}
			else
			{
				//复制VolumePathName 
				memcpy(lpszFilePath, VolumePathName, cchVolumePathName * sizeof(VolumePathName[0]));

				//复制文件名
				memcpy(lpszFilePath + cchVolumePathName, pFileNameInfo->FileName, pFileNameInfo->FileNameLength);

				//NULL边界
				lpszFilePath[cchReturn] = L'\0';
			}
		}
	}

__Exit:
	if (pFileNameInfo)
		GlobalFree(pFileNameInfo);
	if (pObjectName)
		GlobalFree(pObjectName);

	if (lStatus != ERROR_SUCCESS)
	{
		SetLastError(lStatus);
		return 0;
	}
	else
	{
		return cchReturn;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(GetFinalPathNameByHandleW, _16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008
DWORD
WINAPI
GetFinalPathNameByHandleA(
	_In_ HANDLE hFile,
	_Out_writes_(cchFilePath) LPSTR lpszFilePath,
	_In_ DWORD cchFilePath,
	_In_ DWORD dwFlags
	)
{
	if (auto pGetFinalPathNameByHandleA = try_get_GetFinalPathNameByHandleA())
	{
		return pGetFinalPathNameByHandleA(hFile, lpszFilePath, cchFilePath, dwFlags);
	}


	for (DWORD cchszFilePathUnicode = 1040;;)
	{
		auto szFilePathUnicode = (wchar_t*)GlobalAlloc(GMEM_FIXED, cchszFilePathUnicode * sizeof(wchar_t));
		if (!szFilePathUnicode)
		{
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return 0;
		}

		auto cchReturn = GetFinalPathNameByHandleW(hFile, szFilePathUnicode, cchszFilePathUnicode, dwFlags);

		if (cchReturn == 0)
		{
		__Error:

			auto lStatus = GetLastError();
			GlobalFree(szFilePathUnicode);
			SetLastError(lStatus);

			return 0;
		}
		else if (cchReturn > cchszFilePathUnicode)
		{
			//缓冲区不足
			cchszFilePathUnicode = cchReturn;
			GlobalFree(szFilePathUnicode);
			continue;
		}
		else
		{
			//操作成功！

			auto cchReturnANSI = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, szFilePathUnicode, cchReturn, nullptr, 0, nullptr, nullptr);

			if (0 == cchReturnANSI)
			{
				goto __Error;
			}
			else if (cchReturnANSI >= cchFilePath)
			{
				//长度不足
				++cchReturnANSI;
			}
			else
			{
				WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, szFilePathUnicode, cchReturn, lpszFilePath, cchFilePath, nullptr, nullptr);
				lpszFilePath[cchReturnANSI] = '\0';
			}

			GlobalFree(szFilePathUnicode);
			return cchReturnANSI;
		}
	}
}

_LCRT_DEFINE_IAT_SYMBOL(GetFinalPathNameByHandleA, _16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WS03) || (YY_Thunks_Support_Version < NTDDI_WINXPSP3)
//Windows Vista, Windows XP Professional x64 Edition, Windows XP with SP3 [desktop apps | UWP apps]
//Windows Server 2003 [desktop apps | UWP apps]

BOOL
WINAPI
GetLogicalProcessorInformation(
	_Out_writes_bytes_to_opt_(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
	_Inout_ PDWORD ReturnedLength
	)
{
	if (auto pGetLogicalProcessorInformation = try_get_GetLogicalProcessorInformation())
	{
		return pGetLogicalProcessorInformation(Buffer, ReturnedLength);
	}
	
	SetLastError(ERROR_INVALID_FUNCTION);
	return FALSE;
}

_LCRT_DEFINE_IAT_SYMBOL(GetLogicalProcessorInformation, _8);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WS03) || (YY_Thunks_Support_Version < NTDDI_WINXPSP3)
//Windows 7, Windows Server 2008 R2

BOOL
WINAPI
GetLogicalProcessorInformationEx(
	_In_ LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
	_Out_writes_bytes_to_opt_(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Buffer,
	_Inout_ PDWORD ReturnedLength
	)
{
	if (auto pGetLogicalProcessorInformationEx = try_get_GetLogicalProcessorInformationEx())
	{
		return pGetLogicalProcessorInformationEx(RelationshipType, Buffer, ReturnedLength);
	}


	if (!ReturnedLength)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	
	const auto cbBuffer = *ReturnedLength;

	if (cbBuffer != 0 && Buffer == nullptr)
	{
		SetLastError(ERROR_NOACCESS);
		return FALSE;
	}
	
	LSTATUS lStatus = ERROR_SUCCESS;

	SYSTEM_LOGICAL_PROCESSOR_INFORMATION* pProcessorInfo = nullptr;
	DWORD cbLogicalProcessorInformation = 0;

	for (; GetLogicalProcessorInformation(pProcessorInfo, &cbLogicalProcessorInformation) == FALSE;)
	{
		lStatus = GetLastError();

		if (pProcessorInfo)
			GlobalFree(pProcessorInfo);

		if (ERROR_INSUFFICIENT_BUFFER == lStatus)
		{
			pProcessorInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)GlobalAlloc(GMEM_FIXED, cbLogicalProcessorInformation);

			if(pProcessorInfo)
				continue;

			lStatus = ERROR_NOT_ENOUGH_MEMORY;
		}

		goto __End;
	}


	{
		const auto pProcessorInfoLastItem = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)((byte*)pProcessorInfo + cbLogicalProcessorInformation - sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

		DWORD cbBufferUsed = 0;



		for (auto pProcessorInfoStart = pProcessorInfo; pProcessorInfoStart <= pProcessorInfoLastItem; ++pProcessorInfoStart)
		{
			if (RelationshipType == LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll
				|| pProcessorInfoStart->Relationship == RelationshipType)
			{
				DWORD cbInfoNeed;

				switch (pProcessorInfoStart->Relationship)
				{
				case RelationProcessorCore:
				case RelationProcessorPackage:
					cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Processor);
					break;
				case RelationNumaNode:
					cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, NumaNode);
					break;
				case RelationCache:
					cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Cache);
					break;
				default:
					cbInfoNeed = 0;
					break;
				}

				//不支持传输此类型
				if (0 == cbInfoNeed)
					continue;

				cbBufferUsed += cbInfoNeed;

				if (cbBuffer >= cbBufferUsed)
				{
					auto pInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)((byte*)Buffer + cbBufferUsed);

					memset(pInfo, 0, cbInfoNeed);

					pInfo->Size = cbInfoNeed;
					pInfo->Relationship = pProcessorInfoStart->Relationship;

					switch (pProcessorInfoStart->Relationship)
					{
					case RelationProcessorCore:
					case RelationProcessorPackage:
						pInfo->Processor.Flags = pProcessorInfoStart->ProcessorCore.Flags;
						pInfo->Processor.GroupCount = 1;
						pInfo->Processor.GroupMask->Mask = pProcessorInfoStart->ProcessorMask;
						break;
					case RelationNumaNode:
						pInfo->NumaNode.NodeNumber = pProcessorInfoStart->NumaNode.NodeNumber;
						pInfo->NumaNode.GroupMask.Mask = pProcessorInfoStart->ProcessorMask;
						break;
					case RelationCache:
						pInfo->Cache.Level = pProcessorInfoStart->Cache.Level;
						pInfo->Cache.Associativity = pProcessorInfoStart->Cache.Associativity;
						pInfo->Cache.LineSize = pProcessorInfoStart->Cache.LineSize;
						pInfo->Cache.CacheSize = pProcessorInfoStart->Cache.Size;
						pInfo->Cache.Type = pProcessorInfoStart->Cache.Type;
						pInfo->Cache.GroupMask.Mask= pProcessorInfoStart->ProcessorMask;
						break;
					}
				}
			}
		}


		//传输 RelationGroup 信息，这里只能假设只有一组CPU
		if (RelationshipType == LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll
			|| RelationGroup == RelationshipType)
		{
			const auto cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Group);

			cbBufferUsed += cbInfoNeed;

			if (cbBuffer >= cbBufferUsed)
			{
				auto pInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)((byte*)Buffer + cbBufferUsed);

				memset(pInfo, 0, cbInfoNeed);

				pInfo->Size = cbInfoNeed;
				pInfo->Relationship = RelationGroup;

				pInfo->Group.ActiveGroupCount = 1;
				pInfo->Group.MaximumGroupCount = 1;

				SYSTEM_INFO SystemInfo;

				GetSystemInfo(&SystemInfo);


				constexpr const size_t BitCountTable[256] =
				{
					0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
					1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
					1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
					1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
					3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
					4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
				};

				pInfo->Group.GroupInfo->ActiveProcessorMask = SystemInfo.dwActiveProcessorMask;

				auto& ActiveProcessorMask = SystemInfo.dwActiveProcessorMask;

#if defined(_M_IX86) || defined(_M_ARM)
				size_t ActiveProcessorCount = BitCountTable[((byte*)&ActiveProcessorMask)[0]] + BitCountTable[((byte*)&ActiveProcessorMask)[1]] + BitCountTable[((byte*)&ActiveProcessorMask)[2]] + BitCountTable[((byte*)&ActiveProcessorMask)[3]];
#elif defined(_M_AMD64) || defined(_M_IA64) || defined(_M_ARM64)
				size_t ActiveProcessorCount = BitCountTable[((byte*)&ActiveProcessorMask)[0]] + BitCountTable[((byte*)&ActiveProcessorMask)[1]] + BitCountTable[((byte*)&ActiveProcessorMask)[2]] + BitCountTable[((byte*)&ActiveProcessorMask)[3]]
					+ BitCountTable[((byte*)&ActiveProcessorMask)[4]] + BitCountTable[((byte*)&ActiveProcessorMask)[5]] + BitCountTable[((byte*)&ActiveProcessorMask)[6]] + BitCountTable[((byte*)&ActiveProcessorMask)[7]];
#else
				size_t ActiveProcessorCount = 0;
				for (int i = 0; i != sizeof(ActiveProcessorMask); ++i)
				{
					ActiveProcessorCount += BitCountTable[((byte*)&ActiveProcessorMask)[i]];
				}
#endif
				pInfo->Group.GroupInfo->ActiveProcessorCount = ActiveProcessorCount;
				pInfo->Group.GroupInfo->MaximumProcessorCount = SystemInfo.dwNumberOfProcessors;
			}
		}

		*ReturnedLength = cbBufferUsed;

		if (cbBufferUsed > cbBuffer)
		{
			//缓冲区不足
			lStatus = ERROR_INSUFFICIENT_BUFFER;
		}
	}

__End:
	if(pProcessorInfo)
		GlobalFree(pProcessorInfo);

	if (lStatus != ERROR_SUCCESS)
	{
		SetLastError(lStatus);

		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(GetLogicalProcessorInformationEx, _12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows 8.1, Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]


INT
WINAPI
inet_pton(
    _In_                                      INT             Family,
    _In_                                      PCSTR           pszAddrString,
    _When_(Family == AF_INET, _Out_writes_bytes_(sizeof(IN_ADDR)))
    _When_(Family == AF_INET6, _Out_writes_bytes_(sizeof(IN6_ADDR)))
                                              PVOID           pAddrBuf
    )
{
	if (auto pinet_pton = try_get_inet_pton())
	{
		return pinet_pton(Family, pszAddrString, pAddrBuf);
	}

	typedef char Char;

	if (nullptr == pszAddrString || nullptr == pAddrBuf)
	{
		WSASetLastError(WSAEFAULT);
		return -1;
	}

	if (AF_INET == Family)
	{
		//IPv4
		UCHAR IPAddress[4] = {};

		int i = 0;
		for (; i != _countof(IPAddress); )
		{
			auto& IPNum = IPAddress[i++];

			auto Start = pszAddrString;
			for (; *Start && *Start != Char('.'); ++Start)
			{
				auto& Ch = *Start;

				//必须是0~9数字
				if (Ch >= Char('0') && Ch <= Char('9'))
				{
					auto NewNum = IPNum * 10ul + (Ch & 0x0Ful);
					if (NewNum > 0xFF)
					{
						//不能大于255
						return 0;
					}

					IPNum = (UCHAR)NewNum;
				}
				else
				{
					return 0;
				}
			}

			//
			if (Start == pszAddrString)
				return 0;

			pszAddrString = Start;

			if (*pszAddrString == Char('\0'))
				break;
			else
				++pszAddrString;
		}

		//未正常截断
		if (i != _countof(IPAddress) || *pszAddrString != Char('\0'))
			return 0;


		((IN_ADDR *)pAddrBuf)->S_un.S_addr = *(ULONG*)IPAddress;

		return 1;
	}
	else if (AF_INET6 == Family)
	{
		//IPv6

		IN6_ADDR IPAddress;

		int i = 0;
		int InsertIndex = -1;

		for (; i != _countof(IPAddress.u.Word) && *pszAddrString; )
		{
			if (pszAddrString[0] == Char(':') && pszAddrString[1] == Char(':'))
			{
				//缩进只能有一次
				if (InsertIndex != -1)
					return 0;


				InsertIndex = i;

				pszAddrString += 2;
				continue;
			}

			auto& IPNum = IPAddress.u.Word[i++];
			IPNum = 0;

			auto Start = pszAddrString;
			for (; *Start && *Start != Char(':'); ++Start)
			{
				auto& Ch = *Start;


				unsigned int NewNum;

				//0~9数字
				if (Ch >= Char('0') && Ch <= Char('9'))
				{
					NewNum = Ch & 0x0Ful;
				}
				else if (Ch >= Char('a') && Ch <= Char('f'))
				{
					NewNum = Ch - Char('a') + 10;
				}
				else if (Ch >= Char('A') && Ch <= Char('F'))
				{
					NewNum = Ch - Char('A') + 10;
				}
				else
				{
					//无法识别
					return 0;
				}

				NewNum = ((unsigned int)IPNum << 4) | NewNum;

				if (NewNum > 0xFFFF)
				{
					//不能大于255
					return 0;
				}

				IPNum = NewNum;
			}


			//截断错误
			if (Start == pszAddrString)
			{
				return 0;
			}


			IPNum = _byteswap_ushort(IPNum);

			pszAddrString = Start;

			if (*pszAddrString == Char('\0'))
			{
				break;
			}
			else if (pszAddrString[1] == Char(':'))
			{
			}
			else
			{
				++pszAddrString;
			}
		}


		//未正常截断
		if (*pszAddrString != Char('\0'))
		{
			return 0;
		}
		else if (i != _countof(IPAddress.u.Word) && InsertIndex == -1)
		{
			return 0;
		}
		else
		{
			if (InsertIndex == -1)
			{
				*(IN6_ADDR*)pAddrBuf = IPAddress;
			}
			else
			{
				//先复制头

				auto j = 0;
				for (; j != InsertIndex; ++j)
				{
					((IN6_ADDR*)pAddrBuf)->u.Word[j] = IPAddress.u.Word[j];
				}

				//补充中间 0
				for (const auto Count = _countof(IPAddress.u.Word) - i + j; j != Count; ++j)
				{
					((IN6_ADDR*)pAddrBuf)->u.Word[j] = 0;
				}

				//复制小尾巴
				for (; j != _countof(IPAddress.u.Word); ++j, ++InsertIndex)
				{
					((IN6_ADDR*)pAddrBuf)->u.Word[j] = IPAddress.u.Word[InsertIndex];
				}
			}
			return 1;
		}
	}
	else
	{
		WSASetLastError(WSAEAFNOSUPPORT);
		return -1;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(inet_pton, _12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows 8.1, Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

INT
WINAPI
InetPtonW(
	_In_                                      INT             Family,
	_In_                                      PCWSTR          pszAddrString,
	_When_(Family == AF_INET, _Out_writes_bytes_(sizeof(IN_ADDR)))
	_When_(Family == AF_INET6, _Out_writes_bytes_(sizeof(IN6_ADDR)))
                                              PVOID           pAddrBuf
	)
{
	if (auto pInetPtonW = try_get_InetPtonW())
	{
		return pInetPtonW(Family, pszAddrString, pAddrBuf);
	}

	typedef wchar_t Char;

	if (nullptr == pszAddrString || nullptr == pAddrBuf)
	{
		WSASetLastError(WSAEFAULT);
		return -1;
	}

	if (AF_INET == Family)
	{
		//IPv4
		UCHAR IPAddress[4] = {};

		int i = 0;
		for (; i != _countof(IPAddress); )
		{
			auto& IPNum = IPAddress[i++];

			auto Start = pszAddrString;
			for (; *Start && *Start != Char('.'); ++Start)
			{
				auto& Ch = *Start;

				//必须是0~9数字
				if (Ch >= Char('0') && Ch <= Char('9'))
				{
					auto NewNum = IPNum * 10ul + (Ch & 0x0Ful);
					if (NewNum > 0xFF)
					{
						//不能大于255
						return 0;
					}

					IPNum = (UCHAR)NewNum;
				}
				else
				{
					return 0;
				}
			}

			//
			if (Start == pszAddrString)
				return 0;

			pszAddrString = Start;

			if (*pszAddrString == Char('\0'))
				break;
			else
				++pszAddrString;
		}

		//未正常截断
		if (i != _countof(IPAddress) || *pszAddrString != Char('\0'))
			return 0;


		((IN_ADDR *)pAddrBuf)->S_un.S_addr = *(ULONG*)IPAddress;

		return 1;
	}
	else if (AF_INET6 == Family)
	{
		//IPv6

		IN6_ADDR IPAddress;

		int i = 0;
		int InsertIndex = -1;

		for (; i != _countof(IPAddress.u.Word) && *pszAddrString; )
		{
			if (pszAddrString[0] == Char(':') && pszAddrString[1] == Char(':'))
			{
				//缩进只能有一次
				if (InsertIndex != -1)
					return 0;


				InsertIndex = i;

				pszAddrString += 2;
				continue;
			}

			auto& IPNum = IPAddress.u.Word[i++];
			IPNum = 0;

			auto Start = pszAddrString;
			for (; *Start && *Start != Char(':'); ++Start)
			{
				auto& Ch = *Start;


				unsigned int NewNum;

				//0~9数字
				if (Ch >= Char('0') && Ch <= Char('9'))
				{
					NewNum = Ch & 0x0Ful;
				}
				else if (Ch >= Char('a') && Ch <= Char('f'))
				{
					NewNum = Ch - Char('a') + 10;
				}
				else if (Ch >= Char('A') && Ch <= Char('F'))
				{
					NewNum = Ch - Char('A') + 10;
				}
				else
				{
					//无法识别
					return 0;
				}

				NewNum = ((unsigned int)IPNum << 4) | NewNum;

				if (NewNum > 0xFFFF)
				{
					//不能大于255
					return 0;
				}

				IPNum = NewNum;
			}


			//截断错误
			if (Start == pszAddrString)
			{
				return 0;
			}


			IPNum = _byteswap_ushort(IPNum);

			pszAddrString = Start;

			if (*pszAddrString == Char('\0'))
			{
				break;
			}
			else if (pszAddrString[1] == Char(':'))
			{
			}
			else
			{
				++pszAddrString;
			}
		}


		//未正常截断
		if (*pszAddrString != Char('\0'))
		{
			return 0;
		}
		else if (i != _countof(IPAddress.u.Word) && InsertIndex == -1)
		{
			return 0;
		}
		else
		{
			if (InsertIndex == -1)
			{
				*(IN6_ADDR*)pAddrBuf = IPAddress;
			}
			else
			{
				//先复制头

				auto j = 0;
				for (; j != InsertIndex; ++j)
				{
					((IN6_ADDR*)pAddrBuf)->u.Word[j] = IPAddress.u.Word[j];
				}

				//补充中间 0
				for (const auto Count = _countof(IPAddress.u.Word) - i + j; j != Count; ++j)
				{
					((IN6_ADDR*)pAddrBuf)->u.Word[j] = 0;
				}

				//复制小尾巴
				for (; j != _countof(IPAddress.u.Word); ++j, ++InsertIndex)
				{
					((IN6_ADDR*)pAddrBuf)->u.Word[j] = IPAddress.u.Word[InsertIndex];
				}
			}
			return 1;
		}
	}
	else
	{
		WSASetLastError(WSAEAFNOSUPPORT);
		return -1;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(InetPtonW, _12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows 8.1, Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

PCSTR
WINAPI
inet_ntop(
	_In_                                INT             Family,
	_In_                                const VOID *    pAddr,
	_Out_writes_(StringBufSize)         PSTR            pStringBuf,
	_In_                                size_t          StringBufSize
	)
{
	if (auto pinet_ntop = try_get_inet_ntop())
	{
		return pinet_ntop(Family, pAddr, pStringBuf, StringBufSize);
	}

	NTSTATUS WSAError = ERROR_SUCCESS;

	if (pAddr == nullptr || pStringBuf == nullptr)
	{
		WSAError = ERROR_INVALID_PARAMETER;
	}
	else
	{
		char BufferTemp[64];
		auto szString = pStringBuf;
		size_t cchString = 0;

		constexpr const char Hex[] = "0123456789abcdef";


		if (AF_INET == Family)
		{
			//IPv4
			if (StringBufSize < 16)
			{
				szString = BufferTemp;
			}

			auto& IPv4 = *((unsigned char(*)[4])pAddr);

			for (int i = 0; i != _countof(IPv4); ++i)
			{
				auto Num = IPv4[i];
				if (Num < 10)
				{
					//1 位
					szString[cchString++] = Hex[Num];
				}
				else if (Num < 100)
				{
					//2 位
					szString[cchString++] = Hex[Num / 10];
					szString[cchString++] = Hex[Num % 10];
				}
				else
				{
					//3 位
					szString[cchString++] = Hex[Num / 100];
					szString[cchString++] = Hex[(Num / 10) % 10];
					szString[cchString++] = Hex[Num % 10];
				}

				szString[cchString++] = '.';
			}

			--cchString;
		}
		else if (AF_INET6 == Family)
		{
			//IPv6
			if (StringBufSize < 46)
			{
				szString = BufferTemp;
			}

			auto& IPv6 = ((const IN6_ADDR*)pAddr)->u.Word;

			int ZeroIndex = -1;
			int ZeroCount = 1;

			//统计 连续 0个数最多的情况
			for (int i = 0; i != _countof(IPv6);)
			{
				auto j = i;

				for (; j != _countof(IPv6) && IPv6[j] == 0; ++j);

				auto Count = j - i;

				if (Count)
				{
					if (Count > ZeroCount)
					{
						ZeroCount = Count;
						ZeroIndex = i;
					}

					i = j;
				}
				else
				{
					++i;
				}
			}


			for (int i = 0; i != _countof(IPv6);)
			{
				if (ZeroIndex == i)
				{
					if (i == 0)
					{
						szString[cchString++] = ':';
					}

					szString[cchString++] = ':';
					i += ZeroCount;
				}
				else
				{
					auto Num = _byteswap_ushort(IPv6[i++]);

					if (Num <= 0xF)
					{
						//1 位
						szString[cchString++] = Hex[Num];
					}
					else if (Num <= 0xFF)
					{
						//2 位
						szString[cchString++] = Hex[Num >> 4];
						szString[cchString++] = Hex[Num & 0x0F];
					}
					else if (Num <= 0xFFF)
					{
						//3 位
						szString[cchString++] = Hex[(Num >> 8)];

						szString[cchString++] = Hex[(Num >> 4) & 0x0F];
						szString[cchString++] = Hex[(Num >> 0) & 0x0F];
					}
					else
					{
						//4位
						szString[cchString++] = Hex[(Num >> 12)];
						szString[cchString++] = Hex[(Num >> 8) & 0x0F];

						szString[cchString++] = Hex[(Num >> 4) & 0x0F];
						szString[cchString++] = Hex[(Num >> 0) & 0x0F];
					}

					if (i != _countof(IPv6))
						szString[cchString++] = ':';
				}
			}
		}
		else
		{
			WSAError = WSAEAFNOSUPPORT;
		}

		if (WSAError == ERROR_SUCCESS)
		{
			if (cchString >= StringBufSize)
			{
				WSAError = ERROR_INVALID_PARAMETER;
			}
			else
			{
				if (szString != BufferTemp)
				{
					//缓冲区不足

					memcpy(pStringBuf, szString, sizeof(szString[0]) * cchString);
				}

				pStringBuf[cchString] = '\0';

				return pStringBuf;
			}
		}
	}


	WSASetLastError(WSAError);
	return nullptr;
}

_LCRT_DEFINE_IAT_SYMBOL(inet_ntop, _16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows 8.1, Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

PCWSTR
WINAPI
InetNtopW(
	_In_                                INT             Family,
	_In_                                const VOID *    pAddr,
	_Out_writes_(StringBufSize)         PWSTR           pStringBuf,
	_In_                                size_t          StringBufSize
	)
{
	if (auto pInetNtopW = try_get_InetNtopW())
	{
		return pInetNtopW(Family, pAddr, pStringBuf, StringBufSize);
	}

	NTSTATUS WSAError = ERROR_SUCCESS;

	if (pAddr == nullptr || pStringBuf == nullptr)
	{
		WSAError = ERROR_INVALID_PARAMETER;
	}
	else
	{
		wchar_t BufferTemp[64];
		auto szString = pStringBuf;
		size_t cchString = 0;

		constexpr const char Hex[] = "0123456789abcdef";


		if (AF_INET == Family)
		{
			//IPv4
			if (StringBufSize < 16)
			{
				szString = BufferTemp;
			}

			auto& IPv4 = *((unsigned char(*)[4])pAddr);

			for (int i = 0; i != _countof(IPv4); ++i)
			{
				auto Num = IPv4[i];
				if (Num < 10)
				{
					//1 位
					szString[cchString++] = Hex[Num];
				}
				else if (Num < 100)
				{
					//2 位
					szString[cchString++] = Hex[Num / 10];
					szString[cchString++] = Hex[Num % 10];
				}
				else
				{
					//3 位
					szString[cchString++] = Hex[Num / 100];
					szString[cchString++] = Hex[(Num / 10) % 10];
					szString[cchString++] = Hex[Num % 10];
				}

				szString[cchString++] = '.';
			}

			--cchString;
		}
		else if (AF_INET6 == Family)
		{
			//IPv6
			if (StringBufSize < 46)
			{
				szString = BufferTemp;
			}

			auto& IPv6 = ((const IN6_ADDR*)pAddr)->u.Word;

			int ZeroIndex = -1;
			int ZeroCount = 1;

			//统计 连续 0个数最多的情况
			for (int i = 0; i != _countof(IPv6);)
			{
				auto j = i;

				for (; j != _countof(IPv6) && IPv6[j] == 0; ++j);

				auto Count = j - i;

				if (Count)
				{
					if (Count > ZeroCount)
					{
						ZeroCount = Count;
						ZeroIndex = i;
					}

					i = j;
				}
				else
				{
					++i;
				}
			}


			for (int i = 0; i != _countof(IPv6);)
			{
				if (ZeroIndex == i)
				{
					if (i == 0)
					{
						szString[cchString++] = ':';
					}

					szString[cchString++] = ':';
					i += ZeroCount;
				}
				else
				{
					auto Num = _byteswap_ushort(IPv6[i++]);

					if (Num <= 0xF)
					{
						//1 位
						szString[cchString++] = Hex[Num];
					}
					else if (Num <= 0xFF)
					{
						//2 位
						szString[cchString++] = Hex[Num >> 4];
						szString[cchString++] = Hex[Num & 0x0F];
					}
					else if (Num <= 0xFFF)
					{
						//3 位
						szString[cchString++] = Hex[(Num >> 8)];

						szString[cchString++] = Hex[(Num >> 4) & 0x0F];
						szString[cchString++] = Hex[(Num >> 0) & 0x0F];
					}
					else
					{
						//4位
						szString[cchString++] = Hex[(Num >> 12)];
						szString[cchString++] = Hex[(Num >> 8) & 0x0F];

						szString[cchString++] = Hex[(Num >> 4) & 0x0F];
						szString[cchString++] = Hex[(Num >> 0) & 0x0F];
					}

					if (i != _countof(IPv6))
						szString[cchString++] = ':';
				}
			}
		}
		else
		{
			WSAError = WSAEAFNOSUPPORT;
		}

		if (WSAError == ERROR_SUCCESS)
		{
			if (cchString >= StringBufSize)
			{
				WSAError = ERROR_INVALID_PARAMETER;
			}
			else
			{
				if (szString != BufferTemp)
				{
					//缓冲区不足

					memcpy(pStringBuf, szString, sizeof(szString[0]) * cchString);
				}

				pStringBuf[cchString] = '\0';

				return pStringBuf;
			}
		}
	}


	WSASetLastError(WSAError);
	return nullptr;
}

_LCRT_DEFINE_IAT_SYMBOL(InetNtopW, _16);

#endif

EXTERN_C_END