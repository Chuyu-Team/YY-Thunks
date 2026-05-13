#if (YY_Thunks_Target < __WindowsNT10_15063)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#endif

#if (YY_Thunks_Target < __WindowsNT10_15063) && !defined(__Comment_Lib_iphlpapi)
#define __Comment_Lib_iphlpapi
#pragma comment(lib, "Iphlpapi.lib")
#endif

#if defined(YY_Thunks_Implemented)
namespace YY::Thunks
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT6)

        static void MIB_IFROW_To_MIB_IF_ROW2(_In_ const MIB_IFROW* _pRow, _Out_ MIB_IF_ROW2* _pRow2)
        {
            memset(_pRow2, 0, sizeof(*_pRow2));

            _pRow2->InterfaceLuid.Info.NetLuidIndex = _pRow->dwIndex;
            _pRow2->InterfaceLuid.Info.IfType = _pRow->dwType;
            _pRow2->InterfaceIndex = _pRow->dwIndex;

            internal::StringToGuid(wcsrchr(_pRow->wszName, L'{'), &_pRow2->InterfaceGuid);
            // memcpy(_pRow2->Alias, _pRow->wszName, sizeof(_pRow->wszName));
            const auto _cchDescr = min(sizeof(_pRow->bDescr), _pRow->dwDescrLen);
            for (size_t i = 0; i != _cchDescr; ++i)
            {
                _pRow2->Description[i] = _pRow->bDescr[i];
            }
            _pRow2->PhysicalAddressLength = _pRow->dwPhysAddrLen;
            memcpy(_pRow2->PhysicalAddress, _pRow->bPhysAddr, min(sizeof(_pRow->bPhysAddr), _pRow->dwPhysAddrLen));
            memcpy(_pRow2->PermanentPhysicalAddress, _pRow->bPhysAddr, min(sizeof(_pRow->bPhysAddr), _pRow->dwPhysAddrLen));

            _pRow2->Mtu = _pRow->dwMtu;
            _pRow2->Type = _pRow->dwType;

            if (IF_TYPE_SOFTWARE_LOOPBACK == _pRow->dwType)
            {
                _pRow2->MediaType = NdisMediumLoopback;
                _pRow2->AccessType = NET_IF_ACCESS_LOOPBACK;
                _pRow2->DirectionType = NET_IF_DIRECTION_SENDRECEIVE;
            }
            else
            {
                // 设置一个默认值，都假装自己是普通的网口
                _pRow2->MediaType = NdisMedium802_3;
                _pRow2->AccessType = NET_IF_ACCESS_BROADCAST;
                _pRow2->DirectionType = NET_IF_DIRECTION_SENDRECEIVE;
            }

            switch (_pRow->dwOperStatus)
            {
            case IF_OPER_STATUS_OPERATIONAL:
            case IF_OPER_STATUS_CONNECTED:
                _pRow2->OperStatus = IF_OPER_STATUS::IfOperStatusUp;
                _pRow2->MediaConnectState = MediaConnectStateConnected;
                break;
            default:
                _pRow2->OperStatus = IF_OPER_STATUS::IfOperStatusDown;
                break;
            }

            // 
            _pRow2->AdminStatus = (NET_IF_ADMIN_STATUS)_pRow->dwAdminStatus;
            _pRow2->ConnectionType = NET_IF_CONNECTION_DEDICATED;


            _pRow2->TransmitLinkSpeed = _pRow->dwSpeed;
            _pRow2->ReceiveLinkSpeed = _pRow->dwSpeed;
            _pRow2->InOctets = _pRow->dwInOctets;
            _pRow2->InUcastPkts = _pRow->dwInUcastPkts;
            _pRow2->InNUcastPkts = _pRow->dwInNUcastPkts;
            _pRow2->InDiscards = _pRow->dwInDiscards;
            _pRow2->InErrors = _pRow->dwInErrors;
            _pRow2->InUnknownProtos = _pRow->dwInUnknownProtos;
            // 伪造一下 InUcastOctets
            _pRow2->InUcastOctets = _pRow->dwInOctets;
            _pRow2->InMulticastOctets = 0;
            _pRow2->InBroadcastOctets = 0;
            _pRow2->OutOctets = _pRow->dwOutOctets;
            _pRow2->OutUcastPkts = _pRow->dwOutUcastPkts;
            _pRow2->OutNUcastPkts = _pRow->dwOutNUcastPkts;
            _pRow2->OutDiscards = _pRow->dwOutDiscards;
            _pRow2->OutErrors = _pRow->dwOutErrors;
            // 伪造一下 OutUcastOctets
            _pRow2->OutUcastOctets = _pRow->dwOutOctets;
            _pRow2->OutMulticastOctets = 0;
            _pRow2->OutBroadcastOctets = 0;
            _pRow2->OutQLen = _pRow->dwOutQLen;
        }

        static
        NETIOAPI_API
        GetIfTable2ExDownlevel(
            _In_ MIB_IF_TABLE_LEVEL _eLevel,
            _Outptr_ PMIB_IF_TABLE2* _ppTable2
            )
        {
            // Windows XP 无法支持_eLevel
            // 相比较无法启动，一些状态报告不对就这样凑合吧。
            UNREFERENCED_PARAMETER(_eLevel);

            if (!_ppTable2)
                return ERROR_INVALID_PARAMETER;
            *_ppTable2 = nullptr;

            const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;

            constexpr DWORD _cbTable = sizeof(MIB_IFTABLE) + 256 * sizeof(MIB_IFROW);
            auto _pTable = (MIB_IFTABLE*)HeapAlloc(_hProcessHeap, 0, _cbTable);
            if (!_pTable)
                return ERROR_NOT_ENOUGH_MEMORY;

            LSTATUS _lStatus;
            do
            {
                DWORD _uSize = _cbTable;
                _lStatus = ::GetIfTable(_pTable, &_uSize, FALSE);
                if (_lStatus != NO_ERROR)
                    break;

                const DWORD _cbTable2 = sizeof(MIB_IF_TABLE2) + _pTable->dwNumEntries * sizeof(MIB_IF_ROW2);
                auto _pTable2 = (MIB_IF_TABLE2*)HeapAlloc(_hProcessHeap, 0, _cbTable2);
                if (!_pTable2)
                {
                    _lStatus = ERROR_NOT_ENOUGH_MEMORY;
                    break;
                }

                _pTable2->NumEntries = _pTable->dwNumEntries;
                for (DWORD i = 0; i != _pTable->dwNumEntries; ++i)
                {
                    MIB_IFROW_To_MIB_IF_ROW2(&_pTable->table[i], &_pTable2->Table[i]);
                }

                *_ppTable2 = _pTable2;
            } while (false);

            HeapFree(_hProcessHeap, 0, _pTable);

            return _lStatus;
        }

        struct IfTypeToNameItem
        {
            IFTYPE uType;
            const char* szName;
        };

        static const IfTypeToNameItem(&GetIfTypeMapItems(void))[9]
        {
            static const IfTypeToNameItem s_Map[] =
            {
                {IF_TYPE_OTHER, "other"},
                {IF_TYPE_ETHERNET_CSMACD, "ethernet"},
                {IF_TYPE_ISO88025_TOKENRING, "tokenring"},
                {IF_TYPE_PPP, "ppp"},
                {IF_TYPE_SOFTWARE_LOOPBACK, "loopback"},
                {IF_TYPE_ATM, "atm"},
                {IF_TYPE_IEEE80211, "wireless"},
                {IF_TYPE_TUNNEL, "tunnel"},
                {IF_TYPE_IEEE1394, "ieee1394"},
            };

            return s_Map;
        }

        static const char* __fastcall GetAnsiNameFormeIndex(IFTYPE _uType)
        {
            auto& _Map = GetIfTypeMapItems();

            int bottom = 0;
            int top = _countof(_Map) - 1;

            while (bottom <= top)
            {
                int middle = (bottom + top) / 2;
                int testIndex = _uType - _Map[middle].uType;

                if (testIndex == 0)
                {
                    return _Map[middle].szName;
                }

                if (testIndex < 0)
                    top = middle - 1;
                else
                    bottom = middle + 1;
            }

            return "other";
        }

        static NETIO_STATUS __fastcall GetAdaptersAddressesSnapshot(
            _In_ ADDRESS_FAMILY _uFamily,
            _Outptr_result_maybenull_ PIP_ADAPTER_ADDRESSES* _ppAdapters
            )
        {
            *_ppAdapters = nullptr;

            ULONG _cbAdaptersAddresses = 0;
            auto _lStatus = GetAdaptersAddresses(_uFamily, 0, nullptr, nullptr, &_cbAdaptersAddresses);
            if (_lStatus == ERROR_NO_DATA)
            {
                return NO_ERROR;
            }

            if (_lStatus != ERROR_BUFFER_OVERFLOW)
            {
                return _lStatus;
            }

            auto _pAdapters = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(internal::Alloc(_cbAdaptersAddresses));
            if (_pAdapters == nullptr)
            {
                return ERROR_NOT_ENOUGH_MEMORY;
            }

            _lStatus = GetAdaptersAddresses(_uFamily, 0, nullptr, _pAdapters, &_cbAdaptersAddresses);
            if (_lStatus != NO_ERROR)
            {
                internal::Free(_pAdapters);
                return _lStatus;
            }

            *_ppAdapters = _pAdapters;
            return NO_ERROR;
        }

        static NETIOAPI_API GetUnicastIpAddressTableDownlevel(
            _In_ ADDRESS_FAMILY _uFamily,
            _Outptr_ PMIB_UNICASTIPADDRESS_TABLE* _ppTable
            )
        {
            if (_ppTable == nullptr)
            {
                return ERROR_INVALID_PARAMETER;
            }

            if (_uFamily != AF_INET && _uFamily != AF_INET6 && _uFamily != AF_UNSPEC)
            {
                *_ppTable = nullptr;
                return ERROR_INVALID_PARAMETER;
            }

            *_ppTable = nullptr;

            PIP_ADAPTER_ADDRESSES _pAdaptersAddresses = nullptr;
            auto _lStatus = GetAdaptersAddressesSnapshot(_uFamily, &_pAdaptersAddresses);
            if (_lStatus != NO_ERROR)
            {
                return _lStatus;
            }

            ULONG _uCount = 0;
            for (auto _pAdapter = _pAdaptersAddresses; _pAdapter != nullptr; _pAdapter = _pAdapter->Next)
            {
                for (auto _pUnicast = _pAdapter->FirstUnicastAddress; _pUnicast != nullptr; _pUnicast = _pUnicast->Next)
                {
                    if (_pUnicast->Address.lpSockaddr == nullptr)
                    {
                        continue;
                    }

                    if (_pUnicast->Address.lpSockaddr->sa_family == AF_INET
                        || _pUnicast->Address.lpSockaddr->sa_family == AF_INET6)
                    {
                        ++_uCount;
                    }
                }
            }

            const auto _cbTable = FIELD_OFFSET(MIB_UNICASTIPADDRESS_TABLE, Table) + _uCount * sizeof(MIB_UNICASTIPADDRESS_ROW);
            auto _pTable = reinterpret_cast<PMIB_UNICASTIPADDRESS_TABLE>(internal::Alloc(_cbTable));
            if (_pTable == nullptr)
            {
                internal::Free(_pAdaptersAddresses);
                return ERROR_NOT_ENOUGH_MEMORY;
            }

            _pTable->NumEntries = _uCount;

            ULONG _uIndex = 0;
            for (auto _pAdapter = _pAdaptersAddresses; _pAdapter != nullptr; _pAdapter = _pAdapter->Next)
            {
                for (auto _pUnicast = _pAdapter->FirstUnicastAddress; _pUnicast != nullptr; _pUnicast = _pUnicast->Next)
                {
                    if (_pUnicast->Address.lpSockaddr == nullptr)
                    {
                        continue;
                    }

                    auto _pAddress = _pUnicast->Address.lpSockaddr;
                    if (_pAddress->sa_family != AF_INET && _pAddress->sa_family != AF_INET6)
                    {
                        continue;
                    }

                    auto& _Item = _pTable->Table[_uIndex++];
                    _Item.InterfaceLuid.Info.IfType = _pAdapter->IfType;
                    _Item.InterfaceLuid.Info.Reserved = 0;
                    _Item.Address.si_family = _pAddress->sa_family;
                    if (_pAddress->sa_family == AF_INET6)
                    {
                        _Item.InterfaceIndex = _pAdapter->Ipv6IfIndex;
                        _Item.InterfaceLuid.Value = _pAdapter->Ipv6IfIndex;

                        _Item.Address.Ipv6 = *reinterpret_cast<const SOCKADDR_IN6*>(_pAddress);
                        _Item.ScopeId.Value = _Item.Address.Ipv6.sin6_scope_id;
                    }
                    else
                    {
                        _Item.InterfaceIndex = _pAdapter->IfIndex;
                        _Item.InterfaceLuid.Value = _pAdapter->IfIndex;

                        _Item.Address.Ipv4 = *reinterpret_cast<const SOCKADDR_IN*>(_pAddress);
                        _Item.ScopeId.Value = 0;
                    }
                    _Item.PrefixOrigin = _pUnicast->PrefixOrigin;
                    _Item.SuffixOrigin = _pUnicast->SuffixOrigin;
                    _Item.ValidLifetime = _pUnicast->ValidLifetime;
                    _Item.PreferredLifetime = _pUnicast->PreferredLifetime;
                    _Item.OnLinkPrefixLength = _pUnicast->OnLinkPrefixLength;
                    _Item.SkipAsSource = true;

                    _Item.DadState = _pUnicast->DadState;
                    _Item.CreationTimeStamp.QuadPart = 0;
                }
            }

            internal::Free(_pAdaptersAddresses);
            *_ppTable = _pTable;
            return NO_ERROR;
        }

        static bool __fastcall IsSameSockaddrInet(_In_ const SOCKADDR_INET& _Address1, _In_ const SOCKADDR_INET& _Address2)
        {
            if (_Address1.si_family != _Address2.si_family)
            {
                return false;
            }

            if (_Address1.si_family == AF_INET)
            {
                return _Address1.Ipv4.sin_addr.S_un.S_addr == _Address2.Ipv4.sin_addr.S_un.S_addr;
            }
            else if (_Address1.si_family == AF_INET6)
            {
                return memcmp(&_Address1.Ipv6.sin6_addr, &_Address2.Ipv6.sin6_addr, sizeof(_Address1.Ipv6.sin6_addr)) == 0
                    && _Address1.Ipv6.sin6_scope_id == _Address2.Ipv6.sin6_scope_id;
            }

            return false;
        }

        static bool __fastcall ShouldIncludeUnicastAddress(
            _In_ ADDRESS_FAMILY _uFamily,
            _In_ const SOCKADDR* _pAddress
            )
        {
            if (_pAddress == nullptr)
            {
                return false;
            }

            const auto _uAddressFamily = static_cast<ADDRESS_FAMILY>(_pAddress->sa_family);
            if (_uAddressFamily != AF_INET && _uAddressFamily != AF_INET6)
            {
                return false;
            }

            return _uFamily == AF_UNSPEC || _uFamily == _uAddressFamily;
        }

        //static const IP_ADAPTER_ADDRESSES* __fastcall FindAdapterByAdapterName(
        //    _In_ const IP_ADAPTER_ADDRESSES* _pAdapters,
        //    _In_z_ LPCSTR _szAdapterName
        //    )
        //{
        //    for (auto _pAdapter = _pAdapters; _pAdapter != nullptr; _pAdapter = _pAdapter->Next)
        //    {
        //        if (StringCompareIgnoreCaseByAscii(_pAdapter->AdapterName, _szAdapterName, -1) == 0)
        //        {
        //            return _pAdapter;
        //        }
        //    }

        //    return nullptr;
        //}

        static NET_IFINDEX __fastcall GetUnicastAddressInterfaceIndex(
            _In_ ADDRESS_FAMILY _uAddressFamily,
            _In_ const IP_ADAPTER_ADDRESSES* _pAdapter
            )
        {
            return _uAddressFamily == AF_INET6 ? _pAdapter->Ipv6IfIndex : _pAdapter->IfIndex;
        }

        static bool __fastcall IsInterfaceUp(_In_ const IP_ADAPTER_ADDRESSES* _pAdapter)
        {
            return _pAdapter->OperStatus == IfOperStatusUp;
        }

        static const IP_ADAPTER_ADDRESSES* __fastcall FindAdapterByInterfaceIndex(
            _In_opt_ const IP_ADAPTER_ADDRESSES* _pAdapters,
            _In_ ADDRESS_FAMILY _uFamily,
            _In_ NET_IFINDEX _uInterfaceIndex
            )
        {
            for (auto _pAdapter = _pAdapters; _pAdapter != nullptr; _pAdapter = _pAdapter->Next)
            {
                if (GetUnicastAddressInterfaceIndex(_uFamily, _pAdapter) == _uInterfaceIndex)
                {
                    return _pAdapter;
                }
            }

            return nullptr;
        }

        static void __fastcall BuildIpInterfaceRowFromAdapter(
            _In_ ADDRESS_FAMILY _uFamily,
            _In_ const IP_ADAPTER_ADDRESSES* _pAdapter,
            _Out_ MIB_IPINTERFACE_ROW* _pRow
            )
        {
            memset(_pRow, 0, sizeof(*_pRow));

            _pRow->Family = _uFamily;
            _pRow->InterfaceIndex = GetUnicastAddressInterfaceIndex(_uFamily, _pAdapter);
            _pRow->InterfaceLuid.Info.IfType = _pAdapter->IfType;
            _pRow->InterfaceLuid.Info.NetLuidIndex = _pRow->InterfaceIndex;
            _pRow->InterfaceLuid.Info.Reserved = 0;
            _pRow->NlMtu = _pAdapter->Mtu;
            _pRow->Connected = IsInterfaceUp(_pAdapter);
        }

        static bool __fastcall IsSameIpInterfaceParameters(
            _In_ const IP_ADAPTER_ADDRESSES* _pAdapter1,
            _In_ const IP_ADAPTER_ADDRESSES* _pAdapter2
            )
        {
            return _pAdapter1->Mtu == _pAdapter2->Mtu
                && _pAdapter1->OperStatus == _pAdapter2->OperStatus;
        }

        static constexpr DWORD kMibChangeNotifyDownlevelContextSignature = 0x4D42434E;

        struct MibChangeNotifyDownlevelContext
        {
            DWORD uSignature = kMibChangeNotifyDownlevelContextSignature;

            virtual ~MibChangeNotifyDownlevelContext() = default;

            virtual LSTATUS __fastcall Cancel() = 0;
        };
