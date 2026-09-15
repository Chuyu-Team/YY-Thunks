#include "pch.h"

#include "Thunks/api-ms-win-core-winrt.hpp"
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>

#include <windows.ui.viewmanagement.h>
#include <UIViewSettingsInterop.h>

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
