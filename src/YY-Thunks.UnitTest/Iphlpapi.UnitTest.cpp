#include "pch.h"
#include "Thunks/Iphlpapi.hpp"


namespace Iphlpapi
{
    TEST_CLASS(GetIfTable2)
    {
        AwaysNullGuard Guard;

    public:
        GetIfTable2()
        {
            Guard |= YY::Thunks::aways_null_try_get_GetIfEntry2;
            Guard |= YY::Thunks::aways_null_try_get_GetIfTable2;
            Guard |= YY::Thunks::aways_null_try_get_FreeMibTable;
        }

        TEST_METHOD(GetIfTable2简单验证)
        {
            YY::Thunks::aways_null_try_get_GetIfTable2 = false;
            PMIB_IF_TABLE2 _pTable2_OS;
            auto _lStatus = ::GetIfTable2(&_pTable2_OS);
            Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);

            YY::Thunks::aways_null_try_get_GetIfTable2 = true;
            PMIB_IF_TABLE2 _pTable2_YY;
            _lStatus = ::GetIfTable2(&_pTable2_YY);
            Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);


            Assert::AreEqual(_pTable2_YY->NumEntries, _pTable2_OS->NumEntries);

            for (DWORD i = 0; i != _pTable2_YY->NumEntries; ++i)
            {
                Assert::AreEqual(_pTable2_YY->Table[i].InterfaceIndex, _pTable2_OS->Table[i].InterfaceIndex);
                Assert::AreEqual(std::wstring(_pTable2_YY->Table[i].Description), std::wstring(_pTable2_OS->Table[i].Description));
                Assert::IsTrue(_pTable2_YY->Table[i].InterfaceGuid == _pTable2_OS->Table[i].InterfaceGuid);
            }

