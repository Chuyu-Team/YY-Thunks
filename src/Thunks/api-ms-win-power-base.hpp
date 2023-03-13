#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#include <powrprof.h>
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#pragma comment(lib, "PowrProf.lib")
#endif

namespace YY
{
	namespace Thunks
	{
#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		// 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
		// 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		powrprof,
		4,
		POWER_PLATFORM_ROLE,
		WINAPI,
		PowerDeterminePlatformRoleEx,
			_In_ ULONG _uVersion
			)
		{
			if (auto const _pfnPowerDeterminePlatformRoleEx = try_get_PowerDeterminePlatformRoleEx())
			{
				return _pfnPowerDeterminePlatformRoleEx(_uVersion);
			}

			if (_uVersion == POWER_PLATFORM_ROLE_V1)
			{
				return PowerDeterminePlatformRole();
			}
			else
			{
				return PlatformRoleUnspecified;
			}
		}
#endif
	}
}
