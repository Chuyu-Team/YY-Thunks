

#ifdef YY_Thunks_Implemented

//读写锁参考了https://blog.csdn.net/yichigo/article/details/36898561
#define YY_SRWLOCK_Locked_BIT           0
#define YY_SRWLOCK_Waiting_BIT          1
#define YY_SRWLOCK_Waking_BIT           2
#define YY_SRWLOCK_MultipleShared_BIT   3
//已经有人获得这个锁
#define YY_SRWLOCK_Locked               0x00000001ul
//有人正在等待锁
#define YY_SRWLOCK_Waiting              0x00000002ul
//有人正在唤醒锁
#define YY_SRWLOCK_Waking               0x00000004ul
//
#define YY_SRWLOCK_MultipleShared       0x00000008ul

#define YY_SRWLOCK_MASK    (size_t(0xF))
#define YY_SRWLOCK_BITS    4
#define YY_SRWLOCK_GET_BLOCK(SRWLock) ((YY_SRWLOCK_WAIT_BLOCK*)(SRWLock & (~YY_SRWLOCK_MASK)))

//SRWLock自旋次数
#define SRWLockSpinCount 1024

//取自Win7
struct _YY_RTL_SRWLOCK
{
	union
	{
		struct
		{
			ULONG_PTR Locked : 1;       
			ULONG_PTR Waiting : 1;           
			ULONG_PTR Waking : 1;            
			ULONG_PTR MultipleShared : 1;
#ifdef _WIN64
			ULONG_PTR Shared : 60;
#else
			ULONG_PTR Shared : 28;
#endif
		};
		ULONG_PTR Value;
		VOID* Ptr;
	};
};

typedef struct __declspec(align(16)) _YY_SRWLOCK_WAIT_BLOCK
{
	_YY_SRWLOCK_WAIT_BLOCK* back;
	_YY_SRWLOCK_WAIT_BLOCK* notify;
	_YY_SRWLOCK_WAIT_BLOCK* next;
	volatile size_t         shareCount;
	volatile size_t         flag;
} YY_SRWLOCK_WAIT_BLOCK;


//正在优化锁
#define YY_CV_OPTIMIZE_LOCK 0x00000008ul
#define YY_CV_MASK size_t(0x0000000F)
#define YY_CV_GET_BLOCK(CV) ((YY_CV_WAIT_BLOCK*)(CV & (~YY_CV_MASK)))

#define ConditionVariableSpinCount 1024

typedef struct __declspec(align(16)) _YY_CV_WAIT_BLOCK
{
	_YY_CV_WAIT_BLOCK* back;
	_YY_CV_WAIT_BLOCK* notify;
	_YY_CV_WAIT_BLOCK* next;
	volatile size_t    shareCount;
	volatile size_t    flag;
	volatile PSRWLOCK  SRWLock;
} YY_CV_WAIT_BLOCK;


#define YY_ADDRESS_GET_BLOCK(AW) ((YY_ADDRESS_WAIT_BLOCK*)(size_t(AW) & (~size_t(0x3))))

//WaitOnAddress自旋次数
#define RtlpWaitOnAddressSpinCount 1024

typedef struct __declspec(align(8)) _YY_ADDRESS_WAIT_BLOCK
{
	volatile void* Address;
	//因为Windows 8以及更高版本才支持 ZwWaitForAlertByThreadId，所以我们直接把 ThreadId 砍掉了，反正没鸟用
	//ULONG_PTR            ThreadId;

	// 它是后继
	_YY_ADDRESS_WAIT_BLOCK* back;
	// 它是前驱
	_YY_ADDRESS_WAIT_BLOCK* notify;
	// 似乎指向Root，但是Root时才指向自己，其余情况为 nullptr，这是一种安全性？
	_YY_ADDRESS_WAIT_BLOCK* next;
	volatile size_t         flag;

} YY_ADDRESS_WAIT_BLOCK;


//YY-Thunks中Barrier采用了Windows 8实现
typedef struct _YY_BARRIER {
	volatile LONG  lRemainderThreads;
	volatile LONG  lTotalThreads;
	HANDLE         hEvent[2];
	DWORD          dwNumProcessors;
	DWORD          dwSpinCount;
} YY_BARRIER;

static_assert(sizeof(SYNCHRONIZATION_BARRIER) >= sizeof(YY_BARRIER), "必须跟系统ABI兼容！！！！");

#endif



namespace YY
{
	namespace Thunks
	{
#ifdef YY_Thunks_Implemented
		namespace internal
		{
			static HANDLE __fastcall GetGlobalKeyedEventHandle()
			{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)
				//Windows XP等平台则 使用系统自身的 CritSecOutOfMemoryEvent，Vista或者更高平台 我们直接返回 nullptr 即可。
				if (NtCurrentTeb()->ProcessEnvironmentBlock->OSMajorVersion < 6)
				{
					if (_GlobalKeyedEventHandle == nullptr)
					{
						auto pNtOpenKeyedEvent = try_get_NtOpenKeyedEvent();

						if(pNtOpenKeyedEvent == nullptr)
							RaiseStatus(STATUS_RESOURCE_NOT_OWNED);

						constexpr const wchar_t Name[] = L"\\KernelObjects\\CritSecOutOfMemoryEvent";

						UNICODE_STRING ObjectName = {sizeof(Name) - sizeof(wchar_t),sizeof(Name) - sizeof(wchar_t) ,(PWSTR)Name };
						OBJECT_ATTRIBUTES attr = { sizeof(attr),nullptr,&ObjectName };

						HANDLE KeyedEventHandle;

						if (pNtOpenKeyedEvent(&KeyedEventHandle, MAXIMUM_ALLOWED, &attr) < 0)
						{
							RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
						}

						if (InterlockedCompareExchange((size_t*)&_GlobalKeyedEventHandle, (size_t)KeyedEventHandle, (size_t)nullptr))
						{
							CloseHandle(KeyedEventHandle);
						}
					}

					return _GlobalKeyedEventHandle;
				}
#endif
				//Vista以上平台支持给 KeyedEvent直接传 nullptr
				return nullptr;
			}

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

			static void __fastcall RtlpWakeSRWLock(SRWLOCK* SRWLock, size_t Status)
			{
				auto GlobalKeyedEventHandle = GetGlobalKeyedEventHandle();
				auto pNtReleaseKeyedEvent = try_get_NtReleaseKeyedEvent();

				if (!pNtReleaseKeyedEvent)
				{
					RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
				}

				for (;;)
				{
					if ((Status & YY_SRWLOCK_Locked) == 0)
					{
						//微软就不判断下空指针？如此自信？
						auto pWatiBlock = YY_SRWLOCK_GET_BLOCK(Status);
						
						YY_SRWLOCK_WAIT_BLOCK* notify;

						for (auto pBlock = pWatiBlock; (notify = pBlock->notify) == nullptr;)
						{
							auto back = pBlock->back;
							back->next = pBlock;

							pBlock = back;
						}
						
						pWatiBlock->notify = notify;


						//判断是否是一个独占链
						if (notify->next && (notify->flag & 1))
						{
							pWatiBlock->notify = notify->next;
							notify->next = nullptr;

							//SRWLock & (~YY_SRWLOCK_Waking)
#ifdef _WIN64
							_InterlockedAnd64((volatile LONG_PTR *)SRWLock, ~LONG_PTR(YY_SRWLOCK_Waking));
#else
							_InterlockedAnd((volatile LONG_PTR *)SRWLock, ~LONG_PTR(YY_SRWLOCK_Waking));
#endif
							if (!InterlockedBitTestAndReset((volatile LONG*)&notify->flag, 1))
							{
								//块处于等待状态，我们进行线程唤醒

								//if(!RtlpWaitCouldDeadlock())
								
								pNtReleaseKeyedEvent(GlobalKeyedEventHandle, notify, 0, nullptr);
							}

							return;
						}
						else
						{
							//等待的是一个共享锁，那么唤醒所有等待的共享锁。
							auto NewStatus = InterlockedCompareExchange((volatile size_t *)SRWLock, 0, Status);

							if (NewStatus == Status)
							{
								//更新成功！
								for (; notify;)
								{
									auto next = notify->next;


									if (!InterlockedBitTestAndReset((volatile LONG*)&notify->flag, 1))
									{
										//块处于等待状态，我们进行线程唤醒

										//if(!RtlpWaitCouldDeadlock())

										pNtReleaseKeyedEvent(GlobalKeyedEventHandle, notify, 0, nullptr);
									}

									notify = next;
								}

								return;
							}

							Status = NewStatus;
						}

						pWatiBlock->notify = notify;
					}
					else
					{
						auto NewStatus = InterlockedCompareExchange((volatile size_t*)SRWLock, Status & ~YY_SRWLOCK_Waking, Status);
						if (NewStatus == Status)
							return;

						Status = NewStatus;
					}
				}
			}

			static void __fastcall RtlpOptimizeSRWLockList(SRWLOCK* SRWLock, size_t Status)
			{
				for (;;)
				{
					if (Status & YY_SRWLOCK_Locked)
					{
						if (auto WatiBlock = (YY_SRWLOCK_WAIT_BLOCK*)(Status & (~YY_SRWLOCK_MASK)))
						{
							auto pBlock = WatiBlock;

							for (; pBlock->notify == nullptr;)
							{
								auto back = pBlock->back;
								back->next = pBlock;

								pBlock = back;
							}

							WatiBlock->notify = pBlock->notify;
						}

						//微软为什么用 Status - YY_SRWLOCK_Waking，而为什么不用 Status & ~YY_SRWLOCK_Waking ？
						auto CurrentStatus = InterlockedCompareExchange((volatile size_t *)SRWLock, Status - YY_SRWLOCK_Waking, Status);
						if (CurrentStatus == Status)
							break;

						Status = CurrentStatus;
					}
					else
					{
						RtlpWakeSRWLock(SRWLock, Status);
						break;
					}
				}
			}

