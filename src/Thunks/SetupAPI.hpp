#if (YY_Thunks_Support_Version < NTDDI_WIN6)
#include <setupapi.h>
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6) && !defined(__Comment_Lib_setupapi)
#define __Comment_Lib_setupapi
#pragma comment(lib, "Setupapi.lib")
#endif

#ifdef YY_Thunks_Implemented
namespace YY::Thunks::internal
{
    namespace
    {
#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        static int __fastcall DevicePropertyKeyToPropertyType(CONST DEVPROPKEY* PropertyKey, _Out_ DEVPROPTYPE* pPropertyType)
        {
            static constexpr const GUID PropertyId = { 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } };

            constexpr int IndexStart = 2;

            static constexpr const DEVPROPTYPE PropertyIdTypeIndex[] =
            {
                DEVPROP_TYPE_STRING,       //2
                DEVPROP_TYPE_STRING_LIST,  //3
                DEVPROP_TYPE_STRING_LIST,  //4
                -1,                        //5
                DEVPROP_TYPE_STRING,       //6
                -1,                        //7
                -1,                        //8
                DEVPROP_TYPE_STRING,       //9
                DEVPROP_TYPE_GUID,         //10
                DEVPROP_TYPE_STRING,       //11
                DEVPROP_TYPE_UINT32,       //12
                DEVPROP_TYPE_STRING,       //13
                DEVPROP_TYPE_STRING,       //14
                DEVPROP_TYPE_STRING,       //15
                DEVPROP_TYPE_STRING,       //16
                DEVPROP_TYPE_UINT32,       //17
                DEVPROP_TYPE_UINT32,       //18
                DEVPROP_TYPE_STRING_LIST,  //19,
                DEVPROP_TYPE_STRING_LIST,  //20
                DEVPROP_TYPE_GUID,         //21
                DEVPROP_TYPE_UINT32,       //22
                DEVPROP_TYPE_UINT32,       //23
                DEVPROP_TYPE_STRING,       //24
                DEVPROP_TYPE_SECURITY_DESCRIPTOR,        //25
                DEVPROP_TYPE_SECURITY_DESCRIPTOR_STRING, //26
                DEVPROP_TYPE_UINT32,       //27
                DEVPROP_TYPE_BOOLEAN,      //28
                DEVPROP_TYPE_UINT32,       //29
                DEVPROP_TYPE_UINT32,       //30
                DEVPROP_TYPE_STRING,       //31
                DEVPROP_TYPE_BINARY,       //32
                DEVPROP_TYPE_UINT32,       //33
                DEVPROP_TYPE_UINT32,       //34
                DEVPROP_TYPE_UINT32,       //35
                DEVPROP_TYPE_UINT32,       //36
                DEVPROP_TYPE_STRING_LIST,  //37
                DEVPROP_TYPE_GUID,         //38
            };

            if ( memcmp(&PropertyKey->fmtid, &PropertyId, sizeof(PropertyId)) != 0
                || PropertyKey->pid < IndexStart || PropertyKey->pid >= _countof(PropertyIdTypeIndex) + IndexStart)
            {
                return -1;
            }

            const auto Property = PropertyKey->pid - IndexStart;
            auto PropertyType = PropertyIdTypeIndex[Property];

            if (PropertyType == -1)
                return -1;
                
            *pPropertyType = PropertyType;

            return Property;
        }


