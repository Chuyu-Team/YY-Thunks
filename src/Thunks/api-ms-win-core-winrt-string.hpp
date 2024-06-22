#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#include <hstring_private.h>
#include <winstring.h>
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8) && __YY_Thunks_libs && !defined(__Comment_Lib_runtimeobject)
#define __Comment_Lib_runtimeobject
// 193行 WindowsCreateString
#pragma comment(lib, "runtimeobject.lib")
#endif

namespace YY::Thunks
{

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    12,
    HRESULT,
    STDAPICALLTYPE,
    WindowsCreateString,
        _In_reads_opt_(length) PCNZWCH sourceString,
        UINT32 length,
        _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING* string
        )
    {
        if (auto const pWindowsCreateString = try_get_WindowsCreateString())
        {
            return pWindowsCreateString(sourceString, length, string);
        }

        if (!string)
        {
            return E_INVALIDARG;
        }
        *string = nullptr;

        if (!sourceString && 0 != length)
        {
            return E_POINTER;
        }

        SIZE_T RequiredSize = sizeof(internal::STRING_OPAQUE) + length * sizeof(WCHAR);
        if (MAXUINT32 < RequiredSize)
        {
            return MEM_E_INVALID_SIZE;
        }

        internal::PSTRING_OPAQUE Result =
            reinterpret_cast<internal::PSTRING_OPAQUE>(::HeapAlloc(
                ::GetProcessHeap(),
                HEAP_ZERO_MEMORY,
                RequiredSize));
        if (!Result)
        {
            return E_OUTOFMEMORY;
        }

        Result->Header.Flags = WRHF_NONE;
        Result->Header.Length = length;
        Result->Header.StringRef = Result->String;
        Result->RefCount = 1;
        ::memcpy(Result->String, sourceString, length * sizeof(WCHAR));
        Result->String[length] = L'\0';

        *string = reinterpret_cast<HSTRING>(Result);

        return S_OK;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    16,
    HRESULT,
    STDAPICALLTYPE,
    WindowsCreateStringReference,
        _In_reads_opt_(length + 1) PCWSTR sourceString,
        UINT32 length,
        _Out_ HSTRING_HEADER* hstringHeader,
        _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING* string
        )
    {
        if (auto const pWindowsCreateStringReference = try_get_WindowsCreateStringReference())
        {
            return pWindowsCreateStringReference(sourceString, length, hstringHeader, string);
        }

        if (!string || !hstringHeader)
        {
            return E_INVALIDARG;
        }
        *string = nullptr;

        if (!sourceString && 0 != length)
        {
            return E_POINTER;
        }

        if (sourceString)
        {
            if (L'0' != sourceString[length])
            {
                return E_STRING_NOT_NULL_TERMINATED;
            }
        }

        internal::PHSTRING_HEADER_INTERNAL Header =
            reinterpret_cast<internal::PHSTRING_HEADER_INTERNAL>(hstringHeader);
        ::memset(Header, 0, sizeof(internal::HSTRING_HEADER_INTERNAL));

        Header->Flags = WRHF_STRING_REFERENCE;
        Header->Length = length;
        Header->StringRef = sourceString;

        *string = reinterpret_cast<HSTRING>(Header);

        return S_OK;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    4,
    HRESULT,
    STDAPICALLTYPE,
    WindowsDeleteString,
        _In_opt_ HSTRING string
        )
    {
        if (auto const pWindowsDeleteString = try_get_WindowsDeleteString())
        {
            return pWindowsDeleteString(string);
        }

        if (string)
        {
            internal::PSTRING_OPAQUE OpaqueString =
                reinterpret_cast<internal::PSTRING_OPAQUE>(string);
            if (!(OpaqueString->Header.Flags & WRHF_STRING_REFERENCE))
            {
                if (0 == ::InterlockedDecrement(
                    reinterpret_cast<LONG volatile*>(&OpaqueString->RefCount)))
                {
                    ::HeapFree(::GetProcessHeap(), 0, OpaqueString);
                }
            }
        }

        return S_OK;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    8,
    HRESULT,
    STDAPICALLTYPE,
    WindowsDuplicateString,
        _In_opt_ HSTRING string,
        _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING* newString
        )
    {
        if (auto const pWindowsDuplicateString = try_get_WindowsDuplicateString())
        {
            return pWindowsDuplicateString(string, newString);
        }

        if (!newString)
        {
            return E_INVALIDARG;
        }
        *newString = nullptr;

        if (string)
        {
            internal::PSTRING_OPAQUE OpaqueString =
                reinterpret_cast<internal::PSTRING_OPAQUE>(string);
            if (OpaqueString->Header.Flags & WRHF_STRING_REFERENCE)
            {
                return ::WindowsCreateString(
                    OpaqueString->Header.StringRef,
                    OpaqueString->Header.Length,
                    newString);
            }

            ::InterlockedIncrement(
                reinterpret_cast<LONG volatile*>(&OpaqueString->RefCount));
            *newString = string;
        }

        return S_OK;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    4,
    UINT32,
    STDAPICALLTYPE,
    WindowsGetStringLen,
        _In_opt_ HSTRING string
        )
    {
        if (auto const pWindowsGetStringLen = try_get_WindowsGetStringLen())
        {
            return pWindowsGetStringLen(string);
        }

        if (string)
        {
            internal::PSTRING_OPAQUE OpaqueString =
                reinterpret_cast<internal::PSTRING_OPAQUE>(string);
            return OpaqueString->Header.Length;
        }

        return 0;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    8,
    PCWSTR,
    STDAPICALLTYPE,
    WindowsGetStringRawBuffer,
        _In_opt_ HSTRING string,
        _Out_opt_ UINT32* length
        )
    {
        if (auto const pWindowsGetStringRawBuffer = try_get_WindowsGetStringRawBuffer())
        {
            return pWindowsGetStringRawBuffer(string, length);
        }

        if (!string)
        {
            if (length)
            {
                *length = 0;
            }

            return L"\0";
        }

        internal::PSTRING_OPAQUE OpaqueString =
            reinterpret_cast<internal::PSTRING_OPAQUE>(string);
        if (length)
        {
            *length = OpaqueString->Header.Length;
        }

        return OpaqueString->Header.StringRef;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    4,
    BOOL,
    STDAPICALLTYPE,
    WindowsIsStringEmpty,
        _In_opt_ HSTRING string
        )
    {
        if (auto const pWindowsIsStringEmpty = try_get_WindowsIsStringEmpty())
        {
            return pWindowsIsStringEmpty(string);
        }

        if (string)
        {
            internal::PSTRING_OPAQUE OpaqueString =
                reinterpret_cast<internal::PSTRING_OPAQUE>(string);
            return 0 == OpaqueString->Header.Length;
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    8,
    HRESULT,
    STDAPICALLTYPE,
    WindowsStringHasEmbeddedNull,
        _In_opt_ HSTRING string,
        _Out_ BOOL* hasEmbedNull
        )
    {
        if (auto const pWindowsStringHasEmbeddedNull = try_get_WindowsStringHasEmbeddedNull())
        {
            return pWindowsStringHasEmbeddedNull(string, hasEmbedNull);
        }

        if (!hasEmbedNull)
        {
            return E_INVALIDARG;
        }
        *hasEmbedNull = FALSE;

        if (string)
        {
            internal::PSTRING_OPAQUE OpaqueString =
                reinterpret_cast<internal::PSTRING_OPAQUE>(string);
            if (!(OpaqueString->Header.Flags & WRHF_EMBEDDED_NULLS_COMPUTED))
            {
                OpaqueString->Header.Flags |= WRHF_EMBEDDED_NULLS_COMPUTED;
                for (UINT32 i = 0; i < OpaqueString->Header.Length; ++i)
                {
                    if (OpaqueString->Header.StringRef[i] == L'\0')
                    {
                        OpaqueString->Header.Flags |= WRHF_HAS_EMBEDDED_NULLS;
                        break;
                    }
                }
            }
            *hasEmbedNull = OpaqueString->Header.Flags & WRHF_HAS_EMBEDDED_NULLS;
        }

        return S_OK;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    12,
    HRESULT,
    STDAPICALLTYPE,
    WindowsCompareStringOrdinal,
        _In_opt_ HSTRING string1,
        _In_opt_ HSTRING string2,
        _Out_ INT32* result
        )
    {
        if (auto const pWindowsCompareStringOrdinal = try_get_WindowsCompareStringOrdinal())
        {
            return pWindowsCompareStringOrdinal(string1, string2, result);
        }

        if (!result)
        {
            return E_INVALIDARG;
        }
        *result = 0;

        if (string1 && string2)
        {
            internal::PSTRING_OPAQUE OpaqueString1 =
                reinterpret_cast<internal::PSTRING_OPAQUE>(string1);
            internal::PSTRING_OPAQUE OpaqueString2 =
                reinterpret_cast<internal::PSTRING_OPAQUE>(string2);

            int CompareResult = ::CompareStringOrdinal(
                OpaqueString1->Header.StringRef,
                OpaqueString1->Header.Length,
                OpaqueString2->Header.StringRef,
                OpaqueString2->Header.Length,
                FALSE);
            if (CompareResult == CSTR_LESS_THAN)
            {
                *result = -1;
            }
            else if (CompareResult == CSTR_GREATER_THAN)
            {
                *result = 1;
            }
        }
        else if (string1 && !string2)
        {
            internal::PSTRING_OPAQUE OpaqueString1 =
                reinterpret_cast<internal::PSTRING_OPAQUE>(string1);
            if (OpaqueString1->Header.Length)
            {
                *result = 1;
            }
        }
        else if (!string1 && string2)
        {
            internal::PSTRING_OPAQUE OpaqueString2 =
                reinterpret_cast<internal::PSTRING_OPAQUE>(string2);
            if (OpaqueString2->Header.Length)
            {
                *result = 1;
            }
        }

        return S_OK;
    }
#endif

}
