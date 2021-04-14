

#ifndef YY_Thunks_Defined

//��д���ο���https://blog.csdn.net/yichigo/article/details/36898561
#define YY_SRWLOCK_OWNED_BIT           0
#define YY_SRWLOCK_CONTENDED_BIT       1
#define YY_SRWLOCK_SHARED_BIT          2
#define YY_SRWLOCK_CONTENTION_LOCK_BIT 3
//�Ѿ����˻�������
#define YY_SRWLOCK_OWNED               0x00000001ul
//�������ڵȴ���
#define YY_SRWLOCK_CONTENDED           0x00000002ul
//���������Ż���
#define YY_SRWLOCK_SHARED              0x00000004ul
//
#define YY_SRWLOCK_CONTENTION_LOCK     0x00000008ul
#define YY_SRWLOCK_MASK    (size_t(YY_SRWLOCK_OWNED | YY_SRWLOCK_CONTENDED | YY_SRWLOCK_SHARED | YY_SRWLOCK_CONTENTION_LOCK))
#define YY_SRWLOCK_BITS    4
#define YY_SRWLOCK_GET_BLOCK(SRWLock) ((YY_SRWLOCK_WAIT_BLOCK*)(SRWLock & (~YY_SRWLOCK_MASK)))

//SRWLock��������
#define SRWLockSpinCount 1024


typedef struct __declspec(align(16)) _YY_SRWLOCK_WAIT_BLOCK
{
	_YY_SRWLOCK_WAIT_BLOCK* back;
	_YY_SRWLOCK_WAIT_BLOCK* notify;
	_YY_SRWLOCK_WAIT_BLOCK* next;
	volatile size_t         shareCount;
	volatile size_t         flag;
} YY_SRWLOCK_WAIT_BLOCK;


//�����Ż���
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

//WaitOnAddress��������
#define RtlpWaitOnAddressSpinCount 1024

typedef struct __declspec(align(8)) _YY_ADDRESS_WAIT_BLOCK
{
	volatile void* Address;
	//��ΪWindows 8�Լ����߰汾��֧�� ZwWaitForAlertByThreadId����������ֱ�Ӱ� ThreadId �����ˣ�����û����
	//ULONG_PTR            ThreadId;

	_YY_ADDRESS_WAIT_BLOCK* back;
	_YY_ADDRESS_WAIT_BLOCK* notify;
	_YY_ADDRESS_WAIT_BLOCK* next;
	volatile size_t         flag;

} YY_ADDRESS_WAIT_BLOCK;


//YY-Thunks��Barrier������Windows 8ʵ��
typedef struct _YY_BARRIER {
	volatile LONG  lRemainderThreads;
	volatile LONG  lTotalThreads;
	HANDLE         hEvent[2];
	DWORD          dwNumProcessors;
	DWORD          dwSpinCount;
} YY_BARRIER;

static_assert(sizeof(SYNCHRONIZATION_BARRIER) >= sizeof(YY_BARRIER), "�����ϵͳABI���ݣ�������");

#endif