        static int __fastcall ClassPropertyKeyToPropertyType(CONST DEVPROPKEY* PropertyKey, _Out_ DEVPROPTYPE* PropertyType)
        {
            static constexpr const GUID PropertyId = { 0x4321918b, 0xf69e, 0x470d, { 0xa5, 0xde, 0x4d, 0x88, 0xc7, 0x5a, 0xd2, 0x4b } };

            constexpr int IndexStart = 19;

            static constexpr const DEVPROPTYPE PropertyIdTypeIndex[] =
            {
                DEVPROP_TYPE_STRING_LIST,
                DEVPROP_TYPE_STRING_LIST,
                DEVPROP_TYPE_SECURITY_DESCRIPTOR,
                DEVPROP_TYPE_SECURITY_DESCRIPTOR_STRING,
                DEVPROP_TYPE_UINT32,
                DEVPROP_TYPE_BOOLEAN,
                DEVPROP_TYPE_UINT32,
            };

            if (memcmp(&PropertyKey->fmtid, &PropertyId, sizeof(PropertyId)) != 0
                || PropertyKey->pid < IndexStart || PropertyKey->pid >= _countof(PropertyIdTypeIndex) + IndexStart)
            {
                return -1;
            }

            auto Property = PropertyKey->pid - IndexStart;
                
            *PropertyType = PropertyIdTypeIndex[Property];

            return Property + 0x11;
        };
#endif
    }
}
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    //Available in Windows Vista and later versions of Windows.
    __DEFINE_THUNK(
    setupapi,
    32,
    _Success_(return)
	BOOL,
    WINAPI,
    SetupDiGetDevicePropertyW,
        _In_         HDEVINFO         DeviceInfoSet,
        _In_         PSP_DEVINFO_DATA DeviceInfoData,
        _In_   CONST DEVPROPKEY      *PropertyKey,
        _Out_        DEVPROPTYPE     *PropertyType,
        _Out_writes_bytes_to_opt_(PropertyBufferSize, *RequiredSize) PBYTE PropertyBuffer,
        _In_         DWORD            PropertyBufferSize,
        _Out_opt_    PDWORD           RequiredSize,
        _In_         DWORD            Flags
        )
    {
        if(const auto pSetupDiGetDevicePropertyW = try_get_SetupDiGetDevicePropertyW())
        {
            return pSetupDiGetDevicePropertyW(DeviceInfoSet, DeviceInfoData, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, RequiredSize, Flags);
        }

        if(Flags != 0)
        {
            SetLastError(ERROR_INVALID_FLAGS);
            return FALSE;
        }


        DEVPROPTYPE Type;

        const auto Property = internal::DevicePropertyKeyToPropertyType(PropertyKey, &Type);

        if (Property == -1)
        {
            SetLastError(ERROR_NOT_FOUND);
            return FALSE;
        }

        if (!SetupDiGetDeviceRegistryPropertyW(DeviceInfoSet, DeviceInfoData, Property, NULL, PropertyBuffer, PropertyBufferSize, RequiredSize))
            return FALSE;

        if(PropertyType)
        {
            *PropertyType = Type;
        }

        return TRUE;
    }
