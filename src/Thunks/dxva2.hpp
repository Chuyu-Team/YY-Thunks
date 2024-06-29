#if (YY_Thunks_Target < __WindowsNT6)
#include <d3d9.h>
#include <dxva2api.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    dxva2,
    8,
    HRESULT,
    WINAPI,
    DXVA2CreateDirect3DDeviceManager9,
        _Out_ UINT* pResetToken,
        _Outptr_ IDirect3DDeviceManager9** ppDeviceManager
        )
    {
        if (const auto _pfnDXVA2CreateDirect3DDeviceManager9 = try_get_DXVA2CreateDirect3DDeviceManager9())
        {
            return _pfnDXVA2CreateDirect3DDeviceManager9(pResetToken, ppDeviceManager);
        }
        
        if (ppDeviceManager)
            *ppDeviceManager = nullptr;
        return E_NOINTERFACE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    dxva2,
    12,
    HRESULT,
    WINAPI,
    DXVA2CreateVideoService,
        _In_ IDirect3DDevice9* pDD,
        _In_ REFIID riid,
        _Outptr_ void** ppService
        )
    {
        if (const auto _pfnDXVA2CreateVideoService = try_get_DXVA2CreateVideoService())
        {
            return _pfnDXVA2CreateVideoService(pDD, riid, ppService);
        }
        
        if (ppService)
            *ppService = nullptr;
        return E_NOINTERFACE;
    }
#endif
}
