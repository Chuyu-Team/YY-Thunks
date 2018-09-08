#pragma once


#include <Windows.h>
#include <crtdbg.h>
#include <intrin.h>



#if defined(_M_IX86)
	#define _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f,prefixed) _CRT_CONCATENATE(_CRT_CONCATENATE(_imp__, f), prefixed)
#elif defined(_M_AMD64)
	#define _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f,prefixed) _CRT_CONCATENATE(__imp_, f)
#else
	 #error "不支持此体系"
#endif

#define _LCRT_DEFINE_IAT_SYMBOL(f,prefixed)                                                          \
    extern "C" __declspec(selectany) void const* const _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f,prefixed) \
        = reinterpret_cast<void const*>(f)






enum module_id : unsigned
{
#define _APPLY(_SYMBOL, _NAME) _SYMBOL,
	_YY_APPLY_TO_LATE_BOUND_MODULES(_APPLY)
#undef _APPLY

	module_id_count
};


enum function_id : unsigned
{
#define _APPLY(_FUNCTION, _MODULES) _CRT_CONCATENATE(_FUNCTION, _id),
	_YY_APPLY_TO_LATE_BOUND_FUNCTIONS(_APPLY)
#undef _APPLY

	function_id_count
};

static wchar_t const* const module_names[module_id_count] =
{
#define _APPLY(_SYMBOL, _NAME) _CRT_WIDE(_NAME),
	_YY_APPLY_TO_LATE_BOUND_MODULES(_APPLY)
#undef _APPLY
};

static HMODULE module_handles[module_id_count];

static void* encoded_function_pointers[function_id_count];


#define _APPLY(_FUNCTION, _MODULES) \
    using _CRT_CONCATENATE(_FUNCTION, _pft) = decltype(_FUNCTION)*;
	_YY_APPLY_TO_LATE_BOUND_FUNCTIONS(_APPLY)
#undef _APPLY

enum : int
{
	__crt_maximum_pointer_shift = sizeof(uintptr_t) * 8
};

static __forceinline unsigned int __crt_rotate_pointer_value(unsigned int const value, int const shift) throw()
{
	return RotateRight32(value, shift);
}

static __forceinline unsigned __int64 __crt_rotate_pointer_value(unsigned __int64 const value, int const shift) throw()
{
	return RotateRight64(value, shift);
}

static PVOID __fastcall __CRT_DecodePointer(
		PVOID Ptr
	)
{
	return reinterpret_cast<PVOID>(
		__crt_rotate_pointer_value(
			reinterpret_cast<uintptr_t>(Ptr) ^ __security_cookie,
			__security_cookie % __crt_maximum_pointer_shift
		)
		);
}

static PVOID __fastcall __CRT_EncodePointer(PVOID const Ptr)
{
	return reinterpret_cast<PVOID>(
		__crt_rotate_pointer_value(
			reinterpret_cast<uintptr_t>(Ptr),
			__crt_maximum_pointer_shift - (__security_cookie % __crt_maximum_pointer_shift)
		) ^ __security_cookie
		);
}

template <typename T>
static __forceinline T __crt_fast_decode_pointer(T const p) throw()
{
	return reinterpret_cast<T>(__CRT_DecodePointer(p));
}

template <typename T>
static __forceinline T __crt_fast_encode_pointer(T const p) throw()
{
	return reinterpret_cast<T>(__CRT_EncodePointer(p));
}

template <typename T, typename V>
static __forceinline T* __crt_interlocked_exchange_pointer(T* const volatile* target, V const value) throw()
{
	// This is required to silence a spurious unreferenced formal parameter
	// warning.
	UNREFERENCED_PARAMETER(value);

	return reinterpret_cast<T*>(_InterlockedExchangePointer((void**)target, (void*)value));
}

template <typename T>
static __forceinline T* __crt_interlocked_read_pointer(T* const volatile* target) throw()
{
	return __crt_interlocked_compare_exchange_pointer(target, nullptr, nullptr);
}

template <typename T, typename E, typename C>
static __forceinline T* __crt_interlocked_compare_exchange_pointer(T* const volatile* target, E const exchange, C const comparand) throw()
{
	UNREFERENCED_PARAMETER(exchange);  // These are required to silence spurious
	UNREFERENCED_PARAMETER(comparand); // unreferenced formal parameter warnings.

	return reinterpret_cast<T*>(_InterlockedCompareExchangePointer(
		(void**)target, (void*)exchange, (void*)comparand));
}


static HMODULE __cdecl try_load_library_from_system_directory(wchar_t const* const name) throw()
{
	HMODULE const handle = LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (handle)
	{
		return handle;
	}

	//目标系统不支持 LOAD_LIBRARY_SEARCH_SYSTEM32 ？
	if (GetLastError() == ERROR_INVALID_PARAMETER )
	{
		return LoadLibraryExW(name, nullptr, 0);
	}

	return nullptr;
}


