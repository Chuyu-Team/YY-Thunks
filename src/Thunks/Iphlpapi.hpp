#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS2)

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Iphlpapi.lib")
#endif

namespace YY
{
	namespace Thunks
	{
#if defined(YY_Thunks_Implemented)
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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

#endif // (YY_Thunks_Support_Version < NTDDI_WIN6)
#endif // !YY_Thunks_Implemented
		
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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
		
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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
			_IfRow.dwIndex = _pRow->InterfaceLuid.Value ? _pRow->InterfaceLuid.Info.NetLuidIndex : _pRow->InterfaceIndex;

			auto _lStatus = GetIfEntry(&_IfRow);
			if (NO_ERROR != _lStatus)
				return _lStatus;

			MIB_IFROW_To_MIB_IF_ROW2(&_IfRow, _pRow);
			return NO_ERROR;
		}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS2)

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

			return GetIfEntry2(_pRow);
		}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

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
	}
}
