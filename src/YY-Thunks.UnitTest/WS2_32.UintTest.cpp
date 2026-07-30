#include "pch.h"
#include "Thunks/WS2_32.hpp"

namespace WS2_32
{
    TEST_CLASS(GetAddrInfoExCancel)
    {
        AwaysNullGuard Guard;
        WSADATA wsaData;

    public:
        GetAddrInfoExCancel()
        {
            Guard |= YY::Thunks::aways_null_try_get_GetAddrInfoExCancel;

            WSAStartup(MAKEWORD(2, 2), &wsaData);
        }

        ~GetAddrInfoExCancel()
        {
            // WSACleanup();
        }

        struct GetAddrInfoEx_OVERLAPPED : public OVERLAPPED
        {
            DWORD dwError;
            DWORD dwBytes;
            LPWSAOVERLAPPED lpOverlapped;
        };

        TEST_METHOD(异步不取消)
        {
            {
                ADDRINFOEXW _Hints = {};
                _Hints.ai_family = AF_UNSPEC;
                GetAddrInfoEx_OVERLAPPED _QueryOverlapped = {};
                _QueryOverlapped.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
                PADDRINFOEXW _QueryResults;

                HANDLE _hHandle;
                DWORD _lStatus = GetAddrInfoExW(L"baidu.com",
                    NULL,
                    NS_DNS,
                    NULL,
                    &_Hints,
                    &_QueryResults,
                    NULL,
                    &_QueryOverlapped,
                    [](_In_      DWORD    dwError,
                        _In_      DWORD    dwBytes,
                        _In_      LPWSAOVERLAPPED lpOverlapped)
                    {
                        auto _lpOverlapped2 = (GetAddrInfoEx_OVERLAPPED*)lpOverlapped;
                        _lpOverlapped2->dwError = dwError;
                        _lpOverlapped2->dwBytes = dwBytes;
                        _lpOverlapped2->lpOverlapped = lpOverlapped;
                    },
                    &_hHandle);

                Assert::AreEqual(_lStatus, (DWORD)WSA_IO_PENDING);

                WaitForSingleObject(_QueryOverlapped.hEvent, INFINITE);
                Assert::IsNotNull(_QueryResults);
                Assert::AreEqual(_QueryOverlapped.dwError, 0ul);
                Assert::AreEqual(_QueryOverlapped.dwBytes, 0ul);
                Assert::AreEqual((uintptr_t)_QueryOverlapped.lpOverlapped, (uintptr_t)&_QueryOverlapped);
                Assert::AreEqual((uintptr_t)_QueryOverlapped.Pointer, (uintptr_t)&_QueryResults);
                FreeAddrInfoExW(_QueryResults);
                CloseHandle(_QueryOverlapped.hEvent);
            }

            {
                ADDRINFOEXA _Hints = {};
                _Hints.ai_family = AF_UNSPEC;
                GetAddrInfoEx_OVERLAPPED _QueryOverlapped = {};
                _QueryOverlapped.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
                PADDRINFOEXA _QueryResults;

                HANDLE _hHandle;
                DWORD _lStatus = GetAddrInfoExA("baidu.com",
                    NULL,
                    NS_DNS,
                    NULL,
                    &_Hints,
                    &_QueryResults,
                    NULL,
                    &_QueryOverlapped,
                    [](_In_      DWORD    dwError,
                        _In_      DWORD    dwBytes,
                        _In_      LPWSAOVERLAPPED lpOverlapped)
                    {
                        auto _lpOverlapped2 = (GetAddrInfoEx_OVERLAPPED*)lpOverlapped;
                        _lpOverlapped2->dwError = dwError;
                        _lpOverlapped2->dwBytes = dwBytes;
                        _lpOverlapped2->lpOverlapped = lpOverlapped;
                    },
                    &_hHandle);

                Assert::AreEqual(_lStatus, (DWORD)WSA_IO_PENDING);

                WaitForSingleObject(_QueryOverlapped.hEvent, INFINITE);
                Assert::IsNotNull(_QueryResults);
                Assert::AreEqual(_QueryOverlapped.dwError, 0ul);
                Assert::AreEqual(_QueryOverlapped.dwBytes, 0ul);
                Assert::AreEqual((uintptr_t)_QueryOverlapped.lpOverlapped, (uintptr_t)&_QueryOverlapped);
                Assert::AreEqual((uintptr_t)_QueryOverlapped.Pointer, (uintptr_t)&_QueryResults);
                FreeAddrInfoExA(_QueryResults);
                CloseHandle(_QueryOverlapped.hEvent);
            }
        }

        TEST_METHOD(任务取消测试)
        {
            {
                for (int i = 0; i != 5; ++i)
                {
                    ADDRINFOEXW _Hints = {};
                    _Hints.ai_family = AF_UNSPEC;
                    GetAddrInfoEx_OVERLAPPED _QueryOverlapped = {};
                    _QueryOverlapped.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
                    Assert::IsNotNull(_QueryOverlapped.hEvent);

                    PADDRINFOEXW _QueryResults;

                    HANDLE _hHandle;
                    DWORD _lStatus = GetAddrInfoExW(L"baidu.com",
                        NULL,
                        NS_DNS,
                        NULL,
                        &_Hints,
                        &_QueryResults,
                        NULL,
                        &_QueryOverlapped,
                        [](_In_      DWORD    dwError,
                            _In_      DWORD    dwBytes,
                            _In_      LPWSAOVERLAPPED lpOverlapped)
                        {
                            auto _lpOverlapped2 = (GetAddrInfoEx_OVERLAPPED*)lpOverlapped;
                            _lpOverlapped2->dwError = dwError;
                            _lpOverlapped2->dwBytes = dwBytes;
                            _lpOverlapped2->lpOverlapped = lpOverlapped;
                        },
                        &_hHandle);

                    Assert::AreEqual(_lStatus, (DWORD)WSA_IO_PENDING);
                    _lStatus = ::GetAddrInfoExCancel(&_hHandle);
                    Assert::IsTrue(_lStatus == ERROR_SUCCESS || _lStatus == WSA_INVALID_HANDLE);

                    if (_lStatus == WSA_INVALID_HANDLE)
                    {
                        WaitForSingleObject(_QueryOverlapped.hEvent, INFINITE);

                        Assert::IsNotNull(_QueryResults);
                        Assert::AreEqual(_QueryOverlapped.dwError, 0ul);
                        Assert::AreEqual(_QueryOverlapped.dwBytes, 0ul);
                        Assert::AreEqual((uintptr_t)_QueryOverlapped.lpOverlapped, (uintptr_t)&_QueryOverlapped);
                        Assert::AreEqual((uintptr_t)_QueryOverlapped.Pointer, (uintptr_t)&_QueryResults);
                        FreeAddrInfoExW(_QueryResults);
                    }
                    else
                    {
                        Assert::AreEqual(_QueryOverlapped.dwError, (DWORD)WSA_E_CANCELLED);
                    }
                    CloseHandle(_QueryOverlapped.hEvent);
                }
            }

            {
                for (int i = 0; i != 5; ++i)
                {
                    ADDRINFOEXA _Hints = {};
                    _Hints.ai_family = AF_UNSPEC;
                    GetAddrInfoEx_OVERLAPPED _QueryOverlapped = {};
                    _QueryOverlapped.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
                    Assert::IsNotNull(_QueryOverlapped.hEvent);

                    PADDRINFOEXA _QueryResults;

                    HANDLE _hHandle;
                    DWORD _lStatus = GetAddrInfoExA("baidu.com",
                        NULL,
                        NS_DNS,
                        NULL,
                        &_Hints,
                        &_QueryResults,
                        NULL,
                        &_QueryOverlapped,
                        [](_In_      DWORD    dwError,
                            _In_      DWORD    dwBytes,
                            _In_      LPWSAOVERLAPPED lpOverlapped)
                        {
                            auto _lpOverlapped2 = (GetAddrInfoEx_OVERLAPPED*)lpOverlapped;
                            _lpOverlapped2->dwError = dwError;
                            _lpOverlapped2->dwBytes = dwBytes;
                            _lpOverlapped2->lpOverlapped = lpOverlapped;
                        },
                        &_hHandle);

                    Assert::AreEqual(_lStatus, (DWORD)WSA_IO_PENDING);
                    _lStatus = ::GetAddrInfoExCancel(&_hHandle);
                    Assert::IsTrue(_lStatus == ERROR_SUCCESS || _lStatus == WSA_INVALID_HANDLE);

                    if (_lStatus == WSA_INVALID_HANDLE)
                    {
                        WaitForSingleObject(_QueryOverlapped.hEvent, INFINITE);

                        Assert::IsNotNull(_QueryResults);
                        Assert::AreEqual(_QueryOverlapped.dwError, 0ul);
                        Assert::AreEqual(_QueryOverlapped.dwBytes, 0ul);
                        Assert::AreEqual((uintptr_t)_QueryOverlapped.lpOverlapped, (uintptr_t)&_QueryOverlapped);
                        Assert::AreEqual((uintptr_t)_QueryOverlapped.Pointer, (uintptr_t)&_QueryResults);
                        FreeAddrInfoExA(_QueryResults);
                    }
                    else
                    {
                        Assert::AreEqual(_QueryOverlapped.dwError, (DWORD)WSA_E_CANCELLED);
                    }
                    CloseHandle(_QueryOverlapped.hEvent);
                }
            }
        }
    };

#undef GetAddrInfoEx