#endif //!YY_Thunks_Support_Version < NTDDI_WIN6


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    //Available in Windows Vista and later versions of Windows.
    __DEFINE_THUNK(
    setupapi,
    28,
    BOOL,
    WINAPI,
    SetupDiSetDevicePropertyW,
        _In_         HDEVINFO         DeviceInfoSet,
        _In_         PSP_DEVINFO_DATA DeviceInfoData,
        _In_   CONST DEVPROPKEY      *PropertyKey,
        _In_         DEVPROPTYPE      PropertyType,
        _In_reads_bytes_opt_(PropertyBufferSize) CONST PBYTE PropertyBuffer,
        _In_         DWORD            PropertyBufferSize,
        _In_         DWORD            Flags
        )
    {
        if (const auto pSetupDiSetDevicePropertyW = try_get_SetupDiSetDevicePropertyW())
        {
            return pSetupDiSetDevicePropertyW(DeviceInfoSet, DeviceInfoData, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, Flags);
        }

        if (Flags != 0)
        {
            SetLastError(ERROR_INVALID_FLAGS);
            return FALSE;
        }

        DEVPROPTYPE Type;

        const auto Property = internal::DevicePropertyKeyToPropertyType(PropertyKey, &Type);

        if (Property == -1)
        {
            SetLastError(ERROR_NOT_FOUND);
            return FALSE;
        }

        if(Type == PropertyType)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        return SetupDiSetDeviceRegistryPropertyW(DeviceInfoSet, DeviceInfoData, Property, PropertyBuffer, PropertyBufferSize);
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    //Available in Windows Vista and later versions of Windows.
    __DEFINE_THUNK(
    setupapi,
    28,
    _Success_(return)
    BOOL,
    WINAPI,
    SetupDiGetClassPropertyW,
        _In_   CONST GUID            *ClassGuid,
        _In_   CONST DEVPROPKEY      *PropertyKey,
        _Out_        DEVPROPTYPE     *PropertyType,
        _Out_writes_bytes_to_opt_(PropertyBufferSize, *RequiredSize) PBYTE PropertyBuffer,
        _In_         DWORD            PropertyBufferSize,
        _Out_opt_    PDWORD           RequiredSize,
        _In_         DWORD            Flags
        )
    {
        if(const auto pSetupDiGetClassPropertyW = try_get_SetupDiGetClassPropertyW())
        {
            return pSetupDiGetClassPropertyW(ClassGuid, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, RequiredSize, Flags);
        }

        DEVPROPTYPE Type;

        const auto Property = internal::ClassPropertyKeyToPropertyType(PropertyKey, &Type);

        if (Property == -1)
        {
            SetLastError(ERROR_NOT_FOUND);
            return FALSE;
        }

        if(!SetupDiGetClassRegistryPropertyW(ClassGuid, Property, nullptr, PropertyBuffer, PropertyBufferSize, RequiredSize, nullptr, nullptr))
        {
            return FALSE;
        }

        if (PropertyType)
        {
            *PropertyType = Type;
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    //Available in Windows Vista and later versions of Windows.
    __DEFINE_THUNK(
    setupapi,
    36,
    BOOL,
    WINAPI,
    SetupDiGetClassPropertyExW,
        _In_   CONST GUID            *ClassGuid,
        _In_   CONST DEVPROPKEY      *PropertyKey,
        _Out_        DEVPROPTYPE     *PropertyType,
        _Out_writes_bytes_to_opt_(PropertyBufferSize, *RequiredSize) PBYTE PropertyBuffer,
        _In_         DWORD            PropertyBufferSize,
        _Out_opt_    PDWORD           RequiredSize,
        _In_         DWORD            Flags,
        _In_opt_     PCWSTR           MachineName,
        _Reserved_   PVOID            Reserved
        )
    {
        if (const auto pSetupDiGetClassPropertyExW = try_get_SetupDiGetClassPropertyExW())
        {
            return pSetupDiGetClassPropertyExW(ClassGuid, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, RequiredSize, Flags, MachineName, Reserved);
        }

        DEVPROPTYPE Type;

        const auto Property = internal::ClassPropertyKeyToPropertyType(PropertyKey, &Type);

        if (Property == -1)
        {
            SetLastError(ERROR_NOT_FOUND);
            return FALSE;
        }

        if (!SetupDiGetClassRegistryPropertyW(ClassGuid, Property, nullptr, PropertyBuffer, PropertyBufferSize, RequiredSize, MachineName, Reserved))
        {
            return FALSE;
        }

        if (PropertyType)
        {
            *PropertyType = Type;
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    //Available in Windows Vista and later versions of Windows.
    __DEFINE_THUNK(
    setupapi,
    24,
    BOOL,
    WINAPI,
    SetupDiSetClassPropertyW,
        _In_   CONST GUID            *ClassGuid,
        _In_   CONST DEVPROPKEY      *PropertyKey,
        _In_         DEVPROPTYPE      PropertyType,
        _In_reads_bytes_opt_(PropertyBufferSize) CONST PBYTE PropertyBuffer,
        _In_         DWORD            PropertyBufferSize,
        _In_         DWORD            Flags
        )
    {
        if (const auto pSetupDiSetClassPropertyW = try_get_SetupDiSetClassPropertyW())
        {
            return pSetupDiSetClassPropertyW(ClassGuid, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, Flags);
        }

        DEVPROPTYPE Type;

        const auto Property = internal::ClassPropertyKeyToPropertyType(PropertyKey, &Type);

        if (Property == -1)
        {
            SetLastError(ERROR_NOT_FOUND);
            return FALSE;
        }

        if (Type != PropertyType)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        return SetupDiSetClassRegistryPropertyW(ClassGuid, Property, PropertyBuffer, PropertyBufferSize, nullptr, nullptr);
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    //Available in Windows Vista and later versions of Windows.
    __DEFINE_THUNK(
    setupapi,
    32,
    BOOL,
    WINAPI,
    SetupDiSetClassPropertyExW,
        _In_   CONST GUID            *ClassGuid,
        _In_   CONST DEVPROPKEY      *PropertyKey,
        _In_         DEVPROPTYPE      PropertyType,
        _In_reads_bytes_opt_(PropertyBufferSize) CONST PBYTE PropertyBuffer,
        _In_         DWORD            PropertyBufferSize,
        _In_         DWORD            Flags,
        _In_opt_     PCWSTR           MachineName,
        _Reserved_   PVOID            Reserved
        )
    {
        if (const auto pSetupDiSetClassPropertyExW = try_get_SetupDiSetClassPropertyExW())
        {
            return pSetupDiSetClassPropertyExW(ClassGuid, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, Flags, MachineName, Reserved);
        }

        DEVPROPTYPE Type;

        const auto Property = internal::ClassPropertyKeyToPropertyType(PropertyKey, &Type);

        if (Property == -1)
        {
            SetLastError(ERROR_NOT_FOUND);
            return FALSE;
        }

        if(Type != PropertyType)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        return SetupDiSetClassRegistryPropertyW(ClassGuid, Property, PropertyBuffer, PropertyBufferSize, MachineName, Reserved);
    }
#endif
}
