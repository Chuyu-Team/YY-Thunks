#if (YY_Thunks_Target < __WindowsNT6)
#include <propvarutil.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_oleaut32)
#define __Comment_Lib_oleaut32
// SafeArrayGetElement、SafeArrayGetLBound、SafeArrayGetUBound、VariantClear、VarCmp
#pragma comment(lib, "OleAut32.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_ole32)
#define __Comment_Lib_ole32
// CoTaskMemAlloc
#pragma comment(lib, "ole32.lib")
#endif

#ifdef YY_Thunks_Implemented
namespace YY::Thunks::internal
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT6)
        static HRESULT __fastcall SafeArrayGetCount(SAFEARRAY* _pArray, LONG* _pnLbound)
        {
            *_pnLbound = 0;
            if (SafeArrayGetDim(_pArray) != 1)
                return 0x80028CA0;

            LONG _nLBound = 0;
            auto _hr = SafeArrayGetLBound(_pArray, 1u, &_nLBound);
            if (FAILED(_hr))
                return _hr;

            LONG _nUBound = 0;
            _hr = SafeArrayGetUBound(_pArray, 1u, &_nUBound);
            if (FAILED(_hr))
                return _hr;

            if (_nUBound >= _nLBound)
                *_pnLbound = _nUBound - _nLBound + 1;

            return S_OK;
        }

        static constexpr DWORD __fastcall MakeVarTypePair(VARTYPE _eLeft, VARTYPE _eRigth)
        {
            return (DWORD(UINT16(_eRigth)) << 16) | UINT16(_eLeft);
        }

        struct FastValue
        {
            union
            {
                ULONGLONG ullVal;
                LONGLONG llVal;
                DOUBLE dblVal;
            };

            VARTYPE vt;
        };

        static FastValue TryGetFastValue(const VARIANT& _Value)
        {
            FastValue _Resutl = {};
            switch (_Value.vt)
            {
            case VT_UI1:
                _Resutl.vt = VT_UI8;
                _Resutl.ullVal = _Value.bVal;
                break;
            case VT_UI2:
                _Resutl.vt = VT_UI8;
                _Resutl.ullVal = _Value.uiVal;
                break;
            case VT_UI4:
                _Resutl.vt = VT_UI8;
                _Resutl.ullVal = _Value.ulVal;
                break;
            case VT_UI8:
                _Resutl.vt = VT_UI8;
                _Resutl.ullVal = _Value.ulVal;
                break;
            case VT_UINT:
                _Resutl.vt = VT_UI8;
                _Resutl.ullVal = _Value.uintVal;
                break;
            case VT_I1:
                _Resutl.vt = VT_I8;
                _Resutl.llVal = _Value.cVal;
                break;
            case VT_I2:
                _Resutl.vt = VT_I8;
                _Resutl.llVal = _Value.iVal;
                break;
            case VT_I4:
                _Resutl.vt = VT_I8;
                _Resutl.llVal = _Value.lVal;
                break;
            case VT_I8:
                _Resutl.vt = VT_I8;
                _Resutl.llVal = _Value.lVal;
                break;
            case VT_INT:
                _Resutl.vt = VT_I8;
                _Resutl.llVal = _Value.intVal;
                break;
            case VT_R4:
                _Resutl.vt = VT_R8;
                _Resutl.dblVal = _Value.fltVal;
                break;
            case VT_R8:
                _Resutl.vt = VT_R8;
                _Resutl.dblVal = _Value.dblVal;
                break;
            }

            return _Resutl;
        }

        static int CompareFastValue(const FastValue& _Left, const FastValue& _Rigth)
        {
            // 虽然这里有比较浮点，但是微软原版就是直接==比较浮点
            // 我们也不引入浮点误差。
            if (_Left.vt == VT_UI8)
            {
                switch (_Rigth.vt)
                {
                case VT_I8:
                    if (_Rigth.llVal < 0ll)
                        return 1;
                case VT_UI8:
                    if (_Left.ullVal < _Rigth.ullVal)
                    {
                        return -1;
                    }
                    else if (_Left.ullVal > _Rigth.ullVal)
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                    break;
                case VT_R8:
                    if (_Rigth.dblVal < 0.0)
                    {
                        return 1;
                    }
                    else
                    {
                        auto _uTmp = ULONG64(_Rigth.dblVal);
                        if (_Left.ullVal < _uTmp)
                        {
                            return -1;
                        }
                        else if (_Left.ullVal > _uTmp)
                        {
                            return 1;
                        }
                        else
                        {
                            return double(_Left.ullVal) == _Rigth.dblVal ? 0 : -1;
                        }
                    }
                    break;
                }
            }
            else if (_Left.vt == VT_I8)
            {
                switch (_Rigth.vt)
                {
                case VT_I8:
                    if (_Left.llVal < _Rigth.llVal)
                    {
                        return -1;
                    }
                    else if (_Left.llVal > _Rigth.llVal)
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                    break;
                case VT_UI8:
                    if (_Left.llVal < 0)
                        return -1;

                    if (_Left.ullVal < _Rigth.ullVal)
                    {
                        return -1;
                    }
                    else if (_Left.ullVal > _Rigth.ullVal)
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                    break;
                case VT_R8:
                {
                    auto _uTmp = LONG64(_Rigth.dblVal);
                    if (_Left.llVal < _uTmp)
                    {
                        return -1;
                    }
                    else if (_Left.llVal > _uTmp)
                    {
                        return 1;
                    }
                    else
                    {
                        return double(_Left.llVal) == _Rigth.dblVal ? 0 : -1;
                    }
                    break;
                }
                }
            }
            else if (_Left.vt == VT_R8)
            {
                switch (_Rigth.vt)
                {
                case VT_I8:
                {
                    LONG64 _Tmp = LONG64(_Left.dblVal);
                    if (_Tmp < _Rigth.llVal)
                    {
                        return -1;
                    }
                    else if (_Tmp > _Rigth.llVal)
                    {
                        return 1;
                    }
                    else
                    {
                        return _Left.dblVal == double(_Rigth.llVal) ? 0 : 1;
                    }
                    break;
                }
                case VT_UI8:
                    if (_Left.dblVal < 0.0)
                    {
                        return -1;
                    }
                    else
                    {
                        ULONG64 _Tmp = LONG64(_Left.dblVal);
                        if (_Tmp < _Rigth.ullVal)
                        {
                            return -1;
                        }
                        else if (_Tmp > _Rigth.ullVal)
                        {
                            return 1;
                        }
                        else
                        {
                            return _Left.dblVal == double(_Rigth.ullVal) ? 0 : 1;
                        }
                    } 
                    break;
                case VT_R8:
                    if (_Left.dblVal < _Rigth.dblVal)
                    {
                        return -1;
                    }
                    else if (_Left.dblVal > _Rigth.dblVal)
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                    break;
                }
            }

            return 0;
        }