namespace YY
{
	namespace Thunks
	{
#ifndef YY_Thunks_Defined
		namespace internal
		{
			static HANDLE __fastcall GetGlobalKeyedEventHandle()
			{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)
				//Windows XP��ƽ̨�� ʹ��ϵͳ����� CritSecOutOfMemoryEvent��Vista���߸���ƽ̨ ����ֱ�ӷ��� nullptr ���ɡ�
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
				//Vista����ƽ̨֧�ָ� KeyedEventֱ�Ӵ� nullptr
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
					if ((Status & YY_SRWLOCK_OWNED) == 0)
					{
						//΢��Ͳ��ж��¿�ָ�룿������ţ�
						auto pWatiBlock = YY_SRWLOCK_GET_BLOCK(Status);
						
						YY_SRWLOCK_WAIT_BLOCK* notify;

						for (auto pBlock = pWatiBlock; (notify = pBlock->notify) == nullptr;)
						{
							auto back = pBlock->back;
							back->next = pBlock;

							pBlock = back;
						}
						
						pWatiBlock->notify = notify;


						//�ж��Ƿ���һ����ռ��
						if (notify->next && (notify->flag & 1))
						{
							pWatiBlock->notify = notify->next;
							notify->next = nullptr;

							//SRWLock & (~YY_SRWLOCK_SHARED)
#ifdef _WIN64
							_InterlockedAnd64((volatile LONG_PTR *)SRWLock, ~LONG_PTR(YY_SRWLOCK_SHARED));
#else
							_InterlockedAnd((volatile LONG_PTR *)SRWLock, ~LONG_PTR(YY_SRWLOCK_SHARED));
#endif
							if (!InterlockedBitTestAndReset((volatile LONG*)&notify->flag, 1))
							{
								//�鴦�ڵȴ�״̬�����ǽ����̻߳���

								//if(!RtlpWaitCouldDeadlock())
								
								pNtReleaseKeyedEvent(GlobalKeyedEventHandle, notify, 0, nullptr);
							}

							return;
						}
						else
						{
							//�ȴ�����һ������������ô�������еȴ��Ĺ�������
							auto NewStatus = InterlockedCompareExchange((volatile size_t *)SRWLock, 0, Status);

							if (NewStatus == Status)
							{
								//���³ɹ���
								for (; notify;)
								{
									auto next = notify->next;


									if (!InterlockedBitTestAndReset((volatile LONG*)&notify->flag, 1))
									{
										//�鴦�ڵȴ�״̬�����ǽ����̻߳���

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
						auto NewStatus = InterlockedCompareExchange((volatile LONG *)SRWLock, Status & ~YY_SRWLOCK_SHARED, Status);
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
					if (Status & YY_SRWLOCK_OWNED)
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

						//΢��Ϊʲô�� Status - YY_SRWLOCK_SHARED����Ϊʲô���� Status & ~YY_SRWLOCK_SHARED ��
						auto CurrentStatus = InterlockedCompareExchange((volatile size_t *)SRWLock, Status & ~YY_SRWLOCK_SHARED, Status);
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

			//���ȴ������ SRWLock ��
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

						//_YY_CV_WAIT_BLOCK �ṹ��� _YY_SRWLOCK_WAIT_BLOCK���ݣ�����������ǿת
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

					//���� ���ֵ�Bug������Ӧ�÷��� TRUE�����ٱ�Ҫ���ں˵ȴ���
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
						//WinXP����ϵͳ��֧�ֿվ�����룬����Ϊ�����հ�Windows 7
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
				//�������
				if ((dwFlags & ~(RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC)) || ((dwFlags - 1) & dwFlags))
				{
					return STATUS_INVALID_PARAMETER_2;
				}

				auto Current = *(volatile size_t*)lpInitOnce;

				if ((Current & (RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC)) == RTL_RUN_ONCE_ASYNC)
				{
					InterlockedExchange((volatile size_t *)lpInitOnce, dwFlags);

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
						//�ɻ�Ϊʲô΢��Ҫ�����жϡ���
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
					//��ֹ��ַ��Ч�������ȱ�����
					auto NextWakeAddress = *(LPVOID*)WakeAddress;

					//WinXP����ϵͳ��֧�ֿվ�����룬����Ϊ�����հ�Windows 7
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
				//���������Ч  ���� ͬʱʹ���˶�����λ
				if ((dwFlags & ~(RTL_RUN_ONCE_ASYNC | RTL_RUN_ONCE_INIT_FAILED)) || ((dwFlags - 1) & dwFlags))
				{
					return STATUS_INVALID_PARAMETER_2;
				}

				/*
				dwFlags =   0, dwNewFlags = 0x3��RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_ASYNC��
				dwFlags = 0x2, dwNewFlags = 0x2��RTL_RUN_ONCE_ASYNC��
				dwFlags = 0x4, dwNewFlags = 0x5��RTL_RUN_ONCE_CHECK_ONLY | RTL_RUN_ONCE_INIT_FAILED��
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
				if (pNtReleaseKeyedEvent)
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
						if (pItem->next == nullptr)
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
						//0x2״̬�����仯 ����Ҫ�����Ż�����
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

							//ͬ���ɹ���
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
									pNotify->back->back = Tmp;
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
				//���� ��������������ֱ�ӽ���ȴ����̼���
				if (NtCurrentTeb()->ProcessEnvironmentBlock->NumberOfProcessors > 1 && RtlpWaitOnAddressSpinCount)
				{
					for (DWORD SpinCount = 0; SpinCount < RtlpWaitOnAddressSpinCount;++SpinCount)
					{
						if ((pWaitBlock->flag & 1) == 0)
						{
							//���������У��ȵ����źŸı�
							return STATUS_SUCCESS;
						}

						YieldProcessor();
					}
				}

				if (!_interlockedbittestandreset((volatile long *)&pWaitBlock->flag, 0))
				{
					//�������Ǿܾ��ģ����������ã�״̬�Ѿ������˷�ת
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
			static void __fastcall YY_RtlpWakeByAddress(DWORD dwReserved/*����ƽ��ջ��Ҫ�����ڱ������Ż���jmp*/, BOOL bWakeAll, LPVOID Address)
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
											//�������ƣ�����һ��
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
								//NtReleaseKeyedEvent ���ú� pItem �ڴ�������Ч����˱����ȱ���һ�ݡ�
								auto Tmp = pItem->back;

								//���ѵȴ���Key
								pNtReleaseKeyedEvent(GlobalKeyedEventHandle, pItem, FALSE, nullptr);

								pItem = Tmp;
							}

							if (!bNoRemove)
							{
								//���±�ͷ��


								for (auto Current = *ppFirstBlock;;)
								{
									const auto Last = InterlockedCompareExchange(ppFirstBlock, (Current & 1) == 0 ? size_t(YY_ADDRESS_GET_BLOCK(Current)) : 0, Current);

									if (Last == Current)
									{
										if(Current & 1)
											RtlpWaitOnAddressWakeEntireList(YY_ADDRESS_GET_BLOCK(Current));

										break;
									}
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

EXTERN_C
BOOL
WINAPI
InitializeCriticalSectionEx(
    _Out_ LPCRITICAL_SECTION lpCriticalSection,
    _In_ DWORD dwSpinCount,
    _In_ DWORD Flags
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pInitializeCriticalSectionEx = try_get_InitializeCriticalSectionEx())
	{
		return pInitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags);
	}

	return InitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
}
#endif

__YY_Thunks_Expand_Function(kernel32, InitializeCriticalSectionEx, 12);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
VOID
WINAPI
InitOnceInitialize(
    _Out_ PINIT_ONCE InitOnce
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	*InitOnce = INIT_ONCE_STATIC_INIT;
}
#endif

__YY_Thunks_Expand_Function(kernel32, InitOnceInitialize, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
BOOL
WINAPI
InitOnceBeginInitialize(
    _Inout_ LPINIT_ONCE lpInitOnce,
    _In_ DWORD dwFlags,
    _Out_ PBOOL fPending,
    _Outptr_opt_result_maybenull_ LPVOID* lpContext
    )
#ifdef YY_Thunks_Defined
	;
#else
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

__YY_Thunks_Expand_Function(kernel32, InitOnceBeginInitialize, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
BOOL
WINAPI
InitOnceComplete(
    _Inout_ LPINIT_ONCE lpInitOnce,
    _In_ DWORD dwFlags,
    _In_opt_ LPVOID lpContext
    )
#ifdef YY_Thunks_Defined
	;
#else
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

__YY_Thunks_Expand_Function(kernel32, InitOnceComplete, 12);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
BOOL
WINAPI
InitOnceExecuteOnce(
    _Inout_ PINIT_ONCE InitOnce,
    _In_ __callback PINIT_ONCE_FN InitFn,
    _Inout_opt_ PVOID Parameter,
    _Outptr_opt_result_maybenull_ LPVOID* Context
    )
#ifdef YY_Thunks_Defined
	;
#else
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

	//������Ϊʲô������LastError������Windows 7���������Ҳû���� LastError����Ȼ���������Ƕ�͵���°ɣ���Ϊ��΢����һ�¡�
	return Status >= STATUS_SUCCESS;
}
#endif

__YY_Thunks_Expand_Function(kernel32, InitOnceExecuteOnce, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
HANDLE
WINAPI
CreateEventExW(
    _In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
    _In_opt_ LPCWSTR lpName,
    _In_ DWORD dwFlags,
    _In_ DWORD dwDesiredAccess
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pCreateEventExW = try_get_CreateEventExW())
	{
		return pCreateEventExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
	}

	return CreateEventW(lpEventAttributes, dwFlags & CREATE_EVENT_MANUAL_RESET, dwFlags & CREATE_EVENT_INITIAL_SET, lpName);
}
#endif

__YY_Thunks_Expand_Function(kernel32, CreateEventExW, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
HANDLE
WINAPI
CreateEventExA(
    _In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
    _In_opt_ LPCSTR lpName,
    _In_ DWORD dwFlags,
    _In_ DWORD dwDesiredAccess
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pCreateEventExA = try_get_CreateEventExA())
	{
		return pCreateEventExA(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
	}

	return CreateEventA(lpEventAttributes, dwFlags & CREATE_EVENT_MANUAL_RESET, dwFlags & CREATE_EVENT_INITIAL_SET, lpName);
}
#endif

__YY_Thunks_Expand_Function(kernel32, CreateEventExA, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
HANDLE
WINAPI
CreateMutexExW(
    _In_opt_ LPSECURITY_ATTRIBUTES lpMutexAttributes,
    _In_opt_ LPCWSTR lpName,
    _In_ DWORD dwFlags,
    _In_ DWORD dwDesiredAccess
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pCreateMutexExW = try_get_CreateMutexExW())
	{
		return pCreateMutexExW(lpMutexAttributes, lpName, dwFlags, dwDesiredAccess);
	}

	return CreateMutexW(lpMutexAttributes, dwFlags & CREATE_MUTEX_INITIAL_OWNER, lpName);
}
#endif

__YY_Thunks_Expand_Function(kernel32, CreateMutexExW, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
HANDLE
WINAPI
CreateMutexExA(
    _In_opt_ LPSECURITY_ATTRIBUTES lpMutexAttributes,
    _In_opt_ LPCSTR lpName,
    _In_ DWORD dwFlags,
    _In_ DWORD dwDesiredAccess
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pCreateMutexExA = try_get_CreateMutexExA())
	{
		return pCreateMutexExA(lpMutexAttributes, lpName, dwFlags, dwDesiredAccess);
	}

	return CreateMutexA(lpMutexAttributes, dwFlags & CREATE_MUTEX_INITIAL_OWNER, lpName);
}
#endif

__YY_Thunks_Expand_Function(kernel32, CreateMutexExA, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
HANDLE
WINAPI
CreateSemaphoreExW(
    _In_opt_ LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
    _In_ LONG lInitialCount,
    _In_ LONG lMaximumCount,
    _In_opt_ LPCWSTR lpName,
    _Reserved_ DWORD dwFlags,
    _In_ DWORD dwDesiredAccess
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pCreateSemaphoreExW = try_get_CreateSemaphoreExW())
	{
		return pCreateSemaphoreExW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess);
	}

	return CreateSemaphoreW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);
}
#endif

__YY_Thunks_Expand_Function(kernel32, CreateSemaphoreExW, 24);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
HANDLE
WINAPI
CreateWaitableTimerExW(
    _In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
    _In_opt_ LPCWSTR lpTimerName,
    _In_ DWORD dwFlags,
    _In_ DWORD dwDesiredAccess
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pCreateWaitableTimerExW = try_get_CreateWaitableTimerExW())
	{
		return pCreateWaitableTimerExW(lpTimerAttributes, lpTimerName, dwFlags, dwDesiredAccess);
	}

	return CreateWaitableTimerW(lpTimerAttributes, dwFlags&CREATE_WAITABLE_TIMER_MANUAL_RESET, lpTimerName);
}
#endif

__YY_Thunks_Expand_Function(kernel32, CreateWaitableTimerExW, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
VOID
WINAPI
InitializeSRWLock(
    _Out_ PSRWLOCK SRWLock
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	*SRWLock = RTL_SRWLOCK_INIT;
}
#endif

__YY_Thunks_Expand_Function(kernel32, InitializeSRWLock, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
VOID
WINAPI
AcquireSRWLockExclusive(
    _Inout_ PSRWLOCK SRWLock
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pAcquireSRWLockExclusive = try_get_AcquireSRWLockExclusive())
	{
		return pAcquireSRWLockExclusive(SRWLock);
	}

	YY_SRWLOCK_WAIT_BLOCK StackWaitBlock;
	bool bOptimize;

	//���Լ���һ��
#if defined(_WIN64)
	auto OldBit = InterlockedBitTestAndSet64((volatile LONG_PTR*)SRWLock, YY_SRWLOCK_OWNED_BIT);
#else
	auto OldBit = InterlockedBitTestAndSet((volatile LONG_PTR*)SRWLock, YY_SRWLOCK_OWNED_BIT);
#endif

	if(OldBit == false)
	{
		//�ɹ�����
		return;
	}

	for (;;)
	{
		auto SRWLockOld =  *(volatile size_t*)SRWLock;

		if (YY_SRWLOCK_OWNED & SRWLockOld)
		{
			/*
			if (RtlpWaitCouldDeadlock())
				ZwTerminateProcess((HANDLE)0xFFFFFFFF, 0xC000004B);
			*/

			StackWaitBlock.next = nullptr;
			StackWaitBlock.flag = 3;
			bOptimize = false;

			size_t SRWLockNew;

			if (YY_SRWLOCK_CONTENDED & SRWLockOld)
			{
				//�������ڵȴ�����
				StackWaitBlock.notify = nullptr;
				StackWaitBlock.shareCount = 0;
				StackWaitBlock.back = (YY_SRWLOCK_WAIT_BLOCK*)(SRWLockOld & (~YY_SRWLOCK_MASK));

				SRWLockNew = (size_t)(&StackWaitBlock) | (SRWLockOld & YY_SRWLOCK_CONTENTION_LOCK) | YY_SRWLOCK_SHARED | YY_SRWLOCK_CONTENDED | YY_SRWLOCK_OWNED;

				if ((YY_SRWLOCK_SHARED & SRWLockOld) == 0)
				{
					bOptimize = true;
				}
			}
			else
			{
				//û��������û�еȴ�������������Ҫ����һ��
				StackWaitBlock.notify = (YY_SRWLOCK_WAIT_BLOCK*)&StackWaitBlock;
				StackWaitBlock.shareCount = (SRWLockOld >> YY_SRWLOCK_BITS);


				SRWLockNew = StackWaitBlock.shareCount > 1 ?
					(size_t)(&StackWaitBlock) | YY_SRWLOCK_CONTENTION_LOCK | YY_SRWLOCK_CONTENDED | YY_SRWLOCK_OWNED 
					: (size_t)(&StackWaitBlock) | YY_SRWLOCK_CONTENDED | YY_SRWLOCK_OWNED;
			}

			if (InterlockedCompareExchange((volatile size_t*)SRWLock, SRWLockNew, SRWLockOld) != SRWLockOld)
			{
				//������״̬ʧ�ܣ������߳����ڴ��������Ҫ��զ�ǻ�����������
				
				//RtlBackoff���������ˣ�����ֻ�ǵȴ�һ����ѣ�ֱ��YieldProcessor����һ�ΰɡ�
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

			//����
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
			//���Ի�ȡ��������Ȩ
			if (InterlockedCompareExchange((volatile size_t*)SRWLock, SRWLockOld | YY_SRWLOCK_OWNED, SRWLockOld) == SRWLockOld)
			{
				//�ɹ�����
				return;
			}

			//���ܶ��̲߳������ʣ�������������һ��
			YieldProcessor();
		}
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, AcquireSRWLockExclusive, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7 [desktop apps | UWP apps]
//Windows Server 2008 R2 [desktop apps | UWP apps]

EXTERN_C
BOOLEAN
WINAPI
TryAcquireSRWLockExclusive(
    _Inout_ PSRWLOCK SRWLock
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pTryAcquireSRWLockExclusive = try_get_TryAcquireSRWLockExclusive())
	{
		return pTryAcquireSRWLockExclusive(SRWLock);
	}

#if defined(_WIN64)
	return InterlockedBitTestAndSet64((volatile LONG_PTR*)SRWLock, YY_SRWLOCK_OWNED_BIT);
#else
	return InterlockedBitTestAndSet((volatile LONG_PTR*)SRWLock, YY_SRWLOCK_OWNED_BIT);
#endif
}
#endif

__YY_Thunks_Expand_Function(kernel32, TryAcquireSRWLockExclusive, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
VOID
WINAPI
ReleaseSRWLockExclusive(
    _Inout_ PSRWLOCK SRWLock
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pReleaseSRWLockExclusive = try_get_ReleaseSRWLockExclusive())
	{
		return pReleaseSRWLockExclusive(SRWLock);
	}

	auto OldSRWLock = InterlockedExchangeAdd((volatile size_t *)SRWLock, size_t(-1));
	if ((OldSRWLock & YY_SRWLOCK_OWNED) == 0)
	{
		internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
	}

	if ((OldSRWLock & YY_SRWLOCK_CONTENDED) && (OldSRWLock & YY_SRWLOCK_SHARED) == 0)
	{
		OldSRWLock -= YY_SRWLOCK_OWNED;

		auto NewSRWLock = OldSRWLock | YY_SRWLOCK_SHARED;
		auto CurrentSRWLock = InterlockedCompareExchange((volatile size_t *)SRWLock, NewSRWLock, OldSRWLock);

		if (CurrentSRWLock == OldSRWLock)
			internal::RtlpWakeSRWLock(SRWLock, NewSRWLock);
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, ReleaseSRWLockExclusive, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
VOID
WINAPI
AcquireSRWLockShared(
    _Inout_ PSRWLOCK SRWLock
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pAcquireSRWLockShared = try_get_AcquireSRWLockShared())
	{
		return pAcquireSRWLockShared(SRWLock);
	}

	YY_SRWLOCK_WAIT_BLOCK StackWaitBlock;
	bool bOptimize;

	//���Ը�ȫ�µ�������	
	auto OldSRWLock = InterlockedCompareExchange((volatile size_t*)SRWLock, size_t(0x11), 0);

	//�ɹ�
	if (OldSRWLock == size_t(0))
	{
		return;
	}


	size_t NewSRWLock;

	for (;; OldSRWLock = *(volatile size_t *)SRWLock)
	{
		if ((OldSRWLock & YY_SRWLOCK_OWNED) && ((OldSRWLock & YY_SRWLOCK_CONTENDED) || YY_SRWLOCK_GET_BLOCK(OldSRWLock) == nullptr))
		{
			//if ( RtlpWaitCouldDeadlock() )
			//    ZwTerminateProcess((HANDLE)0xFFFFFFFF, 0xC000004B);


			StackWaitBlock.flag = 2;
			StackWaitBlock.shareCount = 0;
			StackWaitBlock.next = nullptr;

			bOptimize = false;

			if (OldSRWLock & YY_SRWLOCK_CONTENDED)
			{
				//�Ѿ����˵ȴ������ǲ���һ���µĵȴ���
				StackWaitBlock.back = YY_SRWLOCK_GET_BLOCK(OldSRWLock);
				StackWaitBlock.notify = nullptr;

				NewSRWLock = size_t(&StackWaitBlock) | (OldSRWLock & YY_SRWLOCK_CONTENTION_LOCK) | (YY_SRWLOCK_SHARED | YY_SRWLOCK_CONTENDED | YY_SRWLOCK_OWNED);

				if ((OldSRWLock & YY_SRWLOCK_SHARED) == 0)
				{
					bOptimize = true;
				}
			}
			else
			{
				StackWaitBlock.notify = &StackWaitBlock;
				NewSRWLock = size_t(&StackWaitBlock) | (YY_SRWLOCK_CONTENDED | YY_SRWLOCK_OWNED);
			}


			if (InterlockedCompareExchange((volatile size_t *)SRWLock, NewSRWLock, OldSRWLock) == OldSRWLock)
			{
				//���³ɹ���

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

				//����
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
			if (OldSRWLock & YY_SRWLOCK_CONTENDED)
			{
				//��Ȼ�����ڵȴ�������ôYY_SRWLOCK_OWNEDӦ�����¼���
				NewSRWLock = OldSRWLock | YY_SRWLOCK_OWNED;
			}
			else
			{
				//û���˵ȴ�����ô�����Ӹ� 0x10����
				NewSRWLock = (OldSRWLock + 0x10) | YY_SRWLOCK_OWNED;
			}

			if (InterlockedCompareExchange((volatile size_t *)SRWLock, NewSRWLock, OldSRWLock) == OldSRWLock)
				return;
		}

		//͵���£�ֱ�� YieldProcessor ��
		//RtlBackoff(&nBackOff);
		YieldProcessor();
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, AcquireSRWLockShared, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7 [desktop apps | UWP apps] 
//Windows Server 2008 R2 [desktop apps | UWP apps]

EXTERN_C
BOOLEAN
WINAPI
TryAcquireSRWLockShared(
    _Inout_ PSRWLOCK SRWLock
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pTryAcquireSRWLockShared = try_get_TryAcquireSRWLockShared())
	{
		return pTryAcquireSRWLockShared(SRWLock);
	}

	//���Ը�ȫ�µ�������
	auto OldSRWLock = InterlockedCompareExchange((volatile size_t*)SRWLock, size_t(0x11), 0);

	//�ɹ�
	if (OldSRWLock == size_t(0))
	{
		return TRUE;
	}

	for (;;)
	{
		if ((OldSRWLock & YY_SRWLOCK_OWNED) && ((OldSRWLock & YY_SRWLOCK_CONTENDED) || YY_SRWLOCK_GET_BLOCK(OldSRWLock) == nullptr))
		{
			//���ڱ�������
			return FALSE;
		}
		else
		{
			size_t NewSRWLock;

			if (OldSRWLock & YY_SRWLOCK_CONTENDED)
				NewSRWLock = OldSRWLock | YY_SRWLOCK_OWNED;
			else
				NewSRWLock = (OldSRWLock + 0x10) | YY_SRWLOCK_OWNED;

			if (InterlockedCompareExchange((volatile size_t*)SRWLock, NewSRWLock, OldSRWLock) == OldSRWLock)
			{
				//�������
				return TRUE;
			}

			//RtlBackoff((unsigned int *)&v4);
			YieldProcessor();
			OldSRWLock = *(volatile size_t*)SRWLock;
		}
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, TryAcquireSRWLockShared, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
VOID
WINAPI
ReleaseSRWLockShared(
    _Inout_ PSRWLOCK SRWLock
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pReleaseSRWLockShared = try_get_ReleaseSRWLockShared())
	{
		return pReleaseSRWLockShared(SRWLock);
	}

	//���Խ���ֻ��һ�ζ��������

	auto OldSRWLock = InterlockedCompareExchange((volatile size_t*)SRWLock, 0, size_t(0x11));

	//�����ɹ�
	if (OldSRWLock == size_t(0x11))
	{
		return;
	}

	if ((OldSRWLock & YY_SRWLOCK_OWNED) == 0)
	{
		internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
	}

	for (;;)
	{
		if (OldSRWLock & YY_SRWLOCK_CONTENDED)
		{
			if (OldSRWLock & YY_SRWLOCK_CONTENTION_LOCK)
			{
				auto pLastNode = YY_SRWLOCK_GET_BLOCK(OldSRWLock);

				for (; pLastNode->notify == nullptr; pLastNode = pLastNode->back);

				/* 
				��Ȼ�����ͷŹ�������˵��һ�����˻�ȡ�˹�����
				������˻�ȡ�˹���������һ��û���˻�ȡ����ռ��
				ֻ��Ҫ�ѹ��������1
				ȡ��notify�ڵ�Ĺ�����������ĵ�ַ, ԭ�Ӽ�
				*/
				if (InterlockedDecrement((volatile size_t *)&(pLastNode->notify->shareCount)) > 0)
				{
					return;
				}
			}


			for (;;)
			{
				auto NewSRWLock = OldSRWLock & (~(YY_SRWLOCK_CONTENTION_LOCK | YY_SRWLOCK_OWNED));
				size_t LastSRWLock;

				if (OldSRWLock & YY_SRWLOCK_SHARED)
				{
					LastSRWLock = InterlockedCompareExchange((volatile size_t *)SRWLock, NewSRWLock, OldSRWLock);

					if (LastSRWLock == OldSRWLock)
						return;
				}
				else
				{
					NewSRWLock |= YY_SRWLOCK_SHARED;

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

__YY_Thunks_Expand_Function(kernel32, ReleaseSRWLockShared, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
VOID
WINAPI
InitializeConditionVariable(
    _Out_ PCONDITION_VARIABLE ConditionVariable
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	*ConditionVariable = CONDITION_VARIABLE_INIT;
}
#endif

__YY_Thunks_Expand_Function(kernel32, InitializeConditionVariable, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
BOOL
WINAPI
SleepConditionVariableCS(
    _Inout_ PCONDITION_VARIABLE ConditionVariable,
    _Inout_ PCRITICAL_SECTION   CriticalSection,
    _In_    DWORD               dwMilliseconds
    )
#ifdef YY_Thunks_Defined
	;
#else
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

	//0x8 �������ʱ���Ž����Ż� ConditionVariableWaitList
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

	//����
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

__YY_Thunks_Expand_Function(kernel32, SleepConditionVariableCS, 12);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
BOOL
WINAPI
SleepConditionVariableSRW(
    _Inout_ PCONDITION_VARIABLE ConditionVariable,
    _Inout_ PSRWLOCK SRWLock,
    _In_ DWORD dwMilliseconds,
    _In_ ULONG Flags
    )
#ifdef YY_Thunks_Defined
	;
#else
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
			//����0x8 ���λ�ŵ��� RtlpOptimizeConditionVariableWaitList
			internal::RtlpOptimizeConditionVariableWaitList(ConditionVariable, New);
		}

		auto GlobalKeyedEventHandle = internal::GetGlobalKeyedEventHandle();
		auto pNtWaitForKeyedEvent = try_get_NtWaitForKeyedEvent();
		if (!pNtWaitForKeyedEvent)
		{
			internal::RaiseStatus(STATUS_RESOURCE_NOT_OWNED);
		}

		//����
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

__YY_Thunks_Expand_Function(kernel32, SleepConditionVariableSRW, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
VOID
WINAPI
WakeConditionVariable(
    _Inout_ PCONDITION_VARIABLE ConditionVariable
    )
#ifdef YY_Thunks_Defined
	;
#else
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

__YY_Thunks_Expand_Function(kernel32, WakeConditionVariable, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
VOID
WINAPI
WakeAllConditionVariable(
    _Inout_ PCONDITION_VARIABLE ConditionVariable
    )
#ifdef YY_Thunks_Defined
	;
#else
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

__YY_Thunks_Expand_Function(kernel32, WakeAllConditionVariable, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps only]
//Windows Server 2012 [desktop apps only]

EXTERN_C
BOOL
WINAPI
InitializeSynchronizationBarrier(
    _Out_ LPSYNCHRONIZATION_BARRIER lpBarrier,
    _In_ LONG lTotalThreads,
    _In_ LONG lSpinCount
    )
#ifdef YY_Thunks_Defined
	;
#else
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

__YY_Thunks_Expand_Function(kernel32, InitializeSynchronizationBarrier, 12);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps only]
//Windows Server 2012 [desktop apps only]

EXTERN_C
BOOL
WINAPI
EnterSynchronizationBarrier(
    _Inout_ LPSYNCHRONIZATION_BARRIER lpBarrier,
    _In_ DWORD dwFlags
    )
#ifdef YY_Thunks_Defined
	;
#else
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
		//ʣ�������Ѿ���� 0 �ˣ�������Ҫ����֮ǰ�ȴ����̣߳����ҷ�ת EventIndex
		const auto NewEventIndex = EventIndex ? 0 : 1;


		//���´���Ҫʹ�õ��¼�����Ϊ ���ź�״̬
		auto NewEvent = pYYBarrier->hEvent[NewEventIndex];
		if (size_t(NewEvent) & 0x1)
		{
			auto hEvent = (HANDLE)(size_t(NewEvent) & ~size_t(0x1));
			pYYBarrier->hEvent[NewEventIndex] = hEvent;

			ResetEvent(hEvent);
		}

		//�ɵ�ǰ�� GroupStatus ״̬�÷���Ȼ����ϳ��µ� RemainderThreads
		auto NewRemainderThreads = pYYBarrier->lTotalThreads | (GroupStatus ^ 0x80000000);

		//�˱�Ǻ��Զ�ɾ�� Barrier������Ҳ���޷��ٴ����� Barrier
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
				//�����ȴ��ɹ�
				auto dwSpinCount = pYYBarrier->dwSpinCount;

				if (dwSpinCount < 5000)
				{
					pYYBarrier->dwSpinCount = dwSpinCount + 1;
				}

				goto __retrun;
			}

			//ָ�� SYNCHRONIZATION_BARRIER_FLAGS_SPIN_ONLY ��ʶʱ��ʼ������������û�д˱�ʶʱ����������µ�ǰ��������
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
		//���û�� 0x1��������һ������ָʾ�Ѿ� WaitForSingleObject ��
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

__YY_Thunks_Expand_Function(kernel32, EnterSynchronizationBarrier, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps only]
//Windows Server 2012 [desktop apps only]

EXTERN_C
BOOL
WINAPI
DeleteSynchronizationBarrier(
    _Inout_ LPSYNCHRONIZATION_BARRIER lpBarrier
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pDeleteSynchronizationBarrier = try_get_DeleteSynchronizationBarrier())
	{
		return pDeleteSynchronizationBarrier(lpBarrier);
	}

	auto pYYBarrier = (YY_BARRIER*)lpBarrier;

	//�����ȴ����� EnterSynchronizationBarrier ���������
	for (; pYYBarrier->lTotalThreads != (pYYBarrier->lRemainderThreads & 0x7FFFFFFF);)
	{
		//�ȴ�16����
		Sleep(16);
	}

	BOOL bSuccess = FALSE;
	if (auto hEnent = (HANDLE)(size_t(pYYBarrier->hEvent[0]) & ~size_t(0x1)))
		CloseHandle(hEnent);

	if (auto hEnent = (HANDLE)(size_t(pYYBarrier->hEvent[1]) & ~size_t(0x1)))
		CloseHandle(hEnent);

	//΢���ĵ�˵�ģ�����������Ƿ��� TRUE������΢��Windows 8��ʵ���������⣬ֱ��ת���� RtlDeleteBarrier������ֵ������ȷ
	return TRUE;
}
#endif

__YY_Thunks_Expand_Function(kernel32, DeleteSynchronizationBarrier, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps] 

EXTERN_C
BOOL
WINAPI
WaitOnAddress(
    _In_reads_bytes_(AddressSize) volatile VOID* Address,
    _In_reads_bytes_(AddressSize) PVOID CompareAddress,
    _In_ SIZE_T AddressSize,
    _In_opt_ DWORD dwMilliseconds
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pWaitOnAddress = try_get_WaitOnAddress())
	{
		return pWaitOnAddress(Address, CompareAddress, AddressSize, dwMilliseconds);
	}

	//������飬AddressSize ֻ�� Ϊ 1,2,4,8
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
		//64λ�����ܱ�֤������ԭ����
		bSame = *(volatile unsigned long long*)Address == *(volatile unsigned long long*)CompareAddress;
#else
		bSame = InterlockedCompareExchange64((volatile long long*)Address, 0, 0) == *(volatile long long*)CompareAddress;
#endif
		break;
	}


	if (!bSame)
	{
		//�������ͬ�����Ǵӵȴ������Ƴ�
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

__YY_Thunks_Expand_Function(kernel32, WaitOnAddress, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps] 

EXTERN_C
VOID
WINAPI
WakeByAddressSingle(
    _In_ PVOID Address
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pWakeByAddressSingle = try_get_WakeByAddressSingle())
	{
		return pWakeByAddressSingle(Address);
	}

	internal::RtlpWakeByAddress(Address, FALSE);
}
#endif

__YY_Thunks_Expand_Function(kernel32, WakeByAddressSingle, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps] 

EXTERN_C
VOID
WINAPI
WakeByAddressAll(
    _In_ PVOID Address
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pWakeByAddressAll = try_get_WakeByAddressAll())
	{
		return pWakeByAddressAll(Address);
	}

	internal::RtlpWakeByAddress(Address, TRUE);
}
#endif

__YY_Thunks_Expand_Function(kernel32, WakeByAddressAll, 4);

#endif
	}//namespace Thunks

} //namespace YY