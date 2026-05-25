#pragma once

#if (YY_Thunks_Target < __WindowsNT10_10240)
#include <roapi.h>
#include <activation.h>
#include <inspectable.h>

#include <windows.ui.viewmanagement.h>
#include <UIViewSettingsInterop.h>

namespace YY::Thunks::Fallback {

namespace {
class CUIViewSettings : public ABI::Windows::UI::ViewManagement::IUIViewSettings
{
    static constexpr const GUID s_Iids[] = { __uuidof(IUnknown), __uuidof(IAgileObject), __uuidof(IInspectable), __uuidof(IUIViewSettings) };

public:
    ////////////////////////////////////////////////////////
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(
        _In_ REFIID riid,
        _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
    {
        if (!ppvObject)
            return E_POINTER;

        *ppvObject = nullptr;
        for (auto _Id : s_Iids)
        {
            if (IsEqualGUID(riid, _Id))
            {
                AddRef();
                *ppvObject = this;
                return S_OK;
            }
        }

        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef(void) override
    {
        return 1;
    }

    ULONG STDMETHODCALLTYPE Release(void) override
    {
        return 1;
    }

    /////////////////////////////////////////////////////////
    // IInspectable
    HRESULT STDMETHODCALLTYPE GetIids(
        _Out_ ULONG* iidCount,
        _Out_ IID** iids) override
    {
        if (!iidCount || !iids)
            return E_POINTER;

        *iids = static_cast<IID*>(CoTaskMemAlloc(sizeof(s_Iids)));
        if (!*iids)
            return E_OUTOFMEMORY;

        memcpy(*iids, s_Iids, sizeof(s_Iids));
        *iidCount = ARRAYSIZE(s_Iids);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetRuntimeClassName(
        _Out_ HSTRING* className) override
    {
        if (!className)
            return E_POINTER;

        return WindowsCreateString(L"Windows.UI.ViewManagement.UIViewSettings", 40, className);
    }

    HRESULT STDMETHODCALLTYPE GetTrustLevel(
        _Out_ TrustLevel* trustLevel) override
    {
        if (!trustLevel)
            return E_POINTER;
        *trustLevel = BaseTrust;
        return S_OK;
    }

    /////////////////////////////////////////////////////////
    // IUIViewSettings
    HRESULT STDMETHODCALLTYPE get_UserInteractionMode(
        _Out_ ABI::Windows::UI::ViewManagement::UserInteractionMode* InteractionMode) override
    {
        if (!InteractionMode)
            return E_POINTER;

        *InteractionMode = ABI::Windows::UI::ViewManagement::UserInteractionMode_Mouse;
        return S_OK;
    }
};

class CUIViewSettingsInterop : public IUIViewSettingsInterop
{
    static constexpr const GUID s_Iids[] = { __uuidof(IUnknown), __uuidof(IAgileObject), __uuidof(IInspectable), __uuidof(IUIViewSettingsInterop) };

public:
    ////////////////////////////////////////////////////////
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(
        _In_ REFIID riid,
        _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
    {
        if (!ppvObject)
            return E_POINTER;

        *ppvObject = nullptr;
        for(auto _Id : s_Iids)
        {
            if (IsEqualGUID(riid, _Id))
            {
                AddRef();
                *ppvObject = this;
                return S_OK;
            }
        }

        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef(void) override
    {
        return 1;
    }

    ULONG STDMETHODCALLTYPE Release(void) override
    {
        return 1;
    }

    /////////////////////////////////////////////////////////
    // IInspectable
    HRESULT STDMETHODCALLTYPE GetIids(
        _Out_ ULONG* iidCount,
        _Out_ IID** iids) override
    {
        if (!iidCount || !iids)
            return E_POINTER;

        *iids = static_cast<IID*>(CoTaskMemAlloc(sizeof(s_Iids)));
        if (!*iids)
            return E_OUTOFMEMORY;

        memcpy(*iids, s_Iids, sizeof(s_Iids));
        *iidCount = ARRAYSIZE(s_Iids);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetRuntimeClassName(
        _Out_ HSTRING* className) override
    {
        if (!className)
            return E_POINTER;

        return WindowsCreateString(L"Windows.UI.ViewManagement.UIViewSettings", 40, className);
    }

    HRESULT STDMETHODCALLTYPE GetTrustLevel(
        _Out_ TrustLevel* trustLevel) override
    {
        if (!trustLevel)
            return E_POINTER;

        *trustLevel = BaseTrust;
        return S_OK;
    }

    /////////////////////////////////////////////////////////
    // IUIViewSettingsInterop
    HRESULT STDMETHODCALLTYPE GetForWindow(
        _In_ HWND appWindow,
        _In_ REFIID riid,
        _Out_ void** ppv) override
    {
        if (!ppv)
            return E_POINTER;
        *ppv = NULL;

        static CUIViewSettings s_UIViewSettings;

        return s_UIViewSettings.QueryInterface(riid, ppv);
    }
};


_Check_return_
static HRESULT __fastcall ResolveUIViewSettingsActivationFactory(_In_ REFIID _iid, _COM_Outptr_ void** _ppFactory) noexcept
{
    if (!_ppFactory)
        return E_POINTER;

    *_ppFactory = nullptr;

    static CUIViewSettingsInterop s_UIViewSettingsInterop;

    return s_UIViewSettingsInterop.QueryInterface(_iid, _ppFactory);
}

} // namespace

__DEFINE_RoGetActivationFactoryMapEntry(Windows.UI.ViewManagement.UIViewSettings, ResolveUIViewSettingsActivationFactory);

} // namespace YY::Thunks::Fallback
#endif // (YY_Thunks_Target < __WindowsNT10_10240)
