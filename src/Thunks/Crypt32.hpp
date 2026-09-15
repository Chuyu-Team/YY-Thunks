#if (YY_Thunks_Target < __WindowsNT6)
#include <dpapi.h>
#include <bcrypt.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    crypt32,
    28,
    BOOL,
    WINAPI,
    CryptHashCertificate2,
        _In_ LPCWSTR _szCNGHashAlgId,
        _In_ DWORD _fFlags,
        _Reserved_ void* pvReserved,
        _In_reads_bytes_opt_(cbEncoded) const BYTE* pbEncoded,
        _In_ DWORD cbEncoded,
        _Out_writes_bytes_to_opt_(*pcbComputedHash, *pcbComputedHash) BYTE* pbComputedHash,
        _Inout_ DWORD* pcbComputedHash
        )
    {
        if (const auto _pfnCryptHashCertificate2 = try_get_CryptHashCertificate2())
        {
            return _pfnCryptHashCertificate2(_szCNGHashAlgId, _fFlags, pvReserved, pbEncoded, cbEncoded, pbComputedHash, pcbComputedHash);
        }

        if (pcbComputedHash == nullptr)
        {
            SetLastError(E_INVALIDARG);
            return FALSE;
        }

        ALG_ID _AlgId;
        do
        {
            if (StringCompareIgnoreCaseByAscii(_szCNGHashAlgId, BCRYPT_MD5_ALGORITHM, -1) == 0)
            {
                _AlgId = CALG_MD5;
                break;
            }

            if (StringCompareIgnoreCaseByAscii(_szCNGHashAlgId, BCRYPT_SHA1_ALGORITHM, -1) == 0)
            {
                _AlgId = CALG_SHA1;
                break;
            }

            if (StringCompareIgnoreCaseByAscii(_szCNGHashAlgId, BCRYPT_SHA256_ALGORITHM, -1) == 0)
            {
                _AlgId = CALG_SHA_256;
                break;
            }

            if (StringCompareIgnoreCaseByAscii(_szCNGHashAlgId, BCRYPT_SHA384_ALGORITHM, -1) == 0)
            {
                _AlgId = CALG_SHA_384;
                break;
            }

            if (StringCompareIgnoreCaseByAscii(_szCNGHashAlgId, BCRYPT_SHA512_ALGORITHM, -1) == 0)
            {
                _AlgId = CALG_SHA_512;
                break;
            }

            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        } while (false);

        // Vista 之前通过旧 CryptoAPI 做算法名映射，避免重复实现证书哈希流程。
        return ::CryptHashCertificate(NULL, _AlgId, 0, pbEncoded, cbEncoded, pbComputedHash, pcbComputedHash);
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
    CryptProtectMemory,
        _Inout_         LPVOID          pDataIn,
        _In_            DWORD           cbDataIn,
        _In_            DWORD           dwFlags
        )
    {
        if (const auto _pfnCryptProtectMemory = try_get_CryptProtectMemory())
        {
            return _pfnCryptProtectMemory(pDataIn, cbDataIn, dwFlags);
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
        if (const auto _pfnCryptUnprotectMemory = try_get_CryptUnprotectMemory())
        {
            return _pfnCryptUnprotectMemory(pDataIn, cbDataIn, dwFlags);
        }
            
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    // CRYPT_STRING_NOCRLF 需要Windows Vista或者更高
    __DEFINE_THUNK(
    crypt32,
    20,
    BOOL,
    WINAPI,
    CryptBinaryToStringW,
        _In_reads_bytes_(_cbBinary) CONST BYTE* _pBinary,
        _In_ DWORD _cbBinary,
        _In_ DWORD _fFlags,
        _Out_writes_to_opt_(*_pcString, *_pcString) LPWSTR _szString,
        _Inout_ DWORD* _pcString
        )
    {
        const auto _pfnCryptBinaryToStringW = try_get_CryptBinaryToStringW();
        if (!_pfnCryptBinaryToStringW)
        {
            // Windows 2000？
            SetLastError(ERROR_FUNCTION_FAILED);
            return FALSE;
        }

        // CRYPT_STRING_NOCRLF 需要 Windows Vista
        if ((_fFlags & CRYPT_STRING_NOCRLF) && internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            _fFlags &= ~CRYPT_STRING_NOCRLF;
            if (!_szString)
            {
                // 少了 CRYPT_STRING_NOCRLF 所需的缓冲区只可能比原版多，所以这样做是安全的。
                return _pfnCryptBinaryToStringW(_pBinary, _cbBinary, _fFlags, _szString, _pcString);
            }

            if (!_pfnCryptBinaryToStringW(_pBinary, _cbBinary, _fFlags, _szString, _pcString))
                return FALSE;

            // 删除所有的 "\r\n"，模拟 CRYPT_STRING_NOCRLF 行为
            const auto _szStringEnd = _szString + *_pcString;
            auto _szStringNew = _szString;
            bool _bHasSkip = false;
            for (auto _szCurrent = _szString; _szCurrent != _szStringEnd; ++_szCurrent)
            {
                const auto _ch = *_szCurrent;
                if (_ch == L'\r' || _ch == L'\n')
                {
                    _bHasSkip = true;
                }
                else
                {
                    if (_bHasSkip)
                    {
                        *_szStringNew = *_szCurrent;
                    }

                    ++_szStringNew;
                }
            }

            *_szStringNew = L'\0';
            *_pcString = _szStringNew - _szString;
            return TRUE;
        }

        return _pfnCryptBinaryToStringW(_pBinary, _cbBinary, _fFlags, _szString, _pcString);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    // CRYPT_STRING_NOCRLF 需要Windows Vista或者更高
    __DEFINE_THUNK(
    crypt32,
    20,
    BOOL,
    WINAPI,
    CryptBinaryToStringA,
        _In_reads_bytes_(_cbBinary) CONST BYTE* _pBinary,
        _In_ DWORD _cbBinary,
        _In_ DWORD _fFlags,
        _Out_writes_to_opt_(*_pcString, *_pcString) LPSTR _szString,
        _Inout_ DWORD* _pcString
        )
    {
        const auto _pfnCryptBinaryToStringA = try_get_CryptBinaryToStringA();
        if (!_pfnCryptBinaryToStringA)
        {
            // Windows 2000？
            SetLastError(ERROR_FUNCTION_FAILED);
            return FALSE;
        }

        // CRYPT_STRING_NOCRLF 需要 Windows Vista
        if ((_fFlags & CRYPT_STRING_NOCRLF) && internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            _fFlags &= ~CRYPT_STRING_NOCRLF;
            if (!_szString)
            {
                // 少了 CRYPT_STRING_NOCRLF 所需的缓冲区只可能比原版多，所以这样做是安全的。
                return _pfnCryptBinaryToStringA(_pBinary, _cbBinary, _fFlags, _szString, _pcString);
            }

            if (!_pfnCryptBinaryToStringA(_pBinary, _cbBinary, _fFlags, _szString, _pcString))
                return FALSE;

            // 删除所有的 "\r\n"，模拟 CRYPT_STRING_NOCRLF 行为
            const auto _szStringEnd = _szString + *_pcString;
            auto _szStringNew = _szString;
            bool _bHasSkip = false;
            for (auto _szCurrent = _szString; _szCurrent != _szStringEnd; ++_szCurrent)
            {
                const auto _ch = *_szCurrent;
                if (_ch == '\r' || _ch == '\n')
                {
                    _bHasSkip = true;
                }
                else
                {
                    if (_bHasSkip)
                    {
                        *_szStringNew = *_szCurrent;
                    }

                    ++_szStringNew;
                }
            }

            *_szStringNew = L'\0';
            *_pcString = _szStringNew - _szString;
            return TRUE;
        }

        return _pfnCryptBinaryToStringA(_pBinary, _cbBinary, _fFlags, _szString, _pcString);
    }
#endif
} // namespace YY::Thunks
