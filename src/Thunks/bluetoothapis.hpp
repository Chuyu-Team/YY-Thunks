#if (YY_Thunks_Target < __WindowsNT6_2)
#include <bluetoothleapis.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8及更高版本的 Windows 中受支持。
    __DEFINE_THUNK(
    bluetoothapis,
    24,
    HRESULT,
    WINAPI,
    BluetoothGATTGetCharacteristicValue,
        _In_ HANDLE _hDevice,
        _In_ PBTH_LE_GATT_CHARACTERISTIC _pCharacteristic,
        _In_ ULONG _cbCharacteristicValueDataSize,
        _Out_opt_ PBTH_LE_GATT_CHARACTERISTIC_VALUE _pCharacteristicValue,
        _Out_opt_ USHORT* _pCharacteristicValueSizeRequired,
        _In_ ULONG _fFlags
        )
    {
        if (const auto _pfnBluetoothGATTGetCharacteristicValue = try_get_BluetoothGATTGetCharacteristicValue())
        {
            return _pfnBluetoothGATTGetCharacteristicValue(_hDevice, _pCharacteristic, _cbCharacteristicValueDataSize, _pCharacteristicValue, _pCharacteristicValueSizeRequired, _fFlags);
        }

        if (_pCharacteristicValueSizeRequired)
            *_pCharacteristicValueSizeRequired = 0;

        if (_pCharacteristicValue && _cbCharacteristicValueDataSize >= sizeof(DWORD))
            _pCharacteristicValue->DataSize = 0;

        return ERROR_NOT_SUPPORTED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8及更高版本的 Windows 中受支持。
    __DEFINE_THUNK(
    bluetoothapis,
    24,
    HRESULT,
    WINAPI,
    BluetoothGATTGetCharacteristics,
        _In_ HANDLE _hDevice,
        _In_opt_ PBTH_LE_GATT_SERVICE _pService,
        _In_ USHORT _uCharacteristicsBufferCount,
        _Out_writes_to_opt_(_uCharacteristicsBufferCount, *_puCharacteristicsBufferActual) PBTH_LE_GATT_CHARACTERISTIC _pCharacteristicsBuffer,
        _Out_ USHORT* _puCharacteristicsBufferActual,
        _In_ ULONG _fFlags
        )
    {
        if (const auto _pfnBluetoothGATTGetCharacteristics = try_get_BluetoothGATTGetCharacteristics())
        {
            return _pfnBluetoothGATTGetCharacteristics(_hDevice, _pService, _uCharacteristicsBufferCount, _pCharacteristicsBuffer, _puCharacteristicsBufferActual, _fFlags);
        }

        if (_puCharacteristicsBufferActual)
            *_puCharacteristicsBufferActual = 0;

        return ERROR_NOT_SUPPORTED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8及更高版本的 Windows 中受支持。
    __DEFINE_THUNK(
    bluetoothapis,
    24,
    HRESULT,
    WINAPI,
    BluetoothGATTGetDescriptors,
        _In_ HANDLE _hDevice,
        _In_ PBTH_LE_GATT_CHARACTERISTIC _pCharacteristic,
        _In_ USHORT _uDescriptorsBufferCount,
        _Out_writes_to_opt_(_uDescriptorsBufferCount, *_puDescriptorsBufferActual) PBTH_LE_GATT_DESCRIPTOR _pDescriptorsBuffer,
        _Out_ USHORT* _puDescriptorsBufferActual,
        _In_ ULONG _fFlags
        )
    {
        if (const auto _pfnBluetoothGATTGetDescriptors = try_get_BluetoothGATTGetDescriptors())
        {
            return _pfnBluetoothGATTGetDescriptors(_hDevice, _pCharacteristic, _uDescriptorsBufferCount, _pDescriptorsBuffer, _puDescriptorsBufferActual, _fFlags);
        }

        if (_puDescriptorsBufferActual)
            *_puDescriptorsBufferActual = 0;

        return ERROR_NOT_SUPPORTED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8及更高版本的 Windows 中受支持。
    __DEFINE_THUNK(
    bluetoothapis,
    20,
    HRESULT,
    WINAPI,
    BluetoothGATTGetServices,
        _In_ HANDLE _hDevice,
        _In_ USHORT _uServicesBufferCount,
        _Out_writes_to_opt_(_uServicesBufferCount, *_puServicesBufferActual) PBTH_LE_GATT_SERVICE _pServicesBuffer,
        _Out_ USHORT* _puServicesBufferActual,
        _In_ ULONG _fFlags
        )
    {
        if (const auto _pfnBluetoothGATTGetServices = try_get_BluetoothGATTGetServices())
        {
            return _pfnBluetoothGATTGetServices(_hDevice, _uServicesBufferCount, _pServicesBuffer, _puServicesBufferActual, _fFlags);
        }

        if (_puServicesBufferActual)
            *_puServicesBufferActual = 0;

        return ERROR_NOT_SUPPORTED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8及更高版本的 Windows 中受支持。
    __DEFINE_THUNK(
    bluetoothapis,
    28,
    HRESULT,
    WINAPI,
    BluetoothGATTRegisterEvent,
        _In_ HANDLE _hService,
        _In_ BTH_LE_GATT_EVENT_TYPE _eEventType,
        _In_ PVOID _pEventParameterIn,
        _In_ PFNBLUETOOTH_GATT_EVENT_CALLBACK _pfnCallback,
        _In_opt_ PVOID _pCallbackContext,
        _Out_ BLUETOOTH_GATT_EVENT_HANDLE * _pEventHandle,
        _In_ ULONG _fFlags
        )
    {
        if (const auto _pfnBluetoothGATTRegisterEvent = try_get_BluetoothGATTRegisterEvent())
        {
            return _pfnBluetoothGATTRegisterEvent(_hService, _eEventType, _pEventParameterIn, _pfnCallback, _pCallbackContext, _pEventHandle, _fFlags);
        }

        return ERROR_NOT_SUPPORTED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8及更高版本的 Windows 中受支持。
    __DEFINE_THUNK(
    bluetoothapis,
    24,
    HRESULT,
    WINAPI,
    BluetoothGATTSetCharacteristicValue,
        _In_ HANDLE _hDevice,
        _In_ PBTH_LE_GATT_CHARACTERISTIC _pCharacteristic,
        _In_ PBTH_LE_GATT_CHARACTERISTIC_VALUE _pCharacteristicValue,
        _In_opt_ BTH_LE_GATT_RELIABLE_WRITE_CONTEXT _ReliableWriteContext,
        _In_ ULONG _fFlags
        )
    {
        if (const auto _pfnBluetoothGATTSetCharacteristicValue = try_get_BluetoothGATTSetCharacteristicValue())
        {
            return _pfnBluetoothGATTSetCharacteristicValue(_hDevice, _pCharacteristic, _pCharacteristicValue, _ReliableWriteContext, _fFlags);
        }

        return ERROR_NOT_SUPPORTED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8及更高版本的 Windows 中受支持。
    __DEFINE_THUNK(
    bluetoothapis,
    16,
    HRESULT,
    WINAPI,
    BluetoothGATTSetDescriptorValue,
        _In_ HANDLE _hDevice,
        _In_ PBTH_LE_GATT_DESCRIPTOR _pDescriptor,
        _In_ PBTH_LE_GATT_DESCRIPTOR_VALUE _pDescriptorValue,
        _In_ ULONG _fFlags
        )
    {
        if (const auto _pfnBluetoothGATTSetDescriptorValue = try_get_BluetoothGATTSetDescriptorValue())
        {
            return _pfnBluetoothGATTSetDescriptorValue(_hDevice, _pDescriptor, _pDescriptorValue, _fFlags);
        }

        return ERROR_NOT_SUPPORTED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8及更高版本的 Windows 中受支持。
    __DEFINE_THUNK(
    bluetoothapis,
    8,
    HRESULT,
    WINAPI,
    BluetoothGATTUnregisterEvent,
        _In_ BLUETOOTH_GATT_EVENT_HANDLE _hEventHandle,
        _In_ ULONG _fFlags
        )
    {
        if (const auto _pfnBluetoothGATTUnregisterEvent = try_get_BluetoothGATTUnregisterEvent())
        {
            return _pfnBluetoothGATTUnregisterEvent(_hEventHandle, _fFlags);
        }

        return ERROR_NOT_SUPPORTED;
    }
#endif
} // namespace YY::Thunks
