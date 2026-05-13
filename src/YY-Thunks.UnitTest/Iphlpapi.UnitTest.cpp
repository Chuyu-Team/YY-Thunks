#include "pch.h"
#include "Thunks/Iphlpapi.hpp"


namespace Iphlpapi
{
    TEST_CLASS(GetUnicastIpAddressTable)
    {
        AwaysNullGuard Guard;

    public:
        GetUnicastIpAddressTable()
        {
            Guard |= YY::Thunks::aways_null_try_get_GetUnicastIpAddressTable;
            Guard |= YY::Thunks::aways_null_try_get_FreeMibTable;
        }

        TEST_METHOD(Fallback简单验证)
        {
            PMIB_UNICASTIPADDRESS_TABLE _pTable = nullptr;
            auto _lStatus = ::GetUnicastIpAddressTable(AF_UNSPEC, &_pTable);
            Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);
            Assert::IsNotNull(_pTable);
            ::FreeMibTable(_pTable);
        }

        TEST_METHOD(参数验证)
        {
            auto _lStatus = ::GetUnicastIpAddressTable(AF_UNSPEC, nullptr);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_INVALID_PARAMETER);

            PMIB_UNICASTIPADDRESS_TABLE _pTable = nullptr;
            _lStatus = ::GetUnicastIpAddressTable(AF_MAX, &_pTable);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_INVALID_PARAMETER);
        }
    };

    TEST_CLASS(NotifyIpInterfaceChange)
    {
        AwaysNullGuard Guard;

        struct CallbackContext
        {
            uint32_t uCalled = 0;
            PMIB_IPINTERFACE_ROW pRow = nullptr;
            MIB_NOTIFICATION_TYPE eNotificationType = MibInitialNotification;
        };

        static void WINAPI Callback(
            _In_ PVOID _pCallerContext,
            _In_ PMIB_IPINTERFACE_ROW _pRow,
            _In_ MIB_NOTIFICATION_TYPE _eNotificationType
            )
        {
            if (MibInitialNotification == _eNotificationType)
            {
                auto _pContext = reinterpret_cast<CallbackContext*>(_pCallerContext);
                InterlockedIncrement(&_pContext->uCalled);
                _pContext->pRow = _pRow;
                _pContext->eNotificationType = _eNotificationType;
            }
        }

    public:
        NotifyIpInterfaceChange()
        {
            Guard |= YY::Thunks::aways_null_try_get_NotifyIpInterfaceChange;
            Guard |= YY::Thunks::aways_null_try_get_CancelMibChangeNotify2;
        }

        TEST_METHOD(InitialNotification验证)
        {
            {
                CallbackContext _oContext;
                HANDLE _hNotify = nullptr;
                auto _lStatus = ::NotifyIpInterfaceChange(AF_INET, Callback, &_oContext, TRUE, &_hNotify);
                Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);
                Assert::AreEqual(_oContext.uCalled, uint32_t(1));
                Assert::IsNull(_oContext.pRow);
                Assert::AreEqual((DWORD)_oContext.eNotificationType, (DWORD)MibInitialNotification);
                Assert::IsNotNull(_hNotify);

                _lStatus = ::CancelMibChangeNotify2(_hNotify);
                Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);
            }

            {
                CallbackContext _oContext;
                HANDLE _hNotify = nullptr;
                auto _lStatus = ::NotifyIpInterfaceChange(AF_INET6, Callback, &_oContext, TRUE, &_hNotify);
                Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);
                Assert::AreEqual(_oContext.uCalled, uint32_t(1));
                Assert::IsNull(_oContext.pRow);
                Assert::AreEqual((DWORD)_oContext.eNotificationType, (DWORD)MibInitialNotification);
                Assert::IsNotNull(_hNotify);

                _lStatus = ::CancelMibChangeNotify2(_hNotify);
                Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);
            }

            {
                CallbackContext _oContext;
                HANDLE _hNotify = nullptr;
                auto _lStatus = ::NotifyIpInterfaceChange(AF_UNSPEC, Callback, &_oContext, TRUE, &_hNotify);
                Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);
                Assert::AreEqual(_oContext.uCalled, uint32_t(2));
                Assert::IsNull(_oContext.pRow);
                Assert::AreEqual((DWORD)_oContext.eNotificationType, (DWORD)MibInitialNotification);
                Assert::IsNotNull(_hNotify);

                _lStatus = ::CancelMibChangeNotify2(_hNotify);
                Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);
            }

            {
                CallbackContext _oContext;
                HANDLE _hNotify = nullptr;
                auto _lStatus = ::NotifyIpInterfaceChange(AF_UNSPEC, Callback, &_oContext, FALSE, &_hNotify);
                Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);
                Assert::AreEqual(_oContext.uCalled, uint32_t(0));
                Assert::IsNull(_oContext.pRow);
                Assert::AreEqual((DWORD)_oContext.eNotificationType, (DWORD)MibInitialNotification);
                Assert::IsNotNull(_hNotify);

                _lStatus = ::CancelMibChangeNotify2(_hNotify);
                Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);
            }
        }

        TEST_METHOD(参数验证)
        {
            auto _lStatus = ::NotifyIpInterfaceChange(AF_UNSPEC, nullptr, nullptr, FALSE, nullptr);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_INVALID_PARAMETER);

            HANDLE _hNotify = nullptr;
            _lStatus = ::NotifyIpInterfaceChange(AF_MAX, Callback, nullptr, FALSE, &_hNotify);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_INVALID_PARAMETER);
            Assert::IsNull(_hNotify);
        }
    };

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

    TEST_CLASS(GetTcpTable2)
    {
    public:
        std::vector<BYTE> GetTcpTable2Helper(BOOL Order)
        {
            ULONG _uTcpTable2Size = 0;
            auto _lStatus = ::GetTcpTable2(nullptr, &_uTcpTable2Size, FALSE);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_INSUFFICIENT_BUFFER);

            std::vector<BYTE> _TcpTable2Buffer(_uTcpTable2Size);
            auto _pTcpTable2 = reinterpret_cast<PMIB_TCPTABLE2>(_TcpTable2Buffer.data());
            _lStatus = ::GetTcpTable2(_pTcpTable2, &_uTcpTable2Size, FALSE);
            Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);
            return _TcpTable2Buffer;
        }

        TEST_METHOD(GetTcpTable2简单验证)
        {
            auto _TcpTable2Buffer1 = GetTcpTable2Helper(FALSE);
            AwaysNullGuard Guard;
            Guard |= YY::Thunks::aways_null_try_get_GetTcpTable2;
            auto _TcpTable2Buffer2 = GetTcpTable2Helper(FALSE);

            auto _pTcpTable1 = reinterpret_cast<PMIB_TCPTABLE2>(_TcpTable2Buffer1.data());
            auto _pTcpTable2 = reinterpret_cast<PMIB_TCPTABLE2>(_TcpTable2Buffer2.data());

            Assert::AreEqual(_pTcpTable2->dwNumEntries, _pTcpTable1->dwNumEntries);
            for (DWORD i = 0; i != _pTcpTable2->dwNumEntries; ++i)
            {
                Assert::AreEqual(_pTcpTable2->table[i].dwState, _pTcpTable1->table[i].dwState);
                Assert::AreEqual(_pTcpTable2->table[i].dwLocalAddr, _pTcpTable1->table[i].dwLocalAddr);
                Assert::AreEqual(_pTcpTable2->table[i].dwLocalPort, _pTcpTable1->table[i].dwLocalPort);
                Assert::AreEqual(_pTcpTable2->table[i].dwRemoteAddr, _pTcpTable1->table[i].dwRemoteAddr);
                Assert::AreEqual(_pTcpTable2->table[i].dwRemotePort, _pTcpTable1->table[i].dwRemotePort);
                Assert::AreEqual(_pTcpTable2->table[i].dwOwningPid, _pTcpTable1->table[i].dwOwningPid);
                Assert::AreEqual(DWORD(_pTcpTable2->table[i].dwOffloadState), DWORD(TCP_CONNECTION_OFFLOAD_STATE::TcpConnectionOffloadStateInHost));
            }
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


    TEST_CLASS(ConvertInterfaceLuidToGuid)
    {
    public:
        ConvertInterfaceLuidToGuid()
        {
        }

        TEST_METHOD(交叉验证)
        {
            NET_LUID InterfaceLuid;
            auto _lStatus =  ConvertInterfaceIndexToLuid(1, &InterfaceLuid);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

            GUID InterfaceGuid1;
            _lStatus = ::ConvertInterfaceLuidToGuid(&InterfaceLuid, &InterfaceGuid1);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

            {
                AwaysNullGuard Guard;
                Guard |= YY::Thunks::aways_null_try_get_ConvertInterfaceLuidToGuid;

                GUID InterfaceGuid2;
                NET_LUID InterfaceLuid = {};
                InterfaceLuid.Info.NetLuidIndex = 1;
                _lStatus = ::ConvertInterfaceLuidToGuid(&InterfaceLuid, &InterfaceGuid2);
                Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

                Assert::AreEqual(InterfaceGuid1, InterfaceGuid2);

            }
        }
    };

    TEST_CLASS(NotifyStableUnicastIpAddressTable)
    {
        AwaysNullGuard Guard;

        static void WINAPI Callback(
            _In_ PVOID _pCallerContext,
            _In_ PMIB_UNICASTIPADDRESS_TABLE _pAddressTable
            )
        {
            auto _pbCalled = reinterpret_cast<bool*>(_pCallerContext);
            *_pbCalled = true;
            Assert::IsNotNull(_pAddressTable);
        }

    public:
        NotifyStableUnicastIpAddressTable()
        {
            Guard |= YY::Thunks::aways_null_try_get_NotifyStableUnicastIpAddressTable;
            Guard |= YY::Thunks::aways_null_try_get_CancelMibChangeNotify2;
        }

        TEST_METHOD(Fallback简单验证)
        {
            {
                bool _bCalled = false;
                PMIB_UNICASTIPADDRESS_TABLE _pTable = nullptr;
                HANDLE _hNotify = nullptr;

                auto _lStatus = ::NotifyStableUnicastIpAddressTable(
                    AF_UNSPEC,
                    &_pTable,
                    Callback,
                    &_bCalled,
                    &_hNotify);

                Assert::AreEqual(_lStatus, (DWORD)ERROR_IO_PENDING);
                Assert::IsNotNull(_hNotify);

                _lStatus = ::CancelMibChangeNotify2(_hNotify);
                Assert::IsFalse(_bCalled);
                Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);

                if (_pTable)
                {
                    ::FreeMibTable(_pTable);
                }
            }

            // 5秒超时获取结果
            {
                bool _bCalled = false;
                PMIB_UNICASTIPADDRESS_TABLE _pTable = nullptr;
                HANDLE _hNotify = nullptr;

                auto _lStatus = ::NotifyStableUnicastIpAddressTable(
                    AF_UNSPEC,
                    &_pTable,
                    Callback,
                    &_bCalled,
                    &_hNotify);

                Assert::AreEqual(_lStatus, (DWORD)ERROR_IO_PENDING);
                Assert::IsNotNull(_hNotify);

                Sleep(6 * 1000);
                _lStatus = ::CancelMibChangeNotify2(_hNotify);
                Assert::IsTrue(_bCalled);
                Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);

                if (_pTable)
                {
                    ::FreeMibTable(_pTable);
                }
            }
        }

        TEST_METHOD(参数验证)
        {
            HANDLE _hNotify = reinterpret_cast<HANDLE>(1);
            PMIB_UNICASTIPADDRESS_TABLE _pTable = nullptr;

            auto _lStatus = ::NotifyStableUnicastIpAddressTable(
                AF_UNSPEC,
                nullptr,
                Callback,
                nullptr,
                &_hNotify);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_INVALID_PARAMETER);

            _lStatus = ::NotifyStableUnicastIpAddressTable(
                AF_UNSPEC,
                &_pTable,
                nullptr,
                nullptr,
                &_hNotify);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_INVALID_PARAMETER);

            _lStatus = ::NotifyStableUnicastIpAddressTable(
                AF_MAX,
                &_pTable,
                Callback,
                nullptr,
                &_hNotify);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_INVALID_PARAMETER);

            _lStatus = ::NotifyStableUnicastIpAddressTable(
                AF_UNSPEC,
                &_pTable,
                Callback,
                nullptr,
                nullptr);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_INVALID_PARAMETER);
        }
    };
}
