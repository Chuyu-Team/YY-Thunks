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

        struct DnsExtraInfo
        {
            uint32_t Version = 0x80000001ul;
            uint32_t Size;
            void* pNext = nullptr;
            uint32_t Id = 10;
            uint32_t Reserved = 0;
            PDNS_ADDR_ARRAY pDnsServerList = nullptr;
        };

        struct DnsQuerySharedData
        {
            _In_opt_ PWSTR szQueryName;
            _In_ WORD QueryType;
            _In_ ULONG64 QueryOptions;
            _In_opt_ void* pExtra;
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

        bool __fastcall IsAsyncDnsQuery(_In_ PDNS_QUERY_REQUEST _pQueryRequest)
        {
            if (!_pQueryRequest->pQueryCompletionCallback)
            {
                return false;
            }

            // 微软规定，查询本地名称的 DNS_TYPE_A/DNS_TYPE_AAAA，接口是同步的
            if (_pQueryRequest->QueryName[0] == L'\0' && (_pQueryRequest->QueryType == DNS_TYPE_A || _pQueryRequest->QueryType == DNS_TYPE_AAAA))
            {
                return false;
            }

            return true;
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
            if (_pDnsServerList->Family == AF_INET)
            {
                // IPv4 Dns
                for (DWORD i = 0; i != _pDnsServerList->AddrCount; ++i)
                {
                    auto _pSockAddr = reinterpret_cast<SOCKADDR_IN*>(_pDnsServerList->AddrArray[i].MaxSa);
                    if (_pSockAddr->sin_family != AF_INET)
                    {
                        return ERROR_INVALID_PARAMETER;
                    }

                    if(_pSockAddr->sin_port != 0)
                    {
                        // 端口号必须为0，为啥你问微软去吧……
                        return ERROR_INVALID_PARAMETER;
                    }
                }
            }
            else if (_pDnsServerList->Family == AF_INET6)
            {
                // IPv6 Dns
                for (DWORD i = 0; i != _pDnsServerList->AddrCount; ++i)
                {
                    auto _pSockAddr = reinterpret_cast<sockaddr_in6*>(_pDnsServerList->AddrArray[i].MaxSa);
                    if (_pSockAddr->sin6_family != AF_INET6)
                    {
                        return ERROR_INVALID_PARAMETER;
                    }

                    if (_pSockAddr->sin6_port != 0)
                    {
                        // 端口号必须为0，为啥你问微软去吧……
                        return ERROR_INVALID_PARAMETER;
                    }
                }
            }
            else
            {
                return ERROR_INVALID_PARAMETER;
            }
        }

        if(_pQueryRequest->QueryName == nullptr)
        {
            // 文档虽然说QueryName可以为nullptr，但实际测试发现此时会返回失败
            return ERROR_INVALID_PARAMETER;
        }

        _pQueryResults->pQueryRecords = nullptr;
        _pQueryResults->QueryOptions = _pQueryRequest->QueryOptions;

        if (!Fallback::IsAsyncDnsQuery(_pQueryRequest))
        {
            void* _pExtra = nullptr;
            BYTE _DnsServerListStaticBuffer[sizeof(IP4_ARRAY) + (128 - 1) * sizeof(IP4_ADDRESS)];
            if (_pQueryRequest->pDnsServerList && _pQueryRequest->pDnsServerList->AddrCount)
            {
                if (_pQueryRequest->pDnsServerList->Family == AF_INET)
                {
                    const auto _cbIpv4DnsServerList = sizeof(IP4_ARRAY) + (_pQueryRequest->pDnsServerList->AddrCount - 1) * sizeof(IP4_ADDRESS);
                    if (sizeof(_DnsServerListStaticBuffer) < _cbIpv4DnsServerList)
                    {
                        _pExtra = internal::Alloc(_cbIpv4DnsServerList);
                        if (!_pExtra)
                        {
                            return ERROR_OUTOFMEMORY;
                        }
                    }
                    else
                    {
                        _pExtra = _DnsServerListStaticBuffer;
                    }

                    Fallback::DNS_ADDR_ARRAY_To_IP4_ARRAY(_pQueryRequest->pDnsServerList, (IP4_ARRAY*)_pExtra);
                }
                else
                {
                    auto _pDnsExtraInfo = (Fallback::DnsExtraInfo*)_DnsServerListStaticBuffer;
                    _pExtra = _pDnsExtraInfo;

                    new(_pDnsExtraInfo) Fallback::DnsExtraInfo();
                    _pDnsExtraInfo->pDnsServerList = _pQueryRequest->pDnsServerList;
                }
            }

            _pQueryResults->QueryStatus = DnsQuery_W(_pQueryRequest->QueryName, _pQueryRequest->QueryType, _pQueryRequest->QueryOptions, _pExtra, &_pQueryResults->pQueryRecords, nullptr);

            if (_pExtra != _DnsServerListStaticBuffer)
            {
                internal::Free(_pExtra);
            }
            return _pQueryResults->QueryStatus;
        }

        const auto _cbName = (wcslen(_pQueryRequest->QueryName) + 1) * sizeof(wchar_t);
        auto _pDnsQueryCancel = reinterpret_cast<Fallback::DnsQueryCancel*>(_pCancelHandle);

        uint32_t _cbDnsServerList = 0;
        if (_pQueryRequest->pDnsServerList && _pQueryRequest->pDnsServerList->AddrCount)
        {
            if (_pQueryRequest->pDnsServerList->Family == AF_INET)
            {
                _cbDnsServerList = sizeof(IP4_ARRAY) + (_pQueryRequest->pDnsServerList->AddrCount - 1) * sizeof(IP4_ADDRESS);
            }
            else
            {
                // IPv6 Dns，必须使用DnsExtraInfo
                _cbDnsServerList = sizeof(Fallback::DnsExtraInfo) + sizeof(_DnsAddrArray) + (_pQueryRequest->pDnsServerList->AddrCount - 1) * sizeof(DNS_ADDR);
            }
        }

        auto _pSharedData = (Fallback::DnsQuerySharedData*)internal::Alloc(sizeof(Fallback::DnsQuerySharedData) + _cbDnsServerList + _cbName);
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

        _pSharedData->pExtra = nullptr;
        if (_cbDnsServerList)
        {
            if (_pQueryRequest->pDnsServerList->Family == AF_INET)
            {
                _pSharedData->pExtra = Fallback::DNS_ADDR_ARRAY_To_IP4_ARRAY(_pQueryRequest->pDnsServerList, (IP4_ARRAY*)_pBuffer);
            }
            else
            {
                // IPv6 Dns，必须使用DnsExtraInfo
                auto _pDnsExtraInfo = reinterpret_cast<Fallback::DnsExtraInfo*>(_pBuffer);
                _pSharedData->pExtra = _pDnsExtraInfo;

                new(_pDnsExtraInfo) Fallback::DnsExtraInfo();
                _pDnsExtraInfo->pDnsServerList = reinterpret_cast<PDNS_ADDR_ARRAY>(&_pDnsExtraInfo[1]);
                memcpy(_pDnsExtraInfo->pDnsServerList, _pQueryRequest->pDnsServerList, _cbDnsServerList - sizeof(Fallback::DnsExtraInfo));
            }

            _pBuffer += _cbDnsServerList;
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

                    auto _QueryStatus = DnsQuery_W(_pSharedData->szQueryName, _pSharedData->QueryType, _pSharedData->QueryOptions, _pSharedData->pExtra, &_pQueryResults->pQueryRecords, nullptr);
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
