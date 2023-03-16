
#define _YY_APPLY_TO_LATE_BOUND_MODULES(_APPLY)                                                                     \
    _APPLY(ntdll,                                        "ntdll"                              , USING_UNSAFE_LOAD ) \
    _APPLY(kernel32,                                     "kernel32"                           , USING_UNSAFE_LOAD ) \
    _APPLY(psapi,                                        "psapi"                              , 0                 ) \
    _APPLY(pdh,                                          "pdh"                                , 0                 ) \
    _APPLY(version,                                      "version"                            , 0                 ) \
	_APPLY(advapi32,                                     "advapi32"                           , 0                 ) \
	_APPLY(bcrypt,                                       "bcrypt"                             , 0                 ) \
    _APPLY(user32,                                       "user32"                             , 0                 ) \
    _APPLY(ws2_32,                                       "ws2_32"                             , 0                 ) \
    _APPLY(shell32,                                      "shell32"                            , 0                 ) \
    _APPLY(shcore,                                       "shcore"                             , 0                 ) \
    _APPLY(shlwapi,                                      "shlwapi"                            , 0                 ) \
    _APPLY(setupapi,                                     "setupapi"                           , 0                 ) \
    _APPLY(ole32,                                        "ole32"                              , 0                 ) \
    _APPLY(iphlpapi,                                     "iphlpapi"                           , 0                 ) \
    _APPLY(userenv,                                      "userenv"                            , 0                 ) \
    _APPLY(mfplat,                                       "mfplat"                             , 0                 ) \
    _APPLY(bluetoothapis,                                "bluetoothapis"                      , 0                 ) \
    _APPLY(netapi32,                                     "netapi32"                           , 0                 ) \
    _APPLY(powrprof,                                     "powrprof"                           , 0                 ) \
    _APPLY(api_ms_win_core_winrt_l1_1_0,                 "api-ms-win-core-winrt-l1-1-0"       , 0                 ) \
    _APPLY(api_ms_win_core_winrt_string_l1_1_0,          "api-ms-win-core-winrt-string-l1-1-0", 0                 ) \
    _APPLY(api_ms_win_core_winrt_error_l1_1_0,           "api-ms-win-core-winrt-error-l1-1-0" , 0                 ) \
    _APPLY(api_ms_win_core_path_l1_1_0,                  "api-ms-win-core-path-l1-1-0"        , 0                 ) \
    _APPLY(api_ms_win_core_synch_l1_2_0,                 "api-ms-win-core-synch-l1-2-0"       , 0                 ) 


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
    _APPLY(NtSetInformationThread,                       ntdll                                         ) \
    _APPLY(NtQueryInformationProcess,                    ntdll                                         ) \
    _APPLY(NtSetInformationProcess,                      ntdll                                         ) \
    _APPLY(NtOpenKeyedEvent,                             ntdll                                         ) \
    _APPLY(NtWaitForKeyedEvent,                          ntdll                                         ) \
    _APPLY(NtReleaseKeyedEvent,                          ntdll                                         ) \
    _APPLY(RtlAdjustPrivilege,                           ntdll                                         ) \
    _APPLY(RtlPcToFileHeader,                            ntdll                                         ) \
    _APPLY(LdrAddRefDll,                                 ntdll                                         ) \
    _APPLY(RtlWow64EnableFsRedirectionEx,                ntdll                                         ) \
    _APPLY(LdrLoadDll,                                   ntdll                                         ) \
    _APPLY(RtlDllShutdownInProgress,                     ntdll                                         ) \
    _APPLY(AddDllDirectory,                              kernel32                                      ) \
    _APPLY(SystemFunction036,                            advapi32                                      )


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

EXTERN_C
BOOLEAN
__stdcall
SystemFunction036(
    _Out_writes_bytes_(RandomBufferLength) PVOID RandomBuffer,
    _In_ ULONG RandomBufferLength
    );

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

#if (YY_Thunks_Support_Version < NTDDI_WIN10)
#pragma comment(lib, "User32.lib")
#endif

//展开函数的所有的 声明 以及 try_get_ 函数
#define __DEFINE_THUNK(_MODULE, _SIZE, _RETURN_, _CONVENTION_, _FUNCTION, ...)                 \
    __APPLY_UNIT_TEST_BOOL(_FUNCTION);                                                         \
    EXTERN_C _RETURN_ _CONVENTION_ _FUNCTION(__VA_ARGS__);                                     \
	static decltype(_FUNCTION)* __cdecl _CRT_CONCATENATE(try_get_, _FUNCTION)() noexcept       \
	{                                                                                          \
        __CHECK_UNIT_TEST_BOOL(_FUNCTION);                                                     \
        __declspec(allocate(".YYThr$AAA")) static void* _CRT_CONCATENATE(pInit_ ,_FUNCTION) =  \
              reinterpret_cast<void*>(&_CRT_CONCATENATE(try_get_, _FUNCTION));                 \
        /*为了避免编译器将 YYThr$AAA 节优化掉*/                                                \
        __foreinclude(_CRT_CONCATENATE(pInit_ ,_FUNCTION));                                    \
		__declspec(allocate(".YYThu$AAB")) static void* _CRT_CONCATENATE(pFun_, _FUNCTION);    \
		return reinterpret_cast<decltype(_FUNCTION)*>(try_get_function(                        \
		&_CRT_CONCATENATE(pFun_ ,_FUNCTION),                                                   \
		_CRT_STRINGIZE(_FUNCTION),                                                             \
        &_CRT_CONCATENATE(try_get_module_, _MODULE)));                                         \
	}                                                                                          \
	__if_not_exists(_CRT_CONCATENATE(try_get_, _FUNCTION))


#include "Thunks\YY_Thunks_List.hpp"

#undef __DEFINE_THUNK

namespace YY
{
	namespace Thunks
	{
		namespace internal
		{
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
		}
	}//namespace Thunks

} //namespace YY

#include "ThreadRunner.h"

//导入实际的实现
#define YY_Thunks_Implemented
#define __DEFINE_THUNK(_MODULE, _SIZE, _RETURN_, _CONVENTION_, _FUNCTION, ...)     \
    _LCRT_DEFINE_IAT_SYMBOL(_FUNCTION, _SIZE);                                     \
    EXTERN_C _RETURN_ _CONVENTION_ _FUNCTION(__VA_ARGS__)

#include "YY_Thunks_List.hpp"

#undef __DEFINE_THUNK
#undef YY_Thunks_Implemented