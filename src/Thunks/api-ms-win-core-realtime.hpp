 

namespace YY::Thunks::Fallback
{
    namespace
    {
#if (YY_Thunks_Support_Version < NTDDI_WIN10) && defined(YY_Thunks_Implemented)
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
    }
}

namespace YY::Thunks
{
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
        Fallback::QueryInterruptTimeDownlevel(_puUnbiasedTime);
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

        // 故意模拟微软原版行为，触发空指针时的内存非法访问。
        *_puUnbiasedInterruptTimePrecise = 0;
        QueryUnbiasedInterruptTime(_puUnbiasedInterruptTimePrecise);
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

        Fallback::QueryInterruptTimeDownlevel(_puInterruptTime);
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

        Fallback::QueryInterruptTimeDownlevel(_puInterruptTimePrecise);
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
            
        // Windows Vista开始才支持：SystemProcessorIdleCycleTimeInformation，目前只能自己实现一份。
        // 目前随便写一个值，表示现在的空闲时间，一般情况下随便写没有太大问题

        const auto _uNumberOfProcessors = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->NumberOfProcessors;
        auto _cBuffer = *_pBufferLength / sizeof(_pProcessorIdleCycleTime[0]);
        *_pBufferLength = _uNumberOfProcessors * sizeof(_pProcessorIdleCycleTime[0]);

        // 仅返回 BufferLength
        if (!_pProcessorIdleCycleTime)
        {
            return TRUE;
        }

        if (_cBuffer < _uNumberOfProcessors)
        {
            // 没有错，缓冲区不足时也填充缓冲区。微软原版就是这样的。
            while (_cBuffer)
            {
                *_pProcessorIdleCycleTime = 568556351955196ll;
                ++_pProcessorIdleCycleTime;
                --_cBuffer;
            }
            SetLastError(ERROR_BAD_LENGTH);
            return FALSE;
        }
        else
        {
            _cBuffer = _uNumberOfProcessors;
            while (_cBuffer)
            {
                *_pProcessorIdleCycleTime = 568556351955196ll;
                ++_pProcessorIdleCycleTime;
                --_cBuffer;
            }
            return TRUE;
        }
    }
#endif
} //namespace YY::Thunks
