/*
YY-Thunks支持的控制宏：
1. __FALLBACK_PREFIX：编译Lib库模式使用的API前缀修饰。一般来说只能为空或者传递 "YY_Thunks_"。
2. __USING_NTDLL_LIB：假定构建环境存在ntdll.lib，这可以减少一些NTDLL相关函数的动态加载。

特殊支持的变通方案：
1. __ENABLE_WORKAROUND_ALL
启用所有兼容方案，即__ENABLE_WORKAROUND_1 ~ __ENABLE_WORKAROUND_N，全部开启。

2. __ENABLE_WORKAROUND_1_GetProcAddress_ProcessPrng
兼容方案1：让GetProcAddress也能取到ProcessPrng函数地址。某些代码可能强制依赖ProcessPrng。

3. __ENABLE_WORKAROUND_2_UNNAME_OBJECT_DACL
兼容方案2：Windows 8.1以前的版本对于匿名对象无法生效DACL。就会导致Chrome的CheckPlatformHandlePermissionsCorrespondToMode判断不准确。
修复方案通过给匿名对象创建一个名字解决该问题。

特定项目的兼容方案：
1.  __APPLY_CHROMIUM_WORKAROUNDS
开启Chrome项目的兼容能力，等效于同时指定__ENABLE_WORKAROUND_1_GetProcAddress_ProcessPrng、__ENABLE_WORKAROUND_2_UNNAME_OBJECT_DACL。

*/

// 忽略非标准的 0 数组警告。
#pragma warning(disable:4200)
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define _YY_APPLY_TO_LATE_BOUND_MODULES(_APPLY)                                                                     \
    _APPLY(ntdll,                                        "ntdll"                              , USING_GET_MODULE_HANDLE ) \
    _APPLY(kernel32,                                     "kernel32"                           , USING_GET_MODULE_HANDLE ) \
    _APPLY(kernelbase,                                   "kernelbase"                         , USING_GET_MODULE_HANDLE ) \
    _APPLY(cfgmgr32,                                     "cfgmgr32"                           , 0                 ) \
    _APPLY(crypt32,                                      "crypt32"                            , 0                 ) \
    _APPLY(dwmapi,                                       "dwmapi"                             , 0                 ) \
    _APPLY(d3d9,                                         "d3d9"                               , 0                 ) \
    _APPLY(d3d11,                                        "d3d11"                              , 0                 ) \
    _APPLY(d3d12,                                        "d3d12"                              , 0                 ) \
    _APPLY(dbghelp,                                      "dbghelp"                            , USING_UNSAFE_LOAD ) \
    _APPLY(dxgi,                                         "dxgi"                               , 0                 ) \
    _APPLY(dwrite,                                       "dwrite"                             , 0                 ) \
    _APPLY(dxva2,                                        "dxva2"                              , 0                 ) \
    _APPLY(esent,                                        "esent"                              , 0                 ) \
    _APPLY(uxtheme,                                      "uxtheme"                            , 0                 ) \
    _APPLY(uiautomationcore,                             "uiautomationcore"                   , 0                 ) \
    _APPLY(psapi,                                        "psapi"                              , 0                 ) \
    _APPLY(pdh,                                          "pdh"                                , 0                 ) \
    _APPLY(version,                                      "version"                            , 0                 ) \
    _APPLY(advapi32,                                     "advapi32"                           , 0                 ) \
    _APPLY(bcrypt,                                       "bcrypt"                             , 0                 ) \
    _APPLY(bcryptprimitives,                             "bcryptprimitives"                   , 0                 ) \
    _APPLY(user32,                                       "user32"                             , 0                 ) \
    _APPLY(ws2_32,                                       "ws2_32"                             , 0                 ) \
    _APPLY(shell32,                                      "shell32"                            , 0                 ) \
    _APPLY(shcore,                                       "shcore"                             , 0                 ) \
    _APPLY(shlwapi,                                      "shlwapi"                            , 0                 ) \
    _APPLY(setupapi,                                     "setupapi"                           , 0                 ) \
    _APPLY(ole32,                                        "ole32"                              , 0                 ) \
    _APPLY(iphlpapi,                                     "iphlpapi"                           , 0                 ) \
    _APPLY(userenv,                                      "userenv"                            , 0                 ) \
    _APPLY(mf,                                           "mf"                                 , 0                 ) \
    _APPLY(mfplat,                                       "mfplat"                             , 0                 ) \
    _APPLY(mfreadwrite,                                  "mfreadwrite"                        , 0                 ) \
    _APPLY(mmdevapi,                                     "mmdevapi"                           , 0                 ) \
    _APPLY(ndfapi,                                       "ndfapi"                             , 0                 ) \
    _APPLY(bluetoothapis,                                "bluetoothapis"                      , 0                 ) \
    _APPLY(netapi32,                                     "netapi32"                           , 0                 ) \
    _APPLY(powrprof,                                     "powrprof"                           , 0                 ) \
    _APPLY(propsys,                                      "propsys"                            , 0                 ) \
    _APPLY(wevtapi,                                      "wevtapi"                            , 0                 ) \
    _APPLY(winhttp,                                      "winhttp"                            , 0                 ) \
    _APPLY(winusb,                                       "winusb"                             , 0                 ) \
    _APPLY(zipfldr,                                      "zipfldr"                            , LOAD_AS_DATA_FILE ) \
    _APPLY(api_ms_win_core_handle_l1_1_0,                "api-ms-win-core-handle-l1-1-0"      , 0                 ) \
    _APPLY(api_ms_win_core_realtime_l1_1_1,              "api-ms-win-core-realtime-l1-1-1"    , 0                 ) \
    _APPLY(api_ms_win_core_winrt_l1_1_0,                 "api-ms-win-core-winrt-l1-1-0"       , 0                 ) \
    _APPLY(api_ms_win_core_winrt_string_l1_1_0,          "api-ms-win-core-winrt-string-l1-1-0", 0                 ) \
    _APPLY(api_ms_win_core_winrt_error_l1_1_0,           "api-ms-win-core-winrt-error-l1-1-0" , 0                 ) \
    _APPLY(api_ms_win_core_winrt_error_l1_1_1,           "api-ms-win-core-winrt-error-l1-1-1" , 0                 ) \
    _APPLY(api_ms_win_core_path_l1_1_0,                  "api-ms-win-core-path-l1-1-0"        , 0                 ) \
    _APPLY(api_ms_win_core_synch_l1_2_0,                 "api-ms-win-core-synch-l1-2-0"       , 0                 ) 


