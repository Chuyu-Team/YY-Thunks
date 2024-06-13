#include "pch.h"
#include "CppUnitTest.h"
#include <set>
#include <atlstr.h>

#pragma push_macro("InterlockedCompareExchange64")
#undef InterlockedCompareExchange64

LSTATUS RunCmd(LPCWSTR FilePath, CString CmdString, CString* pOutString, CString _szCurrentDirectory)
{
    SECURITY_ATTRIBUTES sa;
    HANDLE hRead, hWrite;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
    {
        return GetLastError();
    }

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;

    GetStartupInfo(&si);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    //关键步骤，CreateProcess函数参数意义请查阅MSDN
    //auto TT= EXECDOSCMD.GetBuffer();

    if (_szCurrentDirectory.IsEmpty())
    {
        _szCurrentDirectory.ReleaseBufferSetLength(GetSystemDirectoryW(_szCurrentDirectory.GetBuffer(MAX_PATH), MAX_PATH));
    }

    if (!CreateProcessW(FilePath, CmdString.GetBuffer(), NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT, NULL, _szCurrentDirectory.GetString(), &si, &pi))
    {
        return GetLastError();
    }

    CloseHandle(hWrite);

    DWORD bytesRead;

    CStringA OutString;
    //OutString.reserve(1024);

    //OutString.GetBuffer(1024);
    while (ReadFile(hRead, OutString.GetBuffer(OutString.GetLength() + 1024) + OutString.GetLength(), 1024, &bytesRead, NULL) && bytesRead)
    {
        OutString.ReleaseBufferSetLength(OutString.GetLength() + bytesRead);

        //OutString._Mylast() += bytesRead;
        //OutString.reserve(OutString.size() + 1024);


        //buffer中就是执行的结果，可以保存到文本，也可以直接输出
        //TRACE(buffer);
        //等待10毫秒

        Sleep(5);

    }

    CloseHandle(hRead);

    WaitForSingleObject(pi.hProcess, INFINITE);

    LSTATUS lStatus = ERROR_INVALID_FUNCTION;

    GetExitCodeProcess(pi.hProcess, (LPDWORD)&lStatus);


    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    if (pOutString)
    {
        const auto _iUtf8Length = OutString.GetLength();
        auto _iUtf16Length = MultiByteToWideChar(CP_UTF8, 0, OutString.GetString(), _iUtf8Length, pOutString->GetBuffer(_iUtf8Length), _iUtf8Length);
        pOutString->ReleaseBufferSetLength(_iUtf16Length);
    }
    return lStatus;
}

LSTATUS CreateFileByData(LPCWSTR FilePath, const void* Data, DWORD ccbData)
{
    if (Data == NULL)
        return ERROR_DATABASE_FULL;

    DWORD FileAttr = GetFileAttributes(FilePath);

    if (FileAttr != INVALID_FILE_ATTRIBUTES && (FileAttr & FILE_ATTRIBUTE_READONLY))
    {
        SetFileAttributes(FilePath, FileAttr & (-1 ^ FILE_ATTRIBUTE_READONLY));
    }

    LSTATUS lStatus = ERROR_SUCCESS;

    auto thFile = CreateFile(FilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, 0);

    if (thFile != INVALID_HANDLE_VALUE)
    {
        if (!WriteFile(thFile, Data, ccbData, &ccbData, NULL))
        {
            lStatus = GetLastError();
        }

        CloseHandle(thFile);
    }
    else
    {
        lStatus = GetLastError();
    }

    if (FileAttr != INVALID_FILE_ATTRIBUTES)
    {
        SetFileAttributes(FilePath, FileAttr);
    }


    return lStatus;
}


CString RunMSBuildTest(
    LPCWSTR szPlatform,
    LPCWSTR szConfiguration,
    CStringW szSymbolsTestCppRootPath,
    LPCWSTR BuildProperty = nullptr
)
{
    if (szSymbolsTestCppRootPath.IsEmpty())
        return CString();

    if (szSymbolsTestCppRootPath[szSymbolsTestCppRootPath.GetLength() - 1] != L'\\')
        szSymbolsTestCppRootPath += L'\\';

    CString Cmd;

    Cmd.Format(L"\"%s\" \"%s\" -t:Rebuild \"-p:Configuration=%s;Platform=%s;SymbolsTestCppRootPath=%s",
        MSBuildBinPath LR"(MSBuild.exe)",
        SymbolBuildTestPath LR"(Example\SymbolBuildTest.vcxproj)",
        szConfiguration,
        szPlatform,
        szSymbolsTestCppRootPath.GetString());

    if (BuildProperty && *BuildProperty)
    {
        Cmd += L';';
        Cmd += BuildProperty;
    }

    if (Cmd[Cmd.GetLength() - 1] == L'\\')
        Cmd += L'\\';
    Cmd += L'\"';
    CString OutString;

    auto lStatus = RunCmd(nullptr, Cmd, &OutString, CStringW());



    OutString.Insert(0, (wchar_t)0xfeff);


    auto BuildLog = szSymbolsTestCppRootPath + L"Build.log";

    CreateFileByData(BuildLog, OutString.GetString(), OutString.GetLength() * sizeof(OutString[0]));

    Assert::AreEqual(lStatus, ERROR_SUCCESS, BuildLog);

    return OutString;

}

