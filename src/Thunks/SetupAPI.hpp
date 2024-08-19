#if (YY_Thunks_Target < __WindowsNT6)
#include <setupapi.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_setupapi)
#define __Comment_Lib_setupapi
#pragma comment(lib, "Setupapi.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

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
        if(const auto _pfnSetupDiGetDevicePropertyW = try_get_SetupDiGetDevicePropertyW())
        {
            return _pfnSetupDiGetDevicePropertyW(DeviceInfoSet, DeviceInfoData, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, RequiredSize, Flags);
        }

        if (!DeviceInfoSet)
        {
            SetLastError(ERROR_INVALID_HANDLE);
            return FALSE;
        }

        if (DeviceInfoData == nullptr || PropertyKey == nullptr || PropertyType == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if(Flags != 0)
        {
            SetLastError(ERROR_INVALID_FLAGS);
            return FALSE;
        }

        if (PropertyBuffer == nullptr && (PropertyBufferSize || RequiredSize == nullptr))
        {
            SetLastError(ERROR_INVALID_USER_BUFFER);
            return FALSE;
        }

        const auto _pProperty = Fallback::DevicePropertyToDeviceRegistryProperty(PropertyKey);
        if (!_pProperty)
        {
            SetLastError(ERROR_INVALID_REG_PROPERTY);
            return FALSE;
        }

        *PropertyType = _pProperty->PropertyType;
        if (!Fallback::DevNodeTempPropertyBufffer::HasTransform(_pProperty->PropertyType))
            return SetupDiGetDeviceRegistryPropertyW(DeviceInfoSet, DeviceInfoData, _pProperty->SP_DeviceRegistryProperty, NULL, PropertyBuffer, PropertyBufferSize, RequiredSize);

        Fallback::DevNodeTempPropertyBufffer TempBuffer = {};
        ULONG _cbTempBuffer = sizeof(TempBuffer);
        if (!SetupDiGetDeviceRegistryPropertyW(DeviceInfoSet, DeviceInfoData, _pProperty->SP_DeviceRegistryProperty, NULL, PBYTE(&TempBuffer), _cbTempBuffer, &_cbTempBuffer))
            return FALSE;

        const auto _bRet = TempBuffer.TryTransformGetBuffer(_pProperty->PropertyType, PropertyBuffer, &PropertyBufferSize);
        if (RequiredSize)
            *RequiredSize = PropertyBufferSize;

        if(!_bRet)
            SetLastError(ERROR_INSUFFICIENT_BUFFER);

        return _bRet;
    }
#endif //!YY_Thunks_Target < __WindowsNT6


