#if (YY_Thunks_Target < __WindowsNT6_3)
#include <ShellScalingAPI.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_3) && !defined(__Comment_Lib_user32)
#define __Comment_Lib_user32
#pragma comment(lib, "User32.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_3)

    //Windows 8.1 [desktop apps only]
    //Windows Server 2012 R2 [desktop apps only]
    __DEFINE_THUNK(
    shcore,
    16,
    HRESULT,
    STDAPICALLTYPE,
    GetDpiForMonitor,
        _In_ HMONITOR hmonitor,
        _In_ MONITOR_DPI_TYPE dpiType,
        _Out_ UINT* dpiX,
        _Out_ UINT* dpiY
        )
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


#if (YY_Thunks_Target < __WindowsNT6_3)

    //Windows 8.1 [desktop apps only]
    //Windows Server 2012 R2 [desktop apps only]
    __DEFINE_THUNK(
    shcore,
    4,
    HRESULT,
    STDAPICALLTYPE,
    SetProcessDpiAwareness,
        _In_ PROCESS_DPI_AWARENESS value
        )
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

#if (YY_Thunks_Target < __WindowsNT6_3)
    // https://learn.microsoft.com/en-us/windows/win32/api/shellscalingapi/nf-shellscalingapi-getprocessdpiawareness
    //Windows 8.1 [desktop apps only]
    //Windows Server 2012 R2 [desktop apps only]
    __DEFINE_THUNK(
    shcore,
    8,
    HRESULT,
    STDAPICALLTYPE,
    GetProcessDpiAwareness,
        _In_ HANDLE hprocess,
        _Out_ PROCESS_DPI_AWARENESS* value
        )
    {
        if (auto const pGetProcessDpiAwareness = try_get_GetProcessDpiAwareness())
        {
            return pGetProcessDpiAwareness(hprocess, value);
        }
        
        // 抄自 VxKex 源码，但其中有个 HANDLE 到 ULONG 的强转，会报警告
        if (hprocess == NULL || hprocess == NtCurrentProcess()
           ||  GetProcessId(hprocess) == (ULONG) NtCurrentTeb()->ClientId.UniqueProcess)
        {
            *value = IsProcessDPIAware() ? PROCESS_SYSTEM_DPI_AWARE : PROCESS_DPI_UNAWARE;
        } else {
            *value = PROCESS_DPI_UNAWARE;
        }

        return S_OK;
    }
#endif

} //namespace YY::Thunks
