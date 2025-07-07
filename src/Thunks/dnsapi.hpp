#if (YY_Thunks_Target < __WindowsNT6_2)
#include <windns.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_2) && !defined(__Comment_Lib_dnsapi)
#define __Comment_Lib_dnsapi
#pragma comment(lib, "Dnsapi.lib")
#endif

#if defined(YY_Thunks_Implemented) && (YY_Thunks_Target < __WindowsNT6_2)
namespace YY::Thunks::Fallback
{
    namespace
    {
        union DnsQueryCancel
        {
            DNS_QUERY_CANCEL CancelHandle;

            struct
            {
                volatile LSTATUS lStatus;
            };
        };

        struct DnsQuerySharedData
        {
            _In_opt_ PWSTR szQueryName;
            _In_ WORD QueryType;
            _In_ ULONG64 QueryOptions;
            _In_opt_ IP4_ARRAY* pIpv4DnsServerList;
            _In_ PVOID pQueryContext;
            _In_opt_ PDNS_QUERY_COMPLETION_ROUTINE   pQueryCompletionCallback;
            PDNS_QUERY_RESULT pQueryResults;
            _In_opt_ DnsQueryCancel* pCancelHandle;
            BYTE Buffer[0];
        };

        IP4_ARRAY* __fastcall DNS_ADDR_ARRAY_To_IP4_ARRAY(PDNS_ADDR_ARRAY _pDnsServerList, IP4_ARRAY* _pIpv4DnsServerList)
        {
            if (_pDnsServerList == nullptr || _pDnsServerList->AddrCount == 0)
                return nullptr;

            _pIpv4DnsServerList->AddrCount = _pDnsServerList->AddrCount;

            for (DWORD i = 0; i < _pDnsServerList->AddrCount; ++i)
            {
                _pIpv4DnsServerList->AddrArray[i] = reinterpret_cast<sockaddr_in*>(_pDnsServerList->AddrArray[i].MaxSa)->sin_addr.S_un.S_addr;
            }

            return _pIpv4DnsServerList;
        }
    }
}
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    // Minimum supported client	Windows 8 [desktop apps only]
    // Minimum supported server	Windows Server 2012[desktop apps only]
    __DEFINE_THUNK(
    dnsapi,
    12,
    DNS_STATUS,
    WINAPI,
    DnsQueryEx,
        _In_        PDNS_QUERY_REQUEST _pQueryRequest,
        _Inout_     PDNS_QUERY_RESULT _pQueryResults,
        _Inout_opt_ PDNS_QUERY_CANCEL _pCancelHandle
        )
    {
        if (const auto _pfnDnsQueryEx = try_get_DnsQueryEx())
        {
            return _pfnDnsQueryEx(_pQueryRequest, _pQueryResults, _pCancelHandle);
        }

        if (_pQueryRequest == nullptr || _pQueryResults == nullptr)
        {
            return ERROR_INVALID_PARAMETER;
        }

        if (_pQueryRequest->Version != DNS_QUERY_REQUEST_VERSION1)
        {
            return ERROR_INVALID_PARAMETER;
        }

        if (_pQueryResults->Version != DNS_QUERY_RESULTS_VERSION1)
        {
            return ERROR_INVALID_PARAMETER;
        }

        if (auto _pDnsServerList = _pQueryRequest->pDnsServerList)
        {
            if (_pDnsServerList->AddrCount && _pDnsServerList->Family != AF_INET)
            {
                // DnsQuery_W只支持Ipv4
                return ERROR_INVALID_PARAMETER;
            }
        }

        _pQueryResults->pQueryRecords = nullptr;
        _pQueryResults->QueryOptions = _pQueryRequest->QueryOptions;

        const auto _cbIpv4DnsServerList = (_pQueryRequest->pDnsServerList && _pQueryRequest->pDnsServerList->AddrCount) ? (sizeof(IP4_ARRAY) + (_pQueryRequest->pDnsServerList->AddrCount - 1) * sizeof(IP4_ADDRESS)) : 0;

        if (!_pQueryRequest->pQueryCompletionCallback)
        {
            IP4_ARRAY* _pIpv4DnsServerList = nullptr;
            BYTE _Ipv4DnsServerListStaticBuffer[sizeof(IP4_ARRAY) + (128 - 1) * sizeof(IP4_ADDRESS)];
            if (_cbIpv4DnsServerList)
            {
                if (sizeof(_Ipv4DnsServerListStaticBuffer) < _cbIpv4DnsServerList)
                {
                    _pIpv4DnsServerList = (IP4_ARRAY*)internal::Alloc(_cbIpv4DnsServerList);
                    if (!_pIpv4DnsServerList)
                    {
                        return ERROR_OUTOFMEMORY;
                    }
                }
                else
                {
                    _pIpv4DnsServerList = reinterpret_cast<IP4_ARRAY*>(_Ipv4DnsServerListStaticBuffer);
                }
                
                Fallback::DNS_ADDR_ARRAY_To_IP4_ARRAY(_pQueryRequest->pDnsServerList, _pIpv4DnsServerList);
            }

            _pQueryResults->QueryStatus = DnsQuery_W(_pQueryRequest->QueryName, _pQueryRequest->QueryType, _pQueryRequest->QueryOptions, _pIpv4DnsServerList, &_pQueryResults->pQueryRecords, nullptr);

            if ((BYTE*)_pIpv4DnsServerList != _Ipv4DnsServerListStaticBuffer)
            {
                internal::Free(_pIpv4DnsServerList);
            }
            return _pQueryResults->QueryStatus;
        }

        const auto _cbName = (_pQueryRequest->QueryName) ? (wcslen(_pQueryRequest->QueryName) + 1) * sizeof(wchar_t) : 0;
        auto _pDnsQueryCancel = reinterpret_cast<Fallback::DnsQueryCancel*>(_pCancelHandle);
        auto _pSharedData = (Fallback::DnsQuerySharedData*)internal::Alloc(sizeof(Fallback::DnsQuerySharedData) + _cbIpv4DnsServerList + _cbName);
        if (!_pSharedData)
        {
            return ERROR_OUTOFMEMORY;
        }

        auto _pBuffer = _pSharedData->Buffer;
        _pSharedData->szQueryName = nullptr;
        if (_cbName)
        {
            _pSharedData->szQueryName = reinterpret_cast<PWSTR>(_pBuffer);
            memcpy(_pSharedData->szQueryName, _pQueryRequest->QueryName, _cbName);
            _pBuffer += _cbName;
        }

        _pSharedData->QueryType = _pQueryRequest->QueryType;
        _pSharedData->QueryOptions = _pQueryRequest->QueryOptions;

        _pSharedData->pIpv4DnsServerList = nullptr;
        if (_cbIpv4DnsServerList)
        {
            _pSharedData->pIpv4DnsServerList = Fallback::DNS_ADDR_ARRAY_To_IP4_ARRAY(_pQueryRequest->pDnsServerList, (IP4_ARRAY*)_pBuffer);
            _pBuffer += _cbIpv4DnsServerList;
        }

        _pSharedData->pQueryContext = _pQueryRequest->pQueryContext;
        _pSharedData->pQueryCompletionCallback = _pQueryRequest->pQueryCompletionCallback;

        _pSharedData->pQueryResults = _pQueryResults;
        _pSharedData->pCancelHandle = _pDnsQueryCancel;

        if (_pDnsQueryCancel)
        {
            _pDnsQueryCancel->lStatus = DNS_REQUEST_PENDING;
        }

#if (YY_Thunks_Target < __WindowsNT6)
        auto _bRet = QueueUserWorkItem(
            [](LPVOID Context) -> DWORD
#else
        auto _bRet = TrySubmitThreadpoolCallback(
            [](_Inout_ PTP_CALLBACK_INSTANCE Instance, _Inout_opt_ PVOID Context)
#endif
            {
                auto _pSharedData = reinterpret_cast<Fallback::DnsQuerySharedData*>(Context);
                auto _pQueryResults = _pSharedData->pQueryResults;

                do
                {
                    if (_pSharedData->pCancelHandle)
                    {
                        if (_pSharedData->pCancelHandle->lStatus != DNS_REQUEST_PENDING)
                        {
                            _pSharedData->pQueryResults->QueryStatus = _pSharedData->pCancelHandle->lStatus;
                            break;
                        }
                    }

                    auto _QueryStatus = DnsQuery_W(_pSharedData->szQueryName, _pSharedData->QueryType, _pSharedData->QueryOptions, _pSharedData->pIpv4DnsServerList, &_pQueryResults->pQueryRecords, nullptr);
                    if (_pSharedData->pCancelHandle)
                    {
                        InterlockedExchange(&_pSharedData->pCancelHandle->lStatus, _QueryStatus);
                    }

                    _pSharedData->pQueryResults->QueryStatus = _QueryStatus;

                } while (false);

                _pSharedData->pQueryCompletionCallback(_pSharedData->pQueryContext, _pQueryResults);
                internal::Free(_pSharedData);

#if (YY_Thunks_Target < __WindowsNT6)
                return 0ul;
            },
            _pSharedData,
            0);
#else
            },
            _pSharedData,
            nullptr);
#endif

        if(_bRet)
            return DNS_REQUEST_PENDING;

        internal::Free(_pSharedData);
        return ERROR_FUNCTION_FAILED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // Minimum supported client	Windows 8 [desktop apps only]
    // Minimum supported server	Windows Server 2012[desktop apps only]
    __DEFINE_THUNK(
    dnsapi,
    4,
    DNS_STATUS,
    WINAPI,
    DnsCancelQuery,
            _In_ PDNS_QUERY_CANCEL _pCancelHandle
        )
    {
        if (const auto _pfnDnsCancelQuery = try_get_DnsCancelQuery())
        {
            return _pfnDnsCancelQuery(_pCancelHandle);
        }

        auto _pDnsQueryCancel = reinterpret_cast<Fallback::DnsQueryCancel*>(_pCancelHandle);

        auto _Status = InterlockedCompareExchange(&_pDnsQueryCancel->lStatus, ERROR_CANCELLED, DNS_REQUEST_PENDING);
        return ERROR_SUCCESS;
    }
#endif
}
