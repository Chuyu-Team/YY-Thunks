﻿#if (YY_Thunks_Target < __WindowsNT6_3)
#include <appmodel.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_1)

    //Minimum supported client	Windows Vista
    //Minimum supported server	Windows Server 2008
    //Windows XP有这个API啊……微软文档抽风了？
    __DEFINE_THUNK(
    kernel32,
    0,
    DWORD,
    WINAPI,
    WTSGetActiveConsoleSessionId,
        VOID
        )
    {
        if (const auto pWTSGetActiveConsoleSessionId = try_get_WTSGetActiveConsoleSessionId())
        {
            return pWTSGetActiveConsoleSessionId();
        }

        //因为Windows 2000没有会话隔离，所有的进程始终在 0 会话中运行，因此直接返回 0 即可。
        return 0;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1_SP2)

    //Minimum supported client	Windows Vista, Windows XP Professional x64 Edition, Windows XP with SP2[desktop apps only]
    //Minimum supported server	Windows Server 2003 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    8,
    BOOL,
    WINAPI,
    GetNumaProcessorNode,
        _In_  UCHAR Processor,
        _Out_ PUCHAR NodeNumber
        )
    {
        if (const auto pGetNumaProcessorNode = try_get_GetNumaProcessorNode())
        {
            return pGetNumaProcessorNode(Processor, NodeNumber);
        }

        //对于没有 Node 概念的系统，我们统一认为只有一个 Node。

        SYSTEM_INFO SystemInfo;
        GetSystemInfo(&SystemInfo);

        if (SystemInfo.dwNumberOfProcessors < Processor)
        {
            *NodeNumber = '\0';
            return TRUE;
        }
        else
        {
            *NodeNumber = 0xffu;

            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    //Minimum supported client	Windows 7 [desktop apps only]
    //Minimum supported server	Windows Server 2008 R2 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    8,
    BOOL,
    WINAPI,
    GetNumaNodeNumberFromHandle,
        _In_  HANDLE hFile,
        _Out_ PUSHORT NodeNumber
        )
    {
        if (const auto pGetNumaNodeNumberFromHandle = try_get_GetNumaNodeNumberFromHandle())
        {
            return pGetNumaNodeNumberFromHandle(hFile, NodeNumber);
        }

        //始终认为来自 Node 0
        *NodeNumber = 0;

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    //Minimum supported client	Windows 7 [desktop apps only]
    //Minimum supported server	Windows Server 2008 R2 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    8,
    BOOL,
    WINAPI,
    GetNumaProcessorNodeEx,
        _In_  PPROCESSOR_NUMBER Processor,
        _Out_ PUSHORT NodeNumber
        )
    {
        if (const auto pGetNumaProcessorNodeEx = try_get_GetNumaProcessorNodeEx())
        {
            return pGetNumaProcessorNodeEx(Processor, NodeNumber);
        }

        //老版本系统假定只有一组处理器
        if (Processor->Group == 0)
        {
            UCHAR NodeNumberTmp;
            auto bRet = GetNumaProcessorNode(Processor->Number, &NodeNumberTmp);

            if (bRet)
            {
                *NodeNumber = NodeNumberTmp;
            }
            else
            {
                *NodeNumber = 0xffffu;
            }

            return bRet;
        }

        *NodeNumber = 0xffffu;

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1_SP2)

    //Minimum supported client	Windows Vista, Windows XP Professional x64 Edition, Windows XP with SP2 [desktop apps only]
    //Minimum supported server	Windows Server 2003 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    8,
    _Success_(return)
    BOOL,
    WINAPI,
    GetNumaAvailableMemoryNode,
        _In_  UCHAR Node,
        _Out_ PULONGLONG AvailableBytes
        )
    {
        if (const auto pGetNumaAvailableMemoryNode = try_get_GetNumaAvailableMemoryNode())
        {
            return pGetNumaAvailableMemoryNode(Node, AvailableBytes);
        }

        if (Node == 0)
        {
            //统一的假定，Node数量为 1，所以该值必然为 0
            //我们把所有可用内存都认为是该节点0的可用内存。
            MEMORYSTATUSEX statex = { sizeof(statex) };

            if (!GlobalMemoryStatusEx(&statex))
            {
                return FALSE;
            }

            *AvailableBytes = statex.ullAvailPhys;
            return TRUE;
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    //Minimum supported client	Windows 7 [desktop apps only]
    //Minimum supported server	Windows Server 2008 R2 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    8,
    _Success_(return)
    BOOL,
    WINAPI,
    GetNumaAvailableMemoryNodeEx,
        _In_  USHORT Node,
        _Out_ PULONGLONG AvailableBytes
        )
    {
        if (const auto pGetNumaAvailableMemoryNodeEx = try_get_GetNumaAvailableMemoryNodeEx())
        {
            return pGetNumaAvailableMemoryNodeEx(Node, AvailableBytes);
        }


        //GetNumaAvailableMemoryNode 最大只接受 0xFF
        if (Node >= 0x100u)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        return GetNumaAvailableMemoryNode(UCHAR(Node), AvailableBytes);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps only]
    //Minimum supported server	Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    8,
    BOOL,
    WINAPI,
    GetNumaProximityNode,
        _In_  ULONG ProximityId,
        _Out_ PUCHAR NodeNumber
        )
    {
        if (const auto pGetNumaProximityNode = try_get_GetNumaProximityNode())
        {
            return pGetNumaProximityNode(ProximityId, NodeNumber);
        }

        //我们不知道CPU的组成情况，但是我们可以假定最接近的分组就是 Node 0。
        //对于电脑来说，Node数量始终等于 1，因此问题不是特别的大。
        *NodeNumber = 0;
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps only]
    //Minimum supported server	Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    28,
    LPVOID,
    WINAPI,
    MapViewOfFileExNuma,
        _In_     HANDLE hFileMappingObject,
        _In_     DWORD dwDesiredAccess,
        _In_     DWORD dwFileOffsetHigh,
        _In_     DWORD dwFileOffsetLow,
        _In_     SIZE_T dwNumberOfBytesToMap,
        _In_opt_ LPVOID lpBaseAddress,
        _In_     DWORD nndPreferred
        )
    {
        if (const auto pMapViewOfFileExNuma = try_get_MapViewOfFileExNuma())
        {
            return pMapViewOfFileExNuma(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap, lpBaseAddress, nndPreferred);
        }

        return MapViewOfFileEx(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap, lpBaseAddress);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    GetFirmwareType,
        _Inout_ PFIRMWARE_TYPE _peFirmwareType
        )
    {
        if (const auto _pfnGetFirmwareType = try_get_GetFirmwareType())
        {
            return _pfnGetFirmwareType(_peFirmwareType);
        }

        if (!_peFirmwareType)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtQuerySystemInformation = try_get_NtQuerySystemInformation();
        if (!NtQuerySystemInformation)
        {
            // 理论上不可能走到这里，最大的可能就是Windows 2000或者更早的系统了。
            // 所以我们这里兜底返回 FirmwareTypeBios，因为以前的系统只能是这个了。
            *_peFirmwareType = FIRMWARE_TYPE::FirmwareTypeBios;
            return TRUE;
        }
#endif

        SYSTEM_BOOT_ENVIRONMENT_INFORMATION _Information;
        const LONG _Status = NtQuerySystemInformation(SystemBootEnvironmentInformation, &_Information, sizeof(_Information), nullptr);

        if (_Status >= 0)
        {
            *_peFirmwareType = _Information.FirmwareType;
            return TRUE;
        }
        else if (_Status != STATUS_INVALID_INFO_CLASS && _Status != STATUS_NOT_IMPLEMENTED)
        {
            internal::BaseSetLastNTError(_Status);
            return FALSE;
        }
        else
        {
            // 当前系统不支持 SystemBootEnvironmentInformation，应该是 Windows 2000。
            *_peFirmwareType = FIRMWARE_TYPE::FirmwareTypeBios;
            return TRUE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    IsNativeVhdBoot,
        _Out_ PBOOL _pbNativeVhdBoot
        )
    {
        if (const auto _pfnIsNativeVhdBoot = try_get_IsNativeVhdBoot())
        {
            return _pfnIsNativeVhdBoot(_pbNativeVhdBoot);
        }

        if (!_pbNativeVhdBoot)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtQuerySystemInformation = try_get_NtQuerySystemInformation();
        if (!NtQuerySystemInformation)
        {
            // 兜底处理，现在是早期不支持VHD启动的系统。
            *_pbNativeVhdBoot = FALSE;
            return TRUE;
        }
#endif
        // Windows 7支持VHD启动，但是没有这个接口，直接调用 NtQuerySystemInformation 兼容一下。
        ULONG _uReturnLength = 0;
        LONG _Status = NtQuerySystemInformation(SystemVhdBootInformation, nullptr, 0, &_uReturnLength);
        if (_Status == STATUS_BUFFER_TOO_SMALL)
        {
            union
            {
                char Buffer[1024];
                SYSTEM_VHD_BOOT_INFORMATION Info;
            } _StaticBuffer;
                    
            if (_uReturnLength > sizeof(_StaticBuffer))
            {
                const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
                auto _pInformation = (SYSTEM_VHD_BOOT_INFORMATION*)HeapAlloc(_hProcessHeap, 0, _uReturnLength);
                if (!_pInformation)
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    return FALSE;
                }

                _Status = NtQuerySystemInformation(SystemVhdBootInformation, _pInformation, _uReturnLength, &_uReturnLength);
                _StaticBuffer.Info.OsDiskIsVhd = _pInformation->OsDiskIsVhd;
                HeapFree(_hProcessHeap, 0, _pInformation);
            }
            else
            {
                _Status = NtQuerySystemInformation(SystemVhdBootInformation, &_StaticBuffer.Info, _uReturnLength, &_uReturnLength);
            }

            if (_Status >= 0)
            {
                *_pbNativeVhdBoot = _StaticBuffer.Info.OsDiskIsVhd;
                return TRUE;
            }
        }
        
        if (_Status != STATUS_INVALID_INFO_CLASS && _Status != STATUS_NOT_IMPLEMENTED)
        {
            internal::BaseSetLastNTError(_Status);
            return FALSE;
        }

        // 当前系统不支持VHD，兜底处理。
        *_pbNativeVhdBoot = FALSE;
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_SP1)

    // 最低受支持的客户端	Windows Vista SP1、Windows XP SP3 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    SetProcessDEPPolicy,
        _In_ DWORD dwFlags
        )
    {
        if (const auto _pfnSetProcessDEPPolicy = try_get_SetProcessDEPPolicy())
        {
            return _pfnSetProcessDEPPolicy(dwFlags);
        }

#if defined(_AMD64_)
        // https://learn.microsoft.com/zh-cn/windows/win32/api/winbase/nf-winbase-setprocessdeppolicy
        // 微软文档说明，这个函数只限于x64进程调用。
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
#elif defined(_X86_)
        if (dwFlags & (~(PROCESS_DEP_ENABLE | PROCESS_DEP_DISABLE_ATL_THUNK_EMULATION)))
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        DWORD _uInfo = 0;
        if (dwFlags == 0)
        {
            _uInfo = 2;
        }
        else if (dwFlags & PROCESS_DEP_ENABLE)
        {
            _uInfo = (dwFlags & PROCESS_DEP_DISABLE_ATL_THUNK_EMULATION) ? 13 : 9;
        }
        else
        {
            internal::BaseSetLastNTError(0xC0000030);
            return FALSE;
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtSetInformationProcess = try_get_NtSetInformationProcess();
        if (!NtSetInformationProcess)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
#endif

        LONG _Status = NtSetInformationProcess(NtCurrentProcess(), PROCESSINFOCLASS::ProcessExecuteFlags, &_uInfo, sizeof(_uInfo));
        if (_Status >=0 || STATUS_INVALID_INFO_CLASS == _Status || STATUS_NOT_SUPPORTED == _Status)
        {
            // 如果不支持这个接口，那么也认为是成功的。反正不支持DEP。
            return TRUE;
        }
        else
        {
            internal::BaseSetLastNTError(_Status);
            return FALSE;
        }
#else
#error 未知系统？
#endif
    }
#endif // (YY_Thunks_Target < __WindowsNT6_SP1)


#if (YY_Thunks_Target < __WindowsNT6_SP1)

    // 最低受支持的客户端	带 SP1 的 Windows Vista、带 SP3 的 Windows XP [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    0,
    DEP_SYSTEM_POLICY_TYPE,
    WINAPI,
    GetSystemDEPPolicy,
        )
    {
        if (const auto _pfnGetSystemDEPPolicy = try_get_GetSystemDEPPolicy())
        {
            return _pfnGetSystemDEPPolicy();
        }

        return DEP_SYSTEM_POLICY_TYPE::DEPPolicyAlwaysOff;
    }
#endif // (YY_Thunks_Target < __WindowsNT6_SP1)


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
