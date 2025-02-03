#if (YY_Thunks_Target < __WindowsNT6_3)
#include <roerrorapi.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_3) && (YY_Thunks_Target >= __WindowsNT6_2 || __YY_Thunks_libs) && !defined(__Comment_Lib_runtimeobject)
#define __Comment_Lib_runtimeobject
// RoOriginateError
#pragma comment(lib, "runtimeobject.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_0,
    8,
    BOOL,
    WINAPI,
    RoOriginateError,
        _In_ HRESULT _hrError,
        _In_opt_ HSTRING _szMessage
        )
    {
        if (const auto _pfnRoOriginateError = try_get_RoOriginateError())
        {
            return _pfnRoOriginateError(_hrError, _szMessage);
        }

        return FAILED(_hrError);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_0,
    12,
    BOOL,
    WINAPI,
    RoOriginateErrorW,
        _In_ HRESULT _hrError,
        _In_ UINT _cchMax,
        _When_(_cchMax == 0, _In_reads_or_z_opt_(MAX_ERROR_MESSAGE_CHARS) )
                    _When_(_cchMax > 0 && _cchMax < MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(_cchMax) )
                    _When_(_cchMax >= MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(MAX_ERROR_MESSAGE_CHARS) ) PCWSTR _szMessage
        )
    {
        if (const auto _pfnRoOriginateErrorW = try_get_RoOriginateErrorW())
        {
            return _pfnRoOriginateErrorW(_hrError, _cchMax, _szMessage);
        }

        return FAILED(_hrError);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_0,
    4,
    void,
    WINAPI,
    RoFailFastWithErrorContext,
        HRESULT _hrError
        )
    {
        if (const auto _pfnRoFailFastWithErrorContext = try_get_RoFailFastWithErrorContext())
        {
            return _pfnRoFailFastWithErrorContext(_hrError);
        }

        RaiseFailFastException(nullptr, nullptr, 0);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_3)

    // 最低受支持的客户端	Windows 8.1 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012 R2[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_1,
    12,
    BOOL,
    WINAPI,
    RoOriginateLanguageException,
        _In_ HRESULT _hrError,
        _In_opt_ HSTRING _szMessage,
        _In_opt_ IUnknown* _pLanguageException
        )
    {
        if (const auto _pfnRoOriginateLanguageException = try_get_RoOriginateLanguageException())
        {
            return _pfnRoOriginateLanguageException(_hrError, _szMessage, _pLanguageException);
        }

        return RoOriginateError(_hrError, _szMessage);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_0,
    12,
    BOOL,
    WINAPI,
    RoTransformError,
        _In_ HRESULT _hrOldError,
        _In_ HRESULT _hrNewError,
        _In_opt_ HSTRING _szMessage
        )
    {
        if (const auto _pfnRoTransformError = try_get_RoTransformError())
        {
            return _pfnRoTransformError(_hrOldError, _hrNewError, _szMessage);
        }

        if (_hrOldError == _hrNewError || (SUCCEEDED(_hrOldError) && SUCCEEDED(_hrNewError)))
        {
            return FALSE;
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_error_l1_1_0,
    16,
    BOOL,
    WINAPI,
    RoTransformErrorW,
        _In_ HRESULT _hrOldError,
        _In_ HRESULT _hrNewError,
        _In_ UINT _cchMax,
        _When_(_cchMax == 0, _In_reads_or_z_opt_(MAX_ERROR_MESSAGE_CHARS))
                _When_(_cchMax > 0 && _cchMax < MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(_cchMax) )
                _When_(_cchMax >= MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(MAX_ERROR_MESSAGE_CHARS) ) PCWSTR _szMessage
        )
    {
        if (const auto _pfnRoTransformErrorW = try_get_RoTransformErrorW())
        {
            return _pfnRoTransformErrorW(_hrOldError, _hrNewError, _cchMax, _szMessage);
        }

        if (_hrOldError == _hrNewError || (SUCCEEDED(_hrOldError) && SUCCEEDED(_hrNewError)))
        {
            return FALSE;
        }

        return TRUE;
    }
#endif
}
