

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows XP with SP2, Windows Server 2003 with SP1
    __DEFINE_THUNK(
    kernel32,
    4,
    PVOID,
    WINAPI,
    DecodePointer,
        _In_opt_ PVOID Ptr
        )
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


#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows XP with SP2, Windows Server 2003 with SP1
    __DEFINE_THUNK(
    kernel32,
    4,
    PVOID,
    WINAPI,
    EncodePointer,
        _In_opt_ PVOID Ptr
        )
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
} //namespace YY::Thunks
