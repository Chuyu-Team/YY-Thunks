#include "pch.h"
#include "Thunks/dnsapi.hpp"

#include <ws2tcpip.h>

namespace Dnsapi
{
    TEST_CLASS(DnsQueryEx)
    {
        AwaysNullGuard Guard;

        static VOID WINAPI QueryCompletionCallback(
            _In_        PVOID               pQueryContext,
            _Inout_     PDNS_QUERY_RESULT   pQueryResults
            )
        {
            SetEvent(static_cast<HANDLE>(pQueryContext));
        }

    public:
        DnsQueryEx()
        {
            Guard |= YY::Thunks::aways_null_try_get_DnsQueryEx;
        }

        TEST_METHOD(普通查询)
        {
            DNS_QUERY_REQUEST request = {};

            request.Version = DNS_QUERY_REQUEST_VERSION1;
            request.QueryName = L"learn.microsoft.com";
            request.QueryType = DNS_TYPE_A;
            request.QueryOptions = DNS_QUERY_STANDARD;


            // 同步查询
            {
                DNS_QUERY_RESULT result = {};
                result.Version = DNS_QUERY_RESULTS_VERSION1;

                auto _Status = ::DnsQueryEx(&request, &result, nullptr);

                Assert::AreEqual(_Status, ERROR_SUCCESS);

                Assert::AreEqual(result.QueryStatus, ERROR_SUCCESS);

                Assert::IsNotNull(result.pQueryRecords);

                DnsRecordListFree(result.pQueryRecords, DnsFreeRecordList);
            }

            // 异步查询
            {
                request.pQueryContext = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                request.pQueryCompletionCallback = QueryCompletionCallback;

                DNS_QUERY_RESULT result = {};
                result.Version = DNS_QUERY_RESULTS_VERSION1;

                auto _Status = ::DnsQueryEx(&request, &result, nullptr);
                if (_Status != ERROR_SUCCESS)
                {
                    Assert::AreEqual(_Status, DNS_REQUEST_PENDING);

                    Assert::AreEqual(WaitForSingleObject(request.pQueryContext, 20 * 1000), DWORD(WAIT_OBJECT_0));

                    Assert::AreEqual(result.QueryStatus, ERROR_SUCCESS);
                }

                Assert::IsNotNull(result.pQueryRecords);

                DnsRecordListFree(result.pQueryRecords, DnsFreeRecordList);
                CloseHandle(request.pQueryContext);
            }
        }

        TEST_METHOD(本地查询)
        {
            DNS_QUERY_REQUEST request = {};

            request.Version = DNS_QUERY_REQUEST_VERSION1;
            request.QueryName = L"";
            request.QueryType = DNS_TYPE_A;
            request.QueryOptions = DNS_QUERY_STANDARD;


            // 同步查询
            {
                DNS_QUERY_RESULT result = {};
                result.Version = DNS_QUERY_RESULTS_VERSION1;

                auto _Status = ::DnsQueryEx(&request, &result, nullptr);

                Assert::AreEqual(_Status, ERROR_SUCCESS);

                Assert::AreEqual(result.QueryStatus, ERROR_SUCCESS);

                Assert::IsNotNull(result.pQueryRecords);

                DnsRecordListFree(result.pQueryRecords, DnsFreeRecordList);
            }

            // 异步查询
            {
                request.pQueryCompletionCallback = QueryCompletionCallback;

                DNS_QUERY_RESULT result = {};
                result.Version = DNS_QUERY_RESULTS_VERSION1;

                auto _Status = ::DnsQueryEx(&request, &result, nullptr);

                // 本地查询时接口内部会强制到同步
                Assert::AreEqual(_Status, ERROR_SUCCESS);


                Assert::IsNotNull(result.pQueryRecords);

                DnsRecordListFree(result.pQueryRecords, DnsFreeRecordList);
                CloseHandle(request.pQueryContext);
            }
        }

        TEST_METHOD(使用特定Ipv4Dns服务器)
        {
            DNS_QUERY_REQUEST request = {};

            DNS_ADDR_ARRAY DnsServerList = {};
            DnsServerList.MaxCount = sizeof(DnsServerList);
            DnsServerList.AddrCount = 1;
            DnsServerList.Family = AF_INET;
            
            auto _pSockAddr = reinterpret_cast<SOCKADDR_IN*>(DnsServerList.AddrArray[0].MaxSa);
            Assert::AreEqual(inet_pton(DnsServerList.Family, "8.8.8.8", &_pSockAddr->sin_addr), 1);
            _pSockAddr->sin_family = DnsServerList.Family;

            request.Version = DNS_QUERY_REQUEST_VERSION1;
            request.QueryName = L"learn.microsoft.com";
            request.QueryType = DNS_TYPE_A;
            request.QueryOptions = DNS_QUERY_STANDARD;
            request.pDnsServerList = &DnsServerList;

            // 同步查询
            {
                DNS_QUERY_RESULT result = {};
                result.Version = DNS_QUERY_RESULTS_VERSION1;

                auto _Status = ::DnsQueryEx(&request, &result, nullptr);

                Assert::AreEqual(_Status, ERROR_SUCCESS);

                Assert::AreEqual(result.QueryStatus, ERROR_SUCCESS);

                Assert::IsNotNull(result.pQueryRecords);

                DnsRecordListFree(result.pQueryRecords, DnsFreeRecordList);
            }

            // 异步查询
            {
                request.pQueryContext = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                request.pQueryCompletionCallback = QueryCompletionCallback;

                DNS_QUERY_RESULT result = {};
                result.Version = DNS_QUERY_RESULTS_VERSION1;

                auto _Status = ::DnsQueryEx(&request, &result, nullptr);
                if (_Status != ERROR_SUCCESS)
                {
                    Assert::AreEqual(_Status, DNS_REQUEST_PENDING);

                    Assert::AreEqual(WaitForSingleObject(request.pQueryContext, 20 * 1000), DWORD(WAIT_OBJECT_0));

                    Assert::AreEqual(result.QueryStatus, ERROR_SUCCESS);
                }

                Assert::IsNotNull(result.pQueryRecords);

                DnsRecordListFree(result.pQueryRecords, DnsFreeRecordList);
            }
        }

        TEST_METHOD(使用特定Ipv6Dns服务器)
        {
            DNS_QUERY_REQUEST request = {};

            DNS_ADDR_ARRAY DnsServerList = {};
            DnsServerList.MaxCount = sizeof(DnsServerList);
            DnsServerList.AddrCount = 1;
            DnsServerList.Family = AF_INET6;

            auto _pSockAddr = reinterpret_cast<sockaddr_in6*>(DnsServerList.AddrArray[0].MaxSa);
            Assert::AreEqual(inet_pton(DnsServerList.Family, "2001:4860:4860::8888", &_pSockAddr->sin6_addr), 1);
            _pSockAddr->sin6_family = DnsServerList.Family;

            request.Version = DNS_QUERY_REQUEST_VERSION1;
            request.QueryName = L"learn.microsoft.com";
            request.QueryType = DNS_TYPE_AAAA;
            request.QueryOptions = DNS_QUERY_STANDARD;
            request.pDnsServerList = &DnsServerList;

            // 同步查询
            {
                DNS_QUERY_RESULT result = {};
                result.Version = DNS_QUERY_RESULTS_VERSION1;

                auto _Status = ::DnsQueryEx(&request, &result, nullptr);

                if (_Status == ERROR_TIMEOUT || _Status == WSAENETUNREACH)
                {
                    Logger::WriteMessage(L"当前网络不支持IPv6，可能会导致超时，我们暂时忽略。");
                    return;
                }
                Assert::AreEqual(_Status, ERROR_SUCCESS);

                Assert::AreEqual(result.QueryStatus, ERROR_SUCCESS);

                Assert::IsNotNull(result.pQueryRecords);

                DnsRecordListFree(result.pQueryRecords, DnsFreeRecordList);
            }

            // 异步查询
            {
                request.pQueryContext = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                request.pQueryCompletionCallback = QueryCompletionCallback;

                DNS_QUERY_RESULT result = {};
                result.Version = DNS_QUERY_RESULTS_VERSION1;

                auto _Status = ::DnsQueryEx(&request, &result, nullptr);
                if (_Status != ERROR_SUCCESS)
                {
                    Assert::AreEqual(_Status, DNS_REQUEST_PENDING);

                    Assert::AreEqual(WaitForSingleObject(request.pQueryContext, 20 * 1000), DWORD(WAIT_OBJECT_0));

                    Assert::AreEqual(result.QueryStatus, ERROR_SUCCESS);
                }

                Assert::IsNotNull(result.pQueryRecords);

                DnsRecordListFree(result.pQueryRecords, DnsFreeRecordList);
            }
        }
    };
}
