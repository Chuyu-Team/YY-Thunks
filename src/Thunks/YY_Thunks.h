#pragma once


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

//YY-Thunks保证STDIO新老模式兼容
#define _CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS

#include <Windows.h>
#include <crtdbg.h>
#include <intrin.h>



#if defined(_M_IX86)
    #define _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(_FUNCTION, _SIZE) _CRT_CONCATENATE(_CRT_CONCATENATE(_imp__, _FUNCTION), _CRT_CONCATENATE(_, _SIZE))
#elif defined(_M_AMD64)
    #define _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(_FUNCTION, _SIZE) _CRT_CONCATENATE(__imp_, _FUNCTION)
#else
     #error "不支持此体系"
#endif

#if defined(_M_IX86)
//x86的符号存在@ 我们使用 identifier 特性解决
#define _LCRT_DEFINE_IAT_SYMBOL(_FUNCTION, _SIZE)                                                                       \
    __pragma(warning(suppress:4483))                                                                                    \
    extern "C" __declspec(selectany) void const* const __identifier(_CRT_STRINGIZE_(_imp__ ## _FUNCTION ## @ ## _SIZE)) \
        = reinterpret_cast<void const*>(_FUNCTION)
#else
#define _LCRT_DEFINE_IAT_SYMBOL(_FUNCTION, _SIZE)                                                          \
    extern "C" __declspec(selectany) void const* const _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(_FUNCTION, _SIZE) \
        = reinterpret_cast<void const*>(_FUNCTION)
#endif

#if defined(_M_IX86)
// 根据 https://github.com/Chuyu-Team/YY-Thunks/issues/78 修正一下rust raw-dylib引用规则
#define _YY_THUNKS_DEFINE_RUST_RAW_DYLIB_IAT_SYMBOL(_FUNCTION, _SIZE, _FUNCTION_ADDRESS)                                    \
    __pragma(warning(suppress:4483))                                                                     \
    extern "C" __declspec(selectany) void const* const __identifier(_CRT_STRINGIZE_(_imp_ ## _FUNCTION)) \
        = reinterpret_cast<void const*>(_FUNCTION_ADDRESS)
#else
#define _YY_THUNKS_DEFINE_RUST_RAW_DYLIB_IAT_SYMBOL(_FUNCTION, _SIZE, _FUNCTION_ADDRESS)
#endif

#ifdef __YY_Thunks_Unit_Test
    #define __APPLY_UNIT_TEST_BOOL(_FUNCTION) bool _CRT_CONCATENATE(aways_null_try_get_, _FUNCTION) = false
    #define __CHECK_UNIT_TEST_BOOL(_FUNCTION) if(_CRT_CONCATENATE(aways_null_try_get_, _FUNCTION)) return nullptr
#else
    #define __APPLY_UNIT_TEST_BOOL(_FUNCTION)
    #define __CHECK_UNIT_TEST_BOOL(_FUNCTION)
#endif


#pragma section(".YYThu$AAA",    long, read, write) //鸭船模块缓存节点
#pragma section(".YYThu$AAB",    long, read, write) //鸭船函数缓存节点
#pragma section(".YYThu$AAC",    long, read, write) //保留，暂时用于边界结束

#pragma section(".YYThr$AAA",    long, read)        //鸭船函数缓存初始化函数
#pragma section(".YYThr$AAB",    long, read)        //鸭船函数反初始化函数
#pragma section(".YYThr$AAC",    long, read)        //保留，暂时用于边界结束

#pragma comment(linker, "/merge:.YYThu=.data")
#pragma comment(linker, "/merge:.YYThr=.rdata")

__declspec(allocate(".YYThu$AAA")) static void* __YY_THUNKS_MODULE_START[] = { nullptr };
__declspec(allocate(".YYThu$AAB")) static void* __YY_THUNKS_FUN_START[] = { nullptr }; //鸭船指针缓存开始位置
__declspec(allocate(".YYThu$AAC")) static void* __YY_THUNKS_FUN_END[] = { nullptr };   //鸭船指针缓存结束位置

__declspec(allocate(".YYThr$AAA")) static void* __YY_THUNKS_INIT_FUN_START[] = { nullptr }; //鸭船函数初始化开始位置
#define __YY_THUNKS_INIT_FUN_END __YY_THUNKS_UNINIT_FUN_START                               //鸭船函数初始化结束位置

__declspec(allocate(".YYThr$AAB")) static void* __YY_THUNKS_UNINIT_FUN_START[] = { nullptr }; //鸭船函数反初始化函数开始
__declspec(allocate(".YYThr$AAC")) static void* __YY_THUNKS_UNINIT_FUN_END[] = { nullptr };   //鸭船函数反初始化函数结束位置

typedef void* (__cdecl* InitFunType)();
typedef void (__cdecl* UninitFunType)();

#pragma detect_mismatch("YY-Thunks-Mode", "ver:" _CRT_STRINGIZE(YY_Thunks_Target))

/*
导出一个外部符号，指示当前鸭船Thunks等级。
1. 方便外部检测是否使用了鸭船。
2. 当调用者同时使用多个鸭船时，使其链接失败！

//自行定义为 C 弱符号，C++支持不佳！
EXTERN_C const UINT64 __YY_Thunks_Installed;

if(__YY_Thunks_Installed)
{
    wprintf(
        L"检测到使用YY-Thunks，Thunks级别=%hd.%hd.%hd.%hd。\n",
        UINT16(__YY_Thunks_Installed >> 48),
        UINT16(__YY_Thunks_Installed >> 32),
        UINT16(__YY_Thunks_Installed >> 16),
        UINT16(__YY_Thunks_Installed));
}
else
{
    wprintf(L"没有使用YY-Thunks！\n");
}

// 如果选择兼容到Vista，上述代码将输出：检测到使用YY-Thunks，Thunks级别=6.0.6000.0。
// 如果没有使用YY-Thunks，那么上述代码将输出：没有使用YY-Thunks！
*/
EXTERN_C const UINT64 __YY_Thunks_Installed = YY_Thunks_Target;

/*
导出一个外部弱符号，指示当前是否关闭DLL预载（默认开启DLL预载）。
警告：关闭DLL预载虽然可以提升初始化速度，但是这可能带来死锁问题。目前已知的有：
  1. locale锁/LoadDll锁交叉持锁产生死锁（https://github.com/Chuyu-Team/YY-Thunks/issues/7）
  2. thread safe init锁/LoadDll锁交叉持锁产生死锁

// 如果需要关闭DLL预载，那么再任意位置定义如下变量即可：
EXTERN_C const BOOL __YY_Thunks_Disable_Rreload_Dlls = TRUE;
*/
EXTERN_C extern BOOL __YY_Thunks_Disable_Rreload_Dlls /* = FALSE*/;


// 直接通过GetModuleHandleW获取，改选项非常危险，如果dll尚未加载会将不会加载！！！
#define USING_GET_MODULE_HANDLE 0x00000001
// 以 LOAD_LIBRARY_AS_DATAFILE 标记作为资源加载。
#define LOAD_AS_DATA_FILE 0x00000002
// 直接使用LoadLibrary，该加载模式存在劫持风险，使用前请确认该DLL处于KnownDll。
#define USING_UNSAFE_LOAD 0x00000004
/// <summary>
/// 如果对YY-Thunks的内置的LoadLibrary加载方式不满意，则通过设置__pfnYY_Thunks_CustomLoadLibrary以实现自定义DLL加载。
/// </summary>
/// <param name="_szModuleName">需要加载的模块名称，比如`ntdll.dll`。</param>
/// <param name="_fFlags">请参考 USING_GET_MODULE_HANDLE 等宏。</param>
/// <returns>
/// 返回 nullptr：继续执行YY_Thunk默认DLL加载流程。
/// 返回 -1 ：加载失败，并阻止执行YY_Thunks默认加载流程。
/// 其他：CustomLoadLibrary加载成功，必须返回有效的 HMODULE。
/// </returns>
EXTERN_C extern HMODULE (__fastcall * const __pfnYY_Thunks_CustomLoadLibrary)(const wchar_t* _szModuleName, DWORD _fFlags);

// 从DllMain缓存RtlDllShutdownInProgress状态，规避退出时调用RtlDllShutdownInProgress。
// 0：缓存无效
// 1：模块正常卸载
// -1：开始进程准备终止
static int __YY_Thunks_Process_Terminating;

#if (YY_Thunks_Target < __WindowsNT6)
static HANDLE _GlobalKeyedEventHandle;
#endif

static uintptr_t __security_cookie_yy_thunks;

extern "C" IMAGE_DOS_HEADER __ImageBase;

#define _APPLY(_SYMBOL, _NAME, ...) \
    constexpr const wchar_t* _CRT_CONCATENATE(module_name_, _SYMBOL) = _CRT_WIDE(_NAME);
    _YY_APPLY_TO_LATE_BOUND_MODULES(_APPLY)
#undef _APPLY

#define _APPLY(_FUNCTION, _MODULES) \
    using _CRT_CONCATENATE(_FUNCTION, _pft) = decltype(_FUNCTION)*;
    _YY_APPLY_TO_LATE_BOUND_FUNCTIONS(_APPLY)
#undef _APPLY


// Implements wcsncpmp for ASCII chars only.
// NOTE: We can't use wcsncmp in this context because we may end up trying to modify
// locale data structs or even calling the same function in NLS code.
template<typename Char1, typename Char2>
static int _fastcall StringCompareIgnoreCaseByAscii(const Char1* string1, const Char2* string2, size_t count) noexcept
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

template<typename Char1, typename Char2>
static int _fastcall StringCompare(const Char1* string1, const Char2* string2, size_t count) noexcept
{
    wchar_t f, l;
    int result = 0;

    if (count)
    {
        /* validation section */
        do {
            f = *string1;
            l = *string2;
            string1++;
            string2++;
        } while ((--count) && f && (f == l));

        result = (int)(f - l);
    }

    return result;
}

static PVOID __fastcall YY_ImageDirectoryEntryToData(
    __in PVOID pBaseAddress,
    __in ULONG dwDirectory,
    __out PULONG pSize
    )
{
    auto _pDosHeader = (PIMAGE_DOS_HEADER)pBaseAddress;
    auto _pNtHerder = reinterpret_cast<PIMAGE_NT_HEADERS>(PBYTE(pBaseAddress) + _pDosHeader->e_lfanew);
    auto& _DataDirectory = _pNtHerder->OptionalHeader.DataDirectory[dwDirectory];

    *pSize = _DataDirectory.Size;
    if (_DataDirectory.Size == 0 || _DataDirectory.VirtualAddress == 0)
        return nullptr;

    return PBYTE(pBaseAddress) + _DataDirectory.VirtualAddress;
}

static DWORD __fastcall GetDllTimeDateStamp(_In_ HMODULE _hModule)
{
    if (!_hModule)
        return 0;

    auto _pDosHeader = (PIMAGE_DOS_HEADER)_hModule;
    auto _pNtHerder = reinterpret_cast<PIMAGE_NT_HEADERS>(PBYTE(_hModule) + _pDosHeader->e_lfanew);
    return _pNtHerder->FileHeader.TimeDateStamp;
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

#ifdef _WIN64
#define __foreinclude(p)                          \
    __pragma(warning(suppress:6031))              \
    _bittest64((LONG_PTR*)(&p), 0)
#else
#define __foreinclude(p)                          \
    __pragma(warning(suppress:6031))              \
    _bittest((LONG_PTR*)(&p), 0)
#endif

enum class ThunksInitStatus : LONG
{
    // 已经反初始化
    Uninitialized = -2,
    // 其他线程正在处理
    Wait = -1,
    None = 0,
    InitByDllMain,
    InitByCRT,
    // 调用被Thunks的API时发生的延后初始化
    InitByAPI,
};

static ThunksInitStatus __cdecl _YY_initialize_winapi_thunks(ThunksInitStatus _sInitStatus);

static PVOID __fastcall __CRT_DecodePointer(
        PVOID Ptr
    )
{
    return reinterpret_cast<PVOID>(
        __crt_rotate_pointer_value(
            reinterpret_cast<uintptr_t>(Ptr) ^ __security_cookie_yy_thunks,
            __security_cookie_yy_thunks % __crt_maximum_pointer_shift
        )
        );
}

static PVOID __fastcall __CRT_EncodePointer(PVOID const Ptr)
{
    return reinterpret_cast<PVOID>(
        __crt_rotate_pointer_value(
            reinterpret_cast<uintptr_t>(Ptr),
            __crt_maximum_pointer_shift - (__security_cookie_yy_thunks % __crt_maximum_pointer_shift)
        ) ^ __security_cookie_yy_thunks
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

static HMODULE __fastcall try_get_module(volatile HMODULE* pModule, const wchar_t* module_name, int Flags) noexcept;

#define _APPLY(_MODULE, _NAME, _FLAGS)                                                         \
    static HMODULE __fastcall _CRT_CONCATENATE(try_get_module_, _MODULE)() noexcept            \
    {                                                                                          \
        __declspec(allocate(".YYThu$AAA")) static volatile HMODULE hModule;                    \
        return try_get_module(&hModule, _CRT_CONCATENATE(module_name_, _MODULE), _FLAGS);      \
    }
_YY_APPLY_TO_LATE_BOUND_MODULES(_APPLY)
#undef _APPLY

struct ProcInfo;
typedef HMODULE (__fastcall* try_get_module_fun)();
typedef void*(__fastcall* custom_try_get_proc_fun)(const ProcInfo& _ProcInfo);

struct ProcInfo
{
    char const* const szProcName;
    try_get_module_fun pfnGetModule;
    custom_try_get_proc_fun pfnCustomGetProcAddress;
};

static __forceinline void* __fastcall try_get_proc_address_from_first_available_module(
    const ProcInfo& _ProcInfo
    ) noexcept;

static __forceinline void* __fastcall try_get_proc_address_from_dll(
    const ProcInfo& _ProcInfo
    ) noexcept;

struct ProcOffsetInfo
{
    DWORD uTimeDateStamp;
    DWORD uProcOffset;
};

template<size_t kLength>
static __forceinline void* __fastcall try_get_proc_address_from_offset(
    HMODULE _hModule,
    const ProcOffsetInfo (&_ProcOffsetInfos)[kLength]
    ) noexcept
{
    if (_hModule)
    {
        __try
        {
            const auto _uTimeDateStamp = GetDllTimeDateStamp(_hModule);
            if (_uTimeDateStamp)
            {
                for (auto& _ProcOffsetInfo : _ProcOffsetInfos)
                {
                    if (_ProcOffsetInfo.uTimeDateStamp == _uTimeDateStamp)
                    {
                        return PBYTE(_hModule) + _ProcOffsetInfo.uProcOffset;
                    }
                }
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            // 避免遇到畸形DLL而触发异常。
        }
    }

    return nullptr;
}

static __forceinline void* __cdecl invalid_function_sentinel() noexcept
{
    return reinterpret_cast<void*>(static_cast<uintptr_t>(-1));
}


static void* __fastcall try_get_function(
    void**      ppFunAddress,
    const ProcInfo& _ProcInfo
    ) noexcept
{
    // 指针为空，那么往往还没有调用初始化，尝试动态初始化
    if (*ppFunAddress == nullptr)
    {
        _YY_initialize_winapi_thunks(ThunksInitStatus::InitByAPI);
    }

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
    void* const new_fp = try_get_proc_address_from_first_available_module(_ProcInfo);
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
    __APPLY_UNIT_TEST_BOOL(_FUNCTION);                                                                \
    static _CRT_CONCATENATE(_FUNCTION, _pft) __cdecl _CRT_CONCATENATE(try_get_, _FUNCTION)() noexcept \
    {                                                                                                 \
        __CHECK_UNIT_TEST_BOOL(_FUNCTION);                                                            \
        __declspec(allocate(".YYThr$AAA")) static void* const _CRT_CONCATENATE(pInit_ ,_FUNCTION) =   \
              reinterpret_cast<void*>(&_CRT_CONCATENATE(try_get_, _FUNCTION));                        \
        /*为了避免编译器将 YYThr$AAA 节优化掉*/                                                       \
        __foreinclude(_CRT_CONCATENATE(pInit_ ,_FUNCTION));                                           \
        __declspec(allocate(".YYThu$AAB")) static void* _CRT_CONCATENATE( pFun_ ,_FUNCTION);          \
        static const ProcInfo _ProcInfo =                                                             \
        {                                                                                             \
            _CRT_STRINGIZE(_FUNCTION),                                                                \
            &_CRT_CONCATENATE(try_get_module_, _MODULE),                                              \
__if_exists(YY::Thunks::Fallback::_CRT_CONCATENATE(try_get_, _FUNCTION))                              \
{                                                                                                     \
            &YY::Thunks::Fallback::_CRT_CONCATENATE(try_get_, _FUNCTION)                              \
}                                                                                                     \
        };                                                                                            \
        return reinterpret_cast<_CRT_CONCATENATE(_FUNCTION, _pft)>(try_get_function(                  \
            &_CRT_CONCATENATE(pFun_ ,_FUNCTION),                                                      \
            _ProcInfo));                                                                              \
    }
    _YY_APPLY_TO_LATE_BOUND_FUNCTIONS(_APPLY)
#undef _APPLY


#ifdef _WIN64
#define DEFAULT_SECURITY_COOKIE ((uintptr_t)0x00002B992DDFA232)
#else  /* _WIN64 */
#define DEFAULT_SECURITY_COOKIE ((uintptr_t)0xBB40E64E)
#endif  /* _WIN64 */

static UINT_PTR GetSecurityNewCookie()
{
    /*
    * Union to facilitate converting from FILETIME to unsigned __int64
    */
    typedef union {
        unsigned __int64 ft_scalar;
        FILETIME ft_struct;
    } FT;

    UINT_PTR cookie;
    FT systime = { 0 };
    LARGE_INTEGER perfctr;

    GetSystemTimeAsFileTime(&systime.ft_struct);
#if defined (_WIN64)
    cookie = systime.ft_scalar;
#else  /* defined (_WIN64) */
    cookie = systime.ft_struct.dwLowDateTime;
    cookie ^= systime.ft_struct.dwHighDateTime;
#endif  /* defined (_WIN64) */

    cookie ^= GetCurrentThreadId();
    cookie ^= GetCurrentProcessId();

#if (YY_Thunks_Target >= __WindowsNT6)
#if defined (_WIN64)
    cookie ^= (((UINT_PTR)GetTickCount64()) << 56);
#endif  /* defined (_WIN64) */
    cookie ^= (UINT_PTR)GetTickCount64();
#else // (YY_Thunks_Target < __WindowsNT6)
    cookie ^= (UINT_PTR)GetTickCount();
#endif // !(YY_Thunks_Target < __WindowsNT6)

    QueryPerformanceCounter(&perfctr);
#if defined (_WIN64)
    cookie ^= (((UINT_PTR)perfctr.LowPart << 32) ^ perfctr.QuadPart);
#else  /* defined (_WIN64) */
    cookie ^= perfctr.LowPart;
    cookie ^= perfctr.HighPart;
#endif  /* defined (_WIN64) */

    /*
    * Increase entropy using ASLR relocation
    */
    cookie ^= (UINT_PTR)&cookie;

#if defined (_WIN64)
    /*
    * On Win64, generate a cookie with the most significant word set to zero,
    * as a defense against buffer overruns involving null-terminated strings.
    * Don't do so on Win32, as it's more important to keep 32 bits of cookie.
    */
    cookie &= 0x0000FFFFffffFFFFi64;
#endif  /* defined (_WIN64) */

    if (cookie == UINT_PTR(0) || cookie == DEFAULT_SECURITY_COOKIE)
    {
        cookie = DEFAULT_SECURITY_COOKIE + 1;
    }
    return cookie;
}

static volatile ThunksInitStatus s_eThunksStatus /*= ThunksInitStatus::None*/;

static bool __YY_DllShutdownInProgress()
{
    __if_exists(__YY_Thunks_Process_Terminating)
    {
        if (__YY_Thunks_Process_Terminating != 0)
            return __YY_Thunks_Process_Terminating == -1;
    }

#if (YY_Thunks_Target < __WindowsNT5_1) || !defined(__USING_NTDLL_LIB)
    if (const auto RtlDllShutdownInProgress = (decltype(::RtlDllShutdownInProgress)*)GetProcAddress(try_get_module_ntdll(), "RtlDllShutdownInProgress"))
#endif
    {
        return RtlDllShutdownInProgress();
    }

    // 按理说不太可能走到这里，难道是老系统时没有被DllMain接管？
    return false;
}

static void __cdecl __YY_uninitialize_winapi_thunks()
{
    // 只反初始化一次
    const auto _eStatus = (ThunksInitStatus)InterlockedExchange((volatile LONG*)&s_eThunksStatus, LONG(ThunksInitStatus::Uninitialized));

    // Uninitialized : 已经反初始化，那么也就没有必要再次反初始化。
    // Wait : 其他线程还在等待……保险起见我就直接跳过反初始化吧。崩溃还是如何听天由命吧
    // None : 怎么还没初始化呢……这是在逗我？
    if (LONG(_eStatus) <= 0)
        return;

    //当DLL被强行卸载时，我们什么都不做，因为依赖的函数指针可能是无效的。
    if(__YY_DllShutdownInProgress())
        return;

    auto pModule = (HMODULE*)__YY_THUNKS_MODULE_START;
    auto pModuleEnd = (HMODULE*)__YY_THUNKS_FUN_START;

    for (; pModule != pModuleEnd; ++pModule)
    {
        auto& module = *pModule;
        if (module)
        {
            if (module != INVALID_HANDLE_VALUE)
            {
                FreeLibrary(module);
            }

            module = nullptr;
        }
    }
#if (YY_Thunks_Target < __WindowsNT6)
    CloseHandle(_GlobalKeyedEventHandle);
#endif

    //执行本库中所有的反初始化工作。
    for (auto p = (UninitFunType*)__YY_THUNKS_UNINIT_FUN_START; p != (UninitFunType*)__YY_THUNKS_UNINIT_FUN_END; ++p)
    {
        if (auto pUninitFun = *p)
            pUninitFun();
    }
}

static ThunksInitStatus __cdecl _YY_initialize_winapi_thunks(ThunksInitStatus _sInitStatus)
{
#ifndef NDEBUG
    if (LONG(_sInitStatus) <= 0)
    {
        // 非法输入，请检查
        __debugbreak();
    }
#endif

    auto _eStatus = (ThunksInitStatus)InterlockedCompareExchange((volatile LONG*)&s_eThunksStatus, LONG(ThunksInitStatus::Wait), LONG(ThunksInitStatus::None));
    if (_eStatus == ThunksInitStatus::None)
    {
        // 首次进入，正在初始化
        if (__security_cookie == 0 || __security_cookie == DEFAULT_SECURITY_COOKIE)
        {
            __security_cookie_yy_thunks = GetSecurityNewCookie();
        }
        else
        {
            __security_cookie_yy_thunks = __security_cookie;
        }

        void* const encoded_nullptr = __crt_fast_encode_pointer((void*)nullptr);

        for (auto p = __YY_THUNKS_FUN_START; p != __YY_THUNKS_FUN_END; ++p)
        {
            *p = encoded_nullptr;
        }

        // 后续过程已经可以线程安全执行了，所以我们立即解锁，避免其他线程过于长时间的等待
        InterlockedExchange((volatile LONG*)&s_eThunksStatus, LONG(_sInitStatus));

        if (!__YY_Thunks_Disable_Rreload_Dlls)
        {
            /*
             * https://github.com/Chuyu-Team/YY-Thunks/issues/7
             * 为了避免 try_get 系列函数竞争 DLL load锁，因此我们将所有被Thunk的API都预先加载完毕。
             */
            for (auto p = (InitFunType*)__YY_THUNKS_INIT_FUN_START; p != (InitFunType*)__YY_THUNKS_INIT_FUN_END; ++p)
            {
                if (auto pInitFun = *p)
                    pInitFun();
            }
        }

        return _sInitStatus;
    }
    else if (_eStatus == ThunksInitStatus::Wait)
    {
        // 其他线程正在初始化……
        do
        {
            YieldProcessor();
            _eStatus = s_eThunksStatus;
        } while (_eStatus != ThunksInitStatus::Wait);
    }

    // 初始化已经完成
    return _eStatus;
}

// 外部weak符号，用于判断当前是否静态
EXTERN_C extern void* __acrt_atexit_table;

static int __cdecl __YY_initialize_winapi_thunks()
{
    const auto _eStatus = _YY_initialize_winapi_thunks(ThunksInitStatus::InitByCRT);
    if (_eStatus != ThunksInitStatus::InitByCRT && _eStatus != ThunksInitStatus::InitByAPI)
    {
        // 不接管由DllMain触发的初始化，因为它可以自行反初始化。
        return 0;
    }

    // 当前模块是exe不注册反初始化，如果模块就是exe自己，那么反初始化必然代表进程退出
    // 当进程退出时，关闭句柄或者卸载DLL这些都不是必须进行的，以提高性能。
    if (PVOID(&__ImageBase) == ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ImageBaseAddress)
        return 0;

    // 如果 == null，那么有2种情况：
    //   1. 非UCRT，比如2008，VC6，这时，调用 atexit是安全的，因为atexit在 XIA初始化完成了。
    //   2. UCRT 并且处于MD状态。这时 atexit初始化 会提与 XIA，这时也是没有问题的。
    // 如果 != null：
    //   那么说明UCRT处于静态状态（CRT DLL其实也是静态的一种）
    //   这时，XTY是可以正确执行的，这时就不用调用atexit了
    //   由于XTY晚于onexit，所以它是可靠的。
    if (__acrt_atexit_table == nullptr)
    {
        atexit(__YY_uninitialize_winapi_thunks);
    }

    return 0;
}


typedef int(__cdecl* _PIFV)(void);
typedef void(__cdecl* _PVFV)(void);

// CRT C Initializers
#pragma section(".CRT$XID",    long, read)
// CRT C Terminators（仅稍微之后于XTZ），这是故意的。
// YY-Thunks属于API底层理应尽可能的滞后，且反初始化工作不依赖与CRT
#pragma section(".CRT$XTY",    long, read)

__declspec(allocate(".CRT$XID")) static _PIFV ___Initialization = __YY_initialize_winapi_thunks;
__declspec(allocate(".CRT$XTY")) static _PVFV ___Uninitialization = __YY_uninitialize_winapi_thunks;
