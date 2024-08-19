#if (YY_Thunks_Target < __WindowsNT6)
#include <winevt.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_wevtapi)
#define __Comment_Lib_wevtapi
#pragma comment(lib, "wevtapi.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps only]
    // Minimum supported server	Windows Server 2008[desktop apps only]
    __DEFINE_THUNK(
    wevtapi,
    4,
    BOOL,
    WINAPI,
    EvtClose,
        _In_ _Post_invalid_ EVT_HANDLE Object
        )
    {
        if (auto const _pfnEvtClose = try_get_EvtClose())
        {
            return _pfnEvtClose(Object);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps only]
    // Minimum supported server	Windows Server 2008[desktop apps only]
    __DEFINE_THUNK(
    wevtapi,
    12,
    EVT_HANDLE,
    WINAPI,
    EvtCreateRenderContext,
        DWORD ValuePathsCount,
        _In_reads_opt_(ValuePathsCount) LPCWSTR* ValuePaths,
        DWORD Flags
        )
    {
        if (auto const _pfnEvtCreateRenderContext = try_get_EvtCreateRenderContext())
        {
            return _pfnEvtCreateRenderContext(ValuePathsCount, ValuePaths, Flags);
        }

        SetLastError(ERROR_NOT_SUPPORTED);
        return NULL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps only]
    // Minimum supported server	Windows Server 2008[desktop apps only]
    __DEFINE_THUNK(
    wevtapi,
    24,
    BOOL,
    WINAPI,
    EvtNext,
        _In_ EVT_HANDLE ResultSet,
        DWORD EventsSize,
        _Out_writes_to_(EventsSize, *Returned) PEVT_HANDLE Events,
        DWORD Timeout,
        DWORD Flags,
        _Out_range_(0, EventsSize) PDWORD Returned
        )
    {
        if (auto const _pfnEvtNext = try_get_EvtNext())
        {
            return _pfnEvtNext(ResultSet, EventsSize, Events, Timeout, Flags, Returned);
        }

        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps only]
    // Minimum supported server	Windows Server 2008[desktop apps only]
    __DEFINE_THUNK(
    wevtapi,
    16,
    EVT_HANDLE,
    WINAPI,
    EvtQuery,
        _In_opt_ EVT_HANDLE Session,
        _In_opt_z_ LPCWSTR Path,
        _In_opt_z_ LPCWSTR Query,
        DWORD Flags
        )
    {
        if (auto const _pfnEvtQuery = try_get_EvtQuery())
        {
            return _pfnEvtQuery(Session, Path, Query, Flags);
        }
        SetLastError(ERROR_NOT_SUPPORTED);
        return  NULL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps only]
    // Minimum supported server	Windows Server 2008[desktop apps only]
    __DEFINE_THUNK(
    wevtapi,
    28,
    BOOL,
    WINAPI,
    EvtRender,
        _In_opt_ EVT_HANDLE Context,
        _In_ EVT_HANDLE Fragment,
        DWORD Flags,
        DWORD BufferSize,
        _Out_writes_bytes_to_opt_(BufferSize, *BufferUsed) PVOID Buffer,
        _Out_ PDWORD BufferUsed,
        _Out_ PDWORD PropertyCount
        )
    {
        if (auto const _pfnEvtRender = try_get_EvtRender())
        {
            return _pfnEvtRender(Context, Fragment, Flags, BufferSize, Buffer, BufferUsed, PropertyCount);
        }
        SetLastError(ERROR_NOT_SUPPORTED);
        return  FALSE;
    }
#endif
} //namespace YY::Thunks
