#if (YY_Thunks_Target < __WindowsNT10_10240)
#include <dcomp.h>
#endif

#if (YY_Thunks_Target < __WindowsNT10_10240) && (YY_Thunks_Target >= __WindowsNT6_2 || __YY_Thunks_libs) && !defined(__Comment_Lib_dcomp)
#define __Comment_Lib_dcomp
#pragma comment(lib, "Dcomp.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    dcomp,
    12,
    HRESULT,
    WINAPI,
    DCompositionCreateDevice,
        _In_opt_ IDXGIDevice* _pDxgiDevice,
        _In_ REFIID iid,
        _Outptr_ void** _pDcompositionDevice
        )
    {
        if (const auto _pfnDCompositionCreateDevice = try_get_DCompositionCreateDevice())
        {
            return _pfnDCompositionCreateDevice(_pDxgiDevice, iid, _pDcompositionDevice);
        }

        if (!_pDcompositionDevice)
            return E_POINTER;

        *_pDcompositionDevice = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_3)

    // 最低受支持的客户端	Windows 8.1 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 R2[仅限桌面应用]
    __DEFINE_THUNK(
    dcomp,
    12,
    HRESULT,
    WINAPI,
    DCompositionCreateDevice2,
        _In_opt_ IUnknown* _pRenderingDevice,
        _In_ REFIID iid,
        _Outptr_ void** _pDcompositionDevice
        )
    {
        if (const auto _pfnDCompositionCreateDevice2 = try_get_DCompositionCreateDevice2())
        {
            return _pfnDCompositionCreateDevice2(_pRenderingDevice, iid, _pDcompositionDevice);
        }

        if (!_pDcompositionDevice)
            return E_POINTER;

        *_pDcompositionDevice = nullptr;

        IDXGIDevice* _pDxgiDevice = nullptr;
        if (_pRenderingDevice)
        {
            auto _hr = _pRenderingDevice->QueryInterface(IID_PPV_ARGS(&_pDxgiDevice));
            if(FAILED(_hr))
            {
                return _hr;
            }

            if (!_pDxgiDevice)
            {
                return E_NOINTERFACE;
            }
        }

        auto _hr = DCompositionCreateDevice(_pDxgiDevice, iid, _pDcompositionDevice);
        if (_pDxgiDevice)
        {
            _pDxgiDevice->Release();
        }

        return _hr;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端	Windows 10.0.10240
    __DEFINE_THUNK(
    dcomp,
    12,
    HRESULT,
    WINAPI,
    DCompositionCreateDevice3,
        _In_opt_ IUnknown* _pRenderingDevice,
        _In_ REFIID iid,
        _Outptr_ void** _pDcompositionDevice
        )
    {
        if (const auto _pfnDCompositionCreateDevice3 = try_get_DCompositionCreateDevice3())
        {
            return _pfnDCompositionCreateDevice3(_pRenderingDevice, iid, _pDcompositionDevice);
        }

        return DCompositionCreateDevice2(_pRenderingDevice, iid, _pDcompositionDevice);
    }
#endif
}
