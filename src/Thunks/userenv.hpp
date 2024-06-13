#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#include <userenv.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    // 最低受支持的客户端    Windows 8 [仅限桌面应用]
    // 最低受支持的服务器    Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    userenv,
    24,
    HRESULT,
    WINAPI,
    CreateAppContainerProfile,
        _In_ PCWSTR _szAppContainerName,
        _In_ PCWSTR _szDisplayName,
        _In_ PCWSTR _szDescription,
        _In_reads_opt_(_uCapabilityCount) PSID_AND_ATTRIBUTES _pCapabilities,
        _In_ DWORD _uCapabilityCount,
        _Outptr_ PSID* _ppSidAppContainerSid
        )
    {
        if (const auto _pfnCreateAppContainerProfile = try_get_CreateAppContainerProfile())
        {
            return _pfnCreateAppContainerProfile(_szAppContainerName, _szDisplayName, _szDescription, _pCapabilities, _uCapabilityCount, _ppSidAppContainerSid);
        }

        if (!_ppSidAppContainerSid)
            return E_INVALIDARG;
        *_ppSidAppContainerSid = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    // 最低受支持的客户端    Windows 8 [仅限桌面应用]
    // 最低受支持的服务器    Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    userenv,
    4,
    HRESULT,
    WINAPI,
    DeleteAppContainerProfile,
        _In_ PCWSTR _szAppContainerName
        )
    {
        if (const auto _pfnDeleteAppContainerProfile = try_get_DeleteAppContainerProfile())
        {
            return _pfnDeleteAppContainerProfile(_szAppContainerName);
        }

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    // 最低受支持的客户端    Windows 8 [仅限桌面应用]
    // 最低受支持的服务器    Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    userenv,
    8,
    HRESULT,
    WINAPI,
    DeriveAppContainerSidFromAppContainerName,
        _In_ PCWSTR _szAppContainerName,
        _Outptr_ PSID* _ppsidAppContainerSid
        )
    {
        if (const auto _pfnDeriveAppContainerSidFromAppContainerName = try_get_DeriveAppContainerSidFromAppContainerName())
        {
            return _pfnDeriveAppContainerSidFromAppContainerName(_szAppContainerName, _ppsidAppContainerSid);
        }
        if (!_ppsidAppContainerSid)
            return E_INVALIDARG;
        *_ppsidAppContainerSid = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    // 最低受支持的客户端    Windows 8 [仅限桌面应用]
    // 最低受支持的服务器    Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    userenv,
    8,
    HRESULT,
    WINAPI,
    GetAppContainerFolderPath,
        _In_ PCWSTR _szAppContainerSid,
        _Outptr_ PWSTR* _ppszPath
        )
    {
        if (const auto _pfnGetAppContainerFolderPath = try_get_GetAppContainerFolderPath())
        {
            return _pfnGetAppContainerFolderPath(_szAppContainerSid, _ppszPath);
        }
        if (!_ppszPath)
            return E_INVALIDARG;
        *_ppszPath = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    // 最低受支持的客户端    Windows 8 [仅限桌面应用]
    // 最低受支持的服务器    Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    userenv,
    8,
    HRESULT,
    WINAPI,
    GetAppContainerRegistryLocation,
        _In_ REGSAM _DesiredAccess,
        _Outptr_ PHKEY _phAppContainerKey
        )
    {
        if (const auto _pfnGetAppContainerRegistryLocation = try_get_GetAppContainerRegistryLocation())
        {
            return _pfnGetAppContainerRegistryLocation(_DesiredAccess, _phAppContainerKey);
        }
        if (!_phAppContainerKey)
            return E_INVALIDARG;
        *_phAppContainerKey = NULL;
        return E_NOTIMPL;
    }
#endif
} // namespace YY::Thunks
