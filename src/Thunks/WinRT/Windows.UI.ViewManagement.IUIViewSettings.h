#pragma once

#include <inspectable.h>

// Intentionally does NOT include <windows.ui.viewmanagement.h>. That header transitively pulls in
// almost the entire WinRT API, and its ~1,800 `__declspec(selectany)` RuntimeClass_* strings all end
// up in the obj, inflating the libs several times over. They are also linked into the final binary
// when /OPT:NOREF is in effect (the Debug default). See Issue #186.
// Only the minimal ABI actually used is declared here, in a separate namespace so it can never
// conflict with the SDK definitions. Consistency with the SDK is enforced by static_asserts in
// YY-Thunks.UnitTest/api-ms-win-core-winrt.UnitTest.cpp.
namespace YY::Thunks::WinRT::Windows::UI::ViewManagement
{
    enum UserInteractionMode : int
    {
        UserInteractionMode_Mouse = 0,
        UserInteractionMode_Touch = 1,
    };

    MIDL_INTERFACE("c63657f6-8850-470d-88f8-455e16ea2c26")
    IUIViewSettings : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_UserInteractionMode(
            _Out_ UserInteractionMode* value
            ) = 0;
    };
} // namespace YY::Thunks::WinRT::Windows::UI::ViewManagement