//全局可能使用到的函数
#define _YY_APPLY_TO_LATE_BOUND_FUNCTIONS(_APPLY)                                                        \
    _APPLY(NtOpenSection,                                ntdll                                         ) \
    _APPLY(NtCreateFile,                                 ntdll                                         ) \
    _APPLY(NtClose,                                      ntdll                                         ) \
    _APPLY(NtQueryDirectoryFile,                         ntdll                                         ) \
    _APPLY(NtQueryInformationFile,                       ntdll                                         ) \
    _APPLY(NtQuerySystemInformation,                     ntdll                                         ) \
    _APPLY(NtSetInformationFile,                         ntdll                                         ) \
    _APPLY(RtlNtStatusToDosError,                        ntdll                                         ) \
    _APPLY(RtlDetermineDosPathNameType_U,                ntdll                                         ) \
    _APPLY(RtlDosPathNameToNtPathName_U,                 ntdll                                         ) \
    _APPLY(RtlDosPathNameToNtPathName_U_WithStatus,      ntdll                                         ) \
    _APPLY(RtlFreeUnicodeString,                         ntdll                                         ) \
    _APPLY(NtQueryObject,                                ntdll                                         ) \
    _APPLY(NtQueryInformationThread,                     ntdll                                         ) \
    _APPLY(NtSetInformationThread,                       ntdll                                         ) \
    _APPLY(NtQueryInformationProcess,                    ntdll                                         ) \
    _APPLY(NtSetInformationProcess,                      ntdll                                         ) \
    _APPLY(NtDeleteKey,                                  ntdll                                         ) \
    _APPLY(NtCreateKey,                                  ntdll                                         ) \
    _APPLY(NtOpenKey,                                    ntdll                                         ) \
    _APPLY(NtOpenKeyedEvent,                             ntdll                                         ) \
    _APPLY(NtWaitForKeyedEvent,                          ntdll                                         ) \
    _APPLY(NtReleaseKeyedEvent,                          ntdll                                         ) \
    _APPLY(RtlAdjustPrivilege,                           ntdll                                         ) \
    _APPLY(RtlPcToFileHeader,                            ntdll                                         ) \
    _APPLY(LdrAddRefDll,                                 ntdll                                         ) \
    _APPLY(RtlWow64EnableFsRedirectionEx,                ntdll                                         ) \
    _APPLY(LdrLoadDll,                                   ntdll                                         ) \
    _APPLY(RtlDllShutdownInProgress,                     ntdll                                         ) \
    _APPLY(RtlCutoverTimeToSystemTime,                   ntdll                                         ) \
    _APPLY(NtCancelIoFile,                               ntdll                                         ) \
    _APPLY(NtWow64ReadVirtualMemory64,                   ntdll                                         ) \
    _APPLY(RtlValidSid,                                  ntdll                                         ) \
    _APPLY(RtlValidAcl,                                  ntdll                                         ) \
    _APPLY(RtlFirstFreeAce,                              ntdll                                         ) \
    _APPLY(RtlCopySid,                                   ntdll                                         ) \
    _APPLY(AddDllDirectory,                              kernel32                                      ) \
    _APPLY(SystemFunction036,                            advapi32                                      )


#include <sdkddkver.h>
#include <SharedDefs.h>

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#define _WINSOCKAPI_
#define PSAPI_VERSION 1

#if (YY_Thunks_Target < __WindowsNT6)
#define INITKNOWNFOLDERS
#endif

#ifndef __WarningMessage__
#define STRING2(x) #x
#define STRING(x) STRING2(x)
#define __WarningMessage__(msg) __pragma(message (__FILE__ "(" STRING(__LINE__) "): warning Thunks: " # msg))
#endif

#if !defined(__ENABLE_WORKAROUND_1_GetProcAddress_ProcessPrng) && (defined(__ENABLE_WORKAROUND_ALL) || defined(__APPLY_CHROMIUM_WORKAROUNDS))
#define __ENABLE_WORKAROUND_1_GetProcAddress_ProcessPrng 1
#endif

#if !defined(__ENABLE_WORKAROUND_2_UNNAME_OBJECT_DACL) && (defined(__ENABLE_WORKAROUND_ALL) || defined(__APPLY_CHROMIUM_WORKAROUNDS))
#define __ENABLE_WORKAROUND_2_UNNAME_OBJECT_DACL 1
#endif

#ifndef __FALLBACK_PREFIX
#define __FALLBACK_PREFIX
#define __YY_Thunks_libs 0
#else
#define __YY_Thunks_libs 1
#endif

#if !defined(__USING_NTDLL_LIB) && (__YY_Thunks_libs || YY_Thunks_Target >= __WindowsNT10_10240)
// lib模式下必然存在 ntdll.lib，此外最小支持Windows 10时，我们因为强制依赖Windows 10 SDK，所以也必然存在ntdll.lib。
#define __USING_NTDLL_LIB 1
#endif

#define _Disallow_YY_KM_Namespace
#include "km.h"
#include <Shlwapi.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <psapi.h>
#include <winnls.h>

#include <type_traits>

EXTERN_C
BOOLEAN
__stdcall
SystemFunction036(
    _Out_writes_bytes_(RandomBufferLength) PVOID RandomBuffer,
    _In_ ULONG RandomBufferLength
    );

EXTERN_C
BOOLEAN
__stdcall
RtlCutoverTimeToSystemTime(
    PTIME_FIELDS CutoverTime,
    PLARGE_INTEGER SystemTime,
    PLARGE_INTEGER CurrentSystemTime,
    BOOLEAN ThisYear
    );

#include "YY_Thunks.h"

#if (YY_Thunks_Target < __WindowsNT5_2_SP1) && !defined(__Comment_Lib_advapi32)
#define __Comment_Lib_advapi32
#pragma comment(lib, "Advapi32.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_shlwapi)
#define __Comment_Lib_shlwapi
#pragma comment(lib, "Shlwapi.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_ws2_32)
#define __Comment_Lib_ws2_32
#pragma comment(lib, "Ws2_32.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_version)
#define __Comment_Lib_version
#pragma comment(lib, "version.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_ole32)
#define __Comment_Lib_ole32
#pragma comment(lib, "ole32.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_shell32)
#define __Comment_Lib_shell32
#pragma comment(lib, "shell32.lib")
#endif

// PSAPI2Kernel32.def
#if (YY_Thunks_Target < __WindowsNT6_1) && !defined(__Comment_Lib_psapi)
#define __Comment_Lib_psapi
#pragma comment(lib, "psapi.lib")
#endif

#if (YY_Thunks_Target >= __WindowsNT6_3) && !defined(__Comment_Lib_shcore)
#define __Comment_Lib_shcore
#pragma comment(lib, "Shcore.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6_3) && !defined(__Comment_Lib_gdi32)
#define __Comment_Lib_gdi32
#pragma comment(lib, "Gdi32.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT10_10240) && !defined(__Comment_Lib_user32)
#define __Comment_Lib_user32
#pragma comment(lib, "User32.lib")
#endif

#if defined(__USING_NTDLL_LIB)
#pragma comment(lib, "ntdll.lib")
#endif

#include <HookThunk.h>

//展开函数的所有的 声明 以及 try_get_ 函数
#define __DEFINE_THUNK_EXTERN_PREFIX(_PREFIX, _MODULE, _SIZE, _RETURN_, _CONVENTION_, _FUNCTION, ...)                      \
    __APPLY_UNIT_TEST_BOOL(_FUNCTION);                                                                                     \
    EXTERN_C _RETURN_ _CONVENTION_ _CRT_CONCATENATE_(_PREFIX, _FUNCTION)(__VA_ARGS__);                                     \
    static decltype(_CRT_CONCATENATE_(_PREFIX, _FUNCTION))* __cdecl _CRT_CONCATENATE(try_get_, _FUNCTION)() noexcept;      \
    __if_not_exists(_CRT_CONCATENATE(try_get_, _FUNCTION))

#define __DEFINE_THUNK(_MODULE, _SIZE, _RETURN_, _CONVENTION_, _FUNCTION, ...) __DEFINE_THUNK_EXTERN_PREFIX(__FALLBACK_PREFIX, _MODULE, _SIZE, _RETURN_, _CONVENTION_, _FUNCTION, __VA_ARGS__)

#include "Thunks\YY_Thunks_List.hpp"

#undef __DEFINE_THUNK

namespace YY::Thunks::internal
{
    namespace
    {
        inline UINT8 __fastcall BitsCount(ULONG32 _fBitMask)
        {
#if defined(_M_IX86) || defined(_M_AMD64)
            return static_cast<UINT8>(__popcnt(_fBitMask));
#else
            _fBitMask = (_fBitMask & 0x55555555) + ((_fBitMask >> 1) & 0x55555555);
            _fBitMask = (_fBitMask & 0x33333333) + ((_fBitMask >> 2) & 0x33333333);
            _fBitMask = (_fBitMask & 0x0f0f0f0f) + ((_fBitMask >> 4) & 0x0f0f0f0f);
            _fBitMask = (_fBitMask & 0x00ff00ff) + ((_fBitMask >> 8) & 0x00ff00ff);
            _fBitMask = (_fBitMask & 0x0000ffff) + ((_fBitMask >> 16) & 0x0000ffff);
            return static_cast<UINT8>(_fBitMask);
#endif
        }

