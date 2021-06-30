

namespace YY
{
	namespace Thunks
	{
#if defined(YY_Thunks_Implemented) && (YY_Thunks_Support_Version < NTDDI_WS03)
		namespace Downlevel
		{
			struct FlsData
			{
				//所属线程ID
				DWORD dwThreadId;
				//此线程存储的数据
				LPVOID lpFlsData;
			};

			enum FlsStatus
			{
				//此节点是自由的
				FlsStatusFree     = 0x00000000,
				//正在使用
				FlsStatusUsing    = 0x00000001,
				//正在初始化
				FlsStatusInit     = 0x00000002,
				//FlsFree已经调用，引用归零时为我自动释放
				FlsStatusAutoFree = 0x00000004,
			};

			struct FlsIndex
			{
				
				union
				{
					struct
					{
						FlsStatus Status : 3;
#ifdef _WIN64
						//访问计数
						DWORD_PTR nRef : 61;
#else
						//访问计数
						DWORD_PTR nRef : 29;
#endif
					};

					volatile DWORD_PTR RawValue;
				};

				//FlsAlloc 传入的参数
				PFLS_CALLBACK_FUNCTION lpCallback;

				//长度uFlsDataCount，我们做一个桶，分成128组，每组128个元素
				static constexpr int uFlsDataCount = 0x4000;
				FlsData* pFlsDataArray;
				
			};

			//尝试锁定
			static
			bool
			__fastcall
			TryAddSharedLock(FlsIndex* pIndex)
			{
				for (auto RawValue = pIndex->RawValue;;)
				{
					if ((RawValue & (FlsStatusUsing | FlsStatusInit | FlsStatusAutoFree)) != FlsStatusUsing)
					{
						return false;
					}

					//增加一次引用计数
					auto OrgRawValue = InterlockedCompareExchange(&pIndex->RawValue, RawValue + 8, RawValue);

					if (OrgRawValue == RawValue)
					{
						//更新成功！
						break;
					}
					//锁定失败，重新尝试锁定
					RawValue = OrgRawValue;
				}

				return true;
			}

			static
			void
			__fastcall
			ReleaseSharedLock(FlsIndex* pIndex)
			{
				auto RawValue = pIndex->RawValue;

				for (;;)
				{
					//减少一次引用计数
					auto OrgRawValue = InterlockedCompareExchange(&pIndex->RawValue, RawValue - 8, RawValue);

					if (OrgRawValue == RawValue)
					{
						//更新成功！
						break;
					}
					//锁定失败，重新尝试锁定
					RawValue = OrgRawValue;
				}


				//引用计数归零，并且 存在 FlsStatusAutoFree标志，那么自动释放此节点
				if (RawValue == (8 | FlsStatusUsing | FlsStatusAutoFree))
				{
					auto lpCallback = pIndex->lpCallback;

					auto pFlsDataArray = (FlsData*)InterlockedExchangePointer((PVOID*)&pIndex->pFlsDataArray, nullptr);

					//将状态调整为 Free
					InterlockedExchange((unsigned long long*) &pIndex->RawValue, FlsStatusFree);

					if (pFlsDataArray)
					{
						if (lpCallback)
						{
							for (auto pItem = pFlsDataArray, pItemEnd = pItem + FlsIndex::uFlsDataCount; pItem != pItemEnd; ++pItem)
							{
								if (pItem->dwThreadId && pItem->lpFlsData)
								{
									lpCallback(pItem->lpFlsData);
								}
							}
						}

						const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
						HeapFree(ProcessHeap, 0, pFlsDataArray);
					}
				}
			}


			static FlsData* GetFlsData(FlsData* pFlsDataArray, bool bCanAlloc = false)
			{
				const auto dwCurrentThreadId = GetCurrentThreadId();

				//我们做一个桶，分成128组，每组128个元素
				auto pFlsDataGroup = &pFlsDataArray[(dwCurrentThreadId >> 1) & 0x7F];

				for (int i = 0; i != 128; ++i)
				{
					if (pFlsDataGroup[i].dwThreadId == dwCurrentThreadId)
					{
						return &pFlsDataGroup[i];
					}
				}

				//因为在调用者线程运行，所以这是线程安全的，上面没有找到，那么现在必然没有。
				if (bCanAlloc)
				{
					for (int i = 0; i != 128; ++i)
					{
						//尝试搜索一个空闲的位置
						auto dwOrgThreadId = InterlockedCompareExchange(&pFlsDataGroup[i].dwThreadId, dwCurrentThreadId, 0);

						if (dwOrgThreadId == 0)
						{
							pFlsDataGroup[i].lpFlsData = nullptr;
							return &pFlsDataGroup[i];
						}
					}
				}

				return nullptr;
			}


