#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#include <evntprov.h>
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
	}
}
