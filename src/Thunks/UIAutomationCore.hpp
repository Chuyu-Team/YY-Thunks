#if (YY_Thunks_Target < __WindowsNT10_16299)
#include <uiautomation.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]（需要安装 .NET）
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    uiautomationcore,
    0,
    BOOL,
    WINAPI,
    UiaClientsAreListening
        )
    {
        if (const auto _pfnUiaClientsAreListening = try_get_UiaClientsAreListening())
        {
            return _pfnUiaClientsAreListening();
        }
        
        // 系统都不支持Uia，肯定不会客户端在监听。
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]（需要安装 .NET）
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    uiautomationcore,
    8,
    HRESULT,
    WINAPI,
    UiaHostProviderFromHwnd,
        HWND hwnd,
        IRawElementProviderSimple ** ppProvider
        )
    {
        if (const auto _pfnUiaHostProviderFromHwnd = try_get_UiaHostProviderFromHwnd())
        {
            return _pfnUiaHostProviderFromHwnd(hwnd, ppProvider);
        }
        
        if (!ppProvider)
            *ppProvider = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]（需要安装 .NET）
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    uiautomationcore,
    8,
    HRESULT,
    WINAPI,
    UiaRaiseAutomationEvent,
        IRawElementProviderSimple * pProvider,
        EVENTID id
        )
    {
        if (const auto _pfnUiaRaiseAutomationEvent = try_get_UiaRaiseAutomationEvent())
        {
            return _pfnUiaRaiseAutomationEvent(pProvider, id);
        }
        
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]（需要安装 .NET）
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    uiautomationcore,
    40,
    HRESULT,
    WINAPI,
    UiaRaiseAutomationPropertyChangedEvent,
        IRawElementProviderSimple * pProvider,
        PROPERTYID id,
        VARIANT oldValue,
        VARIANT newValue
        )
    {
        if (const auto _pfnUiaRaiseAutomationPropertyChangedEvent = try_get_UiaRaiseAutomationPropertyChangedEvent())
        {
            return _pfnUiaRaiseAutomationPropertyChangedEvent(pProvider, id, oldValue, newValue);
        }

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]（需要安装 .NET）
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    uiautomationcore,
    16,
    HRESULT,
    WINAPI,
    UiaReturnRawElementProvider,
        HWND hwnd,
        WPARAM wParam,
        LPARAM lParam,
        IRawElementProviderSimple * el
        )
    {
        if (const auto _pfnUiaReturnRawElementProvider = try_get_UiaReturnRawElementProvider())
        {
            return _pfnUiaReturnRawElementProvider(hwnd, wParam, lParam, el);
        }
        
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]（需要安装 .NET）
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    uiautomationcore,
    4,
    HRESULT,
    WINAPI,
    UiaGetReservedMixedAttributeValue,
        _Outptr_ IUnknown** punkMixedAttributeValue
        )
    {
        if (auto const _pfnUiaGetReservedMixedAttributeValue = try_get_UiaGetReservedMixedAttributeValue())
        {
            return _pfnUiaGetReservedMixedAttributeValue(punkMixedAttributeValue);
        }

        if (punkMixedAttributeValue)
            *punkMixedAttributeValue = nullptr;

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]（需要安装 .NET）
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    uiautomationcore,
    4,
    HRESULT,
    WINAPI,
    UiaGetReservedNotSupportedValue,
        _Outptr_ IUnknown** punkNotSupportedValue
        )
    {
        if (auto const _pfnUiaGetReservedNotSupportedValue = try_get_UiaGetReservedNotSupportedValue())
        {
            return _pfnUiaGetReservedNotSupportedValue(punkNotSupportedValue);
        }
        if (punkNotSupportedValue)
            *punkNotSupportedValue = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]（需要安装 .NET）
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    uiautomationcore,
    16,
    HRESULT,
    WINAPI,
    UiaRaiseStructureChangedEvent,
        IRawElementProviderSimple* pProvider, 
        enum StructureChangeType structureChangeType, 
        int* pRuntimeId, 
        int cRuntimeIdLen
        )
    {
        if (auto const _pfnUiaRaiseStructureChangedEvent = try_get_UiaRaiseStructureChangedEvent())
        {
            return _pfnUiaRaiseStructureChangedEvent(pProvider, structureChangeType, pRuntimeId, cRuntimeIdLen);
        }

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_16299)

    // 最低受支持的客户端	Windows 10版本 1709 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2016[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    uiautomationcore,
    20,
    HRESULT,
    WINAPI,
    UiaRaiseNotificationEvent,
        _In_ IRawElementProviderSimple* _pProvider,
        enum NotificationKind _eNotificationKind,
        enum NotificationProcessing _eNotificationProcessing,
        _In_opt_ BSTR _szDisplayString,
        _In_ BSTR _szActivityId
        )
    {
        if (auto const _pfnUiaRaiseNotificationEvent = try_get_UiaRaiseNotificationEvent())
        {
            return _pfnUiaRaiseNotificationEvent(_pProvider, _eNotificationKind, _eNotificationProcessing, _szDisplayString, _szActivityId);
        }
        
        return S_OK;
    }
#endif
}
