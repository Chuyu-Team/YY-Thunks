#if defined(YY_Thunks_Implemented)

namespace YY::Thunks::internal
{
    extern "C" BOOL WINAPI _DllMainCRTStartup(
        HINSTANCE const instance,
        DWORD     const reason,
        LPVOID    const reserved
        );

    // 如果希望自定义函数入口点，可以设置此函数
    extern "C" extern decltype(_DllMainCRTStartup)* const __pfnDllMainCRTStartupForYY_Thunks;

#if YY_Thunks_Target < __WindowsNT6
    extern "C" ULONG _tls_index;
    static ULONG _tls_index_old;
#ifdef _WIN64
    extern "C" const IMAGE_TLS_DIRECTORY64 _tls_used;
#else
    extern "C" const IMAGE_TLS_DIRECTORY _tls_used;
#endif

#pragma section(".CRT$XLB",    long, read) // First MS CRT Loader TLS Callback

    enum class TlsMode
    {
        None,
        ByFirstCallback,
        ByDllMainCRTStartupForYY_Thunks,
    };

    // 用来记录第一次Tls回调的时机
    // 如果来自ByFirstCallback，那么说明当前模块的TLS功能完全正常（模块是EXE或者来自于隐式依赖的DLL），YY-Thunks无需额外修复
    // 如果来自ByDllMainCRTStartupForYY_Thunks，那么说明它是动态加载的DLL，TLS没有初始化
    static TlsMode g_TlsMode;

    static
    VOID
    NTAPI
    FirstCallback(
        PVOID DllHandle,
        DWORD Reason,
        PVOID Reserved
        )
    {
        if (DLL_PROCESS_ATTACH == Reason)
        {
            if (g_TlsMode == TlsMode::None)
            {
                g_TlsMode = TlsMode::ByFirstCallback;
            }
        }
    }

    struct TlsRawItem
    {
        TlsRawItem* pNext;
        TlsRawItem* pPrev;
        BYTE* pBase;
        void** pOldTlsIndex;

        void __fastcall Free() noexcept
        {
            YY::Thunks::internal::Free(pOldTlsIndex);
            pOldTlsIndex = nullptr;
            auto _pBase = pBase;
            pBase = nullptr;
            YY::Thunks::internal::Free(_pBase);
        }
    };

    struct TlsHeader
    {
        TlsRawItem* volatile pRoot = nullptr;

        void __fastcall RemoveItem(TlsRawItem* _pItem) noexcept
        {
            if (pRoot == _pItem)
            {
                _pItem->pPrev = nullptr;
                pRoot = _pItem->pNext;
            }
            else
            {
                auto _pPrev = _pItem->pPrev;
                auto _pNext = _pItem->pNext;

                _pPrev->pNext = _pNext;

                if (_pNext)
                {
                    _pNext->pPrev = _pPrev;
                }
            }
        }

        void __fastcall AddItem(TlsRawItem* _pFirst, TlsRawItem* _pLast = nullptr) noexcept
        {
            if (!_pLast)
                _pLast = _pFirst;

            _pFirst->pPrev = nullptr;
            auto _pRoot = pRoot;
            _pLast->pNext = _pRoot;
            if (_pRoot)
            {
                _pRoot->pPrev = _pLast;
            }
            pRoot = _pFirst;
        }

        TlsRawItem* __fastcall Flush() noexcept
        {
            return (TlsRawItem*)InterlockedExchange((uintptr_t*)&pRoot, 0);
        }
    };

    enum class TlsStatus
    {
        // 没有人尝试释放Tls
        None,
        // 线程Tls数据需要申请或者释放
        ThreadLock,
        // DLL正在释放
        DllUnload
    };

    static volatile LONG uStatus = 0;
    static TlsHeader g_TlsHeader;

    static SIZE_T __fastcall GetTlsIndexBufferCount(TEB* _pTeb)
    {
        auto _ppThreadLocalStoragePointer = (void**)_pTeb->ThreadLocalStoragePointer;
        if (!_ppThreadLocalStoragePointer)
            return 0;

        return HeapSize(_pTeb->ProcessEnvironmentBlock->ProcessHeap, 0, _ppThreadLocalStoragePointer) / sizeof(void*);
    }
    
