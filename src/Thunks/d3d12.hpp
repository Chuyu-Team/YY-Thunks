#if (YY_Thunks_Target < __WindowsNT10_10240)
#include <d3d12.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT10_10240)

    // Windows 10
    __DEFINE_THUNK(
    d3d12,
    16,
    HRESULT,
    WINAPI,
    D3D12CreateDevice,
        _In_opt_ IUnknown* pAdapter,
        D3D_FEATURE_LEVEL MinimumFeatureLevel,
        _In_ REFIID riid, // Expected: ID3D12Device
        _COM_Outptr_opt_ void** ppDevice)
    {
        if (const auto _pfnD3D12CreateDevice = try_get_D3D12CreateDevice())
        {
            return _pfnD3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
        }

        if (ppDevice)
            *ppDevice = nullptr;
        return E_NOINTERFACE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // Windows 10
    __DEFINE_THUNK(
    d3d12,
    8,
    HRESULT,
    WINAPI,
    D3D12GetDebugInterface,
        _In_ REFIID _oId,
        _COM_Outptr_opt_ void** _ppvDebug)
    {
        if (const auto _pfnD3D12GetDebugInterface = try_get_D3D12GetDebugInterface())
        {
            return _pfnD3D12GetDebugInterface(_oId, _ppvDebug);
        }

        if (_ppvDebug)
            *_ppvDebug = nullptr;

        return E_NOINTERFACE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // Windows 10
    __DEFINE_THUNK(
    d3d12,
    12,
    HRESULT,
    WINAPI,
    D3D12SerializeVersionedRootSignature ,
        _In_ const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* _pRootSignature,
        _Out_ ID3DBlob** _ppBlob,
        _Always_(_Outptr_opt_result_maybenull_) ID3DBlob** _ppErrorBlob)
    {
        if (const auto _pfnD3D12SerializeVersionedRootSignature = try_get_D3D12SerializeVersionedRootSignature())
        {
            return _pfnD3D12SerializeVersionedRootSignature(_pRootSignature, _ppBlob, _ppErrorBlob);
        }

        if (_ppBlob)
            *_ppBlob = nullptr;

        if (_ppErrorBlob)
            *_ppErrorBlob = nullptr;

        return E_NOINTERFACE;
    }
#endif
}
