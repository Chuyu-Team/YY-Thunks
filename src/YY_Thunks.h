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

#define _LCRT_DEFINE_IAT_SYMBOL(_FUNCTION,prefixed)                                                           \
    extern "C" __declspec(selectany) void const* const _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(_FUNCTION, prefixed) \
        = reinterpret_cast<void const*>(_FUNCTION)


#pragma section(".YYThu$AAA",    long, read, write) //鸭穿模块缓存节点
#pragma section(".YYThu$AAB",    long, read, write) //鸭船函数缓存节点
#pragma section(".YYThu$AAC",    long, read, write) //保留，暂时用于边界结束

#pragma comment(linker, "/merge:.YYThu=.data")


__declspec(allocate(".YYThu$AAB")) static void* __YY_THUNKS_FUN_START[] = { nullptr }; //鸭船指针缓存开始位置
__declspec(allocate(".YYThu$AAC")) static void* __YY_THUNKS_FUN_END[] = { nullptr };   //鸭船指针缓存开始位置


#pragma detect_mismatch("YY-Thunks-Mode", "ver:" _CRT_STRINGIZE(YY_Thunks_Support_Version))

/*
导出一个外部符号，指示当前鸭船Thunks等级。
1. 方便外部检测是否使用了鸭船。
2. 当调用者同时使用多个鸭船时，使其链接失败！

//自行定义为 C 弱符号，C++支持不佳！
EXTERN_C const DWORD __YY_Thunks_Installed;

if(__YY_Thunks_Installed)
	wprintf(L"检测到使用YY-Thunks，Thunks级别=0x%.8X\n", __YY_Thunks_Installed);
else
	wprintf(L"没有使用YY-Thunks！\n");

*/
EXTERN_C const DWORD __YY_Thunks_Installed = YY_Thunks_Support_Version;

/*
导出一个外部弱符号，指示当前是否处于强行卸载模式。

EXTERN_C BOOL __YY_Thunks_Process_Terminating;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	swtich(dwReason)
	{
		case DLL_PROCESS_DETACH:
		//我们可以通过 lpReserved != NULL 判断，当前是否处于强行卸载模式。
		__YY_Thunks_Process_Terminating = lpReserved != NULL;

		……
		break;
	……
*/
EXTERN_C extern BOOL __YY_Thunks_Process_Terminating;

enum module_id : unsigned
{
#define _APPLY(_SYMBOL, _NAME) _SYMBOL,
	_YY_APPLY_TO_LATE_BOUND_MODULES(_APPLY)
#undef _APPLY

	module_id_count
};


static constexpr wchar_t const* const module_names[module_id_count] =
{
#define _APPLY(_SYMBOL, _NAME) _CRT_WIDE(_NAME),
	_YY_APPLY_TO_LATE_BOUND_MODULES(_APPLY)
#undef _APPLY
};

__declspec(allocate(".YYThu$AAA"))
static HMODULE module_handles[module_id_count];

#define _APPLY(_FUNCTION, _MODULES) \
    using _CRT_CONCATENATE(_FUNCTION, _pft) = decltype(_FUNCTION)*;
	_YY_APPLY_TO_LATE_BOUND_FUNCTIONS(_APPLY)
#undef _APPLY


// Implements wcsncpmp for ASCII chars only.
// NOTE: We can't use wcsncmp in this context because we may end up trying to modify
// locale data structs or even calling the same function in NLS code.
static int _fastcall __wcsnicmp_ascii(const wchar_t* string1, const wchar_t* string2, size_t count) noexcept
{
	wchar_t f, l;
	int result = 0;

	if (count)
	{
		/* validation section */
		do {
			f = __ascii_towlower(*string1);
			l = __ascii_towlower(*string2);
			string1++;
			string2++;
		} while ((--count) && f && (f == l));

		result = (int)(f - l);
	}

	return result;
}

enum : int
{
	__crt_maximum_pointer_shift = sizeof(uintptr_t) * 8
};

static __forceinline unsigned int __fastcall __crt_rotate_pointer_value(unsigned int const value, int const shift) noexcept
{
	return RotateRight32(value, shift);
}

static __forceinline unsigned __int64 __fastcall __crt_rotate_pointer_value(unsigned __int64 const value, int const shift) noexcept
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
static __forceinline T __fastcall __crt_fast_decode_pointer(T const p) noexcept
{
	return reinterpret_cast<T>(__CRT_DecodePointer(p));
}

template <typename T>
static __forceinline T __fastcall __crt_fast_encode_pointer(T const p) noexcept
{
	return reinterpret_cast<T>(__CRT_EncodePointer(p));
}

template <typename T, typename V>
static __forceinline T* __fastcall __crt_interlocked_exchange_pointer(T* const volatile* target, V const value) noexcept
{
	// This is required to silence a spurious unreferenced formal parameter
	// warning.
	UNREFERENCED_PARAMETER(value);

	return reinterpret_cast<T*>(_InterlockedExchangePointer((void**)target, (void*)value));
}

template <typename T, typename E, typename C>
static __forceinline T* __fastcall __crt_interlocked_compare_exchange_pointer(T* const volatile* target, E const exchange, C const comparand) noexcept
{
	UNREFERENCED_PARAMETER(exchange);  // These are required to silence spurious
	UNREFERENCED_PARAMETER(comparand); // unreferenced formal parameter warnings.

	return reinterpret_cast<T*>(_InterlockedCompareExchangePointer(
		(void**)target, (void*)exchange, (void*)comparand));
}


template <typename T>
static __forceinline T* __fastcall __crt_interlocked_read_pointer(T* const volatile* target) noexcept
{
	return __crt_interlocked_compare_exchange_pointer(target, nullptr, nullptr);
}

