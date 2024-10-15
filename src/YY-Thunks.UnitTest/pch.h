// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

#include <sal.h>

// 添加要在此处预编译的标头
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define UMDF_USING_NTSTATUS
#define _Disallow_YY_KM_Namespace
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#include <km.h>

#include <Windows.h>

#include <atltime.h>

#include "CppUnitTest.h"
#include <process.h>
#include <vector>
#include <string>

#include <SharedDefs.h>

EXTERN_C uint64_t g_uSystemVersion;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define __DEFINE_THUNK(_MODULE, _SIZE, _RETURN_, _CONVENTION_, _FUNCTION, ...)     \
    extern bool _CRT_CONCATENATE(aways_null_try_get_, _FUNCTION);                  \
    EXTERN_C _RETURN_ _CONVENTION_ _FUNCTION(__VA_ARGS__);                         \
    __if_not_exists(_FUNCTION)

//#undef TEST_METHOD
//
//#define TEST_METHOD(methodName)\
//	static const EXPORT_METHOD ::Microsoft::VisualStudio::CppUnitTestFramework::MemberMethodInfo* CALLING_CONVENTION CATNAME(__GetTestMethodInfo_, __COUNTER__)()\
//	{\
//		__GetTestClassInfo();\
//		__GetTestVersion();\
//		ALLOCATE_TESTDATA_SECTION_METHOD\
//		static const ::Microsoft::VisualStudio::CppUnitTestFramework::MethodMetadata s_Metadata = {L"TestMethodInfo", L## #methodName, reinterpret_cast<const unsigned char*>(__FUNCTION__), reinterpret_cast<const unsigned char*>(__FUNCDNAME__), __WFILE__, __LINE__};\
//\
//		static ::Microsoft::VisualStudio::CppUnitTestFramework::MemberMethodInfo s_Info = {::Microsoft::VisualStudio::CppUnitTestFramework::MemberMethodInfo::TestMethod, {NULL}, &s_Metadata};\
//		s_Info.method.pVoidMethod = static_cast<::Microsoft::VisualStudio::CppUnitTestFramework::TestClassImpl::__voidFunc>(&ThisClass::methodName);\
//		return &s_Info;\
//	}\
//	void methodName()


class AwaysNullGuard
{
private:
    std::vector<bool*> GuardAddress;

public:

    ~AwaysNullGuard()
    {
        for (auto& _pValue : GuardAddress)
        {
            *_pValue = false;
        }
    }

    void Add(bool& _bValue)
    {
        if (_bValue)
        {
            // 已经是true
            return;
        }
        _bValue = true;
        GuardAddress.push_back(&_bValue);
    }

    void operator|=(bool& _bValue)
    {
        Add(_bValue);
    }
};

inline std::string ToHexString(const void* _pData, size_t _cbData)
{
    std::string _szResult;
    _szResult.reserve(_cbData * 2);
    constexpr const char kHex[] = "0123456789ABCDEF";

    for (auto _pItem = reinterpret_cast<const BYTE*>(_pData); _cbData; --_cbData, ++_pItem)
    {
        _szResult += kHex[*_pItem >> 4];
        _szResult += kHex[*_pItem & 0xFu];
    }

    return _szResult;
}

inline std::string ToHexString(const std::vector<BYTE> _Data)
{
    return ToHexString(_Data.data(), _Data.size());
}

template<size_t kDataLength>
inline std::string ToHexString(const BYTE (&_Data)[kDataLength])
{
    return ToHexString(_Data, kDataLength);
}

__forceinline constexpr uint64_t __fastcall MakeVersion(_In_ uint16_t _uMajor, _In_ uint16_t _uMinor, uint16_t _uBuild = 0, UINT16 _uRevision = 0)
{
    uint64_t _uVersion = uint64_t(_uMajor) << 48;
    _uVersion |= uint64_t(_uMinor) << 32;
    _uVersion |= uint64_t(_uBuild) << 16;
    _uVersion |= _uRevision;
    return _uVersion;
}

std::string ReadFileData(LPCWSTR _szFilePath);

namespace Microsoft::VisualStudio::CppUnitTestFramework
{
    template<>
    inline std::wstring ToString<GUID>(const GUID& t)
    {
        wchar_t _szResult[128] = {};

        swprintf_s(_szResult, L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", t.Data1, t.Data2, t.Data3,
            t.Data4[0], t.Data4[1], t.Data4[2], t.Data4[3], t.Data4[4], t.Data4[5], t.Data4[7], t.Data4[7]);

        return _szResult;
    }
}

#endif //PCH_H
