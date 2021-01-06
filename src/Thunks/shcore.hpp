
#include <ShellScalingAPI.h>

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WINBLUE)
//Windows 8.1 [desktop apps only]
//Windows Server 2012 R2 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
GetDpiForMonitor(
	_In_ HMONITOR hmonitor,
	_In_ MONITOR_DPI_TYPE dpiType,
	_Out_ UINT* dpiX,
	_Out_ UINT* dpiY
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pGetDpiForMonitor = try_get_GetDpiForMonitor())
	{
		return pGetDpiForMonitor(hmonitor, dpiType, dpiX, dpiY);
	}

	if (dpiX == nullptr || dpiY == nullptr)
	{
		return __HRESULT_FROM_WIN32(ERROR_INVALID_ADDRESS);
	}

	auto DpiForSystem = internal::GetDpiForSystemDownlevel();

	*dpiX = DpiForSystem;
	*dpiY = DpiForSystem;

	return S_OK;
}
#endif

__YY_Thunks_Expand_Function(shcore, GetDpiForMonitor, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WINBLUE)
//Windows 8.1 [desktop apps only]
//Windows Server 2012 R2 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SetProcessDpiAwareness(
	_In_ PROCESS_DPI_AWARENESS value
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pSetProcessDpiAwareness = try_get_SetProcessDpiAwareness())
	{
		return pSetProcessDpiAwareness(value);
	}

	if (value != PROCESS_DPI_UNAWARE)
	{
		return SetProcessDPIAware() ? S_OK : E_FAIL;
	}

	return S_OK;
}
#endif

__YY_Thunks_Expand_Function(shcore, SetProcessDpiAwareness, 4);

#endif

	}//namespace Thunks

} //namespace YY