#if (YY_Thunks_Target < __WindowsNT6)
#include <wincred.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_credui)
#define __Comment_Lib_credui
#pragma comment(lib, "Credui.Lib")
#endif

#ifdef YY_Thunks_Implemented
namespace YY::Thunks::Fallback {
namespace {

#if (YY_Thunks_Target < __WindowsNT6)
template<typename Char>
BOOL WINAPI CredPackAuthenticationBufferDownlevel(
    _In_ DWORD                                      dwFlags,
    _In_ Char*                                     pszUserName,
    _In_ Char*                                     pszPassword,
    _Out_writes_bytes_opt_(*pcbPackedCredentials) PBYTE   pPackedCredentials,
    _Inout_ DWORD* pcbPackedCredentials
    )
{
    // 计算所需格式大小： [ULong cbHeader][ULong cbUser][ULong cbPassword][User][Password]
    ULONG cbUser = internal::StringLength(pszUserName) * sizeof(Char);
    ULONG cbPassword = internal::StringLength(pszPassword) * sizeof(Char);

    ULONG cbTotal = sizeof(ULONG) * 3 + cbUser + cbPassword;
    if (!pPackedCredentials)
    {
        *pcbPackedCredentials = cbTotal;
        return TRUE;
    }

    if (cbTotal > *pcbPackedCredentials)
    {
        *pcbPackedCredentials = cbTotal;
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }

    *(ULONG*)pPackedCredentials = cbTotal;
    pPackedCredentials += sizeof(ULONG);

    *(ULONG*)pPackedCredentials = cbUser;
    pPackedCredentials += sizeof(ULONG);

    *(ULONG*)pPackedCredentials = cbPassword;
    pPackedCredentials += sizeof(ULONG);

    memcpy(pPackedCredentials, pszUserName, cbUser);
    pPackedCredentials += cbUser;

    memcpy(pPackedCredentials, pszPassword, cbPassword);
    *pcbPackedCredentials = cbTotal;
    return TRUE;
}

template<typename Char>
BOOL WINAPI CredUnPackAuthenticationBufferDownlevel(
    _In_ DWORD                                      dwFlags,
    _In_reads_bytes_(cbAuthBuffer) BYTE*                 pAuthBuffer,
    _In_ DWORD                                      cbAuthBuffer,
    _Out_writes_opt_(*pcchMaxUserName) Char*       pszUserName,
    _Inout_ DWORD* pcchMaxUserName,
    _Out_writes_opt_(*pcchMaxDomainName) Char*     pszDomainName,
    _Inout_opt_ DWORD* pcchMaxDomainName,
    _Out_writes_opt_(*pcchMaxPassword) Char*       pszPassword,
    _Inout_ DWORD* pcchMaxPassword
    )
{
    // 计算所需格式大小： [ULong cbHeader][ULong cbUser][ULong cbPassword][User][Password]

    pAuthBuffer += sizeof(ULONG);

    const auto _cbUser = *(ULONG*)pAuthBuffer;
    pAuthBuffer += sizeof(ULONG);

    const auto _cbPassword = *(ULONG*)pAuthBuffer;
    pAuthBuffer += sizeof(ULONG);

    const Char* _szUser = (const Char*)pAuthBuffer;
    const auto _cchUser = _cbUser / sizeof(Char);
    pAuthBuffer += _cbUser;

    if (pszUserName)
    {
        if (*pcchMaxUserName < _cchUser)
        {
            *pcchMaxUserName = _cchUser;
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        memcpy(pszUserName, _szUser, _cbUser);
        pszUserName[_cchUser] = Char('\0');
    }

    if (pszDomainName)
    {
        // DomainName\UserName
        uint32_t _cchDomain = 0;
        for (int i = 0; i != _cchUser; ++i)
        {
            if (_szUser[i] == Char('\\'))
            {
                _cchDomain = i;
                break;
            }
        }

        if (*pcchMaxDomainName < _cchDomain)
        {
            *pcchMaxDomainName = _cchDomain;
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        memcpy(pszDomainName, _szUser, _cchDomain * sizeof(Char));
        pszDomainName[_cchDomain] = Char('\0');
    }

    const Char* _szPassword = (const Char*)pAuthBuffer;
    const auto _cchPassword = _cbPassword / sizeof(Char);
    if (pszPassword)
    {
        if (*pcchMaxPassword < _cchPassword)
        {
            *pcchMaxPassword = _cchPassword;
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        memcpy(pszPassword, _szPassword, _cbPassword);
        pszPassword[_cchPassword] = Char('\0');
    }

    *pcchMaxUserName = _cchUser;
    *pcchMaxPassword = _cchPassword;
    return TRUE;
}

#endif
} // namespace
} // namespace YY::Thunks::Fallback

#endif // #ifdef YY_Thunks_Implemented

namespace YY::Thunks {

#if (YY_Thunks_Target < __WindowsNT6)

// 最低支持的客户端	Windows Vista [仅限桌面应用]
// 支持的最低服务器	Windows Server 2008[仅限桌面应用]
__DEFINE_THUNK(
credui,
20,
BOOL,
WINAPI,
CredPackAuthenticationBufferW,
    _In_ DWORD                                      dwFlags,
    _In_ LPWSTR                                     pszUserName,
    _In_ LPWSTR                                     pszPassword,
    _Out_writes_bytes_opt_(*pcbPackedCredentials) PBYTE   pPackedCredentials,
    _Inout_ DWORD* pcbPackedCredentials
    )
{
    if (const auto _pfnCredPackAuthenticationBufferW = try_get_CredPackAuthenticationBufferW())
    {
        return _pfnCredPackAuthenticationBufferW(dwFlags, pszUserName, pszPassword, pPackedCredentials, pcbPackedCredentials);
    }

    return Fallback::CredPackAuthenticationBufferDownlevel(dwFlags, pszUserName, pszPassword, pPackedCredentials, pcbPackedCredentials);
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低支持的客户端	Windows Vista [仅限桌面应用]
// 支持的最低服务器	Windows Server 2008[仅限桌面应用]
__DEFINE_THUNK(
credui,
20,
BOOL,
WINAPI,
CredPackAuthenticationBufferA,
    _In_ DWORD                                      dwFlags,
    _In_ LPSTR                                     pszUserName,
    _In_ LPSTR                                     pszPassword,
    _Out_writes_bytes_opt_(*pcbPackedCredentials) PBYTE   pPackedCredentials,
    _Inout_ DWORD* pcbPackedCredentials
    )
{
    if (const auto _pfnCredPackAuthenticationBufferA = try_get_CredPackAuthenticationBufferA())
    {
        return _pfnCredPackAuthenticationBufferA(dwFlags, pszUserName, pszPassword, pPackedCredentials, pcbPackedCredentials);
    }

    return Fallback::CredPackAuthenticationBufferDownlevel(dwFlags, pszUserName, pszPassword, pPackedCredentials, pcbPackedCredentials);
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低支持的客户端	Windows Vista [仅限桌面应用]
// 支持的最低服务器	Windows Server 2008[仅限桌面应用]
__DEFINE_THUNK(
credui,
36,
BOOL,
WINAPI,
CredUnPackAuthenticationBufferW,
    _In_ DWORD                                      dwFlags,
    _In_reads_bytes_(cbAuthBuffer) PVOID                 pAuthBuffer,
    _In_ DWORD                                      cbAuthBuffer,
    _Out_writes_opt_(*pcchMaxUserName) LPWSTR       pszUserName,
    _Inout_ DWORD* pcchMaxUserName,
    _Out_writes_opt_(*pcchMaxDomainName) LPWSTR     pszDomainName,
    _Inout_opt_ DWORD* pcchMaxDomainName,
    _Out_writes_opt_(*pcchMaxPassword) LPWSTR       pszPassword,
    _Inout_ DWORD* pcchMaxPassword
    )
{
    if (const auto _pfnCredUnPackAuthenticationBufferW = try_get_CredUnPackAuthenticationBufferW())
    {
        return _pfnCredUnPackAuthenticationBufferW(dwFlags, pAuthBuffer, cbAuthBuffer, pszUserName, pcchMaxUserName, pszDomainName, pcchMaxDomainName, pszPassword, pcchMaxPassword);
    }
    
    return Fallback::CredUnPackAuthenticationBufferDownlevel(dwFlags, reinterpret_cast<BYTE*>(pAuthBuffer), cbAuthBuffer, pszUserName, pcchMaxUserName, pszDomainName, pcchMaxDomainName, pszPassword, pcchMaxPassword);
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低支持的客户端	Windows Vista [仅限桌面应用]
// 支持的最低服务器	Windows Server 2008[仅限桌面应用]
__DEFINE_THUNK(
credui,
36,
BOOL,
WINAPI,
CredUnPackAuthenticationBufferA,
    _In_ DWORD                                      dwFlags,
    _In_reads_bytes_(cbAuthBuffer) PVOID                 pAuthBuffer,
    _In_ DWORD                                      cbAuthBuffer,
    _Out_writes_opt_(*pcchlMaxUserName) LPSTR       pszUserName,
    _Inout_ DWORD*                                  pcchMaxUserName,
    _Out_writes_opt_(*pcchMaxDomainName) LPSTR      pszDomainName,
    _Inout_opt_ DWORD*                              pcchMaxDomainName,
    _Out_writes_opt_(*pcchMaxPassword) LPSTR        pszPassword,
    _Inout_ DWORD*                                  pcchMaxPassword
    )
{
    if (const auto _pfnCredUnPackAuthenticationBufferA = try_get_CredUnPackAuthenticationBufferA())
    {
        return _pfnCredUnPackAuthenticationBufferA(dwFlags, pAuthBuffer, cbAuthBuffer, pszUserName, pcchMaxUserName, pszDomainName, pcchMaxDomainName, pszPassword, pcchMaxPassword);
    }
    
    return Fallback::CredUnPackAuthenticationBufferDownlevel(dwFlags, reinterpret_cast<BYTE*>(pAuthBuffer), cbAuthBuffer, pszUserName, pcchMaxUserName, pszDomainName, pcchMaxDomainName, pszPassword, pcchMaxPassword);
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低支持的客户端	Windows Vista [仅限桌面应用]
// 支持的最低服务器	Windows Server 2008[仅限桌面应用]
__DEFINE_THUNK(
credui,
36,
DWORD,
WINAPI,
CredUIPromptForWindowsCredentialsW,
    _In_opt_ PCREDUI_INFOW pUiInfo,
    _In_ DWORD dwAuthError,
    _Inout_ ULONG* pulAuthPackage,
    _In_reads_bytes_opt_(ulInAuthBufferSize) LPCVOID pvInAuthBuffer,
    _In_ ULONG ulInAuthBufferSize,
    _Outptr_result_bytebuffer_to_(*pulOutAuthBufferSize, *pulOutAuthBufferSize) LPVOID* ppvOutAuthBuffer,
    _Out_ ULONG* pulOutAuthBufferSize,
    _Inout_opt_ BOOL* pfSave,
    _In_ DWORD dwFlags
    )
{
    if (const auto _pfnCredUIPromptForWindowsCredentialsW = try_get_CredUIPromptForWindowsCredentialsW())
    {
        return _pfnCredUIPromptForWindowsCredentialsW(pUiInfo, dwAuthError, pulAuthPackage, pvInAuthBuffer, ulInAuthBufferSize, ppvOutAuthBuffer, pulOutAuthBufferSize, pfSave, dwFlags);
    }

    if (!ppvOutAuthBuffer || !pulOutAuthBufferSize)
        return ERROR_INVALID_PARAMETER;

    *ppvOutAuthBuffer = NULL;
    *pulOutAuthBufferSize = 0;

    WCHAR szUser[CREDUI_MAX_USERNAME_LENGTH + 1] = {};
    WCHAR szPassword[CREDUI_MAX_PASSWORD_LENGTH + 1] = {};

    DWORD xpFlags = CREDUI_FLAGS_DO_NOT_PERSIST;

    if (dwFlags & CREDUIWIN_GENERIC)
        xpFlags |= CREDUI_FLAGS_GENERIC_CREDENTIALS;

    if (dwFlags & CREDUIWIN_CHECKBOX)
        xpFlags |= CREDUI_FLAGS_SHOW_SAVE_CHECK_BOX;

    BOOL save = FALSE;

    DWORD dwResult = CredUIPromptForCredentialsW(
        pUiInfo,
        L"",
        NULL,
        dwAuthError,
        szUser,
        _countof(szUser),
        szPassword,
        _countof(szPassword),
        &save,
        xpFlags);

    if (pfSave)
        *pfSave = save;

    if (dwResult != NO_ERROR)
        return dwResult;

    if (!Fallback::CredPackAuthenticationBufferDownlevel(0, szUser, szPassword, (PBYTE)ppvOutAuthBuffer, pulOutAuthBufferSize))
    {
        return GetLastError();
    }

    if (pulAuthPackage)
        *pulAuthPackage = 0; // XP 无法模拟 AuthPackage

    memset(szPassword, 0, sizeof(szPassword));
    return NO_ERROR;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低支持的客户端	Windows Vista [仅限桌面应用]
// 支持的最低服务器	Windows Server 2008[仅限桌面应用]
__DEFINE_THUNK(
credui,
36,
DWORD,
WINAPI,
CredUIPromptForWindowsCredentialsA,
    _In_opt_ PCREDUI_INFOA pUiInfo,
    _In_ DWORD dwAuthError,
    _Inout_ ULONG* pulAuthPackage,
    _In_reads_bytes_opt_(ulInAuthBufferSize) LPCVOID pvInAuthBuffer,
    _In_ ULONG ulInAuthBufferSize,
    _Outptr_result_bytebuffer_to_(*pulOutAuthBufferSize, *pulOutAuthBufferSize) LPVOID* ppvOutAuthBuffer,
    _Out_ ULONG* pulOutAuthBufferSize,
    _Inout_opt_ BOOL* pfSave,
    _In_ DWORD dwFlags
    )
{
    if (const auto _pfnCredUIPromptForWindowsCredentialsA = try_get_CredUIPromptForWindowsCredentialsA())
    {
        return _pfnCredUIPromptForWindowsCredentialsA(pUiInfo, dwAuthError, pulAuthPackage, pvInAuthBuffer, ulInAuthBufferSize, ppvOutAuthBuffer, pulOutAuthBufferSize, pfSave, dwFlags);
    }

    if (!ppvOutAuthBuffer || !pulOutAuthBufferSize)
        return ERROR_INVALID_PARAMETER;

    *ppvOutAuthBuffer = NULL;
    *pulOutAuthBufferSize = 0;

    CHAR szUser[CREDUI_MAX_USERNAME_LENGTH + 1] = {};
    CHAR szPassword[CREDUI_MAX_PASSWORD_LENGTH + 1] = {};

    DWORD xpFlags = CREDUI_FLAGS_DO_NOT_PERSIST;

    if (dwFlags & CREDUIWIN_GENERIC)
        xpFlags |= CREDUI_FLAGS_GENERIC_CREDENTIALS;

    if (dwFlags & CREDUIWIN_CHECKBOX)
        xpFlags |= CREDUI_FLAGS_SHOW_SAVE_CHECK_BOX;

    BOOL save = FALSE;

    DWORD dwResult = CredUIPromptForCredentialsA(
        pUiInfo,
        "",
        NULL,
        dwAuthError,
        szUser,
        _countof(szUser),
        szPassword,
        _countof(szPassword),
        &save,
        xpFlags);

    if (pfSave)
        *pfSave = save;

    if (dwResult != NO_ERROR)
        return dwResult;

    if (!Fallback::CredPackAuthenticationBufferDownlevel(0, szUser, szPassword, (PBYTE)ppvOutAuthBuffer, pulOutAuthBufferSize))
    {
        return GetLastError();
    }

    if (pulAuthPackage)
        *pulAuthPackage = 0; // XP 无法模拟 AuthPackage

    memset(szPassword, 0, sizeof(szPassword));
    return NO_ERROR;
}
#endif

} // namespace YY::Thunks
