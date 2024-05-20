#include <winuser.h>

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // 最低受支持的客户端	在 Windows Vista 和更高版本的 Windows 操作系统中可用。
    __DEFINE_THUNK(
    user32,
    4,
    LONG,
    WINAPI,
    DisplayConfigGetDeviceInfo,
        _Inout_ DISPLAYCONFIG_DEVICE_INFO_HEADER* requestPacket
        )
    {
        if (const auto _pfnDisplayConfigGetDeviceInfo = try_get_DisplayConfigGetDeviceInfo())
        {
            return _pfnDisplayConfigGetDeviceInfo(requestPacket);
        }
        
        return ERROR_NOT_SUPPORTED;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // 最低受支持的客户端	在 Windows Vista 和更高版本的 Windows 操作系统中可用。
    __DEFINE_THUNK(
    user32,
    12,
    LONG,
    WINAPI,
    GetDisplayConfigBufferSizes,
        _In_ UINT32 flags,
        _Out_ UINT32* numPathArrayElements,
        _Out_ UINT32* numModeInfoArrayElements
        )
    {
        if (const auto _pfnGetDisplayConfigBufferSizes = try_get_GetDisplayConfigBufferSizes())
        {
            return _pfnGetDisplayConfigBufferSizes(flags, numPathArrayElements, numModeInfoArrayElements);
        }
        
        return ERROR_NOT_SUPPORTED;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // 最低受支持的客户端	在 Windows Vista 和更高版本的 Windows 操作系统中可用。
    __DEFINE_THUNK(
    user32,
    24,
    LONG,
    WINAPI,
    QueryDisplayConfig,
        _In_ UINT32 flags,
        _Inout_ UINT32* numPathArrayElements,
        _Out_writes_to_(*numPathArrayElements, *numPathArrayElements) DISPLAYCONFIG_PATH_INFO* pathArray,
        _Inout_ UINT32* numModeInfoArrayElements,
        _Out_writes_to_(*numModeInfoArrayElements, *numModeInfoArrayElements) DISPLAYCONFIG_MODE_INFO* modeInfoArray,
        _When_(!(flags & QDC_DATABASE_CURRENT), _Pre_null_)
        _When_(flags & QDC_DATABASE_CURRENT, _Out_)
            DISPLAYCONFIG_TOPOLOGY_ID* currentTopologyId
        )
    {
        if (const auto _pfnQueryDisplayConfig = try_get_QueryDisplayConfig())
        {
            return _pfnQueryDisplayConfig(flags, numPathArrayElements, pathArray, numModeInfoArrayElements, modeInfoArray, currentTopologyId);
        }
        
        return ERROR_NOT_SUPPORTED;
    }
#endif
}
