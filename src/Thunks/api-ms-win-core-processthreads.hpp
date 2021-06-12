


namespace YY
{
	namespace Thunks
	{


#if (YY_Thunks_Support_Version < NTDDI_WS03)

		//Windows Vista, Windows Server 2003
		__DEFINE_THUNK(
		kernel32,
		0,
		DWORD,
		WINAPI,
		GetCurrentProcessorNumber,
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
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		//Windows 7, Windows Server 2008 R2
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		GetCurrentProcessorNumberEx,
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
#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2003 [desktop apps | UWP apps]
		//感谢 过客 提供
		__DEFINE_THUNK(
		kernel32,
		4,
		DWORD,
		WINAPI,
		GetThreadId,
			_In_ HANDLE Thread
			)
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


#if (YY_Thunks_Support_Version < NTDDI_WS03)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2003 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		DWORD,
		WINAPI,
		GetProcessIdOfThread,
			_In_ HANDLE Thread
			)
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


#if (YY_Thunks_Support_Version < NTDDI_WINXPSP1)

		//Windows Vista, Windows XP with SP1 [desktop apps | UWP apps]
		//Windows Server 2003 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		DWORD,
		WINAPI,
		GetProcessId,
			_In_ HANDLE Process
			)
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


	}//namespace Thunks

} //namespace YY