#endif // (YY_Thunks_Target < __WindowsNT6)
    }
}
#endif // !YY_Thunks_Implemented

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    4,
    NETIO_STATUS,
    NETIOAPI_API_,
    GetIfTable2,
        _Outptr_ PMIB_IF_TABLE2 * _ppTable2
        )
    {
        if (auto _pfnGetIfTable2 = try_get_GetIfTable2())
        {
            return _pfnGetIfTable2(_ppTable2);
        }

        return GetIfTable2ExDownlevel(MibIfTableNormal, _ppTable2);
    }
#endif
        
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    8,
    NETIO_STATUS,
    NETIOAPI_API_,
    GetIfTable2Ex,
        _In_ MIB_IF_TABLE_LEVEL _eLevel,
        _Outptr_ PMIB_IF_TABLE2* _ppTable2
        )
    {
        if (auto _pfnGetIfTable2Ex = try_get_GetIfTable2Ex())
        {
            return _pfnGetIfTable2Ex(_eLevel, _ppTable2);
        }

        return GetIfTable2ExDownlevel(_eLevel, _ppTable2);
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    4,
    NETIO_STATUS,
    NETIOAPI_API_,
    GetIfEntry2,
        _Inout_ PMIB_IF_ROW2 _pRow
        )
    {
        if (auto _pfnGetIfEntry2 = try_get_GetIfEntry2())
        {
            return _pfnGetIfEntry2(_pRow);
        }

        if (!_pRow)
            return ERROR_INVALID_PARAMETER;

        MIB_IFROW _IfRow;
        _IfRow.dwIndex = _pRow->InterfaceLuid.Value ? static_cast<ULONG>(_pRow->InterfaceLuid.Info.NetLuidIndex) : _pRow->InterfaceIndex;

        auto _lStatus = GetIfEntry(&_IfRow);
        if (NO_ERROR != _lStatus)
            return _lStatus;

        MIB_IFROW_To_MIB_IF_ROW2(&_IfRow, _pRow);
        return NO_ERROR;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT10_15063)

    // 最低受支持的客户端	Windows 10版本 1703[仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    iphlpapi,
    8,
    NETIO_STATUS,
    NETIOAPI_API_,
    GetIfEntry2Ex,
        _In_    MIB_IF_ENTRY_LEVEL _eLevel,
        _Inout_ PMIB_IF_ROW2       _pRow
        )
    {
        if (auto _pfnGetIfEntry2Ex = try_get_GetIfEntry2Ex())
        {
            return _pfnGetIfEntry2Ex(_eLevel, _pRow);
        }
        // 忽略Level，理论上说 多取了 Statistics信息也没有太大影响。
        UNREFERENCED_PARAMETER(_eLevel);

        return ::GetIfEntry2(_pRow);
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    4,
    void,
    NETIOAPI_API_,
    FreeMibTable,
        _In_ PVOID _pMemory
        )
    {
        if (auto _pfnFreeMibTable = try_get_FreeMibTable())
        {
            return _pfnFreeMibTable(_pMemory);
        }

        if (_pMemory)
        {
            const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
            HeapFree(_hProcessHeap, 0, _pMemory);
        }
    }
