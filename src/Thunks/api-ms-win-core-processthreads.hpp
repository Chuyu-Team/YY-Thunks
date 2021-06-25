


namespace YY
{
	namespace Thunks
	{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

#ifdef YY_Thunks_Implemented
		namespace Fallback
		{
			static void __cdecl UninitPageVirtualProtect();

			static char* volatile *GetPageVirtualProtect()
			{
				//注册 m_pPageVirtualProtect 的反初始化工作
				__declspec(allocate(".YYThr$AAB")) static void* RunUninitPageVirtualProtect = reinterpret_cast<void*>(&Fallback::UninitPageVirtualProtect);

				__foreinclude(RunUninitPageVirtualProtect);

				static char* volatile m_pPageVirtualProtect = nullptr;

				return &m_pPageVirtualProtect;
			}

			static void __cdecl UninitPageVirtualProtect()
			{
				auto pOrgPageVirtualProtect = (char*)InterlockedExchangePointer((PVOID volatile*)GetPageVirtualProtect(), INVALID_HANDLE_VALUE);

				if (pOrgPageVirtualProtect != nullptr && pOrgPageVirtualProtect != (char*)INVALID_HANDLE_VALUE)
				{
					VirtualFree(pOrgPageVirtualProtect, 0, MEM_RELEASE);
				}
			}
		}
		
#endif
#endif

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


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		0,
		VOID,
		WINAPI,
		FlushProcessWriteBuffers,
			VOID
			)
		{
			if (auto pFlushProcessWriteBuffers = try_get_FlushProcessWriteBuffers())
			{
				return pFlushProcessWriteBuffers();
			}


			/*
			* 参考了 14.29.30037 crt\src\concrt\ResourceManager.cpp 实现
			
			*/

			for (;;)
			{
				auto pOrgPageVirtualProtect = (char*)InterlockedCompareExchangePointer((PVOID volatile*)Fallback::GetPageVirtualProtect(), nullptr, nullptr);


				//内存存在异常，我们什么也不做。
				if (pOrgPageVirtualProtect == (char*)INVALID_HANDLE_VALUE)
				{
					break;
				}
				else if (pOrgPageVirtualProtect == nullptr)
				{
					//我们需要开辟一段内存

					auto pPageVirtualProtectTmp = (char*)VirtualAlloc(NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

					if (pPageVirtualProtectTmp == nullptr)
					{
						//内存申请失败，直接设置为 -1，并且什么也不做
						InterlockedCompareExchangePointer((PVOID volatile*)Fallback::GetPageVirtualProtect(), INVALID_HANDLE_VALUE, nullptr);
						break;
					}

					*pPageVirtualProtectTmp = 1;

					pOrgPageVirtualProtect = (char*)InterlockedCompareExchangePointer((PVOID volatile*)Fallback::GetPageVirtualProtect(), pPageVirtualProtectTmp, nullptr);

					if (pOrgPageVirtualProtect != nullptr)
					{
						//说明另外一个线程已经设置，我们释放这次申请的内存
						VirtualFree(pPageVirtualProtectTmp, 0, MEM_RELEASE);
					}
					else
					{
						pOrgPageVirtualProtect = pPageVirtualProtectTmp;
					}
				}


				// Note that the read of *m_pPageVirtualProtect is very important, as it makes it extremely likely that this memory will
				// be in the working set when we call VirtualProtect (see comments below).
				if (*pOrgPageVirtualProtect == 1)
				{
					//
					// VirtualProtect simulates FlushProcessWriteBuffers because it happens to send an inter-processor interrupt to all CPUs,
					// and inter-processor interrupts happen to cause the CPU's store buffers to be flushed.
					//
					// Unfortunately, VirtualProtect only does this if the page whose status is being changed is in the process' working set
					// (otherwise there's no need to tell the other CPUs that anything has changed).
					//
					// One way to do this is to lock the page into the process' working set. Unfortunately, it can fail if there are already too many
					// locked pages.
					//
					// We could increase the process' working set limit, using SetProcessWorkingSet, but that would be a) intrusive (the process may
					// have its own idea of what the limit should be), and b) race-prone (another thread may be trying to adjust the limit, to a
					// different value, at the same time).
					//
					// We could stop using *m_pPageVirtualProtect as the page we fiddle with, and instead use a page we know is already locked into
					// the working set. There's no way to enumerate such pages, so it'd have to be a well-known fixed location that we know is always
					// locked, and that can have its protection fiddled with without consequence.  We know of no such location, and if we did it would
					// undoubtedly be some internal Windows data structure that would be subject to changes in the way its memory is handled at any time.
					//
					// The VirtualProtect trick has worked for many years in the CLR, without the call to VirtualLock, without apparent problems.
					// Part of the reason is because of the equivalent of the check of *m_pPageVirtualProtect above.
					//
					DWORD oldProtect;

					// We have it on good authority from the kernel team that, although VirtualProtect is repeatedly called with the
					// same protection (PAGE_READONLY), the OS will not optimize out the flush buffers as a result.
					VirtualProtect(pOrgPageVirtualProtect, sizeof(BYTE), PAGE_READONLY, &oldProtect);
				}

				break;
			}
		}

#endif

	}//namespace Thunks

} //namespace YY