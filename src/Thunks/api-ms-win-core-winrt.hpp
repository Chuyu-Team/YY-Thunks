#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#include <roapi.h>
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8) && !defined(__Comment_Lib_ole32)
#define __Comment_Lib_ole32
#pragma comment(lib, "Ole32.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN8)

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

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

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

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

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

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

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


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

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

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

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
		if (auto const pRoGetActivationFactory = try_get_RoGetActivationFactory())
		{
			return pRoGetActivationFactory(activatableClassId, iid, factory);
		}

		if (factory)
			*factory = nullptr;

        // According to the C++/WinRT fallback implementation, we should
        // return CLASS_E_CLASSNOTAVAILABLE.
		return CLASS_E_CLASSNOTAVAILABLE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

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


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

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

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

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
