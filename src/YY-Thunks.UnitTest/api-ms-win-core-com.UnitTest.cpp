#include "pch.h"
#include "Thunks/api-ms-win-core-com.hpp"

#include <objidl.h>
#include <wrl/client.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

// CoTaskMemAlloc, CoTaskMemFree, CoTaskMemRealloc and CoCreateFreeThreadedMarshaler exist in
// ole32 on every supported system. YY-Thunks intercepts them only so that importers that name
// combase.dll (windows-rs >= 0.62, windows-sys >= 0.61) still load on Windows 7 and earlier
// (issue #187). There is no fallback branch, so these tests cover binding and pass-through behavior.

namespace api_ms_win_core_com
{
    // Returns true if _pAddress is inside the current test module, that is, the call was bound
    // to the YY-Thunks thunk instead of a static import.
    static bool IsInCurrentModule(const void* _pAddress)
    {
        HMODULE _hModule = nullptr;
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)_pAddress, &_hModule))
            return false;

        return _hModule == (HMODULE)&__ImageBase;
    }

    TEST_CLASS(CoTaskMemAlloc)
    {
    public:
        TEST_METHOD(导入绑定_指向YY_Thunks)
        {
            Assert::IsTrue(IsInCurrentModule(reinterpret_cast<const void*>(&::CoTaskMemAlloc)));
        }

        TEST_METHOD(常规分配_成功)
        {
            auto _pBuffer = static_cast<BYTE*>(::CoTaskMemAlloc(64));
            Assert::IsNotNull(_pBuffer);

            memset(_pBuffer, 0xA5, 64);
            ::CoTaskMemFree(_pBuffer);
        }

        TEST_METHOD(与IMalloc互通_同一分配器)
        {
            Microsoft::WRL::ComPtr<IMalloc> _pMalloc;
            Assert::AreEqual(S_OK, CoGetMalloc(1, &_pMalloc));

            auto _pBuffer = ::CoTaskMemAlloc(32);
            Assert::IsNotNull(_pBuffer);
            Assert::AreEqual(1, _pMalloc->DidAlloc(_pBuffer));
            Assert::IsTrue(_pMalloc->GetSize(_pBuffer) >= 32);
            _pMalloc->Free(_pBuffer);

            _pBuffer = _pMalloc->Alloc(32);
            Assert::IsNotNull(_pBuffer);
            ::CoTaskMemFree(_pBuffer);
        }
    };

    TEST_CLASS(CoTaskMemFree)
    {
    public:
        TEST_METHOD(导入绑定_指向YY_Thunks)
        {
            Assert::IsTrue(IsInCurrentModule(reinterpret_cast<const void*>(&::CoTaskMemFree)));
        }

        TEST_METHOD(释放空指针_无操作)
        {
            ::CoTaskMemFree(nullptr);
        }
    };

    TEST_CLASS(CoTaskMemRealloc)
    {
    public:
        TEST_METHOD(导入绑定_指向YY_Thunks)
        {
            Assert::IsTrue(IsInCurrentModule(reinterpret_cast<const void*>(&::CoTaskMemRealloc)));
        }

        TEST_METHOD(空指针扩容_等价于分配)
        {
            auto _pBuffer = ::CoTaskMemRealloc(nullptr, 16);
            Assert::IsNotNull(_pBuffer);
            ::CoTaskMemFree(_pBuffer);
        }

        TEST_METHOD(扩容_保留原有内容)
        {
            auto _pBuffer = static_cast<BYTE*>(::CoTaskMemAlloc(16));
            Assert::IsNotNull(_pBuffer);
            for (int i = 0; i != 16; ++i)
                _pBuffer[i] = static_cast<BYTE>(i);

            auto _pNewBuffer = static_cast<BYTE*>(::CoTaskMemRealloc(_pBuffer, 4096));
            Assert::IsNotNull(_pNewBuffer);
            for (int i = 0; i != 16; ++i)
                Assert::AreEqual(static_cast<BYTE>(i), _pNewBuffer[i]);

            ::CoTaskMemFree(_pNewBuffer);
        }
    };

    TEST_CLASS(CoCreateFreeThreadedMarshaler)
    {
    public:
        TEST_METHOD(导入绑定_指向YY_Thunks)
        {
            Assert::IsTrue(IsInCurrentModule(reinterpret_cast<const void*>(&::CoCreateFreeThreadedMarshaler)));
        }

        TEST_METHOD(常规创建_支持IMarshal)
        {
            Microsoft::WRL::ComPtr<IUnknown> _pMarshalUnknown;
            Assert::AreEqual(S_OK, ::CoCreateFreeThreadedMarshaler(nullptr, &_pMarshalUnknown));
            Assert::IsNotNull(_pMarshalUnknown.Get());

            Microsoft::WRL::ComPtr<IMarshal> _pMarshal;
            Assert::AreEqual(S_OK, _pMarshalUnknown.As(&_pMarshal));
        }

        TEST_METHOD(输出参数为空_与系统一致)
        {
            auto _pfnSystem = reinterpret_cast<decltype(::CoCreateFreeThreadedMarshaler)*>(
                GetProcAddress(LoadLibraryW(L"ole32.dll"), "CoCreateFreeThreadedMarshaler"));
            Assert::IsNotNull(_pfnSystem);

            const auto _hrSystem = _pfnSystem(nullptr, nullptr);
            Assert::IsTrue(FAILED(_hrSystem));
            Assert::AreEqual(_hrSystem, ::CoCreateFreeThreadedMarshaler(nullptr, nullptr));
        }
    };
}
