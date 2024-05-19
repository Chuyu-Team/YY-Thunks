
#include <threadpoolapiset.h>

#ifdef YY_Thunks_Implemented

struct _TP_CLEANUP_GROUP_FUNCS
{
	//0
	void (__fastcall* pfnTppWorkpFree)(_TP_WORK* Work);
	//4
    void (__fastcall *pfnTppWorkCallbackEpilog)(_TP_WORK* _pWork);

	//8
	SRWLOCK srwLock;
	//C
    void(__fastcall* pfnTppWorkCancelPendingCallbacks)(_TP_WORK* _pWork);
	//0x10
};

union TaskVFuncs
{
    void* pfnCallback;
	void(__fastcall* pExecuteCallback)(PTP_CALLBACK_INSTANCE Instance, _TP_WORK* Work);
};

struct _TP_CLEANUP_GROUP
{
	//0
	DWORD p0;
	//4
	DWORD p4;
	//8
	SRWLOCK srwLock;
	//0xC
	void* pC;
	//0x10
	void* p10;
	//0x14
	DWORD p14;
	//0x18
	DWORD p18;
	//0x1C
	DWORD p1C;
	//0x20
	SRWLOCK srwLock2;
	//0x24
	DWORD p24;
	//0x28
	SRWLOCK srwLock3;
	//0x2C
	void* p2C;
	//0x30
	void* p30;
	//0x34
	DWORD p34;
	//0x38
};

//此结构从微软Vista的ntdll 内部扒出来的，我们目前只实现到Vista
struct _TP_WORK
{
	//0 该结构的引用计数
	long nRef;
	//4
	const _TP_CLEANUP_GROUP_FUNCS* VFuncs;
	//8
	_TP_CLEANUP_GROUP* CleanupGroup;
	//0xC
	PTP_CLEANUP_GROUP_CANCEL_CALLBACK  CleanupGroupCancelCallback;
	//0x10
	PTP_SIMPLE_CALLBACK FinalizationCallback;
	//0x14
	void* p14;
	//0x18
	void* p18;
	//0x1C
	DWORD un1C;
	//0x20
	DWORD p20;
	//0x24
	DWORD p24;
	//0x28
	SRWLOCK srwLock;
	//0x2C
	DWORD p2C;
	//0x30
	void* Callback;
	//0x34 上下文
	PVOID Context;
	//0x38
	_ACTIVATION_CONTEXT* ActivationContext;
	//0x3C
	void* SubProcessTag;
	//0x40
	PVOID RaceDll;
	//0x44
	PTP_POOL Pool;
	//0x48
	DWORD un48;
	//0x4C
	DWORD un4C;
	//0x50  TP_CALLBACK_ENVIRON 中的 Flags
	// 0x10000 已经调用Release
    // 0x30000 WaitForThreadpoolIoCallbacks里用了，含义未知
	long uFlags1;
	//0x54 似乎保存了 TpAllocWork 函数的返回地址意义暂时不明
	void* retaddr;
	//0x58 保存 TpReleaseWork 函数的返回地址
	void* un58;
	//0x5C
	DWORD un5C;
	//0x60
	const TaskVFuncs* pTaskVFuncs;
	//0x64
    union
    {
        volatile DWORD uFlags;
        struct
        {
            // 作用未知
            DWORD uBit1 : 1;
            // 投递的任务数量
            DWORD uPendingCount : 31;
            // 正在调用的任务（YY-Thunks 自身扩展，微软原版没有）
            DWORD uWorkingCount;
        };

        volatile ULONGLONG uFlags64;
    };
	//0x68
	//附加字段，微软原版没有
	//当没有任务时，改句柄将处于激活状态
	HANDLE hEvent;

    void AddRef()
    {
        InterlockedIncrement(&nRef);
    }

    void Release()
    {
        if (InterlockedDecrement(&nRef) == 0)
        {
            VFuncs->pfnTppWorkpFree(this);
        }
    }

    void AddPendingCount()
    {
        if (InterlockedAdd((volatile LONG*)&uFlags, 2) < 4)
        {
            ResetEvent(hEvent);
        }
    }

    bool TryReleasePendingount()
    {
        ULARGE_INTEGER _uLast;
        for (_uLast.QuadPart = uFlags64;;)
        {
            ULARGE_INTEGER _uNew = _uLast;
            if (_uNew.LowPart < 2)
                return false;

            _uNew.LowPart -= 2;

            auto _uResult = InterlockedCompareExchange64((volatile LONG64*)&uFlags64, _uNew.QuadPart, _uLast.QuadPart);
            if (_uResult == _uLast.QuadPart)
            {
                //计数为 0，唤醒等待
                if (_uNew.QuadPart < 2ull)
                {
                    SetEvent(hEvent);
                }
                return true;
            }

            _uLast.QuadPart = _uResult;
        }
    }

    bool TryReleaseWorkingCountAddWorkingCount()
    {
        ULARGE_INTEGER _uLast;
        for (_uLast.QuadPart = uFlags64;;)
        {
            ULARGE_INTEGER _uNew = _uLast;
            if (_uNew.LowPart < 2)
            {
                return false;
            }
            else
            {
                _uNew.LowPart -= 2;
            }

            _uNew.HighPart += 1;

            auto _uResult = InterlockedCompareExchange64((volatile LONG64*)&uFlags64, _uNew.QuadPart, _uLast.QuadPart);
            if (_uResult == _uLast.QuadPart)
            {
                return true;
            }

            _uLast.QuadPart = _uResult;
        }
    }

    void ReleaseWorkingCount()
    {
        ULARGE_INTEGER _uLast;
        for (_uLast.QuadPart = uFlags64;;)
        {
            ULARGE_INTEGER _uNew = _uLast;
            _uNew.HighPart -= 1;

            auto _uResult = InterlockedCompareExchange64((volatile LONG64*)&uFlags64, _uNew.QuadPart, _uLast.QuadPart);
            if (_uResult == _uLast.QuadPart)
            {
                //计数为 0，唤醒等待
                if (_uNew.QuadPart < 2ull)
                {
                    SetEvent(hEvent);
                }
                return;
            }

            _uLast.QuadPart = _uResult;
        }
    }
};

//0xD8
struct _TP_TIMER : public _TP_WORK
{
    // 以下为YY-Thunks扩展属性
	HANDLE hQueueTimer;
    SRWLOCK QueueTimerLock;
};

//0xA0
struct _TP_WAIT : public _TP_WORK
{
    // 以下为YY-Thunks扩展属性
	HANDLE hWaitObject;
    SRWLOCK WaitObjectLock;
};

struct _TP_IO : public _TP_WORK
{
    // 0x68, StartThreadpoolIo的调用次数，YY-Thunk中复用 uPendingCount
    // volatile DWORD uStartAsyncIoRef;

