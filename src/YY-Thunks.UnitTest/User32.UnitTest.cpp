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

    TEST_CLASS(SystemParametersInfoForDpi)
    {
        AwaysNullGuard Guard;

    public:
        SystemParametersInfoForDpi()
        {
            Guard |= YY::Thunks::aways_null_try_get_SystemParametersInfoForDpi;
        }

        TEST_METHOD(常规测试)
        {
            struct TestItem
            {
                UINT uAction;
                UINT uParam;
                UINT uDpi;
            };

            static constexpr TestItem kTestItems[] =
            {
                { SPI_GETICONTITLELOGFONT, sizeof(LOGFONTW), 0 },
                { SPI_GETICONTITLELOGFONT, sizeof(LOGFONTW), USER_DEFAULT_SCREEN_DPI },
                { SPI_GETICONTITLELOGFONT, sizeof(LOGFONTW), USER_DEFAULT_SCREEN_DPI * 1.5 },
                { SPI_GETICONTITLELOGFONT, sizeof(LOGFONTW), USER_DEFAULT_SCREEN_DPI * 2 },

                { SPI_GETICONMETRICS, sizeof(ICONMETRICSW), 0 },
                { SPI_GETICONMETRICS, sizeof(ICONMETRICSW), USER_DEFAULT_SCREEN_DPI },
                { SPI_GETICONMETRICS, sizeof(ICONMETRICSW), USER_DEFAULT_SCREEN_DPI * 1.5 },
                { SPI_GETICONMETRICS, sizeof(ICONMETRICSW), USER_DEFAULT_SCREEN_DPI * 2 },

                { SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), 0 },
                { SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), USER_DEFAULT_SCREEN_DPI },
                { SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), USER_DEFAULT_SCREEN_DPI * 1.5 },
                { SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), USER_DEFAULT_SCREEN_DPI * 2 },
            };

            for (auto& _Item : kTestItems)
            {
                union
                {
                    LOGFONTW LogFont;
                    ICONMETRICSW IconMeterics;
                    NONCLIENTMETRICSW NonClientMeterocs;
                } _Target, _Current;

                if (SPI_GETICONTITLELOGFONT != _Item.uAction)
                {
                    _Target.IconMeterics.cbSize = _Item.uParam;
                    _Current.IconMeterics.cbSize = _Item.uParam;
                }
                ::SystemParametersInfoForDpi(_Item.uAction, _Item.uParam, &_Current, 0, _Item.uDpi);

                YY::Thunks::aways_null_try_get_SystemParametersInfoForDpi = false;
                ::SystemParametersInfoForDpi(_Item.uAction, _Item.uParam, &_Target, 0, _Item.uDpi);
                YY::Thunks::aways_null_try_get_SystemParametersInfoForDpi = true;

                CStringW _szMessage;
                _szMessage.Format(L"Action = %d", _Item.uAction);
                Assert::IsTrue(memcmp(&_Current, &_Target, _Item.uParam) == 0, _szMessage.GetString());
            }
        }
    };

    TEST_CLASS(GetSystemMetricsForDpi)
    {
    public:
        GetSystemMetricsForDpi()
        {
        }

        TEST_METHOD(常规测试)
        {
            for (int _uIndex = 0; _uIndex != 100; ++_uIndex)
            {
                if (SM_CXMIN == _uIndex || SM_CYMIN == _uIndex
                    || SM_CXMINTRACK == _uIndex || SM_CYMINTRACK == _uIndex
                    || SM_CXMENUCHECK == _uIndex || SM_CYMENUCHECK == _uIndex)
                {
                    // 暂时无法做到完全一致
                    continue;
                }

                const auto _nTarget = ::GetSystemMetricsForDpi(_uIndex, USER_DEFAULT_SCREEN_DPI * 2);
                YY::Thunks::aways_null_try_get_GetSystemMetricsForDpi = true;
                const auto _nCurrent = ::GetSystemMetricsForDpi(_uIndex, USER_DEFAULT_SCREEN_DPI * 2);
                YY::Thunks::aways_null_try_get_GetSystemMetricsForDpi = false;

                CStringW _szMessage;
                _szMessage.Format(L"nIndex = %d", _uIndex);

                Assert::AreEqual(_nTarget, _nCurrent, _szMessage.GetString());
            }
        }
    };
}
