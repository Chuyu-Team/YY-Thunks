#if (YY_Thunks_Target < __WindowsNT6_1)
#include <d3d11.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_1)

    // Windows 7自带，Vista需要安装KB971644
    __DEFINE_THUNK(
    d3d11,
    40,
    HRESULT,
    WINAPI,
    D3D11CreateDevice,
        _In_opt_ IDXGIAdapter* pAdapter,
        D3D_DRIVER_TYPE DriverType,
        HMODULE Software,
        UINT Flags,
        _In_reads_opt_( FeatureLevels ) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
        UINT FeatureLevels,
        UINT SDKVersion,
        _COM_Outptr_opt_ ID3D11Device** ppDevice,
        _Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
        _COM_Outptr_opt_ ID3D11DeviceContext** ppImmediateContext
        )
    {
        if (const auto _pfnD3D11CreateDevice = try_get_D3D11CreateDevice())
        {
            return _pfnD3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
        }

        if (ppDevice)
            *ppDevice = nullptr;

        if (ppImmediateContext)
            *ppImmediateContext = nullptr;
        
        return E_NOINTERFACE;
    }
#endif
}
