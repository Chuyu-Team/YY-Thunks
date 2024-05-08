 

namespace YY
{
	namespace Thunks
	{
#if !defined(_USER_SHARED_DATA_LITE_DEFINED)
#define _USER_SHARED_DATA_LITE_DEFINED 1
typedef struct _KSYSTEM_TIME_LITE 
{
	ULONG LowPart;
	LONG High1Time;
	LONG High2Time;
} KSYSTEM_TIME_LITE, *PKSYSTEM_TIME_LITE;

typedef struct _KUSER_SHARED_DATA_LITE 
{
	ULONG TickCountLowDeprecated;
	ULONG TickCountMultiplier;
	volatile KSYSTEM_TIME_LITE InterruptTime;
	volatile KSYSTEM_TIME_LITE SystemTime;
	volatile KSYSTEM_TIME_LITE TimeZoneBias;
} KUSER_SHARED_DATA_LITE, *PKUSER_SHARED_DATA_LITE;
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps only]
		//Windows Server 2008 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		8,
		_Success_(return)
		BOOL,
		WINAPI,
		QueryThreadCycleTime,
			_In_ HANDLE ThreadHandle,
			_Out_ PULONG64 CycleTime
			)
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


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps only]
		//Windows Server 2008 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		8,
		_Success_(return)
		BOOL,
		WINAPI,
		QueryProcessCycleTime,
			_In_ HANDLE ProcessHandle,
			_Out_ PULONG64 CycleTime
			)
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


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		// 最低受支持的客户端	Windows 7、Windows 8 [桌面应用 |UWP 应用]
        // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		kernel32,
		4,
		BOOL,
		WINAPI,
		QueryUnbiasedInterruptTime,
            _Out_ PULONGLONG _puUnbiasedTime
			)
		{
			if (const auto _pfnQueryUnbiasedInterruptTime = try_get_QueryUnbiasedInterruptTime())
			{
				return _pfnQueryUnbiasedInterruptTime(_puUnbiasedTime);
			}

            if (!_puUnbiasedTime)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            //Read InterruptTime from SharedUserData at address 0x7ffe0000;
            ULONG HighPart = 0, LowPart = 0;
            const PKUSER_SHARED_DATA_LITE SharedUserData = (PKUSER_SHARED_DATA_LITE)0x7ffe0000;

            while (TRUE) 
            {
                HighPart = SharedUserData->InterruptTime.High1Time;
                LowPart = SharedUserData->InterruptTime.LowPart;
                if (HighPart == SharedUserData->InterruptTime.High2Time) 
                {
                  break;
                }
            }
            *_puUnbiasedTime = (ULONGLONG)HighPart << 32 | LowPart;
            return TRUE;
        }
#endif
	}//namespace Thunks

} //namespace YY