			//将等待块插入 SRWLock 中
			static BOOL __fastcall RtlpQueueWaitBlockToSRWLock(YY_CV_WAIT_BLOCK* pBolck, PSRWLOCK SRWLock, DWORD SRWLockMark)
			{
				for (;;)
				{
					auto Current = *(volatile size_t*)SRWLock;

					if ((Current & 0x1) == 0)
						break;


					if (SRWLockMark == 0)
					{
						pBolck->flag |= 0x1;
					}
					else if ((Current & 0x2) == 0 && YY_SRWLOCK_GET_BLOCK(Current))
					{
						return FALSE;
					}

					pBolck->next = nullptr;

					size_t New;

					if (Current & 0x2)
					{
						pBolck->notify = nullptr;
						pBolck->shareCount = 0;

						//_YY_CV_WAIT_BLOCK 结构体跟 _YY_SRWLOCK_WAIT_BLOCK兼容，所以能这样强转
						pBolck->back = (_YY_CV_WAIT_BLOCK*)YY_SRWLOCK_GET_BLOCK(Current);

						New = size_t(pBolck) | (Current & YY_CV_MASK);
					}
					else
					{
						auto shareCount = Current >> 4;

						pBolck->shareCount = shareCount;
						pBolck->notify = pBolck;
						New = shareCount <= 1 ? (size_t(pBolck) | 0x3) : (size_t(pBolck) | 0xB);
					}

					//清泠 发现的Bug，我们应该返回 TRUE，减少必要的内核等待。
					if (InterlockedCompareExchange((volatile size_t*)SRWLock, New, Current) == Current)
						return TRUE;

					//RtlBackoff(&v7);
					YieldProcessor();
				}

				return FALSE;
			}


			static void __fastcall RtlpWakeConditionVariable(PCONDITION_VARIABLE ConditionVariable, size_t ConditionVariableStatus, size_t WakeCount)
			{
				auto GlobalKeyedEventHandle = GetGlobalKeyedEventHandle();
				auto pNtReleaseKeyedEvent = try_get_NtReleaseKeyedEvent();

				if (!pNtReleaseKeyedEvent)
				{
					RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
				}


				//v16
				YY_CV_WAIT_BLOCK* notify = nullptr;

				YY_CV_WAIT_BLOCK* pWake = nullptr;
				YY_CV_WAIT_BLOCK** ppInsert = &pWake;

				size_t Count = 0;

				for (;;)
				{
					auto pWaitBlock = YY_CV_GET_BLOCK(ConditionVariableStatus);

					if ((ConditionVariableStatus & 0x7) == 0x7)
					{
						ConditionVariableStatus = InterlockedExchange((volatile size_t*)ConditionVariable, 0);

						*ppInsert = YY_CV_GET_BLOCK(ConditionVariableStatus);

						break;
					}

					const auto MaxWakeCount = WakeCount + (ConditionVariableStatus & 7);

					auto pBlock = pWaitBlock;

					for (; pBlock->notify == nullptr;)
					{
						auto tmp = pBlock;
						pBlock = pBlock->back;
						pBlock->next = tmp;
					}

					if (MaxWakeCount <= Count)
					{
						const auto LastStatus = InterlockedCompareExchange((volatile size_t*)ConditionVariable, size_t(pWaitBlock), ConditionVariableStatus);

						if (LastStatus == ConditionVariableStatus)
						{
							break;
						}

						ConditionVariableStatus = LastStatus;
					}
					else
					{
						notify = pBlock->notify;

						for (; MaxWakeCount > Count && notify->next;)
						{
							++Count;
							*ppInsert = notify;
							notify->back = nullptr;

							auto next = notify->next;

							pWaitBlock->notify = next;
							next->back = nullptr;

							ppInsert = &notify->back;

							notify = next;

						}

						if (MaxWakeCount <= Count)
						{
							const auto LastStatus = InterlockedCompareExchange((volatile size_t*)ConditionVariable, size_t(pWaitBlock), ConditionVariableStatus);

							if (LastStatus == ConditionVariableStatus)
							{
								break;
							}

							ConditionVariableStatus = LastStatus;
						}
						else
						{
							const auto LastStatus = InterlockedCompareExchange((volatile size_t*)ConditionVariable, 0, ConditionVariableStatus);


							if (LastStatus == ConditionVariableStatus)
							{
								*ppInsert = notify;
								notify->back = 0;

								break;
							}

							ConditionVariableStatus = LastStatus;
						}
					}
				}

				for (; pWake;)
				{
					auto back = pWake->back;

					if (!InterlockedBitTestAndReset((volatile LONG*)&pWake->flag, 1))
					{
						if (pWake->SRWLock == nullptr || RtlpQueueWaitBlockToSRWLock(pWake, pWake->SRWLock, (pWake->flag >> 2) & 0x1) == FALSE)
						{
							pNtReleaseKeyedEvent(GlobalKeyedEventHandle, pWake, 0, nullptr);
						}
					}

					pWake = back;
				}

				return;
			}

			static void __fastcall RtlpOptimizeConditionVariableWaitList(PCONDITION_VARIABLE ConditionVariable, size_t ConditionVariableStatus)
			{
				for (;;)
				{
					auto pWaitBlock = YY_CV_GET_BLOCK(ConditionVariableStatus);
					auto pItem = pWaitBlock;

					for (; pItem->notify == nullptr;)
					{
						auto temp = pItem;
						pItem = pItem->back;
						pItem->next = temp;
					}

					pWaitBlock->notify = pItem->notify;

					const auto LastStatus = InterlockedCompareExchange((volatile size_t*)ConditionVariable, size_t(pWaitBlock), ConditionVariableStatus);

					if (LastStatus == ConditionVariableStatus)
						break;

					ConditionVariableStatus = LastStatus;

					if (ConditionVariableStatus & 7)
					{
						RtlpWakeConditionVariable(ConditionVariable, ConditionVariableStatus, 0);
						return;
					}
				}
			}

			static BOOL __fastcall RtlpWakeSingle(PCONDITION_VARIABLE ConditionVariable, YY_CV_WAIT_BLOCK* pBlock)
			{
				auto Current = *(volatile size_t*)ConditionVariable;

				for (; Current && (Current & 0x7) != 0x7;)
				{
					if (Current & 0x8)
					{
						const auto Last = InterlockedCompareExchange((volatile size_t*)ConditionVariable, Current | 0x7, Current);

						if (Last == Current)
							return FALSE;

						Current = Last;
					}
					else
					{
						auto New = Current | 0x8;

						auto Last = InterlockedCompareExchange((volatile size_t*)ConditionVariable, New, Current);

						if (Last == Current)
						{
							Current = New;

							YY_CV_WAIT_BLOCK* notify = nullptr;
							BOOL bRet = FALSE;

							auto pWaitBlock = YY_CV_GET_BLOCK(Current);
							auto pSuccessor = pWaitBlock;

							if (pWaitBlock)
							{
								for (; pWaitBlock;)
								{
									if (pWaitBlock == pBlock)
									{
										if (notify)
										{
											pWaitBlock = pWaitBlock->back;
											bRet = TRUE;

											notify->back = pWaitBlock;

											if (!pWaitBlock)
												break;

											pWaitBlock->next = notify;
										}
										else
										{
											auto back = size_t(pWaitBlock->back);

											New = back == 0 ? back : back ^ ((New ^ back) & 0xF);

											Last = InterlockedCompareExchange((volatile size_t*)ConditionVariable, New, Current);

											if (Last == Current)
											{
												Current = New;
												if (back == 0)
													return TRUE;

												bRet = TRUE;
											}
											else
											{
												Current = Last;
											}

											pSuccessor = pWaitBlock = YY_CV_GET_BLOCK(Current);
											notify = nullptr;
										}
									}
									else
									{
										pWaitBlock->next = notify;
										notify = pWaitBlock;
										pWaitBlock = pWaitBlock->back;
									}
								}

								if (pSuccessor)
									pSuccessor->notify = notify;
							}

							RtlpWakeConditionVariable(ConditionVariable, Current, 0);
							return bRet;
						}

						Current = Last;
					}
				}

				return FALSE;
			}


			static size_t __fastcall RtlpRunOnceWaitForInit(
				size_t Current,
				LPINIT_ONCE lpInitOnce
				)
			{
				auto GlobalKeyedEventHandle = internal::GetGlobalKeyedEventHandle();
				auto pNtWaitForKeyedEvent = try_get_NtWaitForKeyedEvent();
				if (!pNtWaitForKeyedEvent)
					internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);

				do
				{
					const auto Old = Current;

					Current = Current & ~size_t(RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC);

					const auto New = (size_t(&Current) & ~size_t(RTL_RUN_ONCE_ASYNC)) | RTL_RUN_ONCE_CHECK_ONLY;
					const auto Last = InterlockedCompareExchange((volatile size_t*)lpInitOnce, New, Old);
					if (Last == Old)
					{
						//WinXP等老系统不支持空句柄传入，此行为不能照搬Windows 7
						pNtWaitForKeyedEvent(GlobalKeyedEventHandle, &Current, 0, nullptr);

						Current = *(volatile size_t*)lpInitOnce;
					}
					else
					{
						Current = Last;
					}
				} while ((Current & (RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC)) == RTL_RUN_ONCE_CHECK_ONLY);

				return Current;
			}

			static NTSTATUS __fastcall RtlRunOnceBeginInitialize(
				_Inout_ LPINIT_ONCE lpInitOnce,
				_In_ DWORD dwFlags,
				_Outptr_opt_result_maybenull_ LPVOID* lpContext
				)
			{
				//参数检查
				if ((dwFlags & ~(RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC)) || ((dwFlags - 1) & dwFlags))
				{
					return STATUS_INVALID_PARAMETER_2;
				}

				auto Current = *(volatile size_t*)lpInitOnce;

				if ((Current & (RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC)) == RTL_RUN_ONCE_ASYNC)
				{
					//没有想明白这样做的意图，修改lpInitOnce本身有什么意义？ nop？
					InterlockedExchange((volatile size_t *)&lpInitOnce, dwFlags);

					if (lpContext)
						*lpContext = (LPVOID)(Current & ~size_t(RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC));

					return STATUS_SUCCESS;
				}

				if (dwFlags & RTL_RUN_ONCE_CHECK_ONLY)
				{
					return STATUS_UNSUCCESSFUL;
				}

				const auto New = (dwFlags & RTL_RUN_ONCE_ASYNC) | RTL_RUN_ONCE_CHECK_ONLY;



				for (;;)
				{
					const auto InitOnceData = Current & 3;
					if (InitOnceData == 0)
					{
						const auto Last = InterlockedCompareExchange((volatile size_t *)lpInitOnce, New, Current);
						if (Last == Current)
							return STATUS_PENDING;

						Current = Last;
					}
					else if (InitOnceData == RTL_RUN_ONCE_CHECK_ONLY)
					{
						if (dwFlags & RTL_RUN_ONCE_ASYNC)
							return STATUS_INVALID_PARAMETER_2;

						Current = RtlpRunOnceWaitForInit(Current, lpInitOnce);
					}
					else
					{
						//疑惑？为什么微软要这样判断……
						if (Current != (RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC))
						{
							if (lpContext)
								*lpContext = (LPVOID)(Current & ~size_t(RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC));

							return STATUS_SUCCESS;
						}

						return (dwFlags & RTL_RUN_ONCE_ASYNC) ? STATUS_PENDING : STATUS_INVALID_PARAMETER_2;
					}
				}
			}


