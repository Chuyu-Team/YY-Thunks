

namespace YY
{
	namespace Thunks
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

            // 使用中断时间模拟非中断时间，先合凑合吧……
            static LARGE_INTEGER _Frequency;
            if (_Frequency.QuadPart == 0ll)
            {
                if (!QueryPerformanceFrequency(&_Frequency))
                {
                    return FALSE;
                }
            }

            LARGE_INTEGER _PerformanceCount;
            if (!QueryPerformanceCounter(&_PerformanceCount))
            {
                return FALSE;
            }

            // 单位需要转换到 100纳秒
            *_puUnbiasedTime = _PerformanceCount.QuadPart * 1'000'000'0 / _Frequency.QuadPart;
            return TRUE;
        }
#endif
	}//namespace Thunks

} //namespace YY
