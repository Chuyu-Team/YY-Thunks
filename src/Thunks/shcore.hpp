﻿#if (YY_Thunks_Target < __WindowsNT6_3)
#include <ShellScalingAPI.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_3) && !defined(__Comment_Lib_user32)
#define __Comment_Lib_user32
#pragma comment(lib, "User32.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)
#include <shcore.h>
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

    // Windows 8.1 [desktop apps only]
    // Windows Server 2012 R2 [desktop apps only]
    __DEFINE_THUNK(
    shcore,
    4,
    HRESULT,
    STDAPICALLTYPE,
    SetProcessDpiAwareness,
        _In_ PROCESS_DPI_AWARENESS _eValue
        )
    {
        if (auto const _pfnSetProcessDpiAwareness = try_get_SetProcessDpiAwareness())
        {
            return _pfnSetProcessDpiAwareness(_eValue);
        }

        switch (_eValue)
        {
        case PROCESS_DPI_UNAWARE:
            return S_OK;
        case PROCESS_SYSTEM_DPI_AWARE:
        case PROCESS_PER_MONITOR_DPI_AWARE:
            return SetProcessDPIAware() ? S_OK : E_FAIL;
        default:
            return E_INVALIDARG;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_3)

    // Windows 8.1 [desktop apps only]
    // Windows Server 2012 R2 [desktop apps only]
    __DEFINE_THUNK(
    shcore,
    8,
    HRESULT,
    STDAPICALLTYPE,
    GetProcessDpiAwareness,
        _In_ HANDLE _hProcess,
        _Out_ PROCESS_DPI_AWARENESS* _peValue
        )
    {
        if (auto const _pfnGetProcessDpiAwareness = try_get_GetProcessDpiAwareness())
        {
            return _pfnGetProcessDpiAwareness(_hProcess, _peValue);
        }
        
        if (_hProcess == NULL || _hProcess == NtCurrentProcess()
            || GetProcessId(_hProcess) == (ULONG)NtCurrentTeb()->ClientId.UniqueProcess)
        {
            *_peValue = IsProcessDPIAware() ? PROCESS_SYSTEM_DPI_AWARE : PROCESS_DPI_UNAWARE;
        }
        else
        {
            *_peValue = PROCESS_DPI_UNAWARE;
        }
        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    shcore,
    16,
    HRESULT,
    STDAPICALLTYPE,
    CreateRandomAccessStreamOverStream,
        _In_ IStream* _pStream,
        _In_ BSOS_OPTIONS _eOptions,
        _In_ REFIID _riid,
        _COM_Outptr_ void** _ppv
        )
    {
        if (auto const _pfnCreateRandomAccessStreamOverStream = try_get_CreateRandomAccessStreamOverStream())
        {
            return _pfnCreateRandomAccessStreamOverStream(_pStream, _eOptions, _riid, _ppv);
        }

        if (!_ppv)
            return E_POINTER;

        *_ppv = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    shcore,
    16,
    HRESULT,
    STDAPICALLTYPE,
    CreateRandomAccessStreamOnFile,
        _In_ PCWSTR _szFilePath, 
        _In_ DWORD _fAccessMode, 
        _In_ REFIID _Id, 
        _COM_Outptr_ void** _ppv
        )
    {
        if (auto const _pfnCreateRandomAccessStreamOnFile = try_get_CreateRandomAccessStreamOnFile())
        {
            return _pfnCreateRandomAccessStreamOnFile(_szFilePath, _fAccessMode, _Id, _ppv);
        }

        if (!_ppv)
            return E_POINTER;

        *_ppv = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    shcore,
    12,
    HRESULT,
    STDAPICALLTYPE,
    CreateStreamOverRandomAccessStream,
        _In_ IUnknown* _pRandomAccessStream, 
        _In_ REFIID _Id, 
        _COM_Outptr_ void**_ppv
        )
    {
        if (auto const _pfnCreateStreamOverRandomAccessStream = try_get_CreateStreamOverRandomAccessStream())
        {
            return _pfnCreateStreamOverRandomAccessStream(_pRandomAccessStream, _Id, _ppv);
        }

        if (!_ppv)
            return E_POINTER;

        *_ppv = nullptr;
        return E_NOTIMPL;
    }
#endif

} //namespace YY::Thunks