			static void __fastcall RtlpRunOnceWakeAll(size_t* pWake)
			{
				auto GlobalKeyedEventHandle = internal::GetGlobalKeyedEventHandle();
				auto pNtReleaseKeyedEvent = try_get_NtReleaseKeyedEvent();
				if (!pNtReleaseKeyedEvent)
					internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);

				for (auto WakeAddress = (LPVOID)(*pWake & ~size_t(RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC)); WakeAddress; )
				{
					//防止地址无效，我们先保存下
					auto NextWakeAddress = *(LPVOID*)WakeAddress;

					//WinXP等老系统不支持空句柄传入，此行为不能照搬Windows 7
					pNtReleaseKeyedEvent(GlobalKeyedEventHandle, WakeAddress, 0, nullptr);

					WakeAddress = NextWakeAddress;
				}
			}

			static LSTATUS __fastcall RtlRunOnceComplete(
				_Inout_ LPINIT_ONCE lpInitOnce,
				_In_ DWORD dwFlags,
				_In_opt_ LPVOID lpContext
				)
			{
				//参数检查无效  或者 同时使用了多个标记位
				if ((dwFlags & ~(RTL_RUN_ONCE_ASYNC | RTL_RUN_ONCE_INIT_FAILED)) || ((dwFlags - 1) & dwFlags))
				{
					return STATUS_INVALID_PARAMETER_2;
				}

				/*
				dwFlags =   0, dwNewFlags = 0x3（RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC）
				dwFlags = 0x2, dwNewFlags = 0x2（RTL_RUN_ONCE_ASYNC）
				dwFlags = 0x4, dwNewFlags = 0x5（RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_INIT_FAILED）
				*/
				const auto dwNewFlags = (dwFlags ^ ~(dwFlags >> 1)) & 3 ^ dwFlags;

				if (lpContext && ((dwNewFlags & RTL_RUN_ONCE_ASYNC) == 0 || (size_t(lpContext) & (RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC))))
				{
					return STATUS_INVALID_PARAMETER_3;
				}

				auto Current = *(volatile size_t*)lpInitOnce;
				auto New = (size_t(lpContext) & ~size_t(RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC)) | (dwNewFlags & RTL_RUN_ONCE_ASYNC);

				switch (Current & (RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC))
				{
				case RTL_RUN_ONCE_CHECK_ONLY:
					if ((dwNewFlags & RTL_RUN_ONCE_CHECK_ONLY) == 0)
					{
						return STATUS_INVALID_PARAMETER_2;
					}

					Current = InterlockedExchange((volatile size_t*)lpInitOnce, New);
					if ((Current & (RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC)) == RTL_RUN_ONCE_CHECK_ONLY)
					{
						RtlpRunOnceWakeAll(&Current);

						return STATUS_SUCCESS;
					}

					return STATUS_INVALID_OWNER;
					break;
				case RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC:
					if (dwNewFlags & RTL_RUN_ONCE_CHECK_ONLY)
					{
						return STATUS_INVALID_PARAMETER_2;
					}

					if (InterlockedCompareExchange((volatile size_t*)lpInitOnce, New, Current) == Current)
					{
						return STATUS_SUCCESS;
					}

					return STATUS_OBJECT_NAME_COLLISION;

					break;
				default:
					return STATUS_UNSUCCESSFUL;
					break;
				}
			}

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
			static auto __fastcall GetBlockByWaitOnAddressHashTable(LPVOID Address)
			{
				static volatile ULONG_PTR WaitOnAddressHashTable[128];

				const auto Index = (size_t(Address) >> 5) & 0x7F;

				return &WaitOnAddressHashTable[Index];
			}

			static void __fastcall RtlpWaitOnAddressWakeEntireList(YY_ADDRESS_WAIT_BLOCK* pBlock)
			{
				auto GlobalKeyedEventHandle = GetGlobalKeyedEventHandle();
				auto pNtReleaseKeyedEvent = try_get_NtReleaseKeyedEvent();
				if (!pNtReleaseKeyedEvent)
					internal::RaiseStatus(STATUS_NOT_FOUND);

				for (; pBlock;)
				{
					auto Tmp = pBlock->back;

					if (InterlockedExchange(&pBlock->flag, 2) == 0)
					{
						pNtReleaseKeyedEvent(GlobalKeyedEventHandle, pBlock, 0, nullptr);
					}


					pBlock = Tmp;
				}
			}

			static void __fastcall RtlpOptimizeWaitOnAddressWaitList(volatile ULONG_PTR* ppFirstBlock)
			{
				auto Current = *ppFirstBlock;

				for (;;)
				{
					auto pBlock = YY_ADDRESS_GET_BLOCK(Current);

					for (auto pItem = pBlock;;)
					{
						if (pItem->next != nullptr)
						{
							pBlock->next = pItem->next;
							break;
						}

						auto Tmp = pItem;
						pItem = pItem->back;
						pItem->notify = Tmp;
					}

					const auto Last = InterlockedCompareExchange(ppFirstBlock, (Current & 1) == 0 ? size_t(pBlock) : 0, Current);

					if (Last == Current)
					{
						if(Current & 1)
						{
							RtlpWaitOnAddressWakeEntireList(pBlock);
						}

						return;
					}


					Current = Last;
				}
			}


			static void __fastcall RtlpAddWaitBlockToWaitList(YY_ADDRESS_WAIT_BLOCK* pWaitBlock)
			{
				auto ppFirstBlock = GetBlockByWaitOnAddressHashTable((LPVOID)pWaitBlock->Address);

				auto Current = *ppFirstBlock;

				for (;;)
				{
					auto New = size_t(pWaitBlock) | (size_t(Current) & 0x3);

					auto back = YY_ADDRESS_GET_BLOCK(Current);
					pWaitBlock->back = back;
					if (back)
					{
						New |= 0x2;

						pWaitBlock->next = nullptr;
					}
					else
					{
						pWaitBlock->next = pWaitBlock;
					}

					const auto Last = InterlockedCompareExchange(ppFirstBlock, New, Current);

					if (Last == Current)
					{
						//0x2状态发生变化 才需要重新优化锁。
						if ((Current ^ New) & 0x2)
						{
							RtlpOptimizeWaitOnAddressWaitList(ppFirstBlock);
						}

						return;
					}

					Current = Last;
				}
			}

			static NTSTATUS __fastcall RtlpWaitOnAddressWithTimeout(YY_ADDRESS_WAIT_BLOCK* pWaitBlock, LARGE_INTEGER *TimeOut);

			static void __fastcall RtlpWaitOnAddressRemoveWaitBlock(YY_ADDRESS_WAIT_BLOCK* pWaitBlock)
			{
				auto GlobalKeyedEventHandle = GetGlobalKeyedEventHandle();
				auto pNtWaitForKeyedEvent = try_get_NtWaitForKeyedEvent();
				if (!pNtWaitForKeyedEvent)
					internal::RaiseStatus(STATUS_NOT_FOUND);

				auto ppFirstBlock = GetBlockByWaitOnAddressHashTable((LPVOID)pWaitBlock->Address);

				auto Current = *ppFirstBlock;
				size_t Last;

				for (; Current; Current = Last)
				{
					if (Current & 2)
					{
						Last = InterlockedCompareExchange(ppFirstBlock, Current | 1, Current);

						if (Last == Current)
						{
							break;
						}
					}
					else
					{
						auto New = Current | 0x2;
						Last = InterlockedCompareExchange(ppFirstBlock, New, Current);

						if (Last == Current)
						{
							Current = New;

							bool bFind = false;

							//同步成功！
							auto pBlock = YY_ADDRESS_GET_BLOCK(New);
							auto pItem = pBlock;

							auto pNotify = pBlock->notify;
							YY_ADDRESS_WAIT_BLOCK* Tmp;

							do
							{
								Tmp = pBlock->back;

								if (pBlock != pWaitBlock)
								{
									pBlock->notify = pNotify;
									pNotify = pBlock;


									pBlock = Tmp;
									Tmp = pItem;
									continue;
								}

								bFind = true;


								if (pBlock != pItem)
								{
									pNotify->back = Tmp;
									if (Tmp)
										Tmp->notify = pNotify;
									else
										pNotify->next = pNotify;

									pBlock = Tmp;
									Tmp = pItem;
									continue;
								}

								New = size_t(pBlock->back);
								if (Tmp)
								{
									New = size_t(Tmp) ^ (Current ^ size_t(Tmp)) & 0x3;
								}

								Last = InterlockedCompareExchange(ppFirstBlock, New, Current);

								if (Last == Current)
								{
									if (New == 0)
										return;

									Tmp->notify = nullptr;
									pBlock = Tmp;
								}
								else
								{
									Current = Last;

									Tmp = pBlock = YY_ADDRESS_GET_BLOCK(Current);
									pNotify = pBlock->notify;
								}


								pItem = Tmp;
							} while (pBlock);
							

							if (bFind == false && InterlockedExchange(&pWaitBlock->flag, 0) != 2)
							{
								pNtWaitForKeyedEvent(GlobalKeyedEventHandle, pWaitBlock, 0, nullptr);
							}

							Tmp->next = pNotify;

							for (;;)
							{
								const auto Last = InterlockedCompareExchange(ppFirstBlock, (Current & 1) == 0 ? size_t(YY_ADDRESS_GET_BLOCK(Current)) : 0, Current);

								if (Last == Current)
									break;

								Current = Last;
							}

							if (Current & 1)
								RtlpWaitOnAddressWakeEntireList(YY_ADDRESS_GET_BLOCK(Current));


							return;
						}
					}
				}

				if (InterlockedExchange(&pWaitBlock->flag, 1) == 2)
					return;

				RtlpWaitOnAddressWithTimeout(pWaitBlock, 0);
			}

			static NTSTATUS __fastcall RtlpWaitOnAddressWithTimeout(YY_ADDRESS_WAIT_BLOCK* pWaitBlock, LARGE_INTEGER *TimeOut)
			{
				//单核 我们无需自旋，直接进入等待过程即可
				if (NtCurrentTeb()->ProcessEnvironmentBlock->NumberOfProcessors > 1 && RtlpWaitOnAddressSpinCount)
				{
					for (DWORD SpinCount = 0; SpinCount < RtlpWaitOnAddressSpinCount;++SpinCount)
					{
						if ((pWaitBlock->flag & 1) == 0)
						{
							//自旋过程中，等到了信号改变
							return STATUS_SUCCESS;
						}

						YieldProcessor();
					}
				}

				if (!_interlockedbittestandreset((volatile long *)&pWaitBlock->flag, 0))
				{
					//本来我是拒绝的，但是运气好，状态已经发生了反转
					return STATUS_SUCCESS;
				}

				auto GlobalKeyedEventHandle = GetGlobalKeyedEventHandle();
				auto pNtWaitForKeyedEvent = try_get_NtWaitForKeyedEvent();
				if (!pNtWaitForKeyedEvent)
					internal::RaiseStatus(STATUS_NOT_FOUND);

				auto Status = pNtWaitForKeyedEvent(GlobalKeyedEventHandle, pWaitBlock, 0, TimeOut);

				if (Status == STATUS_TIMEOUT)
				{
					if (InterlockedExchange(&pWaitBlock->flag, 4) == 2)
					{
						Status = pNtWaitForKeyedEvent(GlobalKeyedEventHandle, pWaitBlock, 0, nullptr);
					}
					else
					{
						RtlpWaitOnAddressRemoveWaitBlock(pWaitBlock);
					}
				}

				return Status;
			}

