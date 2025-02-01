#if (YY_Thunks_Target < __WindowsNT6_2)
#include <mfapi.h>

#if !defined(__Comment_Lib_mfplat)
#define __Comment_Lib_mfplat
#pragma comment(lib, "mfplat.lib")
#endif
#endif

#if (YY_Thunks_Target < __WindowsNT6)
#include <mfidl.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    8,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateDXGIDeviceManager,
        _Out_ UINT* _puResetToken,
        _Outptr_ IMFDXGIDeviceManager** _ppDeviceManager
        )
    {
        if (const auto _pfnMFCreateDXGIDeviceManager = try_get_MFCreateDXGIDeviceManager())
        {
            return _pfnMFCreateDXGIDeviceManager(_puResetToken, _ppDeviceManager);
        }

        if (_puResetToken)
            *_puResetToken = 0;
        if (_ppDeviceManager)
            *_ppDeviceManager = nullptr;

        if (_puResetToken == nullptr || _ppDeviceManager == nullptr)
            return E_INVALIDARG;
        return E_NOTIMPL;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    20,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateDXGISurfaceBuffer,
        _In_ REFIID _id,
        _In_ IUnknown* _punkSurface,
        _In_ UINT _uSubresourceIndex,
        _In_ BOOL _bBottomUpWhenLinear,
        _Outptr_ IMFMediaBuffer** _ppBuffer
        )
    {
        if (const auto _pfnMFCreateDXGISurfaceBuffer = try_get_MFCreateDXGISurfaceBuffer())
        {
            return _pfnMFCreateDXGISurfaceBuffer(_id, _punkSurface, _uSubresourceIndex, _bBottomUpWhenLinear, _ppBuffer);
        }

        if(!_ppBuffer)
            return E_INVALIDARG;
        *_ppBuffer = nullptr;
        return E_NOTIMPL;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    8,
    HRESULT,
    STDAPICALLTYPE,
    MFLockDXGIDeviceManager,
        _Out_opt_ UINT* _puResetToken,
        _Outptr_ IMFDXGIDeviceManager** _ppManager
        )
    {
        if (const auto _pfnMFLockDXGIDeviceManager = try_get_MFLockDXGIDeviceManager())
        {
            return _pfnMFLockDXGIDeviceManager(_puResetToken, _ppManager);
        }

        if (_puResetToken)
            *_puResetToken = 0;
        if (_ppManager)
            *_ppManager = nullptr;
        if(_puResetToken == nullptr || _ppManager == nullptr)
            return E_INVALIDARG;
        return E_NOTIMPL;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    0,
    HRESULT,
    STDAPICALLTYPE,
    MFUnlockDXGIDeviceManager,
        )
    {
        if (const auto _pfnMFUnlockDXGIDeviceManager = try_get_MFUnlockDXGIDeviceManager())
        {
            return _pfnMFUnlockDXGIDeviceManager();
        }

        return E_NOTIMPL;
    }
#endif
    