        inline UINT8 __fastcall BitsCount(ULONG64 _fBitMask)
        {
#if defined(_M_IX86)
            return static_cast<UINT8>(__popcnt(static_cast<ULONG32>(_fBitMask)) + __popcnt(static_cast<ULONG32>(_fBitMask >> 32)));
#elif defined(_M_AMD64)
            return static_cast<UINT8>(__popcnt64(_fBitMask));
#else
            _fBitMask = (_fBitMask & 0x55555555'55555555) + ((_fBitMask >> 1) & 0x55555555'55555555);
            _fBitMask = (_fBitMask & 0x33333333'33333333) + ((_fBitMask >> 2) & 0x33333333'33333333);
            _fBitMask = (_fBitMask & 0x0f0f0f0f'0f0f0f0f) + ((_fBitMask >> 4) & 0x0f0f0f0f'0f0f0f0f);
            _fBitMask = (_fBitMask & 0x00ff00ff'00ff00ff) + ((_fBitMask >> 8) & 0x00ff00ff'00ff00ff);
            _fBitMask = (_fBitMask & 0x0000ffff'0000ffff) + ((_fBitMask >> 16) & 0x0000ffff'0000ffff);
            _fBitMask = (_fBitMask & 0x00000000'ffffffff) + ((_fBitMask >> 32) & 0x00000000'ffffffff);
            return static_cast<UINT8>(_fBitMask);
#endif
        }

        __forceinline constexpr uint64_t __fastcall MakeVersion(_In_ uint16_t _uMajor, _In_ uint16_t _uMinor, uint16_t _uBuild = 0, UINT16 _uRevision = 0)
        {
            uint64_t _uVersion = uint64_t(_uMajor) << 48;
            _uVersion |= uint64_t(_uMinor) << 32;
            _uVersion |= uint64_t(_uBuild) << 16;
            _uVersion |= _uRevision;
            return _uVersion;
        }

#ifdef __YY_Thunks_Unit_Test     
        EXTERN_C uint64_t g_uSystemVersion = 0;
#endif

        __forceinline uint64_t __fastcall GetSystemVersion()
        {
#ifdef __YY_Thunks_Unit_Test
            if (g_uSystemVersion)
                return g_uSystemVersion;
#endif
            const auto _pPeb = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock;
            return internal::MakeVersion(_pPeb->OSMajorVersion, _pPeb->OSMinorVersion, _pPeb->OSBuildNumber);
        }

        const SYSTEM_INFO& GetNativeSystemInfo()
        {
            static SYSTEM_INFO s_SystemInfo;
            // 0： 尚未初始化
            // 1：正在初始化
            // 2：已经初始化完成
            static volatile LONG s_InitOnce;

            auto _nResult = InterlockedCompareExchange(&s_InitOnce, 1, 0);
            if (_nResult == 0)
            {
                // 成功锁定
                ::GetNativeSystemInfo(&s_SystemInfo);
                InterlockedExchange(&s_InitOnce, 2);
            }
            else if (_nResult == 1)
            {
                // 其他线程正在初始化
                do
                {
                    YieldProcessor();

                } while (s_InitOnce == 1);
            }

            return s_SystemInfo;
        }

        _Check_return_
            _Ret_maybenull_
            _Post_writable_byte_size_(_cbBytes)
            static void* __fastcall Alloc(_In_ size_t _cbBytes, DWORD _fFlags = 0)
        {
            return HeapAlloc(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap, _fFlags, _cbBytes);
        }

        _Check_return_
            _Ret_maybenull_
            _Post_writable_byte_size_(_cbBytes)
            static void* __fastcall ReAlloc(_Pre_maybenull_ _Post_invalid_ void* _pAddress, _In_ size_t _cbBytes)
        {
            auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
            if (_pAddress)
            {
                return HeapReAlloc(_hProcessHeap, 0, _pAddress, _cbBytes);
            }
            else
            {
                return HeapAlloc(_hProcessHeap, 0, _cbBytes);
            }
        }

        static void __fastcall Free(_Pre_maybenull_ _Post_invalid_ void* _pAddress)
        {
            if(_pAddress)
                HeapFree(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap, 0, _pAddress);
        }

        template<typename Type, typename... Args>
        _Success_(return != NULL) _Check_return_ _Ret_maybenull_
            _CRTALLOCATOR
            inline Type* __fastcall New(Args&&... args)
        {
            Type* _pType = (Type*)Alloc(sizeof(Type));
            if (_pType)
                new (_pType) Type(std::forward<Args>(args)...);

            return _pType;
        }

        template<typename T>
        inline void __fastcall Delete(_Pre_maybenull_ _Post_invalid_ T* _p)
        {
            if (_p)
            {
                _p->~T();
                Free(_p);
            }
        }

        class CppAlloc
        {
        public:
            _NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
                void* __CRTDECL operator new(
                    size_t _Size
                    )
            {
                return Alloc(_Size);
            }

            _NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
                void* __CRTDECL operator new(
                    size_t _Size,
                    ::std::nothrow_t const&
                    ) noexcept
            {
                return Alloc(_Size);
            }

            _NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
                void* __CRTDECL operator new[](
                    size_t _Size
                    )
            {
                return Alloc(_Size);
            }

            _NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
                void* __CRTDECL operator new[](
                    size_t _Size,
                    ::std::nothrow_t const&
                    ) noexcept
            {
                return Alloc(_Size);
            }

            /// <summary>
            /// placement new
            /// </summary>
            /// <param name="_Size"></param>
            /// <param name="_Block"></param>
            /// <returns></returns>
            void* __CRTDECL operator new(
                size_t _Size,
                void* _Block
                ) noexcept
            {
                return _Block;
            }

            void __CRTDECL operator delete(
                void* _Block
                ) noexcept
            {
                Free(_Block);
            }

            void __CRTDECL operator delete(
                void* _Block,
                ::std::nothrow_t const&
                ) noexcept
            {
                Free(_Block);
            }

            void __CRTDECL operator delete[](
                void* _Block
                ) noexcept
            {
                Free(_Block);
            }

            void __CRTDECL operator delete[](
                void* _Block,
                ::std::nothrow_t const&
                ) noexcept
            {
                Free(_Block);
            }

            void __CRTDECL operator delete(
                void* _Block,
                size_t _Size
                ) noexcept
            {
                Free(_Block);
            }

            void __CRTDECL operator delete[](
                void* _Block,
                size_t _Size
                ) noexcept
            {
                Free(_Block);
            }

            void __CRTDECL operator delete(
                void* _Block,
                void*
                ) noexcept
            {
            }
        };


        //代码块，分割任务
        template<class Callback, typename... Params>
        auto __forceinline Block(Callback&& _Callback, Params&&... args) -> decltype(_Callback(args...))
        {
            return _Callback(args...);
        }

        static DWORD __fastcall NtStatusToDosError(
            _In_ NTSTATUS Status
        )
        {
            if (STATUS_TIMEOUT == Status)
            {
                /*
                https://github.com/Chuyu-Team/YY-Thunks/issues/10

                用户报告，Windows XP 无法转换 STATUS_TIMEOUT。实际结果也是rubin，因此，特殊处理一下。
                */
                return ERROR_TIMEOUT;
            }

#if !defined(__USING_NTDLL_LIB)
            const auto RtlNtStatusToDosError = try_get_RtlNtStatusToDosError();
            if (!RtlNtStatusToDosError)
            {
                //如果没有RtlNtStatusToDosError就直接设置Status代码吧，反正至少比没有错误代码强
                return Status;
            }
#endif
            return RtlNtStatusToDosError(Status);
        }

