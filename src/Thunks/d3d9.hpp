#if (YY_Thunks_Target < __WindowsNT6)
#include <d3d9.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    __DEFINE_THUNK(
    d3d9,
    8,
    HRESULT,
    WINAPI,
    Direct3DCreate9Ex,
        UINT SDKVersion,
        IDirect3D9Ex** unnamedParam2
        )
    {
        if (const auto _pfnDirect3DCreate9Ex = try_get_Direct3DCreate9Ex())
        {
            return _pfnDirect3DCreate9Ex(SDKVersion, unnamedParam2);
        }
        
        if (unnamedParam2)
            *unnamedParam2 = nullptr;
        return D3DERR_NOTAVAILABLE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1_SP3)

    __DEFINE_THUNK(
    d3d9,
    4,
    IDirect3D9*,
    WINAPI,
    Direct3DCreate9,
        UINT SDKVersion
        )
    {
        if (const auto _pfnDirect3DCreate9 = try_get_Direct3DCreate9())
        {
            return _pfnDirect3DCreate9(SDKVersion);
        }

        return nullptr;
    }
#endif
}
