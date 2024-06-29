#if (YY_Thunks_Target < __WindowsNT6)
#include <dpapi.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    crypt32,
    12,
    BOOL,
    WINAPI,
    CryptProtectMemory,
        _Inout_         LPVOID          pDataIn,
        _In_            DWORD           cbDataIn,
        _In_            DWORD           dwFlags
        )
    {
        if (const auto _pfnProcessPrng = try_get_CryptProtectMemory())
        {
            return _pfnProcessPrng(pDataIn, cbDataIn, dwFlags);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    crypt32,
    12,
    BOOL,
    WINAPI,
    CryptUnprotectMemory,
        _Inout_         LPVOID          pDataIn,
        _In_            DWORD           cbDataIn,
        _In_            DWORD           dwFlags
        )
    {
        if (const auto _pfnProcessPrng = try_get_CryptUnprotectMemory())
        {
            return _pfnProcessPrng(pDataIn, cbDataIn, dwFlags);
        }
            
        return TRUE;
    }
#endif
} // namespace YY::Thunks