static HMODULE __cdecl try_get_module(module_id const id) throw()
{
	// First check to see if we've cached the module handle:
	if (HMODULE const cached_handle = __crt_interlocked_read_pointer(module_handles + id))
	{
		if (cached_handle == INVALID_HANDLE_VALUE)
		{
			return nullptr;
		}

		return cached_handle;
	}

	// If we haven't yet cached the module handle, try to load the library.  If
	// this fails, cache the sentinel handle value INVALID_HANDLE_VALUE so that
	// we don't attempt to load the module again:
	HMODULE const new_handle = try_load_library_from_system_directory(module_names[id]);
	if (!new_handle)
	{
		if (HMODULE const cached_handle = __crt_interlocked_exchange_pointer(module_handles + id, INVALID_HANDLE_VALUE))
		{
			_ASSERTE(cached_handle == INVALID_HANDLE_VALUE);
		}

		return nullptr;
	}

	// Swap the new handle into the cache.  If the cache no longer contained a
	// null handle, then some other thread loaded the module and cached the
	// handle while we were doing the same.  In that case, we free the handle
	// once to maintain the reference count:
	if (HMODULE const cached_handle = __crt_interlocked_exchange_pointer(module_handles + id, new_handle))
	{
		_ASSERTE(cached_handle == new_handle);
		FreeLibrary(new_handle);
	}

	return new_handle;
}



static __forceinline void* __cdecl try_get_proc_address_from_first_available_module(
	char      const* const name,
	module_id        const module_id
) throw()
{
	HMODULE const module_handle = try_get_module(module_id);
	if (!module_handle)
	{
		return nullptr;
	}

	return reinterpret_cast<void*>(GetProcAddress(module_handle, name));
}


static __forceinline void* __cdecl invalid_function_sentinel() throw()
{
	return reinterpret_cast<void*>(static_cast<uintptr_t>(-1));
}

static void* __cdecl try_get_function(
	function_id      const id,
	char      const* const name,
	module_id        const module_id
) throw()
{
	// First check to see if we've cached the function pointer:
	{
		void* const cached_fp = __crt_fast_decode_pointer(
			__crt_interlocked_read_pointer(encoded_function_pointers + id));

		if (cached_fp == invalid_function_sentinel())
		{
			return nullptr;
		}

		if (cached_fp)
		{
			return cached_fp;
		}
	}

	// If we haven't yet cached the function pointer, try to import it from any
	// of the modules in which it might be defined.  If this fails, cache the
	// sentinel pointer so that we don't attempt to load this function again:
	void* const new_fp = try_get_proc_address_from_first_available_module(name, module_id);
	if (!new_fp)
	{
		void* const cached_fp = __crt_fast_decode_pointer(
			__crt_interlocked_exchange_pointer(
				encoded_function_pointers + id,
				__crt_fast_encode_pointer(invalid_function_sentinel())));

		if (cached_fp)
		{
			_ASSERTE(cached_fp == invalid_function_sentinel());
		}

		return nullptr;
	}

	// Swap the newly obtained function pointer into the cache.  The cache may
	// no longer contain an encoded null pointer if another thread obtained the
	// function address while we were doing the same (both threads should have
	// gotten the same function pointer):
	{
		void* const cached_fp = __crt_fast_decode_pointer(
			__crt_interlocked_exchange_pointer(
				encoded_function_pointers + id,
				__crt_fast_encode_pointer(new_fp)));

		if (cached_fp)
		{
			_ASSERTE(cached_fp == new_fp);
		}
	}

	return new_fp;
}


#define _APPLY(_FUNCTION, _MODULE)                                                                    \
    static _CRT_CONCATENATE(_FUNCTION, _pft) __cdecl _CRT_CONCATENATE(try_get_, _FUNCTION)() throw()  \
    {                                                                                                 \
                                                                                                      \
        return reinterpret_cast<_CRT_CONCATENATE(_FUNCTION, _pft)>(try_get_function(                  \
            _CRT_CONCATENATE(_FUNCTION, _id),                                                         \
            _CRT_STRINGIZE(_FUNCTION),                                                                \
            _MODULE));                                                                                \
    }
	_YY_APPLY_TO_LATE_BOUND_FUNCTIONS(_APPLY)
#undef _APPLY


static void __cdecl __YY_uninitialize_winapi_thunks()
{
	for (HMODULE& module : module_handles)
	{
		if (module)
		{
			if (module != INVALID_HANDLE_VALUE)
			{
				FreeLibrary(module);
			}

			module = nullptr;
		}
	}
}

static int __cdecl __YY_initialize_winapi_thunks()
{
	void* const encoded_nullptr = __crt_fast_encode_pointer((void*)nullptr);

	for (void*& p : encoded_function_pointers)
	{
		p = encoded_nullptr;
	}

	atexit(__YY_uninitialize_winapi_thunks);

	return 0;
}


typedef int(__cdecl* _PIFV)(void);

#pragma section(".CRT$XID",    long, read) // CRT C Initializers

__declspec(allocate(".CRT$XID")) static _PIFV ___Initialization = __YY_initialize_winapi_thunks;