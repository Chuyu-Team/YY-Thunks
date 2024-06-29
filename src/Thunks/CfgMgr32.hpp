#if (YY_Thunks_Target < __WindowsNT6_1)
#include <cfgmgr32.h>
#include <SetupAPI.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_1) && !defined(__Comment_Lib_setupapi)
#define __Comment_Lib_setupapi
#pragma comment(lib, "SetupAPI.Lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6_1) && __YY_Thunks_libs && !defined(__Comment_Lib_cfgmgr32)
#define __Comment_Lib_cfgmgr32
#pragma comment(lib, "cfgmgr32.Lib")
#endif

#ifdef YY_Thunks_Implemented
namespace YY::Thunks::Fallback
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT6_1)
        struct DevNodeTempPropertyBufffer
        {
            wchar_t szId[39];
            DWORD uValue;

            static bool __fastcall HasTransform(DEVPROPTYPE PropertyType) noexcept
            {
                return PropertyType == DEVPROP_TYPE_BOOLEAN || PropertyType == DEVPROP_TYPE_GUID;
            }

            bool __fastcall TryTransformSetBuffer(DEVPROPTYPE PropertyType, _Inout_ PBYTE* _ppPropertyBuffer, _Inout_ ULONG* _pcbPropertyBufferSize) noexcept
            {
                if (PropertyType == DEVPROP_TYPE_BOOLEAN)
                {
                    if (*_pcbPropertyBufferSize < sizeof(DEVPROP_BOOLEAN))
                    {
                        return false;
                    }

                    uValue = *(DEVPROP_BOOLEAN*)*_ppPropertyBuffer ? 1 : 0;
                    *_ppPropertyBuffer = PBYTE(&uValue);
                    *_pcbPropertyBufferSize = sizeof(uValue);
                }
                else if (PropertyType == DEVPROP_TYPE_GUID)
                {
                    if (*_pcbPropertyBufferSize < sizeof(GUID))
                    {
                        return false;
                    }
                    internal::StringBuffer<WCHAR> _StringBuild(szId, _countof(szId));
                    _StringBuild.AppendChar(L'{');
                    _StringBuild.AppendGUID(*(GUID*)*_ppPropertyBuffer);
                    _StringBuild.AppendChar(L'}');

                    *_ppPropertyBuffer = PBYTE(&szId);
                    *_pcbPropertyBufferSize = sizeof(szId);
                }

                return true;
            }

            bool __fastcall TryTransformGetBuffer(DEVPROPTYPE PropertyType, _Out_writes_bytes_to_opt_(*_pcbPropertyBufferSize, *_pcbPropertyBufferSize) PBYTE PropertyBuffer, _Inout_ ULONG* _pcbPropertyBufferSize) noexcept
            {
                if (DEVPROP_TYPE_BOOLEAN == PropertyType)
                {
                    auto _bRet = true;
                    if (PropertyBuffer)
                    {
                        _bRet = *_pcbPropertyBufferSize >= sizeof(DEVPROP_BOOLEAN);
                        if (_bRet)
                        {
                            *(DEVPROP_BOOLEAN*)PropertyBuffer = uValue ? DEVPROP_TRUE : DEVPROP_FALSE;
                        }
                    }

                    *_pcbPropertyBufferSize = sizeof(DEVPROP_BOOLEAN);
                    return _bRet;
                }
                else if (DEVPROP_TYPE_GUID == PropertyType)
                {
                    auto _bRet = true;
                    if (PropertyBuffer)
                    {
                        _bRet = *_pcbPropertyBufferSize >= sizeof(GUID);
                        if (_bRet)
                        {
                            internal::StringToGuid(szId, (GUID*)PropertyBuffer);
                        }
                    }

                    *_pcbPropertyBufferSize = sizeof(GUID);
                    return _bRet;
                }
                else
                {
                    return true;
                }
            }
        };

        struct DevicePropertyToDeviceRegistryPropertyItem
        {
            ULONG CM_DeviceRegistryProperty;
            ULONG SP_DeviceRegistryProperty;
            DEVPROPTYPE PropertyType;
        };

        static const DevicePropertyToDeviceRegistryPropertyItem* __fastcall DevicePropertyToDeviceRegistryProperty(_In_ const DEVPROPKEY* PropertyKey) noexcept
        {
            static const GUID s_BaseId = { 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } };
            constexpr auto kBaseIndex = 2;

            static const DevicePropertyToDeviceRegistryPropertyItem s_Map[] =
            {
                { CM_DRP_DEVICEDESC, SPDRP_DEVICEDESC, DEVPROP_TYPE_STRING }, // 2 DEVPKEY_Device_DeviceDesc
                { CM_DRP_HARDWAREID, SPDRP_HARDWAREID, DEVPROP_TYPE_STRING_LIST }, // 3 DEVPKEY_Device_HardwareIds
                { CM_DRP_COMPATIBLEIDS, SPDRP_COMPATIBLEIDS, DEVPROP_TYPE_STRING_LIST }, // 4 DEVPKEY_Device_CompatibleIds
                {}, //5
                { CM_DRP_SERVICE, SPDRP_SERVICE, DEVPROP_TYPE_STRING }, // 6 DEVPKEY_Device_Service
                {}, // 7
                {}, // 8
                { CM_DRP_CLASS, SPDRP_CLASS, DEVPROP_TYPE_STRING }, // 9 DEVPKEY_Device_Class
                { CM_DRP_CLASSGUID, SPDRP_CLASSGUID, DEVPROP_TYPE_GUID }, // 10 DEVPKEY_Device_ClassGuid
                { CM_DRP_DRIVER, SPDRP_DRIVER, DEVPROP_TYPE_STRING }, // 11 DEVPKEY_Device_Driver
                { CM_DRP_CONFIGFLAGS, SPDRP_CONFIGFLAGS, DEVPROP_TYPE_UINT32 }, // 12 DEVPKEY_Device_ConfigFlags
                { CM_DRP_MFG, SPDRP_MFG, DEVPROP_TYPE_STRING }, // 13 DEVPKEY_Device_Manufacturer
                { CM_DRP_FRIENDLYNAME, SPDRP_FRIENDLYNAME, DEVPROP_TYPE_STRING }, // 14 DEVPKEY_Device_FriendlyName
                { CM_DRP_LOCATION_INFORMATION, SPDRP_LOCATION_INFORMATION, DEVPROP_TYPE_STRING }, // 15 DEVPKEY_Device_LocationInfo
                { CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, DEVPROP_TYPE_STRING }, // 16 DEVPKEY_Device_PDOName
                { CM_DRP_CAPABILITIES, SPDRP_CAPABILITIES, DEVPROP_TYPE_UINT32 }, // 17 DEVPKEY_Device_Capabilities
                { CM_DRP_UI_NUMBER, SPDRP_UI_NUMBER, DEVPROP_TYPE_UINT32 }, // 18 DEVPKEY_Device_UINumber
                { CM_DRP_UPPERFILTERS, SPDRP_UPPERFILTERS, DEVPROP_TYPE_STRING_LIST }, // 19 DEVPKEY_Device_UpperFilters
                { CM_DRP_LOWERFILTERS, SPDRP_LOWERFILTERS, DEVPROP_TYPE_STRING_LIST }, // 20 DEVPKEY_Device_LowerFilters
                { CM_DRP_BUSTYPEGUID, SPDRP_BUSTYPEGUID, DEVPROP_TYPE_GUID }, // 21 DEVPKEY_Device_BusTypeGuid
                { CM_DRP_LEGACYBUSTYPE, SPDRP_LEGACYBUSTYPE, DEVPROP_TYPE_UINT32 }, // 22 DEVPKEY_Device_LegacyBusType
                { CM_DRP_BUSNUMBER, SPDRP_BUSNUMBER, DEVPROP_TYPE_UINT32 }, // 23 DEVPKEY_Device_BusNumber
                { CM_DRP_ENUMERATOR_NAME, SPDRP_ENUMERATOR_NAME, DEVPROP_TYPE_STRING }, // 24 DEVPKEY_Device_EnumeratorName
                { CM_DRP_SECURITY, SPDRP_SECURITY, DEVPROP_TYPE_SECURITY_DESCRIPTOR }, // 25 DEVPKEY_Device_Security
                { CM_DRP_SECURITY_SDS, SPDRP_SECURITY_SDS, DEVPROP_TYPE_SECURITY_DESCRIPTOR_STRING }, // 26 DEVPKEY_Device_SecuritySDS
                { CM_DRP_DEVTYPE, SPDRP_DEVTYPE, DEVPROP_TYPE_UINT32 }, // 27 DEVPKEY_Device_DevType
                { CM_DRP_EXCLUSIVE, SPDRP_EXCLUSIVE, DEVPROP_TYPE_BOOLEAN }, // 28 DEVPKEY_Device_Exclusive
                { CM_DRP_CHARACTERISTICS, SPDRP_CHARACTERISTICS, DEVPROP_TYPE_UINT32 }, // 29 DEVPKEY_Device_Characteristics
                { CM_DRP_ADDRESS, SPDRP_ADDRESS, DEVPROP_TYPE_UINT32 }, // 30 DEVPKEY_Device_Address
                { CM_DRP_UI_NUMBER_DESC_FORMAT, SPDRP_UI_NUMBER_DESC_FORMAT, DEVPROP_TYPE_STRING }, // 31 DEVPKEY_Device_UINumberDescFormat
                { CM_DRP_DEVICE_POWER_DATA, SPDRP_DEVICE_POWER_DATA, DEVPROP_TYPE_BINARY }, // 32 DEVPKEY_Device_PowerData
                { CM_DRP_REMOVAL_POLICY, SPDRP_REMOVAL_POLICY, DEVPROP_TYPE_UINT32 }, // 33 DEVPKEY_Device_RemovalPolicy
                { CM_DRP_REMOVAL_POLICY_HW_DEFAULT, SPDRP_REMOVAL_POLICY_HW_DEFAULT, DEVPROP_TYPE_UINT32 }, // 34 DEVPKEY_Device_RemovalPolicyDefault
                { CM_DRP_REMOVAL_POLICY_OVERRIDE, SPDRP_REMOVAL_POLICY_OVERRIDE, DEVPROP_TYPE_UINT32 }, // 35 DEVPKEY_Device_RemovalPolicyOverride
                { CM_DRP_INSTALL_STATE, SPDRP_INSTALL_STATE, DEVPROP_TYPE_UINT32 }, // 36 DEVPKEY_Device_InstallState
                { CM_DRP_LOCATION_PATHS, SPDRP_LOCATION_PATHS, DEVPROP_TYPE_STRING_LIST }, // 37 DEVPKEY_Device_LocationPaths
                // { CM_DRP_BASE_CONTAINERID, SPDRP_BASE_CONTAINERID, DEVPROP_TYPE_GUID }, // 38 DEVPKEY_Device_BaseContainerId Windows 7才支持
            };

            if (PropertyKey == nullptr || PropertyKey->pid  < kBaseIndex  || PropertyKey->pid >= (kBaseIndex + _countof(s_Map)) || IsEqualGUID(PropertyKey->fmtid, s_BaseId) == false)
                return nullptr;

            auto& _Item = s_Map[PropertyKey->pid];
            if (_Item.PropertyType == DEVPROP_TYPE_EMPTY)
                return nullptr;

            return &_Item;
        }

        struct ClassPropertyToClassRegistryPropertyItem
        {
            ULONG CM_ClassRegistryProperty;
            ULONG SP_ClassRegistryProperty;
            DEVPROPTYPE PropertyType;
        };

        static const ClassPropertyToClassRegistryPropertyItem* __fastcall ClassPropertyToClassRegistryProperty(_In_ const DEVPROPKEY* PropertyKey) noexcept
        {
            static const GUID s_BaseId = { 0x4321918b, 0xf69e, 0x470d, { 0xa5, 0xde, 0x4d, 0x88, 0xc7, 0x5a, 0xd2, 0x4b } };
            constexpr auto kBaseIndex = 19;

            static const ClassPropertyToClassRegistryPropertyItem s_Map[] =
            {
                { CM_CRP_UPPERFILTERS, SPCRP_UPPERFILTERS, DEVPROP_TYPE_STRING_LIST }, // 19 DEVPKEY_DeviceClass_UpperFilters Vista开始才支持
                { CM_CRP_LOWERFILTERS, SPCRP_LOWERFILTERS, DEVPROP_TYPE_STRING_LIST }, // 20 DEVPKEY_DeviceClass_LowerFilters Vista开始才支持
                {}, // 21
                {}, // 22
                {}, // 23
                {}, // 24
                { CM_CRP_SECURITY, SPCRP_SECURITY, DEVPROP_TYPE_SECURITY_DESCRIPTOR }, // 25 DEVPKEY_DeviceClass_Security
                { CM_CRP_SECURITY_SDS, SPCRP_SECURITY_SDS, DEVPROP_TYPE_SECURITY_DESCRIPTOR_STRING }, // 26 DEVPKEY_DeviceClass_SecuritySDS
                { CM_CRP_DEVTYPE, SPCRP_DEVTYPE, DEVPROP_TYPE_UINT32 }, // 27 DEVPKEY_DeviceClass_DevType
                { CM_CRP_EXCLUSIVE, SPCRP_EXCLUSIVE, DEVPROP_TYPE_BOOLEAN }, // 28 DEVPKEY_DeviceClass_Exclusive
                { CM_CRP_CHARACTERISTICS, SPCRP_CHARACTERISTICS, DEVPROP_TYPE_UINT32 }, // 29 DEVPKEY_DeviceClass_Characteristics
            };

            if (PropertyKey == nullptr || PropertyKey->pid < kBaseIndex || PropertyKey->pid >= (kBaseIndex + _countof(s_Map)) || IsEqualGUID(PropertyKey->fmtid, s_BaseId) == false)
                return nullptr;

            auto& _Item = s_Map[PropertyKey->pid];
            if (_Item.PropertyType == DEVPROP_TYPE_EMPTY)
                return nullptr;

            return &_Item;
        }

        void* __fastcall try_get_CM_Get_DevNode_Property_ExW(const ProcInfo& _ProcInfo)
        {
             auto _pProc = try_get_proc_address_from_dll(_ProcInfo);
             if (_pProc)
                 return _pProc;

             static constexpr const ProcOffsetInfo kProcInfo[] =
             {
#if defined(_X86_)
                 { 0x4549BDB0ul, 0x7427B681ul - 0x74270000ul }, // 6.0.6000.16386 (Windows Vista RTM)
                 { 0x4791A754ul, 0x738DB93Dul - 0x738D0000ul }, // 6.0.6002.18000 (Windows Vista SP1)
                 { 0x49E037E9ul, 0x744DED36ul - 0x744D0000ul }, // 6.0.6002.18005 (Windows Vista SP2)
#elif defined(_AMD64_)
                 { 0x4549D318ul, 0x000007FF7C006400ull - 0x7FF7C000000ull }, // 6.0.6000.16386 (Windows Vista RTM)
                 { 0x4791ADA3ul, 0x000007FF7BC06E00ull - 0x7FF7BC00000ull }, // 6.0.6002.18000 (Windows Vista SP1)
                 { 0x49E041EDul, 0x000007FF7C007090ull - 0x7FF7C000000ull }, // 6.0.6002.18005 (Windows Vista SP2)
#endif
             };

             __WarningMessage__("try_get_CM_Get_DevNode_Property_ExW 可能遗漏Vista/2008某些补丁中的setupapi.dll，如果你知道详细可以提交PR。");
             return try_get_proc_address_from_offset(try_get_module_setupapi(), kProcInfo);
        }

        void* __fastcall try_get_CM_Set_DevNode_Property_ExW(const ProcInfo& _ProcInfo)
        {
            auto _pProc = try_get_proc_address_from_dll(_ProcInfo);
            if (_pProc)
                return _pProc;

            static constexpr const ProcOffsetInfo kProcInfo[] =
            {
#if defined(_X86_)
                { 0x4549BDB0ul, 0x7433C705ul - 0x74270000ul }, // 6.0.6000.16386 (Windows Vista RTM)
                { 0x4791A754ul, 0x7399D7ADul - 0x738D0000ul }, // 6.0.6002.18000 (Windows Vista SP1)
                { 0x49E037E9ul, 0x7459DC1Dul - 0x744D0000ul }, // 6.0.6002.18005 (Windows Vista SP2)
#elif defined(_AMD64_)
                { 0x4549D318ul, 0x000007FF7C0FDD70ull - 0x7FF7C000000ull }, // 6.0.6000.16386 (Windows Vista RTM)
                { 0x4791ADA3ul, 0x000007FF7BCE1E08ull - 0x7FF7BC00000ull }, // 6.0.6002.18000 (Windows Vista SP1)
                { 0x49E041EDul, 0x000007FF7C0E2550ull - 0x7FF7C000000ull }, // 6.0.6002.18005 (Windows Vista SP2)
#endif
            };

            __WarningMessage__("try_get_CM_Set_DevNode_Property_ExW 可能遗漏Vista/2008某些补丁中的setupapi.dll，如果你知道详细可以提交PR。");
            return try_get_proc_address_from_offset(try_get_module_setupapi(), kProcInfo);
        }