    // 与微软原版相比稍微做一些些改变，CreateThreadpoolIo直接在TP_IO保存上下文。
    HANDLE hFile;
    //PTP_WIN32_IO_CALLBACK pfnIoCallback;
    //PVOID pUserData;
    YY::Thunks::internal::HookThunkData* pThunkData;
};

//不完善的定义，我们用多少，处理多少。
struct _TP_CALLBACK_INSTANCE
{
	//0x40
	PCRITICAL_SECTION CriticalSection;
	//0x44
	HMODULE DllHandle;
	//0x48
	HANDLE Event;
	//0x4C
	HANDLE Mutex;
	//0x50
	HANDLE Semaphore;
	//0x54
	DWORD ReleaseCount;
};

#endif


namespace YY
{
	namespace Thunks
	{
#if defined(YY_Thunks_Implemented) && (YY_Thunks_Support_Version < NTDDI_WIN6)

		namespace Fallback
		{
			static void __fastcall DoWhenCallbackReturns(_TP_CALLBACK_INSTANCE* Instance)
			{
				if (auto CriticalSection = Instance->CriticalSection)
				{
					LeaveCriticalSection(CriticalSection);
				}

				if (auto DllHandle = Instance->DllHandle)
				{
					FreeLibrary(DllHandle);
				}

				if (auto Event = Instance->Event)
				{
					SetEvent(Event);
				}

				if (auto Mutex = Instance->Mutex)
				{
					ReleaseMutex(Mutex);
				}

				if (auto Semaphore = Instance->Semaphore)
				{
					ReleaseSemaphore(Semaphore, Instance->ReleaseCount, nullptr);
				}
			}

			static void __fastcall TppWorkpFree(_TP_WORK* Work)
			{
				if (Work)
				{
					const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
					HeapFree(ProcessHeap, 0, Work);
				}
			}

			static void __fastcall TppWorkCallbackEpilog(_TP_WORK* _pWork)
			{

			}

			static void __fastcall TppWorkCancelPendingCallbacks(_TP_WORK* _pWork)
			{

			}

			static const _TP_CLEANUP_GROUP_FUNCS* __fastcall GetTppWorkpCleanupGroupMemberVFuncs()
			{
				static _TP_CLEANUP_GROUP_FUNCS TppWorkpCleanupGroupMemberVFuncs = { &TppWorkpFree, &TppWorkCallbackEpilog, 0, &TppWorkCancelPendingCallbacks };
				return &TppWorkpCleanupGroupMemberVFuncs;
			}

			static void __fastcall TppWorkpExecuteCallback(PTP_CALLBACK_INSTANCE Instance, _TP_WORK* Work)
			{
				((PTP_WORK_CALLBACK)Work->Callback)(Instance, Work->Context, Work);
			}

			static const TaskVFuncs* __fastcall GetTppWorkpTaskVFuncs()
			{
				static TaskVFuncs TppWorkpTaskVFuncs = { &TppWorkpExecuteCallback };
				return &TppWorkpTaskVFuncs;
			}

            static void __fastcall TppCleanupGroupMemberWait(_TP_WORK* _pWork, ULONG _uCancelledCallbackCount)
            {
                if(_pWork->uFlags64 & (~1ull))
                    WaitForSingleObject(_pWork->hEvent, INFINITE);
            }

            static void __fastcall TppWorkWait(_TP_WORK* _pWork, BOOL _bCancelPendingCallbacks, bool bReleaseRef = false)
            {
                ULONG _uCancelledCallbackCount = 0;
                if (_bCancelPendingCallbacks)
                {
                    for (auto _uLast = _pWork->uFlags;;)
                    {
                        _uCancelledCallbackCount = _uLast >> 1;
                        if (_uCancelledCallbackCount == 0)
                            break;

                        const auto _uResult = InterlockedCompareExchange(&_pWork->uFlags, _uLast & 1, _uLast);
                        if (_uResult == _uLast)
                        {
                            break;
                        }

                        _uLast = _uResult;
                    }
                }

                TppCleanupGroupMemberWait(_pWork, _uCancelledCallbackCount);

                if (bReleaseRef)
                {
                    if (InterlockedAdd((volatile LONG*)&_pWork->nRef, -LONG(_uCancelledCallbackCount)) == 0)
                    {
                        _pWork->VFuncs->pfnTppWorkpFree(_pWork);
                    }
                }
            }

			static
			NTSTATUS
			__fastcall
			TppCleanupGroupMemberInitialize(
				_Inout_ _TP_WORK* pWork,
				_Inout_opt_ PVOID pv,
				_In_opt_ PTP_CALLBACK_ENVIRON pcbe,
				_In_ DWORD uFlags,
				const _TP_CLEANUP_GROUP_FUNCS* CleanupGroupVFuncs
				)
			{
				pWork->nRef = 1;
				pWork->VFuncs = CleanupGroupVFuncs;
				pWork->p20 = 0;
				pWork->p24 = 0;
				pWork->p2C = 0;

				::InitializeSRWLock(&pWork->srwLock);

				pWork->Context = pv;

				if (pcbe)
				{
					pWork->Pool = pcbe->Pool;
					pWork->CleanupGroup = pcbe->CleanupGroup;
					pWork->CleanupGroupCancelCallback = pcbe->CleanupGroupCancelCallback;
					pWork->FinalizationCallback = NULL;
					pWork->ActivationContext = pcbe->ActivationContext;
					pWork->RaceDll = pcbe->RaceDll;
				}
				else
				{
					pWork->Pool = NULL;
					pWork->CleanupGroup = NULL;
					pWork->CleanupGroupCancelCallback = NULL;
					pWork->FinalizationCallback = NULL;
					pWork->ActivationContext = NULL;
					pWork->RaceDll = NULL;
				}

				pWork->SubProcessTag = NtCurrentTeb()->SubProcessTag;
				pWork->uFlags1 = uFlags;
				pWork->p18 = pWork->p14 = &pWork->p14;


				if (pWork->ActivationContext != NULL)
				{
					if (pWork->ActivationContext != (_ACTIVATION_CONTEXT*)-1)
					{

					}
				}

				auto hEvent = CreateEventW(nullptr, TRUE, TRUE, nullptr);

				if (hEvent == nullptr)
				{
					return STATUS_NO_MEMORY;
				}

				pWork->hEvent = hEvent;

				return STATUS_SUCCESS;
			}

			static
			NTSTATUS
			__fastcall
			TppWorkInitialize(
				_Inout_ _TP_WORK* pWork,
				_Inout_opt_ PVOID pv,
				_In_opt_ PTP_CALLBACK_ENVIRON pcbe,
				_In_ DWORD uFlags,
				_In_ const _TP_CLEANUP_GROUP_FUNCS* CleanupGroupVFuncs,
				_In_ const TaskVFuncs* TaskVFuncs
				)
			{
				auto Status = TppCleanupGroupMemberInitialize(pWork, pv, pcbe, uFlags, CleanupGroupVFuncs);

				if (Status >= 0)
				{
					//if(pWork->)
					pWork->pTaskVFuncs = TaskVFuncs;
					pWork->uFlags = 1;
				}

				return Status;
			}