    TEST_CLASS(GetAddrInfoEx)
    {
        AwaysNullGuard Guard;
        WSADATA wsaData;

    public:
        GetAddrInfoEx()
        {
            Guard |= YY::Thunks::aways_null_try_get_GetAddrInfoExW;
            Guard |= YY::Thunks::aways_null_try_get_GetAddrInfoExA;
            Guard |= YY::Thunks::aways_null_try_get_FreeAddrInfoExW;
            Guard |= YY::Thunks::aways_null_try_get_FreeAddrInfoEx;

            WSAStartup(MAKEWORD(2, 2), &wsaData);
        }

        ~GetAddrInfoEx()
        {
            // WSACleanup();
        }

        TEST_METHOD(Win2003_XPSP3模式)
        {
            {
                ADDRINFOEXW _Hints = {};
                _Hints.ai_family = AF_UNSPEC;
                PADDRINFOEXW _QueryResults;

                DWORD _lStatus = ::GetAddrInfoExW(L"baidu.com",
                    NULL,
                    NS_DNS,
                    NULL,
                    &_Hints,
                    &_QueryResults,
                    NULL,
                    nullptr,
                    nullptr,
                    nullptr);

                Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

                Assert::IsNotNull(_QueryResults);
                FreeAddrInfoExW(_QueryResults);
            }

            {
                ADDRINFOEXA _Hints = {};
                _Hints.ai_family = AF_UNSPEC;
                PADDRINFOEXA _QueryResults;

                DWORD _lStatus = ::GetAddrInfoExA("baidu.com",
                    NULL,
                    NS_DNS,
                    NULL,
                    &_Hints,
                    &_QueryResults,
                    NULL,
                    nullptr,
                    nullptr,
                    nullptr);

                Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

                Assert::IsNotNull(_QueryResults);
                FreeAddrInfoExA(_QueryResults);
            }
        }

        __if_exists(YY::Thunks::aways_null_try_get_GetAddrInfoW)
        {
            TEST_METHOD(WinXP_RTM模式)
            {
                AwaysNullGuard Guard;
                Guard |= YY::Thunks::aways_null_try_get_GetAddrInfoW;

                {
                    ADDRINFOEXW _Hints = {};
                    _Hints.ai_family = AF_UNSPEC;
                    PADDRINFOEXW _QueryResults;

                    DWORD _lStatus = ::GetAddrInfoExW(L"baidu.com",
                        NULL,
                        NS_DNS,
                        NULL,
                        &_Hints,
                        &_QueryResults,
                        NULL,
                        nullptr,
                        nullptr,
                        nullptr);

                    Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

                    Assert::IsNotNull(_QueryResults);
                    FreeAddrInfoExW(_QueryResults);
                }

                {
                    ADDRINFOEXA _Hints = {};
                    _Hints.ai_family = AF_UNSPEC;
                    PADDRINFOEXA _QueryResults;

                    DWORD _lStatus = ::GetAddrInfoExA("baidu.com",
                        NULL,
                        NS_DNS,
                        NULL,
                        &_Hints,
                        &_QueryResults,
                        NULL,
                        nullptr,
                        nullptr,
                        nullptr);

                    Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);

                    Assert::IsNotNull(_QueryResults);
                    FreeAddrInfoExA(_QueryResults);
                }
            }
        }
    };

    TEST_CLASS(GetHostNameW)
    {
        AwaysNullGuard Guard;

    public:
        GetHostNameW()
        {
            Guard |= YY::Thunks::aways_null_try_get_GetHostNameW;
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
        }

        TEST_METHOD(常规测试)
        {
            wchar_t _szHostNameW[256] = {};
            Assert::AreEqual(::GetHostNameW(_szHostNameW, _countof(_szHostNameW)), (int)0);
            Assert::AreNotEqual(_szHostNameW, L"");
        }

        TEST_METHOD(错误参数验证)
        {
            wchar_t _szHostNameW[256] = {};

            // null缓冲区
            WSASetLastError(0);
            Assert::AreEqual(::GetHostNameW(nullptr, _countof(_szHostNameW)), (int)SOCKET_ERROR);
            Assert::AreEqual(WSAGetLastError(), (int)WSAEFAULT);

            // 缓冲区不足
            WSASetLastError(0);
            Assert::AreEqual(::GetHostNameW(_szHostNameW, 1), (int)SOCKET_ERROR);
            Assert::AreEqual(WSAGetLastError(), (int)WSAEFAULT);

            // 缓冲区边界，比够用小一点或者大一点
            {
                Assert::AreEqual(::GetHostNameW(_szHostNameW, _countof(_szHostNameW)), (int)0);
                auto _cchHaostNameW = wcslen(_szHostNameW);

                wchar_t _szHostNameW2[256];
                memset(_szHostNameW2, 0xCC, sizeof(_szHostNameW2));
                Assert::AreEqual(::GetHostNameW(_szHostNameW2, _cchHaostNameW), (int)SOCKET_ERROR);
                Assert::AreEqual(WSAGetLastError(), (int)WSAEFAULT);


                memset(_szHostNameW2, 0xCC, sizeof(_szHostNameW2));
                Assert::AreEqual(::GetHostNameW(_szHostNameW2, _cchHaostNameW + 1), (int)0);
                Assert::AreEqual(_szHostNameW, _szHostNameW2);
            }
        }
    };


    TEST_CLASS(GetNameInfoW)
    {
        AwaysNullGuard Guard;
        WSADATA wsaData;

    public:
        GetNameInfoW()
        {
            __if_exists(YY::Thunks::aways_null_try_get_GetNameInfoW)
            {
                Guard |= YY::Thunks::aways_null_try_get_GetNameInfoW;
            }
            WSAStartup(MAKEWORD(2, 2), &wsaData);
        }

        ~GetNameInfoW()
        {
            // WSACleanup();
        }

        TEST_METHOD(常规测试)
        {
            // 准备一个 IPv4 地址结构 (127.0.0.1)
            SOCKADDR_IN _Addr = {};
            _Addr.sin_family = AF_INET;
            _Addr.sin_port = htons(80);
            _Addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            wchar_t _szNodeBuffer[NI_MAXHOST] = {};
            wchar_t _szServiceBuffer[NI_MAXSERV] = {};

            auto _iResult = ::GetNameInfoW(
                reinterpret_cast<SOCKADDR*>(&_Addr),
                sizeof(_Addr),
                _szNodeBuffer,
                _countof(_szNodeBuffer),
                _szServiceBuffer,
                _countof(_szServiceBuffer),
                0);

            Assert::AreEqual(_iResult, 0);
            // 节点名应该非空 (可能是 localhost 或 127.0.0.1)
            Assert::AreNotEqual(wcslen(_szNodeBuffer), (size_t)0);
            // 服务名应该是 "http" 或 "80"
            Assert::AreNotEqual(wcslen(_szServiceBuffer), (size_t)0);
        }

        TEST_METHOD(仅获取节点名)
        {
            SOCKADDR_IN _Addr = {};
            _Addr.sin_family = AF_INET;
            _Addr.sin_port = htons(443);
            _Addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            wchar_t _szNodeBuffer[NI_MAXHOST] = {};

            auto _iResult = ::GetNameInfoW(
                reinterpret_cast<SOCKADDR*>(&_Addr),
                sizeof(_Addr),
                _szNodeBuffer,
                _countof(_szNodeBuffer),
                nullptr,    // 不需要服务名
                0,
                0);

            Assert::AreEqual(_iResult, 0);
            Assert::AreNotEqual(wcslen(_szNodeBuffer), (size_t)0);
        }

        TEST_METHOD(仅获取服务名)
        {
            SOCKADDR_IN _Addr = {};
            _Addr.sin_family = AF_INET;
            _Addr.sin_port = htons(80);
            _Addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            wchar_t _szServiceBuffer[NI_MAXSERV] = {};

            auto _iResult = ::GetNameInfoW(
                reinterpret_cast<SOCKADDR*>(&_Addr),
                sizeof(_Addr),
                nullptr,    // 不需要节点名
                0,
                _szServiceBuffer,
                _countof(_szServiceBuffer),
                0);

            Assert::AreEqual(_iResult, 0);
            Assert::AreNotEqual(wcslen(_szServiceBuffer), (size_t)0);
        }

        TEST_METHOD(空指针缓冲区)
        {
            SOCKADDR_IN _Addr = {};
            _Addr.sin_family = AF_INET;
            _Addr.sin_port = htons(80);
            _Addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            // 两个缓冲区都是 nullptr，应该成功但不返回任何内容
            auto _iResult = ::GetNameInfoW(
                reinterpret_cast<SOCKADDR*>(&_Addr),
                sizeof(_Addr),
                nullptr,
                0,
                nullptr,
                0,
                0);

            Assert::AreEqual(_iResult, 0);
        }

        TEST_METHOD(缓冲区不足)
        {
            SOCKADDR_IN _Addr = {};
            _Addr.sin_family = AF_INET;
            _Addr.sin_port = htons(80);
            _Addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            // 给一个非常小的缓冲区
            wchar_t _szNodeBuffer[2] = {};

            auto _iResult = ::GetNameInfoW(
                reinterpret_cast<SOCKADDR*>(&_Addr),
                sizeof(_Addr),
                _szNodeBuffer,
                _countof(_szNodeBuffer),
                nullptr,
                0,
                0);

            // 缓冲区不足应该返回错误码 (EAI_OVERFLOW)
            Assert::AreNotEqual(_iResult, 0);
        }

        TEST_METHOD(NI_NUMERICHOST标志)
        {
            SOCKADDR_IN _Addr = {};
            _Addr.sin_family = AF_INET;
            _Addr.sin_port = htons(80);
            _Addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            wchar_t _szNodeBuffer[NI_MAXHOST] = {};

            // 使用 NI_NUMERICHOST 直接返回 IP 地址字符串
            auto _iResult = ::GetNameInfoW(
                reinterpret_cast<SOCKADDR*>(&_Addr),
                sizeof(_Addr),
                _szNodeBuffer,
                _countof(_szNodeBuffer),
                nullptr,
                0,
                NI_NUMERICHOST);

            Assert::AreEqual(_iResult, 0);
            // 应该包含 "127.0.0.1"
            Assert::AreEqual(wcscmp(_szNodeBuffer, L"127.0.0.1"), 0);
        }

        TEST_METHOD(NI_NUMERICSERV标志)
        {
            SOCKADDR_IN _Addr = {};
            _Addr.sin_family = AF_INET;
            _Addr.sin_port = htons(80);
            _Addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            wchar_t _szServiceBuffer[NI_MAXSERV] = {};

            // 使用 NI_NUMERICSERV 直接返回端口号字符串
            auto _iResult = ::GetNameInfoW(
                reinterpret_cast<SOCKADDR*>(&_Addr),
                sizeof(_Addr),
                nullptr,
                0,
                _szServiceBuffer,
                _countof(_szServiceBuffer),
                NI_NUMERICSERV);

            Assert::AreEqual(_iResult, 0);
            // 应该包含 "80"
            Assert::AreEqual(wcscmp(_szServiceBuffer, L"80"), 0);
        }

        __if_exists(YY::Thunks::aways_null_try_get_GetNameInfoW)
        {
            TEST_METHOD(WinXP_RTM模式)
            {
                AwaysNullGuard Guard;
                Guard |= YY::Thunks::aways_null_try_get_GetNameInfoW;

                SOCKADDR_IN _Addr = {};
                _Addr.sin_family = AF_INET;
                _Addr.sin_port = htons(80);
                _Addr.sin_addr.s_addr = inet_addr("127.0.0.1");

                wchar_t _szNodeBuffer[NI_MAXHOST] = {};
                wchar_t _szServiceBuffer[NI_MAXSERV] = {};

                auto _iResult = ::GetNameInfoW(
                    reinterpret_cast<SOCKADDR*>(&_Addr),
                    sizeof(_Addr),
                    _szNodeBuffer,
                    _countof(_szNodeBuffer),
                    _szServiceBuffer,
                    _countof(_szServiceBuffer),
                    0);

                Assert::AreEqual(_iResult, 0);
                Assert::AreNotEqual(wcslen(_szNodeBuffer), (size_t)0);
                Assert::AreNotEqual(wcslen(_szServiceBuffer), (size_t)0);
            }
        }
    };
}