namespace YY
{
	namespace Thunks
	{
		//某些头文件在全局有重载，所以我们复制一份，让符号检测可以正常的工作。
		namespace TopFix
		{
			extern "C" {
				WINPATHCCHAPI
				BOOL
				APIENTRY
				PathIsUNCEx(
					_In_ PCWSTR pszPath,
					_Outptr_opt_ PCWSTR* ppszServer
					);


				WINPATHCCHAPI
				BOOL
				APIENTRY
				PathCchIsRoot(
					_In_opt_ PCWSTR pszPath
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchAddBackslashEx(
					_Inout_updates_(cchPath) PWSTR pszPath,
					_In_ size_t cchPath,
					_Outptr_opt_result_buffer_(*pcchRemaining) PWSTR* ppszEnd,
					_Out_opt_ size_t* pcchRemaining
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchAddBackslash(
					_Inout_updates_(cchPath) PWSTR pszPath,
					_In_ size_t cchPath
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchRemoveBackslashEx(
					_Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
					_In_ size_t cchPath,
					_Outptr_opt_result_buffer_(*pcchRemaining) PWSTR* ppszEnd,
					_Out_opt_ size_t* pcchRemaining
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchRemoveBackslash(
					_Inout_updates_(cchPath) PWSTR pszPath,
					_In_ size_t cchPath
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchSkipRoot(
					_In_ PCWSTR pszPath,
					_Outptr_ PCWSTR* ppszRootEnd
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchStripToRoot(
					_Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
					_In_ size_t cchPath
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchRemoveFileSpec(
					_Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
					_In_ size_t cchPath
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchFindExtension(
					_In_reads_(_Inexpressible_(cchPath)) PCWSTR pszPath,
					_In_ size_t cchPath,
					_Outptr_ PCWSTR* ppszExt
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchAddExtension(
					_Inout_updates_(cchPath) PWSTR pszPath,
					_In_ size_t cchPath,
					_In_ PCWSTR pszExt
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchRenameExtension(
					_Inout_updates_(cchPath) PWSTR pszPath,
					_In_ size_t cchPath,
					_In_ PCWSTR pszExt
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchRemoveExtension(
					_Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
					_In_ size_t cchPath
					);


				/* PATHCCH_OPTIONS */WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchCanonicalizeEx(
					_Out_writes_(cchPathOut) PWSTR pszPathOut,
					_In_ size_t cchPathOut,
					_In_ PCWSTR pszPathIn,
					_In_ ULONG dwFlags
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchCanonicalize(
					_Out_writes_(cchPathOut) PWSTR pszPathOut,
					_In_ size_t cchPathOut,
					_In_ PCWSTR pszPathIn
					);


				/* PATHCCH_OPTIONS */WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchCombineEx(
					_Out_writes_(cchPathOut) PWSTR pszPathOut,
					_In_ size_t cchPathOut,
					_In_opt_ PCWSTR pszPathIn,
					_In_opt_ PCWSTR pszMore,
					_In_ ULONG dwFlags
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchCombine(
					_Out_writes_(cchPathOut) PWSTR pszPathOut,
					_In_ size_t cchPathOut,
					_In_opt_ PCWSTR pszPathIn,
					_In_opt_ PCWSTR pszMore
					);


				/* PATHCCH_OPTIONS */WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchAppendEx(
					_Inout_updates_(cchPath) PWSTR pszPath,
					_In_ size_t cchPath,
					_In_opt_ PCWSTR pszMore,
					_In_ ULONG dwFlags
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchAppend(
					_Inout_updates_(cchPath) PWSTR pszPath,
					_In_ size_t cchPath,
					_In_opt_ PCWSTR pszMore
					);


				WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathCchStripPrefix(
					_Inout_updates_(cchPath) PWSTR pszPath,
					_In_ size_t cchPath
					);


				/* PATHCCH_OPTIONS */WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathAllocCombine(
					_In_opt_ PCWSTR pszPathIn,
					_In_opt_ PCWSTR pszMore,
					_In_ ULONG dwFlags,
					_Outptr_ PWSTR* ppszPathOut
					);


				/* PATHCCH_OPTIONS */WINPATHCCHAPI
				HRESULT
				APIENTRY
				PathAllocCanonicalize(
					_In_ PCWSTR pszPathIn,
					_In_ ULONG dwFlags,
					_Outptr_ PWSTR* ppszPathOut
					);

				DECLSPEC_IMPORT
				LONG64
				WINAPI
				InterlockedCompareExchange64(
					_Inout_ _Interlocked_operand_ LONG64 volatile *Destination,
					_In_ LONG64 ExChange,
					_In_ LONG64 Comperand
					);

			}
		}
	}
}


#ifdef __DEFINE_THUNK
#undef __DEFINE_THUNK
#endif

#pragma section("TMP$__a", read)
#pragma section("TMP$__z", read)

#pragma comment(linker, "/merge:TMP=.rdata")

__declspec(selectany) __declspec(allocate("TMP$__a")) LPCSTR FirstFunctionName[1] = {};
__declspec(selectany) __declspec(allocate("TMP$__z")) LPCSTR LastFunctionName[1] = {};


#define __DEFINE_THUNK(_MODULE, _SIZE, _RETURN_, _CONVENTION_, _FUNCTION, ...)     \
__if_exists(YY::Thunks::TopFix::_FUNCTION) {decltype(YY::Thunks::TopFix::_FUNCTION)* _CRT_CONCATENATE(FunctionInclude, _FUNCTION) = &YY::Thunks::TopFix::_FUNCTION;} \
__if_not_exists(YY::Thunks::TopFix::_FUNCTION){decltype(::_FUNCTION)* _CRT_CONCATENATE(FunctionInclude, _FUNCTION) = &::_FUNCTION;}                                  \
__declspec(allocate("TMP$__a")) LPCSTR _CRT_CONCATENATE(FunctionName, _FUNCTION) = #_FUNCTION; \
__if_not_exists(_CRT_CONCATENATE(FunctionName, _FUNCTION))


#include "Thunks/YY_Thunks_List.hpp"

#undef __DEFINE_THUNK
#pragma pop_macro("InterlockedCompareExchange64")

namespace Others
{
	extern "C" extern IMAGE_DOS_HEADER __ImageBase;

	//template <class _Ty = void>
	struct string_less {
		_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef LPCSTR _FIRST_ARGUMENT_TYPE_NAME;
		_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef LPCSTR _SECOND_ARGUMENT_TYPE_NAME;
		_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef bool _RESULT_TYPE_NAME;

		bool operator()(LPCSTR _Left, LPCSTR _Right) const {
			return strcmp(_Left, _Right) < 0;
		}
	};

	TEST_CLASS(通用项检测)
	{
		static void ReportModule(LPCSTR szImportName, DWORD DLLNameOffset, LPCWSTR szType = L"")
		{
			auto szDllName = DLLNameOffset ? ((const char*)&__ImageBase + DLLNameOffset) : nullptr;

			CStringW ErrorInfo;

			ErrorInfo.Format(L"%ws 模块：%hs 函数名称：%hs 没有被Thunks成功！", szType, szDllName, szImportName);

			Assert::Fail(ErrorInfo);
		}
	public:
		//扫描所有支持的符号，是否正确完成Thunks行为
		TEST_METHOD(符号Thunks检测)
		{
			
			
			//扫描所有导入符号
			std::set<LPCSTR, string_less> Names;

			for (auto p = FirstFunctionName; p != LastFunctionName; ++p)
			{
				if (*p == nullptr)
					continue;

				Names.insert(*p);
			}

			auto pNtHeader = (PIMAGE_NT_HEADERS)((char*)&__ImageBase + __ImageBase.e_lfanew);

			do
			{
				//搜索导入表
				auto pImort = &pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

				if (pImort->VirtualAddress == 0)
					break;

				auto pImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)((char*)&__ImageBase + pImort->VirtualAddress);


				for (; pImportDescriptor->Name; ++pImportDescriptor)
				{
					if (pImportDescriptor->OriginalFirstThunk == 0)
						continue;

					auto pThunkData = (IMAGE_THUNK_DATA*)((char*)&__ImageBase + pImportDescriptor->OriginalFirstThunk);

					for (; pThunkData->u1.AddressOfData; ++pThunkData)
					{
						//这是序数导入
						if (pThunkData->u1.AddressOfData >> ((sizeof(pThunkData->u1.AddressOfData) * 8) - 1))
							continue;

						auto pImportByName = (PIMAGE_IMPORT_BY_NAME)((char*)&__ImageBase + pThunkData->u1.ForwarderString);

						if (Names.find(pImportByName->Name) != Names.end())
						{
							ReportModule(pImportByName->Name, pImportDescriptor->Name, L"IMAGE_DIRECTORY_ENTRY_IMPORT");
						}
					}
				}
			} while (false);


			do
			{
				//检查延迟导入
				auto pImort = &pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT];

				if (pImort->VirtualAddress == 0)
					break;

				auto pDelayLoadDescriptor = (IMAGE_DELAYLOAD_DESCRIPTOR*)((char*)&__ImageBase + pImort->VirtualAddress);

				for (; pDelayLoadDescriptor->DllNameRVA; ++pDelayLoadDescriptor)
				{
					auto pThunkData = (IMAGE_THUNK_DATA*)((char*)&__ImageBase + pDelayLoadDescriptor->ImportNameTableRVA);

					for (; pThunkData->u1.AddressOfData; ++pThunkData)
					{
						//这是序数导入
						if (pThunkData->u1.AddressOfData >> ((sizeof(pThunkData->u1.AddressOfData) * 8) - 1))
							continue;

						auto pImportByName = (PIMAGE_IMPORT_BY_NAME)((char*)&__ImageBase + pThunkData->u1.ForwarderString);

						if (Names.find(pImportByName->Name) != Names.end())
						{
							ReportModule(pImportByName->Name, pDelayLoadDescriptor->DllNameRVA, L"IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT");
						}
					}
				}

			} while (false);
		}
	};

    TEST_CLASS(可链接性检测)
    {
    public:
        TEST_METHOD(obj模式可链接性)
        {
            struct TestInfo
            {
                LPCWSTR szPlatform;
                LPCWSTR szConfiguration;
                LPCWSTR szYY_ThunksFilePath;
            };

            static const TestInfo s_TestInfo[] =
            {
                {L"Win32", L"Static", YY_ThunksRootPath L"objs\\x86\\YY_Thunks_for_Win2K.obj"},
                {L"Win32", L"Static", YY_ThunksRootPath L"objs\\x86\\YY_Thunks_for_WinXP.obj"},
                {L"Win32", L"Static", YY_ThunksRootPath L"objs\\x86\\YY_Thunks_for_Vista.obj"},
                {L"Win32", L"Static", YY_ThunksRootPath L"objs\\x86\\YY_Thunks_for_Win7.obj"},
                {L"Win32", L"Static", YY_ThunksRootPath L"objs\\x86\\YY_Thunks_for_Win8.obj"},
                {L"Win32", L"Static", YY_ThunksRootPath L"objs\\x86\\YY_Thunks_for_Win10.0.10240.obj"},
                {L"Win32", L"Static", YY_ThunksRootPath L"objs\\x86\\YY_Thunks_for_Win10.0.19041.obj"},

                {L"x64", L"Static", YY_ThunksRootPath L"objs\\x64\\YY_Thunks_for_WinXP.obj"},
                {L"x64", L"Static", YY_ThunksRootPath L"objs\\x64\\YY_Thunks_for_Vista.obj"},
                {L"x64", L"Static", YY_ThunksRootPath L"objs\\x64\\YY_Thunks_for_Win7.obj"},
                {L"x64", L"Static", YY_ThunksRootPath L"objs\\x64\\YY_Thunks_for_Win8.obj"},
                {L"x64", L"Static", YY_ThunksRootPath L"objs\\x64\\YY_Thunks_for_Win10.0.10240.obj"},
                {L"x64", L"Static", YY_ThunksRootPath L"objs\\x64\\YY_Thunks_for_Win10.0.19041.obj"},
            };


            for (auto& _TestInfo : s_TestInfo)
            {
                CStringW _szSymbolsTestCppRootPath;
                _szSymbolsTestCppRootPath.Format(SymbolBuildTestPath L"Example\\%s\\%s\\%s\\", _TestInfo.szPlatform, _TestInfo.szConfiguration, PathFindFileNameW(_TestInfo.szYY_ThunksFilePath));

                CStringW _szBuildProperty = L"YY_Thunks_File_Path=";
                _szBuildProperty += _TestInfo.szYY_ThunksFilePath;

                RunMSBuildTest(_TestInfo.szPlatform, _TestInfo.szConfiguration, _szSymbolsTestCppRootPath, _szBuildProperty);
            }
        }

        TEST_METHOD(lib模式可链接性)
        {
            struct TestInfo
            {
                LPCWSTR szPlatform;
                LPCWSTR szConfiguration;
                LPCWSTR szTargetVersion;
            };

            static const TestInfo s_TestInfo[] =
            {
                {L"Win32", L"Static", L"5.0.2195.0"},
                {L"Win32", L"Static", L"5.1.2600.0"},
                {L"Win32", L"Static", L"6.0.6000.0"},
                {L"Win32", L"Static", L"6.1.7600.0"},
                {L"Win32", L"Static", L"6.2.9200.0"},
                {L"Win32", L"Static", L"10.0.10240.0"},
                {L"Win32", L"Static", L"10.0.19041.0"},

                {L"x64", L"Static", L"5.2.3790.1180"},
                {L"x64", L"Static", L"6.0.6000.0"},
                {L"x64", L"Static", L"6.1.7600.0"},
                {L"x64", L"Static", L"6.2.9200.0"},
                {L"x64", L"Static", L"10.0.10240.0"},
                {L"x64", L"Static", L"10.0.19041.0"},
            };


            for (auto& _TestInfo : s_TestInfo)
            {
                CStringW szSymbolsTestCppRootPath;
                szSymbolsTestCppRootPath.Format(SymbolBuildTestPath L"Example\\%s\\%s\\%s\\", _TestInfo.szPlatform, _TestInfo.szConfiguration, _TestInfo.szTargetVersion);

                CStringW _szBuildProperty = L"YY_Thunks_Libs=";
                _szBuildProperty.AppendFormat(YY_ThunksRootPath L"Lib\\%s\\", _TestInfo.szTargetVersion);

                if (_wcsicmp(_TestInfo.szPlatform, L"Win32") == 0)
                {
                    _szBuildProperty += L"x86";
                }
                else
                {
                    _szBuildProperty += _TestInfo.szPlatform;
                }

                RunMSBuildTest(_TestInfo.szPlatform, _TestInfo.szConfiguration, szSymbolsTestCppRootPath, _szBuildProperty);
            }
        }

        TEST_METHOD(SDK6可链接性)
        {
            struct TestInfo
            {
                LPCWSTR szPlatform;
                LPCWSTR szConfiguration;
                LPCWSTR szYY_ThunksFilePath;
            };

            static const TestInfo s_TestInfo[] =
            {
                {L"Win32", L"Static", YY_ThunksRootPath L"objs\\x86\\YY_Thunks_for_Win2K.obj"},
                {L"Win32", L"Static", YY_ThunksRootPath L"objs\\x86\\YY_Thunks_for_WinXP.obj"},
                {L"Win32", L"Static", YY_ThunksRootPath L"objs\\x86\\YY_Thunks_for_Vista.obj"},

                // 打包的SDK6有点问题，缺了很多场景的lib，暂时x64先不管了，懒得再找SDK了。
                // {L"x64", L"Static", YY_ThunksRootPath L"objs\\x64\\YY_Thunks_for_WinXP.obj"},
                // {L"x64", L"Static", YY_ThunksRootPath L"objs\\x64\\YY_Thunks_for_Vista.obj"},
            };

            wchar_t _szWindowsSdkDir_60A[MAX_PATH];
            auto _uResult = GetEnvironmentVariableW(L"WindowsSdkDir_60A_TestPath", _szWindowsSdkDir_60A, _countof(_szWindowsSdkDir_60A));
            if (_uResult && _uResult < _countof(_szWindowsSdkDir_60A))
            {
                for (auto& _TestInfo : s_TestInfo)
                {
                    CStringW szSymbolsTestCppRootPath;
                    szSymbolsTestCppRootPath.Format(SymbolBuildTestPath L"Example\\%s\\%s\\%s_SDK6\\", _TestInfo.szPlatform, _TestInfo.szConfiguration, PathFindFileNameW(_TestInfo.szYY_ThunksFilePath));

                    CStringW _szBuildProperty = L"YY_Thunks_File_Path=";
                    _szBuildProperty += _TestInfo.szYY_ThunksFilePath;
                    _szBuildProperty += L';';
                    _szBuildProperty += L"WindowsSdkDir_60A=";
                    _szBuildProperty += _szWindowsSdkDir_60A;

                    RunMSBuildTest(_TestInfo.szPlatform, _TestInfo.szConfiguration, szSymbolsTestCppRootPath, _szBuildProperty);
                }
            }
        }
    };
}