        static DWORD __fastcall BaseSetLastNTError(
            _In_ NTSTATUS Status
        )
        {
            auto lStatus = NtStatusToDosError(Status);
            SetLastError(lStatus);
            return lStatus;
        }

        static __analysis_noreturn void __fastcall RaiseStatus(NTSTATUS Status)
        {
            RaiseException(Status, EXCEPTION_NONCONTINUABLE, 0, NULL);
        }

        static LARGE_INTEGER* __fastcall BaseFormatTimeOut(LARGE_INTEGER* Timeout, DWORD dwMilliseconds)
        {
            if (dwMilliseconds == INFINITE)
                return nullptr;

            Timeout->QuadPart = -10000ll * dwMilliseconds;

            return Timeout;
        }

        static LSTATUS __fastcall Basep8BitStringToStaticUnicodeString(UNICODE_STRING* pDst, LPCSTR Src)
        {
            const UINT CodePage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

            auto cchDst = MultiByteToWideChar(CodePage, MB_ERR_INVALID_CHARS, Src, -1, pDst->Buffer, pDst->MaximumLength / sizeof(wchar_t));
            if (cchDst <= 0)
            {
                return GetLastError();
            }
            cchDst *= sizeof(wchar_t);
            if (cchDst > MAXUINT16)
            {
                return ERROR_BAD_PATHNAME;
            }

            pDst->Length = static_cast<USHORT>(cchDst);
            return ERROR_SUCCESS;
        }

        static BOOL __fastcall BasepGetVolumeGUIDFromNTName(const UNICODE_STRING* NtName, wchar_t szVolumeGUID[MAX_PATH])
        {
#define __szVolumeMountPointPrefix__ L"\\\\?\\GLOBALROOT"

            //一个设备名称 512 长度够多了吧？
            wchar_t szVolumeMountPoint[512];

            //检查缓冲区是否充足
            auto cbBufferNeed = sizeof(__szVolumeMountPointPrefix__) + NtName->Length;

            if (cbBufferNeed > sizeof(szVolumeMountPoint))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                return FALSE;
            }

            memcpy(szVolumeMountPoint, __szVolumeMountPointPrefix__, sizeof(__szVolumeMountPointPrefix__) - sizeof(__szVolumeMountPointPrefix__[0]));
            memcpy((char*)szVolumeMountPoint + sizeof(__szVolumeMountPointPrefix__) - sizeof(__szVolumeMountPointPrefix__[0]), NtName->Buffer, NtName->Length);

            szVolumeMountPoint[cbBufferNeed / 2 - 1] = L'\0';


            return GetVolumeNameForVolumeMountPointW(szVolumeMountPoint, szVolumeGUID, MAX_PATH);

#undef __szVolumeMountPointPrefix__
        }

        static BOOL __fastcall BasepGetVolumeDosLetterNameFromNTName(const UNICODE_STRING* NtName, wchar_t szVolumeDosLetter[MAX_PATH])
        {
            wchar_t szVolumeName[MAX_PATH];

            if (!BasepGetVolumeGUIDFromNTName(NtName, szVolumeName))
            {
                return FALSE;
            }

            DWORD cchVolumePathName = 0;

            if (!GetVolumePathNamesForVolumeNameW(szVolumeName, szVolumeDosLetter + 4, MAX_PATH - 4, &cchVolumePathName))
            {
                return FALSE;
            }

            szVolumeDosLetter[0] = L'\\';
            szVolumeDosLetter[1] = L'\\';
            szVolumeDosLetter[2] = L'?';
            szVolumeDosLetter[3] = L'\\';

            return TRUE;
        }

        static unsigned __fastcall CharToHex(_In_ wchar_t _ch)
        {
            if (_ch >= L'0' && _ch <= L'9')
            {
                return _ch - L'0';
            }
            else if (_ch >= L'A' && _ch <= L'F')
            {
                return _ch - L'A' + 0xA;
            }
            else if (_ch >= L'a' && _ch <= L'f')
            {
                return _ch - L'a' + 0xA;
            }
            else
            {
                return -1;
            }
        }

        static BOOL __fastcall StringToGuid(_In_z_ const wchar_t* _szInput, _Out_ GUID* _pId)
        {
            *_pId = GUID{};

            if (!_szInput)
                return FALSE;

            if (*_szInput == L'{')
                ++_szInput;

#define _IS_HEX_CHAR(C) (CharToHex(C) != (unsigned)-1)

            if (!(_IS_HEX_CHAR(_szInput[0]) && _IS_HEX_CHAR(_szInput[1]) && _IS_HEX_CHAR(_szInput[2]) && _IS_HEX_CHAR(_szInput[3])
                && _IS_HEX_CHAR(_szInput[4]) && _IS_HEX_CHAR(_szInput[5]) && _IS_HEX_CHAR(_szInput[6]) && _IS_HEX_CHAR(_szInput[7])
                && _szInput[8] == L'-'
                && _IS_HEX_CHAR(_szInput[9]) && _IS_HEX_CHAR(_szInput[10]) && _IS_HEX_CHAR(_szInput[11]) && _IS_HEX_CHAR(_szInput[12])
                && _szInput[13] == L'-'
                && _IS_HEX_CHAR(_szInput[14]) && _IS_HEX_CHAR(_szInput[15]) && _IS_HEX_CHAR(_szInput[16]) && _IS_HEX_CHAR(_szInput[17])
                && _szInput[18] == L'-'
                && _IS_HEX_CHAR(_szInput[19]) && _IS_HEX_CHAR(_szInput[20]) && _IS_HEX_CHAR(_szInput[21]) && _IS_HEX_CHAR(_szInput[22])
                && _szInput[23] == L'-'
                && _IS_HEX_CHAR(_szInput[24]) && _IS_HEX_CHAR(_szInput[25]) && _IS_HEX_CHAR(_szInput[26]) && _IS_HEX_CHAR(_szInput[27])
                && _IS_HEX_CHAR(_szInput[28]) && _IS_HEX_CHAR(_szInput[29]) && _IS_HEX_CHAR(_szInput[30]) && _IS_HEX_CHAR(_szInput[31])
                && _IS_HEX_CHAR(_szInput[32]) && _IS_HEX_CHAR(_szInput[33]) && _IS_HEX_CHAR(_szInput[34]) && _IS_HEX_CHAR(_szInput[35])))
            {
                return FALSE;
            }
#undef _IS_HEX_CHAR

            _pId->Data1 = (CharToHex(_szInput[0]) << 28) | (CharToHex(_szInput[1]) << 24) | (CharToHex(_szInput[2]) << 20) | (CharToHex(_szInput[3]) << 16)
                | (CharToHex(_szInput[4]) << 12) | (CharToHex(_szInput[5]) << 8) | (CharToHex(_szInput[6]) << 4) | (CharToHex(_szInput[7]) << 0);

            _pId->Data2 = (CharToHex(_szInput[9]) << 12) | (CharToHex(_szInput[10]) << 8) | (CharToHex(_szInput[11]) << 4) | (CharToHex(_szInput[12]) << 0);

            _pId->Data3 = (CharToHex(_szInput[14]) << 12) | (CharToHex(_szInput[15]) << 8) | (CharToHex(_szInput[16]) << 4) | (CharToHex(_szInput[17]) << 0);

            _pId->Data4[0] = (CharToHex(_szInput[19]) << 4) | (CharToHex(_szInput[20]) << 0);
            _pId->Data4[1] = (CharToHex(_szInput[21]) << 4) | (CharToHex(_szInput[22]) << 0);
            _pId->Data4[2] = (CharToHex(_szInput[24]) << 4) | (CharToHex(_szInput[25]) << 0);
            _pId->Data4[3] = (CharToHex(_szInput[26]) << 4) | (CharToHex(_szInput[27]) << 0);
            _pId->Data4[4] = (CharToHex(_szInput[28]) << 4) | (CharToHex(_szInput[29]) << 0);
            _pId->Data4[5] = (CharToHex(_szInput[30]) << 4) | (CharToHex(_szInput[31]) << 0);
            _pId->Data4[6] = (CharToHex(_szInput[32]) << 4) | (CharToHex(_szInput[33]) << 0);
            _pId->Data4[7] = (CharToHex(_szInput[34]) << 4) | (CharToHex(_szInput[35]) << 0);
            return TRUE;
        }