#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    12,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateAlignedMemoryBuffer,
        _In_ DWORD                      cbMaxLength,
        _In_ DWORD                      cbAligment, 
        _Out_ IMFMediaBuffer**          ppBuffer
        )
    {
        if (const auto _pfnMFCreateAlignedMemoryBuffer = try_get_MFCreateAlignedMemoryBuffer())
        {
            return _pfnMFCreateAlignedMemoryBuffer(cbMaxLength, cbAligment, ppBuffer);
        }

        if (!ppBuffer)
            return E_POINTER;
        *ppBuffer = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    16,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateAsyncResult,
        IUnknown* punkObject,
        IMFAsyncCallback* pCallback,
        IUnknown* punkState,
        _Out_ IMFAsyncResult** ppAsyncResult
        )
    {
        if (const auto _pfnMFCreateAsyncResult = try_get_MFCreateAsyncResult())
        {
            return _pfnMFCreateAsyncResult(punkObject, pCallback, punkState, ppAsyncResult);
        }

        if (!ppAsyncResult)
            return E_POINTER;
        *ppAsyncResult = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    8,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateAttributes,
        _Out_   IMFAttributes** ppMFAttributes,
        _In_    UINT32          cInitialSize
        )
    {
        if (const auto _pfnMFCreateAttributes = try_get_MFCreateAttributes())
        {
            return _pfnMFCreateAttributes(ppMFAttributes, cInitialSize);
        }

        if (!ppMFAttributes)
            return E_POINTER;
        *ppMFAttributes = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    4,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateEventQueue,
        _Out_ IMFMediaEventQueue** ppMediaEventQueue
        )
    {
        if (const auto _pfnMFCreateEventQueue = try_get_MFCreateEventQueue())
        {
            return _pfnMFCreateEventQueue(ppMediaEventQueue);
        }

        if (!ppMediaEventQueue)
            return E_POINTER;
        *ppMediaEventQueue = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    16,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateMediaBufferWrapper,
        _In_ IMFMediaBuffer*   pBuffer,
        _In_ DWORD             cbOffset,
        _In_ DWORD             dwLength,
        _Out_ IMFMediaBuffer** ppBuffer
        )
    {
        if (const auto _pfnMFCreateMediaBufferWrapper = try_get_MFCreateMediaBufferWrapper())
        {
            return _pfnMFCreateMediaBufferWrapper(pBuffer, cbOffset, dwLength, ppBuffer);
        }

        if (!ppBuffer)
            return E_POINTER;
        *ppBuffer = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    20,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateMediaEvent,
        _In_ MediaEventType met,
        _In_ REFGUID guidExtendedType,
        _In_ HRESULT hrStatus,
        _In_opt_ const PROPVARIANT* pvValue,
        _Out_ IMFMediaEvent** ppEvent
        )
    {
        if (const auto _pfnMFCreateMediaEvent = try_get_MFCreateMediaEvent())
        {
            return _pfnMFCreateMediaEvent(met, guidExtendedType, hrStatus, pvValue, ppEvent);
        }

        if (!ppEvent)
            return E_POINTER;
        *ppEvent = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    4,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateMediaType,
        _Outptr_ IMFMediaType**  ppMFType
        )
    {
        if (const auto _pfnMFCreateMediaType = try_get_MFCreateMediaType())
        {
            return _pfnMFCreateMediaType(ppMFType);
        }

        if (!ppMFType)
            return E_POINTER;
        *ppMFType = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    8,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateMemoryBuffer,
        _In_ DWORD             cbMaxLength,
        _Out_ IMFMediaBuffer** ppBuffer
        )
    {
        if (const auto _pfnMFCreateMemoryBuffer = try_get_MFCreateMemoryBuffer())
        {
            return _pfnMFCreateMemoryBuffer(cbMaxLength, ppBuffer);
        }

        if (!ppBuffer)
            return E_POINTER;
        *ppBuffer = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    12,
    HRESULT,
    STDAPICALLTYPE,
    MFCreatePresentationDescriptor,
        DWORD cStreamDescriptors,
        _In_reads_opt_(cStreamDescriptors) IMFStreamDescriptor** apStreamDescriptors,
        _Outptr_ IMFPresentationDescriptor** ppPresentationDescriptor
        )
    {
        if (const auto _pfnMFCreatePresentationDescriptor = try_get_MFCreatePresentationDescriptor())
        {
            return _pfnMFCreatePresentationDescriptor(cStreamDescriptors, apStreamDescriptors, ppPresentationDescriptor);
        }

        if (!ppPresentationDescriptor)
            return E_POINTER;
        *ppPresentationDescriptor = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    4,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateSample,
        _Out_ IMFSample** ppIMFSample
        )
    {
        if (const auto _pfnMFCreateSample = try_get_MFCreateSample())
        {
            return _pfnMFCreateSample(ppIMFSample);
        }

        if (!ppIMFSample)
            return E_POINTER;
        *ppIMFSample = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    16,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateStreamDescriptor,
        DWORD dwStreamIdentifier,
        DWORD cMediaTypes,
        _In_reads_(cMediaTypes) IMFMediaType** apMediaTypes,
        _Outptr_ IMFStreamDescriptor** ppDescriptor
        )
    {
        if (const auto _pfnMFCreateStreamDescriptor = try_get_MFCreateStreamDescriptor())
        {
            return _pfnMFCreateStreamDescriptor(dwStreamIdentifier, cMediaTypes, apMediaTypes, ppDescriptor);
        }

        if (!ppDescriptor)
            return E_POINTER;
        *ppDescriptor = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    16,
    HRESULT,
    STDAPICALLTYPE,
    MFCreateWaveFormatExFromMFMediaType,
        _In_        IMFMediaType*   pMFType,
        _Out_       WAVEFORMATEX**  ppWF,
        _Out_opt_   UINT32*         pcbSize,
        _In_        UINT32          Flags
        )
    {
        if (const auto _pfnMFCreateWaveFormatExFromMFMediaType = try_get_MFCreateWaveFormatExFromMFMediaType())
        {
            return _pfnMFCreateWaveFormatExFromMFMediaType(pMFType, ppWF, pcbSize, Flags);
        }

        if (pcbSize)
            *pcbSize = 0;

        if (!ppWF)
            return E_POINTER;
        *ppWF = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    12,
    HRESULT,
    STDAPICALLTYPE,
    MFFrameRateToAverageTimePerFrame,
        _In_  UINT32  unNumerator,
        _In_  UINT32  unDenominator,
        _Out_ UINT64* punAverageTimePerFrame
        )
    {
        if (const auto _pfnMFFrameRateToAverageTimePerFrame = try_get_MFFrameRateToAverageTimePerFrame())
        {
            return _pfnMFFrameRateToAverageTimePerFrame(unNumerator, unDenominator, punAverageTimePerFrame);
        }

        if (punAverageTimePerFrame)
            return E_POINTER;

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    0,
    MFTIME,
    STDAPICALLTYPE,
    MFGetSystemTime,
        void
        )
    {
        if (const auto _pfnMFGetSystemTime = try_get_MFGetSystemTime())
        {
            return _pfnMFGetSystemTime();
        }

        FILETIME _ftTime;
        GetSystemTimeAsFileTime(&_ftTime);
        return *(LONG64*)(&_ftTime);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    12,
    HRESULT,
    STDAPICALLTYPE,
    MFInitMediaTypeFromWaveFormatEx,
        _In_                    IMFMediaType*           pMFType,
        _In_reads_bytes_(cbBufSize)  const WAVEFORMATEX*     pWaveFormat,
        _In_                    UINT32                  cbBufSize
        )
    {
        if (const auto _pfnMFInitMediaTypeFromWaveFormatEx = try_get_MFInitMediaTypeFromWaveFormatEx())
        {
            return _pfnMFInitMediaTypeFromWaveFormatEx(pMFType, pWaveFormat, cbBufSize);
        }

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    0,
    HRESULT,
    STDAPICALLTYPE,
    MFShutdown
        )
    {
        if (const auto _pfnMFShutdown = try_get_MFShutdown())
        {
            return _pfnMFShutdown();
        }

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008[desktop apps | UWP apps]
    // Windows XP SP2需要安装补丁
    __DEFINE_THUNK(
    mfplat,
    8,
    HRESULT,
    STDAPICALLTYPE,
    MFStartup,
        ULONG Version,
        DWORD dwFlags
        )
    {
        if (const auto _pfnMFStartup = try_get_MFStartup())
        {
            return _pfnMFStartup(Version, dwFlags);
        }

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // Minimum supported client	Windows 7 [desktop apps | UWP apps]
    // Minimum supported server	Windows Server 2008 R2[desktop apps | UWP apps]
    __DEFINE_THUNK(
    mfplat,
    36,
    HRESULT,
    STDAPICALLTYPE,
    MFTEnumEx,
        _In_                                GUID                          guidCategory,
        _In_                                UINT32                        Flags,
        _In_opt_                            const MFT_REGISTER_TYPE_INFO* pInputType,
        _In_opt_                            const MFT_REGISTER_TYPE_INFO* pOutputType,
        _Outptr_result_buffer_(*pnumMFTActivate) IMFActivate***           pppMFTActivate,
        _Out_                               UINT32*                       pnumMFTActivate
        )
    {
        if (const auto _pfnMFTEnumEx = try_get_MFTEnumEx())
        {
            return _pfnMFTEnumEx(guidCategory, Flags, pInputType, pOutputType, pppMFTActivate, pnumMFTActivate);
        }

        if (!pppMFTActivate)
            return E_POINTER;

        *pppMFTActivate = nullptr;

        if (!pnumMFTActivate)
            return E_POINTER;

        *pnumMFTActivate = 0;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端    Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    8,
    HRESULT,
    STDAPICALLTYPE,
    MFCancelWorkItem,
        MFWORKITEM_KEY _Key
        )
    {
        if (const auto _pfnMFCancelWorkItem = try_get_MFCancelWorkItem())
        {
            return _pfnMFCancelWorkItem(_Key);
        }
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    16,
    HRESULT,
    STDAPICALLTYPE,
    MFLockSharedWorkQueue,
        _In_ PCWSTR _szClass,
        _In_ LONG _nBasePriority,
        _Inout_ DWORD* _puTaskId,
        _Out_ DWORD* _puId 
        )
    {
        if (const auto _pfnMFLockSharedWorkQueue = try_get_MFLockSharedWorkQueue())
        {
            return _pfnMFLockSharedWorkQueue(_szClass, _nBasePriority, _puTaskId, _puId);
        }
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端    Windows Vista [仅限桌面应用]
    // 最低受支持的服务器    Windows Server 2008 [仅限桌面应用]
    __DEFINE_THUNK(
    mfplat,
    12,
    HRESULT,
    STDAPICALLTYPE,
    MFPutWorkItem,
        DWORD _uQueue,
        IMFAsyncCallback* _pCallback,
        IUnknown* _pState
        )
    {
        if (const auto _pfnMFPutWorkItem = try_get_MFPutWorkItem())
        {
            return _pfnMFPutWorkItem(_uQueue, _pCallback, _pState);
        }

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    16,
    HRESULT,
    STDAPICALLTYPE,
    MFPutWorkItem2,
        DWORD _uQueue,
        LONG _nPriority,
        _In_ IMFAsyncCallback* _pCallback,
        _In_opt_ IUnknown* _pState
        )
    {
        if (const auto _pfnMFPutWorkItem2 = try_get_MFPutWorkItem2())
        {
            return _pfnMFPutWorkItem2(_uQueue, _nPriority, _pCallback, _pState);
        }
        return MFPutWorkItem(_uQueue, _pCallback, _pState);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    16,
    HRESULT,
    STDAPICALLTYPE,
    MFPutWaitingWorkItem,
        HANDLE _hEvent,
        LONG _nPriority,
        _In_ IMFAsyncResult* _pResult,
        _Out_opt_ MFWORKITEM_KEY* _pKey
        )
    {
        if (const auto _pfnMFPutWaitingWorkItem = try_get_MFPutWaitingWorkItem())
        {
            return _pfnMFPutWaitingWorkItem(_hEvent, _nPriority, _pResult, _pKey);
        }
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 [桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    mfplat,
    4,
    HRESULT,
    STDAPICALLTYPE,
    MFUnlockWorkQueue,
        _In_ DWORD _uWorkQueue
        )
    {
        if (const auto _pfnMFUnlockWorkQueue = try_get_MFUnlockWorkQueue())
        {
            return _pfnMFUnlockWorkQueue(_uWorkQueue);
        }

        return E_NOTIMPL;
    }
#endif
} // namespace YY::Thunks
