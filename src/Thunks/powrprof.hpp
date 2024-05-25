#if (YY_Thunks_Support_Version < NTDDI_VISTA)
#include <powrprof.h>
#endif

namespace YY::Thunks
{

#if (YY_Thunks_Support_Version < NTDDI_VISTA)

    // �����֧�ֵĿͻ���    Windows 8 [����Ӧ��|UWP Ӧ��]
    // �����֧�ֵķ�����    Windows Server 2012[����Ӧ�� | UWP Ӧ��]
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

        // �ٶ��Լ�û�е��
        return PlatformRoleDesktop;
    }
#endif

} // namespace YY::Thunks