    static ULONG __fastcall GetMaxTlsIndex() noexcept
    {
        ULONG uMaxTlsIndex = 0;

        auto _pLdr = (_PEB_LDR_DATA_XP*)(TEB*)NtCurrentTeb()->ProcessEnvironmentBlock->Ldr;
        auto _pHerder = &_pLdr->InLoadOrderModuleList;
        for (auto _pItem = _pHerder->Flink; _pItem != _pHerder; )
        {
            auto _pEntry = CONTAINING_RECORD(_pItem, _LDR_DATA_TABLE_ENTRY_XP, InLoadOrderModuleList);
            _pItem = _pItem->Flink;

            __try
            {
                // XP不会为动态加载的DLL设置这个字段（符合预期，毕竟没有真的初始化）
                /*if (!_pEntry->TlsIndex)
                    continue;*/
                ULONG TlsSize;
                auto pTlsImage = (PIMAGE_TLS_DIRECTORY)YY_ImageDirectoryEntryToData(
                    _pEntry->BaseAddress,
                    IMAGE_DIRECTORY_ENTRY_TLS,
                    &TlsSize);

                if (pTlsImage == nullptr || pTlsImage->AddressOfIndex == 0)
                {
                    continue;
                }

                const auto _TlsIndex = *(ULONG*)pTlsImage->AddressOfIndex;
                if (uMaxTlsIndex < _TlsIndex)
                    uMaxTlsIndex = _TlsIndex;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
            }
        }

        return uMaxTlsIndex;
        
    }

    static SYSTEM_PROCESS_INFORMATION* __fastcall GetCurrentProcessInfo(StringBuffer<char>& _szBuffer)
    {
#if !defined(__USING_NTDLL_LIB)
        const auto NtQuerySystemInformation = try_get_NtQuerySystemInformation();
        if (!NtQuerySystemInformation)
            return nullptr;
#endif

        auto _cbBuffer = max(4096, _szBuffer.uBufferLength);
        ULONG _cbRet = 0;
        for (;;)
        {
            auto _pBuffer = _szBuffer.GetBuffer(_cbBuffer);
            if (!_pBuffer)
                return nullptr;

            LONG _Status = NtQuerySystemInformation(SystemProcessInformation, _pBuffer, _cbBuffer, &_cbRet);
            if (_Status >= 0)
                break;

            if (STATUS_INFO_LENGTH_MISMATCH != _Status)
                return nullptr;

            _cbBuffer = _cbRet;
        }

        const auto _uCurrentProcessId = GetCurrentProcessId();
        auto _pInfo = (SYSTEM_PROCESS_INFORMATION*)_szBuffer.GetBuffer(0);
        for (;;)
        {
            if (static_cast<DWORD>(reinterpret_cast<UINT_PTR>(_pInfo->ProcessId)) == _uCurrentProcessId)
                return _pInfo;

            if (_pInfo->NextEntryDelta == 0)
                break;

            _pInfo = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(reinterpret_cast<char*>(_pInfo) + _pInfo->NextEntryDelta);
        }

        return nullptr;
    }