            ::FreeMibTable(_pTable2_YY);
        }
    };

    TEST_CLASS(ConvertInterfaceNameToLuid)
    {
        struct Input
        {
            const char* szInterfaceName;
            ULONG64 IfType;
            ULONG64 NetLuidIndex;
        };

        AwaysNullGuard Guard;

    public:
        ConvertInterfaceNameToLuid()
        {
            Guard |= YY::Thunks::aways_null_try_get_ConvertInterfaceNameToLuidA = true;
            Guard |= YY::Thunks::aways_null_try_get_ConvertInterfaceNameToLuidW = true;
        }

        TEST_METHOD(输入验证)
        {
            static const Input s_Input[] =
            {
                {"other_1", IF_TYPE_OTHER, 1},
                {"ethernet_12", IF_TYPE_ETHERNET_CSMACD, 12},
                {"tokenring_123", IF_TYPE_ISO88025_TOKENRING, 123},
                {"ppp_4567", IF_TYPE_PPP, 4567},
                {"loopback_4567", IF_TYPE_SOFTWARE_LOOPBACK, 4567},
                {"atm_4567", IF_TYPE_ATM, 4567},
                {"wireless_4567", IF_TYPE_IEEE80211, 4567},
                {"tunnel_4567", IF_TYPE_TUNNEL, 4567},
                {"ieee1394_4567", IF_TYPE_IEEE1394, 4567},
            };

            for (auto& _Item : s_Input)
            {
                NET_LUID InterfaceLuid;
                auto _lStatus = ::ConvertInterfaceNameToLuidA(_Item.szInterfaceName, &InterfaceLuid);

                Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);
                Assert::AreEqual(InterfaceLuid.Info.IfType, _Item.IfType);
                Assert::AreEqual(InterfaceLuid.Info.NetLuidIndex, _Item.NetLuidIndex);
            }

            for (auto& _Item : s_Input)
            {
                NET_LUID InterfaceLuid;
                auto _lStatus = ::ConvertInterfaceNameToLuidW(CStringW(_Item.szInterfaceName), &InterfaceLuid);

                Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);
                Assert::AreEqual(InterfaceLuid.Info.IfType, _Item.IfType);
                Assert::AreEqual(InterfaceLuid.Info.NetLuidIndex, _Item.NetLuidIndex);
            }
        }

        TEST_METHOD(大小写验证)
        {
            static const Input s_Input[] =
            {
                {"Other_1", IF_TYPE_OTHER, 1},
                {"OtheR_1", IF_TYPE_OTHER, 1},
                {"OTHER_1", IF_TYPE_OTHER, 1},
            };

            for (auto& _Item : s_Input)
            {
                NET_LUID InterfaceLuid;
                auto _lStatus = ::ConvertInterfaceNameToLuidA(_Item.szInterfaceName, &InterfaceLuid);

                Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);
                Assert::AreEqual(InterfaceLuid.Info.IfType, _Item.IfType);
                Assert::AreEqual(InterfaceLuid.Info.NetLuidIndex, _Item.NetLuidIndex);
            }

            for (auto& _Item : s_Input)
            {
                NET_LUID InterfaceLuid;
                auto _lStatus = ::ConvertInterfaceNameToLuidW(CStringW(_Item.szInterfaceName), &InterfaceLuid);

                Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);
                Assert::AreEqual(InterfaceLuid.Info.IfType, _Item.IfType);
                Assert::AreEqual(InterfaceLuid.Info.NetLuidIndex, _Item.NetLuidIndex);
            }
        }

        TEST_METHOD(畸形输入写验证)
        {
            static const char* s_Input[] =
            {
                "other",
                "other_",
                "otherwww_"
                "otherqq_1"
            };

            for (auto& _Item : s_Input)
            {
                NET_LUID InterfaceLuid;
                auto _lStatus = ::ConvertInterfaceNameToLuidA(_Item, &InterfaceLuid);
                Assert::AreNotEqual(_lStatus, (DWORD)ERROR_SUCCESS);
            }

            for (auto& _Item : s_Input)
            {
                NET_LUID InterfaceLuid;
                auto _lStatus = ::ConvertInterfaceNameToLuidW(CStringW(_Item), &InterfaceLuid);
                Assert::AreNotEqual(_lStatus, (DWORD)ERROR_SUCCESS);
            }
        }
    };

    TEST_CLASS(ConvertInterfaceLuidToName)
    {
        struct Input
        {
            const char* szInterfaceName;
            ULONG64 IfType;
            ULONG64 NetLuidIndex;
        };

        AwaysNullGuard Guard;

    public:
        ConvertInterfaceLuidToName()
        {
            Guard |= YY::Thunks::aways_null_try_get_ConvertInterfaceLuidToNameA;
            Guard |= YY::Thunks::aways_null_try_get_ConvertInterfaceLuidToNameW;
        }

        TEST_METHOD(输入验证)
        {
            static const Input s_Input[] =
            {
                {"other_1", IF_TYPE_OTHER, 1},
                {"other_1", IF_TYPE_RFC877_X25, 1},
                {"ethernet_12", IF_TYPE_ETHERNET_CSMACD, 12},
                {"tokenring_123", IF_TYPE_ISO88025_TOKENRING, 123},
                {"ppp_4567", IF_TYPE_PPP, 4567},
                {"loopback_4567", IF_TYPE_SOFTWARE_LOOPBACK, 4567},
                {"atm_4567", IF_TYPE_ATM, 4567},
                {"wireless_4567", IF_TYPE_IEEE80211, 4567},
                {"tunnel_4567", IF_TYPE_TUNNEL, 4567},
                {"ieee1394_4567", IF_TYPE_IEEE1394, 4567},
            };

            for (auto& _Item : s_Input)
            {
                NET_LUID InterfaceLuid;
                InterfaceLuid.Info.IfType = _Item.IfType;
                InterfaceLuid.Info.NetLuidIndex = _Item.NetLuidIndex;
                char _szNameBuffer[256];
                auto _lStatus = ::ConvertInterfaceLuidToNameA(&InterfaceLuid, _szNameBuffer, std::size(_szNameBuffer));

                Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);
                Assert::AreEqual(std::string(_Item.szInterfaceName), std::string(_szNameBuffer));
            }

            for (auto& _Item : s_Input)
            {
                NET_LUID InterfaceLuid;
                InterfaceLuid.Info.IfType = _Item.IfType;
                InterfaceLuid.Info.NetLuidIndex = _Item.NetLuidIndex;
                wchar_t _szNameBuffer[256];
                auto _lStatus = ::ConvertInterfaceLuidToNameW(&InterfaceLuid, _szNameBuffer, std::size(_szNameBuffer));

                Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);
                Assert::AreEqual(std::wstring(CStringW(_Item.szInterfaceName)), std::wstring(_szNameBuffer));
            }
        }


        TEST_METHOD(畸形输入验证)
        {
            NET_LUID InterfaceLuid;
            InterfaceLuid.Info.IfType = IF_TYPE_ETHERNET_CSMACD;
            InterfaceLuid.Info.NetLuidIndex = 999;
            {
                char _szNameBuffer[256];
                auto _lStatus = ::ConvertInterfaceLuidToNameA(&InterfaceLuid, _szNameBuffer, 3);
                Assert::AreEqual(_lStatus, (DWORD)ERROR_NOT_ENOUGH_MEMORY);
            }

            {
                wchar_t _szNameBuffer[256];
                auto _lStatus = ::ConvertInterfaceLuidToNameW(&InterfaceLuid, _szNameBuffer, 3);
                Assert::AreEqual(_lStatus, (DWORD)ERROR_NOT_ENOUGH_MEMORY);
            }
        }
    };

    TEST_CLASS(if_nametoindex与if_indextoname)
    {
        AwaysNullGuard Guard;

    public:
        if_nametoindex与if_indextoname()
        {
            Guard |= YY::Thunks::aways_null_try_get_if_nametoindex;
            Guard |= YY::Thunks::aways_null_try_get_if_indextoname;
            Guard |= YY::Thunks::aways_null_try_get_ConvertInterfaceIndexToLuid;
            Guard |= YY::Thunks::aways_null_try_get_ConvertInterfaceLuidToNameA;
        }

        TEST_METHOD(交叉验证)
        {
            char _szNameBuffer[256];
            auto _szResult = ::if_indextoname(1, _szNameBuffer);
            Assert::IsNotNull(_szResult);

            auto _uResultIndex = if_nametoindex(_szNameBuffer);
            Assert::AreEqual(_uResultIndex, NET_IFINDEX(1));
        }
    };
}
