#if (YY_Thunks_Support_Version < NTDDI_VISTA)
#include <powrprof.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_VISTA)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    powrprof,
    0,
    POWER_PLATFORM_ROLE,
    WINAPI,
    PowerDeterminePlatformRole,
        )
    {
        if (auto const _pfnPowerDeterminePlatformRole = try_get_PowerDeterminePlatformRole())
        {
            return _pfnPowerDeterminePlatformRole();
        }

        // 假定自己没有电池
        return PlatformRoleDesktop;
    }
#endif
} // namespace YY::Thunks