#endif
    }
}
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_1)

    // Windows 7 RTM 导出
    // Windows Vista有这个函数但是没有导出
    __DEFINE_THUNK(
    cfgmgr32,
    28,
    CONFIGRET,
    WINAPI,
    CM_Get_DevNode_Property_ExW,
        _In_  DEVINST       dnDevInst,
        _In_  CONST DEVPROPKEY* PropertyKey,
        _Out_ DEVPROPTYPE* PropertyType,
        _Out_writes_bytes_opt_(*PropertyBufferSize) PBYTE PropertyBuffer,
        _Inout_ PULONG      PropertyBufferSize,
        _In_  ULONG         ulFlags,
        _In_opt_ HMACHINE   hMachine
        )
    {
        if (const auto _pfnCM_Get_DevNode_Property_ExW = try_get_CM_Get_DevNode_Property_ExW())
        {
            return _pfnCM_Get_DevNode_Property_ExW(dnDevInst, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, ulFlags, hMachine);
        }

        if (!PropertyKey)
            return CR_INVALID_DEVINST;

        if (PropertyKey == nullptr || PropertyType == nullptr || PropertyBufferSize == nullptr)
            return CR_INVALID_POINTER;

        if (ulFlags != 0)
            return CR_INVALID_FLAG;

        const auto _pProperty = Fallback::DevicePropertyToDeviceRegistryProperty(PropertyKey);
        if (!_pProperty)
            return CR_INVALID_PROPERTY;

        *PropertyType = _pProperty->PropertyType;

        if (!Fallback::DevNodeTempPropertyBufffer::HasTransform(_pProperty->PropertyType))
        {
            return CM_Get_DevNode_Registry_Property_ExW(dnDevInst, _pProperty->CM_DeviceRegistryProperty, nullptr, PropertyBuffer, PropertyBufferSize, ulFlags, hMachine);
        }

        Fallback::DevNodeTempPropertyBufffer TempBuffer = {};
        ULONG _cbTempBuffer = sizeof(TempBuffer);
        auto _Error = CM_Get_DevNode_Registry_Property_ExW(dnDevInst, _pProperty->CM_DeviceRegistryProperty, nullptr, &TempBuffer, &_cbTempBuffer, ulFlags, hMachine);
        if (CR_SUCCESS != _Error)
        {
            return _Error;
        }

        if (!TempBuffer.TryTransformGetBuffer(_pProperty->PropertyType, PropertyBuffer, PropertyBufferSize))
            return CR_BUFFER_SMALL;

        return CR_SUCCESS;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // Windows 7 RTM 导出
    // Windows Vista有这个函数但是没有导出
    __DEFINE_THUNK(
    cfgmgr32,
    28,
    CONFIGRET,
    WINAPI,
    CM_Set_DevNode_Property_ExW,
        _In_  DEVINST       dnDevInst,
        _In_  CONST DEVPROPKEY* PropertyKey,
        _In_  DEVPROPTYPE   PropertyType,
        _In_reads_bytes_opt_(PropertyBufferSize) PBYTE PropertyBuffer,
        _In_  ULONG         PropertyBufferSize,
        _In_  ULONG         ulFlags,
        _In_opt_ HMACHINE   hMachine
        )
    {
        if (const auto _pfnCM_Set_DevNode_Property_ExW = try_get_CM_Set_DevNode_Property_ExW())
        {
            return _pfnCM_Set_DevNode_Property_ExW(dnDevInst, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, ulFlags, hMachine);
        }

        if (!PropertyKey)
            return CR_INVALID_DEVINST;

        if (PropertyKey == nullptr || (PropertyBuffer == nullptr && PropertyBufferSize))
            return CR_INVALID_POINTER;

        if (ulFlags != 0)
            return CR_INVALID_FLAG;

        const auto _pProperty = Fallback::DevicePropertyToDeviceRegistryProperty(PropertyKey);
        if (!_pProperty)
            return CR_INVALID_PROPERTY;

        if (_pProperty->PropertyType != PropertyType)
            return CR_WRONG_TYPE;

        Fallback::DevNodeTempPropertyBufffer TempBuffer;
        if (!TempBuffer.TryTransformSetBuffer(PropertyType, &PropertyBuffer, &PropertyBufferSize))
            return CR_BUFFER_SMALL;

        return CM_Set_DevNode_Registry_Property_ExW(dnDevInst, _pProperty->CM_DeviceRegistryProperty, PropertyBuffer, PropertyBufferSize, ulFlags, hMachine);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	在 Microsoft Windows Vista 和更高版本的 Windows 中可用。
    // 实际Windows Vista RTM根本就没有这个接口
    __DEFINE_THUNK(
    cfgmgr32,
    24,
    CONFIGRET,
    WINAPI,
    CM_Get_DevNode_PropertyW,
        _In_  DEVINST       dnDevInst,
        _In_  CONST DEVPROPKEY* PropertyKey,
        _Out_ DEVPROPTYPE* PropertyType,
        _Out_writes_bytes_opt_(*PropertyBufferSize) PBYTE PropertyBuffer,
        _Inout_ PULONG      PropertyBufferSize,
        _In_  ULONG         ulFlags
        )
    {
        if (const auto _pfnCM_Get_DevNode_PropertyW = try_get_CM_Get_DevNode_PropertyW())
        {
            return _pfnCM_Get_DevNode_PropertyW(dnDevInst, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, ulFlags);
        }

        return ::CM_Get_DevNode_Property_ExW(dnDevInst, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, ulFlags, nullptr);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	在 Microsoft Windows Vista 和更高版本的 Windows 中可用。
    // 实际Windows Vista RTM根本就没有这个接口
    __DEFINE_THUNK(
    cfgmgr32,
    24,
    CONFIGRET,
    WINAPI,
    CM_Set_DevNode_PropertyW,
        _In_  DEVINST       dnDevInst,
        _In_  CONST DEVPROPKEY *PropertyKey,
        _In_  DEVPROPTYPE   PropertyType,
        _In_reads_bytes_opt_(PropertyBufferSize) PBYTE PropertyBuffer,
        _In_  ULONG         PropertyBufferSize,
        _In_  ULONG         ulFlags
        )
    {
        if (const auto _pfnCM_Set_DevNode_PropertyW = try_get_CM_Set_DevNode_PropertyW())
        {
            return _pfnCM_Set_DevNode_PropertyW(dnDevInst, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, ulFlags);
        }      

        return ::CM_Set_DevNode_Property_ExW(dnDevInst, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, ulFlags, nullptr);
    }
#endif
}