        template<typename Char1, typename Char2>
        static bool __fastcall StringStartsWithI(_In_z_ const Char1* _szStr, _In_z_ const Char2* _szStartsWith, _Outptr_opt_result_z_ const Char1** _szEnd = nullptr)
        {
            if (_szEnd)
                *_szEnd = _szStr;

            if (_szStr == (Char1*)_szStartsWith)
                return true;

            if (_szStr == nullptr)
                return false;
            if (_szStartsWith == nullptr)
                return false;

            for (; *_szStartsWith;++_szStr, ++_szStartsWith)
            {
                if (*_szStr == *_szStartsWith)
                {
                    continue;
                }
                else if (__ascii_tolower(*_szStr) == __ascii_tolower(*_szStartsWith))
                {
                    continue;
                }

                return false;
            }
            if (_szEnd)
                *_szEnd = _szStr;
            return true;
        }

        template<typename Char>
        static bool __fastcall StringToUint32(_In_z_ const Char* _szStr, _Out_ DWORD* _puResult, _Outptr_opt_result_z_ Char const** _pszEnd = nullptr)
        {
            auto _szEnd = _szStr;

            if (_pszEnd)
                *_pszEnd = _szEnd;

            *_puResult = 0;

            DWORD64 _uResult64 = 0;
            for (;;++_szEnd)
            {
                if (*_szEnd <= '9' && *_szEnd >= '0')
                {
                    _uResult64 *= 10;
                    _uResult64 += *_szEnd - '0';

                    // 溢出
                    if (_uResult64 > 0xFFFFFFFFull)
                    {
                        return false;
                    }
                }
                else
                {
                    break;
                }
            }

            if (_szStr == _szEnd)
                return false;

            *_puResult = static_cast<DWORD>(_uResult64);
            if (_pszEnd)
                *_pszEnd = _szEnd;
            return true;
        }


        /// <summary>
        /// 计算字符串长度，某些场景我们特意不依赖wcslen之类的，防止发生死锁。
        /// </summary>
        /// <typeparam name="Char"></typeparam>
        /// <param name="_szString"></param>
        /// <param name="_cchMaxLength"></param>
        /// <returns></returns>
        template<typename Char>
        constexpr size_t StringLength(_In_z_ const Char* _szString, size_t _cchMaxLength = -1)
        {
            if (!_szString)
                return 0;

            size_t _cchString = 0;
            for (;_cchMaxLength && *_szString;--_cchMaxLength, ++_szString)
            {
                ++_cchString;
            }

            return _cchString;
        }

        template<typename Char>
        class StringBuffer
        {
        public:
            Char* szBuffer = nullptr;
            size_t uLength = 0;
            size_t uBufferLength = 0;
            bool bCanFree = false;

            constexpr StringBuffer()
                : bCanFree(true)
            {
            }

            constexpr StringBuffer(StringBuffer&& _Other) noexcept
                : szBuffer(_Other.szBuffer)
                , uLength(_Other.uLength)
                , uBufferLength(_Other.uBufferLength)
                , bCanFree(_Other.bCanFree)
            {
                _Other.szBuffer = nullptr;
                _Other.uLength = 0;
                _Other.uBufferLength = 0;
            }


            constexpr StringBuffer(Char* _szBuffer, size_t _uBufferLength)
                : szBuffer(_szBuffer)
                , uLength(0)
                , uBufferLength(_uBufferLength)
            {
                if (uBufferLength)
                {
                    *szBuffer = '\0';
                }
            }

            StringBuffer(const StringBuffer&) = delete;

            ~StringBuffer()
            {
                if (bCanFree)
                {
                    internal::Free(szBuffer);
                }
            }

            template<typename Char2>
            bool __fastcall AppendString(_In_z_ const Char2* _szSrc)
            {
                if (_szSrc == nullptr || *_szSrc == '\0')
                    return true;

                TryBuy(128);

                if (uLength == uBufferLength)
                    return false;

                for (auto _uLengthNew = uLength; _uLengthNew != uBufferLength; ++_uLengthNew, ++_szSrc)
                {
                    szBuffer[_uLengthNew] = *_szSrc;
                    if (*_szSrc == '\0')
                    {
                        uLength = _uLengthNew;
                        return true;
                    }
                    TryBuy();
                }

                szBuffer[uLength] = '\0';
                return false;
            }

            template<typename Char2>
            bool __fastcall AppendChar(_In_z_ Char2 _Ch)
            {
                if (_Ch == '\0')
                    return true;
                TryBuy();
                const auto _uNew = uLength + 1;
                if (_uNew >= uBufferLength)
                    return false;
                szBuffer[uLength] = _Ch;
                szBuffer[_uNew] = '\0';
                uLength = _uNew;
                return true;
            }

            bool __fastcall AppendUint32(_In_ DWORD _uAppdendData)
            {
                TryBuy(32);

                if (uLength == uBufferLength)
                    return false;

                auto _uLengthNew = uLength;
                for(; _uLengthNew != uBufferLength;)
                {
                    const auto _uData = _uAppdendData % 10;
                    _uAppdendData /= 10;

                    szBuffer[_uLengthNew] = static_cast<Char>('0' + _uData);
                    ++_uLengthNew;
                    if (_uAppdendData == 0)
                    {
                        if (_uLengthNew == uBufferLength)
                        {
                            break;
                        }
                        szBuffer[_uLengthNew] = '\0';
                        auto _szStart = szBuffer + uLength;
                        auto _szLast = szBuffer + _uLengthNew;

                        for (; _szStart < _szLast;)
                        {
                            --_szLast;
                            const auto _ch = *_szLast;
                            *_szLast = *_szStart;
                            *_szStart = _ch;

                            ++_szStart;
                        }
                        uLength = _uLengthNew;
                        return true;
                    }
                }

                *szBuffer = '\0';
                return false;
            }

            _Ret_z_ const Char* __fastcall GetC_String() const
            {
                return szBuffer ? szBuffer : (Char*)L"";
            }

            _Ret_maybenull_ Char* __fastcall GetBuffer(size_t _cNewBufferLength)
            {
                // 字符串有一个末尾 0
                ++_cNewBufferLength;
                if (uBufferLength > _cNewBufferLength)
                    return szBuffer;

                if (!bCanFree)
                    return nullptr;

                auto _szNewBuffer = (Char*)internal::ReAlloc(szBuffer, _cNewBufferLength * sizeof(Char));
                if (!_szNewBuffer)
                    return nullptr;

                szBuffer = _szNewBuffer;
                uBufferLength = _cNewBufferLength;
                szBuffer[uLength] = 0;
                return _szNewBuffer;
            }

            void __fastcall SetLength(size_t _cNewLength)
            {
                if (_cNewLength == uLength)
                {
                    return;
                }
                else if (uBufferLength <= _cNewLength)
                {
                    __debugbreak();
                    return;
                }

                uLength = _cNewLength;
                szBuffer[uLength] = '\0';
            }

