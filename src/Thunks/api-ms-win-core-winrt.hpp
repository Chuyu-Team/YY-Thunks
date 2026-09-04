#if (YY_Thunks_Target < __WindowsNT6_2)
#include <roapi.h>
#include <activation.h>
#include <inspectable.h>
#endif

#if (YY_Thunks_Target < __WindowsNT10_10240)
#include <windows.ui.viewmanagement.h>
#include <UIViewSettingsInterop.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_2) && !defined(__Comment_Lib_ole32)
#define __Comment_Lib_ole32
#pragma comment(lib, "Ole32.lib")
#endif

#if defined(YY_Thunks_Implemented)
namespace YY::Thunks::Fallback
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT10_10240)
        class CUIViewSettings : public ABI::Windows::UI::ViewManagement::IUIViewSettings
        {
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
                if (IsEqualGUID(riid, __uuidof(IUnknown))
                    || IsEqualGUID(riid, __uuidof(IAgileObject))
                    || IsEqualGUID(riid, __uuidof(IInspectable))
                    || IsEqualGUID(riid, __uuidof(IUIViewSettings)))
                {
                    AddRef();
                    *ppvObject = this;
                    return S_OK;
                }
                else
                {
                    return E_NOINTERFACE;
                }
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

                *iids = static_cast<IID*>(CoTaskMemAlloc(sizeof(IID)));

                if (!*iids) 
                    return E_OUTOFMEMORY;

                (*iids)[0] = __uuidof(IUIViewSettings);
                *iidCount = 1;

                return S_OK;
            }

            HRESULT STDMETHODCALLTYPE GetRuntimeClassName(
                _Out_ HSTRING* className) override 
            {
                if (!className)
                    return E_POINTER;
                return WindowsCreateString(L"Windows.UI.ViewManagement.UIViewSettings", 36, className);
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

        static CUIViewSettings g_UIViewSettings;

        class CUIViewSettingsInterop : public IUIViewSettingsInterop
        {
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
                if (IsEqualGUID(riid, __uuidof(IUnknown))
                    || IsEqualGUID(riid, __uuidof(IAgileObject))
                    || IsEqualGUID(riid, __uuidof(IInspectable))
                    || IsEqualGUID(riid, __uuidof(IUIViewSettingsInterop)))
                {
                    AddRef();
                    *ppvObject = this;
                    return S_OK;
                }
                else
                {
                    return E_NOINTERFACE;
                }
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

                *iids = static_cast<IID*>(CoTaskMemAlloc(sizeof(IID)));

                if (!*iids)
                    return E_OUTOFMEMORY;

                (*iids)[0] = __uuidof(IUIViewSettingsInterop);
                *iidCount = 1;

                return S_OK;
            }

            HRESULT STDMETHODCALLTYPE GetRuntimeClassName(
                _Out_ HSTRING* className) override
            {
                if (!className)
                    return E_POINTER;
                return WindowsCreateString(L"Windows.UI.ViewManagement.UIViewSettings", 36, className);
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

                if (!IsEqualIID(riid, __uuidof(ABI::Windows::UI::ViewManagement::IUIViewSettings)))
                { 
                    return E_NOINTERFACE; 
                }

                *ppv = &g_UIViewSettings;
                return S_OK;
            }
        };

        static CUIViewSettingsInterop g_UIViewSettingsInterop;
