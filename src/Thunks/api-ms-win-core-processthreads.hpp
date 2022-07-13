﻿



#ifdef YY_Thunks_Implemented

//结构体来源：http://www.rohitab.com/discuss/topic/38601-proc-thread-attribute-list-structure-documentation/


// This structure stores the value for each attribute
typedef struct _PROC_THREAD_ATTRIBUTE_ENTRY
{
	DWORD_PTR   Attribute;  // PROC_THREAD_ATTRIBUTE_xxx
	SIZE_T      cbSize;
	PVOID       lpValue;
} PROC_THREAD_ATTRIBUTE_ENTRY, * LPPROC_THREAD_ATTRIBUTE_ENTRY;

// This structure contains a list of attributes that have been added using UpdateProcThreadAttribute
typedef struct _PROC_THREAD_ATTRIBUTE_LIST
{
	DWORD                          dwFlags;
	ULONG                          Size;
	ULONG                          Count;
	ULONG                          Reserved;
	LPPROC_THREAD_ATTRIBUTE_ENTRY  lpExtendedFlags;
	PROC_THREAD_ATTRIBUTE_ENTRY    Entries[ANYSIZE_ARRAY];
} PROC_THREAD_ATTRIBUTE_LIST, * LPPROC_THREAD_ATTRIBUTE_LIST;

#endif

namespace YY
{
	namespace Thunks
	{
#if defined(YY_Thunks_Implemented) && (YY_Thunks_Support_Version < NTDDI_WIN6)
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