    static bool __fastcall AllocTlsData(TEB* _pTeb = nullptr) noexcept
    {
        const size_t _cbTlsRaw = _tls_used.EndAddressOfRawData - _tls_used.StartAddressOfRawData;
        if (_cbTlsRaw == 0)
            return true;

        if (_tls_index == 0)
            return false;

        if (!_pTeb)
            _pTeb = (TEB*)NtCurrentTeb();

        auto _pTlsIndex = (void**)_pTeb->ThreadLocalStoragePointer;
        const auto _cTlsIndexLength = GetTlsIndexBufferCount(_pTeb);
        if (_cTlsIndexLength > _tls_index)
        {
            InterlockedExchange((uintptr_t*)&_pTlsIndex[_tls_index], 0);
        }

        auto _pRawTlsData = (BYTE*)Alloc(_cbTlsRaw + sizeof(TlsRawItem), HEAP_ZERO_MEMORY);
        if (!_pRawTlsData)
        {
            return false;
        }

        auto _pCurrentNode = (TlsRawItem*)(_pRawTlsData + _cbTlsRaw);
        if (_cTlsIndexLength <= _tls_index)
        {
            // Index不足，扩充……
            auto _cNewTlsIndexLength = _tls_index + 128;
            auto _pNewTlsIndex = (void**)Alloc(_cNewTlsIndexLength * sizeof(void*), HEAP_ZERO_MEMORY);
            if (!_pNewTlsIndex)
            {
                Free(_pRawTlsData);
                return false;
            }

            memcpy(_pNewTlsIndex, _pTlsIndex, _cTlsIndexLength * sizeof(void*));
            if ((void*)InterlockedCompareExchange((uintptr_t*)&_pTeb->ThreadLocalStoragePointer, (uintptr_t)_pNewTlsIndex, (uintptr_t)_pTlsIndex) != _pTlsIndex)
            {
                // 这是什么情况，DllMain期间桌怎么会有其他线程操作Tls？
                Free(_pNewTlsIndex);
                Free(_pRawTlsData);
                return false;
            }

            if (_pTeb == (TEB*)NtCurrentTeb())
            {
                Free(_pTlsIndex);
            }
            else
            {
                // 其他线程的无法直接释放，玩意缓存里恰好正在使用这块那么会崩溃的
                _pCurrentNode->pOldTlsIndex = _pTlsIndex;
            }
            _pTlsIndex = _pNewTlsIndex;
        }

        memcpy(_pRawTlsData, (void*)_tls_used.StartAddressOfRawData, _cbTlsRaw);
        _pCurrentNode->pBase = _pRawTlsData;
        for (;;)
        {
            const auto _Status = (TlsStatus)InterlockedCompareExchange(&uStatus, LONG(TlsStatus::ThreadLock), LONG(TlsStatus::None));
            // 锁定成功？
            if (_Status == TlsStatus::None)
            {
                g_TlsHeader.AddItem(_pCurrentNode);
                // 解除锁定
                InterlockedExchange(&uStatus, LONG(TlsStatus::None));
                InterlockedExchange((uintptr_t*)&_pTlsIndex[_tls_index], (uintptr_t)_pRawTlsData);
                break;
            }

            // 当前Dll正在卸载，不能再添加Tls
            if (_Status == TlsStatus::DllUnload)
            {
                _pCurrentNode->Free();
                return false;
            }
        }
        return true;
    }

    static void __fastcall FreeTlsData()
    {
        if (_tls_index == 0)
            return;

        const size_t _cbTlsRaw = _tls_used.EndAddressOfRawData - _tls_used.StartAddressOfRawData;
        if (_cbTlsRaw == 0)
            return;

        auto _pTeb = (TEB*)NtCurrentTeb();
        if (_tls_index >= GetTlsIndexBufferCount(_pTeb))
            return;

        auto _ppTlsIndex = (void**)_pTeb->ThreadLocalStoragePointer;
        auto _pTlsRawData = (BYTE*)InterlockedExchange((uintptr_t*)&_ppTlsIndex[_tls_index], 0);
        if (_pTlsRawData == nullptr)
            return;

        auto _pCurrentNode = (TlsRawItem*)(_pTlsRawData + _cbTlsRaw);

        for (;;)
        {
            const auto _Status = (TlsStatus)InterlockedCompareExchange(&uStatus, LONG(TlsStatus::ThreadLock), LONG(TlsStatus::None));
            // 锁定成功？
            if (_Status == TlsStatus::None)
            {
                __try
                {
                    // 检查一下这块Tls数据是否是我们申请的
                    if (_pCurrentNode->pBase == _pTlsRawData)
                    {
                        g_TlsHeader.RemoveItem(_pCurrentNode);                    
                    }
                    else
                    {
                        _pCurrentNode = nullptr;
                    }
                }
                __except (EXCEPTION_EXECUTE_HANDLER)
                {
                    _pCurrentNode = nullptr;
                }

                // 解除锁定
                InterlockedExchange(&uStatus, LONG(TlsStatus::None));

                if (_pCurrentNode)
                    _pCurrentNode->Free();
                return;
            }

            // 当前Dll正在卸载，这些内存统一由DllMain接管，FreeTlsIndex会统一释放内存
            if (_Status == TlsStatus::DllUnload)
                return;
        }
    }

