#include <winstring.h>

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
STDAPICALLTYPE
WindowsCreateString(
    _In_reads_opt_(length) PCNZWCH sourceString,
    UINT32 length,
    _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING* string
    )
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pWindowsCreateString = try_get_WindowsCreateString())
    {
        return pWindowsCreateString(sourceString, length, string);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_string_l1_1_0, WindowsCreateString, 12);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
STDAPICALLTYPE
WindowsCreateStringReference(
    _In_reads_opt_(length + 1) PCWSTR sourceString,
    UINT32 length,
    _Out_ HSTRING_HEADER* hstringHeader,
    _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING* string
    )
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pWindowsCreateStringReference = try_get_WindowsCreateStringReference())
    {
        return pWindowsCreateStringReference(sourceString, length, hstringHeader, string);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_string_l1_1_0, WindowsCreateStringReference, 16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
STDAPICALLTYPE
WindowsDeleteString(
    _In_opt_ HSTRING string
    )
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pWindowsDeleteString = try_get_WindowsDeleteString())
    {
        return pWindowsDeleteString(string);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_string_l1_1_0, WindowsDeleteString, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
STDAPICALLTYPE
WindowsDuplicateString(
    _In_opt_ HSTRING string,
    _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING* newString
    )
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pWindowsDuplicateString = try_get_WindowsDuplicateString())
    {
        return pWindowsDuplicateString(string, newString);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_string_l1_1_0, WindowsDuplicateString, 8);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
UINT32
STDAPICALLTYPE
WindowsGetStringLen(
    _In_opt_ HSTRING string
    )
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pWindowsGetStringLen = try_get_WindowsGetStringLen())
    {
        return pWindowsGetStringLen(string);
    }

    return 0;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_string_l1_1_0, WindowsGetStringLen, 4);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
PCWSTR
STDAPICALLTYPE
WindowsGetStringRawBuffer(
    _In_opt_ HSTRING string,
    _Out_opt_ UINT32* length
    )
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pWindowsGetStringRawBuffer = try_get_WindowsGetStringRawBuffer())
    {
        return pWindowsGetStringRawBuffer(string, length);
    }

    return nullptr;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_string_l1_1_0, WindowsGetStringRawBuffer, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
BOOL
STDAPICALLTYPE
WindowsIsStringEmpty(
    _In_opt_ HSTRING string
    )
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pWindowsIsStringEmpty = try_get_WindowsIsStringEmpty())
    {
        return pWindowsIsStringEmpty(string);
    }

    return TRUE;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_string_l1_1_0, WindowsIsStringEmpty, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
STDAPICALLTYPE
WindowsStringHasEmbeddedNull(
    _In_opt_ HSTRING string,
    _Out_ BOOL* hasEmbedNull
    )
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pWindowsStringHasEmbeddedNull = try_get_WindowsStringHasEmbeddedNull())
    {
        return pWindowsStringHasEmbeddedNull(string, hasEmbedNull);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_string_l1_1_0, WindowsStringHasEmbeddedNull, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HRESULT
STDAPICALLTYPE
WindowsCompareStringOrdinal(
    _In_opt_ HSTRING string1,
    _In_opt_ HSTRING string2,
    _Out_ INT32* result
    )
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto const pWindowsCompareStringOrdinal = try_get_WindowsCompareStringOrdinal())
    {
        return pWindowsCompareStringOrdinal(string1, string2, result);
    }

    return E_NOTIMPL;
}
#endif

__YY_Thunks_Expand_Function(api_ms_win_core_winrt_string_l1_1_0, WindowsCompareStringOrdinal, 12);

#endif

    }
}