#if defined(_X86_) || defined(_M_IX86)
static decltype(RtlWow64EnableFsRedirectionEx)* pRtlWow64EnableFsRedirectionEx;
#endif

static bool bSupportSafe;


static HMODULE __fastcall try_load_library_from_system_directory(wchar_t const* const name) noexcept
{
	if(bSupportSafe)
	{
		//我们引入 bSupportSafe 变量是因为联想一键影音，会导致老系统 LoadLibraryExW 错误代码 变成 ERROR_ACCESS_DENIED，而不是预期的ERROR_INVALID_PARAMETER。
		return LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	}
	else
	{
		//名字不可能为 空。
		if (name == nullptr || *name == L'\0')
			return nullptr;

		wchar_t szDllFilePath[512];

		auto cchResult = GetSystemDirectoryW(szDllFilePath, _countof(szDllFilePath));

		if (cchResult == 0 || cchResult >= _countof(szDllFilePath))
		{
			//失败或者缓冲区不足则不处理（因为很显然 512 大小都不够？开玩笑……）
			return nullptr;
		}

		szDllFilePath[cchResult++] = L'\\';
		if (cchResult >= _countof(szDllFilePath))
		{
			return nullptr;
		}

		for (auto szName = name; *szName; ++szName)
		{
			szDllFilePath[cchResult++] = *szName;
			if (cchResult >= _countof(szDllFilePath))
			{
				return nullptr;
			}
		}

		// 将字符串 \0 截断
		szDllFilePath[cchResult] = L'\0';


#if defined(_X86_) || defined(_M_IX86)
		PVOID OldFsRedirectionLevel;

		/*
		Windows 7 RTM以其以前版本 LoadLibrary内部默认不会关闭重定向。
		为了防止某些线程在关闭重定向的情况下调用API，依然能正常加载相关dll，因此我们在此处恢复重定向。
		*/
		auto Status = pRtlWow64EnableFsRedirectionEx ? pRtlWow64EnableFsRedirectionEx(nullptr, &OldFsRedirectionLevel) : STATUS_INVALID_PARAMETER;
#endif
		auto hModule = LoadLibraryExW(szDllFilePath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);

#if defined(_X86_) || defined(_M_IX86)
		//将重定向恢复到以前的状态。
		if (Status >= 0 && pRtlWow64EnableFsRedirectionEx)
			pRtlWow64EnableFsRedirectionEx(OldFsRedirectionLevel, &OldFsRedirectionLevel);
#endif
		return hModule;
	}
}


static HMODULE __fastcall try_get_module(module_id const id) noexcept
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



static __forceinline void* __fastcall try_get_proc_address_from_first_available_module(
	char      const* const name,
	module_id        const module_id
) noexcept
{
	HMODULE const module_handle = try_get_module(module_id);
	if (!module_handle)
	{
		return nullptr;
	}

	return reinterpret_cast<void*>(GetProcAddress(module_handle, name));
}


static __forceinline void* __cdecl invalid_function_sentinel() noexcept
{
	return reinterpret_cast<void*>(static_cast<uintptr_t>(-1));
}

static void* __fastcall try_get_function(
	void**      ppFunAddress,
	char      const* const name,
	module_id        const module_id
) noexcept
{
	// First check to see if we've cached the function pointer:
	{
		void* const cached_fp = __crt_fast_decode_pointer(
			__crt_interlocked_read_pointer(ppFunAddress));

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
				ppFunAddress,
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
				ppFunAddress,
				__crt_fast_encode_pointer(new_fp)));

		if (cached_fp)
		{
			_ASSERTE(cached_fp == new_fp);
		}
	}

	return new_fp;
}


#define _APPLY(_FUNCTION, _MODULE)                                                                    \
    static _CRT_CONCATENATE(_FUNCTION, _pft) __cdecl _CRT_CONCATENATE(try_get_, _FUNCTION)() noexcept \
    {                                                                                                 \
        __declspec(allocate(".YYThu$AAB")) static void* _CRT_CONCATENATE( pFun_ ,_FUNCTION);            \
        return reinterpret_cast<_CRT_CONCATENATE(_FUNCTION, _pft)>(try_get_function(                  \
            &_CRT_CONCATENATE( pFun_ ,_FUNCTION),                                                     \
            _CRT_STRINGIZE(_FUNCTION),                                                                \
            _MODULE));                                                                                \
    }
	_YY_APPLY_TO_LATE_BOUND_FUNCTIONS(_APPLY)
#undef _APPLY


static void __cdecl __YY_uninitialize_winapi_thunks()
{
	//当DLL被强行卸载时，我们什么都不做。
	if (__YY_Thunks_Process_Terminating)
		return;

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
#if defined(_X86_) || defined(_M_IX86)
	if (auto hNtdll = GetModuleHandleW(L"ntdll"))
	{
		pRtlWow64EnableFsRedirectionEx = (decltype(RtlWow64EnableFsRedirectionEx)*)GetProcAddress(hNtdll, "RtlWow64EnableFsRedirectionEx");
	}
#endif
	if (auto hKernel32 = GetModuleHandleW(L"kernel32"))
	{
		bSupportSafe = GetProcAddress(hKernel32, "AddDllDirectory") != nullptr;
	}

	void* const encoded_nullptr = __crt_fast_encode_pointer((void*)nullptr);

	for (auto p = __YY_THUNKS_FUN_START; p != __YY_THUNKS_FUN_END; ++p)
	{
		*p = encoded_nullptr;
	}

	atexit(__YY_uninitialize_winapi_thunks);

	return 0;
}


typedef int(__cdecl* _PIFV)(void);

#pragma section(".CRT$XID",    long, read) // CRT C Initializers

__declspec(allocate(".CRT$XID")) static _PIFV ___Initialization = __YY_initialize_winapi_thunks;