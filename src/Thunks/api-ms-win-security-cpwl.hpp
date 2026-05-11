#include <userenv.h>

#if (YY_Thunks_Target < __WindowsNT5_2) && !defined(__Comment_Lib_userenv)
#define __Comment_Lib_userenv
#pragma comment(lib, "userenv.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_2)

// 最低受支持的客户端	Windows Vista [仅限桌面应用]
// 最低受支持的服务器	Windows Server 2003[仅限桌面应用]
__DEFINE_THUNK(
advapi32,
36,
BOOL,
WINAPI,
CreateProcessWithTokenW,
    _In_        HANDLE hToken,
    _In_        DWORD dwLogonFlags,
    _In_opt_    LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_        DWORD _fCreationFlags,
    _In_opt_    LPVOID lpEnvironment,
    _In_opt_    LPCWSTR lpCurrentDirectory,
    _In_        LPSTARTUPINFOW lpStartupInfo,
    _Out_       LPPROCESS_INFORMATION lpProcessInformation
    )
{
    if (auto _pfnCreateProcessWithTokenW = try_get_CreateProcessWithTokenW())
    {
        return _pfnCreateProcessWithTokenW(hToken, dwLogonFlags, lpApplicationName, lpCommandLine, _fCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    }

    if (!hToken || !lpStartupInfo || !lpProcessInformation)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if (dwLogonFlags & ~(LOGON_WITH_PROFILE | LOGON_NETCREDENTIALS_ONLY))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    //if (dwLogonFlags & LOGON_NETCREDENTIALS_ONLY)
    //{
    //    // XP/2003 无法等效模拟 NetCredentialsOnly 语义。暂时就放行吧。
    //    SetLastError(ERROR_NOT_SUPPORTED);
    //    return FALSE;
    //}

    memset(lpProcessInformation, 0, sizeof(*lpProcessInformation));

    HANDLE hPrimaryToken = NULL;

    // XP 必须把 Impersonation Token 转成 Primary Token
    if (!DuplicateTokenEx(
        hToken,
        TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY,
        NULL,
        SecurityImpersonation,
        TokenPrimary,
        &hPrimaryToken))
    {
        return FALSE;
    }

    LSTATUS _lStatus = ERROR_SUCCESS;

    LPVOID pEnv = lpEnvironment;
    PROFILEINFOW pi = { sizeof(pi) };
    pi.dwFlags = PI_NOUI;

    do
    {
        if (dwLogonFlags & LOGON_WITH_PROFILE)
        {
            DWORD _cbTokenInfo = 0;
            GetTokenInformation(hPrimaryToken, TokenUser, nullptr, 0, &_cbTokenInfo);
            if (_cbTokenInfo == 0)
            {
                _lStatus = GetLastError();
                break;
            }

            auto _pTokenUser = reinterpret_cast<TOKEN_USER*>(_malloca(_cbTokenInfo));
            if (!_pTokenUser)
            {
                _lStatus = ERROR_OUTOFMEMORY;
                break;
            }

            if (!GetTokenInformation(hPrimaryToken, TokenUser, _pTokenUser, _cbTokenInfo, &_cbTokenInfo))
            {
                _lStatus = GetLastError();
                _freea(_pTokenUser);
                break;
            }

            DWORD _cchUser = 0;
            DWORD _cchDomain = 0;
            SID_NAME_USE _eSidNameUse = SidTypeUnknown;
            LookupAccountSidW(nullptr, _pTokenUser->User.Sid, nullptr, &_cchUser, nullptr, &_cchDomain, &_eSidNameUse);
            if (_cchUser == 0)
            {
                _lStatus = GetLastError();
                _freea(_pTokenUser);
                break;
            }

            auto _szUser = reinterpret_cast<LPWSTR>(_malloca(sizeof(WCHAR) * _cchUser));
            if (!_szUser)
            {
                _lStatus = ERROR_OUTOFMEMORY;
                _freea(_pTokenUser);
                break;
            }

            LPWSTR _szDomain = nullptr;
            if (_cchDomain)
            {
                _szDomain = reinterpret_cast<LPWSTR>(_malloca(sizeof(WCHAR) * _cchDomain));
                if (!_szDomain)
                {
                    _lStatus = ERROR_OUTOFMEMORY;
                    _freea(_szUser);
                    _freea(_pTokenUser);
                    break;
                }
            }

            if (!LookupAccountSidW(nullptr, _pTokenUser->User.Sid, _szUser, &_cchUser, _szDomain, &_cchDomain, &_eSidNameUse))
            {
                _lStatus = GetLastError();
                if (_szDomain)
                    _freea(_szDomain);
                _freea(_szUser);
                _freea(_pTokenUser);
                break;
            }

            pi.lpUserName = _szUser;
            if (!LoadUserProfileW(hPrimaryToken, &pi))
            {
                _lStatus = GetLastError();
                if (_szDomain)
                    _freea(_szDomain);
                _freea(_szUser);
                _freea(_pTokenUser);
                break;
            }

            if (_szDomain)
                _freea(_szDomain);
            _freea(_szUser);
            _freea(_pTokenUser);
        }

        if (!lpEnvironment)
        {
            if (!CreateEnvironmentBlock(&pEnv, hPrimaryToken, FALSE))
            {
                pEnv = NULL;
            }
            else
            {
                _fCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
            }
        }

        if (!CreateProcessAsUserW(
            hPrimaryToken,
            lpApplicationName,
            lpCommandLine,
            NULL,
            NULL,
            FALSE,
            _fCreationFlags,
            pEnv,
            lpCurrentDirectory,
            lpStartupInfo,
            lpProcessInformation))
        {
            _lStatus = GetLastError();
            break;
        }
    } while (false);

    if (pEnv && pEnv != lpEnvironment)
        DestroyEnvironmentBlock(pEnv);

    if (pi.hProfile)
        UnloadUserProfile(hPrimaryToken, pi.hProfile);

    CloseHandle(hPrimaryToken);

    if(_lStatus != ERROR_SUCCESS)
    {
        SetLastError(_lStatus);
        return FALSE;
    }

    return TRUE;
}
#endif
}
