

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_1)

    //Windows 7 [desktop apps | UWP apps]
    //Windows Server 2008 R2 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    12,
    VOID,
    WINAPI,
    RaiseFailFastException,
        _In_opt_ PEXCEPTION_RECORD pExceptionRecord,
        _In_opt_ PCONTEXT pContextRecord,
        _In_ DWORD dwFlags
        )
    {
        if (auto pRaiseFailFastException = try_get_RaiseFailFastException())
        {
            return pRaiseFailFastException(pExceptionRecord, pContextRecord, dwFlags);
        }

        //直接结束进程
        TerminateProcess(NtGetCurrentProcess(), pExceptionRecord ? pExceptionRecord->ExceptionCode : STATUS_FAIL_FAST_EXCEPTION);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    //Windows 7 [desktop apps | UWP apps]
    //Windows Server 2008 R2 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    8,
    BOOL,
    WINAPI,
    SetThreadErrorMode,
        _In_ DWORD dwNewMode,
        _In_opt_ LPDWORD lpOldMode
        )
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


#if (YY_Thunks_Target < __WindowsNT6_1)

    //Windows 7 [desktop apps | UWP apps]
    //Windows Server 2008 R2 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    0,
    DWORD,
    WINAPI,
    GetThreadErrorMode,
        VOID
        )
    {
        if (auto pGetThreadErrorMode = try_get_GetThreadErrorMode())
        {
            return pGetThreadErrorMode();
        }
    
        return GetErrorMode();
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    0,
    UINT,
    WINAPI,
    GetErrorMode,
        VOID
        )
    {
        if (auto pGetErrorMode = try_get_GetErrorMode())
        {
            return pGetErrorMode();
        }
        
#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryInformationProcess = try_get_NtQueryInformationProcess();
        if (!NtQueryInformationProcess)
        {
            SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
            return 0;
        }
#endif
        DWORD dwDefaultHardErrorMode;

        LONG Status = NtQueryInformationProcess(NtCurrentProcess(), ProcessDefaultHardErrorMode, &dwDefaultHardErrorMode, sizeof(dwDefaultHardErrorMode), nullptr);

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
#endif
} //namespace YY::Thunks