            void TryBuy(DWORD _uAppdendData = 1)
            {
                if (bCanFree ==false || _uAppdendData == 0)
                    return;

                auto _uNew = uLength + _uAppdendData;
                if (_uNew >= uBufferLength)
                {
                    GetBuffer(max(uBufferLength * 2, _uNew));
                }
            }

            bool __fastcall AppendGUID(_In_ const GUID& _Id)
            {
                TryBuy(36);

                const auto _uLengthNew = uLength + 36;
                if (_uLengthNew >= uBufferLength)
                {
                    return false;
                }

                // C0F8B35B-3CA6-4E57-9B65-B654B33AE583
                auto _szHexChar = "0123456789abcdef";
                auto _pBuffer = szBuffer + uLength;
                *_pBuffer++ = _szHexChar[_Id.Data1 >> 28];
                *_pBuffer++ = _szHexChar[(_Id.Data1 >> 24) & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data1 >> 20) & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data1 >> 16) & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data1 >> 12) & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data1 >> 8) & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data1 >> 4) & 0xF];
                *_pBuffer++ = _szHexChar[_Id.Data1 & 0xF];

                *_pBuffer++ = '-';
                *_pBuffer++ = _szHexChar[(_Id.Data2 >> 12) & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data2 >> 8) & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data2 >> 4) & 0xF];
                *_pBuffer++ = _szHexChar[_Id.Data2 & 0xF];

                *_pBuffer++ = '-';
                *_pBuffer++ = _szHexChar[(_Id.Data3 >> 12) & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data3 >> 8) & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data3 >> 4) & 0xF];
                *_pBuffer++ = _szHexChar[_Id.Data3 & 0xF];

                *_pBuffer++ = '-';
                *_pBuffer++ = _szHexChar[(_Id.Data4[0] >> 4) & 0xF];
                *_pBuffer++ = _szHexChar[_Id.Data4[0] & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data4[1] >> 4) & 0xF];
                *_pBuffer++ = _szHexChar[_Id.Data4[1] & 0xF];

                *_pBuffer++ = '-';
                *_pBuffer++ = _szHexChar[(_Id.Data4[2] >> 4) & 0xF];
                *_pBuffer++ = _szHexChar[_Id.Data4[2] & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data4[3] >> 4) & 0xF];
                *_pBuffer++ = _szHexChar[_Id.Data4[3] & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data4[4] >> 4) & 0xF];
                *_pBuffer++ = _szHexChar[_Id.Data4[4] & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data4[5] >> 4) & 0xF];
                *_pBuffer++ = _szHexChar[_Id.Data4[5] & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data4[6] >> 4) & 0xF];
                *_pBuffer++ = _szHexChar[_Id.Data4[6] & 0xF];
                *_pBuffer++ = _szHexChar[(_Id.Data4[7] >> 4) & 0xF];
                *_pBuffer++ = _szHexChar[_Id.Data4[7] & 0xF];
                *_pBuffer = 0;
                uLength = _uLengthNew;
                return true;
            }
        };

        static LSTATUS Convert(_In_NLS_string_(_cchSrc) LPCWSTR _szSrc, _In_ int _cchSrc, StringBuffer<char>* _pBuffer)
        {
            _pBuffer->SetLength(0);
            if (_cchSrc == 0)
                return ERROR_SUCCESS;

            if (_cchSrc < 0)
            {
                const auto _cchLength = _szSrc ? wcslen(_szSrc) : size_t(0);
                if(_cchLength > MAXINT32)
                    return ERROR_NOT_ENOUGH_MEMORY;

                _cchSrc = static_cast<int>(_cchLength);
                if (_cchSrc == 0)
                    return ERROR_SUCCESS;
            }

            const auto _cchDst = WideCharToMultiByte(CP_ACP, 0, _szSrc, _cchSrc, nullptr, 0, nullptr, nullptr);
            if (_cchDst > 0)
            {
                auto _szDst = _pBuffer->GetBuffer(_cchDst);
                if (!_szDst)
                    return ERROR_NOT_ENOUGH_MEMORY;

                WideCharToMultiByte(CP_ACP, 0, _szSrc, _cchSrc, _szDst, _cchDst, nullptr, nullptr);
                _pBuffer->SetLength(_cchDst);
            }
            return ERROR_SUCCESS;
        }

        static bool __fastcall IsEqualI(const UNICODE_STRING& _Left, const UNICODE_STRING& _Right)
        {
            if (_Left.Length != _Right.Length)
                return false;

            return StringCompareIgnoreCaseByAscii(_Left.Buffer, _Right.Buffer, _Left.Length / 2) == 0;
        }

        static bool __fastcall IsEqual(const UNICODE_STRING& _Left, const UNICODE_STRING& _Right)
        {
            if (_Left.Length != _Right.Length)
                return false;

            return memcmp(_Left.Buffer, _Right.Buffer, _Left.Length) == 0;
        }

        static constexpr UNICODE_STRING __fastcall MakeNtString(_In_z_ const wchar_t* _szString)
        {
            const auto _cbString = StringLength(_szString) * sizeof(_szString[0]);
            UNICODE_STRING _Result = { (USHORT)min(UINT16_MAX, _cbString),  (USHORT)min(UINT16_MAX, _cbString + sizeof(_szString[0])), const_cast<PWSTR>(_szString) };
            return _Result;
        }

        static constexpr ANSI_STRING __fastcall MakeNtString(_In_z_ const char* _szString)
        {
            const auto _cbString = StringLength(_szString) * sizeof(_szString[0]);

            ANSI_STRING _Result = { (USHORT)min(UINT16_MAX, _cbString),  (USHORT)min(UINT16_MAX, _cbString + sizeof(_szString[0])), const_cast<PSTR>(_szString)};
            return _Result;
        }

        template<size_t kLength>
        static constexpr UNICODE_STRING __fastcall MakeStaticUnicodeString(const wchar_t (&_Right)[kLength])
        {
            UNICODE_STRING _Result = { (kLength - 1)* sizeof(_Right[0]), kLength * sizeof(_Right[0]), const_cast<PWSTR>(_Right) };
            return _Result;
        }

        static constexpr bool __fastcall UnicodeStringIsStaticBuffer(const UNICODE_STRING& _szString) noexcept
        {
            return LPBYTE(&_szString) + sizeof(_szString) == LPBYTE(_szString.Buffer);
        }

        static constexpr LSTATUS __fastcall UnicodeStringAllocByteBuffer(UNICODE_STRING& _szString, size_t _cbNewBuffer) noexcept
        {
            if (_szString.MaximumLength >= _cbNewBuffer)
                return ERROR_SUCCESS;

            if (UINT16_MAX < _cbNewBuffer)
                return ERROR_TOO_MANY_NAMES;

            wchar_t* _pBuffer = nullptr;
            if (UnicodeStringIsStaticBuffer(_szString))
            {
                _pBuffer = (wchar_t*)internal::Alloc(_cbNewBuffer);
                if (!_pBuffer)
                {
                    return ERROR_NOT_ENOUGH_MEMORY;
                }

                memcpy(_pBuffer, _szString.Buffer, _szString.Length);
            }
            else
            {
                _pBuffer = (wchar_t*)internal::ReAlloc(_szString.Buffer, _cbNewBuffer);
                if (!_pBuffer)
                {
                    return ERROR_NOT_ENOUGH_MEMORY;
                }
            }

            _szString.Buffer = _pBuffer;
            _szString.MaximumLength = _cbNewBuffer;
            return ERROR_SUCCESS;
        }

        static constexpr void __fastcall UnicodeStringFree(UNICODE_STRING& _szString) noexcept
        {
            if (!UnicodeStringIsStaticBuffer(_szString))
            {
                internal::Free(_szString.Buffer);
                _szString.Buffer = nullptr;
                _szString.Length = 0;
                _szString.MaximumLength = 0;
            }
        }

        static constexpr LSTATUS __fastcall UnicodeStringAppendByte(UNICODE_STRING& _szString, LPCWSTR szAppend, size_t _cbAppend) noexcept
        {
            if (_cbAppend == 0)
                return ERROR_SUCCESS;

            const auto _cbLength = _szString.Length + _cbAppend;
            if (_cbLength > UINT16_MAX)
            {
                return ERROR_TOO_MANY_NAMES;
            }

            if (_cbLength > _szString.MaximumLength)
            {
                auto _cbAlloc = max(_szString.MaximumLength * 2, _cbLength + sizeof(wchar_t));
                if (_cbAlloc > UINT16_MAX)
                    _cbAlloc = _cbLength;

                auto _lStatus = UnicodeStringAllocByteBuffer(_szString, max(_cbAlloc, 128));
                if (_lStatus)
                {
                    return _lStatus;
                }
            }

            memcpy(LPBYTE(_szString.Buffer) + _szString.Length, szAppend, _cbAppend);
            _szString.Length = _cbLength;
            if (_cbLength + sizeof(wchar_t) < _szString.MaximumLength)
            {
                *(wchar_t*)(LPBYTE(_szString.Buffer) + _cbLength) = L'\0';
            }
            return ERROR_SUCCESS;
        }

        static constexpr LSTATUS __fastcall UnicodeStringAppend(UNICODE_STRING& _szString, LPCWSTR szAppend, size_t _cchAppend) noexcept
        {
            return UnicodeStringAppendByte(_szString, szAppend, _cchAppend * sizeof(wchar_t));
        }

        static constexpr LSTATUS __fastcall UnicodeStringAppend(UNICODE_STRING& _szString, wchar_t _ch) noexcept
        {
            return UnicodeStringAppend(_szString, &_ch, 1);
        }

        static constexpr UNICODE_STRING __fastcall UnicodeStringGetDir(LPCWSTR _szPath, size_t _cchPath) noexcept
        {
            auto _szPathEnd = _szPath + _cchPath;
            for (; _szPathEnd != _szPath; )
            {
                --_szPathEnd;
                if (*_szPathEnd == L'\\' || *_szPathEnd == L'/')
                {
                    ++_szPathEnd;
                    break;
                }
            }

            const auto _cbData = LPBYTE(_szPathEnd) - LPBYTE(_szPath);
            return UNICODE_STRING{ USHORT(min(_cbData, UINT16_MAX)), USHORT(min(_cchPath * sizeof(wchar_t), UINT16_MAX)), PWSTR(_szPath)};
        }

        static constexpr UNICODE_STRING __fastcall UnicodeStringGetItem(LPWSTR _szString, LPWSTR _szStringEnd) noexcept
        {
            UNICODE_STRING _Result = { 0, 0, _szString };
            for (; _szString != _szStringEnd; ++_szString)
            {
                if (*_szString == L';')
                {
                    _Result.Length = LPBYTE(_szString) - LPBYTE(_Result.Buffer);
                    _Result.MaximumLength = _Result.Length + sizeof(wchar_t);
                    return _Result;
                }
            }

            _Result.Length = LPBYTE(_szString) - LPBYTE(_Result.Buffer);
            _Result.MaximumLength = _Result.Length;
            return _Result;
        }

        template<size_t _kcchStaticBuffer>
        class UnicodeStringBuffer : public UNICODE_STRING
        {
        public:
            static constexpr auto kcchStaticBuffer = _kcchStaticBuffer;
            wchar_t StaticBuffer[kcchStaticBuffer];

            constexpr UnicodeStringBuffer()
                : UNICODE_STRING{0, _kcchStaticBuffer * sizeof(wchar_t), StaticBuffer }
                , StaticBuffer{}
            {
            }

            ~UnicodeStringBuffer()
            {
                UnicodeStringFree(*this);
            }

            UnicodeStringBuffer(const UnicodeStringBuffer&) = delete;

            LSTATUS __fastcall Append(LPCWSTR szAppend) noexcept
            {
                return UnicodeStringAppend(*this, szAppend, internal::StringLength(szAppend));
            }

            LSTATUS __fastcall Append(LPCWSTR szAppend, size_t _cchAppend) noexcept
            {
                return UnicodeStringAppend(*this, szAppend, _cchAppend);
            }

            LSTATUS __fastcall Append(wchar_t _ch) noexcept
            {
                return UnicodeStringAppend(*this, &_ch, 1);
            }

            LSTATUS __fastcall Append(const UNICODE_STRING& _szAppend) noexcept
            {
                return UnicodeStringAppend(*this, _szAppend.Buffer, _szAppend.Length / sizeof(wchar_t));
            }

            LSTATUS __fastcall AppendPath(_In_z_ LPCWSTR _szAppendPath) noexcept
            {
                for (; *_szAppendPath == L'\\' || *_szAppendPath == L'/'; ++_szAppendPath);

                if (*_szAppendPath == L'\0')
                    return ERROR_SUCCESS;

                auto _cchLength = Length / sizeof(wchar_t);
                if (_cchLength)
                {
                    const auto _ch = Buffer[_cchLength - 1];
                    if (_ch != L'\\' && _ch != L'/')
                    {
                        auto _lStatus = Append(L'\\');
                        if (_lStatus)
                            return _lStatus;
                    }
                }

                return Append(_szAppendPath);
            }

            LPWSTR __fastcall GetByteBuffer(size_t _cbLength) noexcept
            {
                if (UnicodeStringAllocByteBuffer(*this, _cbLength + sizeof(wchar_t)))
                {
                    return nullptr;
                }

                return Buffer;
            }

            LPWSTR __fastcall GetBuffer(size_t _cchLength) noexcept
            {
                return GetByteBuffer(_cchLength * sizeof(wchar_t));
            }

            void __fastcall SetLength(size_t _cchLength) noexcept
            {
                return SetByteLength(_cchLength * sizeof(wchar_t));
            }

            void __fastcall SetByteLength(size_t _cbLength) noexcept
            {
                _ASSERT(_cbLength <= MaximumLength);
                Length = _cbLength;
                if (Length + 2 < MaximumLength)
                {
                    *(wchar_t*)(LPBYTE(Buffer) + Length) = L'\0';
                }
            }

            void __fastcall Empty() noexcept
            {
                SetByteLength(0);
            }

            LPWSTR __fastcall GetAppendBuffer(size_t _cchAppendLength) noexcept
            {
                auto _pBuffer = GetByteBuffer(Length + (_cchAppendLength + 1) * sizeof(wchar_t));
                if (!_pBuffer)
                    return nullptr;

                return LPWSTR(LPBYTE(_pBuffer) + Length);
            }

            void __fastcall SetAppendByteLength(size_t _cbLength) noexcept
            {
                SetByteLength(Length + _cbLength);
            }

            void __fastcall SetAppendLength(size_t _cchLength) noexcept
            {
                SetByteLength(Length + _cchLength * sizeof(wchar_t));
            }
        };
    }

} //namespace YY

