

namespace YY
{
	namespace Thunks
	{


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
BOOL
WINAPI
QueryThreadCycleTime(
    _In_ HANDLE ThreadHandle,
    _Out_ PULONG64 CycleTime
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pQueryThreadCycleTime = try_get_QueryThreadCycleTime())
	{
		return pQueryThreadCycleTime(ThreadHandle, CycleTime);
	}
	

	//GetThreadTimes凑合用吧……
	FILETIME CreationTime;
	FILETIME ExitTime;
	FILETIME KernelTime;
	FILETIME UserTime;

	if (!GetThreadTimes(ThreadHandle, &CreationTime, &ExitTime, &KernelTime, &UserTime))
	{
		return FALSE;
	}

	((ULARGE_INTEGER*)CycleTime)->LowPart = UserTime.dwLowDateTime;
	((ULARGE_INTEGER*)CycleTime)->HighPart = UserTime.dwHighDateTime;

	return TRUE;
}
#endif

__YY_Thunks_Expand_Function(kernel32, QueryThreadCycleTime, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
BOOL
WINAPI
QueryProcessCycleTime(
    _In_ HANDLE ProcessHandle,
    _Out_ PULONG64 CycleTime
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pQueryProcessCycleTime = try_get_QueryProcessCycleTime())
	{
		return pQueryProcessCycleTime(ProcessHandle, CycleTime);
	}

	//GetProcessTimes凑合用吧……
	FILETIME CreationTime;
	FILETIME ExitTime;
	FILETIME KernelTime;
	FILETIME UserTime;

	if (!GetProcessTimes(ProcessHandle, &CreationTime, &ExitTime, &KernelTime, &UserTime))
	{
		return FALSE;
	}

	((ULARGE_INTEGER*)CycleTime)->LowPart = UserTime.dwLowDateTime;
	((ULARGE_INTEGER*)CycleTime)->HighPart = UserTime.dwHighDateTime;

	return TRUE;
}
#endif

__YY_Thunks_Expand_Function(kernel32, QueryProcessCycleTime, 8);

#endif

	}//namespace Thunks

} //namespace YY