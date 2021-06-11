


namespace YY
{
	namespace Thunks
	{


#if (YY_Thunks_Support_Version < NTDDI_WS03)
//Windows Vista, Windows Server 2003

EXTERN_C
DWORD
WINAPI
GetCurrentProcessorNumber(
	VOID
    )
#ifdef YY_Thunks_Defined
	;
#else
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
#endif

__YY_Thunks_Expand_Function(kernel32, GetCurrentProcessorNumber, 0);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7, Windows Server 2008 R2

EXTERN_C
VOID
WINAPI
GetCurrentProcessorNumberEx(
	_Out_ PPROCESSOR_NUMBER ProcNumber
    )
#ifdef YY_Thunks_Defined
	;
#else
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
#endif

__YY_Thunks_Expand_Function(kernel32, GetCurrentProcessorNumberEx, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2003 [desktop apps | UWP apps]

//感谢 过客 提供
EXTERN_C
DWORD
WINAPI
GetThreadId(
    _In_ HANDLE Thread
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetThreadId = try_get_GetThreadId())
	{
		return pGetThreadId(Thread);
	}
	else if (auto pNtQueryInformationThread = try_get_NtQueryInformationThread())
	{
		THREAD_BASIC_INFORMATION ThreadBasicInfo;

		auto Status = pNtQueryInformationThread(Thread, ThreadBasicInformation, &ThreadBasicInfo, sizeof(ThreadBasicInfo), nullptr);

		if (Status < 0)
		{
			internal::BaseSetLastNTError(Status);
			return 0;
		}
		else
		{
			return (DWORD)ThreadBasicInfo.ClientId.UniqueThread;
		}
	}
	else
	{
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		return 0;
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetThreadId, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2003 [desktop apps | UWP apps]

EXTERN_C
DWORD
WINAPI
GetProcessIdOfThread(
    _In_ HANDLE Thread
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetProcessIdOfThread = try_get_GetProcessIdOfThread())
	{
		return pGetProcessIdOfThread(Thread);
	}
	else if (auto pNtQueryInformationThread = try_get_NtQueryInformationThread())
	{
		THREAD_BASIC_INFORMATION ThreadBasicInfo;

		auto Status = pNtQueryInformationThread(Thread, ThreadBasicInformation, &ThreadBasicInfo, sizeof(ThreadBasicInfo), nullptr);

		if (Status < 0)
		{
			internal::BaseSetLastNTError(Status);
			return 0;
		}
		else
		{
			return (DWORD)ThreadBasicInfo.ClientId.UniqueProcess;
		}
	}
	else
	{
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		return 0;
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetProcessIdOfThread, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WINXPSP1)
//Windows Vista, Windows XP with SP1 [desktop apps | UWP apps]
//Windows Server 2003 [desktop apps | UWP apps]

EXTERN_C
DWORD
WINAPI
GetProcessId(
    _In_ HANDLE Process
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetProcessId = try_get_GetProcessId())
	{
		return pGetProcessId(Process);
	}
	else if (auto pNtQueryInformationProcess = try_get_NtQueryInformationProcess())
	{
		PROCESS_BASIC_INFORMATION ProcessBasicInfo;

		auto Status = pNtQueryInformationProcess(Process, ProcessBasicInformation, &ProcessBasicInfo, sizeof(ProcessBasicInfo), nullptr);

		if (Status < 0)
		{
			internal::BaseSetLastNTError(Status);
			return 0;
		}
		else
		{
			return (DWORD)ProcessBasicInfo.UniqueProcessId;
		}
	}
	else
	{
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		return 0;
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetProcessId, 4);

#endif


	}//namespace Thunks

} //namespace YY