			static
			void
			__fastcall
			TppCleanupGroupAddMember(
				_Outptr_ _TP_WORK* pWork
				)
			{
				::AcquireSRWLockExclusive(&pWork->CleanupGroup->srwLock);

				pWork->p14 = &pWork->CleanupGroup->pC;
				pWork->p18 = pWork->CleanupGroup->p10;

				*(void**)pWork->CleanupGroup->p10 = pWork->p14;
				pWork->CleanupGroup->p10 = &pWork->p14;

				::ReleaseSRWLockExclusive(&pWork->CleanupGroup->srwLock);
			}

			static
			NTSTATUS
			__fastcall
			TpAllocWork(
				_Outptr_ _TP_WORK** ppWork,
				_In_ PTP_WORK_CALLBACK pfnwk,
				_Inout_opt_ PVOID pv,
				_In_opt_ PTP_CALLBACK_ENVIRON pcbe
				)
			{
				if (ppWork == nullptr || pfnwk == nullptr || (pcbe && (pcbe->u.Flags & 0xFFFFFFFE)))
				{
					return STATUS_INVALID_PARAMETER;
				}

				const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
				auto pWork = (_TP_WORK*)HeapAlloc(ProcessHeap, HEAP_ZERO_MEMORY, sizeof(_TP_WORK));

				if (!pWork)
					return STATUS_NO_MEMORY;

				pWork->retaddr = _ReturnAddress();

				auto Status = TppWorkInitialize(pWork, pv, pcbe, pcbe ? pcbe->u.Flags : 0, GetTppWorkpCleanupGroupMemberVFuncs(), GetTppWorkpTaskVFuncs());

				do
				{
					if (Status < 0)
						break;

					pWork->Callback = pfnwk;

					if (pcbe)
						pWork->FinalizationCallback = pcbe->FinalizationCallback;

					if (pWork->CleanupGroup)
						TppCleanupGroupAddMember(pWork);

					

					*ppWork = pWork;

					return STATUS_SUCCESS;
				} while (false);

				
				if(pWork)
					HeapFree(ProcessHeap, 0, pWork);
				

				return Status;
			}

			static void __fastcall TppWaitpFree(_TP_WORK* Work)
			{
				if (Work)
				{
					const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
					HeapFree(ProcessHeap, 0, Work);
				}
			}

			static const _TP_CLEANUP_GROUP_FUNCS* __fastcall GetTppWaitpCleanupGroupMemberVFuncs()
			{
				static const _TP_CLEANUP_GROUP_FUNCS TppWorkpCleanupGroupMemberVFuncs = { &TppWaitpFree, &TppWorkCallbackEpilog, 0, &TppWorkCancelPendingCallbacks };
				return &TppWorkpCleanupGroupMemberVFuncs;
			}

			static void __fastcall TppWaitpExecuteCallback(PTP_CALLBACK_INSTANCE Instance, _TP_WORK* Work)
			{
				((PTP_WAIT_CALLBACK)Work->Callback)(Instance, Work->Context, (PTP_WAIT)Work, 0);
			}


			static const TaskVFuncs* __fastcall GetTppWaitpTaskVFuncs()
			{
				static TaskVFuncs TppWorkpTaskVFuncs = { &TppWaitpExecuteCallback };
				return &TppWorkpTaskVFuncs;
			}

			static void __fastcall TppWorkPost(_TP_WORK* pwk)
			{
				//增加一次引用。 
                pwk->AddRef();
                pwk->AddPendingCount();

				auto bRet = QueueUserWorkItem([](LPVOID lpThreadParameter) -> DWORD
					{
						auto pwk = (PTP_WORK)lpThreadParameter;

                        if (pwk->TryReleaseWorkingCountAddWorkingCount())
                        {
                            TP_CALLBACK_INSTANCE Instance = {};
                            pwk->pTaskVFuncs->pExecuteCallback(&Instance, pwk);
                            Fallback::DoWhenCallbackReturns(&Instance);

                            pwk->ReleaseWorkingCount();
                        }

                        pwk->Release();
						return 0;

					}, pwk, 0);

				if (!bRet)
				{
					//QueueUserWorkItem失败，重新减少引用计数
                    pwk->TryReleasePendingount();
                    pwk->Release();
				}
			}

			//暂时我们不需要实现 GetTppSimplepCleanupGroupMemberVFuncs
#define GetTppSimplepCleanupGroupMemberVFuncs GetTppWorkpCleanupGroupMemberVFuncs


			static void __fastcall TppSimplepExecuteCallback(PTP_CALLBACK_INSTANCE Instance, _TP_WORK* Work)
			{
				((PTP_SIMPLE_CALLBACK)Work->Callback)(Instance, Work->Context);
			}


			static const TaskVFuncs* __fastcall GetTppSimplepTaskVFuncs()
			{
				static TaskVFuncs TppWorkpTaskVFuncs = { &TppSimplepExecuteCallback };
				return &TppWorkpTaskVFuncs;
			}

            static void __fastcall TppIopExecuteCallback(_TP_IO* _pIoWork, DWORD _lStatus, DWORD _cbNumberOfBytesTransfered, LPOVERLAPPED _pOverlapped)
            {
                auto _pfnIoCallback = reinterpret_cast<PTP_WIN32_IO_CALLBACK>(_pIoWork->Callback);

                if (_pIoWork->TryReleaseWorkingCountAddWorkingCount())
                {
                    _TP_CALLBACK_INSTANCE _Instance = {};
                    _pfnIoCallback(&_Instance, _pIoWork->Context, _pOverlapped, _lStatus, _cbNumberOfBytesTransfered, _pIoWork);
                    DoWhenCallbackReturns(&_Instance);

                    _pIoWork->ReleaseWorkingCount();
                    _pIoWork->Release();
                }
            }

            static const TaskVFuncs* __fastcall GetTppIopExecuteCallback()
            {
                static TaskVFuncs TppWorkpTaskVFuncs = { &TppIopExecuteCallback};
                return &TppWorkpTaskVFuncs;
            }

            static void __fastcall TppIopFree(_TP_WORK* _pWork)
            {
                if (auto _pIoWork = static_cast<_TP_IO*>(_pWork))
                {
                    if (_pIoWork->pThunkData)
                        _pIoWork->pThunkData->Free();

                    internal::Free(_pWork);
                }
            }

            static void __fastcall TppIopCallbackEpilog(_TP_WORK* _pWork)
            {

            }

            static void __fastcall TppIopCancelPendingCallbacks(_TP_WORK* _pWork)
            {
            }