		//Windows Vista, Windows XP Professional x64 Edition [desktop apps only]
		//Windows Server 2008, Windows Server 2003 with SP1 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		4,
		BOOL,
		WINAPI,
		SetThreadStackGuarantee,
			_Inout_ ULONG *StackSizeInBytes
		)
		{
			if (auto pSetThreadStackGuarantee = try_get_SetThreadStackGuarantee())
			{
				return pSetThreadStackGuarantee(StackSizeInBytes);
			}
			else
			{
				ULONG prev_size = ((TEB*)NtCurrentTeb())->GuaranteedStackBytes;
				ULONG new_size = (*StackSizeInBytes + 4095) & ~4095;

				/* at least 2 pages on 64-bit */
				if (sizeof(void*) > sizeof(int) && new_size) new_size = max(new_size, 8192);

				*StackSizeInBytes = prev_size;
				if (new_size >= (char*)((TEB*)NtCurrentTeb())->NtTib.StackBase - (char*)((TEB*)NtCurrentTeb())->DeallocationStack)
				{
					SetLastError(ERROR_INVALID_PARAMETER);
					return FALSE;
				}
				if (new_size > prev_size) ((TEB*)NtCurrentTeb())->GuaranteedStackBytes = (new_size + 4095) & ~4095;
				return TRUE;
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


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		16,
		_Success_(return != FALSE)
		BOOL,
		WINAPI,
		InitializeProcThreadAttributeList,
			_Out_writes_bytes_to_opt_(*lpSize,*lpSize) LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
			_In_ DWORD dwAttributeCount,
			_Reserved_ DWORD dwFlags,
			_When_(lpAttributeList == nullptr,_Out_) _When_(lpAttributeList != nullptr,_Inout_) PSIZE_T lpSize
			)
		{
			if (const auto pInitializeProcThreadAttributeList = try_get_InitializeProcThreadAttributeList())
			{
				return pInitializeProcThreadAttributeList(lpAttributeList, dwAttributeCount, dwFlags, lpSize);
			}


			LSTATUS lStatus = ERROR_SUCCESS;

			do
			{
				//参数验证
				if (dwFlags)
				{
					lStatus = ERROR_INVALID_PARAMETER;
					break;
				}

				//Vista只支持 3个 Type，所以只有三个
				if (dwAttributeCount > 3)
				{
					lStatus = ERROR_INVALID_PARAMETER;
					break;
				}


				const auto cbSize = *lpSize;
				const auto cbSizeNeed = sizeof(PROC_THREAD_ATTRIBUTE_LIST) + dwAttributeCount * sizeof(PROC_THREAD_ATTRIBUTE_ENTRY);
				*lpSize = cbSizeNeed;

				if (lpAttributeList == nullptr || cbSize < cbSizeNeed)
				{
					lStatus = ERROR_INSUFFICIENT_BUFFER;
					break;
				}

				lpAttributeList->dwFlags = 0;
				lpAttributeList->lpExtendedFlags = nullptr;
				lpAttributeList->Size = dwAttributeCount;
				lpAttributeList->Count = 0;

				return TRUE;

			} while (false);


			SetLastError(lStatus);
			return FALSE;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		DeleteProcThreadAttributeList,
			_Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList
			)
		{
			if (const auto pDeleteProcThreadAttributeList = try_get_DeleteProcThreadAttributeList())
			{
				return pDeleteProcThreadAttributeList(lpAttributeList);
			}


			//Vista原版什么也没有做……
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		28,
		BOOL,
		WINAPI,
		UpdateProcThreadAttribute,
			_Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
			_In_ DWORD dwFlags,
			_In_ DWORD_PTR Attribute,
			_In_reads_bytes_opt_(cbSize) PVOID lpValue,
			_In_ SIZE_T cbSize,
			_Out_writes_bytes_opt_(cbSize) PVOID lpPreviousValue,
			_In_opt_ PSIZE_T lpReturnSize
			)
		{
			if (const auto pUpdateProcThreadAttribute = try_get_UpdateProcThreadAttribute())
			{
				return pUpdateProcThreadAttribute(lpAttributeList, dwFlags, Attribute, lpValue, cbSize, lpPreviousValue, lpReturnSize);
			}

		
			LSTATUS lStatus = ERROR_SUCCESS;

			do
			{
				auto AttributeMark = 1ul << Attribute;

				/////////////////////////////////////////////////////
				//
				// 参数检查
				//

				if (dwFlags & (~PROC_THREAD_ATTRIBUTE_REPLACE_VALUE))
				{
					lStatus = ERROR_INVALID_PARAMETER;
					break;
				}


				if ((Attribute & PROC_THREAD_ATTRIBUTE_ADDITIVE) == 0)
				{
					if (lpAttributeList->Count == lpAttributeList->Size)
					{
						//internal::BaseSetLastNTError(0xC0000001);
						lStatus = ERROR_GEN_FAILURE;
						break;
					}
					else if (AttributeMark & lpAttributeList->dwFlags)
					{
						//internal::BaseSetLastNTError(0x40000000);
						lStatus = ERROR_OBJECT_NAME_EXISTS;
						break;
					}
					else if (lpPreviousValue)
					{
						//internal::BaseSetLastNTError(0xC00000F4);
						lStatus = ERROR_INVALID_PARAMETER;
						break;
					}
					else if (dwFlags & PROC_THREAD_ATTRIBUTE_REPLACE_VALUE)
					{
						//internal::BaseSetLastNTError(0xC00000F0);
						lStatus = ERROR_INVALID_PARAMETER;
						break;
					}
				}


				if ((PROC_THREAD_ATTRIBUTE_INPUT & Attribute) && lpReturnSize)
				{
					//internal::BaseSetLastNTError(0xC00000F5);
					lStatus = ERROR_INVALID_PARAMETER;
					break;
				}

				//
				//
				////////////////////////////////////////////////////


				auto pAttribute = &lpAttributeList->Entries[lpAttributeList->Count];

				constexpr auto ProcThreadAttributeExtendedFlags = 1;
				//0x60001，文档没有公开
				constexpr auto PROC_THREAD_ATTRIBUTE_EXTENDED_FLAGS = ProcThreadAttributeValue(ProcThreadAttributeExtendedFlags, FALSE, TRUE, TRUE);


				if (Attribute == PROC_THREAD_ATTRIBUTE_PARENT_PROCESS) //0x20000
				{
					//WinXP不支持 UAC，没实现似乎也没什么的。

					if (cbSize != sizeof(HANDLE))
					{
						//internal::BaseSetLastNTError(0xC0000004);
						lStatus = ERROR_INVALID_PARAMETER;
						break;
					}
				}
				else if (Attribute == PROC_THREAD_ATTRIBUTE_EXTENDED_FLAGS) //0x60001
				{
					//系统没有公开这个含义，暂时让他允许通过把……

					if (cbSize != sizeof(DWORD))
					{
						//internal::BaseSetLastNTError(0xC0000004);
						lStatus = ERROR_INVALID_PARAMETER;
						break;
					}

					DWORD dwOrgFlags;

					if (lpAttributeList->lpExtendedFlags)
					{
						pAttribute = lpAttributeList->lpExtendedFlags;
						dwOrgFlags = (DWORD)lpAttributeList->lpExtendedFlags->lpValue;
						AttributeMark = 0;
					}
					else
					{
						lpAttributeList->lpExtendedFlags = pAttribute;
						dwOrgFlags = 0;
					}


					auto dwNewFlags = *(DWORD*)lpValue;

					if (dwNewFlags & ~0x00000003ul)
					{
						//internal::BaseSetLastNTError(0xC000000D);
						lStatus = ERROR_BAD_LENGTH;
						break;
					}

					if ((dwFlags & PROC_THREAD_ATTRIBUTE_REPLACE_VALUE) == 0 && dwOrgFlags)
					{
						dwNewFlags |= dwOrgFlags;
					}

					if (lpPreviousValue)
						*(DWORD*)lpPreviousValue = dwOrgFlags;

					lpValue = (PVOID)dwNewFlags;
				}
				else if (Attribute == PROC_THREAD_ATTRIBUTE_HANDLE_LIST) //0x20002
				{
					//WinXP也不支持指定句柄继承，他会直接继承所有可继承的句柄，所以没实现好像也没什么大不了的。

					if (cbSize == 0 || cbSize % sizeof(HANDLE) != 0)
					{
						//internal::BaseSetLastNTError(0xC0000004);
						lStatus = ERROR_INVALID_PARAMETER;
						break;
					}
				}
				else
				{
					//internal::BaseSetLastNTError(0xC00000BB);
					lStatus = ERROR_NOT_SUPPORTED;
					break;
				}

				//LABEL_17
				pAttribute->lpValue = lpValue;

				if (AttributeMark)
				{
					pAttribute->Attribute = Attribute;
					pAttribute->cbSize = cbSize;
					++lpAttributeList->Count;
					lpAttributeList->dwFlags |= AttributeMark;
				}

				return TRUE;

			} while (false);

			SetLastError(lStatus);
			
			return FALSE;
		}
#endif
	}//namespace Thunks

} //namespace YY
