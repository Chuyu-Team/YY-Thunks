#if (YY_Thunks_Target < __WindowsNT6)
#include <winusb.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持XP，但是XP需要打补丁，Vista开始直接内置
    __DEFINE_THUNK(
    winusb,
    28,
    BOOL,
    __stdcall,
    WinUsb_ControlTransfer,
        _In_  WINUSB_INTERFACE_HANDLE InterfaceHandle,
        _In_  WINUSB_SETUP_PACKET SetupPacket,
        _Out_writes_bytes_to_opt_(BufferLength, *LengthTransferred) PUCHAR Buffer,
        _In_  ULONG BufferLength,
        _Out_opt_ PULONG LengthTransferred,
        _In_opt_  LPOVERLAPPED Overlapped
        )
    {
        if (const auto _pfnWinUsb_ControlTransfer = try_get_WinUsb_ControlTransfer())
        {
            return _pfnWinUsb_ControlTransfer(InterfaceHandle, SetupPacket, Buffer, BufferLength, LengthTransferred, Overlapped);
        }

        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持XP，但是XP需要打补丁，Vista开始直接内置
    __DEFINE_THUNK(
    winusb,
    4,
    BOOL,
    __stdcall,
    WinUsb_Free,
        _In_  WINUSB_INTERFACE_HANDLE InterfaceHandle
        )
    {
        if (const auto _pfnWinUsb_Free = try_get_WinUsb_Free())
        {
            return _pfnWinUsb_Free(InterfaceHandle);
        }

        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持XP，但是XP需要打补丁，Vista开始直接内置
    __DEFINE_THUNK(
    winusb,
    12,
    BOOL,
    __stdcall,
    WinUsb_GetAssociatedInterface,
        _In_  WINUSB_INTERFACE_HANDLE InterfaceHandle,
        _In_  UCHAR AssociatedInterfaceIndex,
        _Out_ PWINUSB_INTERFACE_HANDLE AssociatedInterfaceHandle
        )
    {
        if (const auto _pfnWinUsb_GetAssociatedInterface = try_get_WinUsb_GetAssociatedInterface())
        {
            return _pfnWinUsb_GetAssociatedInterface(InterfaceHandle, AssociatedInterfaceIndex, AssociatedInterfaceHandle);
        }
        
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持XP，但是XP需要打补丁，Vista开始直接内置
    __DEFINE_THUNK(
    winusb,
    16,
    BOOL,
    __stdcall,
    WinUsb_GetOverlappedResult,
        _In_  WINUSB_INTERFACE_HANDLE InterfaceHandle,
        _In_  LPOVERLAPPED lpOverlapped,
        _Out_ LPDWORD lpNumberOfBytesTransferred,
        _In_  BOOL bWait
        )
    {
        if (const auto _pfnWinUsb_GetOverlappedResult = try_get_WinUsb_GetOverlappedResult())
        {
            return _pfnWinUsb_GetOverlappedResult(InterfaceHandle, lpOverlapped, lpNumberOfBytesTransferred, bWait);
        }
        
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持XP，但是XP需要打补丁，Vista开始直接内置
    __DEFINE_THUNK(
    winusb,
    8,
    BOOL,
    __stdcall,
    WinUsb_Initialize,
        _In_  HANDLE DeviceHandle,
        _Out_ PWINUSB_INTERFACE_HANDLE InterfaceHandle
        )
    {
        if (const auto _pfnWinUsb_Initialize = try_get_WinUsb_Initialize())
        {
            return _pfnWinUsb_Initialize(DeviceHandle, InterfaceHandle);
        }

        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持XP，但是XP需要打补丁，Vista开始直接内置
    __DEFINE_THUNK(
    winusb,
    24,
    BOOL,
    __stdcall,
    WinUsb_ReadPipe,
        _In_  WINUSB_INTERFACE_HANDLE InterfaceHandle,
        _In_  UCHAR PipeID,
        _Out_writes_bytes_to_opt_(BufferLength,*LengthTransferred) PUCHAR Buffer,
        _In_  ULONG BufferLength,
        _Out_opt_ PULONG LengthTransferred,
        _In_opt_ LPOVERLAPPED Overlapped
        )
    {
        if (const auto _pfnWinUsb_ReadPipe = try_get_WinUsb_ReadPipe())
        {
            return _pfnWinUsb_ReadPipe(InterfaceHandle, PipeID, Buffer, BufferLength, LengthTransferred, Overlapped);
        }
        
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持XP，但是XP需要打补丁，Vista开始直接内置
    __DEFINE_THUNK(
    winusb,
    8,
    BOOL,
    __stdcall,
    WinUsb_ResetPipe,
        _In_  WINUSB_INTERFACE_HANDLE InterfaceHandle,
        _In_  UCHAR PipeID
        )
    {
        if (const auto _pfnWinUsb_ResetPipe = try_get_WinUsb_ResetPipe())
        {
            return _pfnWinUsb_ResetPipe(InterfaceHandle, PipeID);
        }

        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持XP，但是XP需要打补丁，Vista开始直接内置
    __DEFINE_THUNK(
    winusb,
    8,
    BOOL,
    __stdcall,
    WinUsb_SetCurrentAlternateSetting,
        _In_  WINUSB_INTERFACE_HANDLE InterfaceHandle,
        _In_  UCHAR SettingNumber
        )
    {
        if (const auto _pfnWinUsb_SetCurrentAlternateSetting = try_get_WinUsb_SetCurrentAlternateSetting())
        {
            return _pfnWinUsb_SetCurrentAlternateSetting(InterfaceHandle, SettingNumber);
        }

        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持XP，但是XP需要打补丁，Vista开始直接内置
    __DEFINE_THUNK(
    winusb,
    24,
    BOOL,
    __stdcall,
    WinUsb_WritePipe,
        _In_  WINUSB_INTERFACE_HANDLE InterfaceHandle,
        _In_  UCHAR PipeID,
        _In_reads_bytes_(BufferLength) PUCHAR Buffer,
        _In_  ULONG BufferLength,
        _Out_opt_ PULONG LengthTransferred,
        _In_opt_ LPOVERLAPPED Overlapped
        )
    {
        if (const auto _pfnWinUsb_WritePipe = try_get_WinUsb_WritePipe())
        {
            return _pfnWinUsb_WritePipe(InterfaceHandle, PipeID, Buffer, BufferLength, LengthTransferred, Overlapped);
        }

        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif
}
