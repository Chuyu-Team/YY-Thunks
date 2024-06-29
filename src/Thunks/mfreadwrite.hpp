#if (YY_Thunks_Target < __WindowsNT6_1)
#include <mfreadwrite.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	适用于 Windows Vista 的 Windows 7、Windows Vista 和平台更新补充 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfreadwrite,
    12,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateSourceReaderFromMediaSource,
        _In_ IMFMediaSource* pMediaSource,
        _In_opt_ IMFAttributes* pAttributes,
        _Out_ IMFSourceReader** ppSourceReader
        )
    {
        if (const auto _pfnMFCreateSourceReaderFromMediaSource = try_get_MFCreateSourceReaderFromMediaSource())
        {
            return _pfnMFCreateSourceReaderFromMediaSource(pMediaSource, pAttributes, ppSourceReader);
        }

        if (!ppSourceReader)
            return E_POINTER;
        
        *ppSourceReader = nullptr;
        return E_NOTIMPL;
    }
#endif
}
