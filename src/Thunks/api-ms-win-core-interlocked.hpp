

namespace YY {
namespace Thunks {

#if (YY_Thunks_Support_Version < NTDDI_WS03)
// Windows Vista [desktop apps | UWP apps]
// Windows Server 2003 [desktop apps | UWP apps]

#pragma push_macro("InterlockedCompareExchange64")
#undef InterlockedCompareExchange64
EXTERN_C
LONG64
WINAPI
InterlockedCompareExchange64(
    _Inout_ _Interlocked_operand_ LONG64 volatile *Destination,
    _In_ LONG64 ExChange,
    _In_ LONG64 Comperand)
#ifdef YY_Thunks_Defined
    ;
#else
{
    return _InterlockedCompareExchange64(Destination, ExChange, Comperand);
}
#endif

__YY_Thunks_Expand_Function(kernel32, InterlockedCompareExchange64, 20);
#pragma pop_macro("InterlockedCompareExchange64")

#endif

#if (YY_Thunks_Support_Version < NTDDI_WINXP)
// Windows XP [desktop apps | UWP apps]
// Windows Server 2003 [desktop apps | UWP apps]

EXTERN_C
VOID WINAPI InitializeSListHead(_Out_ PSLIST_HEADER ListHead)
#ifdef YY_Thunks_Defined
    ;
#else
{
    *ListHead = SLIST_HEADER{};
}
#endif

__YY_Thunks_Expand_Function(kernel32, InitializeSListHead, 4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WINXP)
// Windows XP [desktop apps | UWP apps]
// Windows Server 2003 [desktop apps | UWP apps]

EXTERN_C
PSLIST_ENTRY
WINAPI
InterlockedFlushSList(_Inout_ PSLIST_HEADER ListHead)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (const auto pInterlockedFlushSList = try_get_InterlockedFlushSList())
    {
        return pInterlockedFlushSList(ListHead);
    }

    __asm {
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

__YY_Thunks_Expand_Function(kernel32, InterlockedFlushSList, 4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WINXP)
// Windows XP [desktop apps | UWP apps]
// Windows Server 2003 [desktop apps | UWP apps]

EXTERN_C
USHORT
WINAPI
QueryDepthSList(_In_ PSLIST_HEADER ListHead)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (const auto pQueryDepthSList = try_get_QueryDepthSList())
    {
        return pQueryDepthSList(ListHead);
    }

    return ListHead->Depth;
}
#endif

__YY_Thunks_Expand_Function(kernel32, QueryDepthSList, 4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WINXP)
// Windows XP [desktop apps | UWP apps]
// Windows Server 2003 [desktop apps | UWP apps]

EXTERN_C
PSLIST_ENTRY
WINAPI
InterlockedPushEntrySList(
    _Inout_ PSLIST_HEADER ListHead,
    _Inout_ __drv_aliasesMem PSLIST_ENTRY ListEntry)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (const auto pInterlockedPushEntrySList = try_get_InterlockedPushEntrySList())
    {
        return pInterlockedPushEntrySList(ListHead, ListEntry);
    }

    __asm {
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

    // asm会更新eax
}
#endif

__YY_Thunks_Expand_Function(kernel32, InterlockedPushEntrySList, 8);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WINXP)
// Windows XP [desktop apps | UWP apps]
// Windows Server 2003 [desktop apps | UWP apps]

EXTERN_C
PSLIST_ENTRY
WINAPI
InterlockedPopEntrySList(_Inout_ PSLIST_HEADER ListHead)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (const auto pInterlockedPopEntrySList = try_get_InterlockedPopEntrySList())
    {
        return pInterlockedPopEntrySList(ListHead);
    }

    __asm {
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

    // asm 会修改eax
}
#endif

__YY_Thunks_Expand_Function(kernel32, InterlockedPopEntrySList, 4);

#endif

} // namespace Thunks

} // namespace YY
