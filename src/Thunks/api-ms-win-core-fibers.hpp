

namespace YY::Thunks
{
#if defined(YY_Thunks_Implemented) && (YY_Thunks_Support_Version < NTDDI_WS03)
    namespace Fallback
    {
        namespace
        {
            //Vista的Fls数量只有128，所以我们将长度固定为128，模拟Vista
            constexpr auto kMaxFlsIndexCount = 128;

            struct FlsData
            {
                FlsData* pNext;
                FlsData* pPrev;

                //此线程存储的数据
                LPVOID lpFlsData;
            };

            struct FlsDataBlock
            {
                FlsData FlsDataArray[kMaxFlsIndexCount] = {};
                DWORD uRef = 1;

                void AddRef()
                {
                    InterlockedIncrement(&uRef);
                }

                void Release()
                {
                    if (InterlockedDecrement(&uRef) == 0)
                    {
                        internal::Delete(this);
                    }
                }
            };

            static thread_local FlsDataBlock* s_pFlsDataBlock;

            struct FlsIndex
            {
                volatile LONG fFlags;
                //FlsAlloc 传入的参数
                volatile PFLS_CALLBACK_FUNCTION pfnCallback;
                FlsData Root;

                enum
                {
                    TlsUsedBitIndex = 0,
                    DataLockBitIndex,
                };
            };
            static FlsIndex s_FlsIndexArray[kMaxFlsIndexCount];
            static DWORD s_FlsIndexArrayBitMap[kMaxFlsIndexCount / 32];
            static_assert(sizeof(s_FlsIndexArrayBitMap) * 8 == kMaxFlsIndexCount, "");

            static VOID NTAPI ThreadDetachCallback(
                PVOID DllHandle,
                DWORD Reason,
                PVOID Reserved
                )
            {
                if (DLL_THREAD_DETACH == Reason && s_pFlsDataBlock)
                {
                    //线程退出时我们需要调用Fls的 Callback
                    for (DWORD i = 0; i != kMaxFlsIndexCount; ++i)
                    {
                        auto& _FlsData = s_pFlsDataBlock->FlsDataArray[i];
                        if (/*_FlsData.pPrev == nullptr ||*/ _FlsData.lpFlsData == nullptr)
                            continue;

                        auto& _FlsIndex = s_FlsIndexArray[i];

                        for (auto _fLastFlags = _FlsIndex.fFlags; _FlsIndex.pfnCallback;)
                        {
                            if ((_fLastFlags & (1 << FlsIndex::TlsUsedBitIndex)) == 0)
                            {
                                break;
                            }

                            if (_fLastFlags & (1 << FlsIndex::DataLockBitIndex))
                            {
                                YieldProcessor();
                                _fLastFlags = _FlsIndex.fFlags;
                                continue;
                            }

                            auto _Result = InterlockedCompareExchange(&_FlsIndex.fFlags, _fLastFlags | (1 << FlsIndex::DataLockBitIndex), _fLastFlags);
                            if (_Result == _fLastFlags)
                            {
                                auto pPrev = _FlsData.pPrev;
                                auto pNext = _FlsData.pNext;
                                _FlsData.pNext = nullptr;
                                _FlsData.pPrev = nullptr;
                                if (pNext)
                                {
                                    pNext->pPrev = pPrev;
                                }
                                pPrev->pNext = pNext;

                                auto _pfnCallback = _FlsIndex.pfnCallback;
                                _interlockedbittestandreset(&_FlsIndex.fFlags, FlsIndex::DataLockBitIndex);

                                __try
                                {
                                    if (_pfnCallback && _FlsData.lpFlsData)
                                    {
                                        _pfnCallback(_FlsData.lpFlsData);
                                    }
                                }
                                __except (EXCEPTION_EXECUTE_HANDLER)
                                {
                                }
                                s_pFlsDataBlock->Release();
                                break;
                            }

                            _fLastFlags = _Result;
                        }
                    }

                    s_pFlsDataBlock->Release();
                }
            }

#pragma section(".CRT$XLY",    long, read) // MS CRT Loader TLS Callback
            extern "C" extern bool _tls_used;
        }
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
		__declspec(allocate(".CRT$XLY")) static PIMAGE_TLS_CALLBACK const s_ThreadDetachCallback = Fallback::ThreadDetachCallback;

		__foreinclude(s_ThreadDetachCallback);
		__foreinclude(Fallback::_tls_used);

        DWORD _uFlsIndex;
        for (size_t i = 0; i != _countof(Fallback::s_FlsIndexArrayBitMap); ++i)
        {
            if (BitScanForward(&_uFlsIndex, ~Fallback::s_FlsIndexArrayBitMap[i]))
            {
                if (_interlockedbittestandset((volatile LONG*)&Fallback::s_FlsIndexArrayBitMap[i], _uFlsIndex) == 0)
                {
                    _uFlsIndex += i * 32;
                    auto& _FlsIndex = Fallback::s_FlsIndexArray[_uFlsIndex];

                    _FlsIndex.fFlags = (1 << Fallback::FlsIndex::TlsUsedBitIndex);
                    _FlsIndex.pfnCallback = lpCallback;
                    _FlsIndex.Root.pNext = nullptr;
                    return _uFlsIndex;
                }
            }
        }

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
            
