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

#if YY_Thunks_Support_Version < NTDDI_WIN6
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
    };
    static thread_local TlsRawItem s_CurrentNode;

    static volatile LONG uStatus = 0;
    static TlsRawItem* volatile pRoot = nullptr;

    static ULONG __fastcall GetTlsIndexBufferCount(TEB* _pTeb)
    {
        auto _ppThreadLocalStoragePointer = (void**)_pTeb->ThreadLocalStoragePointer;
        if (!_ppThreadLocalStoragePointer)
            return 0;

        return HeapSize(_pTeb->ProcessEnvironmentBlock->ProcessHeap, 0, _ppThreadLocalStoragePointer) / sizeof(void*);
    }
    static PVOID NTAPI RtlImageDirectoryEntryToData(
        __in PVOID pBaseAddress,
        __in ULONG dwDirectory,
        __out PULONG pSize
        )
    {
        auto _pDosHeader = (PIMAGE_DOS_HEADER)pBaseAddress;
        auto _pNtHerder = reinterpret_cast<PIMAGE_NT_HEADERS>(PBYTE(pBaseAddress) + _pDosHeader->e_lfanew);
        auto& _DataDirectory = _pNtHerder->OptionalHeader.DataDirectory[dwDirectory];

        *pSize = _DataDirectory.Size;
        if (_DataDirectory.Size == 0 || _DataDirectory.VirtualAddress == 0)
            return nullptr;

        return PBYTE(pBaseAddress) + _DataDirectory.VirtualAddress;
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
                auto pTlsImage = (PIMAGE_TLS_DIRECTORY)RtlImageDirectoryEntryToData(
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
        const auto _pfnNtQuerySystemInformation = try_get_NtQuerySystemInformation();
        if (!_pfnNtQuerySystemInformation)
            return nullptr;

        ULONG _cbBuffer = max(4096, _szBuffer.uBufferLength);
        ULONG _cbRet = 0;
        for (;;)
        {
            auto _pBuffer = _szBuffer.GetBuffer(_cbBuffer);
            if (!_pBuffer)
                return nullptr;

            LONG _Status = _pfnNtQuerySystemInformation(SystemProcessInformation, _pBuffer, _cbBuffer, &_cbRet);
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
            if (_pInfo->ProcessId == (HANDLE)_uCurrentProcessId)
                return _pInfo;

            if (_pInfo->NextEntryDelta == 0)
                break;

            _pInfo = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(reinterpret_cast<char*>(_pInfo) + _pInfo->NextEntryDelta);
        }

        return nullptr;
    }

    static bool __fastcall AllocTlsData(TEB* _pTeb = nullptr) noexcept
    {
        if (_tls_index == 0)
            return false;
        if (!_pTeb)
            _pTeb = (TEB*)NtCurrentTeb();

        auto _pTlsIndex = (void**)_pTeb->ThreadLocalStoragePointer;
        void** _pOldTlsIndex = nullptr;
        auto _cTlsIndexLength = GetTlsIndexBufferCount(_pTeb);
        if (_cTlsIndexLength <= _tls_index)
        {
            // Index不足，扩充……
            auto _cNewTlsIndexLength = _tls_index + 128;
            auto _pNewTlsIndex = (void**)Alloc(_cNewTlsIndexLength * sizeof(void*), HEAP_ZERO_MEMORY);
            if (!_pNewTlsIndex)
                return false;

            memcpy(_pNewTlsIndex, _pTlsIndex, _cTlsIndexLength * sizeof(void*));
            if ((void*)InterlockedCompareExchange((uintptr_t*)&_pTeb->ThreadLocalStoragePointer, (uintptr_t)_pNewTlsIndex, (uintptr_t)_pTlsIndex) != _pTlsIndex)
            {
                Free(_pNewTlsIndex);
                return false;
            }

            if (_pTeb == (TEB*)NtCurrentTeb())
            {
                Free(_pTlsIndex);
            }
            else
            {
                // 其他线程的无法直接释放，玩意缓存里恰好正在使用这块那么会崩溃的
                _pOldTlsIndex = _pTlsIndex;
            }
            _pTlsIndex = _pNewTlsIndex;
        }
        const size_t _cbTlsRaw = _tls_used.EndAddressOfRawData - _tls_used.StartAddressOfRawData;
        auto _pRawTlsData = (BYTE*)Alloc(_cbTlsRaw, HEAP_ZERO_MEMORY);
        if (!_pRawTlsData)
        {
            // 释放不是安全的，极小的概率可能野，但是现在现在就这样吧。
            Free(_pOldTlsIndex);
            return false;
        }

        memcpy(_pRawTlsData, (void*)_tls_used.StartAddressOfRawData, _cbTlsRaw);
        InterlockedExchange((uintptr_t*)&_pTlsIndex[_tls_index], (uintptr_t)_pRawTlsData);

        s_CurrentNode.pBase = _pRawTlsData;
        s_CurrentNode.pOldTlsIndex = _pOldTlsIndex;

        for (;;)
        {
            if (!_interlockedbittestandset(&uStatus, 0))
            {
                s_CurrentNode.pNext = pRoot;
                if (pRoot)
                {
                    pRoot->pPrev = &s_CurrentNode;
                }
                pRoot = &s_CurrentNode;
                _interlockedbittestandreset(&uStatus, 0);
                break;
            }
        }
        return true;
    }

    static void __fastcall FreeTlsData()
    {
        if (_tls_index == 0)
            return;
        auto _pTeb = (TEB*)NtCurrentTeb();
        if (_tls_index >= GetTlsIndexBufferCount(_pTeb))
            return;

        auto _ppTlsIndex = (void**)_pTeb->ThreadLocalStoragePointer;
        if (_ppTlsIndex[_tls_index] == nullptr)
            return;

        if (s_CurrentNode.pBase != _ppTlsIndex[_tls_index])
            return;

        if (s_CurrentNode.pOldTlsIndex)
        {
            Free(s_CurrentNode.pOldTlsIndex);
            s_CurrentNode.pOldTlsIndex = nullptr;
        }

        s_CurrentNode.pBase = nullptr;

        for (;;)
        {
            if (!_interlockedbittestandset(&uStatus, 0))
            {
                auto pPrev = s_CurrentNode.pPrev;
                auto pNext = s_CurrentNode.pNext;
                if (pPrev)
                {
                    pPrev->pNext = pNext;
                }
                else
                {
                    pRoot = pNext;
                }

                if (pNext)
                {
                    pNext->pPrev = pPrev;
                }
                _interlockedbittestandreset(&uStatus, 0);

                auto _pTlsRawData = (void*)InterlockedExchange((uintptr_t*)&_ppTlsIndex[_tls_index], 0);
                Free(_pTlsRawData);
                break;
            }
        }
    }

    static bool __fastcall CreateTlsIndex() noexcept
    {
        if (_tls_index != 0)
            return false;

        _tls_index = GetMaxTlsIndex() + 1;
        AllocTlsData((TEB*)NtCurrentTeb());

        // 同时给所有历史的线程追加新DLL产生的Tls内存
        do
        {
            const auto _pfnNtQueryInformationThread = try_get_NtQueryInformationThread();
            if (!_pfnNtQueryInformationThread)
                break;

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

                if (_uCurrentThreadId == (DWORD)_Thread.ClientId.UniqueThread)
                    continue;

                auto _hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, (DWORD)_Thread.ClientId.UniqueThread);
                if (!_hThread)
                {
                    continue;
                }

                THREAD_BASIC_INFORMATION _ThreadBasicInfo = {};
                LONG _Status = _pfnNtQueryInformationThread(_hThread, ThreadBasicInformation, &_ThreadBasicInfo, sizeof(_ThreadBasicInfo), nullptr);
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

        // 故意不加锁……
        for (auto _pItem = pRoot; _pItem;)
        {
            auto _pNext = _pItem->pNext;
            if (_pItem->pOldTlsIndex)
            {
                Free(_pItem->pOldTlsIndex);
                _pItem->pOldTlsIndex = nullptr;
            }
            Free(_pItem->pBase);
            _pItem = _pNext;
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
#endif // YY_Thunks_Support_Version < NTDDI_WIN6

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

#if YY_Thunks_Support_Version < NTDDI_WIN6
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
                    CreateTlsIndex();
                }
            }
#endif
            return _pfnDllMainCRTStartup(_hInstance, _uReason, _pReserved);
            break;
#if YY_Thunks_Support_Version < NTDDI_WIN6
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
#if (YY_Thunks_Support_Version < NTDDI_WINXP)
            __YY_Thunks_Process_Terminating = _pReserved != nullptr;
#endif

#if YY_Thunks_Support_Version < NTDDI_WIN6
            if (internal::GetSystemVersion() < internal::MakeVersion(6, 0) && _tls_index_old == 0 && g_TlsMode == TlsMode::ByDllMainCRTStartupForYY_Thunks)
            {
                CallTlsCallback(_hInstance, _uReason);
                auto _bRet = _pfnDllMainCRTStartup(_hInstance, _uReason, _pReserved);

                FreeTlsIndex();
                if (_pReserved != nullptr)
                {
                    __YY_uninitialize_winapi_thunks();
                }
                return _bRet;
            }
            else
#endif
            {
                auto _bRet = _pfnDllMainCRTStartup(_hInstance, _uReason, _pReserved);
                if (_pReserved != nullptr)
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
