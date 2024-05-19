#include "pch.h"
#include "Thunks/WinHttp.hpp"

namespace WinHppt
{
    std::wstring ToString(const WINHTTP_PROXY_RESULT_ENTRY& _Item)
    {
        CStringW _Result;
        _Result.Format(L"%d %d %d %s:%d", _Item.fProxy, _Item.fBypass, _Item.ProxyScheme, _Item.pwszProxy, _Item.ProxyPort);
        return _Result.GetString();
    }

    std::wstring ToString(const WINHTTP_PROXY_RESULT& _Item)
    {
        std::wstring _Result;
        for (DWORD i = 0; i != _Item.cEntries; ++i)
        {
            _Result += ToString(_Item.pEntries[i]);
            _Result += '\t';
        }

        return _Result;
    }

    template<size_t kCount>
    std::wstring ToString(const WINHTTP_PROXY_RESULT_ENTRY(& _Items)[kCount])
    {
        std::wstring _Result;
        for (auto& _Item : _Items)
        {
            _Result += ToString(_Item);
            _Result += '\t';
        }
        return _Result;
    }

    /// <summary>
    /// 微软的老的WinHttpGetProxyForUrl接口支持存在问题
    /// FTP、SOCKS都是无法支持的。
    /// 另外多条并且规则时无法于DIRECT同时存在。
    /// </summary>
    TEST_CLASS(WinHttpGetProxyForUrlEx)
    {
        AwaysNullGuard Guard;
        WINHTTP_AUTOPROXY_OPTIONS _AutoProxyOptions = {};

    public:
        WinHttpGetProxyForUrlEx()
        {
            Guard |= YY::Thunks::aways_null_try_get_WinHttpCreateProxyResolver;
            Guard |= YY::Thunks::aways_null_try_get_WinHttpGetProxyForUrlEx;
            Guard |= YY::Thunks::aways_null_try_get_WinHttpGetProxyResult;
            Guard |= YY::Thunks::aways_null_try_get_WinHttpFreeProxyResult;

            
            _AutoProxyOptions.lpszAutoConfigUrl = L"https://raw.githubusercontent.com/Chuyu-Team/YY-Thunks/master/src/YY-Thunks.UnitTest/proxy.pac";
            // _AutoProxyOptions.lpszAutoConfigUrl = L"http://192.168.1.17/proxy.pac";
            _AutoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL | WINHTTP_AUTOPROXY_NO_CACHE_CLIENT | WINHTTP_AUTOPROXY_NO_CACHE_SVC;
        }

        TEST_METHOD(DIRECT配置)
        {
            static const WINHTTP_PROXY_RESULT_ENTRY kTargetEntries[] =
            {
                {0, 1, 0, nullptr, 0 },
            };

            struct TestInfo
            {
                HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                WINHTTP_PROXY_RESULT ProxyResult = {};

                ~TestInfo()
                {
                    CloseHandle(hEvent);
                }
            };

            TestInfo _TestInfo;

            HINTERNET _hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
                WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                nullptr,
                nullptr, WINHTTP_FLAG_ASYNC);
            Assert::IsNotNull(_hSession);

            HINTERNET hResolver;
            auto _lStatus = ::WinHttpCreateProxyResolver(_hSession, &hResolver);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

            auto _pfnOldCallback = ::WinHttpSetStatusCallback(hResolver, [](
                IN HINTERNET hInternet,
                IN DWORD_PTR dwContext,
                IN DWORD dwInternetStatus,
                IN LPVOID lpvStatusInformation OPTIONAL,
                IN DWORD dwStatusInformationLength)
                {
                    auto& _TestInfo = *reinterpret_cast<TestInfo*>(dwContext);
                    if (dwInternetStatus == WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE)
                    {
                        ::WinHttpGetProxyResult(hInternet, &_TestInfo.ProxyResult);
                        WINHTTP_PROXY_RESULT ProxyResult;
                        ::WinHttpGetProxyResult(hInternet, &ProxyResult);

                        int u = 0;
                    }
                    SetEvent(_TestInfo.hEvent);


                }, WINHTTP_CALLBACK_FLAG_REQUEST_ERROR | WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE, 0);

            Assert::AreNotEqual((void*)_pfnOldCallback, (void*)WINHTTP_INVALID_STATUS_CALLBACK);

            _lStatus = ::WinHttpGetProxyForUrlEx(hResolver, L"http://www.test.com", &_AutoProxyOptions, (DWORD_PTR)&_TestInfo);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_IO_PENDING);

            WaitForSingleObject(_TestInfo.hEvent, INFINITE);

            WINHTTP_PROXY_RESULT ProxyResult;
            ::WinHttpGetProxyResult(hResolver, &ProxyResult);