#if defined(_X86_)
			#define RtlpWakeByAddress(Address, bWakeAll) YY_RtlpWakeByAddress(0, bWakeAll, Address)
			static void __fastcall YY_RtlpWakeByAddress(DWORD dwReserved/*用于平衡栈需要，利于编译器优化成jmp*/, BOOL bWakeAll, LPVOID Address)
#else
			static void __fastcall RtlpWakeByAddress(LPVOID Address, BOOL bWakeAll)
#endif
			{
				auto ppFirstBlock = GetBlockByWaitOnAddressHashTable(Address);
				YY_ADDRESS_WAIT_BLOCK* LastWake = nullptr;

				auto Current = *ppFirstBlock;
				size_t Last;
				bool bNoRemove = false;

				for (; Current && (Current & 1) ==0; Current = Last)
				{
					if (Current & 2)
					{
						Last = InterlockedCompareExchange(ppFirstBlock, Current | 1, Current);

						if (Last == Current)
						{
							return;
						}
					}
					else
					{
						auto New = Current | 0x2;
						Last = InterlockedCompareExchange(ppFirstBlock, New, Current);

						if (Last == Current)
						{
							Current = New;

						__retry:


							auto pBlock = YY_ADDRESS_GET_BLOCK(Current);
							auto pItem = pBlock;

							for (; pItem->next == nullptr;)
							{
								auto Tmp = pItem;
								pItem = pItem->back;
								pItem->notify = Tmp;
							}

							pItem = pItem->next;
							pBlock->next = pItem;

							for (; pItem;)
							{
								auto notify = pItem->notify;

								if (pItem->Address == Address)
								{
									auto back = pItem->back;
									if (pItem == pBlock)
									{
										New = size_t(back);
										if (New)
										{
											New = (Current ^ New) & 3 ^ New;
										}

										Last = InterlockedCompareExchange(ppFirstBlock, New, Current);
										

										if (Last != Current)
										{
											//换个姿势，再来一次
											Current = Last;
											goto __retry;
										}

										pBlock = YY_ADDRESS_GET_BLOCK(Last);

										bNoRemove = New == 0;
										if (back)
										{
											back->notify = nullptr;
											back->next = pItem->next;
										}
									}
									else
									{
										notify->back = back;
										back = pItem->back;
										auto notify = pItem->notify;

										if (back)
										{
											back->notify = notify;
										}
										else
										{
											pBlock->next = notify;
											notify->next = notify;
										}
									}

									const auto LastFlag = InterlockedExchange(&pItem->flag, 2);

									if (LastFlag != 2)
									{
										if (LastFlag == 0)
										{
											pItem->back = LastWake;
											LastWake = pItem;
										}

										if (!bWakeAll)
											break;
									}
								}

								pItem = notify;
							}

							auto GlobalKeyedEventHandle = GetGlobalKeyedEventHandle();
							auto pNtReleaseKeyedEvent = try_get_NtReleaseKeyedEvent();
							if (!pNtReleaseKeyedEvent)
								internal::RaiseStatus(STATUS_NOT_FOUND);

							for (auto pItem = LastWake; pItem;)
							{
								//NtReleaseKeyedEvent 调用后 pItem 内存块可能无效，因此必须先保存一份。
								auto Tmp = pItem->back;

								//唤醒等待的Key
								pNtReleaseKeyedEvent(GlobalKeyedEventHandle, pItem, FALSE, nullptr);

								pItem = Tmp;
							}

							if (!bNoRemove)
							{
								//更新标头块


								for (auto Current = *ppFirstBlock;;)
								{
									const auto Last = InterlockedCompareExchange(ppFirstBlock, (Current & 1) == 0 ? size_t(YY_ADDRESS_GET_BLOCK(Current)) : 0, Current);

									if (Last == Current)
									{
										if(Current & 1)
											RtlpWaitOnAddressWakeEntireList(YY_ADDRESS_GET_BLOCK(Current));

										break;
									}

									Current = Last;
								}
							}

							return;
						}
					}
				}

			}
