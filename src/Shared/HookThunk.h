#pragma once

/*
跨模块ABI兼容的Thunk逻辑。
*/

namespace YY
{
    namespace Thunks
    {
        namespace internal
        {
            union HookThunkData
            {
            private:
                static HookThunkData* volatile pFreeRoot;
                // Windows最小地址管理粒度是64K，所以我们就直接申请一个64K
                static constexpr size_t kAllocBufferBytesCount = 64 * 1024;

            public:
                HookThunkData* volatile pNext;
                // 可以供内存执行权限的一段区域
                struct
                {
                    unsigned char ShellCode[60];
                    // 内存块的引用计数。
                    // 1. 如果是Buffer第一块内存的引用计数，那么引用归零时彻底释放整个缓冲区。
                    // 2. 如果不是Buffer第一块内存的引用计数，引用归零时减少一次该缓冲区第一块内存的引用计数。
                    ULONG uRef;
                };

                static HookThunkData* __fastcall Alloc()
                {
                    __declspec(allocate(".YYThr$AAB")) static void* s_FreeAllHookThunkData = reinterpret_cast<void*>(&HookThunkData::FreeAll);
                    __foreinclude(s_FreeAllHookThunkData);

                    for (auto _pLast = pFreeRoot; _pLast;)
                    {
                        auto _pNext = _pLast->pNext;

                        auto _pResult = (HookThunkData*)InterlockedCompareExchange((volatile uintptr_t*)&pFreeRoot, (uintptr_t)_pNext, (uintptr_t)_pLast);
                        if (_pResult == _pLast)
                        {
                            _pResult->pNext = nullptr;

                            InterlockedIncrement(&_pResult->uRef);
                            return _pResult;
                        }
                        _pLast = _pResult;
                    }

                    // 缓存区域已经耗尽，申请新的内存。
                    auto _pResult = (HookThunkData*)VirtualAlloc(nullptr, kAllocBufferBytesCount, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
                    if (!_pResult)
                        return nullptr;

                    // 因为当前需要返回 _pResult，所以引用计数额外 + 1
                    _pResult->uRef = kAllocBufferBytesCount / sizeof(HookThunkData) + 1;

                    static_assert(kAllocBufferBytesCount % sizeof(HookThunkData) == 0, "");
                    // 因为等会需要返回一块内存，所以第一快内存我们就需要跳过，不添加到pFreeRoot了。
                    const auto _pFirstBuffer = _pResult + 1;
                    const auto _pLastBuffer = reinterpret_cast<HookThunkData*>((uintptr_t)_pResult + kAllocBufferBytesCount - sizeof(HookThunkData));

                    for (auto _pItem = _pFirstBuffer; ;)
                    {
                        _pItem->uRef = 1;
                        if (_pItem == _pLastBuffer)
                            break;
                        auto _pNext = _pItem + 1;
                        _pItem->pNext = _pNext;
                        _pItem = _pNext;
                    }

                    for (auto _pLast = pFreeRoot; ;)
                    {
                        _pLastBuffer->pNext = _pLast;
                        auto _pResult = (HookThunkData*)InterlockedCompareExchange((volatile uintptr_t*)&pFreeRoot, (uintptr_t)_pFirstBuffer, (uintptr_t)_pLast);
                        if (_pResult == _pLast)
                        {
                            break;
                        }
                        _pLast = _pResult;
                    }
                    return _pResult;
                }

                void __fastcall Free()
                {
                    // 防止内存执行区域被人利用，数据全部清除！！
                    memset(ShellCode, 0, sizeof(ShellCode));

                    if (Release() == 0)
                        return;

                    // 重新将内存加入缓存区域。
                    for (auto _pLast = pFreeRoot; ;)
                    {
                        pNext = _pLast;
                        auto _pResult = (HookThunkData*)InterlockedCompareExchange((volatile uintptr_t*)&pFreeRoot, (uintptr_t)this, (uintptr_t)_pLast);
                        if (_pResult == _pLast)
                        {
                            return;
                        }
                        _pLast = _pResult;
                    }
                }

            private:
                static void __cdecl FreeAll() noexcept
                {
                    HookThunkData* _pRoot = (HookThunkData*)InterlockedExchange((volatile uintptr_t*)&pFreeRoot, (uintptr_t)nullptr);
                    for (auto _pItem = _pRoot; _pItem; )
                    {
                        auto _pNext = _pItem->pNext;
                        _pItem->Release();
                        
                        _pItem = _pNext;
                    }
                }

                /// <summary>
                /// 减少一次内存引用。
                /// </summary>
                /// <returns>返回新的引用计数。</returns>
                ULONG __fastcall Release() noexcept
                {
                    const auto _uNewRef = InterlockedDecrement(&uRef);
                    if (_uNewRef == 0)
                    {
                        // 引用归0，这块内存可能来自于其他模块，并且目标模块已经释放。
                        auto _pFirstBlock = reinterpret_cast<HookThunkData*>(uintptr_t(this) & (~(kAllocBufferBytesCount - 1)));
                        if (_pFirstBlock == this || InterlockedDecrement(&_pFirstBlock->uRef) == 0)
                        {
                            VirtualFree(_pFirstBlock, 0, MEM_RELEASE);
                        }
                    }

                    return _uNewRef;
                }
            };

            HookThunkData* volatile HookThunkData::pFreeRoot = nullptr;
        }
    }
}
