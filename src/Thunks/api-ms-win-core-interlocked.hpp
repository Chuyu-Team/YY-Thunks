

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_2)

    #pragma push_macro("InterlockedCompareExchange64")
    #undef InterlockedCompareExchange64

    //Windows Vista [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    20,
    LONG64,
    WINAPI,
    InterlockedCompareExchange64,
        _Inout_ _Interlocked_operand_ LONG64 volatile *Destination,
        _In_ LONG64 ExChange,
        _In_ LONG64 Comperand
        )
    {
        return _InterlockedCompareExchange64(Destination, ExChange, Comperand);
    }
    #pragma pop_macro("InterlockedCompareExchange64")

#endif


#if (YY_Thunks_Target < __WindowsNT5_1)

    //Windows XP [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    4,
    VOID,
    WINAPI,
    InitializeSListHead,
        _Out_ PSLIST_HEADER ListHead
        )
    {
        *ListHead = SLIST_HEADER{};
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1)

    //Windows XP [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    4,
    PSLIST_ENTRY,
    WINAPI,
    InterlockedFlushSList,
        _Inout_ PSLIST_HEADER ListHead
        )
    {
        if (const auto pInterlockedFlushSList = try_get_InterlockedFlushSList())
        {
            return pInterlockedFlushSList(ListHead);
        }

        __asm
        {
            mov     ebp, [ListHead]
            xor     ebx, ebx
            mov     edx, [ebp + 4]
            mov     eax, [ebp + 0]

        _Loop:
            or      eax, eax
            jz      _End
            mov     ecx, edx
            mov     cx, bx
            lock cmpxchg8b qword ptr[ebp]
            jnz _Loop
            _End:
        }

        //直接返回eax即可。
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1)

    //Windows XP [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    4,
    USHORT,
    WINAPI,
    QueryDepthSList,
        _In_ PSLIST_HEADER ListHead
        )
    {
        if (const auto pQueryDepthSList = try_get_QueryDepthSList())
        {
            return pQueryDepthSList(ListHead);
        }

        return ListHead->Depth;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1)

    //Windows XP [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    8,
    PSLIST_ENTRY,
    WINAPI,
    InterlockedPushEntrySList,
        _Inout_ PSLIST_HEADER ListHead,
        _Inout_ __drv_aliasesMem PSLIST_ENTRY ListEntry
        )
    {
        if (const auto pInterlockedPushEntrySList = try_get_InterlockedPushEntrySList())
        {
            return pInterlockedPushEntrySList(ListHead, ListEntry);
        }

        __asm
        {
            mov     ebx, [ListEntry]
            mov     ebp, [ListHead]

            mov     edx, [ebp + 4]
            mov     eax, [ebp + 0]

            _Loop:
            mov     [ebx], eax
            lea     ecx, [edx + 10001h]
            lock cmpxchg8b qword ptr [ebp]
            jnz _Loop
        }

        //asm会更新eax
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1)

    //Windows XP [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    4,
    PSLIST_ENTRY,
    WINAPI,
    InterlockedPopEntrySList,
        _Inout_ PSLIST_HEADER ListHead
        )
    {
        if (const auto pInterlockedPopEntrySList = try_get_InterlockedPopEntrySList())
        {
            return pInterlockedPopEntrySList(ListHead);
        }

        __asm
        {
            mov     ebp, [ListHead]
            mov     edx, [ebp + 4]
            mov     eax, [ebp + 0]
        _Loop:
            or      eax, eax
            jz      _End
            lea     ecx, [edx-1]
            mov     ebx, [eax]
            lock cmpxchg8b qword ptr [ebp]
            jnz     _Loop
        _End:

        }

        //asm 会修改eax
    }
#endif
} //namespace YY::Thunks