        if (dwFlsIndex >= Fallback::kMaxFlsIndexCount || Fallback::s_pFlsDataBlock == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return nullptr;
        }

		return Fallback::s_pFlsDataBlock->FlsDataArray[dwFlsIndex].lpFlsData;
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
            if (dwFlsIndex >= Fallback::kMaxFlsIndexCount)
                break;

            if (!Fallback::s_pFlsDataBlock)
            {
                if (!lpFlsData)
                    return TRUE;

                auto _pFlsDataBlock = internal::New<Fallback::FlsDataBlock>();
                if (!_pFlsDataBlock)
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    return FALSE;
                }
                Fallback::s_pFlsDataBlock = _pFlsDataBlock;
            }

            auto& _FlsData = Fallback::s_pFlsDataBlock->FlsDataArray[dwFlsIndex];
            if (_FlsData.pPrev == nullptr && lpFlsData == nullptr)
            {
                // 值为空时不用特意添加到Fls列表
                return TRUE;
            }
            auto& _FlsIndex = Fallback::s_FlsIndexArray[dwFlsIndex];

            if (_FlsData.pPrev)
            {
                InterlockedExchange((uintptr_t*)&_FlsData.lpFlsData, uintptr_t(lpFlsData));
                return TRUE;
            }
            else
            {
                Fallback::s_pFlsDataBlock->AddRef();
                for (auto _fLastFlags = _FlsIndex.fFlags;;)
                {
                    if ((_fLastFlags & (1 << Fallback::FlsIndex::TlsUsedBitIndex)) == 0)
                    {
                        break;
                    }

                    if (_fLastFlags & (1 << Fallback::FlsIndex::DataLockBitIndex))
                    {
                        YieldProcessor();
                        _fLastFlags = _FlsIndex.fFlags;
                        continue;
                    }

                    auto _Result = InterlockedCompareExchange(&_FlsIndex.fFlags, _fLastFlags | (1 << Fallback::FlsIndex::DataLockBitIndex), _fLastFlags);
                    if (_Result == _fLastFlags)
                    {
                        _FlsData.lpFlsData = lpFlsData;
                        /*
                        _FlsIndex.Root  <-->  _FlsData  <--> _pNext
                        */
                        auto _pNext = _FlsIndex.Root.pNext;
                        _FlsData.pNext = _pNext;
                        _FlsData.pPrev = &_FlsIndex.Root;

                        _FlsIndex.Root.pNext = &_FlsData;
                        if(_pNext)
                            _pNext->pPrev = &_FlsData;

                        _interlockedbittestandreset(&_FlsIndex.fFlags, Fallback::FlsIndex::DataLockBitIndex);
                        return TRUE;
                        break;
                    }
                    _fLastFlags = _Result;
                }
                Fallback::s_pFlsDataBlock->Release();
                break;
            }
            
		} while (false);

		SetLastError(ERROR_INVALID_PARAMETER);
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

        if (dwFlsIndex >= _countof(Fallback::s_FlsIndexArray))
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        auto& _FlsIndex = Fallback::s_FlsIndexArray[dwFlsIndex];
        // 标记为删除，让后续Set过程全部失效。
        if (_interlockedbittestandreset(&_FlsIndex.fFlags, Fallback::FlsIndex::TlsUsedBitIndex) == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        for (;;)
        {
            if (!_interlockedbittestandreset(&_FlsIndex.fFlags, Fallback::FlsIndex::DataLockBitIndex))
            {
                break;
            }
            YieldProcessor();
        }

        auto _pFlsData = (Fallback::FlsData*)InterlockedExchange((uintptr_t*)&_FlsIndex.Root.pNext, 0);
        while (_pFlsData)
        {
            auto _pNext = _pFlsData->pNext;
            _pFlsData->pNext = nullptr;
            _pFlsData->pPrev = nullptr;
            __try
            {
                if (_pFlsData->lpFlsData && _FlsIndex.pfnCallback)
                    _FlsIndex.pfnCallback(_pFlsData->lpFlsData);
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
            }
            _pFlsData->lpFlsData = nullptr;
            ((Fallback::FlsDataBlock*)(_pFlsData - dwFlsIndex))->Release();
            _pFlsData = _pNext;
        }

        _interlockedbittestandreset((volatile LONG*)&Fallback::s_FlsIndexArrayBitMap[dwFlsIndex / 32], dwFlsIndex % 32);
        return TRUE;
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
