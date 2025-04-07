#if (YY_Thunks_Target < __WindowsNT6_3)
#include <appmodel.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8  [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    8,
    LONG,
    WINAPI,
    GetCurrentPackageFullName,
        _Inout_ UINT32* _pcPackageFullNameLength,
        _Out_writes_opt_(*_pcPackageFullNameLength) PWSTR _szPackageFullName
        )
    {
        if (const auto _pfnGetCurrentPackageFullName = try_get_GetCurrentPackageFullName())
        {
            return _pfnGetCurrentPackageFullName(_pcPackageFullNameLength, _szPackageFullName);
        }

        return APPMODEL_ERROR_NO_PACKAGE;
    }
#endif // (YY_Thunks_Target < __WindowsNT6_2)


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8  [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    LONG,
    WINAPI,
    GetPackageFullName,
        _In_ HANDLE _hProcess,
        _Inout_ UINT32* _pcchPackageFullNameLength,
        _Out_writes_opt_(*_pcchPackageFullNameLength) PWSTR _szPackageFullName
        )
    {
        if (const auto _pfnGetPackageFullName = try_get_GetPackageFullName())
        {
            return _pfnGetPackageFullName(_hProcess, _pcchPackageFullNameLength, _szPackageFullName);
        }

        if (_hProcess && _hProcess != NtGetCurrentProcess())
        {
            if (GetProcessId(_hProcess) == 0)
            {
                return GetLastError();
            }
        }

        return APPMODEL_ERROR_NO_PACKAGE;
    }
#endif // (YY_Thunks_Target < __WindowsNT6_2)


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8  [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    LONG,
    WINAPI,
    GetPackageFamilyName,
        _In_ HANDLE _hProcess,
        _Inout_ UINT32* _pcchPackageFamilyNameLength,
        _Out_writes_opt_(*_pcchPackageFamilyNameLength) PWSTR _szPackageFamilyName
        )
    {
        if (const auto _pfnGetPackageFamilyName = try_get_GetPackageFamilyName())
        {
            return _pfnGetPackageFamilyName(_hProcess, _pcchPackageFamilyNameLength, _szPackageFamilyName);
        }

        if (_hProcess && _hProcess != NtGetCurrentProcess())
        {
            if (GetProcessId(_hProcess) == 0)
            {
                return GetLastError();
            }
        }

        return APPMODEL_ERROR_NO_PACKAGE;
    }
#endif // (YY_Thunks_Target < __WindowsNT6_2)


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8  [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    LONG,
    WINAPI,
    OpenPackageInfoByFullName,
        _In_ PCWSTR _szPackageFullName,
        _Reserved_ const UINT32 _uReserved,
        _Out_ PACKAGE_INFO_REFERENCE* _pPackageInfoReference
        )
    {
        if (const auto _pfnOpenPackageInfoByFullName = try_get_OpenPackageInfoByFullName())
        {
            return _pfnOpenPackageInfoByFullName(_szPackageFullName, _uReserved, _pPackageInfoReference);
        }

        if (_szPackageFullName == nullptr || _pPackageInfoReference == nullptr)
        {
            return ERROR_INVALID_PARAMETER;
        }

        return ERROR_NOT_FOUND;
    }
#endif
    