#endif
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista, Windows Server 2008
		__DEFINE_THUNK(
		kernel32,
		12,
		BOOL,
		WINAPI,
		InitializeCriticalSectionEx,
			_Out_ LPCRITICAL_SECTION lpCriticalSection,
			_In_ DWORD dwSpinCount,
			_In_ DWORD Flags
			)
		{
			if (auto const pInitializeCriticalSectionEx = try_get_InitializeCriticalSectionEx())
			{
				return pInitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags);
			}

			return InitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		InitOnceInitialize,
			_Out_ PINIT_ONCE InitOnce
			)
		{
			*InitOnce = INIT_ONCE_STATIC_INIT;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		16,
		_Success_(return)
		BOOL,
		WINAPI,
		InitOnceBeginInitialize,
			_Inout_ LPINIT_ONCE lpInitOnce,
			_In_ DWORD dwFlags,
			_Out_ PBOOL fPending,
			_Outptr_opt_result_maybenull_ LPVOID* lpContext
			)
		{
			if (auto const pInitOnceBeginInitialize = try_get_InitOnceBeginInitialize())
			{
				return pInitOnceBeginInitialize(lpInitOnce, dwFlags, fPending, lpContext);
			}

			auto Status = internal::RtlRunOnceBeginInitialize(lpInitOnce, dwFlags, lpContext);

			if (Status >= STATUS_SUCCESS)
			{
				*fPending = Status == STATUS_PENDING;
				return TRUE;
			}
			else
			{
				internal::BaseSetLastNTError(Status);
				return FALSE;
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		12,
		BOOL,
		WINAPI,
		InitOnceComplete,
			_Inout_ LPINIT_ONCE lpInitOnce,
			_In_ DWORD dwFlags,
			_In_opt_ LPVOID lpContext
			)
		{
			if (auto const pInitOnceComplete = try_get_InitOnceComplete())
			{
				return pInitOnceComplete(lpInitOnce, dwFlags, lpContext);
			}
	
			auto Status = internal::RtlRunOnceComplete(lpInitOnce, dwFlags, lpContext);

			if (Status >= 0)
			{
				return TRUE;
			}
			else
			{
				internal::BaseSetLastNTError(Status);
				return FALSE;
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		16,
		BOOL,
		WINAPI,
		InitOnceExecuteOnce,
			_Inout_ PINIT_ONCE InitOnce,
			_In_ __callback PINIT_ONCE_FN InitFn,
			_Inout_opt_ PVOID Parameter,
			_Outptr_opt_result_maybenull_ LPVOID* Context
			)
		{
			if (auto const pInitOnceExecuteOnce = try_get_InitOnceExecuteOnce())
			{
				return pInitOnceExecuteOnce(InitOnce, InitFn, Parameter, Context);
			}

			auto Status = internal::RtlRunOnceBeginInitialize(InitOnce, 0, Context);

			ULONG_PTR ExceptionArgument;

			do
			{
				if (Status < STATUS_SUCCESS)
				{
					ExceptionArgument = 0;
				}
				else if (Status == STATUS_PENDING)
				{
					if (InitFn(InitOnce, Parameter, Context))
					{
						auto NewContext = Context;
						if (NewContext)
							NewContext = (LPVOID*)*NewContext;

						Status = internal::RtlRunOnceComplete(InitOnce, 0, NewContext);

						if (Status >= STATUS_SUCCESS)
							break;

						ExceptionArgument = 1;
					}
					else
					{
						Status = internal::RtlRunOnceComplete(InitOnce, RTL_RUN_ONCE_INIT_FAILED, nullptr);
						if (Status >= STATUS_SUCCESS)
						{
							Status = STATUS_UNSUCCESSFUL;
							break;
						}
						ExceptionArgument = 2;
					}
				}
				else
				{
					break;
				}

				RaiseException(Status, EXCEPTION_NONCONTINUABLE, 1, &ExceptionArgument);

			} while (false);

			//别问我为什么不设置LastError……，Windows 7的这个函数也没设置 LastError，既然这样，我们都偷懒下吧，行为跟微软保持一致。
			return Status >= STATUS_SUCCESS;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		16,
		HANDLE,
		WINAPI,
		CreateEventExW,
			_In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
			_In_opt_ LPCWSTR lpName,
			_In_ DWORD dwFlags,
			_In_ DWORD dwDesiredAccess
			)
		{
			if (auto pCreateEventExW = try_get_CreateEventExW())
			{
				return pCreateEventExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
			}

			return CreateEventW(lpEventAttributes, dwFlags & CREATE_EVENT_MANUAL_RESET, dwFlags & CREATE_EVENT_INITIAL_SET, lpName);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		16,
		HANDLE,
		WINAPI,
		CreateEventExA,
			_In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
			_In_opt_ LPCSTR lpName,
			_In_ DWORD dwFlags,
			_In_ DWORD dwDesiredAccess
			)
		{
			if (auto pCreateEventExA = try_get_CreateEventExA())
			{
				return pCreateEventExA(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
			}

			return CreateEventA(lpEventAttributes, dwFlags & CREATE_EVENT_MANUAL_RESET, dwFlags & CREATE_EVENT_INITIAL_SET, lpName);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		16,
		HANDLE,
		WINAPI,
		CreateMutexExW,
			_In_opt_ LPSECURITY_ATTRIBUTES lpMutexAttributes,
			_In_opt_ LPCWSTR lpName,
			_In_ DWORD dwFlags,
			_In_ DWORD dwDesiredAccess
			)
		{
			if (auto pCreateMutexExW = try_get_CreateMutexExW())
			{
				return pCreateMutexExW(lpMutexAttributes, lpName, dwFlags, dwDesiredAccess);
			}

			return CreateMutexW(lpMutexAttributes, dwFlags & CREATE_MUTEX_INITIAL_OWNER, lpName);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		16,
		HANDLE,
		WINAPI,
		CreateMutexExA,
			_In_opt_ LPSECURITY_ATTRIBUTES lpMutexAttributes,
			_In_opt_ LPCSTR lpName,
			_In_ DWORD dwFlags,
			_In_ DWORD dwDesiredAccess
			)
		{
			if (auto pCreateMutexExA = try_get_CreateMutexExA())
			{
				return pCreateMutexExA(lpMutexAttributes, lpName, dwFlags, dwDesiredAccess);
			}

			return CreateMutexA(lpMutexAttributes, dwFlags & CREATE_MUTEX_INITIAL_OWNER, lpName);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		24,
		HANDLE,
		WINAPI,
		CreateSemaphoreExW,
			_In_opt_ LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
			_In_ LONG lInitialCount,
			_In_ LONG lMaximumCount,
			_In_opt_ LPCWSTR lpName,
			_Reserved_ DWORD dwFlags,
			_In_ DWORD dwDesiredAccess
			)
		{
			if (auto pCreateSemaphoreExW = try_get_CreateSemaphoreExW())
			{
				return pCreateSemaphoreExW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess);
			}

			return CreateSemaphoreW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		16,
		HANDLE,
		WINAPI,
		CreateWaitableTimerExW,
			_In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
			_In_opt_ LPCWSTR lpTimerName,
			_In_ DWORD dwFlags,
			_In_ DWORD dwDesiredAccess
			)
		{
			if (auto pCreateWaitableTimerExW = try_get_CreateWaitableTimerExW())
			{
				return pCreateWaitableTimerExW(lpTimerAttributes, lpTimerName, dwFlags, dwDesiredAccess);
			}

			return CreateWaitableTimerW(lpTimerAttributes, dwFlags&CREATE_WAITABLE_TIMER_MANUAL_RESET, lpTimerName);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		InitializeSRWLock,
			_Out_ PSRWLOCK SRWLock
			)
		{
			*SRWLock = RTL_SRWLOCK_INIT;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		AcquireSRWLockExclusive,
			_Inout_ PSRWLOCK SRWLock
			)
		{
			if (auto const pAcquireSRWLockExclusive = try_get_AcquireSRWLockExclusive())
			{
				return pAcquireSRWLockExclusive(SRWLock);
			}

			YY_SRWLOCK_WAIT_BLOCK StackWaitBlock;
			bool bOptimize;

			//尝试加锁一次
#if defined(_WIN64)
			auto OldBit = InterlockedBitTestAndSet64((volatile LONG_PTR*)SRWLock, YY_SRWLOCK_Locked_BIT);
#else
			auto OldBit = InterlockedBitTestAndSet((volatile LONG_PTR*)SRWLock, YY_SRWLOCK_Locked_BIT);
#endif

			if(OldBit == false)
			{
				//成功锁定
				return;
			}

			for (;;)
			{
				auto SRWLockOld =  *(volatile size_t*)SRWLock;

				if (YY_SRWLOCK_Locked & SRWLockOld)
				{
					/*
					if (RtlpWaitCouldDeadlock())
						ZwTerminateProcess((HANDLE)0xFFFFFFFF, 0xC000004B);
					*/

					StackWaitBlock.next = nullptr;
					StackWaitBlock.flag = 3;
					bOptimize = false;

					size_t SRWLockNew;

					if (YY_SRWLOCK_Waiting & SRWLockOld)
					{
						//有人正在等待连接
						StackWaitBlock.notify = nullptr;
						StackWaitBlock.shareCount = 0;
						StackWaitBlock.back = (YY_SRWLOCK_WAIT_BLOCK*)(SRWLockOld & (~YY_SRWLOCK_MASK));

						SRWLockNew = (size_t)(&StackWaitBlock) | (SRWLockOld & YY_SRWLOCK_MultipleShared) | YY_SRWLOCK_Waking | YY_SRWLOCK_Waiting | YY_SRWLOCK_Locked;

						if ((YY_SRWLOCK_Waking & SRWLockOld) == 0)
						{
							bOptimize = true;
						}
					}
					else
					{
						//没有其他人没有等待，所以我们需要创建一个
						StackWaitBlock.notify = (YY_SRWLOCK_WAIT_BLOCK*)&StackWaitBlock;
						StackWaitBlock.shareCount = (SRWLockOld >> YY_SRWLOCK_BITS);


						SRWLockNew = StackWaitBlock.shareCount > 1 ?
							(size_t)(&StackWaitBlock) | YY_SRWLOCK_MultipleShared | YY_SRWLOCK_Waiting | YY_SRWLOCK_Locked
							: (size_t)(&StackWaitBlock) | YY_SRWLOCK_Waiting | YY_SRWLOCK_Locked;
					}

					if (InterlockedCompareExchange((volatile size_t*)SRWLock, SRWLockNew, SRWLockOld) != SRWLockOld)
					{
						//更新锁状态失败，其他线程正在处理改锁，要不咋们换个姿势再来
				
						//RtlBackoff就懒得做了，反正只是等待一会而已，直接YieldProcessor再来一次吧。
						//RtlBackoff(&nBackOff)

						YieldProcessor();
						continue;
					}


					if (bOptimize)
					{
						internal::RtlpOptimizeSRWLockList(SRWLock, SRWLockNew);
					}

					auto GlobalKeyedEventHandle = internal::GetGlobalKeyedEventHandle();
					auto pNtWaitForKeyedEvent = try_get_NtWaitForKeyedEvent();
					if (!pNtWaitForKeyedEvent)
					{
						internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
					}

					//自旋
					for (DWORD SpinCount = SRWLockSpinCount; SpinCount; --SpinCount)
					{
						if ((StackWaitBlock.flag & 2) == 0)
							break;

						YieldProcessor();
					}

					if (InterlockedBitTestAndReset((volatile LONG*)&StackWaitBlock.flag, 1))
					{
						pNtWaitForKeyedEvent(GlobalKeyedEventHandle, (PVOID)&StackWaitBlock, 0, nullptr);
					}
				}
				else
				{
					//尝试获取锁的所有权
					if (InterlockedCompareExchange((volatile size_t*)SRWLock, SRWLockOld | YY_SRWLOCK_Locked, SRWLockOld) == SRWLockOld)
					{
						//成功加锁
						return;
					}

					//可能多线程并发访问，换个姿势再来一次
					YieldProcessor();
				}
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		//Windows 7 [desktop apps | UWP apps]
		//Windows Server 2008 R2 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		BOOLEAN,
		WINAPI,
		TryAcquireSRWLockExclusive,
			_Inout_ PSRWLOCK SRWLock
			)
		{
			if (auto const pTryAcquireSRWLockExclusive = try_get_TryAcquireSRWLockExclusive())
			{
				return pTryAcquireSRWLockExclusive(SRWLock);
			}

#if defined(_WIN64)
			return InterlockedBitTestAndSet64((volatile LONG_PTR*)SRWLock, YY_SRWLOCK_Locked_BIT) == 0;
#else
			return InterlockedBitTestAndSet((volatile LONG_PTR*)SRWLock, YY_SRWLOCK_Locked_BIT) == 0;
#endif
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		ReleaseSRWLockExclusive,
			_Inout_ PSRWLOCK SRWLock
			)
		{
			if (auto const pReleaseSRWLockExclusive = try_get_ReleaseSRWLockExclusive())
			{
				return pReleaseSRWLockExclusive(SRWLock);
			}

			auto OldSRWLock = InterlockedExchangeAdd((volatile size_t *)SRWLock, size_t(-1));
			if ((OldSRWLock & YY_SRWLOCK_Locked) == 0)
			{
				internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
			}

			if ((OldSRWLock & YY_SRWLOCK_Waiting) && (OldSRWLock & YY_SRWLOCK_Waking) == 0)
			{
				OldSRWLock -= YY_SRWLOCK_Locked;

				auto NewSRWLock = OldSRWLock | YY_SRWLOCK_Waking;
				auto CurrentSRWLock = InterlockedCompareExchange((volatile size_t *)SRWLock, NewSRWLock, OldSRWLock);

				if (CurrentSRWLock == OldSRWLock)
					internal::RtlpWakeSRWLock(SRWLock, NewSRWLock);
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		AcquireSRWLockShared,
			_Inout_ PSRWLOCK SRWLock
			)
		{
			if (auto const pAcquireSRWLockShared = try_get_AcquireSRWLockShared())
			{
				return pAcquireSRWLockShared(SRWLock);
			}

			YY_SRWLOCK_WAIT_BLOCK StackWaitBlock;
			bool bOptimize;

			//尝试给全新的锁加锁	
			auto OldSRWLock = InterlockedCompareExchange((volatile size_t*)SRWLock, size_t(0x11), 0);

			//成功
			if (OldSRWLock == size_t(0))
			{
				return;
			}


			size_t NewSRWLock;

			for (;; OldSRWLock = *(volatile size_t *)SRWLock)
			{
				if ((OldSRWLock & YY_SRWLOCK_Locked) && ((OldSRWLock & YY_SRWLOCK_Waiting) || YY_SRWLOCK_GET_BLOCK(OldSRWLock) == nullptr))
				{
					//if ( RtlpWaitCouldDeadlock() )
					//    ZwTerminateProcess((HANDLE)0xFFFFFFFF, 0xC000004B);


					StackWaitBlock.flag = 2;
					StackWaitBlock.shareCount = 0;
					StackWaitBlock.next = nullptr;

					bOptimize = false;

					if (OldSRWLock & YY_SRWLOCK_Waiting)
					{
						//已经有人等待，我们插入一个新的等待块
						StackWaitBlock.back = YY_SRWLOCK_GET_BLOCK(OldSRWLock);
						StackWaitBlock.notify = nullptr;

						NewSRWLock = size_t(&StackWaitBlock) | (OldSRWLock & YY_SRWLOCK_MultipleShared) | (YY_SRWLOCK_Waking | YY_SRWLOCK_Waiting | YY_SRWLOCK_Locked);

						if ((OldSRWLock & YY_SRWLOCK_Waking) == 0)
						{
							bOptimize = true;
						}
					}
					else
					{
						StackWaitBlock.notify = &StackWaitBlock;
						NewSRWLock = size_t(&StackWaitBlock) | (YY_SRWLOCK_Waiting | YY_SRWLOCK_Locked);
					}


					if (InterlockedCompareExchange((volatile size_t *)SRWLock, NewSRWLock, OldSRWLock) == OldSRWLock)
					{
						//更新成功！

						if (bOptimize)
						{
							internal::RtlpOptimizeSRWLockList(SRWLock, NewSRWLock);
						}

						auto GlobalKeyedEventHandle = internal::GetGlobalKeyedEventHandle();
						auto pNtWaitForKeyedEvent = try_get_NtWaitForKeyedEvent();
						if (!pNtWaitForKeyedEvent)
						{
							internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
						}

						//自旋
						for (DWORD SpinCount = SRWLockSpinCount; SpinCount; --SpinCount)
						{
							if ((StackWaitBlock.flag & 2) == 0)
								break;

							YieldProcessor();
						}

						if (InterlockedBitTestAndReset((volatile LONG*)&StackWaitBlock.flag, 1))
						{
							pNtWaitForKeyedEvent(GlobalKeyedEventHandle, (PVOID)&StackWaitBlock, 0, nullptr);
						}

						continue;
					}
				}
				else
				{
					if (OldSRWLock & YY_SRWLOCK_Waiting)
					{
						//既然有人在等待锁，那么YY_SRWLOCK_Locked应该重新加上
						NewSRWLock = OldSRWLock | YY_SRWLOCK_Locked;
					}
					else
					{
						//没有人等待，那么单纯加个 0x10即可
						NewSRWLock = (OldSRWLock + 0x10) | YY_SRWLOCK_Locked;
					}

					if (InterlockedCompareExchange((volatile size_t *)SRWLock, NewSRWLock, OldSRWLock) == OldSRWLock)
						return;
				}

				//偷懒下，直接 YieldProcessor 吧
				//RtlBackoff(&nBackOff);
				YieldProcessor();
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		//Windows 7 [desktop apps | UWP apps] 
		//Windows Server 2008 R2 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		BOOLEAN,
		WINAPI,
		TryAcquireSRWLockShared,
			_Inout_ PSRWLOCK SRWLock
			)
		{
			if (auto const pTryAcquireSRWLockShared = try_get_TryAcquireSRWLockShared())
			{
				return pTryAcquireSRWLockShared(SRWLock);
			}

			//尝试给全新的锁加锁
			auto OldSRWLock = InterlockedCompareExchange((volatile size_t*)SRWLock, size_t(0x11), 0);

			//成功
			if (OldSRWLock == size_t(0))
			{
				return TRUE;
			}

			for (;;)
			{
				if ((OldSRWLock & YY_SRWLOCK_Locked) && ((OldSRWLock & YY_SRWLOCK_Waiting) || YY_SRWLOCK_GET_BLOCK(OldSRWLock) == nullptr))
				{
					//正在被锁定中
					return FALSE;
				}
				else
				{
					size_t NewSRWLock;

					if (OldSRWLock & YY_SRWLOCK_Waiting)
						NewSRWLock = OldSRWLock | YY_SRWLOCK_Locked;
					else
						NewSRWLock = (OldSRWLock + 0x10) | YY_SRWLOCK_Locked;

					if (InterlockedCompareExchange((volatile size_t*)SRWLock, NewSRWLock, OldSRWLock) == OldSRWLock)
					{
						//锁定完成
						return TRUE;
					}

					//RtlBackoff((unsigned int *)&v4);
					YieldProcessor();
					OldSRWLock = *(volatile size_t*)SRWLock;
				}
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		ReleaseSRWLockShared,
			_Inout_ PSRWLOCK SRWLock
			)
		{
			if (auto const pReleaseSRWLockShared = try_get_ReleaseSRWLockShared())
			{
				return pReleaseSRWLockShared(SRWLock);
			}

			//尝试解锁只加一次读锁的情况

			auto OldSRWLock = InterlockedCompareExchange((volatile size_t*)SRWLock, 0, size_t(0x11));

			//解锁成功
			if (OldSRWLock == size_t(0x11))
			{
				return;
			}

			if ((OldSRWLock & YY_SRWLOCK_Locked) == 0)
			{
				internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
			}

			for (;;)
			{
				if (OldSRWLock & YY_SRWLOCK_Waiting)
				{
					if (OldSRWLock & YY_SRWLOCK_MultipleShared)
					{
						auto pLastNode = YY_SRWLOCK_GET_BLOCK(OldSRWLock);

						for (; pLastNode->notify == nullptr; pLastNode = pLastNode->back);

						/* 
						既然是在释放共享锁，说明一定有人获取了共享锁
						如果有人获取了共享锁，就一定没有人获取独到占锁
						只需要把共享次数减1
						取出notify节点的共享次数变量的地址, 原子减
						*/
						if (InterlockedDecrement((volatile size_t *)&(pLastNode->notify->shareCount)) > 0)
						{
							return;
						}
					}


					for (;;)
					{
						auto NewSRWLock = OldSRWLock & (~(YY_SRWLOCK_MultipleShared | YY_SRWLOCK_Locked));
						size_t LastSRWLock;

						if (OldSRWLock & YY_SRWLOCK_Waking)
						{
							LastSRWLock = InterlockedCompareExchange((volatile size_t *)SRWLock, NewSRWLock, OldSRWLock);

							if (LastSRWLock == OldSRWLock)
								return;
						}
						else
						{
							NewSRWLock |= YY_SRWLOCK_Waking;

							LastSRWLock = InterlockedCompareExchange((volatile size_t *)SRWLock, NewSRWLock, OldSRWLock);
							if (LastSRWLock == OldSRWLock)
								return internal::RtlpWakeSRWLock(SRWLock, NewSRWLock);
						}

						OldSRWLock = LastSRWLock;
					}

					break;
				}
				else
				{
					auto NewSRWLock = size_t(YY_SRWLOCK_GET_BLOCK(OldSRWLock)) <= 0x10 ? 0 : OldSRWLock - 0x10;

					auto LastSRWLock = InterlockedCompareExchange((volatile size_t *)SRWLock, NewSRWLock, OldSRWLock);
					if (LastSRWLock == OldSRWLock)
						break;

					OldSRWLock = LastSRWLock;
				}
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		InitializeConditionVariable,
			_Out_ PCONDITION_VARIABLE ConditionVariable
			)
		{
			*ConditionVariable = CONDITION_VARIABLE_INIT;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		12,
		BOOL,
		WINAPI,
		SleepConditionVariableCS,
			_Inout_ PCONDITION_VARIABLE ConditionVariable,
			_Inout_ PCRITICAL_SECTION   CriticalSection,
			_In_    DWORD               dwMilliseconds
			)
		{
			if (auto const pSleepConditionVariableCS = try_get_SleepConditionVariableCS())
			{
				return pSleepConditionVariableCS(ConditionVariable, CriticalSection, dwMilliseconds);
			}

			YY_CV_WAIT_BLOCK StackWaitBlock;

			StackWaitBlock.next = nullptr;
			StackWaitBlock.flag = 2;
			StackWaitBlock.SRWLock = nullptr;

			auto OldConditionVariable = *(size_t*)ConditionVariable;

			size_t NewConditionVariable;

			for (;;)
			{
				NewConditionVariable = size_t(&StackWaitBlock) | (OldConditionVariable & YY_CV_MASK);
				StackWaitBlock.back = YY_CV_GET_BLOCK(OldConditionVariable);

				if (StackWaitBlock.back)
				{
					StackWaitBlock.notify = nullptr;

					NewConditionVariable |= 0x8;
				}
				else
				{
					StackWaitBlock.notify = &StackWaitBlock;
				}

				auto LastConditionVariable = InterlockedCompareExchange((volatile size_t*)ConditionVariable, NewConditionVariable, OldConditionVariable);

				if (LastConditionVariable == OldConditionVariable)
					break;

				OldConditionVariable = LastConditionVariable;
			}

			LeaveCriticalSection(CriticalSection);

			//0x8 标记新增时，才进行优化 ConditionVariableWaitList
			if ((OldConditionVariable ^ NewConditionVariable) & 0x8)
			{
				internal::RtlpOptimizeConditionVariableWaitList(ConditionVariable, NewConditionVariable);
			}


			auto GlobalKeyedEventHandle = internal::GetGlobalKeyedEventHandle();
			auto pNtWaitForKeyedEvent = try_get_NtWaitForKeyedEvent();
			if (!pNtWaitForKeyedEvent)
			{
				internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
			}

			//自旋
			for (auto SpinCount = ConditionVariableSpinCount; SpinCount; --SpinCount)
			{
				if (!(StackWaitBlock.flag & 2))
					break;

				YieldProcessor();
			}

			NTSTATUS Status = 0;

			if (InterlockedBitTestAndReset((volatile LONG*)&StackWaitBlock.flag, 1))
			{
				LARGE_INTEGER TimeOut;

				Status = pNtWaitForKeyedEvent(GlobalKeyedEventHandle, (PVOID)&StackWaitBlock, 0, internal::BaseFormatTimeOut(&TimeOut, dwMilliseconds));

				if (Status == STATUS_TIMEOUT && internal::RtlpWakeSingle(ConditionVariable, &StackWaitBlock) == FALSE)
				{
					pNtWaitForKeyedEvent(GlobalKeyedEventHandle, (PVOID)&StackWaitBlock, 0, nullptr);
					Status = 0;
				}
			}

			EnterCriticalSection(CriticalSection);

			internal::BaseSetLastNTError(Status);

			return Status >= 0 && Status != STATUS_TIMEOUT;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		16,
		BOOL,
		WINAPI,
		SleepConditionVariableSRW,
			_Inout_ PCONDITION_VARIABLE ConditionVariable,
			_Inout_ PSRWLOCK SRWLock,
			_In_ DWORD dwMilliseconds,
			_In_ ULONG Flags
			)
		{
			if (auto const pSleepConditionVariableSRW = try_get_SleepConditionVariableSRW())
			{
				return pSleepConditionVariableSRW(ConditionVariable, SRWLock, dwMilliseconds, Flags);
			}

			YY_CV_WAIT_BLOCK StackWaitBlock;

			NTSTATUS Status = 0;

			do
			{
				if (Flags & ~CONDITION_VARIABLE_LOCKMODE_SHARED)
				{
					Status = STATUS_INVALID_PARAMETER_2;
					break;
				}

				StackWaitBlock.next = nullptr;
				StackWaitBlock.flag = 2;
				StackWaitBlock.SRWLock = nullptr;

				if (Flags& CONDITION_VARIABLE_LOCKMODE_SHARED)
				{
					StackWaitBlock.flag |= 0x4;
				}

				size_t Current = *(volatile size_t*)ConditionVariable;
				size_t New;

				for (;;)
				{
					New = size_t(&StackWaitBlock) | (Current & YY_CV_MASK);

					if (StackWaitBlock.back = YY_CV_GET_BLOCK(Current))
					{
						StackWaitBlock.notify = nullptr;

						New |= 0x8;
					}
					else
					{
						StackWaitBlock.notify = &StackWaitBlock;
					}

					const auto Last = InterlockedCompareExchange((volatile size_t*)ConditionVariable, New, Current);

					if (Last == Current)
					{
						break;
					}


					Current = Last;
				}

				if (Flags& CONDITION_VARIABLE_LOCKMODE_SHARED)
					YY::Thunks::ReleaseSRWLockShared(SRWLock);
				else
					YY::Thunks::ReleaseSRWLockExclusive(SRWLock);

				if ((Current ^ New) & 0x8)
				{
					//新增0x8 标记位才调用 RtlpOptimizeConditionVariableWaitList
					internal::RtlpOptimizeConditionVariableWaitList(ConditionVariable, New);
				}

				auto GlobalKeyedEventHandle = internal::GetGlobalKeyedEventHandle();
				auto pNtWaitForKeyedEvent = try_get_NtWaitForKeyedEvent();
				if (!pNtWaitForKeyedEvent)
				{
					internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
				}

				//自旋
				for (auto SpinCount = ConditionVariableSpinCount; SpinCount; --SpinCount)
				{
					if (!(StackWaitBlock.flag & 2))
						break;

					YieldProcessor();
				}

				if (InterlockedBitTestAndReset((volatile LONG*)&StackWaitBlock.flag, 1))
				{
					LARGE_INTEGER TimeOut;

					Status = pNtWaitForKeyedEvent(GlobalKeyedEventHandle, (PVOID)&StackWaitBlock, 0, internal::BaseFormatTimeOut(&TimeOut, dwMilliseconds));

					if (Status == STATUS_TIMEOUT && internal::RtlpWakeSingle(ConditionVariable, &StackWaitBlock) == FALSE)
					{
						pNtWaitForKeyedEvent(GlobalKeyedEventHandle, (PVOID)&StackWaitBlock, 0, nullptr);
						Status = 0;
					}
				}

				if (Flags& CONDITION_VARIABLE_LOCKMODE_SHARED)
					YY::Thunks::AcquireSRWLockShared(SRWLock);
				else
					YY::Thunks::AcquireSRWLockExclusive(SRWLock);

			} while (false);


			internal::BaseSetLastNTError(Status);

			return Status >= 0 && Status != STATUS_TIMEOUT;

		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		WakeConditionVariable,
			_Inout_ PCONDITION_VARIABLE ConditionVariable
			)
		{
			if (auto const pWakeConditionVariable = try_get_WakeConditionVariable())
			{
				return pWakeConditionVariable(ConditionVariable);
			}

			size_t Current = *(volatile size_t*)ConditionVariable;
			size_t Last;

			for (; Current; Current = Last)
			{
				if (Current & 0x8)
				{
					if ((Current & 0x7) == 0x7)
					{
						return;
					}

					Last = InterlockedCompareExchange((volatile size_t*)ConditionVariable, Current + 1, Current);
					if (Last == Current)
						return;
				}
				else
				{
					Last = InterlockedCompareExchange((volatile size_t*)ConditionVariable, Current | 0x8, Current);
					if (Last == Current)
					{
						internal::RtlpWakeConditionVariable(ConditionVariable, Current + 8, 1);
						return;
					}
				}
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		WakeAllConditionVariable,
			_Inout_ PCONDITION_VARIABLE ConditionVariable
			)
		{
			if (auto const pWakeAllConditionVariable = try_get_WakeAllConditionVariable())
			{
				return pWakeAllConditionVariable(ConditionVariable);
			}

			size_t Current = *(volatile size_t*)ConditionVariable;
			size_t Last;

			for (; Current && (Current & 0x7) != 0x7; Current = Last)
			{
				if (Current & 0x8)
				{
					Last = InterlockedCompareExchange((volatile size_t*)ConditionVariable, Current | 0x7, Current);
					if (Last == Current)
						return;
				}
				else
				{
					Last = InterlockedCompareExchange((volatile size_t*)ConditionVariable, 0, Current);
					if (Last == Current)
					{
						auto GlobalKeyedEventHandle = internal::GetGlobalKeyedEventHandle();
						auto pNtReleaseKeyedEvent = try_get_NtReleaseKeyedEvent();

						if (!pNtReleaseKeyedEvent)
						{
							internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
						}

						for (auto pBlock = YY_CV_GET_BLOCK(Current); pBlock;)
						{
							auto Tmp = pBlock->back;

							if (!InterlockedBitTestAndReset((volatile LONG*)&pBlock->flag, 1))
							{
								pNtReleaseKeyedEvent(GlobalKeyedEventHandle, pBlock, FALSE, nullptr);
							}

							pBlock = Tmp;
						}

						return;
					}
				}
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Windows 8 [desktop apps only]
		//Windows Server 2012 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		12,
		BOOL,
		WINAPI,
		InitializeSynchronizationBarrier,
			_Out_ LPSYNCHRONIZATION_BARRIER lpBarrier,
			_In_ LONG lTotalThreads,
			_In_ LONG lSpinCount
			)
		{
			if (auto const pEnterSynchronizationBarrier = try_get_InitializeSynchronizationBarrier())
			{
				return pEnterSynchronizationBarrier(lpBarrier, lTotalThreads, lSpinCount);
			}

			auto pYYBarrier = (YY_BARRIER*)lpBarrier;

			pYYBarrier->lTotalThreads = lTotalThreads;
			pYYBarrier->lRemainderThreads = lTotalThreads;
			pYYBarrier->dwNumProcessors = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->NumberOfProcessors;
			pYYBarrier->dwSpinCount = lSpinCount == -1 ? 2000 : lSpinCount;

			if ((pYYBarrier->hEvent[0] = CreateEventW(nullptr, TRUE, FALSE, nullptr)) == nullptr)
				return FALSE;

			if ((pYYBarrier->hEvent[1] = CreateEventW(nullptr, TRUE, FALSE, nullptr)) == nullptr)
			{
				auto lStatus = GetLastError();
				CloseHandle(pYYBarrier->hEvent[0]);
				SetLastError(lStatus);
				return FALSE;
			}

			return TRUE;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Windows 8 [desktop apps only]
		//Windows Server 2012 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		8,
		BOOL,
		WINAPI,
		EnterSynchronizationBarrier,
			_Inout_ LPSYNCHRONIZATION_BARRIER lpBarrier,
			_In_ DWORD dwFlags
			)
		{
			if (auto const pEnterSynchronizationBarrier = try_get_EnterSynchronizationBarrier())
			{
				return pEnterSynchronizationBarrier(lpBarrier, dwFlags);
			}

			if (dwFlags & ~(SYNCHRONIZATION_BARRIER_FLAGS_SPIN_ONLY | SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY | SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE))
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
			}

			DWORD dwRtlBarrierFlags = dwFlags & (SYNCHRONIZATION_BARRIER_FLAGS_SPIN_ONLY | SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY);

			if ((dwFlags & SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE) == 0)
				dwRtlBarrierFlags |= 0x10000;


			auto pYYBarrier = (YY_BARRIER*)lpBarrier;


			auto Current = InterlockedDecrement(&pYYBarrier->lRemainderThreads);

			const auto GroupStatus = Current & 0x80000000;

			const auto EventIndex = Current >= 0 ? 1 : 0;
			DWORD dwRemainderThreads = Current & 0x7FFFFFFF;

			if (dwRemainderThreads == 0)
			{
				//剩余数量已经变成 0 了，我们需要唤醒之前等待的线程，并且翻转 EventIndex
				const auto NewEventIndex = EventIndex ? 0 : 1;


				//将下次需要使用的事件设置为 无信号状态
				auto NewEvent = pYYBarrier->hEvent[NewEventIndex];
				if (size_t(NewEvent) & 0x1)
				{
					auto hEvent = (HANDLE)(size_t(NewEvent) & ~size_t(0x1));
					pYYBarrier->hEvent[NewEventIndex] = hEvent;

					ResetEvent(hEvent);
				}

				//吧当前的 GroupStatus 状态置反，然后组合成新的 RemainderThreads
				auto NewRemainderThreads = pYYBarrier->lTotalThreads | (GroupStatus ^ 0x80000000);

				//此标记后将自动删除 Barrier，后续也将无法再次利用 Barrier
				if (dwRtlBarrierFlags & 0x10000)
				{
					pYYBarrier->lTotalThreads = 1;
				}
				pYYBarrier->lRemainderThreads = NewRemainderThreads;

				auto Event = pYYBarrier->hEvent[EventIndex];
				if (size_t(Event) & 0x1)
				{
					SetEvent((HANDLE)(size_t(Event) & ~size_t(0x1)));
				}

				return TRUE;
			}

			if (pYYBarrier->dwSpinCount && ((dwRemainderThreads < pYYBarrier->dwNumProcessors && (dwRtlBarrierFlags & SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY) == 0) || (dwRtlBarrierFlags & SYNCHRONIZATION_BARRIER_FLAGS_SPIN_ONLY)))
			{
				DWORD CurrentSpinCount = 0;
				for (;;)
				{
					if ((pYYBarrier->lRemainderThreads & 0x80000000) != GroupStatus)
					{
						//自旋等待成功
						auto dwSpinCount = pYYBarrier->dwSpinCount;

						if (dwSpinCount < 5000)
						{
							pYYBarrier->dwSpinCount = dwSpinCount + 1;
						}

						goto __retrun;
					}

					//指定 SYNCHRONIZATION_BARRIER_FLAGS_SPIN_ONLY 标识时会始终自旋，所以没有此标识时我们无需更新当前自旋次数
					if ((dwRtlBarrierFlags & SYNCHRONIZATION_BARRIER_FLAGS_SPIN_ONLY) == 0)
					{
						++CurrentSpinCount;
					}

					YieldProcessor();

					auto dwSpinCount = pYYBarrier->dwSpinCount;

					if (CurrentSpinCount >= dwSpinCount)
					{
						if (dwSpinCount > 50)
						{
							pYYBarrier->dwSpinCount = dwSpinCount - 1;
						}

						break;
					}
				}
			}

			{
				//如果没有 0x1标记则添加一个，来指示已经 WaitForSingleObject 了
				HANDLE hEnvet = pYYBarrier->hEvent[EventIndex];
				if ((size_t(hEnvet) & 0x1) == 0)
				{
					pYYBarrier->hEvent[EventIndex] = HANDLE(size_t(hEnvet) | 0x1);
				}

				if ((pYYBarrier->lRemainderThreads & 0x80000000) == GroupStatus)
					WaitForSingleObject((HANDLE)(size_t(hEnvet) & ~size_t(0x1)), INFINITE);
			}

		__retrun:

			if (dwRtlBarrierFlags & 0x10000)
				InterlockedIncrement(&pYYBarrier->lTotalThreads);

			return TRUE;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Windows 8 [desktop apps only]
		//Windows Server 2012 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		4,
		BOOL,
		WINAPI,
		DeleteSynchronizationBarrier,
			_Inout_ LPSYNCHRONIZATION_BARRIER lpBarrier
			)
		{
			if (auto const pDeleteSynchronizationBarrier = try_get_DeleteSynchronizationBarrier())
			{
				return pDeleteSynchronizationBarrier(lpBarrier);
			}

			auto pYYBarrier = (YY_BARRIER*)lpBarrier;

			//自旋等待所有 EnterSynchronizationBarrier 都进入就绪
			for (; pYYBarrier->lTotalThreads != (pYYBarrier->lRemainderThreads & 0x7FFFFFFF);)
			{
				//等待16毫秒
				Sleep(16);
			}

			BOOL bSuccess = FALSE;
			if (auto hEnent = (HANDLE)(size_t(pYYBarrier->hEvent[0]) & ~size_t(0x1)))
				CloseHandle(hEnent);

			if (auto hEnent = (HANDLE)(size_t(pYYBarrier->hEvent[1]) & ~size_t(0x1)))
				CloseHandle(hEnent);

			//微软文档说的，这个函数总是返回 TRUE，但是微软Windows 8的实现略有问题，直接转发了 RtlDeleteBarrier，返回值并不正确
			return TRUE;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Windows 8 [desktop apps | UWP apps]
		//Windows Server 2012 [desktop apps | UWP apps] 
		__DEFINE_THUNK(
		api_ms_win_core_synch_l1_2_0,
		16,
		BOOL,
		WINAPI,
		WaitOnAddress,
			_In_reads_bytes_(AddressSize) volatile VOID* Address,
			_In_reads_bytes_(AddressSize) PVOID CompareAddress,
			_In_ SIZE_T AddressSize,
			_In_opt_ DWORD dwMilliseconds
			)
		{
			if (auto const pWaitOnAddress = try_get_WaitOnAddress())
			{
				return pWaitOnAddress(Address, CompareAddress, AddressSize, dwMilliseconds);
			}

			//参数检查，AddressSize 只能 为 1,2,4,8
			if (AddressSize > 8 || AddressSize == 0 || ((AddressSize - 1) & AddressSize) != 0)
			{
				//STATUS_INVALID_PARAMETER
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}

			YY_ADDRESS_WAIT_BLOCK WaitBlock;
			WaitBlock.Address = Address;
			WaitBlock.back = nullptr;
			WaitBlock.notify = nullptr;
			WaitBlock.next = nullptr;
			WaitBlock.flag = 1;

			internal::RtlpAddWaitBlockToWaitList(&WaitBlock);

			bool bSame;
			switch (AddressSize)
			{
			case 1:
				bSame = *(volatile byte*)Address == *(volatile byte*)CompareAddress;
				break;
			case 2:
				bSame = *(volatile WORD*)Address == *(volatile WORD*)CompareAddress;
				break;
			case 4:
				bSame = *(volatile DWORD*)Address == *(volatile DWORD*)CompareAddress;
				break;
			default:
			//case 8:
#if _WIN64
				//64位自身能保证操作的原子性
				bSame = *(volatile unsigned long long*)Address == *(volatile unsigned long long*)CompareAddress;
#else
				bSame = InterlockedCompareExchange64((volatile long long*)Address, 0, 0) == *(volatile long long*)CompareAddress;
#endif
				break;
			}


			if (!bSame)
			{
				//结果不相同，我们从等待队列移除
				internal::RtlpWaitOnAddressRemoveWaitBlock(&WaitBlock);
				return TRUE;
			}

			LARGE_INTEGER TimeOut;

			auto Status = internal::RtlpWaitOnAddressWithTimeout(&WaitBlock, internal::BaseFormatTimeOut(&TimeOut, dwMilliseconds));

			if (Status)
			{
				internal::BaseSetLastNTError(Status);
			}

			return Status < 0 || Status == STATUS_TIMEOUT ? FALSE : TRUE;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Windows 8 [desktop apps | UWP apps]
		//Windows Server 2012 [desktop apps | UWP apps] 
		__DEFINE_THUNK(
		api_ms_win_core_synch_l1_2_0,
		4,
		VOID,
		WINAPI,
		WakeByAddressSingle,
			_In_ PVOID Address
			)
		{
			if (auto const pWakeByAddressSingle = try_get_WakeByAddressSingle())
			{
				return pWakeByAddressSingle(Address);
			}

			internal::RtlpWakeByAddress(Address, FALSE);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Windows 8 [desktop apps | UWP apps]
		//Windows Server 2012 [desktop apps | UWP apps] 
		__DEFINE_THUNK(
		api_ms_win_core_synch_l1_2_0,
		4,
		VOID,
		WINAPI,
		WakeByAddressAll,
			_In_ PVOID Address
			)
		{
			if (auto const pWakeByAddressAll = try_get_WakeByAddressAll())
			{
				return pWakeByAddressAll(Address);
			}

			internal::RtlpWakeByAddress(Address, TRUE);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		// Minimum supported client	Windows 7 [desktop apps | UWP apps]
		// Minimum supported server	Windows Server 2008 R2 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		28,
		BOOL,
		WINAPI,
		SetWaitableTimerEx,
			_In_ HANDLE _hTimer,
			_In_ const LARGE_INTEGER* _lpDueTime,
			_In_ LONG _iPeriod,
			_In_opt_ PTIMERAPCROUTINE _pfnCompletionRoutine,
			_In_opt_ LPVOID _lpArgToCompletionRoutine,
			_In_opt_ PREASON_CONTEXT _pWakeContext,
			_In_ ULONG _uTolerableDelay
			)
		{
			if (auto const _pfnSetWaitableTimerEx = try_get_SetWaitableTimerEx())
			{
				return _pfnSetWaitableTimerEx(_hTimer, _lpDueTime, _iPeriod, _pfnCompletionRoutine, _lpArgToCompletionRoutine, _pWakeContext, _uTolerableDelay);
			}

			UNREFERENCED_PARAMETER(_uTolerableDelay);

			LSTATUS _lStatus = ERROR_SUCCESS;
			// 检查 _pWakeContext 参数是否合法
			// Version 必须为 POWER_REQUEST_CONTEXT_VERSION
			// Flags 无效标记可参考 DIAGNOSTIC_REASON_INVALID_FLAGS，此外 POWER_REQUEST_CONTEXT_DETAILED_STRING，POWER_REQUEST_CONTEXT_SIMPLE_STRING 必须设置。
			// 有种特殊情况是 DIAGNOSTIC_REASON_NOT_SPECIFIED，它会被直接放行。不要问我……逆向结果如此，微软文档并未公开。
			// 微软文档链接：https://learn.microsoft.com/zh-cn/windows/win32/api/minwinbase/ns-minwinbase-reason_context
			do
			{
				if (!_pWakeContext)
					break;

				if (_pWakeContext->Version != POWER_REQUEST_CONTEXT_VERSION)
				{
					_lStatus = ERROR_INVALID_PARAMETER;
					break;
				}

				const auto _fFlags = _pWakeContext->Flags;

				// 微软原版就是如此，定义 DIAGNOSTIC_REASON_NOT_SPECIFIED 时会直接放行……
				// 我们保持微软的逻辑，这时不进一步的参数验证。
				if (_fFlags & DIAGNOSTIC_REASON_NOT_SPECIFIED)
					break;

				// 无效的Flags组合
				if ((_fFlags & DIAGNOSTIC_REASON_INVALID_FLAGS) != 0
					|| (_fFlags & (POWER_REQUEST_CONTEXT_DETAILED_STRING | POWER_REQUEST_CONTEXT_SIMPLE_STRING)) == 0)
				{
					_lStatus = ERROR_INVALID_PARAMETER;
					break;
				}

				if (_fFlags & POWER_REQUEST_CONTEXT_DETAILED_STRING)
				{
					if(_pWakeContext->Reason.Detailed.LocalizedReasonId > 0xFFFFu)
					{
						_lStatus = ERROR_INVALID_PARAMETER;
						break;
					}
				}
			} while (false);

			if (_lStatus != ERROR_SUCCESS)
			{
				SetLastError(_lStatus);
				return FALSE;
			}

			return SetWaitableTimer(_hTimer, _lpDueTime, _iPeriod, _pfnCompletionRoutine, _lpArgToCompletionRoutine, _pWakeContext != nullptr);
		}
#endif
	}//namespace Thunks

} //namespace YY