            Assert::IsTrue(::WinHttpCloseHandle(hResolver));

            Assert::AreEqual(ToString(_TestInfo.ProxyResult), ToString(kTargetEntries));

            ::WinHttpFreeProxyResult(&_TestInfo.ProxyResult);
            Assert::IsTrue(::WinHttpCloseHandle(_hSession));
        }

        TEST_METHOD(单条HTTP配置)
        {
            static const WINHTTP_PROXY_RESULT_ENTRY kTargetEntries[] =
            {
                {1, 0, INTERNET_SCHEME_HTTP, (PWSTR)L"192.168.18.193", 1081 },
            };

            struct TestInfo
            {
                HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                WINHTTP_PROXY_RESULT ProxyResult = {};

                ~TestInfo()
                {
                    CloseHandle(hEvent);
                }
            };

            TestInfo _TestInfo;

            HINTERNET _hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
                WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                nullptr,
                nullptr, WINHTTP_FLAG_ASYNC);
            Assert::IsNotNull(_hSession);

            HINTERNET hResolver;
            auto _lStatus = ::WinHttpCreateProxyResolver(_hSession, &hResolver);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

            auto _pfnOldCallback = ::WinHttpSetStatusCallback(hResolver, [](
                IN HINTERNET hInternet,
                IN DWORD_PTR dwContext,
                IN DWORD dwInternetStatus,
                IN LPVOID lpvStatusInformation OPTIONAL,
                IN DWORD dwStatusInformationLength)
                {
                    auto& _TestInfo = *reinterpret_cast<TestInfo*>(dwContext);
                    if (dwInternetStatus == WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE)
                    {
                        ::WinHttpGetProxyResult(hInternet, &_TestInfo.ProxyResult);
                    }
                    SetEvent(_TestInfo.hEvent);


                }, WINHTTP_CALLBACK_FLAG_REQUEST_ERROR | WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE, 0);

            Assert::AreNotEqual((void*)_pfnOldCallback, (void*)WINHTTP_INVALID_STATUS_CALLBACK);

            _lStatus = ::WinHttpGetProxyForUrlEx(hResolver, L"http://www.test1.com", &_AutoProxyOptions, (DWORD_PTR)&_TestInfo);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_IO_PENDING);

            WaitForSingleObject(_TestInfo.hEvent, INFINITE);

            Assert::AreEqual(ToString(_TestInfo.ProxyResult), ToString(kTargetEntries));

            ::WinHttpFreeProxyResult(&_TestInfo.ProxyResult);
        }

        TEST_METHOD(二条HTTP配置)
        {
            static const WINHTTP_PROXY_RESULT_ENTRY kTargetEntries[] =
            {
                {1, 0, INTERNET_SCHEME_HTTP, (PWSTR)L"192.168.18.193", 80 },
                {1, 0, INTERNET_SCHEME_HTTP, (PWSTR)L"192.168.18.194", 100 },
            };

            struct TestInfo
            {
                HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                WINHTTP_PROXY_RESULT ProxyResult = {};

                ~TestInfo()
                {
                    CloseHandle(hEvent);
                }
            };

            TestInfo _TestInfo;

            HINTERNET _hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
                WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                nullptr,
                nullptr, WINHTTP_FLAG_ASYNC);
            Assert::IsNotNull(_hSession);

            HINTERNET hResolver;
            auto _lStatus = ::WinHttpCreateProxyResolver(_hSession, &hResolver);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

            auto _pfnOldCallback = ::WinHttpSetStatusCallback(hResolver, [](
                IN HINTERNET hInternet,
                IN DWORD_PTR dwContext,
                IN DWORD dwInternetStatus,
                IN LPVOID lpvStatusInformation OPTIONAL,
                IN DWORD dwStatusInformationLength)
                {
                    auto& _TestInfo = *reinterpret_cast<TestInfo*>(dwContext);
                    if (dwInternetStatus == WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE)
                    {
                        ::WinHttpGetProxyResult(hInternet, &_TestInfo.ProxyResult);
                    }
                    SetEvent(_TestInfo.hEvent);


                }, WINHTTP_CALLBACK_FLAG_REQUEST_ERROR | WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE, 0);

            Assert::AreNotEqual((void*)_pfnOldCallback, (void*)WINHTTP_INVALID_STATUS_CALLBACK);

            _lStatus = ::WinHttpGetProxyForUrlEx(hResolver, L"http://www.test3.com", &_AutoProxyOptions, (DWORD_PTR)&_TestInfo);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_IO_PENDING);

            WaitForSingleObject(_TestInfo.hEvent, INFINITE);

            Assert::AreEqual(ToString(_TestInfo.ProxyResult), ToString(kTargetEntries));

            ::WinHttpFreeProxyResult(&_TestInfo.ProxyResult);
        }

        TEST_METHOD(单条HTTPS配置)
        {
            static const WINHTTP_PROXY_RESULT_ENTRY kTargetEntries[] =
            {
                {1, 0, INTERNET_SCHEME_HTTPS, (PWSTR)L"192.168.18.193", 80 },
            };

            struct TestInfo
            {
                HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                WINHTTP_PROXY_RESULT ProxyResult = {};

                ~TestInfo()
                {
                    CloseHandle(hEvent);
                }
            };

            TestInfo _TestInfo;

            HINTERNET _hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
                WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                nullptr,
                nullptr, WINHTTP_FLAG_ASYNC);
            Assert::IsNotNull(_hSession);

            HINTERNET hResolver;
            auto _lStatus = ::WinHttpCreateProxyResolver(_hSession, &hResolver);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

            auto _pfnOldCallback = ::WinHttpSetStatusCallback(hResolver, [](
                IN HINTERNET hInternet,
                IN DWORD_PTR dwContext,
                IN DWORD dwInternetStatus,
                IN LPVOID lpvStatusInformation OPTIONAL,
                IN DWORD dwStatusInformationLength)
                {
                    auto& _TestInfo = *reinterpret_cast<TestInfo*>(dwContext);
                    if (dwInternetStatus == WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE)
                    {
                        ::WinHttpGetProxyResult(hInternet, &_TestInfo.ProxyResult);
                    }
                    SetEvent(_TestInfo.hEvent);


                }, WINHTTP_CALLBACK_FLAG_REQUEST_ERROR | WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE, 0);

            Assert::AreNotEqual((void*)_pfnOldCallback, (void*)WINHTTP_INVALID_STATUS_CALLBACK);


            _lStatus = ::WinHttpGetProxyForUrlEx(hResolver, L"http://www.test2.com", &_AutoProxyOptions, (DWORD_PTR)&_TestInfo);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_IO_PENDING);

            WaitForSingleObject(_TestInfo.hEvent, INFINITE);

            Assert::AreEqual(ToString(_TestInfo.ProxyResult), ToString(kTargetEntries));

            ::WinHttpFreeProxyResult(&_TestInfo.ProxyResult);
        }

        TEST_METHOD(三条混合配置)
        {
            static const WINHTTP_PROXY_RESULT_ENTRY kTargetEntries[] =
            {
                {1, 0, INTERNET_SCHEME_HTTPS, (PWSTR)L"192.168.18.193", 80 },
                {1, 0, INTERNET_SCHEME_HTTPS, (PWSTR)L"192.168.18.194", 102 },
                {1, 0, INTERNET_SCHEME_HTTP, (PWSTR)L"192.168.18.195", 103 },
            };

            struct TestInfo
            {
                HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                WINHTTP_PROXY_RESULT ProxyResult = {};

                ~TestInfo()
                {
                    CloseHandle(hEvent);
                }
            };

            TestInfo _TestInfo;

            HINTERNET _hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
                WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                nullptr,
                nullptr, WINHTTP_FLAG_ASYNC);
            Assert::IsNotNull(_hSession);

            HINTERNET hResolver;
            auto _lStatus = ::WinHttpCreateProxyResolver(_hSession, &hResolver);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

            auto _pfnOldCallback = ::WinHttpSetStatusCallback(hResolver, [](
                IN HINTERNET hInternet,
                IN DWORD_PTR dwContext,
                IN DWORD dwInternetStatus,
                IN LPVOID lpvStatusInformation OPTIONAL,
                IN DWORD dwStatusInformationLength)
                {
                    auto& _TestInfo = *reinterpret_cast<TestInfo*>(dwContext);
                    if (dwInternetStatus == WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE)
                    {
                        ::WinHttpGetProxyResult(hInternet, &_TestInfo.ProxyResult);
                    }
                    SetEvent(_TestInfo.hEvent);


                }, WINHTTP_CALLBACK_FLAG_REQUEST_ERROR | WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE, 0);

            Assert::AreNotEqual((void*)_pfnOldCallback, (void*)WINHTTP_INVALID_STATUS_CALLBACK);

            _lStatus = ::WinHttpGetProxyForUrlEx(hResolver, L"http://www.test4.com", &_AutoProxyOptions, (DWORD_PTR)&_TestInfo);
            Assert::AreEqual(_lStatus, (DWORD)ERROR_IO_PENDING);

            WaitForSingleObject(_TestInfo.hEvent, INFINITE);

            Assert::AreEqual(ToString(_TestInfo.ProxyResult), ToString(kTargetEntries));

            ::WinHttpFreeProxyResult(&_TestInfo.ProxyResult);
        }

    };
}