#include "ThreadRunner.h"

#define _DEFINE_IAT_SYMBOL_PREFIX(_PREFIX, _FUNCTION, _SIZE) _LCRT_DEFINE_IAT_SYMBOL(_PREFIX ## _FUNCTION, _SIZE)
#define _YY_THUNKS_DEFINE_RUST_RAW_DYLIB_IAT_SYMBOL_PREFIX(_PREFIX, _FUNCTION, _SIZE) _YY_THUNKS_DEFINE_RUST_RAW_DYLIB_IAT_SYMBOL(_FUNCTION, _SIZE, _PREFIX ## _FUNCTION)

//导入实际的实现
#define YY_Thunks_Implemented
#define __DEFINE_THUNK_IMP_PREFIX(_PREFIX, _MODULE, _SIZE, _RETURN_, _CONVENTION_, _FUNCTION, ...)                 \
    static decltype(_CRT_CONCATENATE_(_PREFIX, _FUNCTION))* __cdecl _CRT_CONCATENATE(try_get_, _FUNCTION)() noexcept       \
    {                                                                                          \
        __CHECK_UNIT_TEST_BOOL(_FUNCTION);                                                     \
        __declspec(allocate(".YYThr$AAA")) static void* _CRT_CONCATENATE(pInit_ ,_FUNCTION) =  \
              reinterpret_cast<void*>(&_CRT_CONCATENATE(try_get_, _FUNCTION));                 \
        /*为了避免编译器将 YYThr$AAA 节优化掉*/                                                \
        __foreinclude(_CRT_CONCATENATE(pInit_ ,_FUNCTION));                                    \
        __declspec(allocate(".YYThu$AAB")) static void* _CRT_CONCATENATE(pFun_, _FUNCTION);    \
        static const ProcInfo _ProcInfo =                                                      \
        {                                                                                      \
            _CRT_STRINGIZE(_FUNCTION),                                                         \
            &_CRT_CONCATENATE(try_get_module_, _MODULE),                                       \
__if_exists(YY::Thunks::Fallback::_CRT_CONCATENATE(try_get_, _FUNCTION))                       \
{                                                                                              \
            &YY::Thunks::Fallback::_CRT_CONCATENATE(try_get_, _FUNCTION)                       \
}                                                                                              \
        };                                                                                     \
        return reinterpret_cast<decltype(_CRT_CONCATENATE_(_PREFIX, _FUNCTION))*>(try_get_function(                        \
        &_CRT_CONCATENATE(pFun_ ,_FUNCTION),                                                   \
        _ProcInfo));                                                                           \
    }                                                                                          \
    _DEFINE_IAT_SYMBOL_PREFIX(_PREFIX, _FUNCTION, _SIZE);                                                 \
    _YY_THUNKS_DEFINE_RUST_RAW_DYLIB_IAT_SYMBOL_PREFIX(_PREFIX, _FUNCTION, _SIZE);                             \
    EXTERN_C _RETURN_ _CONVENTION_ _CRT_CONCATENATE_(_PREFIX, _FUNCTION)(__VA_ARGS__)