#endif

        
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    8,
    DWORD,
    NETIOAPI_API_,
    ConvertInterfaceIndexToLuid,
        _In_ NET_IFINDEX InterfaceIndex,
        _Out_ PNET_LUID InterfaceLuid
        )
    {
        if (const auto _pfnConvertInterfaceIndexToLuid = try_get_ConvertInterfaceIndexToLuid())
        {
            return _pfnConvertInterfaceIndexToLuid(InterfaceIndex, InterfaceLuid);
        }
            
        MIB_IFROW _IfRow;
        _IfRow.dwIndex = InterfaceIndex;
        auto _lStatus = GetIfEntry(&_IfRow);
        if (_lStatus != ERROR_SUCCESS)
        {
            return _lStatus;
        }

        // 使用 InterfaceIndex 假冒 LuidIndex
        InterfaceLuid->Info.IfType = _IfRow.dwType;
        InterfaceLuid->Info.NetLuidIndex = InterfaceIndex;
        InterfaceLuid->Info.Reserved = 0;
        return ERROR_SUCCESS;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    iphlpapi,
    12,
    DWORD,
    NETIOAPI_API_,
    ConvertInterfaceLuidToNameA,
        _In_ CONST NET_LUID* InterfaceLuid,
        _Out_writes_(Length) PSTR InterfaceName,
        _In_ SIZE_T Length
        )
    {
        if (const auto _pfnConvertInterfaceLuidToNameA = try_get_ConvertInterfaceLuidToNameA())
        {
            return _pfnConvertInterfaceLuidToNameA(InterfaceLuid, InterfaceName, Length);
        }

        internal::StringBuffer<char> _StringBuffer(InterfaceName, Length);
        if (!_StringBuffer.AppendString(GetAnsiNameFormeIndex(InterfaceLuid->Info.IfType)))
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }

        if (!_StringBuffer.AppendChar('_'))
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }

        if (!_StringBuffer.AppendUint32(InterfaceLuid->Info.NetLuidIndex))
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }
        return ERROR_SUCCESS;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)
    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    iphlpapi,
    12,
    DWORD,
    NETIOAPI_API_,
    ConvertInterfaceLuidToNameW,
        _In_ CONST NET_LUID* InterfaceLuid,
        _Out_writes_(Length) PWSTR InterfaceName,
        _In_ SIZE_T Length
        )
    {
        if (const auto _pfnConvertInterfaceLuidToNameW = try_get_ConvertInterfaceLuidToNameW())
        {
            return _pfnConvertInterfaceLuidToNameW(InterfaceLuid, InterfaceName, Length);
        }

        internal::StringBuffer<wchar_t> _StringBuffer(InterfaceName, Length);
        if (!_StringBuffer.AppendString(GetAnsiNameFormeIndex(InterfaceLuid->Info.IfType)))
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }

        if (!_StringBuffer.AppendChar('_'))
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }

        if (!_StringBuffer.AppendUint32(InterfaceLuid->Info.NetLuidIndex))
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }
        return ERROR_SUCCESS;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    iphlpapi,
    8,
    DWORD,
    NETIOAPI_API_,
    ConvertInterfaceNameToLuidA,
        _In_z_ CONST CHAR* InterfaceName,
        _Out_ NET_LUID* InterfaceLuid
        )
    {
        if (const auto _pfnConvertInterfaceNameToLuidA = try_get_ConvertInterfaceNameToLuidA())
        {
            return _pfnConvertInterfaceNameToLuidA(InterfaceName, InterfaceLuid);
        }

        auto& _Map = GetIfTypeMapItems();

        IFTYPE _uType = 0;
        DWORD _uIfIndex;

        for (auto& _Item : _Map)
        {
            const char* _szEnd;
            if (internal::StringStartsWithI(InterfaceName, _Item.szName, &_szEnd))
            {
                if (*_szEnd == '_')
                {
                    ++_szEnd;
                    if (internal::StringToUint32(_szEnd, &_uIfIndex, &_szEnd) && *_szEnd == '\0')
                    {
                        _uType = _Item.uType;
                    }
                    break;
                }
            }
        }

        if (_uType == 0)
        {
            return ERROR_INVALID_PARAMETER;
        }

        InterfaceLuid->Info.IfType = _uType;
        InterfaceLuid->Info.NetLuidIndex = _uIfIndex;
        InterfaceLuid->Info.Reserved = 0;
        return ERROR_SUCCESS;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    iphlpapi,
    8,
    DWORD,
    NETIOAPI_API_,
    ConvertInterfaceNameToLuidW,
        _In_z_ CONST WCHAR* InterfaceName,
        _Out_ NET_LUID* InterfaceLuid
        )
    {
        if (const auto _pfnConvertInterfaceNameToLuidW = try_get_ConvertInterfaceNameToLuidW())
        {
            return _pfnConvertInterfaceNameToLuidW(InterfaceName, InterfaceLuid);
        }

        auto& _Map = GetIfTypeMapItems();

        IFTYPE _uType = 0;
        DWORD _uIfIndex;

        for (auto& _Item : _Map)
        {
            const WCHAR* _szEnd;
            if (internal::StringStartsWithI(InterfaceName, _Item.szName, &_szEnd))
            {
                if (*_szEnd == '_')
                {
                    ++_szEnd;
                    if (internal::StringToUint32(_szEnd, &_uIfIndex, &_szEnd) && *_szEnd == '\0')
                    {
                        _uType = _Item.uType;
                    }
                    break;
                }
            }
        }

        if (_uType == 0)
        {
            return ERROR_INVALID_PARAMETER;
        }

        InterfaceLuid->Info.IfType = _uType;
        InterfaceLuid->Info.NetLuidIndex = _uIfIndex;
        InterfaceLuid->Info.Reserved = 0;
        return ERROR_SUCCESS;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    4,
    NET_IFINDEX,
    NETIOAPI_API_,
    if_nametoindex,
        _In_ PCSTR _szInterfaceName
        )
    {
        if (const auto _pfnif_nametoindex = try_get_if_nametoindex())
        {
            return _pfnif_nametoindex(_szInterfaceName);
        }

        auto& _Map = GetIfTypeMapItems();

        IFTYPE _uType = 0;
        MIB_IFROW _IfRow;

        for (auto& _Item : _Map)
        {
            const char* _szEnd;
            if (internal::StringStartsWithI(_szInterfaceName, _Item.szName, &_szEnd))
            {
                if (*_szEnd == '_')
                {
                    ++_szEnd;
                    if (internal::StringToUint32(_szEnd, &_IfRow.dwIndex, &_szEnd) && *_szEnd == '\0')
                    {
                        _uType = _Item.uType;
                    }
                    break;
                }
            }
        }

        if (_uType == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }

        auto _lStatus = GetIfEntry(&_IfRow);
        if (_lStatus != ERROR_SUCCESS)
        {
            SetLastError(_lStatus);
            return 0;
        }

        if (_IfRow.dwType == _uType)
        {
            return _IfRow.dwIndex;
        }
        else if (_uType == IF_TYPE_OTHER)
        {
            for (auto& _Item : _Map)
            {
                if (_Item.uType == _IfRow.dwType)
                {
                    SetLastError(ERROR_INVALID_PARAMETER);
                    return 0;
                }
            }

            return _IfRow.dwIndex;
        }
            
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    8,
    PCHAR,
    NETIOAPI_API_,
    if_indextoname,
        _In_ NET_IFINDEX InterfaceIndex,
        _Out_writes_(IF_NAMESIZE) PCHAR InterfaceName
        )
    {
        if (const auto _pfnif_indextoname = try_get_if_indextoname())
        {
            return _pfnif_indextoname(InterfaceIndex, InterfaceName);
        }

        InterfaceName[0] = '\0';
        NET_LUID InterfaceLuid;
        auto _lStatus = ::ConvertInterfaceIndexToLuid(InterfaceIndex, &InterfaceLuid);
        if (_lStatus != ERROR_SUCCESS)
        {
            SetLastError(_lStatus);
            return nullptr;
        }

        _lStatus = ::ConvertInterfaceLuidToNameA(&InterfaceLuid, InterfaceName, IF_NAMESIZE);
        if (_lStatus != ERROR_SUCCESS)
        {
            SetLastError(_lStatus);
            return nullptr;
        }
        return InterfaceName;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    iphlpapi,
    8,
    NETIO_STATUS,
    NETIOAPI_API_,
    ConvertInterfaceLuidToGuid,
        _In_ CONST NET_LUID* _pInterfaceLuid,
        _Out_ GUID* _pInterfaceGuid
        )
    {
        if (const auto _pfnConvertInterfaceLuidToGuid = try_get_ConvertInterfaceLuidToGuid())
        {
            return _pfnConvertInterfaceLuidToGuid(_pInterfaceLuid, _pInterfaceGuid);
        }
            
        if (_pInterfaceLuid == nullptr || _pInterfaceGuid == nullptr)
        {
            return ERROR_INVALID_PARAMETER;
        }        

        MIB_IFROW _IfRow;
        _IfRow.dwIndex = _pInterfaceLuid->Info.NetLuidIndex;

        auto _lStatus = GetIfEntry(&_IfRow);
        if (NO_ERROR != _lStatus)
            return _lStatus;

        if (!internal::StringToGuid(wcsrchr(_IfRow.wszName, L'{'), _pInterfaceGuid))
        {
            return ERROR_BAD_FORMAT;
        }

        return ERROR_SUCCESS;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    iphlpapi,
    8,
    NETIO_STATUS,
    NETIOAPI_API_,
    ConvertInterfaceLuidToIndex,
        _In_ CONST NET_LUID* _pInterfaceLuid,
        _Out_ PNET_IFINDEX _pInterfaceIndex
        )
    {
        if (const auto _pfnConvertInterfaceLuidToIndex = try_get_ConvertInterfaceLuidToIndex())
        {
            return _pfnConvertInterfaceLuidToIndex(_pInterfaceLuid, _pInterfaceIndex);
        }
            
        if (_pInterfaceLuid == nullptr || _pInterfaceIndex == nullptr)
        {
            return ERROR_INVALID_PARAMETER;
        }        

        *_pInterfaceIndex = _pInterfaceLuid->Info.NetLuidIndex;
        return ERROR_SUCCESS;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    20,
    NETIO_STATUS,
    NETIOAPI_API_,
    NotifyIpInterfaceChange,
        _In_ ADDRESS_FAMILY _uFamily,
        _In_ PIPINTERFACE_CHANGE_CALLBACK _pfnCallback,
        _In_opt_ PVOID _pCallerContext,
        _In_ BOOLEAN _bInitialNotification,
        _Inout_ HANDLE* _phNotificationHandle
        )
    {
        if (const auto _pfnNotifyIpInterfaceChange = try_get_NotifyIpInterfaceChange())
        {
            return _pfnNotifyIpInterfaceChange(_uFamily, _pfnCallback, _pCallerContext, _bInitialNotification, _phNotificationHandle);
        }

        if (!_phNotificationHandle)
        {
            return ERROR_INVALID_PARAMETER;
        }

        *_phNotificationHandle = nullptr;
        if (_pfnCallback == nullptr
            || (_uFamily != AF_INET && _uFamily != AF_INET6 && _uFamily != AF_UNSPEC))
        {
            return ERROR_INVALID_PARAMETER;
        }

        struct NotifyIpInterfaceChangeDownlevelContext : MibChangeNotifyDownlevelContext
        {
            ADDRESS_FAMILY uFamily = AF_UNSPEC;
            PIPINTERFACE_CHANGE_CALLBACK pfnCallback = nullptr;
            PVOID pCallerContext = nullptr;
            HANDLE hWaitObject = nullptr;
            HANDLE hNotifyHandle = nullptr;
            OVERLAPPED Overlapped = {};
            PIP_ADAPTER_ADDRESSES pAdapters = nullptr;

            ~NotifyIpInterfaceChangeDownlevelContext() override
            {
                Cancel();

                if (Overlapped.hEvent)
                {
                    CloseHandle(Overlapped.hEvent);
                    Overlapped.hEvent = nullptr;
                }

                if (pAdapters)
                {
                    internal::Free(pAdapters);
                    pAdapters = nullptr;
                }
            }

            LSTATUS __fastcall InstallNotify()
            {
                auto _lStatus = NotifyAddrChange(&hNotifyHandle, &Overlapped);
                if (_lStatus == ERROR_IO_PENDING || _lStatus == ERROR_SUCCESS)
                {
                    return ERROR_SUCCESS;
                }

                return _lStatus;
            }

            LSTATUS __fastcall Cancel() override
            {
                if (InterlockedExchange(&uSignature, 0) == 0)
                {
                    return NO_ERROR;
                }

                if (auto _hWaitObject = InterlockedExchangePointer(&hWaitObject, NULL))
                {
                    UnregisterWaitEx(_hWaitObject, INVALID_HANDLE_VALUE);
                }

                if (hNotifyHandle)
                {
                    ::CancelIPChangeNotify(&Overlapped);
                    hNotifyHandle = NULL;
                }

                return NO_ERROR;
            }

            void __fastcall Fired(ADDRESS_FAMILY _uFamily, const IP_ADAPTER_ADDRESSES* _pAdapter, MIB_NOTIFICATION_TYPE _eNotificationType)
            {
                MIB_IPINTERFACE_ROW _Row;
                if (uFamily == AF_UNSPEC || uFamily == _uFamily)
                {
                    BuildIpInterfaceRowFromAdapter(_uFamily, _pAdapter, &_Row);
                    pfnCallback(pCallerContext, &_Row, _eNotificationType);
                }
            }

            void __fastcall DiffAdapters(ADDRESS_FAMILY _uFamily, const IP_ADAPTER_ADDRESSES* _pNewAdapters)
            {
                if (uFamily != AF_UNSPEC && uFamily != _uFamily)
                {
                    return;
                }

                for (auto _pAdapter = _pNewAdapters; _pAdapter != nullptr; _pAdapter = _pAdapter->Next)
                {
                    auto _uInterfaceIndex = GetUnicastAddressInterfaceIndex(_uFamily, _pAdapter);
                    if (_uInterfaceIndex == 0)
                        continue;

                    auto _pOldAdapter = FindAdapterByInterfaceIndex(pAdapters, _uFamily, _uInterfaceIndex);
                    if (!_pOldAdapter)
                    {
                        // 新增的适配器
                        Fired(_uFamily, _pAdapter, MibAddInstance);
                        Fired(_uFamily, _pAdapter, MibParameterNotification);
                    }
                    else if (!IsSameIpInterfaceParameters(_pOldAdapter, _pAdapter))
                    {
                        Fired(_uFamily, _pAdapter, MibParameterNotification);
                    }
                }

                for (auto _pOldAdapter = pAdapters; _pOldAdapter != nullptr; _pOldAdapter = _pOldAdapter->Next)
                {
                    auto _uInterfaceIndex = GetUnicastAddressInterfaceIndex(_uFamily, _pOldAdapter);
                    if (_uInterfaceIndex == 0)
                        continue;

                    auto _pNewAdapter = FindAdapterByInterfaceIndex(_pNewAdapters, _uFamily, _uInterfaceIndex);
                    if (!_pNewAdapter)
                    {
                        // 删除的适配器
                        Fired(_uFamily, _pOldAdapter, MibParameterNotification);
                        Fired(_uFamily, _pOldAdapter, MibDeleteInstance);
                    }
                }
            }
        };

        auto _pContext = internal::New<NotifyIpInterfaceChangeDownlevelContext>();
        if (_pContext == nullptr)
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }

        _pContext->uFamily = _uFamily;
        _pContext->pfnCallback = _pfnCallback;
        _pContext->pCallerContext = _pCallerContext;

        auto _lStatus = GetAdaptersAddressesSnapshot(_uFamily, &_pContext->pAdapters);
        if (_lStatus != NO_ERROR)
        {
            internal::Delete(_pContext);
            return _lStatus;
        }

        _pContext->Overlapped.hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (_pContext->Overlapped.hEvent == nullptr)
        {
            _lStatus = GetLastError();
            internal::Delete(_pContext);
            return _lStatus;
        }

        _lStatus = _pContext->InstallNotify();
        if (_lStatus != NO_ERROR)
        {
            internal::Delete(_pContext);
            return _lStatus;
        }

        if (_bInitialNotification)
        {
            if (_uFamily == AF_UNSPEC)
            {
                _pfnCallback(_pCallerContext, nullptr, MibInitialNotification);
            }

            _pfnCallback(_pCallerContext, nullptr, MibInitialNotification);
        }

        if (!RegisterWaitForSingleObject(
            &_pContext->hWaitObject,
            _pContext->Overlapped.hEvent,
            [](_In_ PVOID _pParameter, _In_ BOOLEAN _bTimerOrWaitFired)
            {
                UNREFERENCED_PARAMETER(_bTimerOrWaitFired);
                auto _pContext = reinterpret_cast<NotifyIpInterfaceChangeDownlevelContext*>(_pParameter);

                PIP_ADAPTER_ADDRESSES _pNewAdapters = nullptr;
                auto _lStatus = GetAdaptersAddressesSnapshot(_pContext->uFamily, &_pNewAdapters);
                if (_lStatus == NO_ERROR)
                {
                    _pContext->DiffAdapters(AF_INET, _pNewAdapters);
                    _pContext->DiffAdapters(AF_INET6, _pNewAdapters);

                    auto _pOldAdapters = _pContext->pAdapters;
                    _pContext->pAdapters = _pNewAdapters;
                    _pNewAdapters = nullptr;
                    if (_pOldAdapters)
                    {
                        internal::Free(_pOldAdapters);
                    }
                }

                _pContext->InstallNotify();
            },
            _pContext,
            INFINITE,
            WT_EXECUTEDEFAULT))
        {
            // 这里失败了也没有太好的补救措施，通知已经注册成功了，只是无法收到后续通知了。
        }

        *_phNotificationHandle = reinterpret_cast<HANDLE>(_pContext);
        return NO_ERROR;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    8,
    NETIO_STATUS,
    NETIOAPI_API_,
    GetUnicastIpAddressTable,
        _In_ ADDRESS_FAMILY _uFamily,
        _Outptr_ PMIB_UNICASTIPADDRESS_TABLE* _ppTable
        )
    {
        if (const auto _pfnGetUnicastIpAddressTable = try_get_GetUnicastIpAddressTable())
        {
            return _pfnGetUnicastIpAddressTable(_uFamily, _ppTable);
        }

        return GetUnicastIpAddressTableDownlevel(_uFamily, _ppTable);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    20,
    NETIO_STATUS,
    NETIOAPI_API_,
    NotifyStableUnicastIpAddressTable,
        _In_ ADDRESS_FAMILY _uFamily,
        _Outptr_ PMIB_UNICASTIPADDRESS_TABLE* _ppTable,
        _In_ PSTABLE_UNICAST_IPADDRESS_TABLE_CALLBACK _pfnCallback,
        _In_ PVOID _pCallerContext,
        _Inout_ HANDLE* _phNotificationHandle
        )
    {
        if (const auto _pfnNotifyStableUnicastIpAddressTable = try_get_NotifyStableUnicastIpAddressTable())
        {
            return _pfnNotifyStableUnicastIpAddressTable(_uFamily, _ppTable, _pfnCallback, _pCallerContext, _phNotificationHandle);
        }

        if (_ppTable == nullptr || _phNotificationHandle == nullptr)
        {
            return ERROR_INVALID_PARAMETER;
        }

        *_ppTable = nullptr;
        *_phNotificationHandle = nullptr;

        if (_pfnCallback == nullptr
            || (_uFamily != AF_INET && _uFamily != AF_INET6 && _uFamily != AF_UNSPEC))
        {
            return ERROR_INVALID_PARAMETER;
        }

        struct StableUnicastIpAddressChangeContext : MibChangeNotifyDownlevelContext
        {
            ADDRESS_FAMILY uFamily = AF_UNSPEC;
            PSTABLE_UNICAST_IPADDRESS_TABLE_CALLBACK pfnCallback = nullptr;
            PVOID pCallerContext = nullptr;
            HANDLE hWaitObject = nullptr;
            HANDLE hNotifyHandle = nullptr;
            OVERLAPPED Overlapped = {};

            ~StableUnicastIpAddressChangeContext() override
            {
                Cancel();

                if (Overlapped.hEvent)
                {
                    CloseHandle(Overlapped.hEvent);
                    Overlapped.hEvent = nullptr;
                }
            }

            LSTATUS __fastcall Cancel() override
            {
                if (InterlockedExchange(&uSignature, 0) == 0)
                {
                    return NO_ERROR;
                }

                if (auto _hWaitObject = InterlockedExchangePointer(&hWaitObject, NULL))
                {
                    UnregisterWaitEx(_hWaitObject, INVALID_HANDLE_VALUE);
                }

                if (hNotifyHandle)
                {
                    ::CancelIPChangeNotify(&Overlapped);
                    hNotifyHandle = NULL;
                }

                return NO_ERROR;
            }

            LSTATUS __fastcall InstallNotify()
            {
                auto _lStatus = NotifyAddrChange(&hNotifyHandle, &Overlapped);
                if (_lStatus == ERROR_IO_PENDING || _lStatus == ERROR_SUCCESS)
                {
                    return ERROR_SUCCESS;
                }

                return _lStatus;
            }
        };

        auto _pContext = internal::New<StableUnicastIpAddressChangeContext>();
        if (_pContext == nullptr)
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }

        _pContext->uFamily = _uFamily;
        _pContext->pfnCallback = _pfnCallback;
        _pContext->pCallerContext = _pCallerContext;

        _pContext->Overlapped.hEvent= CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (_pContext->Overlapped.hEvent == nullptr)
        {
            auto _lStatus = GetLastError();
            internal::Delete(_pContext);
            return _lStatus;
        }

        auto _lStatus = _pContext->InstallNotify();
        if (_lStatus != NO_ERROR)
        {
            internal::Delete(_pContext);
            return _lStatus;
        }

        if (!RegisterWaitForSingleObject(
            &_pContext->hWaitObject,
            _pContext->Overlapped.hEvent,
            [](_In_ PVOID _pParameter, _In_ BOOLEAN _bTimerOrWaitFired)
            {
                UNREFERENCED_PARAMETER(_bTimerOrWaitFired);
                auto _pContext = reinterpret_cast<StableUnicastIpAddressChangeContext*>(_pParameter);

                if (_bTimerOrWaitFired)
                {
                    PMIB_UNICASTIPADDRESS_TABLE _pTable = nullptr;
                    auto _lStatus = GetUnicastIpAddressTableDownlevel(_pContext->uFamily, &_pTable);
                    if (_lStatus == NO_ERROR)
                    {
                        _pContext->pfnCallback(_pContext->pCallerContext, _pTable);
                        internal::Free(_pTable);
                    }
                    else
                    {
                        MIB_UNICASTIPADDRESS_TABLE _oTable = {};
                        _pContext->pfnCallback(_pContext->pCallerContext, &_oTable);
                    }

                    if (auto _hWaitObject = InterlockedExchangePointer(&_pContext->hWaitObject, NULL))
                    {
                        UnregisterWaitEx(_hWaitObject, NULL);
                    }
                    return;
                }
                else
                {
                    _pContext->InstallNotify();
                }
            },
            _pContext,
            5 * 1000,
            WT_EXECUTEDEFAULT))
        {
            _lStatus = GetLastError();
            internal::Delete(_pContext);
            return _lStatus;
        }

        *_phNotificationHandle = reinterpret_cast<HANDLE>(_pContext);
        return ERROR_IO_PENDING;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    4,
    NETIO_STATUS,
    NETIOAPI_API_,
    CancelMibChangeNotify2,
        _In_ HANDLE _hNotificationHandle
        )
    {
        if (const auto _pfnCancelMibChangeNotify2 = try_get_CancelMibChangeNotify2())
        {
            return _pfnCancelMibChangeNotify2(_hNotificationHandle);
        }

        if (_hNotificationHandle == NULL)
        {
            return ERROR_INVALID_PARAMETER;
        }
        else if (_hNotificationHandle == reinterpret_cast<HANDLE>(2))
        {
            // 早期 NotifyIpInterfaceChange返回的句柄始终等于 2。
            return NO_ERROR;
        }
        else
        {
            auto _pBaseContext = reinterpret_cast<MibChangeNotifyDownlevelContext*>(_hNotificationHandle);
            __try
            {
                if (_pBaseContext->uSignature != kMibChangeNotifyDownlevelContextSignature)
                {
                    return ERROR_INVALID_PARAMETER;
                }

                auto _lStatus = _pBaseContext->Cancel();
                internal::Delete(_pBaseContext);
                return _lStatus;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return ERROR_INVALID_PARAMETER;
            }
        }

        return NO_ERROR;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    iphlpapi,
    12,
    ULONG,
    WINAPI,
    GetTcpTable2,
        _Out_writes_bytes_opt_(*_pSizePointer)   PMIB_TCPTABLE2 _pTcpTable,
        _Inout_                          PULONG         _pSizePointer,
        _In_                             BOOL           _bOrder
        )
    {
        if (const auto _pfnGetTcpTable2 = try_get_GetTcpTable2())
        {
            return _pfnGetTcpTable2(_pTcpTable, _pSizePointer, _bOrder);
        }

        if (_pSizePointer == nullptr)
        {
            return ERROR_INVALID_PARAMETER;
        }

        DWORD _uOwnerPidTableSize = *_pSizePointer;
        auto _pOwnerPidTable = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(_pTcpTable);
        const auto _lStatus = GetExtendedTcpTable(_pOwnerPidTable, &_uOwnerPidTableSize, _bOrder, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
        if (_lStatus == ERROR_INSUFFICIENT_BUFFER)
        {
            const auto _uNeedSize = FIELD_OFFSET(MIB_TCPTABLE2, table) + (_uOwnerPidTableSize - FIELD_OFFSET(MIB_TCPTABLE_OWNER_PID, table)) / sizeof(MIB_TCPROW_OWNER_PID) * sizeof(MIB_TCPROW2);
            *_pSizePointer = _uNeedSize;
            return ERROR_INSUFFICIENT_BUFFER;
        }
        else if (_lStatus != NO_ERROR)
        {
            return _lStatus;
        }
        
        const auto _uNeedSize = FIELD_OFFSET(MIB_TCPTABLE2, table) + (_uOwnerPidTableSize - FIELD_OFFSET(MIB_TCPTABLE_OWNER_PID, table)) / sizeof(MIB_TCPROW_OWNER_PID) * sizeof(MIB_TCPROW2);
        if (*_pSizePointer < _uNeedSize)
        {
            *_pSizePointer = _uNeedSize;
            return ERROR_INSUFFICIENT_BUFFER;
        }

        for (DWORD i = _pOwnerPidTable->dwNumEntries; i != 0; )
        {
            --i;
            _pTcpTable->table[i].dwOffloadState = TcpConnectionOffloadStateInHost;
            _pTcpTable->table[i].dwOwningPid = _pOwnerPidTable->table[i].dwOwningPid;
            _pTcpTable->table[i].dwRemotePort = _pOwnerPidTable->table[i].dwRemotePort;
            _pTcpTable->table[i].dwRemoteAddr = _pOwnerPidTable->table[i].dwRemoteAddr;
            _pTcpTable->table[i].dwLocalPort = _pOwnerPidTable->table[i].dwLocalPort;
            _pTcpTable->table[i].dwLocalAddr = _pOwnerPidTable->table[i].dwLocalAddr;
            _pTcpTable->table[i].dwState = _pOwnerPidTable->table[i].dwState;
        }

        *_pSizePointer = _uNeedSize;
        return NO_ERROR;
    }
#endif
}
