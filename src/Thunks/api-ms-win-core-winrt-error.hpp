#if (YY_Thunks_Target < __WindowsNT6_3)
#include <roerrorapi.h>
#include <winstring.h>
#endif

namespace YY::Thunks
{

#if (YY_Thunks_Target < __WindowsNT6_2)
    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_0,
    4,
    void,
    WINAPI,
    RoFailFastWithErrorContext,
        HRESULT hrError
        )
    {
        if (auto pRoFailFastWithErrorContext = try_get_RoFailFastWithErrorContext())
        {
            pRoFailFastWithErrorContext(hrError);
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_0,
    8,
    BOOL,
    WINAPI,
    RoOriginateError,
        _In_ HRESULT error,
        _In_opt_ HSTRING message
        )
    {
        if (auto pRoOriginateError = try_get_RoOriginateError())
        {
            return pRoOriginateError(error, message);
        }

        // According to the C++/WinRT fallback implementation, we should
        // return TRUE to tell the caller that the error message was
        // reported successfully.
        return TRUE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_0,
    12,
    BOOL,
    WINAPI,
    RoOriginateErrorW,
        _In_ HRESULT error,
        _In_ UINT cchMax,
        _When_(cchMax == 0, _In_reads_or_z_opt_(MAX_ERROR_MESSAGE_CHARS) ) 
                    _When_(cchMax > 0 && cchMax < MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(cchMax) )
                    _When_(cchMax >= MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(MAX_ERROR_MESSAGE_CHARS) ) PCWSTR message
        )
    {
        if (auto pRoOriginateErrorW = try_get_RoOriginateErrorW())
        {
            return pRoOriginateErrorW(error, cchMax, message);
        }

        // According to the C++/WinRT fallback implementation, we should
        // return TRUE to tell the caller that the error message was
        // reported successfully.
        return TRUE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_3)
    //Windows 8.1 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_1,
    12,
    BOOL,
    WINAPI,
    RoOriginateLanguageException,
        _In_ HRESULT error,
        _In_opt_ HSTRING message,
        _In_opt_ IUnknown* languageException
        )
    {
        if (auto pRoOriginateLanguageException = try_get_RoOriginateLanguageException())
        {
            return pRoOriginateLanguageException(error, message, languageException);
        }
        if ((error == S_OK) || (!languageException) || WindowsIsStringEmpty(message))
            return FALSE;
        // The RoOriginateLanguageException function behaves like RoOriginateError but takes another parameter that stores extra information about the error.
        return TRUE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)
    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_0,
    12,
    BOOL,
    WINAPI,
    RoTransformError,
        _In_ HRESULT oldError,
        _In_ HRESULT newError,
        _In_opt_ HSTRING message
        )
    {
        if (auto pRoTransformError = try_get_RoTransformError())
        {
            return pRoTransformError(oldError, newError, message);
        }
        if ((oldError == S_OK && newError == S_OK) || WindowsIsStringEmpty(message))
            return FALSE;
        return TRUE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)
    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_0,
    16,
    BOOL,
    WINAPI,
    RoTransformErrorW,
        _In_ HRESULT oldError,
        _In_ HRESULT newError,
        _In_ UINT cchMax,
        _When_(cchMax == 0, _In_reads_or_z_opt_(MAX_ERROR_MESSAGE_CHARS))
                _When_(cchMax > 0 && cchMax < MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(cchMax) )
                _When_(cchMax >= MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(MAX_ERROR_MESSAGE_CHARS) ) PCWSTR message
        )
    {
        if (auto pRoTransformErrorW = try_get_RoTransformErrorW())
        {
            return pRoTransformErrorW(oldError, newError, cchMax, message);
        }
        if ((oldError == S_OK && newError == S_OK) || ((!cchMax) || !(message)))
            return FALSE;
        return TRUE;
    }
#endif
}
