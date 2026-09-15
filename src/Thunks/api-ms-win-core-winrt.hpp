#if (YY_Thunks_Target < __WindowsNT10_10240)
#include <roapi.h>
#include <activation.h>
#include <inspectable.h>
#endif

#if (YY_Thunks_Target < __WindowsNT10_10240) && !defined(__Comment_Lib_ole32)
#define __Comment_Lib_ole32
#pragma comment(lib, "Ole32.lib")
#endif

#if defined(YY_Thunks_Implemented)
namespace YY::Thunks::Fallback
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT10_10240)
        struct RoGetActivationFactoryMapEntry
        {
            _In_z_ PCWSTR RuntimeClassId;
            _In_ HRESULT(__fastcall* Resolver)(_In_ REFIID iid, _COM_Outptr_ void** factory);
        };

        #pragma section(".RoGetActivationFactory$AAA", read)
        #pragma section(".RoGetActivationFactory$AAB", read)
        #pragma section(".RoGetActivationFactory$AAC", read)
        #pragma comment(linker, "/merge:.RoGetActivationFactory=.rdata")

        __declspec(allocate(".RoGetActivationFactory$AAA")) static const RoGetActivationFactoryMapEntry g_RoGetActivationFactoryMapStart[] =
        {
            { nullptr, nullptr }
        };

#if defined(_M_IX86)
#define __DEFINE_RoGetActivationFactoryMapEntry(_CLASS_ID, _FUNCTION)                                                                                                   \
    __pragma(warning(suppress:4483))                                                                                                                                    \
    extern "C" __declspec(allocate(".RoGetActivationFactory$AAB")) const YY::Thunks::Fallback::RoGetActivationFactoryMapEntry __identifier(_CRT_STRINGIZE_(RoGetActivationFactory ## @ ## _CLASS_ID)) = {  _CRT_WIDE_(# _CLASS_ID), &_FUNCTION }
#else
#define __DEFINE_RoGetActivationFactoryMapEntry(_CLASS_ID, _FUNCTION)                                                                                                   \
    __pragma(warning(suppress:4483))                                                                                                                                    \
    extern "C" __declspec(allocate(".RoGetActivationFactory$AAB")) const YY::Thunks::Fallback::RoGetActivationFactoryMapEntry __identifier(_CRT_STRINGIZE_(_RoGetActivationFactory ## @ ## _CLASS_ID)) = {  _CRT_WIDE_(# _CLASS_ID), &_FUNCTION }

#endif

        __declspec(allocate(".RoGetActivationFactory$AAC")) static const RoGetActivationFactoryMapEntry g_RoGetActivationFactoryMapEnd[] =
        {
            { nullptr, nullptr }
        };
#endif
    }
}

#include "WinRT/Windows.UI.ViewManagement.UIViewSettings.h"

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

#if (YY_Thunks_Target < __WindowsNT10_10240)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    12,
    HRESULT,
    WINAPI,
    RoGetActivationFactory,
        _In_ HSTRING _hActivatableClassId,
        _In_ REFIID _iid,
        _COM_Outptr_ void** _ppFactory
        )
    {
        if (auto const _pfnRoGetActivationFactory = try_get_RoGetActivationFactory())
        {
            auto _hr = _pfnRoGetActivationFactory(_hActivatableClassId, _iid, _ppFactory);

            if (_hr != CLASS_E_CLASSNOTAVAILABLE && _hr != REGDB_E_CLASSNOTREG)
            {
                return _hr;
            }

            // 支持RoGetActivationFactory不代表就一定支持所有的ClassId，所以我们需要Fallback到我们的对象。
        }

        if (!_ppFactory)
            return E_POINTER;

        *_ppFactory = nullptr;

        if (!_hActivatableClassId)
            return E_INVALIDARG;

        UINT32 _cchActivatableClassId = 0;
        auto _szActivatableClassId = WindowsGetStringRawBuffer(_hActivatableClassId, &_cchActivatableClassId);

        for (auto _pEntry = YY::Thunks::Fallback::g_RoGetActivationFactoryMapStart + 1; _pEntry < YY::Thunks::Fallback::g_RoGetActivationFactoryMapEnd; ++_pEntry)
        {
            if (!_pEntry->RuntimeClassId || !_pEntry->Resolver)
                continue;

            const auto _cchRuntimeClassId = internal::StringLength(_pEntry->RuntimeClassId);
            if (_cchRuntimeClassId != _cchActivatableClassId)
            {
                continue;
            }

            if (CompareStringOrdinal(_szActivatableClassId, _cchActivatableClassId, _pEntry->RuntimeClassId, _cchRuntimeClassId, FALSE) != CSTR_EQUAL)
            {
                continue;
            }

            return _pEntry->Resolver(_iid, _ppFactory);
        }

        return REGDB_E_CLASSNOTREG;
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
