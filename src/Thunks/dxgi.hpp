#if (YY_Thunks_Target < __WindowsNT6_SP2)
#include <dxgi.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_SP2)

    __DEFINE_THUNK(
    dxgi,
    8,
    HRESULT,
    WINAPI,
    CreateDXGIFactory1,
        REFIID riid,
        _COM_Outptr_ void ** _ppFactory
        )
    {
        if (const auto _pfnCreateDXGIFactory1 = try_get_CreateDXGIFactory1())
        {
            return _pfnCreateDXGIFactory1(riid, _ppFactory);
        }

        if (_ppFactory)
            *_ppFactory = nullptr;
        return DXGI_ERROR_UNSUPPORTED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista
    // Minimum supported server	Windows Server 2008
    __DEFINE_THUNK(
    dxgi,
    8,
    HRESULT,
    STDAPICALLTYPE,
    CreateDXGIFactory,
        REFIID riid,
        _COM_Outptr_ void** _ppFactory
        )
    {
        if (auto const _pfnCreateDXGIFactory = try_get_CreateDXGIFactory())
        {
            return _pfnCreateDXGIFactory(riid, _ppFactory);
        }

        if (_ppFactory)
            *_ppFactory = nullptr;
        return DXGI_ERROR_UNSUPPORTED;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_3)
    // https://learn.microsoft.com/en-us/windows/win32/api/dxgi1_3/nf-dxgi1_3-createdxgifactory2
    // Minimum supported client	Windows 8.1
    // Minimum supported server	Windows Server 2012 R2
    __DEFINE_THUNK(
    dxgi,
    8,
    HRESULT,
    STDAPICALLTYPE,
    CreateDXGIFactory2,
        REFIID riid,
        _COM_Outptr_ void** _ppFactory
        )
    {
        if (auto const _pfnCreateDXGIFactory2 = try_get_CreateDXGIFactory2())
        {
            return _pfnCreateDXGIFactory2(riid, _ppFactory);
        }
        
        // 根据 VxKex 的实现，会映射到 CreateDXGIFactory1 （Win7）
        
        if (_ppFactory)
            *_ppFactory = nullptr;
        return DXGI_ERROR_UNSUPPORTED;
    }
#endif

}
