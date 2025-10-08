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
}