#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8  [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    4,
    LONG,
    WINAPI,
    ClosePackageInfo,
        _In_ PACKAGE_INFO_REFERENCE _PackageInfoReference
        )
    {
        if (const auto _pfnClosePackageInfo = try_get_ClosePackageInfo())
        {
            return _pfnClosePackageInfo(_PackageInfoReference);
        }

        return ERROR_INVALID_PARAMETER;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8  [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    20,
    LONG,
    WINAPI,
    GetPackageInfo,
        _In_ PACKAGE_INFO_REFERENCE _PackageInfoReference,
        _In_ const UINT32 _fFlags,
        _Inout_ UINT32* _pcbBufferLength,
        _Out_writes_bytes_opt_(*_pcbBufferLength) BYTE* _pBuffer,
        _Out_opt_ UINT32* _pcCount
        )
    {
        if (const auto _pfnGetPackageInfo = try_get_GetPackageInfo())
        {
            return _pfnGetPackageInfo(_PackageInfoReference, _fFlags, _pcbBufferLength, _pBuffer, _pcCount);
        }

        return ERROR_INVALID_PARAMETER;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_3)

    // 最低受支持的客户端	Windows 8.1  [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 R2 [仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    LONG,
    WINAPI,
    GetPackagePathByFullName,
        _In_ PCWSTR _szPackageFullName,
        _Inout_ UINT32* _pcchPathLength,
        _Out_writes_opt_(*_pcchPathLength) PWSTR _szPath
        )
    {
        if (const auto _pfnGetPackagePathByFullName = try_get_GetPackagePathByFullName())
        {
            return _pfnGetPackagePathByFullName(_szPackageFullName, _pcchPathLength, _szPath);
        }

        if (!_szPackageFullName || !_pcchPathLength)
        {
            return ERROR_INVALID_PARAMETER;
        }

        __if_exists(try_get_OpenPackageInfoByFullName)
        {
            const auto OpenPackageInfoByFullName = try_get_OpenPackageInfoByFullName();
            if (!OpenPackageInfoByFullName)
            {
                return ERROR_NOT_FOUND;
            }
        }

        __if_exists(try_get_ClosePackageInfo)
        {
            const auto ClosePackageInfo = try_get_ClosePackageInfo();
            if (!ClosePackageInfo)
            {
                return ERROR_NOT_FOUND;
            }
        }

        __if_exists(try_get_GetPackageInfo)
        {
            const auto GetPackageInfo = try_get_GetPackageInfo();
            if (!GetPackageInfo)
            {
                return ERROR_NOT_FOUND;
            }
        }

        PACKAGE_INFO_REFERENCE _PackageInfoReference;
        auto _lStatus = OpenPackageInfoByFullName(_szPackageFullName, 0, &_PackageInfoReference);
        if (_lStatus != ERROR_SUCCESS)
            return _lStatus;
        
        PACKAGE_INFO* _pPackahgeInfo = nullptr;

        do
        {
            UINT32 _cbBufferLength = 0;
            _lStatus = GetPackageInfo(_PackageInfoReference, PACKAGE_FILTER_HEAD, &_cbBufferLength, nullptr, nullptr);
            if (_lStatus != ERROR_INSUFFICIENT_BUFFER)
            {
                if (_lStatus == ERROR_SUCCESS)
                {
                    // 边界防御
                    _lStatus = ERROR_FUNCTION_FAILED;
                }
                break;
            }

            _pPackahgeInfo = (PACKAGE_INFO*)_malloca(_cbBufferLength);
            if (!_pPackahgeInfo)
            {
                _lStatus = ERROR_OUTOFMEMORY;
                break;
            }

            UINT32 _cCount = 0;
            _lStatus = GetPackageInfo(_PackageInfoReference, PACKAGE_FILTER_HEAD, &_cbBufferLength, (BYTE*)_pPackahgeInfo, &_cCount);
            if (_lStatus != ERROR_SUCCESS)
                break;

            _lStatus = ERROR_NOT_FOUND;
            for (UINT32 i = 0; i != _cCount; ++i)
            {
                if (StringCompareIgnoreCaseByAscii(_pPackahgeInfo[i].packageFullName, _szPackageFullName, -1) == 0)
                {
                    _lStatus = ERROR_INSUFFICIENT_BUFFER;
                    // *_pcchPathLength的字符计数包含null
                    const auto _cchPath = wcslen(_pPackahgeInfo->path) + 1;
                    if (_szPath)
                    {
                        const auto _cchPathLength = *_pcchPathLength;
                        if (_cchPathLength >= _cchPath)
                        {
                            memcpy(_szPath, _pPackahgeInfo->path, _cchPath * sizeof(_szPath[0]));
                            _lStatus = ERROR_SUCCESS;
                        }
                    }

                    *_pcchPathLength = _cchPath;
                    break;
                }
            }
        } while (false);

        if (_pPackahgeInfo)
            _freea(_pPackahgeInfo);

        ClosePackageInfo(_PackageInfoReference);
        return _lStatus;
    }