            static const _TP_CLEANUP_GROUP_FUNCS* __fastcall GetTppIopCleanupGroupMemberVFuncs()
            {
                static _TP_CLEANUP_GROUP_FUNCS TppWorkpCleanupGroupMemberVFuncs = { &TppIopFree, &TppWorkCallbackEpilog, 0, &TppIopCancelPendingCallbacks };
                return &TppWorkpCleanupGroupMemberVFuncs;
            }


			static
			NTSTATUS
			__fastcall
			TpSimpleTryPost(
				_In_ PTP_SIMPLE_CALLBACK Callback,
				_Inout_opt_ PVOID Context,
				_In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron
				)
			{
				auto Flags = CallbackEnviron ? CallbackEnviron->u.Flags : 0;

				if (Callback == nullptr || (Flags & 0xFFFFFFFE) != 0)
				{
					internal::RaiseStatus(STATUS_INVALID_PARAMETER);
					return STATUS_INVALID_PARAMETER;
				}



				const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
				auto pWork = (_TP_WORK*)HeapAlloc(ProcessHeap, HEAP_ZERO_MEMORY, sizeof(_TP_WORK));

				if (!pWork)
					return STATUS_NO_MEMORY;

				pWork->retaddr = _ReturnAddress();

				auto Status = TppWorkInitialize(pWork, Context, CallbackEnviron, Flags, GetTppSimplepCleanupGroupMemberVFuncs(), GetTppSimplepTaskVFuncs());

				do
				{
					if (Status < 0)
						break;

					pWork->Callback = Callback;

					if (CallbackEnviron)
						pWork->FinalizationCallback = CallbackEnviron->FinalizationCallback;

					if (pWork->CleanupGroup)
						Fallback::TppCleanupGroupAddMember(pWork);

					TppWorkPost(pWork);

					return STATUS_SUCCESS;
				} while (false);


				if (pWork)
					HeapFree(ProcessHeap, 0, pWork);


				return Status;
			}

			static
			bool
			__fastcall
			TppCleanupGroupMemberRelease(
				_TP_WORK* CleanupGroupMember,
				bool RaiseIfAlreadyRaised
				)
			{
				auto orgFlags1 = InterlockedOr(&CleanupGroupMember->uFlags1, 0x10000);

				if (RaiseIfAlreadyRaised && (orgFlags1 & 0x10000))
				{
					internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				}

				return (orgFlags1 & 0x30000) == 0;
			}

			static
			void
			WINAPI
			TpReleaseWork(
				_TP_WORK* Work)
			{
				if (Work == nullptr || (Work->uFlags1 & 0x10000) || Work->VFuncs != Fallback::GetTppWorkpCleanupGroupMemberVFuncs())
				{
					internal::RaiseStatus(STATUS_INVALID_PARAMETER);
					return;
				}

				if (TppCleanupGroupMemberRelease(Work, true))
				{
					Work->un58 = _ReturnAddress();

					if (InterlockedExchangeAdd(&Work->nRef, -1) == 0)
					{
						Work->VFuncs->pfnTppWorkpFree(Work);
					}
				}
			}

			static void __fastcall TppTimerpFree(_TP_WORK* Work)
			{
				if (Work)
				{
					const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
					HeapFree(ProcessHeap, 0, (_TP_TIMER*)Work);
				}
			}

			static const _TP_CLEANUP_GROUP_FUNCS* __fastcall GetTppTimerpCleanupGroupMemberVFuncs()
			{
				static _TP_CLEANUP_GROUP_FUNCS TppTimerpCleanupGroupMemberVFuncs = { &TppTimerpFree };
				return &TppTimerpCleanupGroupMemberVFuncs;
			}


			static void __fastcall TppTimerpExecuteCallback(PTP_CALLBACK_INSTANCE Instance, _TP_WORK* Work)
			{
				((PTP_TIMER_CALLBACK)Work->Callback)(Instance, Work->Context, (PTP_TIMER)Work);
			}

			static const TaskVFuncs* __fastcall GetTppTimerpTaskVFuncs()
			{
				static TaskVFuncs TppTimerpTaskVFuncs = { &TppTimerpExecuteCallback };

				return &TppTimerpTaskVFuncs;
			}
			

			static
			NTSTATUS
			__fastcall
			TppTimerAlloc(
				_TP_TIMER** ppTimer,
				_In_ PTP_TIMER_CALLBACK Callback,
				_Inout_opt_ PVOID Context,
				_In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron,
				DWORD Flags
				)
			{
				
				*ppTimer = nullptr;


				const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
				auto pTimer = (_TP_TIMER*)HeapAlloc(ProcessHeap, HEAP_ZERO_MEMORY, sizeof(_TP_TIMER));

				if (!pTimer)
					return STATUS_NO_MEMORY;
				
				pTimer->retaddr = _ReturnAddress();

				auto Status = TppWorkInitialize(pTimer, Context, CallbackEnviron, Flags | 0x1040000, GetTppTimerpCleanupGroupMemberVFuncs(), GetTppTimerpTaskVFuncs());

				if (Status >= 0)
				{
					pTimer->Callback = Callback;

					if(CallbackEnviron)
						pTimer->FinalizationCallback = CallbackEnviron->FinalizationCallback;

					if (pTimer->CleanupGroup)
						TppCleanupGroupAddMember(pTimer);

					*ppTimer = pTimer;

					return Status;
				}

				HeapFree(ProcessHeap, 0, pTimer);
				return Status;

			}


