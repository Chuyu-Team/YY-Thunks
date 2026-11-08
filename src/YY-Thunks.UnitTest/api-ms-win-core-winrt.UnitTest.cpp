#include "pch.h"

#include "Thunks/api-ms-win-core-winrt.hpp"
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>

#include <windows.ui.viewmanagement.h>
#include <UIViewSettingsInterop.h>

#include "Thunks/WinRT/Windows.UI.ViewManagement.IUIViewSettings.h"

// YY-Thunks declares its own minimal IUIViewSettings ABI to avoid pulling in the huge WinRT headers
// (Issue #186). Make sure it stays in sync with the SDK definitions.
namespace
{
    namespace Sdk = ABI::Windows::UI::ViewManagement;
    namespace Local = YY::Thunks::WinRT::Windows::UI::ViewManagement;

    constexpr bool IsSameGuid(const GUID& _Left, const GUID& _Right)
    {
        if (_Left.Data1 != _Right.Data1 || _Left.Data2 != _Right.Data2 || _Left.Data3 != _Right.Data3)
            return false;

        for (int i = 0; i != 8; ++i)
        {
            if (_Left.Data4[i] != _Right.Data4[i])
                return false;
        }
        return true;
    }

    static_assert(IsSameGuid(__uuidof(Local::IUIViewSettings), __uuidof(Sdk::IUIViewSettings)), "IUIViewSettings IID does not match the SDK");
    static_assert(sizeof(Local::UserInteractionMode) == sizeof(Sdk::UserInteractionMode), "UserInteractionMode size does not match the SDK");
    static_assert(static_cast<int>(Local::UserInteractionMode_Mouse) == static_cast<int>(Sdk::UserInteractionMode_Mouse), "UserInteractionMode_Mouse does not match the SDK");
    static_assert(static_cast<int>(Local::UserInteractionMode_Touch) == static_cast<int>(Sdk::UserInteractionMode_Touch), "UserInteractionMode_Touch does not match the SDK");
    static_assert(sizeof(Local::IUIViewSettings) == sizeof(Sdk::IUIViewSettings), "IUIViewSettings layout does not match the SDK");
    static_assert(std::is_same_v<decltype(&Local::IUIViewSettings::get_UserInteractionMode), HRESULT(STDMETHODCALLTYPE Local::IUIViewSettings::*)(Local::UserInteractionMode*)>, "get_UserInteractionMode signature mismatch");
    static_assert(std::is_same_v<decltype(&Sdk::IUIViewSettings::get_UserInteractionMode), HRESULT(STDMETHODCALLTYPE Sdk::IUIViewSettings::*)(Sdk::UserInteractionMode*)>, "SDK get_UserInteractionMode signature has changed");
}

#pragma comment(linker, "/INCLUDE:_RoGetActivationFactory@Windows.UI.ViewManagement.UIViewSettings")

namespace Microsoft {
namespace VisualStudio {
namespace CppUnitTestFramework {

template<>
inline std::wstring ToString<ABI::Windows::UI::ViewManagement::UserInteractionMode>(const ABI::Windows::UI::ViewManagement::UserInteractionMode& _oValue)
{
    switch (_oValue)
    {
    case ABI::Windows::UI::ViewManagement::UserInteractionMode_Mouse:
        return L"Mouse";
    case ABI::Windows::UI::ViewManagement::UserInteractionMode_Touch:
        return L"Touch";
    default:
        return L"Unknown";
    }
}

} // namespace CppUnitTestFramework
} // namespace VisualStudio
} // namespace Microsoft

namespace api_ms_win_core_winrt
{
    TEST_CLASS(RoGetActivationFactory)
    {
        AwaysNullGuard Guard;

    public:
        RoGetActivationFactory()
        {
            Guard |= YY::Thunks::aways_null_try_get_RoGetActivationFactory;
        }

        TEST_METHOD(当ClassId不匹配时应返回REGDB_E_CLASSNOTREG)
        {
            Microsoft::WRL::ComPtr<IUIViewSettingsInterop> pInterop;
            const auto hr = ::RoGetActivationFactory(
                Microsoft::WRL::Wrappers::HString::MakeReference(L"Windows.UI.ViewManagement.INotSupported").Get(),
                __uuidof(IUIViewSettingsInterop),
                reinterpret_cast<void**>(pInterop.ReleaseAndGetAddressOf()));

            Assert::AreEqual(REGDB_E_CLASSNOTREG, hr);
            Assert::IsNull(pInterop.Get());
        }

        TEST_METHOD(当Iid不匹配时应返回E_NOINTERFACE)
        {
            IUnknown* pFactory = reinterpret_cast<IUnknown*>(1);
            const auto hr = ::RoGetActivationFactory(
                Microsoft::WRL::Wrappers::HString::MakeReference(L"Windows.UI.ViewManagement.UIViewSettings").Get(),
                IID_IClassFactory,
                reinterpret_cast<void**>(&pFactory));

            Assert::AreEqual(E_NOINTERFACE, hr);
            Assert::IsNull(pFactory);
        }

        TEST_METHOD(IUIViewSettingsInterop对象创建)
        {
            Microsoft::WRL::ComPtr<IUIViewSettingsInterop> pInterop;
            const auto hr = ::RoGetActivationFactory(
                Microsoft::WRL::Wrappers::HString::MakeReference(L"Windows.UI.ViewManagement.UIViewSettings").Get(),
                __uuidof(IUIViewSettingsInterop),
                reinterpret_cast<void**>(pInterop.GetAddressOf()));

            Assert::AreEqual(S_OK, hr);
            Assert::IsNotNull(pInterop.Get());

            Microsoft::WRL::ComPtr<IUnknown> pUnknown;
            Assert::AreEqual(S_OK, pInterop->QueryInterface(pUnknown.ReleaseAndGetAddressOf()));
            Assert::IsNotNull(pUnknown.Get());
        }

        TEST_METHOD(IUIViewSettings对象创建)
        {
            Microsoft::WRL::ComPtr<IUIViewSettingsInterop> pInterop;
            auto hr = ::RoGetActivationFactory(
                Microsoft::WRL::Wrappers::HString::MakeReference(L"Windows.UI.ViewManagement.UIViewSettings").Get(),
                __uuidof(IUIViewSettingsInterop),
                reinterpret_cast<void**>(pInterop.ReleaseAndGetAddressOf()));

            Assert::AreEqual(S_OK, hr);
            Assert::IsNotNull(pInterop.Get());

            Microsoft::WRL::ComPtr<ABI::Windows::UI::ViewManagement::IUIViewSettings> pUIViewSettings;
            hr = pInterop->GetForWindow(GetDesktopWindow(), __uuidof(ABI::Windows::UI::ViewManagement::IUIViewSettings), reinterpret_cast<void**>(pUIViewSettings.ReleaseAndGetAddressOf()));

            Assert::AreEqual(S_OK, hr);
            Assert::IsNotNull(pUIViewSettings.Get());

            ABI::Windows::UI::ViewManagement::UserInteractionMode InteractionMode =
                ABI::Windows::UI::ViewManagement::UserInteractionMode_Touch;
            Assert::AreEqual(S_OK, pUIViewSettings->get_UserInteractionMode(&InteractionMode));
            Assert::AreEqual(
                ABI::Windows::UI::ViewManagement::UserInteractionMode_Mouse,
                InteractionMode);
        }
    };
}
