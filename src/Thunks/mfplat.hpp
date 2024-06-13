#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#include <mfapi.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    8,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateDXGIDeviceManager,
        _Out_ UINT* _puResetToken,
        _Outptr_ IMFDXGIDeviceManager** _ppDeviceManager
        )
    {
        if (const auto _pfnMFCreateDXGIDeviceManager = try_get_MFCreateDXGIDeviceManager())
        {
            return _pfnMFCreateDXGIDeviceManager(_puResetToken, _ppDeviceManager);
        }

        if (_puResetToken)
            *_puResetToken = 0;
        if (_ppDeviceManager)
            *_ppDeviceManager = nullptr;

        if (_puResetToken == nullptr || _ppDeviceManager == nullptr)
            return E_INVALIDARG;
        return E_NOTIMPL;
    }
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    20,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateDXGISurfaceBuffer,
        _In_ REFIID _id,
        _In_ IUnknown* _punkSurface,
        _In_ UINT _uSubresourceIndex,
        _In_ BOOL _bBottomUpWhenLinear,
        _Outptr_ IMFMediaBuffer** _ppBuffer
        )
    {
        if (const auto _pfnMFCreateDXGISurfaceBuffer = try_get_MFCreateDXGISurfaceBuffer())
        {
            return _pfnMFCreateDXGISurfaceBuffer(_id, _punkSurface, _uSubresourceIndex, _bBottomUpWhenLinear, _ppBuffer);
        }

        if(!_ppBuffer)
            return E_INVALIDARG;
        *_ppBuffer = nullptr;
        return E_NOTIMPL;
    }
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    8,
    HRESULT,
    STDAPICALLTYPE,
    MFLockDXGIDeviceManager,
        _Out_opt_ UINT* _puResetToken,
        _Outptr_ IMFDXGIDeviceManager** _ppManager
        )
    {
        if (const auto _pfnMFLockDXGIDeviceManager = try_get_MFLockDXGIDeviceManager())
        {
            return _pfnMFLockDXGIDeviceManager(_puResetToken, _ppManager);
        }

        if (_puResetToken)
            *_puResetToken = 0;
        if (_ppManager)
            *_ppManager = nullptr;
        if(_puResetToken == nullptr || _ppManager == nullptr)
            return E_INVALIDARG;
        return E_NOTIMPL;
    }
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    0,
    HRESULT,
    STDAPICALLTYPE,
    MFUnlockDXGIDeviceManager,
        )
    {
        if (const auto _pfnMFUnlockDXGIDeviceManager = try_get_MFUnlockDXGIDeviceManager())
        {
            return _pfnMFUnlockDXGIDeviceManager();
        }

        return E_NOTIMPL;
    }
#endif

} // namespace YY::Thunks
