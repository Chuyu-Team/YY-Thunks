#if (YY_Thunks_Target < __WindowsNT6_2)
#include <mmdeviceapi.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器    Windows Server 2012 [桌面应用 |UWP 应用]
    __DEFINE_THUNK(
    mmdevapi,
    20,
    HRESULT,
    STDAPICALLTYPE,
    ActivateAudioInterfaceAsync,
        _In_ LPCWSTR _szDeviceInterfacePath,
        _In_ REFIID riid,
        _In_opt_ PROPVARIANT* _pActivationParams,
        _In_ IActivateAudioInterfaceCompletionHandler* _pCompletionHandler,
        _COM_Outptr_ IActivateAudioInterfaceAsyncOperation** _ppActivationOperation
        )
    {
        if (const auto _pfnActivateAudioInterfaceAsync = try_get_ActivateAudioInterfaceAsync())
        {
            return _pfnActivateAudioInterfaceAsync(_szDeviceInterfacePath, riid, _pActivationParams, _pCompletionHandler, _ppActivationOperation);
        }

        if (!_ppActivationOperation)
            return E_POINTER;

        *_ppActivationOperation = nullptr;
        return E_NOTIMPL;
    }
#endif

}