#if (YY_Thunks_Target < __WindowsNT6)

    // Available in Windows Vista and later versions of Windows.
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
        _In_reads_bytes_opt_(PropertyBufferSize) PBYTE PropertyBuffer,
        _In_         DWORD            PropertyBufferSize,
        _In_         DWORD            Flags
        )
    {
        if (const auto _pfnSetupDiSetDevicePropertyW = try_get_SetupDiSetDevicePropertyW())
        {
            return _pfnSetupDiSetDevicePropertyW(DeviceInfoSet, DeviceInfoData, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, Flags);
        }

        if (!DeviceInfoSet)
        {
            SetLastError(ERROR_INVALID_HANDLE);
            return FALSE;
        }

        if (DeviceInfoData == nullptr || PropertyKey == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if (Flags != 0)
        {
            SetLastError(ERROR_INVALID_FLAGS);
            return FALSE;
        }

        if (PropertyBuffer == nullptr && PropertyBufferSize)
        {
            SetLastError(ERROR_INVALID_USER_BUFFER);
            return FALSE;
        }

        const auto _pProperty = Fallback::DevicePropertyToDeviceRegistryProperty(PropertyKey);
        if (!_pProperty)
        {
            SetLastError(ERROR_INVALID_REG_PROPERTY);
            return FALSE;
        }

        if(_pProperty->PropertyType != PropertyType)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        Fallback::DevNodeTempPropertyBufffer TempBuffer;
        if (!TempBuffer.TryTransformSetBuffer(PropertyType, &PropertyBuffer, &PropertyBufferSize))
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        return SetupDiSetDeviceRegistryPropertyW(DeviceInfoSet, DeviceInfoData, PropertyType, PropertyBuffer, PropertyBufferSize);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Available in Windows Vista and later versions of Windows.
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
        if(const auto _pfnSetupDiGetClassPropertyW = try_get_SetupDiGetClassPropertyW())
        {
            return _pfnSetupDiGetClassPropertyW(ClassGuid, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, RequiredSize, Flags);
        }

        if (ClassGuid == nullptr || PropertyType == nullptr || PropertyKey == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if (PropertyBuffer == nullptr && (PropertyBufferSize || RequiredSize == nullptr))
        {
            SetLastError(ERROR_INVALID_USER_BUFFER);
            return FALSE;
        }

        const auto _pProperty = Fallback::ClassPropertyToClassRegistryProperty(PropertyKey);
        if (!_pProperty)
        {
            SetLastError(ERROR_INVALID_REG_PROPERTY);
            return FALSE;
        }

        *PropertyType = _pProperty->PropertyType;
        if (!Fallback::DevNodeTempPropertyBufffer::HasTransform(_pProperty->PropertyType))
            return SetupDiGetClassRegistryPropertyW(ClassGuid, _pProperty->SP_ClassRegistryProperty, nullptr, PropertyBuffer, PropertyBufferSize, RequiredSize, nullptr, nullptr);

        Fallback::DevNodeTempPropertyBufffer TempBuffer = {};
        ULONG _cbTempBuffer = sizeof(TempBuffer);
        if (!SetupDiGetClassRegistryPropertyW(ClassGuid, _pProperty->SP_ClassRegistryProperty, nullptr, PBYTE(&TempBuffer), _cbTempBuffer, &_cbTempBuffer, nullptr, nullptr))
            return FALSE;

        const auto _bRet = TempBuffer.TryTransformGetBuffer(_pProperty->PropertyType, PropertyBuffer, &PropertyBufferSize);
        if (RequiredSize)
            *RequiredSize = PropertyBufferSize;

        if (!_bRet)
            SetLastError(ERROR_INSUFFICIENT_BUFFER);

        return _bRet;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

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
        if (const auto _pfnSetupDiGetClassPropertyExW = try_get_SetupDiGetClassPropertyExW())
        {
            return _pfnSetupDiGetClassPropertyExW(ClassGuid, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, RequiredSize, Flags, MachineName, Reserved);
        }

        if (ClassGuid == nullptr || PropertyType == nullptr || PropertyKey == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if (PropertyBuffer == nullptr && (PropertyBufferSize || RequiredSize == nullptr))
        {
            SetLastError(ERROR_INVALID_USER_BUFFER);
            return FALSE;
        }

        const auto _pProperty = Fallback::ClassPropertyToClassRegistryProperty(PropertyKey);
        if (!_pProperty)
        {
            SetLastError(ERROR_INVALID_REG_PROPERTY);
            return FALSE;
        }

        *PropertyType = _pProperty->PropertyType;
        if(!Fallback::DevNodeTempPropertyBufffer::HasTransform(_pProperty->PropertyType))
            return SetupDiGetClassRegistryPropertyW(ClassGuid, _pProperty->SP_ClassRegistryProperty, nullptr, PropertyBuffer, PropertyBufferSize, RequiredSize, MachineName, Reserved);

        Fallback::DevNodeTempPropertyBufffer TempBuffer = {};
        ULONG _cbTempBuffer = sizeof(TempBuffer);
        if (!SetupDiGetClassRegistryPropertyW(ClassGuid, _pProperty->SP_ClassRegistryProperty, nullptr, PBYTE(&TempBuffer), _cbTempBuffer, &_cbTempBuffer, MachineName, Reserved))
            return FALSE;

        const auto _bRet = TempBuffer.TryTransformGetBuffer(_pProperty->PropertyType, PropertyBuffer, &PropertyBufferSize);
        if (RequiredSize)
            *RequiredSize = PropertyBufferSize;

        if (!_bRet)
            SetLastError(ERROR_INSUFFICIENT_BUFFER);

        return _bRet;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

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
        _In_reads_bytes_opt_(PropertyBufferSize) PBYTE PropertyBuffer,
        _In_         DWORD            PropertyBufferSize,
        _In_         DWORD            Flags
        )
    {
        if (const auto _pfnSetupDiSetClassPropertyW = try_get_SetupDiSetClassPropertyW())
        {
            return _pfnSetupDiSetClassPropertyW(ClassGuid, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, Flags);
        }

        if (ClassGuid == nullptr || PropertyKey == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if (PropertyBuffer == nullptr && PropertyBufferSize)
        {
            SetLastError(ERROR_INVALID_USER_BUFFER);
            return FALSE;
        }

        const auto _pProperty = Fallback::ClassPropertyToClassRegistryProperty(PropertyKey);
        if (!_pProperty)
        {
            SetLastError(ERROR_INVALID_REG_PROPERTY);
            return FALSE;
        }

        if (_pProperty->PropertyType != PropertyType)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        Fallback::DevNodeTempPropertyBufffer TempBuffer;
        if (!TempBuffer.TryTransformSetBuffer(PropertyType, &PropertyBuffer, &PropertyBufferSize))
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        return SetupDiSetClassRegistryPropertyW(ClassGuid, PropertyType, PropertyBuffer, PropertyBufferSize, nullptr, nullptr);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

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
        _In_reads_bytes_opt_(PropertyBufferSize) PBYTE PropertyBuffer,
        _In_         DWORD            PropertyBufferSize,
        _In_         DWORD            Flags,
        _In_opt_     PCWSTR           MachineName,
        _Reserved_   PVOID            Reserved
        )
    {
        if (const auto _pfnSetupDiSetClassPropertyExW = try_get_SetupDiSetClassPropertyExW())
        {
            return _pfnSetupDiSetClassPropertyExW(ClassGuid, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, Flags, MachineName, Reserved);
        }

        if (ClassGuid == nullptr || PropertyKey == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if (PropertyBuffer == nullptr && PropertyBufferSize)
        {
            SetLastError(ERROR_INVALID_USER_BUFFER);
            return FALSE;
        }

        const auto _pProperty = Fallback::ClassPropertyToClassRegistryProperty(PropertyKey);
        if (!_pProperty)
        {
            SetLastError(ERROR_INVALID_REG_PROPERTY);
            return FALSE;
        }

        if (_pProperty->PropertyType != PropertyType)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        Fallback::DevNodeTempPropertyBufffer TempBuffer;
        if (!TempBuffer.TryTransformSetBuffer(PropertyType, &PropertyBuffer, &PropertyBufferSize))
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        return SetupDiSetClassRegistryPropertyW(ClassGuid, PropertyType, PropertyBuffer, PropertyBufferSize, MachineName, Reserved);
    }
#endif
}
