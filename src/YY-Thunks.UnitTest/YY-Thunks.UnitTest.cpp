#include "pch.h"
#include "CppUnitTest.h"
#include <set>
#include <atlstr.h>

#pragma push_macro("InterlockedCompareExchange64")
#undef InterlockedCompareExchange64

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
}
