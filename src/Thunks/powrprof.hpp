#if (YY_Thunks_Target < __WindowsNT6)
#include <powrprof.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_powrprof)
#define __Comment_Lib_powrprof
#pragma comment(lib, "PowrProf.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_shlwapi)
#define __Comment_Lib_shlwapi
#pragma comment(lib, "Shlwapi.lib")
#endif

#if defined(YY_Thunks_Implemented)
namespace YY::Thunks::Fallback::PowrProf
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT6)
        // 节能模式
        static constexpr const GUID kPowerSaver = { 0xa1841308, 0x3541, 0x4fab, { 0xbc, 0x81, 0xf7, 0x15, 0x56, 0xf2, 0x0b, 0x4a } };
        // 平衡模式
        static constexpr const GUID kBalanced = { 0x381b4222, 0xf694, 0x41f0, { 0x96, 0x85, 0xff, 0x5b, 0xb2, 0x60, 0xdf, 0x2e } };
        // 高性能模式
        static constexpr const GUID kHighPerformance = { 0x8c5e7fda, 0xe8bf, 0x4a96, { 0x9a, 0x85, 0xa6, 0xe2, 0x3a, 0x8c, 0x63, 0x5c } };

        static int __fastcall PowerPolicyGuidToPowerSchemeId(const GUID& PolicyGuid) noexcept
        {
            if (IsEqualGUID(PolicyGuid, kPowerSaver))
            {
                return 5;
            }
            else if (IsEqualGUID(PolicyGuid, kBalanced))
            {
                return 0;
            }
            else if (IsEqualGUID(PolicyGuid, kHighPerformance))
            {
                return 4;
            }
            else if (IsEqualGUID(PolicyGuid, GUID{ PolicyGuid.Data1 }))
            {
                return PolicyGuid.Data1;
            }
            else
            {
                return -1;
            }
        }

        // 恢复时提示输入密码
        // https://learn.microsoft.com/zh-cn/windows-hardware/customize/power-settings/no-subgroup-settings-prompt-for-password-on-resume
        constexpr GUID kConsoleLock = { 0x0E796BDB, 0x100D, 0x47D6, {0xA2, 0xD5, 0xF7, 0xD2, 0xDA, 0xA5, 0x1F, 0x51} };

        static DWORD WINAPI PowerReadSystemValue(
            _In_opt_ HKEY _hRootPowerKey,
            _In_opt_ CONST GUID* _pSchemeGuid,
            _In_opt_ CONST GUID* _pSubGroupOfPowerSettingsGuid,
            _In_opt_ CONST GUID* _pPowerSettingGuid,
            _Out_opt_ PULONG _puType,
            _Out_writes_bytes_opt_(*_puBufferSize) LPBYTE _pBuffer,
            _Inout_ LPDWORD _puBufferSize)
        {
            if (_pSchemeGuid == nullptr || _pPowerSettingGuid == nullptr)
            {
                *_puBufferSize = 0ul;
                return ERROR_INVALID_PARAMETER;
            }

            if (!IsEqualGUID(*_pSchemeGuid, Fallback::PowrProf::kBalanced))
                return ERROR_FILE_NOT_FOUND;

            if (IsEqualGUID(kConsoleLock, *_pPowerSettingGuid) && (_pSubGroupOfPowerSettingsGuid == nullptr || IsEqualGUID(*_pSubGroupOfPowerSettingsGuid, NO_SUBGROUP_GUID)))
            {
                if (_puType)
                    *_puType = REG_DWORD;

                if (!_pBuffer)
                {
                    *_puBufferSize = sizeof(DWORD);
                    return ERROR_SUCCESS;
                }

                if (*_puBufferSize < sizeof(DWORD))
                {
                    *_puBufferSize = sizeof(DWORD);
                    return ERROR_MORE_DATA;
                }

                *_puBufferSize = sizeof(DWORD);

                DWORD _bPromptPasswordOnResume = FALSE;
                DWORD _cbData = sizeof(_bPromptPasswordOnResume);
                if (SHGetValueW(HKEY_CURRENT_USER, L"Software\\Policies\\Microsoft\\Windows\\System\\Power", L"PromptPasswordOnResume", nullptr, &_bPromptPasswordOnResume, &_cbData) == ERROR_SUCCESS)
                {
                    *(DWORD*)_pBuffer = _bPromptPasswordOnResume;
                }
                return ERROR_SUCCESS;
            }

            return ERROR_FILE_NOT_FOUND;
        }
#endif
    }
}
#endif


namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
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

    
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    powrprof,
    8,
    DWORD,
    WINAPI,
    PowerGetActiveScheme,
        _In_opt_ HKEY _hUserRootPowerKey,
        _Outptr_ GUID** _ppActivePolicyGuid
        )
    {
        if (auto const _pfnPowerGetActiveScheme = try_get_PowerGetActiveScheme())
            return _pfnPowerGetActiveScheme(_hUserRootPowerKey, _ppActivePolicyGuid);

#if 1
        // TODO(YY): 未来尝试进行映射，暂时我们固定返回平衡模式
        auto _pPolicyGuid = (GUID*)LocalAlloc(LMEM_FIXED, sizeof(GUID));
        if (!_pPolicyGuid)
            return ERROR_NOT_ENOUGH_MEMORY;

        *_pPolicyGuid = Fallback::PowrProf::kBalanced;
        return ERROR_SUCCESS;
#else
        UINT _uPwrSchemeId = 0;
        if (!GetActivePwrScheme(&_uPwrSchemeId))
            return GetLastError();

        // 映射关系如下：
        //    PwrSchemeId    | PolicyGuid
        // 0 : 家用/办公桌   | 381b4222-f694-41f0-9685-ff5bb260df2e :平衡
        // 1 : 便携/袖珍式   | 
        // 2 : 演示模式      | 
        // 3 : 一直开着      | 
        // 4 : 最少电源管理  | 8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c : 高性能
        // 5 : 最大电池模式  | a1841308-3541-4fab-bc81-f71556f20b4a : 节能
        auto _pPolicyGuid = (GUID*)LocalAlloc(LMEM_FIXED, sizeof(GUID));
        if (!_pPolicyGuid)
            return ERROR_NOT_ENOUGH_MEMORY;

        switch (_uPwrSchemeId)
        {
        case 0:
            // 平衡
            *_pPolicyGuid = Fallback::PowrProf::kBalanced;
            break;
        case 4:
            // 高性能
            *_pPolicyGuid = Fallback::PowrProf::kHighPerformance;
            break;
        case 5:
            // 节能
            *_pPolicyGuid = Fallback::PowrProf::kPowerSaver;
            break;
        default:
            // GUID尾部全为0
            *_pPolicyGuid = GUID{ _uPwrSchemeId };
            break;
        }

        *_ppActivePolicyGuid = _pPolicyGuid;
        return ERROR_SUCCESS;
#endif
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    powrprof,
    28,
    DWORD,
    WINAPI,
    PowerReadACValue,
        _In_opt_ HKEY _hRootPowerKey,
        _In_opt_ CONST GUID* _pSchemeGuid,
        _In_opt_ CONST GUID* _pSubGroupOfPowerSettingsGuid,
        _In_opt_ CONST GUID* _pPowerSettingGuid,
        _Out_opt_ PULONG _puType,
        _Out_writes_bytes_opt_(*_puBufferSize) LPBYTE _pBuffer,
        _Inout_ LPDWORD _puBufferSize
        )
    {
        if (auto const _pfnPowerReadACValue = try_get_PowerReadACValue())
            return _pfnPowerReadACValue(_hRootPowerKey, _pSchemeGuid, _pSubGroupOfPowerSettingsGuid, _pPowerSettingGuid, _puType, _pBuffer, _puBufferSize);
        
        return Fallback::PowrProf::PowerReadSystemValue(_hRootPowerKey, _pSchemeGuid, _pSubGroupOfPowerSettingsGuid, _pPowerSettingGuid, _puType, _pBuffer, _puBufferSize);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    powrprof,
    28,
    DWORD,
    WINAPI,
    PowerReadDCValue,
        _In_opt_ HKEY _hRootPowerKey,
        _In_opt_ CONST GUID* _pSchemeGuid,
        _In_opt_ CONST GUID* _pSubGroupOfPowerSettingsGuid,
        _In_opt_ CONST GUID* _pPowerSettingGuid,
        _Out_opt_ PULONG _puType,
        _Out_writes_bytes_opt_(*_puBufferSize) LPBYTE _pBuffer,
        _Inout_ LPDWORD _puBufferSize
        )
    {
        if (auto const _pfnPowerReadDCValue = try_get_PowerReadDCValue())
            return _pfnPowerReadDCValue(_hRootPowerKey, _pSchemeGuid, _pSubGroupOfPowerSettingsGuid, _pPowerSettingGuid, _puType, _pBuffer, _puBufferSize);
        
        return Fallback::PowrProf::PowerReadSystemValue(_hRootPowerKey, _pSchemeGuid, _pSubGroupOfPowerSettingsGuid, _pPowerSettingGuid, _puType, _pBuffer, _puBufferSize);
    }
#endif
} // namespace YY::Thunks