			static auto& __fastcall GetFlsIndexArray()
			{
				//Vista的Fls数量只有128，所以我们将长度固定为128，模拟Vista
				static FlsIndex FlsIndexArray[128];

				return FlsIndexArray;
			}


			static
			VOID
			NTAPI
			FlsDownlevelCallback(
				PVOID DllHandle,
				DWORD Reason,
				PVOID Reserved
				)
			{



				if (DLL_THREAD_DETACH == Reason)
				{
					//线程退出时我们需要调用Fls的 Callback


					

					for (auto& Item : Downlevel::GetFlsIndexArray())
					{
						PFLS_CALLBACK_FUNCTION lpCallback;

						//锁定并且检测是否存在  Callback
						if(internal::Block([&]() 
							{
								static_assert(sizeof(void*) == 4, "这里的代码只支持 32位哦。");

								auto CurrnetValue = *(unsigned long long*) & Item.RawValue;

								for (;;)
								{
									//未初始化 或者 正在初始化或者释放时，直接跳过
									if ((CurrnetValue & (FlsStatusUsing | FlsStatusInit | FlsStatusAutoFree)) != FlsStatusUsing)
										return false;

									//高32位是 lpCallback，如果为 null，则说明没有设置 lpCallback
									//既然都没设置，那就没有必要 回调
									lpCallback = (PFLS_CALLBACK_FUNCTION)(CurrnetValue >> 32);
									if (lpCallback == nullptr)
										return false;


									//尝试增加一次读取引用计数，避免突然被释放
									auto OrgRawValue = InterlockedCompareExchange((unsigned long long*) &Item.RawValue, CurrnetValue + 8ll, CurrnetValue);

									if (OrgRawValue == CurrnetValue)
									{
										break;
									}

									//锁定失败，重新尝试锁定
									CurrnetValue = OrgRawValue;
								}

								return true;
							}))
						{
							LPVOID lpFlsData = nullptr;

							if (auto pFls = GetFlsData(Item.pFlsDataArray, false))
							{
								//FlsFree 时可能释放 lpFlsData，因此这里我们进行线程安全访问
								lpFlsData = InterlockedExchangePointer(&pFls->lpFlsData, nullptr);
							}

							//重新减少引用计数
							ReleaseSharedLock(&Item);

							//调用回调函数
							if (lpFlsData)
								lpCallback(lpFlsData);
						}
					}
				}
			}

			#pragma section(".CRT$XLB",    long, read) // MS CRT Loader TLS Callback

