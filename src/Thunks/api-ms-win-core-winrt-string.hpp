#if (YY_Thunks_Target < __WindowsNT6_2)
#include <winstring.h>

#include <HStringPrivate.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_2) && __YY_Thunks_libs && !defined(__Comment_Lib_runtimeobject)
#define __Comment_Lib_runtimeobject
// WindowsCreateString
#pragma comment(lib, "runtimeobject.lib")
#endif

namespace YY::Thunks
{

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    12,
    HRESULT,
    STDAPICALLTYPE,
    WindowsCreateString,
        _In_reads_opt_(_cLength) PCNZWCH _sSourceString,
        UINT32 _cLength,
        _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING* _phString
        )
    {
        if (auto const _pfnWindowsCreateString = try_get_WindowsCreateString())
        {
            return _pfnWindowsCreateString(_sSourceString, _cLength, _phString);
        }

        if (!_phString)
        {
            return E_INVALIDARG;
        }

        *_phString = nullptr;
        if (_sSourceString == nullptr && 0 != _cLength)
        {
            return E_POINTER;
        }

        const uint64_t _cbRequiredSize = sizeof(internal::STRING_OPAQUE) + uint64_t(_cLength + 1) * sizeof(WCHAR);
        if (MAXUINT32 < _cbRequiredSize)
        {
            return MEM_E_INVALID_SIZE;
        }

        auto _pStringInternal = reinterpret_cast<internal::PSTRING_OPAQUE>(internal::Alloc((size_t)_cbRequiredSize, HEAP_ZERO_MEMORY));
        if (!_pStringInternal)
        {
            return E_OUTOFMEMORY;
        }

        _pStringInternal->Header.Flags = internal::RuntimeStringFlags::WRHF_NONE;
        _pStringInternal->Header.Length = _cLength;
        _pStringInternal->Header.StringRef = _pStringInternal->String;
        _pStringInternal->RefCount = 1;
        ::memcpy(_pStringInternal->String, _sSourceString, _cLength * sizeof(WCHAR));
        _pStringInternal->String[_cLength] = L'\0';

        *_phString = reinterpret_cast<HSTRING>(_pStringInternal);
        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    16,
    HRESULT,
    STDAPICALLTYPE,
    WindowsCreateStringReference,
        _In_reads_opt_(_cLength + 1) PCWSTR _szSourceString,
        UINT32 _cLength,
        _Out_ HSTRING_HEADER* _phStringHeader,
        _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING* _phString
        )
    {
        if (auto const pfnWindowsCreateStringReference = try_get_WindowsCreateStringReference())
        {
            return pfnWindowsCreateStringReference(_szSourceString, _cLength, _phStringHeader, _phString);
        }

        if (_phString == nullptr || _phStringHeader == nullptr)
        {
            return E_INVALIDARG;
        }
        *_phString = nullptr;

        if (_szSourceString == nullptr && 0 != _cLength)
        {
            return E_POINTER;
        }

        if (_szSourceString)
        {
            if (L'\0' != _szSourceString[_cLength])
            {
                return E_STRING_NOT_NULL_TERMINATED;
            }
        }

        auto _pHeader = reinterpret_cast<internal::PHSTRING_HEADER_INTERNAL>(_phStringHeader);
        ::memset(_pHeader, 0, sizeof(*_pHeader));

        _pHeader->Flags = internal::RuntimeStringFlags::WRHF_STRING_REFERENCE;
        _pHeader->Length = _cLength;
        _pHeader->StringRef = _szSourceString;
        *_phString = reinterpret_cast<HSTRING>(_pHeader);
        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    4,
    HRESULT,
    STDAPICALLTYPE,
    WindowsDeleteString,
        _In_opt_ HSTRING _hString
        )
    {
        if (auto const _pfnWindowsDeleteString = try_get_WindowsDeleteString())
        {
            return _pfnWindowsDeleteString(_hString);
        }

        if (!_hString)
            return S_OK;

        auto _pStringInternal = reinterpret_cast<internal::PSTRING_OPAQUE>(_hString);

        // WRHF_STRING_REFERENCE 表示内存被调用者接管，所以我们无需从栈空间释放它。
        if (internal::HasFlags(_pStringInternal->Header.Flags, internal::RuntimeStringFlags::WRHF_STRING_REFERENCE))
            return S_OK;

        if (0 == ::InterlockedDecrement(&_pStringInternal->RefCount))
            internal::Free(_pStringInternal);

        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    8,
    HRESULT,
    STDAPICALLTYPE,
    WindowsDuplicateString,
        _In_opt_ HSTRING _hString,
        _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING* _phNewString
        )
    {
        if (auto const _pfnWindowsDuplicateString = try_get_WindowsDuplicateString())
        {
            return _pfnWindowsDuplicateString(_hString, _phNewString);
        }

        if (!_phNewString)
            return E_INVALIDARG;

        *_phNewString = nullptr;
        if (!_hString)
            return S_OK;

        auto _pStringInternal = reinterpret_cast<internal::PSTRING_OPAQUE>(_hString);
        if (!internal::HasFlags(_pStringInternal->Header.Flags, internal::RuntimeStringFlags::WRHF_STRING_REFERENCE))
        {
            ::InterlockedIncrement(&_pStringInternal->RefCount);
            *_phNewString = _hString;
            return S_OK;
        }

        // WRHF_STRING_REFERENCE 时内存生命周期被调用者接管，无法简单增加引用计数延长生命周期。
        // 所以需要使用 WindowsCreateString 创建副本。
        return ::WindowsCreateString(_pStringInternal->Header.StringRef, _pStringInternal->Header.Length, _phNewString);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    4,
    UINT32,
    STDAPICALLTYPE,
    WindowsGetStringLen,
        _In_opt_ HSTRING _hString
        )
    {
        if (auto const _pfnWindowsGetStringLen = try_get_WindowsGetStringLen())
        {
            return _pfnWindowsGetStringLen(_hString);
        }

        if (!_hString)
            return 0ul;

        auto _pHeader = reinterpret_cast<internal::PHSTRING_HEADER_INTERNAL>(_hString);
        return _pHeader->Length;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    8,
    PCWSTR,
    STDAPICALLTYPE,
    WindowsGetStringRawBuffer,
        _In_opt_ HSTRING _hString,
        _Out_opt_ UINT32* _pcLength
        )
    {
        if (auto const _pfnWindowsGetStringRawBuffer = try_get_WindowsGetStringRawBuffer())
        {
            return _pfnWindowsGetStringRawBuffer(_hString, _pcLength);
        }

        if (!_hString)
        {
            if (_pcLength)
            {
                *_pcLength = 0;
            }

            return L"";
        }

        auto _pHeader = reinterpret_cast<internal::PHSTRING_HEADER_INTERNAL>(_hString);
        if (_pcLength)
        {
            *_pcLength = _pHeader->Length;
        }

        return _pHeader->StringRef;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    4,
    BOOL,
    STDAPICALLTYPE,
    WindowsIsStringEmpty,
        _In_opt_ HSTRING _hString
        )
    {
        if (auto const _pfnWindowsIsStringEmpty = try_get_WindowsIsStringEmpty())
        {
            return _pfnWindowsIsStringEmpty(_hString);
        }

        auto _pHeader = reinterpret_cast<internal::PHSTRING_HEADER_INTERNAL>(_hString);
        return _pHeader == nullptr || 0ul == _pHeader->Length;      
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    8,
    HRESULT,
    STDAPICALLTYPE,
    WindowsStringHasEmbeddedNull,
        _In_opt_ HSTRING _hString,
        _Out_ BOOL* _pbHasEmbedNull
        )
    {
        if (auto const _pfnWindowsStringHasEmbeddedNull = try_get_WindowsStringHasEmbeddedNull())
        {
            return _pfnWindowsStringHasEmbeddedNull(_hString, _pbHasEmbedNull);
        }

        if (!_pbHasEmbedNull)
            return E_INVALIDARG;

        *_pbHasEmbedNull = FALSE;
        if (!_hString)
            return S_OK;

        auto _pHeader = reinterpret_cast<internal::PHSTRING_HEADER_INTERNAL>(_hString);

        // 已经存在缓存，直接从缓存获取 WRHF_HAS_EMBEDDED_NULLS 即可
        // 这样做是合理的，因为微软将HSTRING定义为一个只读的String。
        if (internal::HasFlags(_pHeader->Flags, internal::RuntimeStringFlags::WRHF_EMBEDDED_NULLS_COMPUTED))
        {
            *_pbHasEmbedNull = internal::HasFlags(_pHeader->Flags, internal::RuntimeStringFlags::WRHF_HAS_EMBEDDED_NULLS);
            return S_OK;
        }

        auto _szEnd = _pHeader->StringRef + _pHeader->Length;
        for (auto _szStr = _pHeader->StringRef; _szStr < _szEnd; ++_szStr)
        {
            if (*_szStr == L'\0')
            {
                constexpr auto kHasEmbedNullFlags = internal::RuntimeStringFlags::WRHF_EMBEDDED_NULLS_COMPUTED | internal::RuntimeStringFlags::WRHF_HAS_EMBEDDED_NULLS;
                InterlockedOr((LONG*)&_pHeader->Flags, LONG(kHasEmbedNullFlags));
                *_pbHasEmbedNull = TRUE;
                return S_OK;
            }
        }

        InterlockedOr((LONG*)&_pHeader->Flags, LONG(internal::RuntimeStringFlags::WRHF_EMBEDDED_NULLS_COMPUTED));
        *_pbHasEmbedNull = FALSE;
        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_string_l1_1_0,
    12,
    HRESULT,
    STDAPICALLTYPE,
    WindowsCompareStringOrdinal,
        _In_opt_ HSTRING _hStringLeft,
        _In_opt_ HSTRING _hStringRigth,
        _Out_ INT32* _pnResult
        )
    {
        if (auto const _pfnWindowsCompareStringOrdinal = try_get_WindowsCompareStringOrdinal())
        {
            return _pfnWindowsCompareStringOrdinal(_hStringLeft, _hStringRigth, _pnResult);
        }

        if (!_pnResult)
            return E_INVALIDARG;

        *_pnResult = 0;
        const auto _hHeaderLeft = reinterpret_cast<internal::PHSTRING_HEADER_INTERNAL>(_hStringLeft);
        const auto _hHeaderRigth = reinterpret_cast<internal::PHSTRING_HEADER_INTERNAL>(_hStringRigth);
        if (_hHeaderLeft && _hHeaderLeft->Length)
        {
            if (_hHeaderRigth && _hHeaderRigth->Length)
            {
                const auto _nResult = ::CompareStringOrdinal(_hHeaderLeft->StringRef, _hHeaderLeft->Length, _hHeaderRigth->StringRef, _hHeaderRigth->Length, FALSE);
                if (_nResult)
                {
                    *_pnResult = _nResult - CSTR_EQUAL;
                }
            }
            else
            {
                *_pnResult = 1;
            }
        }
        else
        {
            *_pnResult = _hHeaderRigth && _hHeaderRigth->Length ? -1 : 0;
        }

        return S_OK;
    }
#endif
}