#endif // (YY_Thunks_Target < __WindowsNT10_10240)
    }
}
#endif // (YY_Thunks_Implemented)

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    4,
    HRESULT,
    WINAPI,
    RoInitialize,
        _In_ RO_INIT_TYPE initType
        )
    {
        if (auto const pRoInitialize = try_get_RoInitialize())
        {
            return pRoInitialize(initType);
        }

        if ((unsigned)initType > (unsigned)RO_INIT_MULTITHREADED)
        {
            return E_INVALIDARG;
        }

        return CoInitializeEx(nullptr, initType ? COINIT_MULTITHREADED : COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    0,
    void,
    WINAPI,
    RoUninitialize,
        )
    {
        if (auto const pRoUninitialize = try_get_RoUninitialize())
        {
            return pRoUninitialize();
        }

        CoUninitialize();
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    8,
    HRESULT,
    WINAPI,
    RoActivateInstance,
        _In_ HSTRING activatableClassId,
        _COM_Outptr_ IInspectable** instance
        )
    {
        if (auto const pRoActivateInstance = try_get_RoActivateInstance())
        {
            return pRoActivateInstance(activatableClassId, instance);
        }

        if (instance)
            *instance = nullptr;

        return E_NOTIMPL;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    16,
    HRESULT,
    WINAPI,
    RoRegisterActivationFactories,
        _In_reads_(count) HSTRING* activatableClassIds,
        _In_reads_(count) PFNGETACTIVATIONFACTORY* activationFactoryCallbacks,
        _In_ UINT32 count,
        _Out_ RO_REGISTRATION_COOKIE* cookie
        )
    {
        if (auto const pRoRegisterActivationFactories = try_get_RoRegisterActivationFactories())
        {
            return pRoRegisterActivationFactories(activatableClassIds, activationFactoryCallbacks, count, cookie);
        }

        if (cookie)
            *cookie = nullptr;

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    4,
    void,
    WINAPI,
    RoRevokeActivationFactories,
        _In_ RO_REGISTRATION_COOKIE cookie
        )
    {
        if (auto const pRoRevokeActivationFactories = try_get_RoRevokeActivationFactories())
        {
            return pRoRevokeActivationFactories(cookie);
        }
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    12,
    HRESULT,
    WINAPI,
    RoGetActivationFactory,
        _In_ HSTRING activatableClassId,
        _In_ REFIID iid,
        _COM_Outptr_ void** factory
        )
    {
        UNREFERENCED_PARAMETER(activatableClassId);

        if (factory)
            *factory = nullptr;

        if (IsEqualIID(iid, __uuidof(ABI::Windows::UI::ViewManagement::IUIViewSettings)))
        {
            return Fallback::g_UIViewSettings.QueryInterface(iid, factory);
        }
        else if (IsEqualIID(iid, __uuidof(IUIViewSettingsInterop)))
        {
            return Fallback::g_UIViewSettingsInterop.QueryInterface(iid, factory);
        }

        if (auto const pRoGetActivationFactory = try_get_RoGetActivationFactory())
        {
            return pRoGetActivationFactory(activatableClassId, iid, factory);
        }

        // According to the C++/WinRT fallback implementation, we should
        // return CLASS_E_CLASSNOTAVAILABLE.
        return CLASS_E_CLASSNOTAVAILABLE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    12,
    HRESULT,
    WINAPI,
    RoRegisterForApartmentShutdown,
        _In_ IApartmentShutdown* callbackObject,
        _Out_ UINT64* apartmentIdentifier,
        _Out_ APARTMENT_SHUTDOWN_REGISTRATION_COOKIE* regCookie
        )
    {
        if (auto const pRoRegisterForApartmentShutdown = try_get_RoRegisterForApartmentShutdown())
        {
            return pRoRegisterForApartmentShutdown(callbackObject, apartmentIdentifier, regCookie);
        }

        if (regCookie)
            *regCookie = nullptr;


        return E_NOTIMPL;

    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    4,
    HRESULT,
    WINAPI,
    RoUnregisterForApartmentShutdown,
        _In_ APARTMENT_SHUTDOWN_REGISTRATION_COOKIE regCookie
        )
    {
        if (auto const pRoUnregisterForApartmentShutdown = try_get_RoUnregisterForApartmentShutdown())
        {
            return pRoUnregisterForApartmentShutdown(regCookie);
        }

        return E_NOTIMPL;

    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    4,
    HRESULT,
    WINAPI,
    RoGetApartmentIdentifier,
        _Out_ UINT64* apartmentIdentifier
        )
    {
        if (auto const pRoGetApartmentIdentifier = try_get_RoGetApartmentIdentifier())
        {
            return pRoGetApartmentIdentifier(apartmentIdentifier);
        }

        return E_NOTIMPL;

    }
#endif
}
