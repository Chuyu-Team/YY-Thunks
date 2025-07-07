#include "pch.h"
#include "Thunks/dnsapi.hpp"

namespace Dnsapi
{
    TEST_CLASS(DnsQueryEx)
    {
        AwaysNullGuard Guard;

    public:
        DnsQueryEx()
        {
            Guard |= YY::Thunks::aways_null_try_get_DnsQueryEx;
        }

        TEST_METHOD(异步查询)
        {
            DNS_QUERY_REQUEST request = {};
            DNS_QUERY_RESULT result = {};

            request.Version = DNS_QUERY_REQUEST_VERSION1;
            request.QueryName = L"learn.microsoft.com";
            request.QueryType = DNS_TYPE_A;
            request.QueryOptions = DNS_QUERY_STANDARD;
            request.pQueryContext = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            request.pQueryCompletionCallback = [](
                _In_        PVOID               pQueryContext,
                _Inout_     PDNS_QUERY_RESULT   pQueryResults)
                {
                    SetEvent(static_cast<HANDLE>(pQueryContext));
                };

            result.Version = DNS_QUERY_RESULTS_VERSION1;

            auto _Status = ::DnsQueryEx(&request, &result, nullptr);

            Assert::AreEqual(_Status, DNS_REQUEST_PENDING);

            WaitForSingleObject(request.pQueryContext, INFINITE);

            Assert::AreEqual(result.QueryStatus, ERROR_SUCCESS);

            Assert::AreEqual(result.QueryOptions, ULONG64(DNS_QUERY_STANDARD));

            Assert::IsNotNull(result.pQueryRecords);

            DnsRecordListFree(result.pQueryRecords, DnsFreeRecordList);
        }

        TEST_METHOD(同步查询)
        {
            DNS_QUERY_REQUEST request = {};
            DNS_QUERY_RESULT result = {};

            request.Version = DNS_QUERY_REQUEST_VERSION1;
            request.QueryName = L"learn.microsoft.com";
            request.QueryType = DNS_TYPE_A;
            request.QueryOptions = DNS_QUERY_STANDARD;


            result.Version = DNS_QUERY_RESULTS_VERSION1;

            auto _Status = ::DnsQueryEx(&request, &result, nullptr);

            Assert::AreEqual(_Status, ERROR_SUCCESS);

            Assert::AreEqual(result.QueryStatus, ERROR_SUCCESS);

            Assert::AreEqual(result.QueryOptions, ULONG64(DNS_QUERY_STANDARD));

            Assert::IsNotNull(result.pQueryRecords);

            DnsRecordListFree(result.pQueryRecords, DnsFreeRecordList);
        }
    };
}
