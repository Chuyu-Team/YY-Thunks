#include <winstring.h>

namespace YY
{
    namespace Thunks
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

            if (string)
                *string = nullptr;

            return E_NOTIMPL;
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

            if (string)
                *string = nullptr;

            return E_NOTIMPL;
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

            return E_NOTIMPL;
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


            if (newString)
                *newString = nullptr;

            return E_NOTIMPL;
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

            if (length)
                *length = 0;

            return nullptr;
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

            return E_NOTIMPL;
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

            return E_NOTIMPL;
        }
#endif

    }
}
