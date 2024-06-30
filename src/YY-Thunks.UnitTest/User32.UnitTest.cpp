#include "pch.h"
#include "Thunks/user32.hpp"

namespace User32
{
    TEST_CLASS(AdjustWindowRectExForDpi)
    {
        AwaysNullGuard Guard;

    public:
        AdjustWindowRectExForDpi()
        {
            Guard |= YY::Thunks::aways_null_try_get_AdjustWindowRectExForDpi;
        }

        TEST_METHOD(常规测试)
        {
            struct TestItem
            {
                DWORD fStyle;
                BOOL bMenu;
                DWORD fExStyle;
                UINT uDpi;
            };

            static constexpr const TestItem kTestItems[] =
            {
                { WS_DLGFRAME, TRUE, 0, USER_DEFAULT_SCREEN_DPI },
                { WS_DLGFRAME, TRUE, 0, USER_DEFAULT_SCREEN_DPI * 2 },
                { WS_DLGFRAME, FALSE, 0, USER_DEFAULT_SCREEN_DPI * 2 },
                { WS_THICKFRAME, TRUE, 0, USER_DEFAULT_SCREEN_DPI },
                { WS_THICKFRAME, TRUE, 0, USER_DEFAULT_SCREEN_DPI * 2 },
                { WS_THICKFRAME, FALSE, 0, USER_DEFAULT_SCREEN_DPI * 2 },
                { WS_THICKFRAME | WS_CAPTION, TRUE, 0, USER_DEFAULT_SCREEN_DPI * 2 },
                { WS_THICKFRAME | WS_CAPTION, FALSE, 0, USER_DEFAULT_SCREEN_DPI * 2 },
                { WS_CAPTION, TRUE, 0, USER_DEFAULT_SCREEN_DPI * 1.25 },
                { WS_CAPTION, FALSE, 0, USER_DEFAULT_SCREEN_DPI * 2 },
                { WS_OVERLAPPEDWINDOW, TRUE, 0, USER_DEFAULT_SCREEN_DPI },
                { WS_OVERLAPPEDWINDOW, TRUE, 0, USER_DEFAULT_SCREEN_DPI * 1.5 },
                { WS_OVERLAPPEDWINDOW, FALSE, 0, USER_DEFAULT_SCREEN_DPI * 2 },
            };

            for (auto& _Item : kTestItems)
            {
                RECT _Current = {};
                ::AdjustWindowRectExForDpi(&_Current, _Item.fStyle, _Item.bMenu, _Item.fExStyle, _Item.uDpi);

                RECT _Target = {};
                YY::Thunks::aways_null_try_get_AdjustWindowRectExForDpi = false;
                ::AdjustWindowRectExForDpi(&_Target, _Item.fStyle, _Item.bMenu, _Item.fExStyle, _Item.uDpi);
                YY::Thunks::aways_null_try_get_AdjustWindowRectExForDpi = true;

                CStringW _szTarget;
                CStringW _szCurrent;
                Assert::AreEqual(_szTarget.GetString(), _szCurrent.GetString());
            }
        }

        TEST_METHOD(畸形测试)
        {
            RECT _Current = {};
            Assert::IsFalse(::AdjustWindowRectExForDpi(&_Current, 0, 0, 0, 0));
        }
    };
}
