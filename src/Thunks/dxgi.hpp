#include <dxgi.h>

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN6SP2)

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


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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
}
