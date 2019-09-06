

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WS03)
//Windows Vista [desktop apps | UWP apps]
//Windows Server 2003 [desktop apps | UWP apps]

#pragma push_macro("InterlockedCompareExchange64")
#undef InterlockedCompareExchange64
EXTERN_C
LONG64
WINAPI
InterlockedCompareExchange64(
    _Inout_ _Interlocked_operand_ LONG64 volatile *Destination,
    _In_ LONG64 ExChange,
    _In_ LONG64 Comperand
    )
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


	}//namespace Thunks

} //namespace YY