

namespace YY {
namespace Thunks {

#if (YY_Thunks_Support_Version < NTDDI_WS03SP1)
// Windows XP with SP2, Windows Server 2003 with SP1

EXTERN_C
PVOID
WINAPI
DecodePointer(_In_opt_ PVOID Ptr)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pDecodePointer = try_get_DecodePointer())
    {
        return pDecodePointer(Ptr);
    }
    else
    {
        return Ptr;
    }
}
#endif

__YY_Thunks_Expand_Function(kernel32, DecodePointer, 4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WS03SP1)
// Windows XP with SP2, Windows Server 2003 with SP1

EXTERN_C
PVOID
WINAPI
EncodePointer(_In_opt_ PVOID Ptr)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pEncodePointer = try_get_EncodePointer())
    {
        return pEncodePointer(Ptr);
    }
    else
    {
        return Ptr;
    }
}
#endif

__YY_Thunks_Expand_Function(kernel32, EncodePointer, 4);

#endif

} // namespace Thunks

} // namespace YY
