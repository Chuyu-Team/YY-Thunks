

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7 [desktop apps | UWP apps]
//Windows Server 2008 R2 [desktop apps | UWP apps]

EXTERN_C
VOID
WINAPI
RaiseFailFastException(
    _In_opt_ PEXCEPTION_RECORD pExceptionRecord,
    _In_opt_ PCONTEXT pContextRecord,
    _In_ DWORD dwFlags
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pRaiseFailFastException = try_get_RaiseFailFastException())
	{
		return pRaiseFailFastException(pExceptionRecord, pContextRecord, dwFlags);
	}

	//直接结束进程
	TerminateProcess(NtGetCurrentProcess(), pExceptionRecord ? pExceptionRecord->ExceptionCode : STATUS_FAIL_FAST_EXCEPTION);
}
#endif

__YY_Thunks_Expand_Function(kernel32, RaiseFailFastException, 12);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7 [desktop apps | UWP apps]
//Windows Server 2008 R2 [desktop apps | UWP apps]

EXTERN_C
BOOL
WINAPI
SetThreadErrorMode(
    _In_ DWORD dwNewMode,
    _In_opt_ LPDWORD lpOldMode
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pSetThreadErrorMode = try_get_SetThreadErrorMode())
	{
		return pSetThreadErrorMode(dwNewMode, lpOldMode);
	}

	auto dwOldMode = SetErrorMode(dwNewMode);
	if (lpOldMode)
		*lpOldMode = dwOldMode;

	return TRUE;
}
#endif

__YY_Thunks_Expand_Function(kernel32, SetThreadErrorMode, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7 [desktop apps | UWP apps]
//Windows Server 2008 R2 [desktop apps | UWP apps]

EXTERN_C
DWORD
WINAPI
GetThreadErrorMode(
    VOID
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetThreadErrorMode = try_get_GetThreadErrorMode())
	{
		return pGetThreadErrorMode();
	}
	
	return GetErrorMode();
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetThreadErrorMode, 0);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
UINT
WINAPI
GetErrorMode(
	VOID
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetErrorMode = try_get_GetErrorMode())
	{
		return pGetErrorMode();
	}
	else if (auto pNtQueryInformationProcess = try_get_NtQueryInformationProcess())
	{
		DWORD dwDefaultHardErrorMode;

		auto Status = pNtQueryInformationProcess(NtCurrentProcess(), ProcessDefaultHardErrorMode, &dwDefaultHardErrorMode, sizeof(dwDefaultHardErrorMode), nullptr);

		if (Status >= 0)
		{
			if (dwDefaultHardErrorMode & 0x00000001)
			{
				return dwDefaultHardErrorMode & 0xFFFFFFFE;
			}
			else
			{
				return dwDefaultHardErrorMode | 0x00000001;
			}
		}

		internal::BaseSetLastNTError(Status);

		return 0;
	}
	else
	{
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		return 0;
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetErrorMode, 0);

#endif

	}//namespace Thunks

} //namespace YY