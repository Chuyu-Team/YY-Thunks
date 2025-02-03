#if (YY_Thunks_Target < __WindowsNT6_3)
#include <combaseapi.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_1) && !defined(__Comment_Lib_ole32)
#define __Comment_Lib_ole32
#pragma comment(lib, "ole32.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_1)

    // Windows 7 [桌面应用|UWP 应用], Windows Server 2008 R2 [桌面应用|UWP 应用]
    __DEFINE_THUNK(
    ole32,
    8,
    HRESULT,
    WINAPI,
    CoGetApartmentType,
        _Out_ APTTYPE* _pAptType,
        _Out_ APTTYPEQUALIFIER* _pAptQualifier
        )
    {
        if (auto _pfnCoGetApartmentType = try_get_CoGetApartmentType())
        {
            return _pfnCoGetApartmentType(_pAptType, _pAptQualifier);
        }

        if (_pAptType == nullptr || _pAptQualifier == nullptr)				
            return E_INVALIDARG;

        *_pAptType = APTTYPE_CURRENT;
        *_pAptQualifier = APTTYPEQUALIFIER_NONE;

        IUnknown* _pContextToken;
        auto _hr = CoGetContextToken((ULONG_PTR*)&_pContextToken);
        if (FAILED(_hr))
            return _hr;

        IComThreadingInfo* _pComThreadingInfo;
        _hr = _pContextToken->QueryInterface(&_pComThreadingInfo);
        if (FAILED(_hr))
            return _hr;

        _hr = _pComThreadingInfo->GetCurrentApartmentType(_pAptType);
        _pComThreadingInfo->Release();
        if (FAILED(_hr))
            return _hr;

        switch (*_pAptType)
        {
        case APTTYPE_STA:
            *_pAptQualifier = APTTYPEQUALIFIER_APPLICATION_STA;
            break;
        case APTTYPE_MTA:
            // todo: 无法区分：
            // APTTYPEQUALIFIER_NONE
            // APTTYPEQUALIFIER_IMPLICIT_MTA
            break;
        case APTTYPE_NA:
            // todo: 无法区分
            // APTTYPEQUALIFIER_NA_ON_MTA
            // APTTYPEQUALIFIER_NA_ON_STA
            // APTTYPEQUALIFIER_NA_ON_IMPLICIT_MTA
            // APTTYPEQUALIFIER_NA_ON_MAINSTA
            break;
        case APTTYPE_MAINSTA:
            break;
        default:
            break;
        }

        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_3)

    // 最低受支持的客户端	Windows 8.1 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012 R2[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    ole32,
    16,
    HRESULT,
    WINAPI,
    RoGetAgileReference,
        _In_ enum AgileReferenceOptions _eOptions,
        _In_ REFIID _Id,
        _In_ IUnknown* pUnk,
        _COM_Outptr_ IAgileReference** _ppAgileReference
        )
    {
        if (const auto _pfnRoGetAgileReference = try_get_RoGetAgileReference())
        {
            return _pfnRoGetAgileReference(_eOptions, _Id, pUnk, _ppAgileReference);
        }

        if (!_ppAgileReference)
            return E_POINTER;

        *_ppAgileReference = nullptr;
        if (!pUnk)
            return E_POINTER;

        return E_NOINTERFACE;
    }
#endif
}
