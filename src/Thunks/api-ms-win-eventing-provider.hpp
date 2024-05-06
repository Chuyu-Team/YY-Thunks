#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#include <evntprov.h>
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
#include <evntrace.h>
#endif

namespace YY
{
	namespace Thunks
	{
#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		// 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
		// 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		advapi32,
		20,
		ULONG,
		__stdcall,
		EventSetInformation,
			_In_ REGHANDLE _hRegHandle,
			_In_ EVENT_INFO_CLASS _eInformationClass,
			_In_reads_bytes_(_cbInformationLength) PVOID _pEventInformation,
			_In_ ULONG _cbInformationLength
			)
		{
			if (auto const _pfnEventSetInformation = try_get_EventSetInformation())
			{
				return _pfnEventSetInformation(_hRegHandle, _eInformationClass, _pEventInformation, _cbInformationLength);
			}

			return ERROR_NOT_SUPPORTED;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
		__DEFINE_THUNK(
		advapi32,
		8,
		ULONG,
        WINAPI,
        EventActivityIdControl,
            _In_ ULONG _uControlCode,
            _Inout_ LPGUID _pActivityId
			)
		{
			if (auto const _pfnEventActivityIdControl = try_get_EventActivityIdControl())
			{
				return _pfnEventActivityIdControl(_uControlCode, _pActivityId);
			}
            
			return ERROR_NOT_SUPPORTED;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
		__DEFINE_THUNK(
		advapi32,
		16,
		ULONG,
        WINAPI,
        EventRegister,
            _In_ LPCGUID _pProviderId,
            _In_opt_ PENABLECALLBACK _pfnEnableCallback,
            _In_opt_ PVOID _pCallbackContext,
            _Out_ PREGHANDLE _phRegHandle
			)
		{
			if (auto const _pfnEventRegister = try_get_EventRegister())
			{
				return _pfnEventRegister(_pProviderId, _pfnEnableCallback, _pCallbackContext, _phRegHandle);
			}
            
			return ERROR_NOT_SUPPORTED;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
		__DEFINE_THUNK(
		advapi32,
		8,
		ULONG,
        WINAPI,
        EventUnregister,
            _In_ REGHANDLE _hRegHandle
			)
		{
			if (auto const _pfnEventUnregister = try_get_EventUnregister())
			{
				return _pfnEventUnregister(_hRegHandle);
			}
			return ERROR_NOT_SUPPORTED;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
		__DEFINE_THUNK(
		advapi32,
		24,
        ULONG,
        WINAPI,
        EnumerateTraceGuidsEx,
            _In_ TRACE_QUERY_INFO_CLASS TraceQueryInfoClass,
            _In_reads_bytes_opt_(InBufferSize) PVOID InBuffer,
            _In_ ULONG InBufferSize,
            _Out_writes_bytes_opt_(OutBufferSize) PVOID OutBuffer,
            _In_ ULONG OutBufferSize,
            _Out_ PULONG ReturnLength
			)
		{
			if (auto const _pfnEnumerateTraceGuidsEx = try_get_EnumerateTraceGuidsEx())
			{
				return _pfnEnumerateTraceGuidsEx(TraceQueryInfoClass, InBuffer, InBufferSize, OutBuffer, OutBufferSize, ReturnLength);
			}
			return ERROR_NOT_SUPPORTED;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
		__DEFINE_THUNK(
		advapi32,
		28,
        ULONG,
        WINAPI,
        EventWriteTransfer,
            _In_ REGHANDLE RegHandle,
            _In_ PCEVENT_DESCRIPTOR EventDescriptor,
            _In_opt_ LPCGUID ActivityId,
            _In_opt_ LPCGUID RelatedActivityId,
            _In_range_(0, MAX_EVENT_DATA_DESCRIPTORS) ULONG UserDataCount,
            _In_reads_opt_(UserDataCount) PEVENT_DATA_DESCRIPTOR UserData
			)
		{
			if (auto const _pfnEventWriteTransfer = try_get_EventWriteTransfer())
			{
				return _pfnEventWriteTransfer(RegHandle, EventDescriptor, ActivityId, RelatedActivityId, UserDataCount, UserData);
			}
            
			return ERROR_NOT_SUPPORTED;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
		__DEFINE_THUNK(
		advapi32,
		12,
        BOOLEAN,
        WINAPI,
        EventEnabled,
            _In_ REGHANDLE RegHandle,
            _In_ PCEVENT_DESCRIPTOR EventDescriptor
			)
		{
			if (auto const _pfnEventEnabled = try_get_EventEnabled())
			{
				return _pfnEventEnabled(RegHandle, EventDescriptor);
			}
            
			return FALSE;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
        // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		advapi32,
		20,
        ULONG,
        WINAPI,
        EventWrite,
            _In_ REGHANDLE RegHandle,
            _In_ PCEVENT_DESCRIPTOR EventDescriptor,
            _In_range_(0, MAX_EVENT_DATA_DESCRIPTORS) ULONG UserDataCount,
            _In_reads_opt_(UserDataCount) PEVENT_DATA_DESCRIPTOR UserData
			)
		{
			if (auto const _pfnEventWrite = try_get_EventWrite())
			{
				return _pfnEventWrite(RegHandle, EventDescriptor, UserDataCount, UserData);
			}
            
			return ERROR_NOT_SUPPORTED;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		// 最低受支持的客户端	Windows 7 [桌面应用 |UWP 应用]
        // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		advapi32,
		40,
        ULONG,
        WINAPI,
        EventWriteEx,
            _In_ REGHANDLE RegHandle,
            _In_ PCEVENT_DESCRIPTOR EventDescriptor,
            _In_ ULONG64 Filter,
            _In_ ULONG Flags,
            _In_opt_ LPCGUID ActivityId,
            _In_opt_ LPCGUID RelatedActivityId,
            _In_range_(0, MAX_EVENT_DATA_DESCRIPTORS) ULONG UserDataCount,
            _In_reads_opt_(UserDataCount) PEVENT_DATA_DESCRIPTOR UserData
			)
		{
			if (auto const _pfnEventWriteEx = try_get_EventWriteEx())
			{
				return _pfnEventWriteEx(RegHandle, EventDescriptor, Filter, Flags, ActivityId, RelatedActivityId, UserDataCount, UserData);
			}
            
			return ::EventWriteTransfer(RegHandle, EventDescriptor, ActivityId, RelatedActivityId, UserDataCount, UserData);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
        // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		advapi32,
		24,
        ULONG,
        WINAPI,
        EventWriteString,
            _In_ REGHANDLE RegHandle,
            _In_ UCHAR Level,
            _In_ ULONGLONG Keyword,
            _In_ PCWSTR String
			)
		{
			if (auto const _pfnEventWriteString = try_get_EventWriteString())
			{
				return _pfnEventWriteString(RegHandle, Level, Keyword, String);
			}
            return ERROR_NOT_SUPPORTED;
		}
#endif
	}
}
