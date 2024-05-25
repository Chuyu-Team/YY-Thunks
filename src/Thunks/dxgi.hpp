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
        return E_NOINTERFACE;
    }
#endif
}