#endif
    }
}
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP SP2、Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2003 SP1[仅限桌面应用]
    // XP、2003需要安装Windows 桌面搜索 (WDS) 3.0
    __DEFINE_THUNK(
    propsys,
    8,
    HRESULT,
    STDAPICALLTYPE,
    InitPropVariantFromCLSID,
        _In_ REFCLSID _ClsId,
        _Out_ PROPVARIANT* _pPropVar
        )
    {
        if (const auto _pfnInitPropVariantFromCLSID = try_get_InitPropVariantFromCLSID())
        {
            return _pfnInitPropVariantFromCLSID(_ClsId, _pPropVar);
        }
        
        _pPropVar->vt = VT_EMPTY;
        auto _pId = (GUID*)CoTaskMemAlloc(sizeof(_ClsId));
        if (!_pId)
            return E_OUTOFMEMORY;

        *_pId = _ClsId;
        _pPropVar->vt = VT_CLSID;
        _pPropVar->puuid = _pId;
        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP SP2、Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2003 SP1[仅限桌面应用]
    // XP、2003需要安装Windows 桌面搜索 (WDS) 3.0
    __DEFINE_THUNK(
    propsys,
    8,
    HRESULT,
    STDAPICALLTYPE,
    PSGetPropertyKeyFromName,
        _In_ PCWSTR _szName,
        _Out_ PROPERTYKEY* _pPropKey
        )
    {
        if (const auto _pfnPSGetPropertyKeyFromName = try_get_PSGetPropertyKeyFromName())
        {
            return _pfnPSGetPropertyKeyFromName(_szName, _pPropKey);
        }
        
        if (_szName == nullptr)
            return E_INVALIDARG;

        return TYPE_E_ELEMENTNOTFOUND;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP SP2、Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2003 SP1[仅限桌面应用]
    // XP、2003需要安装Windows 桌面搜索 (WDS) 3.0
    __DEFINE_THUNK(
    propsys,
    8,
    HRESULT,
    STDAPICALLTYPE,
    PSCreateMemoryPropertyStore,
        _In_ REFIID riid,
        _Outptr_ void** ppv
        )
    {
        if (const auto _pfnPSCreateMemoryPropertyStore = try_get_PSCreateMemoryPropertyStore())
        {
            return _pfnPSCreateMemoryPropertyStore(riid, ppv);
        }
        
        if (!ppv)
            return E_POINTER;
        *ppv = nullptr;
        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP SP2、Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2003 SP1[仅限桌面应用]
    // XP、2003需要安装Windows 桌面搜索 (WDS) 3.0
    __DEFINE_THUNK(
    propsys,
    8,
    int,
    WINAPI,
    VariantCompare,
        _In_ REFVARIANT _Left,
        _In_ REFVARIANT _Rigth
        )
    {
        if (const auto _pfnVariantCompare = try_get_VariantCompare())
        {
            return _pfnVariantCompare(_Left, _Rigth);
        }
        
        // VT_EMPTY VT_NULL小于任何类型
        if (_Left.vt == VT_EMPTY || _Left.vt == VT_NULL)
        {
            if (_Rigth.vt == VT_EMPTY || _Rigth.vt == VT_NULL)
                return 0;

            return -1;
        }
        else
        {
            if (_Rigth.vt == VT_EMPTY || _Rigth.vt == VT_NULL)
                return 1;
        }

        if ((_Left.vt & VT_ARRAY) && (_Rigth.vt & VT_ARRAY))
        {
            LONG _nLeftCount;
            internal::SafeArrayGetCount(_Left.parray, &_nLeftCount);
            LONG _nRigthCount;
            internal::SafeArrayGetCount(_Rigth.parray, &_nRigthCount);

            const auto _nCount = min(_nLeftCount, _nRigthCount);
            for (LONG i = 0; i != _nCount; ++i)
            {
                VARIANT _LeftItem = {};
                if (FAILED(SafeArrayGetElement(_Left.parray, &i, &_LeftItem)))
                    continue;

                VARIANT _RightItem = {};
                int _nResult = 0;
                if (SUCCEEDED(SafeArrayGetElement(_Rigth.parray, &i, &_RightItem)))
                {
                    _nResult = _CRT_CONCATENATE(__FALLBACK_PREFIX, VariantCompare)(_LeftItem, _RightItem);
                    VariantClear(&_RightItem);
                }
                VariantClear(&_LeftItem);
                if (_nResult)
                    return _nResult;
            }

            if (_nLeftCount < _nRigthCount)
            {
                return -1;
            }
            else if (_nLeftCount > _nRigthCount)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

        switch (internal::MakeVarTypePair(_Left.vt, _Rigth.vt))
        {
            // BOOL
        case internal::MakeVarTypePair(VT_BOOL, VT_BOOL):
            if (_Left.boolVal)
            {
                return _Rigth.boolVal ? 0 : 1;
            }
            else
            {
                return _Rigth.boolVal ? -1 : 0;
            }
            // 字符串
        case internal::MakeVarTypePair(VT_BSTR, VT_BSTR):
            return StrCmpLogicalW(_Left.bstrVal ? _Left.bstrVal : L"", _Rigth.bstrVal ? _Rigth.bstrVal : L"");
        case internal::MakeVarTypePair(VT_ERROR, VT_ERROR):
            return _Left.scode - _Rigth.scode;
        case internal::MakeVarTypePair(VT_DATE, VT_DATE):
            if (_Left.date < _Rigth.date)
            {
                return -1;
            }
            else if (_Left.date > _Rigth.date)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        //case internal::MakeVarTypePair(VT_CY, VT_CY): VarCmp完成
        default:
        {
            auto _LeftValue = internal::TryGetFastValue(_Left);
            if (_LeftValue.vt != VT_EMPTY)
            {
                auto _RightValue = internal::TryGetFastValue(_Rigth);
                if (_RightValue.vt != VT_EMPTY)
                {
                    return internal::CompareFastValue(_LeftValue, _RightValue);
                }
            }

            // 微软的保底逻辑
            const auto _hr = VarCmp(const_cast<LPVARIANT>(&_Left), const_cast<LPVARIANT>(&_Rigth), GetUserDefaultLCID(), 0);
            if (SUCCEEDED(_hr))
                return _hr - VARCMP_EQ;

            return 0;
        }
        }
    }
#endif
}
