
namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows XP Professional x64 Edition, Windows Server 2003 with SP1
    __DEFINE_THUNK(
    advapi32,
    16,
    LSTATUS,
    APIENTRY,
    RegDeleteKeyExW,
        _In_ HKEY hKey,
        _In_ LPCWSTR lpSubKey,
        _In_ REGSAM samDesired,
        _Reserved_ DWORD Reserved
        )
    {
        if(auto const pRegDeleteKeyExW = try_get_RegDeleteKeyExW())
        {
            return pRegDeleteKeyExW(hKey, lpSubKey, samDesired, Reserved);
        }


        return RegDeleteKeyW(hKey, lpSubKey);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows XP Professional x64 Edition, Windows Server 2003 with SP1
    __DEFINE_THUNK(
    advapi32,
    16,
    LSTATUS,
    APIENTRY,
    RegDeleteKeyExA,
        _In_ HKEY hKey,
        _In_ LPCSTR lpSubKey,
        _In_ REGSAM samDesired,
        _Reserved_ DWORD Reserved
        )
    {
        if (auto const pRegDeleteKeyExA = try_get_RegDeleteKeyExA())
        {
            return pRegDeleteKeyExA(hKey, lpSubKey, samDesired, Reserved);
        }

        return RegDeleteKeyA(hKey, lpSubKey);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista, Windows Server 2008
    __DEFINE_THUNK(
    advapi32,
    24,
    LSTATUS,
    APIENTRY,
    RegSetKeyValueW,
        _In_ HKEY hKey,
        _In_opt_ LPCWSTR lpSubKey,
        _In_opt_ LPCWSTR lpValueName,
        _In_ DWORD dwType,
        _In_reads_bytes_opt_(cbData) LPCVOID lpData,
        _In_ DWORD cbData
        )
    {
        //Empty?
        if (lpSubKey == nullptr || *lpSubKey == L'\0')
        {
            return RegSetValueExW(hKey, lpValueName, 0, dwType, (const BYTE*)lpData, cbData);
        }
        else
        {
            HKEY hSubKey;
            auto lStatus = RegCreateKeyExW(hKey, lpSubKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hSubKey, nullptr);

            if (lStatus == ERROR_SUCCESS)
            {
                lStatus = RegSetValueExW(hSubKey, lpValueName, 0, dwType, (const BYTE*)lpData, cbData);

                RegCloseKey(hSubKey);
            }

            return lStatus;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista, Windows Server 2008
    __DEFINE_THUNK(
    advapi32,
    24,
    LSTATUS,
    APIENTRY,
    RegSetKeyValueA,
        _In_ HKEY hKey,
        _In_opt_ LPCSTR lpSubKey,
        _In_opt_ LPCSTR lpValueName,
        _In_ DWORD dwType,
        _In_reads_bytes_opt_(cbData) LPCVOID lpData,
        _In_ DWORD cbData
        )
    {
        //Empty?
        if (lpSubKey == nullptr || *lpSubKey == '\0')
        {
            return RegSetValueExA(hKey, lpValueName, 0, dwType, (const BYTE*)lpData, cbData);
        }
        else
        {
            HKEY hSubKey;
            auto lStatus = RegCreateKeyExA(hKey, lpSubKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hSubKey, nullptr);

            if (lStatus == ERROR_SUCCESS)
            {
                lStatus = RegSetValueExA(hSubKey, lpValueName, 0, dwType, (const BYTE*)lpData, cbData);

                RegCloseKey(hSubKey);
            }

            return lStatus;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista, Windows Server 2008
    __DEFINE_THUNK(
    advapi32,
    12,
    LSTATUS,
    APIENTRY,
    RegDeleteKeyValueW,
        _In_ HKEY hKey,
        _In_opt_ LPCWSTR lpSubKey,
        _In_opt_ LPCWSTR lpValueName
        )
    {
        HKEY hSubKey;
        auto lStatus = RegOpenKeyExW(hKey, lpSubKey, 0, KEY_SET_VALUE, &hSubKey);

        if (lStatus == ERROR_SUCCESS)
        {
            lStatus = RegDeleteValueW(hSubKey, lpValueName);
            RegCloseKey(hSubKey);
        }

        return lStatus;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista, Windows Server 2008
    __DEFINE_THUNK(
    advapi32,
    12,
    LSTATUS,
    APIENTRY,
    RegDeleteKeyValueA,
        _In_ HKEY hKey,
        _In_opt_ LPCSTR lpSubKey,
        _In_opt_ LPCSTR lpValueName
        )
    {
        HKEY hSubKey;
        auto lStatus = RegOpenKeyExA(hKey, lpSubKey, 0, KEY_SET_VALUE, &hSubKey);

        if (lStatus == ERROR_SUCCESS)
        {
            lStatus = RegDeleteValueA(hSubKey, lpValueName);
            RegCloseKey(hSubKey);
        }

        return lStatus;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista, Windows Server 2008
    __DEFINE_THUNK(
    advapi32,
    8,
    LSTATUS,
    APIENTRY,
    RegDeleteTreeW,
        _In_ HKEY hKey,
        _In_opt_ LPCWSTR lpSubKey
        )
    {
        return SHDeleteKeyW(hKey, lpSubKey);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista, Windows Server 2008
    __DEFINE_THUNK(
    advapi32,
    8,
    LSTATUS,
    APIENTRY,
    RegDeleteTreeA,
        _In_ HKEY hKey,
        _In_opt_ LPCSTR lpSubKey
        )
    {
        return SHDeleteKeyA(hKey, lpSubKey);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows Vista, Windows XP Professional x64 Edition, Windows Server 2008, Windows Server 2003 with SP1
    __DEFINE_THUNK(
    advapi32,
    28,
    LSTATUS,
    APIENTRY,
    RegGetValueW,
        HKEY    hkey,
        LPCWSTR lpSubKey,
        LPCWSTR lpValue,
        DWORD   dwFlags,
        LPDWORD pdwType,
        PVOID   pvData,
        LPDWORD pcbData
        )
    {
#if (YY_Thunks_Target >= __WindowsNT5_1_SP2)
        return SHRegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
#else
        if (auto const pRegGetValueW = try_get_RegGetValueW())
        {
            return pRegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
        }

        if (pvData && pcbData == nullptr
            || (dwFlags & RRF_RT_ANY) == 0)
        {
            return ERROR_INVALID_PARAMETER;
        }

        LSTATUS lStatus;

        if (lpSubKey&&*lpSubKey)
        {
            lStatus = RegOpenKeyExW(hkey, lpSubKey, 0, KEY_QUERY_VALUE, &hkey);
            if (lStatus != ERROR_SUCCESS)
                return lStatus;
        }

        DWORD dwType;
        DWORD cbData = pvData && pcbData ? *pcbData : 0;
        const auto cbDataBackup = cbData;


        lStatus = RegQueryValueExW(hkey, lpValue, nullptr, &dwType, (LPBYTE)pvData, &cbData);

        DWORD cbDataFix;
        if (lStatus == ERROR_SUCCESS
            || lStatus == ERROR_MORE_DATA)
        {
            if (dwType == REG_MULTI_SZ)
            {
                //连续2个 null 结尾
                cbDataFix = cbData + sizeof(*lpValue) * 2;
            }
            else if (dwType == REG_SZ)
            {
                //一个null 结尾
                cbDataFix = cbData + sizeof(*lpValue);
            }
            else if (dwType == REG_EXPAND_SZ)
            {
                if (dwFlags & RRF_NOEXPAND)
                {
                    //一个null 结尾
                    cbDataFix = cbData + sizeof(*lpValue);
                }
                else
                {
                    //需要展开
                    cbDataFix = pvData && pcbData ? *pcbData : 0;
                    lStatus = SHQueryValueExW(hkey, lpValue, nullptr, &dwType, pvData, &cbDataFix);

                    if (lStatus == ERROR_SUCCESS
                        || lStatus == ERROR_MORE_DATA)
                    {
                        cbData = cbDataFix;
                    }
                }
            }
            else
            {
                cbDataFix = cbData;
            }
        }

        if (lpSubKey&&*lpSubKey)
        {
            RegCloseKey(hkey);
        }


        if (lStatus == ERROR_SUCCESS
            || lStatus == ERROR_MORE_DATA)
        {
            if (pdwType)
                *pdwType = dwType;

            //检测类型是否支持
            switch (dwType)
            {
            case REG_NONE:
            case REG_SZ:
            case REG_EXPAND_SZ:
            case REG_BINARY:
            case REG_DWORD:
                if ((dwFlags & (1 << dwType)) == 0)
                {
                    lStatus = ERROR_UNSUPPORTED_TYPE;
                    break;
                }
                else if (pvData && lStatus == ERROR_SUCCESS)
                {
                    //当函数成功时，保证字符串正常 NULL 结尾
                    if (REG_SZ == dwType || REG_EXPAND_SZ == dwType)
                    {
                        //保证1个 NULL 结尾


                        //sizeof(wchar_t) 边界对齐
                        if (cbData % sizeof(*lpValue))
                        {
                            if (cbData >= cbDataBackup)
                            {
                                lStatus = ERROR_MORE_DATA;
                                break;
                            }

                            ((byte*)pvData)[cbData] = 0;

                            ++cbData;
                        }


                        auto cStringMin = cbData / sizeof(*lpValue);


                        auto pString = (wchar_t*)pvData;

                        if (cStringMin == 0 || pString[cStringMin - 1])
                        {
                            if (cbData + sizeof(*lpValue) > cbDataBackup)
                            {
                                lStatus = ERROR_MORE_DATA;
                                break;
                            }

                            pString[cStringMin++] = L'\0';
                        }

                        cbDataFix = cStringMin * sizeof(*lpValue);
                    }
                }
                break;
            case REG_MULTI_SZ:
                if ((dwFlags & RRF_RT_REG_MULTI_SZ) == 0)
                {
                    lStatus = ERROR_UNSUPPORTED_TYPE;
                }
                else if (pvData && lStatus == ERROR_SUCCESS)
                {
                    //保证最后一个字符串 NULL 结尾 并且 包含 NULL 结束符

                    //sizeof(wchar_t) 边界对齐
                    if (cbData % sizeof(*lpValue))
                    {
                        if (cbData >= cbDataBackup)
                        {
                            lStatus = ERROR_MORE_DATA;
                            break;
                        }

                        ((byte*)pvData)[cbData] = 0;

                        ++cbData;
                    }

                    auto cStringMin = cbData / sizeof(*lpValue);

                    auto pString = (wchar_t*)pvData;

                    if (cStringMin == 0)
                    {
                        if (cbData + sizeof(*lpValue) > cbDataBackup)
                        {
                            lStatus = ERROR_MORE_DATA;
                            break;
                        }

                        pString[cStringMin++] = L'\0';
                    }
                    else if (pString[cStringMin - 1])
                    {
                        if (cbData + sizeof(*lpValue) * 2 > cbDataBackup)
                        {
                            lStatus = ERROR_MORE_DATA;
                            break;
                        }

                        pString[cStringMin++] = L'\0';

                        pString[cStringMin++] = L'\0';
                    }
                    else if (cStringMin >= 2 && pString[cStringMin - 2])
                    {
                        if (cbData + sizeof(*lpValue) > cbDataBackup)
                        {
                            lStatus = ERROR_MORE_DATA;
                            break;
                        }

                        pString[cStringMin++] = L'\0';
                    }

                    cbDataFix = cStringMin * sizeof(*lpValue);
                }
                break;
            case REG_QWORD:
                if ((dwFlags & RRF_RT_REG_QWORD) == 0)
                {
                    lStatus = ERROR_UNSUPPORTED_TYPE;
                }
                break;
            default:
                if((dwFlags & RRF_RT_ANY) != RRF_RT_ANY)
                    lStatus = ERROR_UNSUPPORTED_TYPE;
                break;
            }

            if (pcbData)
                *pcbData = cbDataFix;

        }


        if ((RRF_ZEROONFAILURE & dwFlags) != 0 && lStatus)
        {
            //全0填充
            if (pvData && cbDataBackup)
            {
                memset(pvData, 0, cbDataBackup);
            }
        }

        return lStatus;
#endif
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows Vista, Windows XP Professional x64 Edition, Windows Server 2008, Windows Server 2003 with SP1
    __DEFINE_THUNK(
    advapi32,
    28,
    LSTATUS,
    APIENTRY,
    RegGetValueA,
        HKEY    hkey,
        LPCSTR  lpSubKey,
        LPCSTR  lpValue,
        DWORD   dwFlags,
        LPDWORD pdwType,
        PVOID   pvData,
        LPDWORD pcbData
        )
    {
#if (YY_Thunks_Target >= __WindowsNT5_1_SP2)
        return SHRegGetValueA(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
#else
        if (auto const pRegGetValueA = try_get_RegGetValueA())
        {
            return pRegGetValueA(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
        }

        if (pvData && pcbData == nullptr
            || (dwFlags & RRF_RT_ANY) == 0)
        {
            return ERROR_INVALID_PARAMETER;
        }

        LSTATUS lStatus;

        if (lpSubKey&&*lpSubKey)
        {
            lStatus = RegOpenKeyExA(hkey, lpSubKey, 0, KEY_QUERY_VALUE, &hkey);
            if (lStatus != ERROR_SUCCESS)
                return lStatus;
        }

        DWORD dwType;
        DWORD cbData = pvData && pcbData ? *pcbData : 0;
        const auto cbDataBackup = cbData;


        lStatus = RegQueryValueExA(hkey, lpValue, nullptr, &dwType, (LPBYTE)pvData, &cbData);

        DWORD cbDataFix;
        if (lStatus == ERROR_SUCCESS
            || lStatus == ERROR_MORE_DATA)
        {
            if (dwType == REG_MULTI_SZ)
            {
                //连续2个 null 结尾
                cbDataFix = cbData + sizeof(*lpValue) * 2;
            }
            else if (dwType == REG_SZ)
            {
                //一个null 结尾
                cbDataFix = cbData + sizeof(*lpValue);
            }
            else if (dwType == REG_EXPAND_SZ)
            {
                if (dwFlags & RRF_NOEXPAND)
                {
                    //一个null 结尾
                    cbDataFix = cbData + sizeof(*lpValue);
                }
                else
                {
                    //需要展开
                    cbDataFix = pvData && pcbData ? *pcbData : 0;
                    lStatus = SHQueryValueExA(hkey, lpValue, nullptr, &dwType, pvData, &cbDataFix);

                    if (lStatus == ERROR_SUCCESS
                        || lStatus == ERROR_MORE_DATA)
                    {
                        cbData = cbDataFix;
                    }
                }
            }
            else
            {
                cbDataFix = cbData;
            }
        }

        if (lpSubKey&&*lpSubKey)
        {
            RegCloseKey(hkey);
        }


        if (lStatus == ERROR_SUCCESS
            || lStatus == ERROR_MORE_DATA)
        {
            if (pdwType)
                *pdwType = dwType;

            //检测类型是否支持
            switch (dwType)
            {
            case REG_NONE:
            case REG_SZ:
            case REG_EXPAND_SZ:
            case REG_BINARY:
            case REG_DWORD:
                if ((dwFlags & (1<< dwType)) == 0)
                {
                    lStatus = ERROR_UNSUPPORTED_TYPE;
                }
                else if (pvData && lStatus == ERROR_SUCCESS)
                {
                    //当函数成功时，保证字符串正常 NULL 结尾
                    if (REG_SZ == dwType || REG_EXPAND_SZ == dwType)
                    {
                        //保证1个 NULL 结尾
                        auto cStringMin = cbData;


                        auto pString = (char*)pvData;

                        if (cStringMin == 0 || pString[cStringMin - 1])
                        {
                            if (cbData + sizeof(*lpValue) > cbDataBackup)
                            {
                                lStatus = ERROR_MORE_DATA;
                                break;
                            }

                            pString[cStringMin++] = '\0';
                        }

                        cbDataFix = cStringMin * sizeof(*lpValue);
                    }				
                }
                break;
            case REG_MULTI_SZ:
                if ((dwFlags & RRF_RT_REG_MULTI_SZ) == 0)
                {
                    lStatus = ERROR_UNSUPPORTED_TYPE;
                }
                else
                {
                    //保证最后一个字符串 NULL 结尾 并且 包含 NULL 结束符

                    auto cStringMin = cbData;

                    auto pString = (char*)pvData;

                    if (cStringMin == 0)
                    {
                        if (cbData + sizeof(*lpValue) > cbDataBackup)
                        {
                            lStatus = ERROR_MORE_DATA;
                            break;
                        }

                        pString[cStringMin++] = '\0';
                    }
                    else if (pString[cStringMin - 1])
                    {
                        if (cbData + sizeof(*lpValue) * 2 > cbDataBackup)
                        {
                            lStatus = ERROR_MORE_DATA;
                            break;
                        }

                        pString[cStringMin++] = '\0';

                        pString[cStringMin++] = '\0';
                    }
                    else if (cStringMin >= 2 && pString[cStringMin - 2])
                    {
                        if (cbData + sizeof(*lpValue) > cbDataBackup)
                        {
                            lStatus = ERROR_MORE_DATA;
                            break;
                        }

                        pString[cStringMin++] = '\0';
                    }

                    cbDataFix = cStringMin * sizeof(*lpValue);
                }
                break;
            case REG_QWORD:
                if ((dwFlags & RRF_RT_REG_QWORD) == 0)
                {
                    lStatus = ERROR_UNSUPPORTED_TYPE;
                }
                break;
            default:
                if((dwFlags & RRF_RT_ANY) != RRF_RT_ANY)
                    lStatus = ERROR_UNSUPPORTED_TYPE;
                break;
            }

            if (pcbData)
                *pcbData = cbDataFix;

        }


        if ((RRF_ZEROONFAILURE & dwFlags) != 0 && lStatus)
        {
            //全0填充
            if (pvData && cbDataBackup)
            {
                memset(pvData, 0, cbDataBackup);
            }
        }

        return lStatus;
#endif
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    advapi32,
    12,
    LSTATUS,
    APIENTRY,
    RegCopyTreeW,
        _In_ HKEY hKeySrc,
        _In_opt_ LPCWSTR lpSubKey,
        _In_ HKEY hKeyDest
        )
    {
        if (auto const pRegCopyTreeW = try_get_RegCopyTreeW())
        {
            return pRegCopyTreeW(hKeySrc, lpSubKey, hKeyDest);
        }

        return SHCopyKeyW(hKeySrc, lpSubKey, hKeyDest, 0);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    advapi32,
    12,
    LSTATUS,
    APIENTRY,
    RegCopyTreeA,
        _In_        HKEY     hKeySrc,
        _In_opt_    LPCSTR  lpSubKey,
        _In_        HKEY     hKeyDest
        )
    {
        if (auto const pRegCopyTreeA = try_get_RegCopyTreeA())
        {
            return pRegCopyTreeA(hKeySrc, lpSubKey, hKeyDest);
        }

        return SHCopyKeyA(hKeySrc, lpSubKey, hKeyDest, 0);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端	Windows 2000 Professional [仅限桌面应用]
    // 最低受支持的服务器	Windows 2000 Server[仅限桌面应用]
    // 虽然2000就支持，但是为了实现CompareObjectHandles，目前需要处理一下关闭。
    __DEFINE_THUNK(
    advapi32,
    4,
    LSTATUS,
    APIENTRY,
    RegCloseKey,
        _In_ HKEY _hKey
        )
    {
        auto const _pfnRegCloseKey = try_get_RegCloseKey();
        const auto _pfnCompareObjectHandles = try_get_CompareObjectHandles();

        auto _pSharedData = GetYY_ThunksSharedData();
        // 伪句柄不用加锁
        if (_pfnCompareObjectHandles == nullptr && _hKey && (uintptr_t(_hKey) & uintptr_t(0x80000000)) == 0)
        {
            ::AcquireSRWLockShared(&_pSharedData->CompareObjectHandlesLock);
        }

        // 空指针故意崩溃
        auto _lStatus = _pfnRegCloseKey(_hKey);

        if (_pfnCompareObjectHandles == nullptr && _hKey && (uintptr_t(_hKey) & uintptr_t(0x80000000)) == 0)
        {
            ::ReleaseSRWLockShared(&_pSharedData->CompareObjectHandlesLock);
        }

        return _lStatus;
    }
#endif
} //namespace YY::Thunks
