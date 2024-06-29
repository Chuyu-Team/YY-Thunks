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
}