    static bool __fastcall CreateTlsIndex() noexcept
    {
        if (_tls_index != 0)
            return false;

        _tls_index = GetMaxTlsIndex() + 1;
        if (!AllocTlsData((TEB*)NtCurrentTeb()))
            return false;

        // 同时给所有历史的线程追加新DLL产生的Tls内存
        do
        {
#if !defined(__USING_NTDLL_LIB)
            const auto NtQueryInformationThread = try_get_NtQueryInformationThread();
            if (!NtQueryInformationThread)
                break;
#endif

            StringBuffer<char> _Buffer;
            auto _pProcessInfo = GetCurrentProcessInfo(_Buffer);
            if (!_pProcessInfo)
            {
                break;
            }

            const auto _uCurrentThreadId = GetCurrentThreadId();

            for (ULONG i = 0; i != _pProcessInfo->ThreadCount; ++i)
            {
                auto& _Thread = _pProcessInfo->Threads[i];

                if (_uCurrentThreadId == static_cast<DWORD>(reinterpret_cast<UINT_PTR>(_Thread.ClientId.UniqueThread)))
                    continue;

                auto _hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, static_cast<DWORD>(reinterpret_cast<UINT_PTR>(_Thread.ClientId.UniqueThread)));
                if (!_hThread)
                {
                    continue;
                }

                THREAD_BASIC_INFORMATION _ThreadBasicInfo = {};
                LONG _Status = NtQueryInformationThread(_hThread, ThreadBasicInformation, &_ThreadBasicInfo, sizeof(_ThreadBasicInfo), nullptr);
                if (_Status >= 0 && _ThreadBasicInfo.TebBaseAddress)
                {
                    AllocTlsData((TEB*)_ThreadBasicInfo.TebBaseAddress);
                }

                CloseHandle(_hThread);
            }
        } while (false);

