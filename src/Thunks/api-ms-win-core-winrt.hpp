
#include <roapi.h>

namespace YY {
namespace Thunks {

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
// Windows 8 [desktop apps | UWP apps]
// Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
WINAPI
RoInitialize(_In_ RO_INIT_TYPE initType)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pRoInitialize = try_get_RoInitialize())
    {
        return pRoInitialize(initType);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_l1_1_0, RoInitialize, 4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
// Windows 8 [desktop apps | UWP apps]
// Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
void WINAPI RoUninitialize()
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pRoUninitialize = try_get_RoUninitialize())
    {
        return pRoUninitialize();
    }
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_l1_1_0, RoUninitialize, 0);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
// Windows 8 [desktop apps | UWP apps]
// Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
WINAPI
RoActivateInstance(_In_ HSTRING activatableClassId, _COM_Outptr_ IInspectable **instance)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pRoActivateInstance = try_get_RoActivateInstance())
    {
        return pRoActivateInstance(activatableClassId, instance);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_l1_1_0, RoActivateInstance, 8);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
// Windows 8 [desktop apps | UWP apps]
// Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
WINAPI
RoRegisterActivationFactories(
    _In_reads_(count) HSTRING *activatableClassIds,
    _In_reads_(count) PFNGETACTIVATIONFACTORY *activationFactoryCallbacks,
    _In_ UINT32 count,
    _Out_ RO_REGISTRATION_COOKIE *cookie)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pRoRegisterActivationFactories = try_get_RoRegisterActivationFactories())
    {
        return pRoRegisterActivationFactories(
            activatableClassIds, activationFactoryCallbacks, count, cookie);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_l1_1_0, RoRegisterActivationFactories, 16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
// Windows 8 [desktop apps | UWP apps]
// Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
void WINAPI RoRevokeActivationFactories(_In_ RO_REGISTRATION_COOKIE cookie)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pRoRevokeActivationFactories = try_get_RoRevokeActivationFactories())
    {
        return pRoRevokeActivationFactories(cookie);
    }
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_l1_1_0, RoRevokeActivationFactories, 4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
// Windows 8 [desktop apps | UWP apps]
// Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
WINAPI
RoGetActivationFactory(
    _In_ HSTRING activatableClassId,
    _In_ REFIID iid,
    _COM_Outptr_ void **factory)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pRoGetActivationFactory = try_get_RoGetActivationFactory())
    {
        return pRoGetActivationFactory(activatableClassId, iid, factory);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_l1_1_0, RoGetActivationFactory, 12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
// Windows 8 [desktop apps | UWP apps]
// Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
WINAPI
RoRegisterForApartmentShutdown(
    _In_ IApartmentShutdown *callbackObject,
    _Out_ UINT64 *apartmentIdentifier,
    _Out_ APARTMENT_SHUTDOWN_REGISTRATION_COOKIE *regCookie)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pRoRegisterForApartmentShutdown = try_get_RoRegisterForApartmentShutdown())
    {
        return pRoRegisterForApartmentShutdown(callbackObject, apartmentIdentifier, regCookie);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_l1_1_0, RoRegisterForApartmentShutdown, 12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
// Windows 8 [desktop apps | UWP apps]
// Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
WINAPI
RoUnregisterForApartmentShutdown(_In_ APARTMENT_SHUTDOWN_REGISTRATION_COOKIE regCookie)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pRoUnregisterForApartmentShutdown = try_get_RoUnregisterForApartmentShutdown())
    {
        return pRoUnregisterForApartmentShutdown(regCookie);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_l1_1_0, RoUnregisterForApartmentShutdown, 4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
// Windows 8 [desktop apps | UWP apps]
// Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
WINAPI
RoGetApartmentIdentifier(_Out_ UINT64 *apartmentIdentifier)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pRoGetApartmentIdentifier = try_get_RoGetApartmentIdentifier())
    {
        return pRoGetApartmentIdentifier(apartmentIdentifier);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_l1_1_0, RoGetApartmentIdentifier, 4);

#endif
} // namespace Thunks
} // namespace YY
