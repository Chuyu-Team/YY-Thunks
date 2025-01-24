#if (YY_Thunks_Target < __WindowsNT6_SP1)
#include <Shared/SmBios.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista, Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    0,
    ULONGLONG,
    WINAPI,
    GetTickCount64,
        VOID
        )
    {
        if (auto const pGetTickCount64 = try_get_GetTickCount64())
        {
            return pGetTickCount64();
        }

        __pragma(warning(suppress:28159))
        return GetTickCount();
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8, Windows Server 2012
    __DEFINE_THUNK(
    kernel32,
    4,
    VOID,
    WINAPI,
    GetSystemTimePreciseAsFileTime,
        _Out_ LPFILETIME lpSystemTimeAsFileTime
        )
    {
        if (auto const pGetSystemTimePreciseAsFileTime = try_get_GetSystemTimePreciseAsFileTime())
        {
            return pGetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
        }

        return GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1_SP3)

    //Windows Vista, Windows XP Professional x64 Edition, Windows XP with SP3 [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    8,
    BOOL,
    WINAPI,
    GetLogicalProcessorInformation,
        _Out_writes_bytes_to_opt_(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
        _Inout_ PDWORD ReturnedLength
        )
    {
        if (auto pGetLogicalProcessorInformation = try_get_GetLogicalProcessorInformation())
        {
            return pGetLogicalProcessorInformation(Buffer, ReturnedLength);
        }
    
        SetLastError(ERROR_INVALID_FUNCTION);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    //Windows 7, Windows Server 2008 R2
    __DEFINE_THUNK(
    kernel32,
    12,
    _Success_(return)
    BOOL,
    WINAPI,
    GetLogicalProcessorInformationEx,
        _In_ LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
        _Out_writes_bytes_to_opt_(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Buffer,
        _Inout_ PDWORD ReturnedLength
        )
    {
        if (auto pGetLogicalProcessorInformationEx = try_get_GetLogicalProcessorInformationEx())
        {
            return pGetLogicalProcessorInformationEx(RelationshipType, Buffer, ReturnedLength);
        }


        if (!ReturnedLength)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
    
        const auto cbBuffer = *ReturnedLength;

        if (cbBuffer != 0 && Buffer == nullptr)
        {
            SetLastError(ERROR_NOACCESS);
            return FALSE;
        }
    
        const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
        LSTATUS lStatus = ERROR_SUCCESS;

        SYSTEM_LOGICAL_PROCESSOR_INFORMATION* pProcessorInfo = nullptr;
        DWORD cbLogicalProcessorInformation = 0;

    
        for (; GetLogicalProcessorInformation(pProcessorInfo, &cbLogicalProcessorInformation) == FALSE;)
        {
            lStatus = GetLastError();

            if (ERROR_INSUFFICIENT_BUFFER == lStatus)
            {
                if (pProcessorInfo)
                {
                    auto pNewBuffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)HeapReAlloc(ProcessHeap, 0, pProcessorInfo, cbLogicalProcessorInformation);
                    if (pNewBuffer)
                    {
                        pProcessorInfo = pNewBuffer;
                        continue;
                    }
                }
                else
                {
                    pProcessorInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)HeapAlloc(ProcessHeap, 0, cbLogicalProcessorInformation);

                    if (pProcessorInfo)
                        continue;
                }

                lStatus = ERROR_NOT_ENOUGH_MEMORY;
            }

            goto __End;
        }


        {
            const auto pProcessorInfoLastItem = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)((byte*)pProcessorInfo + cbLogicalProcessorInformation - sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

            DWORD cbBufferUsed = 0;



            for (auto pProcessorInfoStart = pProcessorInfo; pProcessorInfoStart <= pProcessorInfoLastItem; ++pProcessorInfoStart)
            {
                if (RelationshipType == LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll
                    || pProcessorInfoStart->Relationship == RelationshipType)
                {
                    DWORD cbInfoNeed;

                    switch (pProcessorInfoStart->Relationship)
                    {
                    case RelationProcessorCore:
                    case RelationProcessorPackage:
                        cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Processor);
                        break;
                    case RelationNumaNode:
                        cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, NumaNode);
                        break;
                    case RelationCache:
                        cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Cache);
                        break;
                    default:
                        cbInfoNeed = 0;
                        break;
                    }

                    //不支持传输此类型
                    if (0 == cbInfoNeed)
                        continue;

                    cbBufferUsed += cbInfoNeed;

                    if (cbBuffer >= cbBufferUsed)
                    {
                        auto pInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)((byte*)Buffer + cbBufferUsed);

                        memset(pInfo, 0, cbInfoNeed);

                        pInfo->Size = cbInfoNeed;
                        pInfo->Relationship = pProcessorInfoStart->Relationship;

                        switch (pProcessorInfoStart->Relationship)
                        {
                        case RelationProcessorCore:
                        case RelationProcessorPackage:
                            pInfo->Processor.Flags = pProcessorInfoStart->ProcessorCore.Flags;
                            pInfo->Processor.GroupCount = 1;
                            pInfo->Processor.GroupMask->Mask = pProcessorInfoStart->ProcessorMask;
                            break;
                        case RelationNumaNode:
                            pInfo->NumaNode.NodeNumber = pProcessorInfoStart->NumaNode.NodeNumber;
                            pInfo->NumaNode.GroupMask.Mask = pProcessorInfoStart->ProcessorMask;
                            break;
                        case RelationCache:
                            pInfo->Cache.Level = pProcessorInfoStart->Cache.Level;
                            pInfo->Cache.Associativity = pProcessorInfoStart->Cache.Associativity;
                            pInfo->Cache.LineSize = pProcessorInfoStart->Cache.LineSize;
                            pInfo->Cache.CacheSize = pProcessorInfoStart->Cache.Size;
                            pInfo->Cache.Type = pProcessorInfoStart->Cache.Type;
                            pInfo->Cache.GroupMask.Mask= pProcessorInfoStart->ProcessorMask;
                            break;
                        }
                    }
                }
            }


            //传输 RelationGroup 信息，这里只能假设只有一组CPU
            if (RelationshipType == LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll
                || RelationGroup == RelationshipType)
            {
                const auto cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Group);

                cbBufferUsed += cbInfoNeed;

                if (cbBuffer >= cbBufferUsed)
                {
                    auto pInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)((byte*)Buffer + cbBufferUsed);

                    memset(pInfo, 0, cbInfoNeed);

                    pInfo->Size = cbInfoNeed;
                    pInfo->Relationship = RelationGroup;

                    pInfo->Group.ActiveGroupCount = 1;
                    pInfo->Group.MaximumGroupCount = 1;

                    SYSTEM_INFO SystemInfo;

                    GetSystemInfo(&SystemInfo);

                    pInfo->Group.GroupInfo->ActiveProcessorMask = SystemInfo.dwActiveProcessorMask;
                    pInfo->Group.GroupInfo->ActiveProcessorCount = internal::BitsCount(static_cast<size_t>(SystemInfo.dwActiveProcessorMask));
                    pInfo->Group.GroupInfo->MaximumProcessorCount = static_cast<BYTE>(SystemInfo.dwNumberOfProcessors);
                }
            }

            *ReturnedLength = cbBufferUsed;

            if (cbBufferUsed > cbBuffer)
            {
                //缓冲区不足
                lStatus = ERROR_INSUFFICIENT_BUFFER;
            }
        }

    __End:
        if (pProcessorInfo)
            HeapFree(ProcessHeap, 0, pProcessorInfo);

        if (lStatus != ERROR_SUCCESS)
        {
            SetLastError(lStatus);

            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1)

    //Windows XP [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    4,
    VOID,
    WINAPI,
    GetNativeSystemInfo,
        _Out_ LPSYSTEM_INFO lpSystemInfo
        )
    {
        if (const auto pGetNativeSystemInfo = try_get_GetNativeSystemInfo())
        {
            return pGetNativeSystemInfo(lpSystemInfo);
        }

        GetSystemInfo(lpSystemInfo);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    20,
    BOOL,
    WINAPI,
    GetProductInfo,
        _In_ DWORD dwOSMajorVersion,
        _In_ DWORD dwOSMinorVersion,
        _In_ DWORD dwSpMajorVersion,
        _In_ DWORD dwSpMinorVersion,
        _Out_ PDWORD pdwReturnedProductType
        )
    {
        if (const auto pGetProductInfo = try_get_GetProductInfo())
        {
            return pGetProductInfo(dwOSMajorVersion, dwOSMinorVersion, dwSpMajorVersion, dwSpMinorVersion, pdwReturnedProductType);
        }

        if (!pdwReturnedProductType)
            return FALSE;

        //故意不检测版本号，能凑合就尽可能的凑合吧……

        DWORD dwReturnedProductType = PRODUCT_UNLICENSED;

        //由于不支持这个接口，所以系统版本必然是XP或者以下
        OSVERSIONINFOEXW VersionInfo = { sizeof(VersionInfo) };
        #pragma warning(suppress:4996 28159)
        if (GetVersionExW((LPOSVERSIONINFOW)&VersionInfo))
        {
            if (VersionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
            {
                dwReturnedProductType = PRODUCT_ENTERPRISE_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_SMALLBUSINESS_RESTRICTED) // VER_SUITE_SMALLBUSINESS 与 VER_SUITE_SMALLBUSINESS_RESTRICTED
            {
                dwReturnedProductType = PRODUCT_SMALLBUSINESS_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_EMBEDDED_RESTRICTED) // VER_SUITE_EMBEDDEDNT 与 VER_SUITE_EMBEDDED_RESTRICTED
            {
                dwReturnedProductType = PRODUCT_EMBEDDED;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_DATACENTER)
            {
                dwReturnedProductType = PRODUCT_DATACENTER_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_PERSONAL)
            {
                dwReturnedProductType = PRODUCT_HOME_BASIC;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_BLADE)
            {
                dwReturnedProductType = PRODUCT_WEB_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_STORAGE_SERVER)
            {
                dwReturnedProductType = PRODUCT_STORAGE_STANDARD_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
            {
                dwReturnedProductType = PRODUCT_CLUSTER_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_WH_SERVER)
            {
                dwReturnedProductType = PRODUCT_HOME_SERVER;
            }
            else
            {
                if (VersionInfo.wProductType == VER_NT_WORKSTATION)
                {
                    //如果目标系统是5.1，因为SuiteMask中检测了Home，所以这里只可能是专业版
                    // 
                    //如果目标系统是5.0、5.2，只可能是专业版。

                    dwReturnedProductType = PRODUCT_PROFESSIONAL;
                }
                else if (VersionInfo.wProductType == VER_NT_DOMAIN_CONTROLLER || VersionInfo.wProductType == VER_NT_SERVER)
                {
                    //如果目标系统是5.1、5.2，因为SuiteMask中检测了Small Business Server、Web Edition、Enterprise Edition、
                    //Datacenter Edition、Compute Cluster Server、Storage Server，所以这里只可能是Standard Edition
                    //
                    //如果目标系统是5.0，因为SuiteMask中检测了Datacenter Server，这里只能是 Advanced Server或者 Server，但是我没有找到 Advanced，姑且用 PRODUCT_STANDARD_SERVER 凑合吧。
                    dwReturnedProductType = PRODUCT_STANDARD_SERVER;
                }
                else
                {
                    //这是什么鬼？
                    dwReturnedProductType = PRODUCT_UNDEFINED;
                }
            }
        }

        *pdwReturnedProductType = dwReturnedProductType;

        return dwReturnedProductType != PRODUCT_UNDEFINED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    // 最低受支持的客户端	Windows Vista、Windows XP Professional x64 版本 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008、Windows Server 2003 SP1[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    UINT,
    WINAPI,
    GetSystemFirmwareTable,
        _In_ DWORD _uFirmwareTableProviderSignature,
        _In_ DWORD _uFirmwareTableID,
        _Out_writes_bytes_to_opt_(_cbBufferSize,return) PVOID _pFirmwareTableBuffer,
        _In_ DWORD _cbBufferSize
        )
    {
        if (const auto _pfnGetSystemFirmwareTable = try_get_GetSystemFirmwareTable())
        {
            return _pfnGetSystemFirmwareTable(_uFirmwareTableProviderSignature, _uFirmwareTableID, _pFirmwareTableBuffer, _cbBufferSize);
        }

        UINT _cbBufferNeed = 0;
        LSTATUS _lStatus = ERROR_INVALID_FUNCTION;

        if (_uFirmwareTableProviderSignature == 'RSMB')
        {
#if !defined(__USING_NTDLL_LIB)
            const auto NtOpenSection = try_get_NtOpenSection();
            if (!NtOpenSection)
            {
                SetLastError(ERROR_INVALID_FUNCTION);
                return 0ul;
            }
#endif

            HANDLE _hMap = nullptr;
            UNICODE_STRING _szPhysicalMemory = internal::MakeStaticUnicodeString(L"\\Device\\PhysicalMemory");
            OBJECT_ATTRIBUTES _ObjectAttributes = { sizeof(_ObjectAttributes), nullptr, &_szPhysicalMemory ,OBJ_CASE_INSENSITIVE };
            LONG _Status = NtOpenSection(&_hMap, SECTION_MAP_READ, &_ObjectAttributes);
            if (_Status < 0)
            {
                internal::BaseSetLastNTError(_Status);
                return 0ul;
            }

            constexpr auto kMaxSize = 64ul * 1024;
            LPBYTE _pData = (LPBYTE)MapViewOfFile(_hMap, FILE_MAP_READ, 0, 0xF0000, kMaxSize);
            CloseHandle(_hMap);

            do
            {
                if (!_pData)
                {
                    break;
                }

                const auto _pRawSMBIOSDataEnd = _pData + kMaxSize;
                for (auto _pStart = _pData; _pStart + sizeof(SMBEntryPoint32) < _pRawSMBIOSDataEnd; _pStart += 16)
                {
                    if (*(uint32_t*)_pStart == kSMBIOS32)
                    {
                        auto _pSMBEntryPoint32 = (SMBEntryPoint32*)_pStart;
                        const auto _cbData = _pSMBEntryPoint32->entryPointLength;
                        if (_pStart + _cbData > _pRawSMBIOSDataEnd)
                            continue;

                        uint8_t _uChecksum = 0;
                        for (size_t i = 0; i != _cbData; ++i)
                        {
                            _uChecksum += _pStart[i];
                        }

                        if (_uChecksum != 0)
                            continue;

                        if (LPBYTE(_pSMBEntryPoint32->dmi.tableAddress + _pSMBEntryPoint32->dmi.tableLength) > _pRawSMBIOSDataEnd)
                            continue;

                        _cbBufferNeed = sizeof(RawSMBIOSData) + _pSMBEntryPoint32->dmi.tableLength;
                        if (_cbBufferNeed <= _cbBufferSize)
                        {
                            if (_pFirmwareTableBuffer)
                            {
                                auto _pRawSMBIOSData = (RawSMBIOSData*)_pFirmwareTableBuffer;
                                _pRawSMBIOSData->Used20CallingMethod = 0;
                                _pRawSMBIOSData->SMBIOSMajorVersion = _pSMBEntryPoint32->majorVersion;
                                _pRawSMBIOSData->SMBIOSMinorVersion = _pSMBEntryPoint32->minorVersion;
                                _pRawSMBIOSData->DmiRevision = _pSMBEntryPoint32->entryPointRevision;
                                _pRawSMBIOSData->Length = _pSMBEntryPoint32->dmi.tableLength;
                                memcpy(_pRawSMBIOSData->SMBIOSTableData, LPBYTE(_pSMBEntryPoint32->dmi.tableAddress), _pSMBEntryPoint32->dmi.tableLength);
                            }

                            _lStatus = ERROR_SUCCESS;
                        }
                        else
                        {
                            _lStatus = ERROR_INSUFFICIENT_BUFFER;
                        }
                        break;
                    }
                }
            } while (false);

            if (_pData)
            {
                UnmapViewOfFile(_pData);
            }
        }
        else if (_uFirmwareTableProviderSignature == 'ACPI')
        {
            if (!_uFirmwareTableID)
                _uFirmwareTableID = 'PCAF';

            HKEY _hKey = nullptr;
            BYTE _StaticBuffer[2048];
            auto _pData = _StaticBuffer;

            do
            {
                // Windows XP SP2
                if (RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\mssmbios\\Data", &_hKey) == ERROR_SUCCESS)
                {
                    DWORD _cbData = sizeof(_StaticBuffer);
                    _lStatus = RegQueryValueExW(_hKey, L"AcpiData", nullptr, nullptr, _pData, &_cbData);
                    if (_lStatus == ERROR_MORE_DATA)
                    {
                        _pData = (BYTE*)internal::Alloc(_cbData);
                        if (!_pData)
                        {
                            _lStatus = ERROR_OUTOFMEMORY;
                            break;
                        }
                        _lStatus = RegQueryValueExW(_hKey, L"AcpiData", nullptr, nullptr, _pData, &_cbData);
                    }

                    if (_lStatus != ERROR_SUCCESS)
                    {
                        break;
                    }

#pragma pack(push, 1)
                    struct ACPI_DESCRIPTION_HEADER
                    {
                        UINT32  Signature;
                        UINT32  Length;
                        //UINT8   Revision;
                        //UINT8   Checksum;
                        //UINT8   OemId[6];
                        //UINT64  OemTableId;
                        //UINT32  OemRevision;
                        //UINT32  CreatorId;
                        //UINT32  CreatorRevision;
                    };
#pragma pack(pop)
                    const auto _pDataEnd = _pData + _cbData;
                    for (auto _pItem = _pData; _pItem + sizeof(ACPI_DESCRIPTION_HEADER) < _pDataEnd;)
                    {
                        auto _pHeader = (ACPI_DESCRIPTION_HEADER*)_pItem;
                        if (_pHeader->Length < sizeof(ACPI_DESCRIPTION_HEADER))
                            break;

                        _pItem += _pHeader->Length;
                        if (_pItem > _pDataEnd)
                            break;

                        if (_pHeader->Signature == _uFirmwareTableID)
                        {
                            _cbBufferNeed = _pHeader->Length;
                            if (_cbBufferNeed <= _cbBufferSize)
                            {
                                if (_pFirmwareTableBuffer)
                                {
                                    memcpy(_pFirmwareTableBuffer, _pHeader, _cbBufferNeed);
                                }

                                _lStatus = ERROR_SUCCESS;
                            }
                            else
                            {
                                _lStatus = ERROR_INSUFFICIENT_BUFFER;
                            }
                            break;
                        }
                    }
                }
            } while (false);

            if (_pData && _pData != _StaticBuffer)
            {
                internal::Free(_pData);
            }

            if (_hKey)
            {
                ::RegCloseKey(_hKey);
            }
        }

        if (_lStatus)
        {
            SetLastError(_lStatus);
        }

        return _cbBufferNeed;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_SP1)

    // 最低受支持的客户端	Windows Vista SP1 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    GetPhysicallyInstalledSystemMemory,
        _Out_ PULONGLONG _puTotalMemoryInKilobytes
        )
    {
        if (const auto _pfnGetPhysicallyInstalledSystemMemory = try_get_GetPhysicallyInstalledSystemMemory())
        {
            return _pfnGetPhysicallyInstalledSystemMemory(_puTotalMemoryInKilobytes);
        }

        if (!_puTotalMemoryInKilobytes)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        auto _cbRawSMBIOSData = GetSystemFirmwareTable('RSMB', 0, 0, 0);
        if (_cbRawSMBIOSData == 0)
        {
            return FALSE;
        }

        internal::StringBuffer<BYTE> _oSMBIOSDataBuffer;
        auto _pRawSMBIOSData = (RawSMBIOSData*)_oSMBIOSDataBuffer.GetBuffer(_cbRawSMBIOSData);
        if (!_pRawSMBIOSData)
        {
            SetLastError(ERROR_OUTOFMEMORY);
            return FALSE;
        }

        _cbRawSMBIOSData = GetSystemFirmwareTable('RSMB', 0, _pRawSMBIOSData, _cbRawSMBIOSData);
        if (_cbRawSMBIOSData == 0)
        {
            return FALSE;
        }

        if (_cbRawSMBIOSData < sizeof(RawSMBIOSData) || sizeof(RawSMBIOSData) + _pRawSMBIOSData->Length > _cbRawSMBIOSData)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        // 对于早期系统，应该不太可能同时插上256条内存
        // 简化内存申请，先将内存条上限设置在256
        struct PhysicalMemoryInfo
        {
            uint64_t uTotalSize = 0;
            WORD Handle = 0;
            bool bUsed = false;
        };

        PhysicalMemoryInfo _arrPhysicalMemoryInfo[256];
        uint32_t _cPhysicalMemoryInfo = 0;

        const auto _pSMBIOSTableDataEnd = _pRawSMBIOSData->SMBIOSTableData + _pRawSMBIOSData->Length;
        for (auto _pSMBIOSTableData = _pRawSMBIOSData->SMBIOSTableData; _pSMBIOSTableData + sizeof(SMBIOSHEADER) < _pSMBIOSTableDataEnd;)
        {
            auto _pSMBIOSHeader = (SMBIOSHEADER*)_pSMBIOSTableData;
            if (_pSMBIOSHeader->Type == 127 && _pSMBIOSHeader->Length == sizeof(SMBIOSHEADER))
                break;

            auto _pNextSMBIOSTableData = _pSMBIOSTableData + _pSMBIOSHeader->Length;
            if (_pNextSMBIOSTableData > _pSMBIOSTableDataEnd)
                break;

            if (_pSMBIOSHeader->Type == 16)
            {
                if (_pSMBIOSHeader->Length >= RTL_SIZEOF_THROUGH_FIELD(PhysicalMemoryArray, NumberOfMemoryDevices))
                {
                    auto _pPhysicalMemoryArray = (PhysicalMemoryArray*)_pSMBIOSHeader;
                    if (_pPhysicalMemoryArray->Location == 3)
                    {
                        uint32_t i = 0;
                        for (; i != _cPhysicalMemoryInfo && _arrPhysicalMemoryInfo[i].Handle == _pPhysicalMemoryArray->Handle; ++i);

                        if (i == _cPhysicalMemoryInfo)
                        {
                            if (_cPhysicalMemoryInfo >= _countof(_arrPhysicalMemoryInfo))
                                break;

                            ++_cPhysicalMemoryInfo;
                            _arrPhysicalMemoryInfo[i].Handle = _pPhysicalMemoryArray->Handle;
                        }

                        _arrPhysicalMemoryInfo[i].bUsed = true;
                    }
                }
            }
            else if (_pSMBIOSHeader->Type == 17)
            {
                if (_pSMBIOSHeader->Length >= RTL_SIZEOF_THROUGH_FIELD(MemoryDevice, PartNumber))
                {
                    auto _pMemoryDevice = (MemoryDevice*)_pSMBIOSHeader;
                    if (_pMemoryDevice->Size == UINT16_MAX)
                    {
                        SetLastError(ERROR_INVALID_DATA);
                        return FALSE;
                    }

                    uint32_t i = 0;
                    for (; i != _cPhysicalMemoryInfo && _arrPhysicalMemoryInfo[i].Handle == _pMemoryDevice->MemoryArrayHandle; ++i);

                    if (i == _cPhysicalMemoryInfo)
                    {
                        if (_cPhysicalMemoryInfo >= _countof(_arrPhysicalMemoryInfo))
                            break;

                        ++_cPhysicalMemoryInfo;
                        _arrPhysicalMemoryInfo[i].Handle = _pMemoryDevice->MemoryArrayHandle;
                    }

                    // Windows 7的代码也没考虑 ExtendedSize，所以我们就不考虑32GB大内存的情况了。
                    if (_pMemoryDevice->Size & 0x8000ul)
                    {
                        // KB
                        _arrPhysicalMemoryInfo[i].uTotalSize += _pMemoryDevice->Size & 0x7FFFul;
                    }
                    else
                    {
                        // MB
                        _arrPhysicalMemoryInfo[i].uTotalSize += uint64_t(_pMemoryDevice->Size) * 1024ul;
                    }
                }
            }

            for (; _pNextSMBIOSTableData < _pSMBIOSTableDataEnd; ++_pNextSMBIOSTableData)
            {
                if (!*(WORD*)_pNextSMBIOSTableData)
                    break;
            }
            _pNextSMBIOSTableData += 2;
            _pSMBIOSTableData = _pNextSMBIOSTableData;
        }

        if (_cPhysicalMemoryInfo == 0)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        // 单位KB
        uint64_t _uSize = 0;
        for (uint32_t i = 0; i != _cPhysicalMemoryInfo; ++i)
        {
            if (_arrPhysicalMemoryInfo[i].bUsed)
            {
                _uSize += _arrPhysicalMemoryInfo[i].uTotalSize;
            }
        }

        if (_uSize == 0ull)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        MEMORYSTATUSEX _oMemoryStatus = { sizeof(_oMemoryStatus) };
        if (!GlobalMemoryStatusEx(&_oMemoryStatus))
        {
            return FALSE;
        }

        if (_oMemoryStatus.ullTotalPhys / 1024ul > _uSize)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        *_puTotalMemoryInKilobytes = _uSize;
        return TRUE;
    }
#endif
} //namespace YY::Thunks
