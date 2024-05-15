 

namespace YY
{
	namespace Thunks
	{
#if (YY_Thunks_Support_Version < NTDDI_WIN7) && defined(YY_Thunks_Implemented)
        static VOID WINAPI QueryInterruptTimeDownlevel(
            _Out_ PULONGLONG _puInterruptTime
			)
		{
            ULARGE_INTEGER _uInterruptTime;
            while (true)
            {
                _uInterruptTime.HighPart = SharedUserData->InterruptTime.High1Time;
                _uInterruptTime.LowPart = SharedUserData->InterruptTime.LowPart;
                if (_uInterruptTime.HighPart == SharedUserData->InterruptTime.High2Time)
                {
                    break;
                }

                YieldProcessor();
            }
            *_puInterruptTime = _uInterruptTime.QuadPart;
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

            // 用中断时间模拟非中断时间，凑合一下吧。
            QueryInterruptTimeDownlevel(_puUnbiasedTime);
            return TRUE;
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		// 最低受支持的客户端	Windows 10 [桌面应用 |UWP 应用]
        // 最低受支持的服务器	Windows Server 2016[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		api_ms_win_core_realtime_l1_1_1,
		4,
		VOID,
        WINAPI,
        QueryUnbiasedInterruptTimePrecise,
            _Out_ PULONGLONG _puUnbiasedInterruptTimePrecise
			)
		{
			if (const auto _pfnQueryUnbiasedInterruptTimePrecise = try_get_QueryUnbiasedInterruptTimePrecise())
			{
				return _pfnQueryUnbiasedInterruptTimePrecise(_puUnbiasedInterruptTimePrecise);
			}

            // 用中断时间模拟非中断时间，凑合一下吧。
            QueryInterruptTimeDownlevel(_puUnbiasedInterruptTimePrecise);
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		// 最低受支持的客户端	Windows 10 [桌面应用 |UWP 应用]
        // 最低受支持的服务器	Windows Server 2016[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		api_ms_win_core_realtime_l1_1_1,
		4,
		VOID,
        WINAPI,
        QueryInterruptTime,
            _Out_ PULONGLONG _puInterruptTime
			)
		{
			if (const auto _pfnQueryInterruptTime = try_get_QueryInterruptTime())
			{
				return _pfnQueryInterruptTime(_puInterruptTime);
			}

            QueryInterruptTimeDownlevel(_puInterruptTime);
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		// 最低受支持的客户端	Windows 10 [桌面应用 |UWP 应用]
        // 最低受支持的服务器	Windows Server 2016[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		api_ms_win_core_realtime_l1_1_1,
		4,
		VOID,
        WINAPI,
        QueryInterruptTimePrecise,
            _Out_ PULONGLONG _puInterruptTimePrecise
			)
		{
			if (const auto _pfnQueryInterruptTimePrecise = try_get_QueryInterruptTimePrecise())
			{
				return _pfnQueryInterruptTimePrecise(_puInterruptTimePrecise);
			}

            QueryInterruptTimeDownlevel(_puInterruptTimePrecise);
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		// 最低受支持的客户端	Windows 7 [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
		__DEFINE_THUNK(
		kernel32,
		12,
		BOOL,
        WINAPI,
        QueryIdleProcessorCycleTimeEx,
            _In_ USHORT _uGroup,
            _Inout_ PULONG _puBufferLength,
            _Out_writes_bytes_opt_(*_puBufferLength) PULONG64 _puProcessorIdleCycleTime
			)
		{
			if (const auto _pfnQueryIdleProcessorCycleTimeEx = try_get_QueryIdleProcessorCycleTimeEx())
			{
				return _pfnQueryIdleProcessorCycleTimeEx(_uGroup, _puBufferLength, _puProcessorIdleCycleTime);
			}
            
            // 不支持的平台统一认为只有1组CPU可用
            if (_uGroup != 0)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            return QueryIdleProcessorCycleTime(_puBufferLength, _puProcessorIdleCycleTime);
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
		__DEFINE_THUNK(
		kernel32,
		8,
		BOOL,
        WINAPI,
        QueryIdleProcessorCycleTime,
            _Inout_ PULONG _pBufferLength,
            _Out_writes_bytes_opt_(*_pBufferLength) PULONG64 _pProcessorIdleCycleTime
			)
		{
			if (const auto _pfnQueryIdleProcessorCycleTime = try_get_QueryIdleProcessorCycleTime())
			{
				return _pfnQueryIdleProcessorCycleTime(_pBufferLength, _pProcessorIdleCycleTime);
			}
            
            const auto _pfnNtQuerySystemInformation = try_get_NtQuerySystemInformation();
            if (!_pfnNtQuerySystemInformation)
            {
                SetLastError(ERROR_NOT_SUPPORTED);
                return FALSE;
            }

            long _Status = _pfnNtQuerySystemInformation(SystemProcessorIdleCycleTimeInformation, _pProcessorIdleCycleTime, *_pBufferLength, _pBufferLength);
            if (_Status < 0 && _pProcessorIdleCycleTime)
            {
                internal::BaseSetLastNTError(_Status);
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
#endif
	}//namespace Thunks

} //namespace YY
