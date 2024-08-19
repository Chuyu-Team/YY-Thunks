#if (YY_Thunks_Target < __WindowsNT6)
#include <ndfapi.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    ndfapi,
    8,
    HRESULT,
    WINAPI,
    NdfCreateWebIncident,
        _In_ LPCWSTR _szUrl,
        _Outptr_ NDFHANDLE* _phHandle
        )
    {
        if (const auto _pfnNdfCreateWebIncident = try_get_NdfCreateWebIncident())
        {
            return _pfnNdfCreateWebIncident(_szUrl, _phHandle);
        }
        
        if (_szUrl == nullptr || _phHandle == nullptr)
            return NDF_E_BAD_PARAM;

        *_phHandle = INVALID_HANDLE_VALUE;
        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    ndfapi,
    4,
    HRESULT,
    WINAPI,
    NdfCloseIncident,
        NDFHANDLE _hHandle
        )
    {
        if (const auto _pfnNdfCloseIncident = try_get_NdfCloseIncident())
        {
            return _pfnNdfCloseIncident(_hHandle);
        }
        
        if (_hHandle != INVALID_HANDLE_VALUE)
            return E_HANDLE;

        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    ndfapi,
    8,
    HRESULT,
    WINAPI,
    NdfExecuteDiagnosis,
        _In_ NDFHANDLE _hHandle,
        _In_opt_ HWND _hWnd
        )
    {
        if (const auto _pfnNdfExecuteDiagnosis = try_get_NdfExecuteDiagnosis())
        {
            return _pfnNdfExecuteDiagnosis(_hHandle, _hWnd);
        }
        
        if (_hHandle != INVALID_HANDLE_VALUE)
        {
            return E_HANDLE;
        }

        return S_OK;
    }
#endif
}