			static
			NTSTATUS
			__fastcall
			TpAllocTimer(
				_TP_TIMER** ppTimer,
				_In_ PTP_TIMER_CALLBACK Callback,
				_Inout_opt_ PVOID Context,
				_In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron
				)
			{
				auto Flags = CallbackEnviron ? CallbackEnviron->u.Flags : 0;

				if (ppTimer == nullptr || Callback == nullptr || (Flags & 0xFFFFFFFE))
				{
					return STATUS_INVALID_PARAMETER;
				}

				auto Status = TppTimerAlloc(ppTimer, Callback, Context, CallbackEnviron, Flags);

				if (Status >= 0)
				{
					(*ppTimer)->retaddr = _ReturnAddress();
				}

				return Status;
			}

		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		12,
		PTP_WORK,
		WINAPI,
		CreateThreadpoolWork,
			_In_ PTP_WORK_CALLBACK pfnwk,
			_Inout_opt_ PVOID pv,
			_In_opt_ PTP_CALLBACK_ENVIRON pcbe
			)
		{
			if (auto const pCreateThreadpoolWork = try_get_CreateThreadpoolWork())
			{
				return pCreateThreadpoolWork(pfnwk, pv, pcbe);
			}

			//Fallback到我自己实现的 Tp
			_TP_WORK* pWork;

			auto Status = Fallback::TpAllocWork(&pWork, pfnwk, pv, pcbe);

			if (Status >= 0)
			{
				return pWork;
			}

			internal::BaseSetLastNTError(Status);
			return nullptr;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		CloseThreadpoolWork,
			_Inout_ PTP_WORK pwk
			)
		{
			if (auto const pCloseThreadpoolWork = try_get_CloseThreadpoolWork())
			{
				return pCloseThreadpoolWork(pwk);
			}

			Fallback::TpReleaseWork(pwk);
		}

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		SubmitThreadpoolWork,
			_Inout_ PTP_WORK pwk
			)
		{
			if (auto const pSubmitThreadpoolWork = try_get_SubmitThreadpoolWork())
			{
				return pSubmitThreadpoolWork(pwk);
			}

			//参数验证
			if (pwk == nullptr
				|| (pwk->uFlags1 & 0x30000)
				|| pwk->VFuncs != Fallback::GetTppWorkpCleanupGroupMemberVFuncs())
			{
                internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

			Fallback::TppWorkPost(pwk);
		}

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		VOID,
		WINAPI,
		WaitForThreadpoolWorkCallbacks,
			_Inout_ PTP_WORK pwk,
			_In_ BOOL fCancelPendingCallbacks
			)
		{
			if (auto const pWaitForThreadpoolWorkCallbacks = try_get_WaitForThreadpoolWorkCallbacks())
			{
				return pWaitForThreadpoolWorkCallbacks(pwk, fCancelPendingCallbacks);
			}

			if (pwk == nullptr
				|| (pwk->uFlags1 & 0x30000)
				|| (pwk->VFuncs != Fallback::GetTppWorkpCleanupGroupMemberVFuncs()))
			{
                internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

            Fallback::TppWorkWait(pwk, fCancelPendingCallbacks);
		}

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		12,
		PTP_TIMER,
		WINAPI,
		CreateThreadpoolTimer,
			_In_ PTP_TIMER_CALLBACK pfnti,
			_Inout_opt_ PVOID pv,
			_In_opt_ PTP_CALLBACK_ENVIRON pcbe
			)
		{
			if (auto const pCreateThreadpoolTimer = try_get_CreateThreadpoolTimer())
			{
				return pCreateThreadpoolTimer(pfnti, pv, pcbe);
			}

			PTP_TIMER pTimer;

			auto Status = Fallback::TpAllocTimer(&pTimer, pfnti, pv, pcbe);

			if (Status >= 0)
			{
				return pTimer;
			}

			internal::BaseSetLastNTError(Status);

			return nullptr;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		16,
		VOID,
		WINAPI,
		SetThreadpoolTimer,
			_Inout_ PTP_TIMER pti,
			_In_opt_ PFILETIME pftDueTime,
			_In_ DWORD msPeriod,
			_In_opt_ DWORD msWindowLength
			)
		{
			if (auto const pSetThreadpoolTimer = try_get_SetThreadpoolTimer())
			{
				return pSetThreadpoolTimer(pti, pftDueTime, msPeriod, msWindowLength);
			}

			if (pti == nullptr
				|| (pti->uFlags1 & 0x30000)
				|| pti->VFuncs != Fallback::GetTppTimerpCleanupGroupMemberVFuncs())
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

			if (pftDueTime == nullptr)
			{
                if (auto hOrgQueueTimer = InterlockedExchangePointer(&pti->hQueueTimer, nullptr))
                {
                    //销毁计数器
                    DeleteTimerQueueTimer(nullptr, hOrgQueueTimer, INVALID_HANDLE_VALUE);
                    SetEvent(pti->hEvent);
                    pti->Release();
                }
				return;
			}

            pti->AddRef();

			DWORD DueTime = 0;
			LARGE_INTEGER lDueTime;
			lDueTime.LowPart = pftDueTime->dwLowDateTime;
			lDueTime.HighPart = pftDueTime->dwHighDateTime;

			if (lDueTime.QuadPart < 0)
			{
				//相对时间
				DueTime = static_cast<DWORD>(lDueTime.QuadPart / -10'000);
			}
			else if (lDueTime.QuadPart > 0)
			{
				//绝对时间
				FILETIME CurrentTime;
				GetSystemTimeAsFileTime(&CurrentTime);
				LARGE_INTEGER lCurrentTime;
				lCurrentTime.LowPart = CurrentTime.dwLowDateTime;
				lCurrentTime.HighPart = CurrentTime.dwHighDateTime;

				if (lDueTime.QuadPart > lCurrentTime.QuadPart)
				{
					DueTime = static_cast<DWORD>((lDueTime.QuadPart - lCurrentTime.QuadPart) / 10'000);
				}
			}

            // Timer 比较特殊，因为始终只有一个句柄，hEvent不需要引用计数。
            ::AcquireSRWLockExclusive(&pti->QueueTimerLock);
            if (auto hOrgQueueTimer = InterlockedExchangePointer(&pti->hQueueTimer, nullptr))
            {
                //销毁计数器
                DeleteTimerQueueTimer(nullptr, hOrgQueueTimer, INVALID_HANDLE_VALUE);
                pti->Release();
            }
            ResetEvent(pti->hEvent);
			auto bRet = CreateTimerQueueTimer(&pti->hQueueTimer, nullptr, [](PVOID Parameter, BOOLEAN)
				{
					auto _pTimer = (PTP_TIMER)Parameter;
					TP_CALLBACK_INSTANCE Instance = {};

                    _pTimer->pTaskVFuncs->pExecuteCallback(&Instance, _pTimer);

					Fallback::DoWhenCallbackReturns(&Instance);
                    
                   SetEvent(_pTimer->hEvent);

				}, pti, DueTime, msPeriod, 0);
            
            ::ReleaseSRWLockExclusive(&pti->QueueTimerLock);

			if (!bRet)
			{
                // 创建失败了，所以把之前的内存块引用计数撤销
                SetEvent(pti->hEvent);
                pti->Release();
			}       
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		VOID,
		WINAPI,
		WaitForThreadpoolTimerCallbacks,
			_Inout_ PTP_TIMER Timer,
			_In_ BOOL CancelPendingCallbacks
			)
		{
			if (auto const pWaitForThreadpoolTimerCallbacks = try_get_WaitForThreadpoolTimerCallbacks())
			{
				return pWaitForThreadpoolTimerCallbacks(Timer, CancelPendingCallbacks);
			}


			if (Timer == nullptr
				|| (Timer->uFlags1 & 0x30000)
				|| Timer->VFuncs != Fallback::GetTppTimerpCleanupGroupMemberVFuncs())
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

            if (CancelPendingCallbacks)
            {
                if (auto hOrgQueueTimer = InterlockedExchangePointer(&Timer->hQueueTimer, NULL))
                {
                    DeleteTimerQueueTimer(nullptr, hOrgQueueTimer, INVALID_HANDLE_VALUE);
                    SetEvent(Timer->hEvent);
                    Timer->Release();
                }
                return;
            }
            else
            {
                if (Timer->hQueueTimer)
                {
                    WaitForSingleObject(Timer->hEvent, INFINITE);
                }
            }
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		CloseThreadpoolTimer,
			_Inout_ PTP_TIMER Timer
			)
		{
			if (auto const pCloseThreadpoolTimer = try_get_CloseThreadpoolTimer())
			{
				return pCloseThreadpoolTimer(Timer);
			}

			if (Timer == nullptr
				|| (Timer->uFlags1 & 0x10000)
				|| Timer->VFuncs != Fallback::GetTppTimerpCleanupGroupMemberVFuncs())
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

			if (Fallback::TppCleanupGroupMemberRelease(Timer, true))
			{
				Timer->un58 = _ReturnAddress();

				if (auto hOrgQueueTimer = InterlockedExchangePointer(&Timer->hQueueTimer, nullptr))
				{
					//销毁计数器
					DeleteTimerQueueTimer(nullptr, hOrgQueueTimer, INVALID_HANDLE_VALUE);
                    SetEvent(Timer->hEvent);
                    Timer->Release();
				}

                Timer->Release();
			}
		}
#endif

		
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		VOID,
		WINAPI,
		SetEventWhenCallbackReturns,
			_Inout_ PTP_CALLBACK_INSTANCE Instance,
			_In_ HANDLE Event
			)
		{
			if (auto const pSetEventWhenCallbackReturns = try_get_SetEventWhenCallbackReturns())
			{
				return pSetEventWhenCallbackReturns(Instance, Event);
			}


			//只允许被设置一次
			if (Instance == nullptr
				|| Event == nullptr
				|| Event == INVALID_HANDLE_VALUE
				|| Instance->Event != nullptr)
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

			Instance->Event = Event;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		12,
		VOID,
		WINAPI,
		ReleaseSemaphoreWhenCallbackReturns,
			_Inout_ PTP_CALLBACK_INSTANCE Instance,
			_In_ HANDLE Semaphore,
			_In_ DWORD ReleaseCount
			)
		{
			if (auto const pReleaseSemaphoreWhenCallbackReturns = try_get_ReleaseSemaphoreWhenCallbackReturns())
			{
				return pReleaseSemaphoreWhenCallbackReturns(Instance, Semaphore, ReleaseCount);
			}

			//只允许被设置一次
			if (Instance == nullptr
				|| Semaphore == nullptr
				|| Semaphore == INVALID_HANDLE_VALUE
				|| ReleaseCount == 0
				|| Instance->Semaphore != nullptr)
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

			Instance->Semaphore = Semaphore;
			Instance->ReleaseCount = ReleaseCount;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		VOID,
		WINAPI,
		ReleaseMutexWhenCallbackReturns,
			_Inout_ PTP_CALLBACK_INSTANCE Instance,
			_In_ HANDLE Mutex
			)
		{
			if (auto const pReleaseMutexWhenCallbackReturns = try_get_ReleaseMutexWhenCallbackReturns())
			{
				return pReleaseMutexWhenCallbackReturns(Instance, Mutex);
			}

			//只允许被设置一次
			if (Instance == nullptr
				|| Mutex == nullptr
				|| Mutex == INVALID_HANDLE_VALUE
				|| Instance->Mutex != nullptr)
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

			Instance->Mutex = Mutex;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		VOID,
		WINAPI,
		LeaveCriticalSectionWhenCallbackReturns,
			_Inout_ PTP_CALLBACK_INSTANCE Instance,
			_Inout_ PCRITICAL_SECTION CriticalSection
			)
		{
			if (auto const pLeaveCriticalSectionWhenCallbackReturns = try_get_LeaveCriticalSectionWhenCallbackReturns())
			{
				return pLeaveCriticalSectionWhenCallbackReturns(Instance, CriticalSection);
			}

			//只允许被设置一次
			if (Instance == nullptr
				|| CriticalSection == nullptr
				|| Instance->CriticalSection != nullptr)
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

			Instance->CriticalSection = CriticalSection;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		VOID,
		WINAPI,
		FreeLibraryWhenCallbackReturns,
			_Inout_ PTP_CALLBACK_INSTANCE Instance,
			_In_ HMODULE DllHandle
			)
		{
			if (auto const pFreeLibraryWhenCallbackReturns = try_get_FreeLibraryWhenCallbackReturns())
			{
				return pFreeLibraryWhenCallbackReturns(Instance, DllHandle);
			}

			//只允许被设置一次
			if (Instance == nullptr
				|| DllHandle == nullptr
				|| DllHandle == INVALID_HANDLE_VALUE
				|| Instance->DllHandle != nullptr)
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

			Instance->DllHandle = DllHandle;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		12,
		BOOL,
		WINAPI,
		TrySubmitThreadpoolCallback,
			_In_ PTP_SIMPLE_CALLBACK Callback,
			_Inout_opt_ PVOID Context,
			_In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron
			)
		{
			if (auto const pTrySubmitThreadpoolCallback = try_get_TrySubmitThreadpoolCallback())
			{
				return pTrySubmitThreadpoolCallback(Callback, Context, CallbackEnviron);
			}


			auto lStatus = Fallback::TpSimpleTryPost(Callback, Context, CallbackEnviron);
			
			if (lStatus >= 0)
			{
				return TRUE;
			}
			else
			{
				internal::NtStatusToDosError(lStatus);
				return FALSE;
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		12,
		PTP_WAIT,
		WINAPI,
		CreateThreadpoolWait,
			_In_ PTP_WAIT_CALLBACK Callback,
			_Inout_opt_ PVOID Context,
			_In_opt_ PTP_CALLBACK_ENVIRON CallbackEnviron
			)
		{
			if (auto const pCreateThreadpoolWait = try_get_CreateThreadpoolWait())
			{
				return pCreateThreadpoolWait(Callback, Context, CallbackEnviron);
			}


			auto Flags = CallbackEnviron ? CallbackEnviron->u.Flags : 0;

			if (Callback == nullptr || (Flags & 0xFFFFFFFE) != 0)
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return nullptr;
			}

			NTSTATUS Status = STATUS_SUCCESS;

			const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
			auto pWait = (_TP_WAIT*)HeapAlloc(ProcessHeap, HEAP_ZERO_MEMORY, sizeof(_TP_WAIT));

			do
			{
				if (!pWait)
				{
					Status = STATUS_NO_MEMORY;
					break;
				}

	
				pWait->retaddr = _ReturnAddress();


				Status = Fallback::TppWorkInitialize(pWait, Context, CallbackEnviron, Flags | 0x40000, Fallback::GetTppWaitpCleanupGroupMemberVFuncs(), Fallback::GetTppWaitpTaskVFuncs());

				if (Status < 0)
					break;

				pWait->Callback = Callback;

				if (CallbackEnviron)
					pWait->FinalizationCallback = CallbackEnviron->FinalizationCallback;

				if (pWait->CleanupGroup)
					Fallback::TppCleanupGroupAddMember(pWait);

				return pWait;

			} while (false);

			if(pWait)
				HeapFree(ProcessHeap, 0, pWait);

			internal::NtStatusToDosError(Status);

			return nullptr;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		CloseThreadpoolWait,
			_Inout_ PTP_WAIT Wait
			)
		{
			if (auto const pCloseThreadpoolWait = try_get_CloseThreadpoolWait())
			{
				return pCloseThreadpoolWait(Wait);
			}

			if (Wait == nullptr || (Wait->uFlags1 & 0x10000) || Wait->VFuncs != Fallback::GetTppWaitpCleanupGroupMemberVFuncs())
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

			if (Fallback::TppCleanupGroupMemberRelease(Wait, true))
			{
				Wait->un58 = _ReturnAddress();

				if (auto hOrgWaitObject = InterlockedExchangePointer(&Wait->hWaitObject, nullptr))
				{
					UnregisterWait(hOrgWaitObject);

					if (InterlockedExchangeAdd(&Wait->nRef, -1) == 0)
					{
						Wait->VFuncs->pfnTppWorkpFree(Wait);
					}
				}

				if (InterlockedExchangeAdd(&Wait->nRef, -1) == 0)
				{
					Wait->VFuncs->pfnTppWorkpFree(Wait);
				}
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		12,
		VOID,
		WINAPI,
		SetThreadpoolWait,
			_Inout_ PTP_WAIT Wait,
			_In_opt_ HANDLE Handle,
			_In_opt_ PFILETIME Timeout
			)
		{
			if (auto const pSetThreadpoolWait = try_get_SetThreadpoolWait())
			{
				return pSetThreadpoolWait(Wait, Handle, Timeout);
			}

			if (Wait == nullptr || (Wait->uFlags1 & 0x30000) || Wait->VFuncs != Fallback::GetTppWaitpCleanupGroupMemberVFuncs())
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

			if (Handle == nullptr)
			{
				//停止
				if (auto hOrgWaitObject = InterlockedExchangePointer(&Wait->hWaitObject, nullptr))
				{
                    UnregisterWaitEx(hOrgWaitObject, INVALID_HANDLE_VALUE);
                    SetEvent(Wait->hEvent);
                    Wait->Release();
				}

				return;
			}

			ULONG dwMilliseconds;

			if (Timeout == nullptr)
			{
				dwMilliseconds = INFINITE;
			}
			else
			{
				LARGE_INTEGER Time;
				Time.LowPart = Timeout->dwLowDateTime;
				Time.HighPart = Timeout->dwHighDateTime;

				if (Time.QuadPart < 0)
				{
					//这是一个相对时间
					dwMilliseconds = static_cast<ULONG>(Time.QuadPart / -10'000);
				}
				else if (Time.QuadPart == 0)
				{
					dwMilliseconds = 0;
				}
				else
				{
					//绝对时间
					FILETIME CurrentTime;
					GetSystemTimeAsFileTime(&CurrentTime);
					LARGE_INTEGER lCurrentTime;
					lCurrentTime.LowPart = CurrentTime.dwLowDateTime;
					lCurrentTime.HighPart = CurrentTime.dwHighDateTime;

					if (Time.QuadPart > lCurrentTime.QuadPart)
					{
						dwMilliseconds = static_cast<ULONG>((Time.QuadPart - lCurrentTime.QuadPart) / 10'000);
					}
					else
					{
						dwMilliseconds = 0;
					}
				}
			}

            Wait->AddRef();

            // Wait 比较特殊，因为始终只有一个句柄，hEvent不需要引用计数。
            ::AcquireSRWLockExclusive(&Wait->WaitObjectLock);
            if (auto hOrgWaitObject = InterlockedExchangePointer(&Wait->hWaitObject, nullptr))
            {
                UnregisterWaitEx(hOrgWaitObject, INVALID_HANDLE_VALUE);
                
                Wait->Release();
            }
            ResetEvent(Wait->hEvent);
			auto bRet = RegisterWaitForSingleObject(
				&Wait->hWaitObject,
				Handle,
				[](
					PVOID lpParameter,
					BOOLEAN TimerOrWaitFired
					)
				{
					auto Wait = (PTP_WAIT)lpParameter;

                    // 准备开始调用，增加一次Working计数
                    TP_CALLBACK_INSTANCE Instance = {};

                    //Timer->pTaskVFuncs->pExecuteCallback(&Instance, Timer);
                    //暂时这样硬编码……
                    ((PTP_WAIT_CALLBACK)Wait->Callback)(&Instance, Wait->Context, Wait, TimerOrWaitFired ? WAIT_TIMEOUT : WAIT_OBJECT_0);

                    Fallback::DoWhenCallbackReturns(&Instance);

                    SetEvent(Wait->hEvent);
				},
				Wait,
				dwMilliseconds,
				WT_EXECUTEONLYONCE);
            ::ReleaseSRWLockExclusive(&Wait->WaitObjectLock);

			if (!bRet)
			{
                SetEvent(Wait->hEvent);
                Wait->Release();
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		VOID,
		WINAPI,
		WaitForThreadpoolWaitCallbacks,
			_Inout_ PTP_WAIT Wait,
			_In_ BOOL CancelPendingCallbacks
			)
		{
			if (auto const pWaitForThreadpoolWaitCallbacks = try_get_WaitForThreadpoolWaitCallbacks())
			{
				return pWaitForThreadpoolWaitCallbacks(Wait, CancelPendingCallbacks);
			}

			if (Wait == nullptr
				|| (Wait->uFlags1 & 0x30000)
				|| Wait->VFuncs != Fallback::GetTppWaitpCleanupGroupMemberVFuncs())
			{
				internal::RaiseStatus(STATUS_INVALID_PARAMETER);
				return;
			}

            if (CancelPendingCallbacks)
            {
                if (auto hOrgWaitObject = InterlockedExchangePointer(&Wait->hWaitObject, nullptr))
                {
                    UnregisterWaitEx(hOrgWaitObject, INVALID_HANDLE_VALUE);
                    SetEvent(Wait->hEvent);
                    Wait->Release();
                }
            }
            else
            {
                if (Wait->hWaitObject)
                {
                    WaitForSingleObject(Wait->hEvent, INFINITE);
                }
            }
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
        // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		kernel32,
		16,
		PTP_IO,
        WINAPI,
        CreateThreadpoolIo,
            _In_ HANDLE _hFile,
            _In_ PTP_WIN32_IO_CALLBACK _pfnIoCallback,
            _Inout_opt_ PVOID _pUserData,
            _In_opt_ PTP_CALLBACK_ENVIRON _pCallbackEnviron
            )
		{
			if (auto const _pfnCreateThreadpoolIo = try_get_CreateThreadpoolIo())
			{
				return _pfnCreateThreadpoolIo(_hFile, _pfnIoCallback, _pUserData, _pCallbackEnviron);
			}
            
            /////////////////////////////////////////////////////////
            // TpAllocIoCompletion
            if (_hFile == NULL || _pfnIoCallback == NULL)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return nullptr;
            }

            auto _pThunkData = internal::HookThunkData::Alloc();
            if (!_pThunkData)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                return nullptr;
            }

            auto _pIo = (_TP_IO*)internal::Alloc(sizeof(_TP_IO), HEAP_ZERO_MEMORY);
            if (!_pIo)
                return nullptr;

            _pIo->retaddr = _ReturnAddress();
            LONG _Status = Fallback::TppCleanupGroupMemberInitialize(_pIo, _pUserData, _pCallbackEnviron, _pCallbackEnviron ? _pCallbackEnviron->u.Flags : 0, Fallback::GetTppIopCleanupGroupMemberVFuncs());
            if (_Status >= 0)
            {
                _pIo->pTaskVFuncs = Fallback::GetTppIopExecuteCallback();
                _pIo->Callback = _pfnIoCallback;
                _pIo->hFile = _hFile;

                // void WINAPI IoCompletionCallbackWinXP(DWORD _lStatus, DWORD _cbNumberOfBytesTransfered, LPOVERLAPPED _pOverlapped)
                // {
                //    // fastcall
                //     _pIo->pTaskVFuncs->pExecuteCallback(_pIo, _lStatus, _cbNumberOfBytesTransfered, _pOverlapped);
                // }
#if defined(_X86_)
                constexpr auto kOffsetToTpIo = 1;
                static const unsigned char kIoCompletionCallbackWinXPShellCode[] =
                {
                    // _lStatus[ebp + 4]
                    // _cbNumberOfBytesTransfered[ebp + 8]
                    // _pOverlapped[ebp + 0xC]
                    0xB9, 0x00, 0x00, 0x00, 0x00, // mov         ecx,00000000h; _pIo
                    0xFF, 0x74, 0x24, 0x0C,       // push        dword ptr[esp + 0Ch] ; _pOverlapped
                    0x8B, 0x54, 0x24, 0x08,       // mov         edx,dword ptr[esp + 8] ;_lStatus
                    0xFF, 0x74, 0x24, 0x0C,       // push        dword ptr[esp + 0Ch] ; _cbNumberOfBytesTransfered
                    0x8B, 0x41, 0x60,             // mov         eax,dword ptr[ecx + 60h] ; _pIo->pTaskVFuncs->pExecuteCallback
                    0xFF, 0x10,                   // call        dword ptr[eax]
                    0xC2, 0x0C, 0x00,             // ret         0Ch
                };
#elif defined(_AMD64_)
                constexpr auto kOffsetToTpIo = 10;
                static const unsigned char kIoCompletionCallbackWinXPShellCode[] =
                {
                    // _lStatus : ecx
                    // _cbNumberOfBytesTransfered : edx
                    // _pOverlapped : r8
                    0x4D, 0x8B, 0xC8,                                           // mov         r9,r8 ; _pOverlapped
                    0x44, 0x8B, 0xC2,                                           // mov         r8d,edx ; _cbNumberOfBytesTransfered
                    0x8B, 0xD1,                                                 // mov         edx,ecx ; _lStatus
                    0x48, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov         rcx,0000000000000000h ; _pIo
                    0x48, 0x8B, 0x81, 0xB0, 0x00, 0x00, 0x00,                   // mov         rax,qword ptr[rcx + 0B0h] ; _pIo->pTaskVFuncs->pExecuteCallback
                    0x48, 0x8B, 0x00,                                           // mov         rax,qword ptr[rax]
                    0xFF, 0xE0,                                                 // jmp         rax
                };
#else
#error "不应该走到这里。"
#endif
                static_assert(sizeof(kIoCompletionCallbackWinXPShellCode) <= sizeof(_pThunkData->ShellCode), "");
                memcpy(_pThunkData->ShellCode, kIoCompletionCallbackWinXPShellCode, sizeof(kIoCompletionCallbackWinXPShellCode));
                *(_TP_IO**)(_pThunkData->ShellCode + kOffsetToTpIo) = _pIo;
                _pIo->pThunkData = _pThunkData;

                if (_pCallbackEnviron)
                {
                    _pIo->FinalizationCallback = _pCallbackEnviron->FinalizationCallback;
                }

                if (_pIo->CleanupGroup)
                {
                    Fallback::TppCleanupGroupAddMember(_pIo);
                }

                if (!BindIoCompletionCallback(_pIo->hFile,
                    reinterpret_cast<LPOVERLAPPED_COMPLETION_ROUTINE>(_pIo->pThunkData), 0))
                {
                    _pIo->Release();
                    return nullptr;
                }
            }
            //
            ////////////////////////////////////////////////////////

            if (_Status >= 0)
            {
                return _pIo;
            }
            // pThunkData

            _pIo->Release();
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return nullptr;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
        // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
        WINAPI,
        StartThreadpoolIo,
            _Inout_ PTP_IO _pIo
            )
		{
			if (auto const _pfnStartThreadpoolIo = try_get_StartThreadpoolIo())
			{
				return _pfnStartThreadpoolIo(_pIo);
			}

            _pIo->AddRef();
            _pIo->AddPendingCount();
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
        // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
        WINAPI,
        CancelThreadpoolIo,
            _Inout_ PTP_IO _pIo
            )
		{
			if (auto const _pfnCancelThreadpoolIo = try_get_CancelThreadpoolIo())
			{
				return _pfnCancelThreadpoolIo(_pIo);
			}
            
            _pIo->TryReleasePendingount();
            _pIo->Release();
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
        // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
        WINAPI,
        CloseThreadpoolIo,
            _Inout_ PTP_IO _pIo
            )
		{
			if (auto const _pfnCloseThreadpoolIo = try_get_CloseThreadpoolIo())
			{
				return _pfnCloseThreadpoolIo(_pIo);
			}
            // TpReleaseIoCompletion

            if (Fallback::TppCleanupGroupMemberRelease(_pIo, 1))
            {
                _pIo->un58 = _ReturnAddress();
                _pIo->Release();
            }
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
        // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		kernel32,
		8,
		VOID,
        WINAPI,
        WaitForThreadpoolIoCallbacks,
            _Inout_ PTP_IO _pIo,
            _In_ BOOL _bCancelPendingCallbacks
            )
		{
			if (auto const _pfnWaitForThreadpoolIoCallbacks = try_get_WaitForThreadpoolIoCallbacks())
			{
				return _pfnWaitForThreadpoolIoCallbacks(_pIo, _bCancelPendingCallbacks);
			}
            
            // TpWaitForIoCompletion
            if (_pIo == nullptr || (_pIo->uFlags1 & 0x30000) || _pIo->VFuncs != Fallback::GetTppIopCleanupGroupMemberVFuncs())
            {
                internal::RaiseStatus(STATUS_INVALID_PARAMETER);
                return;
            }
            
            Fallback::TppWorkWait(_pIo, _bCancelPendingCallbacks, true);
		}
#endif
	}
}