#define __DEFINE_THUNK(_MODULE, _SIZE, _RETURN_, _CONVENTION_, _FUNCTION, ...) __DEFINE_THUNK_IMP_PREFIX(__FALLBACK_PREFIX, _MODULE, _SIZE, _RETURN_, _CONVENTION_, _FUNCTION, __VA_ARGS__)

#include "YY_Thunks_List.hpp"

#undef __DEFINE_THUNK
#undef YY_Thunks_Implemented

static HMODULE __fastcall try_get_module(volatile HMODULE* pModule, const wchar_t* module_name, int Flags) noexcept
{
    // First check to see if we've cached the module handle:
    if (HMODULE const cached_handle = __crt_interlocked_read_pointer(pModule))
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
    HMODULE new_handle = NULL;

    if (__pfnYY_Thunks_CustomLoadLibrary)
    {
        new_handle = __pfnYY_Thunks_CustomLoadLibrary(module_name, Flags);
    }

    if (new_handle)
    {
        // 使用 CustomLoadLibrary的结果
        if (new_handle == INVALID_HANDLE_VALUE)
            new_handle = nullptr;
    }
    else if (Flags & USING_GET_MODULE_HANDLE)
    {
        new_handle = GetModuleHandleW(module_name);
    }
    else
    {
        // 我们不能直接使用 LoadLibraryExW，因为它可能被Thunk。
        __if_exists(YY::Thunks::try_get_LoadLibraryExW)
        {
            const auto LoadLibraryExW = YY::Thunks::try_get_LoadLibraryExW();
            if (!LoadLibraryExW)
                return nullptr;
        }

        if (Flags & LOAD_AS_DATA_FILE)
        {
            new_handle = LoadLibraryExW(module_name, NULL, LOAD_LIBRARY_AS_DATAFILE);
        }
        else if (Flags & USING_UNSAFE_LOAD)
        {
            new_handle = LoadLibraryExW(module_name, nullptr, 0);
        }
        else
        {
            // 使用DLL安全加载
#if (YY_Thunks_Target < __WindowsNT6_2)
            if (!try_get_AddDllDirectory())
            {
#if !defined(__USING_NTDLL_LIB)
                const auto LdrLoadDll = try_get_LdrLoadDll();
                if (!LdrLoadDll)
                    return nullptr;
#endif
                wchar_t szFilePathBuffer[MAX_PATH] = {};
                const auto _cchSystemPath = GetSystemDirectoryW(szFilePathBuffer, _countof(szFilePathBuffer));
                if (_cchSystemPath == 0 || _cchSystemPath >= _countof(szFilePathBuffer))
                {
                    // 回落普通加载，按理说 GetSystemDirectoryW不应该发生这样的失败。
                    new_handle = LoadLibraryExW(module_name, nullptr, 0);
                }
                else
                {
                    auto _sModuleName = YY::Thunks::internal::MakeNtString(module_name);
                    LdrLoadDll(szFilePathBuffer, nullptr, &_sModuleName, &new_handle);
                }
            }
            else
#endif
            {
                new_handle = LoadLibraryExW(module_name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
            }
        }
    }

    if (!new_handle)
    {
        if (HMODULE const cached_handle = __crt_interlocked_exchange_pointer(pModule, INVALID_HANDLE_VALUE))
        {
            _ASSERTE(cached_handle == INVALID_HANDLE_VALUE);
        }

        return nullptr;
    }

    // Swap the new handle into the cache.  If the cache no longer contained a
    // null handle, then some other thread loaded the module and cached the
    // handle while we were doing the same.  In that case, we free the handle
    // once to maintain the reference count:
    if (HMODULE const cached_handle = __crt_interlocked_exchange_pointer(pModule, new_handle))
    {
        _ASSERTE(cached_handle == new_handle);
        FreeLibrary(new_handle);
    }

    return new_handle;
}

static __forceinline void* __fastcall try_get_proc_address_from_dll(
    const ProcInfo& _ProcInfo
    ) noexcept
{
    HMODULE const module_handle = _ProcInfo.pfnGetModule();
    if (!module_handle)
    {
        return nullptr;
    }

    // 无法直接调用GetProcAddress，因为GetProcAddress可能被Thunk
    // 我们需要严格判断，避免发生死锁。
#if defined(__USING_NTDLL_LIB)
    void* _pProc = nullptr;
    if (uintptr_t(_ProcInfo.szProcName) > UINT16_MAX)
    {
        ANSI_STRING _sFunctionName = YY::Thunks::internal::MakeNtString(_ProcInfo.szProcName);
        LdrGetProcedureAddress(module_handle, &_sFunctionName, 0, &_pProc);
    }
    else
    {
        LdrGetProcedureAddress(module_handle, nullptr, (WORD)uintptr_t(_ProcInfo.szProcName), &_pProc);
    }
    return _pProc;
#else // !defined(__USING_NTDLL_LIB)
    __if_exists(YY::Thunks::try_get_GetProcAddress)
    {
        const auto GetProcAddress = YY::Thunks::try_get_GetProcAddress();
    }

    return reinterpret_cast<void*>(GetProcAddress(module_handle, _ProcInfo.szProcName));
#endif // defined(__USING_NTDLL_LIB)
}

static __forceinline void* __fastcall try_get_proc_address_from_first_available_module(
    const ProcInfo& _ProcInfo
    ) noexcept
{
    if (_ProcInfo.pfnCustomGetProcAddress)
    {
        return _ProcInfo.pfnCustomGetProcAddress(_ProcInfo);
    }

    return try_get_proc_address_from_dll(_ProcInfo);
}
