
#include <winevt.h>

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
		//EvtClose
		//Minimum supported client	Windows Vista
		//Minimum supported server	Windows Server 2008
		__DEFINE_THUNK(
            wevtapi,
			4,
            BOOL,
            WINAPI,
            EvtClose,
            _In_ _Post_invalid_ EVT_HANDLE Object
		)
		{
			if (auto const pEvtClose = try_get_EvtClose())
			{
				return pEvtClose(Object);
			}

			return  TRUE;
		}
	
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //EvtCreateRenderContext
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
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
            if (auto const pEvtCreateRenderContext = try_get_EvtCreateRenderContext())
            {
                return pEvtCreateRenderContext(ValuePathsCount, ValuePaths, Flags);
            }

            return  NULL;
        }

#endif



#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //EvtNext
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
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
            if (auto const pEvtNext = try_get_EvtNext())
            {
                return pEvtNext(ResultSet, EventsSize, Events, Timeout, Flags, Returned);
            }

            return  FALSE;
        }

#endif



#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //EvtQuery
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
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
            if (auto const pEvtQuery = try_get_EvtQuery())
            {
                return pEvtQuery(Session, Path, Query, Flags);
            }

            return  NULL;
        }

#endif




#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //EvtRender
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
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
            if (auto const pEvtRender = try_get_EvtRender())
            {
                return pEvtRender(Context, Fragment, Flags, BufferSize, Buffer, BufferUsed, PropertyCount);
            }

            return  FALSE;
        }

#endif
		
		

	}//namespace Thunks

} //namespace YY
