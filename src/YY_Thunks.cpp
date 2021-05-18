




#define _YY_APPLY_TO_LATE_BOUND_MODULES(_APPLY)                                                                     \
    _APPLY(ntdll,                                        "ntdll"                              , USING_UNSAFE_LOAD ) \
    _APPLY(kernel32,                                     "kernel32"                           , USING_UNSAFE_LOAD ) \
    _APPLY(psapi,                                        "psapi"                              , 0                 ) \
    _APPLY(version,                                      "version"                            , 0                 ) \
	_APPLY(advapi32,                                     "advapi32"                           , 0                 ) \
    _APPLY(user32,                                       "user32"                             , 0                 ) \
    _APPLY(ws2_32,                                       "ws2_32"                             , 0                 ) \
    _APPLY(shell32,                                      "shell32"                            , 0                 ) \
    _APPLY(shcore,                                       "shcore"                             , 0                 ) \
    _APPLY(shlwapi,                                      "shlwapi"                            , 0                 ) \
    _APPLY(setupapi,                                     "setupapi"                           , 0                 ) \
    _APPLY(api_ms_win_core_winrt_l1_1_0,                 "api-ms-win-core-winrt-l1-1-0"       , 0                 ) \
    _APPLY(api_ms_win_core_winrt_string_l1_1_0,          "api-ms-win-core-winrt-string-l1-1-0", 0                 ) \
    _APPLY(api_ms_win_core_winrt_error_l1_1_0,           "api-ms-win-core-winrt-error-l1-1-0" , 0                 ) 


//全局可能使用到的函数
#define _YY_APPLY_TO_LATE_BOUND_FUNCTIONS(_APPLY)                                                        \
    _APPLY(NtCreateFile,                                 ntdll                                         ) \
    _APPLY(NtClose,                                      ntdll                                         ) \
    _APPLY(NtQueryDirectoryFile,                         ntdll                                         ) \
    _APPLY(NtQueryInformationFile,                       ntdll                                         ) \
    _APPLY(NtSetInformationFile,                         ntdll                                         ) \
    _APPLY(RtlNtStatusToDosError,                        ntdll                                         ) \
    _APPLY(RtlDetermineDosPathNameType_U,                ntdll                                         ) \
    _APPLY(RtlDosPathNameToNtPathName_U,                 ntdll                                         ) \
    _APPLY(RtlDosPathNameToNtPathName_U_WithStatus,      ntdll                                         ) \
    _APPLY(RtlFreeUnicodeString,                         ntdll                                         ) \
    _APPLY(NtQueryObject,                                ntdll                                         ) \
    _APPLY(NtQueryInformationThread,                     ntdll                                         ) \
    _APPLY(NtQueryInformationProcess,                    ntdll                                         ) \
    _APPLY(NtOpenKeyedEvent,                             ntdll                                         ) \
    _APPLY(NtWaitForKeyedEvent,                          ntdll                                         ) \
    _APPLY(NtReleaseKeyedEvent,                          ntdll                                         ) \
    _APPLY(RtlAdjustPrivilege,                           ntdll                                         ) \
    _APPLY(RtlPcToFileHeader,                            ntdll                                         ) \
    _APPLY(LdrAddRefDll,                                 ntdll                                         ) \
    _APPLY(RtlWow64EnableFsRedirectionEx,                ntdll                                         ) \
    _APPLY(LdrLoadDll,                                   ntdll                                         ) \
    _APPLY(RtlDllShutdownInProgress,                     ntdll                                         ) \
    _APPLY(AddDllDirectory,                              kernel32                                      )



#include <sdkddkver.h>

#ifndef YY_Thunks_Support_Version
#define YY_Thunks_Support_Version WDK_NTDDI_VERSION
#endif

#define _WINSOCKAPI_
#define PSAPI_VERSION 1

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
#define INITKNOWNFOLDERS
#endif

#define _Disallow_YY_KM_Namespace
#include "km.h"
#include <Shlwapi.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <psapi.h>
#include <winnls.h>

#include "YY_Thunks.h"

#if (YY_Thunks_Support_Version < NTDDI_WS03SP1)
#pragma comment(lib, "Advapi32.lib")
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN7)
#pragma comment(lib, "psapi.lib")
#endif

#if (YY_Thunks_Support_Version >= NTDDI_WINBLUE)
#pragma comment(lib, "Shcore.lib")
#endif

#if (YY_Thunks_Support_Version < NTDDI_WINBLUE)
#pragma comment(lib, "Gdi32.lib")
#endif

//展开函数的所有的 声明 以及 try_get_ 函数
#define YY_Thunks_Defined
#define __YY_Thunks_Expand_Function(_MODULE, _FUNCTION, _SIZE)                                 \
	static decltype(_FUNCTION)* __cdecl _CRT_CONCATENATE(try_get_, _FUNCTION)() noexcept       \
	{                                                                                          \
        __declspec(allocate(".YYThr$AAA")) static void* _CRT_CONCATENATE(pInit_ ,_FUNCTION) =  \
              reinterpret_cast<void*>(&_CRT_CONCATENATE(try_get_, _FUNCTION));                 \
        /* In order to avoid the compiler optimize section YYThr$AAA out */                    \
        __foreinclude(_CRT_CONCATENATE(pInit_ ,_FUNCTION));                                    \
		__declspec(allocate(".YYThu$AAB")) static void* _CRT_CONCATENATE(pFun_, _FUNCTION);    \
		return reinterpret_cast<decltype(_FUNCTION)*>(try_get_function(                        \
		&_CRT_CONCATENATE(pFun_ ,_FUNCTION),                                                   \
		_CRT_STRINGIZE(_FUNCTION),                                                             \
        &_CRT_CONCATENATE(try_get_module_, _MODULE)));                                         \
	}
#include "YY_Thunks_List.hpp"

#undef __YY_Thunks_Expand_Function
#undef YY_Thunks_Defined


namespace YY
{
	namespace Thunks
	{
		namespace internal
		{
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
				else if (auto pRtlNtStatusToDosError = try_get_RtlNtStatusToDosError())
				{
					return pRtlNtStatusToDosError(Status);
				}
				else
				{
					//如果没有RtlNtStatusToDosError就直接设置Status代码吧，反正至少比没有错误代码强
					return Status;
				}

			}

			static DWORD __fastcall BaseSetLastNTError(
				_In_ NTSTATUS Status
				)
			{
				auto lStatus = NtStatusToDosError(Status);
				SetLastError(lStatus);
				return lStatus;
			}

			static void __fastcall RaiseStatus(NTSTATUS Status)
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
				if (cchDst == 0)
				{
					return GetLastError();
				}

				pDst->Length = cchDst * sizeof(wchar_t);

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

		}
	}//namespace Thunks

} //namespace YY

//导入实际的实现
#define __YY_Thunks_Expand_Function(_MODULE, _FUNCTION, _SIZE) _LCRT_DEFINE_IAT_SYMBOL(_FUNCTION, _SIZE)
#include "YY_Thunks_List.hpp"