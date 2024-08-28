#if (YY_Thunks_Target < __WindowsNT6_3)
#include <dxgi1_3.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_3) && !defined(__Comment_Lib_dxgi)
#define __Comment_Lib_dxgi
#pragma comment(lib, "DXGI.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista
    // Minimum supported server	Windows Server 2008
    __DEFINE_THUNK(
    dxgi,
    8,
    HRESULT,
    STDAPICALLTYPE,
    CreateDXGIFactory,
        REFIID _IID,
        _COM_Outptr_ void** _ppFactory
        )
    {
        if (auto const _pfnCreateDXGIFactory = try_get_CreateDXGIFactory())
        {
            return _pfnCreateDXGIFactory(_IID, _ppFactory);
        }

        if (_ppFactory)
            *_ppFactory = nullptr;
        return DXGI_ERROR_UNSUPPORTED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_SP2)

    // 最低受支持的客户端	Windows 7 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    dxgi,
    8,
    HRESULT,
    WINAPI,
    CreateDXGIFactory1,
        REFIID _IID,
        _COM_Outptr_ void ** _ppFactory
        )
    {
        if (const auto _pfnCreateDXGIFactory1 = try_get_CreateDXGIFactory1())
        {
            return _pfnCreateDXGIFactory1(_IID, _ppFactory);
        }

        return CreateDXGIFactory(_IID, _ppFactory);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_3)

    // Minimum supported client	Windows 8.1
    // Minimum supported server	Windows Server 2012 R2
    __DEFINE_THUNK(
    dxgi,
    12,
    HRESULT,
    STDAPICALLTYPE,
    CreateDXGIFactory2,
        UINT   _fFlags,
        REFIID _IID,
        _COM_Outptr_ void** _ppFactory
        )
    {
        if (auto const _pfnCreateDXGIFactory2 = try_get_CreateDXGIFactory2())
        {
            return _pfnCreateDXGIFactory2(_fFlags, _IID, _ppFactory);
        }
        
        return CreateDXGIFactory1(_IID, _ppFactory);
    }
#endif
}