#endif // (YY_Thunks_Target < __WindowsNT6_3)


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8  [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 [仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    20,
    LONG,
    WINAPI,
    GetPackagesByPackageFamily,
        _In_ PCWSTR _szPackageFamilyName,
        _Inout_ UINT32* _pcCount,
        _Out_writes_opt_(*_pcCount) PWSTR* _pszPackageFullNames,
        _Inout_ UINT32* _pcchBufferLength,
        _Out_writes_opt_(*_pcchBufferLength) WCHAR* _szBuffer
        )
    {
        if (const auto _pfnGetPackagesByPackageFamily = try_get_GetPackagesByPackageFamily())
        {
            return _pfnGetPackagesByPackageFamily(_szPackageFamilyName, _pcCount, _pszPackageFullNames, _pcchBufferLength, _szBuffer);
        }

        if (!_szPackageFamilyName || !_pcCount || !_pcchBufferLength)
        {
            return ERROR_INVALID_PARAMETER;
        }

        *_pcCount = 0;
        *_pcchBufferLength = 0;
        return ERROR_SUCCESS;
    }
#endif // (YY_Thunks_Target < __WindowsNT6_2)


#if (YY_Thunks_Target < __WindowsNT6_3)

    // 最低受支持的客户端	Windows 8.1  [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012 R2 [仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    28,
    LONG,
    WINAPI,
    FindPackagesByPackageFamily,
        _In_ PCWSTR _szPackageFamilyName,
        _In_ UINT32 _uPackageFilters,
        _Inout_ UINT32* _pcCount,
        _Out_writes_opt_(*_pcCount) PWSTR* _pszPackageFullNames,
        _Inout_ UINT32* _pcchBufferLength,
        _Out_writes_opt_(*_pcchbufferLength) WCHAR* _szBuffer,
        _Out_writes_opt_(*_pcCount) UINT32* _pcPackageProperties
        )
    {
        if (const auto _pfnFindPackagesByPackageFamily = try_get_FindPackagesByPackageFamily())
        {
            return _pfnFindPackagesByPackageFamily(_szPackageFamilyName, _uPackageFilters, _pcCount, _pszPackageFullNames, _pcchBufferLength, _szBuffer, _pcPackageProperties);
        }

        if (_szPackageFamilyName == nullptr || _uPackageFilters == 0/*PACKAGE_FILTER_ALL_LOADED*/ || _pcCount == nullptr || _pcchBufferLength == nullptr)
        {
            return ERROR_INVALID_PARAMETER;
        }

        LSTATUS _lStatusRet = ERROR_SUCCESS;
        if (_pszPackageFullNames)
        {
            if (_szBuffer == nullptr)
            {
                return ERROR_INVALID_PARAMETER;
            }
        }
        else /*if(_pszPackageFullNames == nullptr)*/
        {
            if (_szBuffer)
            {
                return ERROR_INVALID_PARAMETER;
            }
        }

        UINT32 _cOutCount = 0;
        UINT32 _cchOutBufferLength = 0;

        UINT32 _cRootPackageCount = 0;
        UINT32 _cchRootBufferLength = 0;
        PWSTR* _pszRootPackageFullNames = nullptr;
        WCHAR* _szRootPackagesBuffer = nullptr;


        PACKAGE_INFO* _pPackahgeInfo = nullptr;
        UINT32 _cbPackahgeInfoBuffer = 0;

        do
        {
            __if_exists(try_get_GetPackagesByPackageFamily)
            {
                const auto GetPackagesByPackageFamily = try_get_GetPackagesByPackageFamily();
                if (!GetPackagesByPackageFamily)
                {
                    break;
                }
            }

            __if_exists(try_get_OpenPackageInfoByFullName)
            {
                const auto OpenPackageInfoByFullName = try_get_OpenPackageInfoByFullName();
                if (!OpenPackageInfoByFullName)
                {
                    break;
                }
            }

            __if_exists(try_get_ClosePackageInfo)
            {
                const auto ClosePackageInfo = try_get_ClosePackageInfo();
                if (!ClosePackageInfo)
                {
                    break;
                }
            }

            __if_exists(try_get_GetPackageInfo)
            {
                const auto GetPackageInfo = try_get_GetPackageInfo();
                if (!GetPackageInfo)
                {
                    break;
                }
            }

            constexpr UINT32 kAllPackageProperties = PACKAGE_PROPERTY_RESOURCE | PACKAGE_PROPERTY_BUNDLE | PACKAGE_PROPERTY_OPTIONAL | PACKAGE_FILTER_IS_IN_RELATED_SET | PACKAGE_FILTER_STATIC | PACKAGE_FILTER_DYNAMIC;

            UINT32 _fFlagsMarks = 0;
            if (_uPackageFilters & PACKAGE_FILTER_RESOURCE)
                _fFlagsMarks |= PACKAGE_PROPERTY_RESOURCE;

            if (_uPackageFilters & PACKAGE_FILTER_BUNDLE)
                _fFlagsMarks |= PACKAGE_PROPERTY_BUNDLE;

            if (_uPackageFilters & PACKAGE_FILTER_OPTIONAL)
                _fFlagsMarks |= PACKAGE_PROPERTY_OPTIONAL;
            
            _fFlagsMarks |= _uPackageFilters & (PACKAGE_FILTER_IS_IN_RELATED_SET | PACKAGE_FILTER_STATIC | PACKAGE_FILTER_DYNAMIC);

            _lStatusRet = GetPackagesByPackageFamily(_szPackageFamilyName, &_cRootPackageCount, nullptr, &_cchRootBufferLength, nullptr);
            if (_lStatusRet != ERROR_INSUFFICIENT_BUFFER)
            {
                break;
            }

            _pszRootPackageFullNames = (PWSTR*)_malloca(_cRootPackageCount * sizeof(_pszRootPackageFullNames[0]));
            if (!_pszRootPackageFullNames)
            {
                _lStatusRet = ERROR_OUTOFMEMORY;
                break;
            }

            _szRootPackagesBuffer = (WCHAR*)_malloca(_cchRootBufferLength * sizeof(_szRootPackagesBuffer[0]));
            if (!_szRootPackagesBuffer)
            {
                _lStatusRet = ERROR_OUTOFMEMORY;
                break;
            }

            _lStatusRet = GetPackagesByPackageFamily(_szPackageFamilyName, &_cRootPackageCount, _pszRootPackageFullNames, &_cchRootBufferLength, _szRootPackagesBuffer);
            if (_lStatusRet != ERROR_SUCCESS)
            {
                break;
            }

            for (UINT32 i = 0; i != _cRootPackageCount; ++i)
            {
                auto _szPackageFullName = _pszRootPackageFullNames[i];
                if (!_szPackageFullName)
                    continue;

                PACKAGE_INFO_REFERENCE _PackageInfoReference;
                auto _lStatus = OpenPackageInfoByFullName(_szPackageFullName, 0, &_PackageInfoReference);
                if (_lStatus != ERROR_SUCCESS)
                {
                    _lStatusRet = _lStatus;
                    break;
                }

                do
                {
                    UINT32 _cbBufferLength = _cbPackahgeInfoBuffer;
                    UINT32 _cCount = 0;
                    _lStatus = GetPackageInfo(_PackageInfoReference, _uPackageFilters, &_cbBufferLength, (BYTE*)_pPackahgeInfo, &_cCount);
                    if (_lStatus == ERROR_SUCCESS)
                    {
                    }
                    else if (_lStatus == ERROR_INSUFFICIENT_BUFFER)
                    {
                        _cbBufferLength *= 2;
                        auto _pNew = (PACKAGE_INFO*)internal::ReAlloc(_pPackahgeInfo, _cbBufferLength);
                        if (!_pNew)
                        {
                            _lStatusRet = ERROR_OUTOFMEMORY;
                            break;
                        }

                        _pPackahgeInfo = _pNew;
                        _cbPackahgeInfoBuffer = _cbBufferLength;

                        _lStatus = GetPackageInfo(_PackageInfoReference, _uPackageFilters, &_cbBufferLength, (BYTE*)_pPackahgeInfo, &_cCount);
                        if (_lStatus != ERROR_SUCCESS)
                        {
                            _lStatusRet = _lStatus;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }

                    for (UINT32 j = 0; j != _cCount; ++j)
                    {
                        auto& _PackahgeInfo = _pPackahgeInfo[j];
                        if (StringCompareIgnoreCaseByAscii(_PackahgeInfo.packageFamilyName, _szPackageFamilyName, -1) != 0)
                            continue;

                        if ((_uPackageFilters & PACKAGE_FILTER_HEAD) && (_PackahgeInfo.flags & kAllPackageProperties) == 0)
                        {
                        }
                        else if (_fFlagsMarks)
                        {
                            if ((_PackahgeInfo.flags & _fFlagsMarks) == 0)
                            {
                                continue;
                            }
                        }

                        if (_pszPackageFullNames && _lStatusRet == ERROR_SUCCESS)
                        {
                            if (internal::FindStringTable((LPCWSTR*)_pszPackageFullNames, _cOutCount, _PackahgeInfo.packageFullName))
                                continue;

                            const auto _cchPackageFullName = wcslen(_PackahgeInfo.packageFullName) + 1;
                            _cchOutBufferLength += _cchPackageFullName;

                            if (*_pcCount <= _cOutCount)
                            {
                                _lStatusRet = ERROR_INSUFFICIENT_BUFFER;
                            }
                            else if (*_pcchBufferLength < _cchOutBufferLength)
                            {
                                _lStatusRet = ERROR_INSUFFICIENT_BUFFER;
                            }
                            else
                            {
                                _pcPackageProperties[_cOutCount] = _PackahgeInfo.flags & kAllPackageProperties;
                                _pszPackageFullNames[_cOutCount] = _szBuffer;
                                memcpy(_szBuffer, _PackahgeInfo.packageFullName, _cchPackageFullName * sizeof(wchar_t));
                                _szBuffer += _cchPackageFullName;
                            }

                            ++_cOutCount;
                        }
                        else
                        {
                            const auto _cchPackageFullName = wcslen(_PackahgeInfo.packageFullName) + 1;
                            _cchOutBufferLength += _cchPackageFullName;
                            ++_cOutCount;
                        }
                    }

                } while (false);

                ClosePackageInfo(_PackageInfoReference);

                if (_lStatus != ERROR_SUCCESS)
                    break;
            }
        } while (false);


        if (_pPackahgeInfo)
        {
            internal::Free(_pPackahgeInfo);
        }

        if (_szRootPackagesBuffer)
        {
            _freea(_szRootPackagesBuffer);
        }

        if (_pszRootPackageFullNames)
        {
            _freea(_pszRootPackageFullNames);
        }

        if (_lStatusRet == ERROR_SUCCESS || _lStatusRet == ERROR_INSUFFICIENT_BUFFER)
        {
            *_pcCount = _cOutCount;
            *_pcchBufferLength = _cchOutBufferLength;

            if (_lStatusRet == ERROR_SUCCESS && _pszPackageFullNames == nullptr && _cchOutBufferLength)
            {
                _lStatusRet = ERROR_INSUFFICIENT_BUFFER;
            }
        }
        return _lStatusRet;
    }
#endif // (YY_Thunks_Target < __WindowsNT6_3)
}