			extern "C" extern bool _tls_used;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		DWORD,
		WINAPI,
		FlsAlloc,
			_In_opt_ PFLS_CALLBACK_FUNCTION lpCallback
			)
		{
			if (const auto pFlsAlloc = try_get_FlsAlloc())
			{
				return pFlsAlloc(lpCallback);
			}

			//当系统不支持时，我们使用TLS的机制来实现FLS的功能。
			//当然这个模拟的不完整的，无法完全的支持纤程，但是我认为这不重要。
			//因为很显然，使用纤程的代码很少。


			__declspec(allocate(".CRT$XLB")) static PIMAGE_TLS_CALLBACK __FLS_DOWNLEVEL_CALLBACK = Downlevel::FlsDownlevelCallback;

			__foreinclude(__FLS_DOWNLEVEL_CALLBACK);
			__foreinclude(Downlevel::_tls_used);

			const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;

			auto& FlsIndexArray = Downlevel::GetFlsIndexArray();


			for (auto& Item : FlsIndexArray)
			{
				auto RawValue = Item.RawValue;

				for (;;)
				{
					//正在使用的我们需要跳过
					if (RawValue & Downlevel::FlsStatusUsing)
					{
						break;
					}

					//尝试锁定Index
					auto OrgRawValue = InterlockedCompareExchange(&Item.RawValue, Downlevel::FlsStatusUsing | Downlevel::FlsStatusInit, RawValue);

					if (OrgRawValue != RawValue)
					{
						//锁定失败，重新尝试锁定
						RawValue = OrgRawValue;
						continue;
					}

					//锁定成功
					auto pFlsDataArray = (Downlevel::FlsData*)HeapAlloc(ProcessHeap, HEAP_ZERO_MEMORY, sizeof(Downlevel::FlsData) * Downlevel::FlsIndex::uFlsDataCount);
					
					if (pFlsDataArray)
					{
						Item.pFlsDataArray = pFlsDataArray;
						Item.lpCallback = lpCallback;

						InterlockedExchange(&Item.RawValue, Downlevel::FlsStatusUsing);

						return &Item - &FlsIndexArray[0];
					}


					//内存申请失败，我们回滚更改
					InterlockedExchange(&Item.RawValue, Downlevel::FlsStatusFree);
					
					goto Failed;
				}
			}


		Failed:
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return FLS_OUT_OF_INDEXES;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		PVOID,
		WINAPI,
		FlsGetValue,
			_In_ DWORD dwFlsIndex
			)
		{
			if (const auto pFlsGetValue = try_get_FlsGetValue())
			{
				return pFlsGetValue(dwFlsIndex);
			}


			do
			{
				auto& FlsIndexArray = Downlevel::GetFlsIndexArray();

				if (_countof(FlsIndexArray) <= dwFlsIndex)
				{
					break;
				}

				auto& Item = FlsIndexArray[dwFlsIndex];

				if (!Downlevel::TryAddSharedLock(&Item))
					break;

				LPVOID lpFlsData = nullptr;

				if (auto pFls = Downlevel::GetFlsData(Item.pFlsDataArray, false))
					lpFlsData = pFls->lpFlsData;

				Downlevel::ReleaseSharedLock(&Item);

				return lpFlsData;

			} while (false);


			SetLastError(ERROR_INVALID_PARAMETER);
			return nullptr;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		BOOL,
		WINAPI,
		FlsSetValue,
			_In_ DWORD dwFlsIndex,
			_In_opt_ PVOID lpFlsData
			)
		{
			if (const auto pFlsSetValue = try_get_FlsSetValue())
			{
				return pFlsSetValue(dwFlsIndex, lpFlsData);
			}


			do
			{
				auto& FlsIndexArray = Downlevel::GetFlsIndexArray();

				if (_countof(FlsIndexArray) <= dwFlsIndex)
					break;

				auto& Item = FlsIndexArray[dwFlsIndex];

				if (!Downlevel::TryAddSharedLock(&Item))
					break;

				auto pFls = Downlevel::GetFlsData(Item.pFlsDataArray, true);
				if (pFls)
					pFls->lpFlsData = lpFlsData;

				Downlevel::ReleaseSharedLock(&Item);

				if (pFls)
					return TRUE;

			} while (false);



			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return FALSE;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		BOOL,
		WINAPI,
		FlsFree,
			_In_ DWORD dwFlsIndex
			)
		{
			if (const auto pFlsFree = try_get_FlsFree())
			{
				return pFlsFree(dwFlsIndex);
			}

			do
			{
				auto& FlsIndexArray = Downlevel::GetFlsIndexArray();

				if (_countof(FlsIndexArray) <= dwFlsIndex)
				{
					break;
				}

				auto& Item = FlsIndexArray[dwFlsIndex];


				//加入 FlsStatusInitOrUninit 标记
				if (internal::Block([&]()
					{
						for (auto RawValue = Item.RawValue;;)
						{
							if ((RawValue & (Downlevel::FlsStatusUsing | Downlevel::FlsStatusInit | Downlevel::FlsStatusAutoFree)) != Downlevel::FlsStatusUsing)
							{
								return false;
							}

							//增加一次引用计数，并且标记 FlsFree 已经调用
							auto OrgRawValue = InterlockedCompareExchange(&Item.RawValue, RawValue + (8 | Downlevel::FlsStatusAutoFree), RawValue);

							if (OrgRawValue == RawValue)
							{
								//更新成功！
								break;
							}
							//锁定失败，重新尝试锁定
							RawValue = OrgRawValue;
						}

						return true;
					}))
				{
					Downlevel::ReleaseSharedLock(&Item);
					return TRUE;
				};

			} while (false);


			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;

		}
#endif

		
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		0,
		BOOL,
		WINAPI,
		IsThreadAFiber,
			VOID
			)
		{
			if (const auto pIsThreadAFiber = try_get_IsThreadAFiber())
			{
				return pIsThreadAFiber();
			}

			//如果当前没有 Fiber，那么我们认为这不是一个纤程
			auto pFiber = GetCurrentFiber();

			//0x1e00 是一个魔幻数字，似乎所有NT系统都会这样，当前不是一个Fiber时，第一次会返回 0x1e00。
			return pFiber != nullptr && pFiber != (void*)0x1e00;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		LPVOID,
		WINAPI,
		ConvertThreadToFiberEx,
			_In_opt_ LPVOID lpParameter,
			_In_     DWORD dwFlags
			)
		{
			if (const auto pConvertThreadToFiberEx = try_get_ConvertThreadToFiberEx())
			{
				return pConvertThreadToFiberEx(lpParameter, dwFlags);
			}

			//FIBER_FLAG_FLOAT_SWITCH 无法使用，不过似乎关系不大。一些boost基础设施中都不切换浮点状态。
			return ConvertThreadToFiber(lpParameter);
		}
#endif
	}
}