#if (YY_Thunks_Target < __WindowsNT6_1)
#include <mfidl.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_1)

    // Minimum supported client	Windows 7 [desktop apps only]
    // Minimum supported server	Windows Server 2008 R2[desktop apps only]
    __DEFINE_THUNK(
    mf,
    8,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateDeviceSource,
        _In_  IMFAttributes*   pAttributes,
        _Outptr_ IMFMediaSource** ppSource
        )
    {
        if (const auto _pfnMFCreateDeviceSource = try_get_MFCreateDeviceSource())
        {
            return _pfnMFCreateDeviceSource(pAttributes, ppSource);
        }

        if (!ppSource)
            return E_POINTER;
        
        *ppSource = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // Minimum supported client	Windows 7 [desktop apps only]
    // Minimum supported server	Windows Server 2008 R2[desktop apps only]
    __DEFINE_THUNK(
    mf,
    12,
    HRESULT,
    STDAPICALLTYPE,
    MFEnumDeviceSources,
        _In_                IMFAttributes* pAttributes,
        _Outptr_result_buffer_(*pcSourceActivate) IMFActivate*** pppSourceActivate,
        _Out_               UINT32* pcSourceActivate
        )
    {
        if (const auto _pfnMFEnumDeviceSources = try_get_MFEnumDeviceSources())
        {
            return _pfnMFEnumDeviceSources(pAttributes, pppSourceActivate, pcSourceActivate);
        }

        if (!pcSourceActivate)
            return E_POINTER;
        *pcSourceActivate = 0;

        if (!pppSourceActivate)
            return E_POINTER;
        *pppSourceActivate = nullptr;     
        return E_NOTIMPL;
    }
#endif
}