        return true;
    }

    static void __fastcall FreeTlsIndex() noexcept
    {
        if (_tls_index == 0)
            return;

        for (;;)
        {
            const auto _Status = (TlsStatus)InterlockedCompareExchange(&uStatus, LONG(TlsStatus::DllUnload), LONG(TlsStatus::None));
            // 锁定成功？
            if (_Status == TlsStatus::None)
            {
                for (auto _pItem = g_TlsHeader.Flush(); _pItem;)
                {
                    auto _pNext = _pItem->pNext;
                    _pItem->Free();
                    _pItem = _pNext;
                }
                return;
            }

            // DllUnload 代表全局数据已经释放，只能进入一次
            if (_Status == TlsStatus::DllUnload)
                return;
        }
    }

    static void __fastcall CallTlsCallback(
        HINSTANCE const _hInstance,
        DWORD     const _uReason)
    {
        if (_tls_index == 0)
            return;

        auto _pTeb = (TEB*)NtCurrentTeb();
        auto _ppThreadLocalStoragePointer = (void**)_pTeb->ThreadLocalStoragePointer;
        if (!_ppThreadLocalStoragePointer)
            return;

        if (_tls_index >= GetTlsIndexBufferCount(_pTeb))
            return;

        if (!_ppThreadLocalStoragePointer[_tls_index])
            return;
        
        __try
        {
            auto _pfnTlsCallbacks = reinterpret_cast<PIMAGE_TLS_CALLBACK*>(_tls_used.AddressOfCallBacks);
            if (_pfnTlsCallbacks)
            {
                for (; *_pfnTlsCallbacks; ++_pfnTlsCallbacks)
                {
                    (*_pfnTlsCallbacks)(_hInstance, _uReason, nullptr);
                }
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }
#endif // YY_Thunks_Target < __WindowsNT6

    // 我们始终提供DllMainCRTStartupForYY_Thunks函数，虽然理论上Vista（包含）以上就不用修正了，但是为了做到体验统一
    // 避免Vista模式设置 DllMainCRTStartupForYY_Thunks 就发生链接失败问题。
    // 这个函数的存在意义是修复Windows XP系统显式加载的DLL里无法使用TLS问题。
    extern "C" BOOL WINAPI DllMainCRTStartupForYY_Thunks(
        HINSTANCE const _hInstance,
        DWORD     const _uReason,
        LPVOID    const _pReserved
        )
    {
        const auto _pfnDllMainCRTStartup = __pfnDllMainCRTStartupForYY_Thunks ? __pfnDllMainCRTStartupForYY_Thunks : &_DllMainCRTStartup;

        // Vista开始已经可以动态的处理TLS问题了，所以这里只针对老系统处理。
        switch (_uReason)
        {
        case DLL_PROCESS_ATTACH:
            _YY_initialize_winapi_thunks(ThunksInitStatus::InitByDllMain);

#if YY_Thunks_Target < __WindowsNT6
            if (internal::GetSystemVersion() < internal::MakeVersion(6, 0))
            {
                __declspec(allocate(".CRT$XLB")) static const PIMAGE_TLS_CALLBACK s_FirstCallback = FirstCallback;
                __foreinclude(s_FirstCallback);
                __foreinclude(_tls_used);
                if (g_TlsMode == TlsMode::None)
                {
                    g_TlsMode = TlsMode::ByDllMainCRTStartupForYY_Thunks;
                }
                _tls_index_old = _tls_index;
                if (_tls_index == 0 && g_TlsMode == TlsMode::ByDllMainCRTStartupForYY_Thunks)
                {
                    if (!CreateTlsIndex())
                        return FALSE;
                }
            }
#endif
            return _pfnDllMainCRTStartup(_hInstance, _uReason, _pReserved);
            break;
#if YY_Thunks_Target < __WindowsNT6
        case DLL_THREAD_ATTACH:
            if (internal::GetSystemVersion() < internal::MakeVersion(6, 0) && _tls_index_old == 0 && g_TlsMode == TlsMode::ByDllMainCRTStartupForYY_Thunks)
            {
                AllocTlsData();
                CallTlsCallback(_hInstance, _uReason);
            }
            return _pfnDllMainCRTStartup(_hInstance, _uReason, _pReserved);
            break;
        case DLL_THREAD_DETACH:
            if (internal::GetSystemVersion() < internal::MakeVersion(6, 0) && _tls_index_old == 0 && g_TlsMode == TlsMode::ByDllMainCRTStartupForYY_Thunks)
            {
                CallTlsCallback(_hInstance, _uReason);
                auto _bRet = _pfnDllMainCRTStartup(_hInstance, _uReason, _pReserved);
                FreeTlsData();
                return _bRet;
            }
            else
            {
                return _pfnDllMainCRTStartup(_hInstance, _uReason, _pReserved);
            }
            break;
#endif
        case DLL_PROCESS_DETACH:
#if (YY_Thunks_Target < __WindowsNT5_1)
            __YY_Thunks_Process_Terminating = _pReserved != nullptr;
#endif

#if YY_Thunks_Target < __WindowsNT6
            if (internal::GetSystemVersion() < internal::MakeVersion(6, 0) && _tls_index_old == 0 && g_TlsMode == TlsMode::ByDllMainCRTStartupForYY_Thunks)
            {
                CallTlsCallback(_hInstance, _uReason);
                auto _bRet = _pfnDllMainCRTStartup(_hInstance, _uReason, _pReserved);

                if (_pReserved == nullptr)
                {
                    FreeTlsIndex();
                    __YY_uninitialize_winapi_thunks();
                }
                return _bRet;
            }
            else
#endif
            {
                auto _bRet = _pfnDllMainCRTStartup(_hInstance, _uReason, _pReserved);
                if (_pReserved == nullptr)
                {
                    __YY_uninitialize_winapi_thunks();
                }
                return _bRet;
            }
            break;
        default:
            return _pfnDllMainCRTStartup(_hInstance, _uReason, _pReserved);
            break;
        }
    }
}
#endif
