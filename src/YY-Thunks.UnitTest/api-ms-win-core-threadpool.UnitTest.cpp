﻿#include "pch.h"
#include "Thunks/api-ms-win-core-threadpool.hpp"
#include <process.h>

namespace api_ms_win_core_threadpool
{
    TEST_CLASS(SubmitThreadpoolWork)
    {
        AwaysNullGuard Guard;

    public:
        SubmitThreadpoolWork()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_SubmitThreadpoolWork;

            Guard |= YY::Thunks::aways_null_try_get_SetEventWhenCallbackReturns;
            Guard |= YY::Thunks::aways_null_try_get_ReleaseSemaphoreWhenCallbackReturns;
        }

        TEST_METHOD(一般行为验证)
        {
            long RunCount = 0;


            auto Work = ::CreateThreadpoolWork([](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_WORK              Work)
                {
                    Assert::IsNotNull(Context);

                    InterlockedIncrement((long*)Context);

                }, &RunCount, nullptr);

            Assert::IsNotNull(Work);

            ::SubmitThreadpoolWork(Work);
            Sleep(500);
            Assert::AreEqual(RunCount, 1l);

            ::SubmitThreadpoolWork(Work);
            Sleep(500);
            Assert::AreEqual(RunCount, 2l);
            
            ::SubmitThreadpoolWork(Work);
            ::SubmitThreadpoolWork(Work);
            Sleep(500);
            Assert::AreEqual(RunCount, 4l);
            
            Sleep(500);
            Assert::AreEqual(RunCount, 4l);

            CloseThreadpoolWork(Work);
        }
    };


    TEST_CLASS(TrySubmitThreadpoolCallback)
    {
        AwaysNullGuard Guard;

    public:
        TrySubmitThreadpoolCallback()
        {
            Guard |= YY::Thunks::aways_null_try_get_TrySubmitThreadpoolCallback;
        }

        TEST_METHOD(一般行为验证)
        {
            long RunCount = 0;

            auto bRet = ::TrySubmitThreadpoolCallback([](
                    _Inout_     PTP_CALLBACK_INSTANCE Instance,
                    _Inout_opt_ PVOID                 Context
                    )
                {
                    Assert::IsNotNull(Context);
                    InterlockedIncrement((long*)Context);
                },
                &RunCount,
                nullptr);

            Assert::IsTrue(bRet);

            Sleep(1000);

            //正常来说 1秒钟后函数必然执行成功
            Assert::AreEqual(RunCount, 1l);

            bRet = ::TrySubmitThreadpoolCallback([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context
                )
                {
                    Assert::IsNotNull(Context);
                    InterlockedIncrement((long*)Context);
                },
                &RunCount,
                    nullptr);

            Assert::IsTrue(bRet);

            Sleep(1000);

            //正常来说 1秒钟后函数必然执行成功
            Assert::AreEqual(RunCount, 2l);
        }
    };

    TEST_CLASS(WaitForThreadpoolWorkCallbacks)
    {
        AwaysNullGuard Guard;

    public:
        WaitForThreadpoolWorkCallbacks()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_SubmitThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_WaitForThreadpoolWorkCallbacks;
        }

        TEST_METHOD(一般行为验证)
        {
            long RunCount = 0;

            auto Work = ::CreateThreadpoolWork([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_WORK              Work)
                {
                    Assert::IsNotNull(Context);

                    Sleep(1 * 1000);

                    InterlockedIncrement((long*)Context);

                }, & RunCount, nullptr);

            Assert::IsNotNull(Work);

            ::SubmitThreadpoolWork(Work);

            auto h = (HANDLE)_beginthreadex(nullptr, 0, [](void* Work) -> unsigned
                {
                    ::WaitForThreadpoolWorkCallbacks((PTP_WORK)Work, FALSE);

                    return 0;
                }, Work, 0, nullptr);

            Assert::IsNotNull(h);

            Assert::AreEqual(WaitForSingleObject(h, 2 * 1000), (DWORD)WAIT_OBJECT_0, L"2秒内必须完成，这是预期。");

            CloseHandle(h);

            Assert::AreEqual(RunCount, 1l);

            ::SubmitThreadpoolWork(Work);
            ::SubmitThreadpoolWork(Work);
            ::SubmitThreadpoolWork(Work);

            h = (HANDLE)_beginthreadex(nullptr, 0, [](void* Work) -> unsigned
                {
                    ::WaitForThreadpoolWorkCallbacks((PTP_WORK)Work, FALSE);

                    return 0;
                }, Work, 0, nullptr);

            Assert::IsNotNull(h);

            Assert::AreEqual(WaitForSingleObject(h, 6 * 1000), (DWORD)WAIT_OBJECT_0, L"6秒内必须完成，这是预期。");

            CloseHandle(h);

            Assert::AreEqual(RunCount, 4l);


            CloseThreadpoolWork(Work);
        }

    };


    TEST_CLASS(SetThreadpoolTimer)
    {
        AwaysNullGuard Guard;

    public:
        SetThreadpoolTimer()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_SetThreadpoolTimer;
        }

        TEST_METHOD(当DueTime数值为0时)
        {
            //数值为 0 时则立即触发定时器
            long RunCount = 0;


            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);

                    InterlockedIncrement((long*)Context);

                }, &RunCount, nullptr);

            Assert::IsNotNull(pTimer);

            FILETIME ftDueTime = {};
            ::SetThreadpoolTimer(pTimer, &ftDueTime, 800, 0);
            Sleep(100);
            Assert::AreEqual(RunCount, 1l);

            Sleep(900);

            Assert::AreEqual(RunCount, 2l);

            ::CloseThreadpoolTimer(pTimer);
        }

        TEST_METHOD(当DueTime是绝对时间)
        {
            long RunCount = 0;


            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);

                    InterlockedIncrement((long*)Context);

                }, &RunCount, nullptr);

            Assert::IsNotNull(pTimer);

            CFileTime ftDueTime = {};
            GetSystemTimeAsFileTime(&ftDueTime);

            //2秒后触发此任务
            ftDueTime += CFileTime::Second * 2;
            
            ::SetThreadpoolTimer(pTimer, &ftDueTime, 400, 0);
            Sleep(100);
            Assert::AreEqual(RunCount, 0l);

            Sleep(2 * 1000);

            Assert::AreEqual(RunCount, 1l);

            Sleep(900);
            Assert::AreEqual(RunCount, 3l);

            ::CloseThreadpoolTimer(pTimer);
        }


        TEST_METHOD(当DueTime是相对时间)
        {
            long RunCount = 0;


            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);

                    InterlockedIncrement((long*)Context);

                }, &RunCount, nullptr);

            Assert::IsNotNull(pTimer);

            //2秒后执行此任务
            CFileTime ftDueTime = CFileTime::Second * 2 * -1;

            ::SetThreadpoolTimer(pTimer, &ftDueTime, 400, 0);
            Sleep(100);
            Assert::AreEqual(RunCount, 0l);

            Sleep(2 * 1000);

            Assert::AreEqual(RunCount, 1l);

            Sleep(900);
            Assert::AreEqual(RunCount, 3l);

            ::CloseThreadpoolTimer(pTimer);
        }

        TEST_METHOD(当DueTime是nullptr)
        {
            long RunCount = 0;


            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);

                    InterlockedIncrement((long*)Context);

                }, &RunCount, nullptr);

            Assert::IsNotNull(pTimer);
            CFileTime ftDueTime{};

            ::SetThreadpoolTimer(pTimer, &ftDueTime, 400, 0);

            Sleep(900);
            Assert::AreEqual(RunCount, 3l);

            ::SetThreadpoolTimer(pTimer, nullptr, 0, 0);

            Sleep(900);
            Assert::AreEqual(RunCount, 3l);

            ::CloseThreadpoolTimer(pTimer);
        }

        TEST_METHOD(一次性定时器)
        {
            long RunCount = 0;


            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);

                    InterlockedIncrement((long*)Context);

                }, &RunCount, nullptr);

            Assert::IsNotNull(pTimer);
            CFileTime ftDueTime{};

            ::SetThreadpoolTimer(pTimer, &ftDueTime, 0, 0);

            Sleep(500);
            Assert::AreEqual(RunCount, 1l);

            Sleep(500);
            Assert::AreEqual(RunCount, 1l);

            Sleep(500);
            Assert::AreEqual(RunCount, 1l);

            ::CloseThreadpoolTimer(pTimer);
        }

        TEST_METHOD(定时器覆盖)
        {
            long RunCount = 0;

            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);

                    InterlockedIncrement((long*)Context);

                }, &RunCount, nullptr);

            Assert::IsNotNull(pTimer);
            CFileTime ftDueTime{};

            ::SetThreadpoolTimer(pTimer, &ftDueTime, 0, 0);

            Sleep(100);
            Assert::AreEqual(RunCount, 1l);

            Sleep(100);
            Assert::AreEqual(RunCount, 1l);

            ::SetThreadpoolTimer(pTimer, &ftDueTime, 0, 0);
            Sleep(100);
            Assert::AreEqual(RunCount, 2l);

            Sleep(100);
            Assert::AreEqual(RunCount, 2l);

            ::CloseThreadpoolTimer(pTimer);
        }

    };


    TEST_CLASS(WaitForThreadpoolTimerCallbacks)
    {
        AwaysNullGuard Guard;

    public:
        WaitForThreadpoolTimerCallbacks()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_SetThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_WaitForThreadpoolTimerCallbacks;
        }

        
        TEST_METHOD(一般行为验证)
        {
            //数值为 0 时则立即触发定时器
            struct ContextInfo
            {
                HANDLE hEvent;
                long RunCount;
            };

            ContextInfo Data{ CreateEventW(nullptr, TRUE, FALSE, nullptr) };

            Assert::IsNotNull(Data.hEvent);

            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);

                    auto& Data = *(ContextInfo*)Context;
                    SetEvent(Data.hEvent);
                    Sleep(800);
                    

                    InterlockedIncrement(&Data.RunCount);

                }, &Data, nullptr);

            Assert::IsNotNull(pTimer);

            CFileTime ftDueTime = {};
        
            //ftDueTime = long long(CFileTime::Second) * 5ll * -1ll;

            ::SetThreadpoolTimer(pTimer, &ftDueTime, 1'500, 0);

            Assert::AreEqual(WaitForSingleObject(Data.hEvent, 1 * 1000), (DWORD)WAIT_OBJECT_0, L"1秒内必须完成，这是预期。");

            auto h = (HANDLE)_beginthreadex(nullptr, 0, [](void* Work) -> unsigned
                {
                    ::WaitForThreadpoolTimerCallbacks((PTP_TIMER)Work, FALSE);

                    return 0;
                }, pTimer, 0, nullptr);

            Assert::IsNotNull(h);

            Assert::AreEqual(WaitForSingleObject(h, 30 * 1000), (DWORD)WAIT_OBJECT_0, L"6秒内必须完成，这是预期。");
            CloseHandle(h);

            Assert::AreEqual(Data.RunCount, 1l);


            ::CloseThreadpoolTimer(pTimer);
        }
    };


    TEST_CLASS(SetEventWhenCallbackReturns)
    {
        AwaysNullGuard Guard;

    public:
        SetEventWhenCallbackReturns()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_SubmitThreadpoolWork;

            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_SetThreadpoolTimer;

            Guard |= YY::Thunks::aways_null_try_get_SetEventWhenCallbackReturns;
        }

        struct UserData
        {
            HANDLE hHandle;
            long RunCount;

            UserData()
                : hHandle(CreateEventW(nullptr, FALSE, FALSE, nullptr))
                , RunCount(0)
            {
                Assert::IsNotNull(hHandle);
            }

            ~UserData()
            {
                if (hHandle)
                    CloseHandle(hHandle);
            }
        };
        TEST_METHOD(任务测试)
        {
            UserData Data;

            auto Work = ::CreateThreadpoolWork([](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_WORK              Work)
                {
                    Assert::IsNotNull(Context);
                    auto& Data = *(UserData*)Context;
                    ::SetEventWhenCallbackReturns(Instance, Data.hHandle);
                    InterlockedIncrement(&Data.RunCount);
                }, &Data, nullptr);

            Assert::IsNotNull(Work);
            Assert::AreEqual(WaitForSingleObject(Data.hHandle, 0), (DWORD)WAIT_TIMEOUT);

            ::SubmitThreadpoolWork(Work);

            Assert::AreEqual(WaitForSingleObject(Data.hHandle, 5 * 1000), (DWORD)WAIT_OBJECT_0, L"5秒内句柄必须有信号");
            Assert::AreEqual(Data.RunCount, 1l);

            CloseThreadpoolWork(Work);
        }

        TEST_METHOD(定时器测试)
        {
            UserData Data;

            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);
                    auto& Data = *(UserData*)Context;
                    ::SetEventWhenCallbackReturns(Instance, Data.hHandle);
                    InterlockedIncrement(&Data.RunCount);
                }, &Data, nullptr);

            Assert::IsNotNull(pTimer);

            Assert::AreEqual(WaitForSingleObject(Data.hHandle, 0), (DWORD)WAIT_TIMEOUT);

            FILETIME ftDueTime = {};
            ::SetThreadpoolTimer(pTimer, &ftDueTime, 0, 0);

            Assert::AreEqual(WaitForSingleObject(Data.hHandle, 5 * 1000), (DWORD)WAIT_OBJECT_0, L"5秒内句柄必须有信号");
            Assert::AreEqual(Data.RunCount, 1l);

            ::CloseThreadpoolTimer(pTimer);
        }
    };

    TEST_CLASS(ReleaseSemaphoreWhenCallbackReturns)
    {
        AwaysNullGuard Guard;

    public:
        ReleaseSemaphoreWhenCallbackReturns()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_SubmitThreadpoolWork;

            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_SetThreadpoolTimer;

            Guard |= YY::Thunks::aways_null_try_get_ReleaseSemaphoreWhenCallbackReturns;
        }

        struct UserData
        {
            HANDLE hHandle;
            long RunCount;

            UserData()
                : hHandle(CreateSemaphoreW(nullptr, 0, 1, nullptr))
                , RunCount(0)
            {
                Assert::IsNotNull(hHandle);
            }

            ~UserData()
            {
                if(hHandle)
                    CloseHandle(hHandle);
            }
        };


        TEST_METHOD(任务测试)
        {
            UserData Data;

            auto Work = ::CreateThreadpoolWork([](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_WORK              Work)
                {
                    Assert::IsNotNull(Context);
                    auto& Data = *(UserData*)Context;
                    ::ReleaseSemaphoreWhenCallbackReturns(Instance, Data.hHandle, 1);
                    InterlockedIncrement(&Data.RunCount);
                }, &Data, nullptr);

            Assert::IsNotNull(Work);
            Assert::AreEqual(WaitForSingleObject(Data.hHandle, 0), (DWORD)WAIT_TIMEOUT);

            ::SubmitThreadpoolWork(Work);

            Assert::AreEqual(WaitForSingleObject(Data.hHandle, 5 * 1000), (DWORD)WAIT_OBJECT_0, L"5秒内句柄必须有信号");
            Assert::AreEqual(Data.RunCount, 1l);

            CloseThreadpoolWork(Work);

        }

        TEST_METHOD(定时器测试)
        {
            UserData Data;

            Assert::IsNotNull(Data.hHandle);

            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);
                    auto& Data = *(UserData*)Context;
                    ::ReleaseSemaphoreWhenCallbackReturns(Instance, Data.hHandle, 1);
                    InterlockedIncrement(&Data.RunCount);

                }, &Data, nullptr);

            Assert::IsNotNull(pTimer);

            Assert::AreEqual(WaitForSingleObject(Data.hHandle, 0), (DWORD)WAIT_TIMEOUT);

            FILETIME ftDueTime = {};
            ::SetThreadpoolTimer(pTimer, &ftDueTime, 0, 0);

            Assert::AreEqual(WaitForSingleObject(Data.hHandle, 5 * 1000), (DWORD)WAIT_OBJECT_0, L"5秒内句柄必须有信号");
            Assert::AreEqual(Data.RunCount, 1l);

            ::CloseThreadpoolTimer(pTimer);
        }
    };


    TEST_CLASS(ReleaseMutexWhenCallbackReturns)
    {
        AwaysNullGuard Guard;

    public:
        ReleaseMutexWhenCallbackReturns()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_SubmitThreadpoolWork;

            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_SetThreadpoolTimer;

            Guard |= YY::Thunks::aways_null_try_get_ReleaseMutexWhenCallbackReturns;
        }

        struct UserData
        {
            HANDLE hHandle;
            long RunCount;

            UserData()
                : hHandle(CreateMutexW(nullptr, FALSE, nullptr))
                , RunCount(0)
            {
                Assert::IsNotNull(hHandle);
            }

            ~UserData()
            {
                if (hHandle)
                    CloseHandle(hHandle);
            }
        };

        TEST_METHOD(任务测试)
        {
            UserData Data;

            auto Work = ::CreateThreadpoolWork([](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_WORK              Work)
                {
                    Assert::IsNotNull(Context);
                    auto& Data = *(UserData*)Context;
                    auto dwWaitResult = WaitForSingleObject(Data.hHandle, 1000);
                    Assert::AreEqual(dwWaitResult, (DWORD)WAIT_OBJECT_0);
                    ::ReleaseMutexWhenCallbackReturns(Instance, Data.hHandle);
                    InterlockedIncrement(&Data.RunCount);

                }, & Data, nullptr);

            Assert::IsNotNull(Work);

            ::SubmitThreadpoolWork(Work);
            Sleep(100);

            Assert::AreEqual(WaitForSingleObject(Data.hHandle, 5 * 1000), (DWORD)WAIT_OBJECT_0, L"5秒内句柄必须有信号");
            Assert::AreEqual(Data.RunCount, 1l);

            CloseThreadpoolWork(Work);

        }

        TEST_METHOD(定时器测试)
        {
            UserData Data;            

            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);
                    auto& Data = *(UserData*)Context;
                    auto dwWaitResult = WaitForSingleObject(Data.hHandle, 1000);
                    Assert::AreEqual(dwWaitResult, (DWORD)WAIT_OBJECT_0);
                    ::ReleaseMutexWhenCallbackReturns(Instance, Data.hHandle);
                    InterlockedIncrement(&Data.RunCount);
                }, &Data, nullptr);

            Assert::IsNotNull(pTimer);

            FILETIME ftDueTime = {};
            ::SetThreadpoolTimer(pTimer, &ftDueTime, 0, 0);
            Sleep(100);

            Assert::AreEqual(WaitForSingleObject(Data.hHandle, 5 * 1000), (DWORD)WAIT_OBJECT_0, L"5秒内句柄必须有信号");
            Assert::AreEqual(Data.RunCount, 1l);

            ::CloseThreadpoolTimer(pTimer);
        }
    };


    TEST_CLASS(LeaveCriticalSectionWhenCallbackReturns)
    {
        AwaysNullGuard Guard;

    public:
        LeaveCriticalSectionWhenCallbackReturns()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_SubmitThreadpoolWork;

            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_SetThreadpoolTimer;

            Guard |= YY::Thunks::aways_null_try_get_LeaveCriticalSectionWhenCallbackReturns;
        }

        struct UserData
        {
            BOOL bInit;
            CRITICAL_SECTION cs;
            long RunCount;

            UserData()
                : bInit(false)
                , RunCount(0)
            {
                bInit = InitializeCriticalSectionAndSpinCount(&cs, 4000);

                Assert::IsTrue(bInit);
            }

            ~UserData()
            {
                if (bInit)
                {
                    DeleteCriticalSection(&cs);
                    bInit = FALSE;
                }
            }
        };

        TEST_METHOD(任务测试)
        {
            UserData Data;

            auto Work = ::CreateThreadpoolWork([](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_WORK              Work)
                {
                    Assert::IsNotNull(Context);
                    auto& Data = *(UserData*)Context;

                    EnterCriticalSection(&Data.cs);
                    ::LeaveCriticalSectionWhenCallbackReturns(Instance, &Data.cs);
                    InterlockedIncrement(&Data.RunCount);

                }, &Data, nullptr);

            Assert::IsNotNull(Work);

            ::SubmitThreadpoolWork(Work);
            Sleep(100);

            int i = 0;
            for (; i != 100; ++i)
            {
                if (TryEnterCriticalSection(&Data.cs))
                {
                    break;
                }

                Sleep(50);
            }

            Assert::AreNotEqual(i, 100, L"100次尝试后必须成功");
            Assert::AreEqual(Data.RunCount, 1l);

            LeaveCriticalSection(&Data.cs);

            CloseThreadpoolWork(Work);

        }

        TEST_METHOD(定时器测试)
        {
            UserData Data;


            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);
                    auto& Data = *(UserData*)Context;

                    EnterCriticalSection(&Data.cs);
                    ::LeaveCriticalSectionWhenCallbackReturns(Instance, &Data.cs);
                    InterlockedIncrement(&Data.RunCount);

                }, &Data, nullptr);

            Assert::IsNotNull(pTimer);

            FILETIME ftDueTime = {};
            ::SetThreadpoolTimer(pTimer, &ftDueTime, 0, 0);
            Sleep(100);

            int i = 0;
            for (; i != 100; ++i)
            {
                if (TryEnterCriticalSection(&Data.cs))
                {
                    break;
                }

                Sleep(50);
            }

            Assert::AreNotEqual(i, 100, L"100次尝试后必须成功");
            Assert::AreEqual(Data.RunCount, 1l);

            LeaveCriticalSection(&Data.cs);
            ::CloseThreadpoolTimer(pTimer);
        }
    };


    TEST_CLASS(FreeLibraryWhenCallbackReturns)
    {
        AwaysNullGuard Guard;

    public:
        FreeLibraryWhenCallbackReturns()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_SubmitThreadpoolWork;

            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolTimer;
            Guard |= YY::Thunks::aways_null_try_get_SetThreadpoolTimer;

            Guard |= YY::Thunks::aways_null_try_get_FreeLibraryWhenCallbackReturns;
        }

        struct UserData
        {
            HMODULE hHandle;
            long RunCount;

            UserData(HMODULE hMod)
                : hHandle(hMod)
                , RunCount(0)
            {
                Assert::IsNotNull(hHandle);
            }

            ~UserData()
            {
                //故意泄露，反正是一个单元测试
            }
        };

        TEST_METHOD(任务测试)
        {
            wchar_t DllPath[MAX_PATH];

            auto Count = GetModuleFileNameW((HMODULE)&__ImageBase, DllPath, _countof(DllPath));

            Assert::IsTrue(Count && Count < _countof(DllPath));

            CStringW NewDllPath;
            NewDllPath.Format(L"%ws_任务测试_%d.dll", DllPath, GetCurrentThreadId());

            auto bRet = CopyFileW(DllPath, NewDllPath, FALSE);

            Assert::IsTrue(bRet);

            UserData Data(LoadLibraryExW(NewDllPath, nullptr, 0));

            auto Work = ::CreateThreadpoolWork([](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_WORK              Work)
                {
                    Assert::IsNotNull(Context);
                    auto& Data = *(UserData*)Context;
                    ::FreeLibraryWhenCallbackReturns(Instance, Data.hHandle);
                    InterlockedIncrement(&Data.RunCount);

                }, &Data, nullptr);

            Assert::IsNotNull(Work);

            ::SubmitThreadpoolWork(Work);
            
            
            long i = 0;
            for (; i != 100; ++i)
            {
                Sleep(100);
                HMODULE hModule;
                if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, NewDllPath, &hModule) == FALSE)
                {
                    DeleteFileW(NewDllPath);
                    break;
                }
            }

            Assert::AreNotEqual(i, 100l);
            Assert::AreEqual(Data.RunCount, 1l);

            CloseThreadpoolWork(Work);

        }

        TEST_METHOD(定时器测试)
        {
            wchar_t DllPath[MAX_PATH];

            auto Count = GetModuleFileNameW((HMODULE)&__ImageBase, DllPath, _countof(DllPath));

            Assert::IsTrue(Count && Count < _countof(DllPath));

            CStringW NewDllPath;
            NewDllPath.Format(L"%ws_定时器测试_%d.dll", DllPath, GetCurrentThreadId());

            auto bRet = CopyFileW(DllPath, NewDllPath, FALSE);

            Assert::IsTrue(bRet);

            UserData Data(LoadLibraryExW(NewDllPath, nullptr, 0));;

            auto pTimer = ::CreateThreadpoolTimer([](
                _Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_     PTP_TIMER             Timer)
                {
                    Assert::IsNotNull(Context);
                    auto& Data = *(UserData*)Context;
                    ::FreeLibraryWhenCallbackReturns(Instance, Data.hHandle);
                    InterlockedIncrement(&Data.RunCount);
                }, &Data, nullptr);

            Assert::IsNotNull(pTimer);

            FILETIME ftDueTime = {};
            ::SetThreadpoolTimer(pTimer, &ftDueTime, 0, 0);

            long i = 0;
            for (; i != 100; ++i)
            {
                Sleep(100);
                HMODULE hModule;
                if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, NewDllPath, &hModule) == FALSE)
                {
                    DeleteFileW(NewDllPath);
                    break;
                }
            }

            Assert::AreNotEqual(i, 100l);
            Assert::AreEqual(Data.RunCount, 1l);

            ::CloseThreadpoolTimer(pTimer);
        }
    };


    TEST_CLASS(SetThreadpoolWait)
    {
        AwaysNullGuard Guard;

    public:
        SetThreadpoolWait()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolWait;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolWait;
            Guard |= YY::Thunks::aways_null_try_get_SetThreadpoolWait;
        }


        TEST_METHOD(无限等待)
        {
            auto hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

            Assert::IsNotNull(hEvent);

            long RunCount = 0;

            auto Wait = ::CreateThreadpoolWait([](
                    _Inout_     PTP_CALLBACK_INSTANCE Instance,
                    _Inout_opt_ PVOID                 Context,
                    _Inout_     PTP_WAIT              Wait,
                    _In_        TP_WAIT_RESULT        WaitResult
                    )
                {
                    Assert::AreEqual(WaitResult, (DWORD)WAIT_OBJECT_0);

                    Assert::IsNotNull(Context);

                    InterlockedIncrement((long*)Context);

                }, &RunCount, nullptr);

            Assert::IsNotNull(Wait);

            ::SetThreadpoolWait(Wait, hEvent, nullptr);

            Sleep(1000);

            Assert::AreEqual(RunCount, 0l);

            SetEvent(hEvent);

            Sleep(1000);
            Assert::AreEqual(RunCount, 1l);

            Sleep(1000);
            Assert::AreEqual(RunCount, 1l);

            ::CloseThreadpoolWait(Wait);

            CloseHandle(hEvent);
        }

        TEST_METHOD(相对时间等待)
        {
            {
                auto hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

                Assert::IsNotNull(hEvent);

                volatile long RunCount = 0;

                auto Wait = ::CreateThreadpoolWait([](
                    _Inout_     PTP_CALLBACK_INSTANCE Instance,
                    _Inout_opt_ PVOID                 Context,
                    _Inout_     PTP_WAIT              Wait,
                    _In_        TP_WAIT_RESULT        WaitResult
                    )
                    {
                        Assert::AreEqual(WaitResult, (DWORD)WAIT_TIMEOUT);

                        Assert::IsNotNull(Context);

                        InterlockedIncrement((long*)Context);

                    }, (void*)&RunCount, nullptr);

                Assert::IsNotNull(Wait);

                CFileTime TimeOut = long long(CFileTime::Second) * -2ll;

                ::SetThreadpoolWait(Wait, hEvent, &TimeOut);

                Sleep(1000);

                Assert::AreEqual((long)RunCount, 0l);


                Sleep(2000);
                Assert::AreEqual((long)RunCount, 1l);

                ::CloseThreadpoolWait(Wait);

                CloseHandle(hEvent);
            }


            //未超时
            {
                auto hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

                Assert::IsNotNull(hEvent);

                volatile long RunCount = 0;

                auto Wait = ::CreateThreadpoolWait([](
                    _Inout_     PTP_CALLBACK_INSTANCE Instance,
                    _Inout_opt_ PVOID                 Context,
                    _Inout_     PTP_WAIT              Wait,
                    _In_        TP_WAIT_RESULT        WaitResult
                    )
                    {
                        Assert::AreEqual(WaitResult, (DWORD)WAIT_OBJECT_0);

                        Assert::IsNotNull(Context);

                        InterlockedIncrement((long*)Context);

                    }, (void*)&RunCount, nullptr);

                Assert::IsNotNull(Wait);

                CFileTime TimeOut = long long(CFileTime::Second) * -2ll;

                ::SetThreadpoolWait(Wait, hEvent, &TimeOut);

                Sleep(1000);



                Assert::AreEqual((long)RunCount, 0l);

                SetEvent(hEvent);

                Sleep(2000);
                Assert::AreEqual((long)RunCount, 1l);

                
                ::CloseThreadpoolWait(Wait);

                CloseHandle(hEvent);
            }
        }

        TEST_METHOD(绝对时间等待)
        {
            //等待一个未来的时间
            {
                auto hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

                Assert::IsNotNull(hEvent);

                volatile long RunCount = 0;

                auto Wait = ::CreateThreadpoolWait([](
                        _Inout_     PTP_CALLBACK_INSTANCE Instance,
                        _Inout_opt_ PVOID                 Context,
                        _Inout_     PTP_WAIT              Wait,
                        _In_        TP_WAIT_RESULT        WaitResult
                        )
                    {
                        Assert::AreEqual(WaitResult, (DWORD)WAIT_TIMEOUT);

                        Assert::IsNotNull(Context);

                        InterlockedIncrement((long*)Context);

                    }, (void*)&RunCount, nullptr);

                Assert::IsNotNull(Wait);

                CFileTime TimeOut;
                GetSystemTimeAsFileTime(&TimeOut);

                TimeOut += CFileTime::Second * 2ll;

                ::SetThreadpoolWait(Wait, hEvent, &TimeOut);

                Sleep(1000);

                Assert::AreEqual((long)RunCount, 0l);


                Sleep(2000);
                Assert::AreEqual((long)RunCount, 1l);

                ::CloseThreadpoolWait(Wait);

                CloseHandle(hEvent);
            }

            //等待一个过去的时间
            {
                auto hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

                Assert::IsNotNull(hEvent);

                volatile long RunCount = 0;

                auto Wait = ::CreateThreadpoolWait([](
                    _Inout_     PTP_CALLBACK_INSTANCE Instance,
                    _Inout_opt_ PVOID                 Context,
                    _Inout_     PTP_WAIT              Wait,
                    _In_        TP_WAIT_RESULT        WaitResult
                    )
                    {
                        Assert::AreEqual(WaitResult, (DWORD)WAIT_TIMEOUT);

                        Assert::IsNotNull(Context);

                        InterlockedIncrement((long*)Context);

                    }, (void*)&RunCount, nullptr);

                Assert::IsNotNull(Wait);

                CFileTime TimeOut;
                GetSystemTimeAsFileTime(&TimeOut);

                TimeOut -= CFileTime::Second * 5ll;

                ::SetThreadpoolWait(Wait, hEvent, &TimeOut);

                Sleep(1000);

                Assert::AreEqual((long)RunCount, 1l);

                ::CloseThreadpoolWait(Wait);

                CloseHandle(hEvent);
            }
        }


        TEST_METHOD(取消等待器)
        {
            {
                auto hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

                Assert::IsNotNull(hEvent);

                volatile long RunCount = 0;

                auto Wait = ::CreateThreadpoolWait([](
                    _Inout_     PTP_CALLBACK_INSTANCE Instance,
                    _Inout_opt_ PVOID                 Context,
                    _Inout_     PTP_WAIT              Wait,
                    _In_        TP_WAIT_RESULT        WaitResult
                    )
                    {
                        Assert::AreEqual(WaitResult, (DWORD)WAIT_TIMEOUT);

                        Assert::IsNotNull(Context);

                        InterlockedIncrement((long*)Context);

                    }, (void*)&RunCount, nullptr);

                Assert::IsNotNull(Wait);

                CFileTime TimeOut;
                GetSystemTimeAsFileTime(&TimeOut);

                TimeOut += CFileTime::Second * 2ll;

                ::SetThreadpoolWait(Wait, hEvent, &TimeOut);

                Sleep(1000);

                Assert::AreEqual((long)RunCount, 0l);

                ::SetThreadpoolWait(Wait, NULL, NULL);

                Sleep(2000);
                Assert::AreEqual((long)RunCount, 0l);

                ::CloseThreadpoolWait(Wait);

                CloseHandle(hEvent);
            }
        }
    };

    TEST_CLASS(WaitForThreadpoolWaitCallbacks)
    {
        AwaysNullGuard Guard;

    public:
        WaitForThreadpoolWaitCallbacks()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolWait;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolWait;
            Guard |= YY::Thunks::aways_null_try_get_SetThreadpoolWait;
            Guard |= YY::Thunks::aways_null_try_get_WaitForThreadpoolWaitCallbacks;
        }

        TEST_METHOD(一般行为验证)
        {
            {
                auto hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

                Assert::IsNotNull(hEvent);

                volatile long RunCount = 0;

                auto Wait = ::CreateThreadpoolWait([](
                    _Inout_     PTP_CALLBACK_INSTANCE Instance,
                    _Inout_opt_ PVOID                 Context,
                    _Inout_     PTP_WAIT              Wait,
                    _In_        TP_WAIT_RESULT        WaitResult
                    )
                    {
                        Sleep(5 * 1000);
                        Assert::AreEqual(WaitResult, (DWORD)WAIT_TIMEOUT);

                        Assert::IsNotNull(Context);

                        InterlockedIncrement((long*)Context);

                    }, (void*)&RunCount, nullptr);

                Assert::IsNotNull(Wait);

                CFileTime TimeOut = {};

                ::SetThreadpoolWait(Wait, hEvent, &TimeOut);


                auto h = (HANDLE)_beginthreadex(nullptr, 0, [](void* Wait) -> unsigned
                    {
                        Sleep(1000);

                        ::WaitForThreadpoolWaitCallbacks((PTP_WAIT)Wait, TRUE);

                        return 0;
                    }, Wait, 0, nullptr);

                Assert::IsNotNull(h);

                Assert::AreEqual(WaitForSingleObject(h, 10 * 1000), (DWORD)WAIT_OBJECT_0, L"10秒内必须完成，这是预期。");

                
    
                Assert::AreEqual((long)RunCount, 1l);

                CloseHandle(h);

                ::CloseThreadpoolWait(Wait);

                CloseHandle(hEvent);
            }
        }
    };

    TEST_CLASS(StartThreadpoolIo)
    {
        AwaysNullGuard Guard;

    public:
        StartThreadpoolIo()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolIo;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolIo;
            Guard |= YY::Thunks::aways_null_try_get_StartThreadpoolIo;
            Guard |= YY::Thunks::aways_null_try_get_CancelThreadpoolIo;
        }

        TEST_METHOD(如果不调用StartThreadpoolIo那么Claaback一直不会触发)
        {
            auto _hFile = CreateFileW(LR"(C:\Windows\System32\ntdll.dll)", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

            struct TestInfo
            {
                DWORD uRef;
                PVOID Context;
                PVOID Overlapped;
                ULONG_PTR NumberOfBytesTransferred;
                PTP_IO Io;
                HANDLE hEvent;
            };

            TestInfo _TestData = {};
            _TestData.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

            auto _pIo = CreateThreadpoolIo(_hFile, [](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_opt_ PVOID                 Overlapped,
                _In_        ULONG                 IoResult,
                _In_        ULONG_PTR             NumberOfBytesTransferred,
                _Inout_     PTP_IO                Io)
                {

                    auto _pTestInfo = (TestInfo*)Context;
                    InterlockedIncrement(&_pTestInfo->uRef);
                    _pTestInfo->Context = Context;
                    _pTestInfo->Overlapped = Overlapped;
                    _pTestInfo->NumberOfBytesTransferred = NumberOfBytesTransferred;
                    SetEvent(_pTestInfo->hEvent);
                }, &_TestData, nullptr);

            Assert::IsNotNull(_pIo);

            char Buffer1[100];
            OVERLAPPED _Overlapped = {};
            if (!ReadFile(_hFile, Buffer1, sizeof(Buffer1), nullptr, &_Overlapped) && GetLastError() == ERROR_IO_PENDING)
            {
                auto _uResult = WaitForSingleObject(_TestData.hEvent, 100);
                Assert::AreEqual(_uResult, (DWORD)WAIT_TIMEOUT);
                Assert::AreEqual(_TestData.uRef, 0ul);
            }
            ::CloseThreadpoolIo(_pIo);
        }

        TEST_METHOD(StartThreadpoolIo一次)
        {
            auto _hFile = CreateFileW(LR"(C:\Windows\System32\ntdll.dll)", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

            struct TestInfo
            {
                DWORD uRef;
                PVOID Context;
                PVOID Overlapped;
                ULONG_PTR NumberOfBytesTransferred;
                PTP_IO Io;
                HANDLE hEvent;
            };

            TestInfo _TestData = {};
            _TestData.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

            auto _pIo = CreateThreadpoolIo(_hFile, [](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_opt_ PVOID                 Overlapped,
                _In_        ULONG                 IoResult,
                _In_        ULONG_PTR             NumberOfBytesTransferred,
                _Inout_     PTP_IO                Io)
                {

                    auto _pTestInfo = (TestInfo*)Context;
                    InterlockedIncrement(&_pTestInfo->uRef);
                    _pTestInfo->Context = Context;
                    _pTestInfo->Overlapped = Overlapped;
                    _pTestInfo->NumberOfBytesTransferred = NumberOfBytesTransferred;
                    _pTestInfo->Io = Io;
                    SetEvent(_pTestInfo->hEvent);;
                }, &_TestData, nullptr);

            Assert::IsNotNull(_pIo);

            ::StartThreadpoolIo(_pIo);
            char Buffer1[100];
            {
                OVERLAPPED _Overlapped = {};
                if (!ReadFile(_hFile, Buffer1, sizeof(Buffer1), nullptr, &_Overlapped) && GetLastError() == ERROR_IO_PENDING)
                {
                    auto _uResult = WaitForSingleObject(_TestData.hEvent, 5 * 1000);
                    Assert::AreEqual(_uResult, (DWORD)WAIT_OBJECT_0);
                    Assert::AreEqual(_TestData.uRef, 1ul);

                    Assert::AreEqual(_TestData.Context, (PVOID)&_TestData);
                    Assert::AreEqual((void*)_TestData.Io, (void*)_pIo);
                    Assert::AreEqual(_TestData.NumberOfBytesTransferred, (ULONG_PTR)sizeof(Buffer1));
                    Assert::AreEqual(_TestData.Overlapped, (LPVOID)&_Overlapped);
                }
                else
                {
                    ::CancelThreadpoolIo(_pIo);
                }
            }
            {
                OVERLAPPED _Overlapped = {};
                if (!ReadFile(_hFile, Buffer1, sizeof(Buffer1), nullptr, &_Overlapped) && GetLastError() == ERROR_IO_PENDING)
                {
                    auto _uResult = WaitForSingleObject(_TestData.hEvent, 100);
                    Assert::AreEqual(_uResult, (DWORD)WAIT_TIMEOUT);
                }
            }
            Assert::AreEqual(_TestData.uRef, 1ul);
            ::CloseThreadpoolIo(_pIo);
        }

        TEST_METHOD(StartThreadpoolIo二次)
        {
            auto _hFile = CreateFileW(LR"(C:\Windows\System32\ntdll.dll)", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

            struct TestInfo
            {
                DWORD uRef;
                PVOID Context;
                PVOID Overlapped;
                ULONG_PTR NumberOfBytesTransferred;
                PTP_IO Io;
                HANDLE hEvent;
            };

            TestInfo _TestData = {};
            _TestData.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            auto _pIo = CreateThreadpoolIo(_hFile, [](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_opt_ PVOID                 Overlapped,
                _In_        ULONG                 IoResult,
                _In_        ULONG_PTR             NumberOfBytesTransferred,
                _Inout_     PTP_IO                Io)
                {

                    auto _pTestInfo = (TestInfo*)Context;
                    InterlockedIncrement(&_pTestInfo->uRef);
                    _pTestInfo->Context = Context;
                    _pTestInfo->Overlapped = Overlapped;
                    _pTestInfo->NumberOfBytesTransferred = NumberOfBytesTransferred;
                    _pTestInfo->Io = Io;
                    SetEvent(_pTestInfo->hEvent);
                }, &_TestData, nullptr);

            Assert::IsNotNull(_pIo);

            ::StartThreadpoolIo(_pIo);
            ::StartThreadpoolIo(_pIo);
            
            {
                char Buffer1[100];
                OVERLAPPED _Overlapped = {};
                if (!ReadFile(_hFile, Buffer1, sizeof(Buffer1), nullptr, &_Overlapped) && GetLastError() == ERROR_IO_PENDING)
                {
                    auto _uResult = WaitForSingleObject(_TestData.hEvent, 5 * 1000);
                    Assert::AreEqual(_uResult, (DWORD)WAIT_OBJECT_0);
                    Assert::AreEqual(_TestData.uRef, 1ul);

                    Assert::AreEqual(_TestData.Context, (PVOID)&_TestData);
                    Assert::AreEqual((void*)_TestData.Io, (void*)_pIo);
                    Assert::AreEqual(_TestData.NumberOfBytesTransferred, (ULONG_PTR)sizeof(Buffer1));
                    Assert::AreEqual(_TestData.Overlapped, (LPVOID)&_Overlapped);
                }
                else
                {
                    ::CancelThreadpoolIo(_pIo);
                }
            }
            {
                char Buffer1[200];
                OVERLAPPED _Overlapped = {};
                if (!ReadFile(_hFile, Buffer1, sizeof(Buffer1), nullptr, &_Overlapped) && GetLastError() == ERROR_IO_PENDING)
                {

                    auto _uResult = WaitForSingleObject(_TestData.hEvent, 5 * 1000);
                    Assert::AreEqual(_uResult, (DWORD)WAIT_OBJECT_0);

                    Assert::AreEqual(_TestData.uRef, 2ul);
                    Assert::AreEqual(_TestData.Context, (PVOID)&_TestData);
                    Assert::AreEqual((void*)_TestData.Io, (void*)_pIo);
                    Assert::AreEqual(_TestData.NumberOfBytesTransferred, (ULONG_PTR)sizeof(Buffer1));
                    Assert::AreEqual(_TestData.Overlapped, (LPVOID)&_Overlapped);
                }
                else
                {
                    ::CancelThreadpoolIo(_pIo);
                }
            }
            
            ::CloseThreadpoolIo(_pIo);
        }

    };

    TEST_CLASS(CancelThreadpoolIo)
    {
        AwaysNullGuard Guard;

    public:
        CancelThreadpoolIo()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolIo;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolIo;
            Guard |= YY::Thunks::aways_null_try_get_StartThreadpoolIo;
            Guard |= YY::Thunks::aways_null_try_get_CancelThreadpoolIo;
        }

        TEST_METHOD(彻底取消后不会再触发)
        {
            auto _hFile = CreateFileW(LR"(C:\Windows\System32\ntdll.dll)", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

            struct TestInfo
            {
                DWORD uRef;
                PVOID Context;
                PVOID Overlapped;
                ULONG_PTR NumberOfBytesTransferred;
                PTP_IO Io;
                HANDLE hEvent;
            };

            TestInfo _TestData = {};
            _TestData.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            auto _pIo = CreateThreadpoolIo(_hFile, [](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_opt_ PVOID                 Overlapped,
                _In_        ULONG                 IoResult,
                _In_        ULONG_PTR             NumberOfBytesTransferred,
                _Inout_     PTP_IO                Io)
                {

                    auto _pTestInfo = (TestInfo*)Context;
                    InterlockedIncrement(&_pTestInfo->uRef);
                    _pTestInfo->Context = Context;
                    _pTestInfo->Overlapped = Overlapped;
                    _pTestInfo->NumberOfBytesTransferred = NumberOfBytesTransferred;
                    _pTestInfo->Io = Io;
                    SetEvent(_pTestInfo->hEvent);
                }, &_TestData, nullptr);

            Assert::IsNotNull(_pIo);

            ::StartThreadpoolIo(_pIo);
            ::CancelThreadpoolIo(_pIo);
            char Buffer1[100];
            {
                OVERLAPPED _Overlapped = {};
                if (!ReadFile(_hFile, Buffer1, sizeof(Buffer1), nullptr, &_Overlapped) && GetLastError() == ERROR_IO_PENDING)
                {
                    auto _uResult = WaitForSingleObject(_TestData.hEvent, 300);
                    Assert::AreEqual(_uResult, (DWORD)WAIT_TIMEOUT);
                    Assert::AreEqual(_TestData.uRef, 0ul);
                }
            }
            ::CloseThreadpoolIo(_pIo);
        }

        TEST_METHOD(没有彻底取消时任然可以收到特定次数)
        {
            auto _hFile = CreateFileW(LR"(C:\Windows\System32\ntdll.dll)", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

            struct TestInfo
            {
                DWORD uRef;
                PVOID Context;
                PVOID Overlapped;
                ULONG_PTR NumberOfBytesTransferred;
                PTP_IO Io;
                HANDLE hEvent;
            };

            TestInfo _TestData = {};
            _TestData.hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            auto _pIo = CreateThreadpoolIo(_hFile, [](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_opt_ PVOID                 Overlapped,
                _In_        ULONG                 IoResult,
                _In_        ULONG_PTR             NumberOfBytesTransferred,
                _Inout_     PTP_IO                Io)
                {

                    auto _pTestInfo = (TestInfo*)Context;
                    InterlockedIncrement(&_pTestInfo->uRef);
                    _pTestInfo->Context = Context;
                    _pTestInfo->Overlapped = Overlapped;
                    _pTestInfo->NumberOfBytesTransferred = NumberOfBytesTransferred;
                    _pTestInfo->Io = Io;
                    SetEvent(_pTestInfo->hEvent);
                }, &_TestData, nullptr);

            Assert::IsNotNull(_pIo);

            ::StartThreadpoolIo(_pIo);
            ::StartThreadpoolIo(_pIo);
            ::CancelThreadpoolIo(_pIo);

            {
                char Buffer1[100];
                OVERLAPPED _Overlapped = {};
                if (!ReadFile(_hFile, Buffer1, sizeof(Buffer1), nullptr, &_Overlapped) && GetLastError() == ERROR_IO_PENDING)
                {
                    auto _uResult = WaitForSingleObject(_TestData.hEvent, 100);
                    Assert::AreEqual(_uResult, (DWORD)WAIT_OBJECT_0);
                    Assert::AreEqual(_TestData.uRef, 1ul);

                    Assert::AreEqual(_TestData.Context, (PVOID)&_TestData);
                    Assert::AreEqual((void*)_TestData.Io, (void*)_pIo);
                    Assert::AreEqual(_TestData.NumberOfBytesTransferred, (ULONG_PTR)sizeof(Buffer1));
                    Assert::AreEqual(_TestData.Overlapped, (LPVOID)&_Overlapped);
                }
                else
                {
                    ::CancelThreadpoolIo(_pIo);
                }
            }
            {
                char Buffer1[200];
                OVERLAPPED _Overlapped = {};
                if (!ReadFile(_hFile, Buffer1, sizeof(Buffer1), nullptr, &_Overlapped) && GetLastError() == ERROR_IO_PENDING)
                {
                    auto _uResult = WaitForSingleObject(_TestData.hEvent, 100);
                    Assert::AreEqual(_uResult, (DWORD)WAIT_TIMEOUT);

                    Assert::AreEqual(_TestData.uRef, 1ul);
                }
            }

            ::CloseThreadpoolIo(_pIo);
        }

    };

    TEST_CLASS(WaitForThreadpoolIoCallbacks)
    {
        AwaysNullGuard Guard;

    public:
        WaitForThreadpoolIoCallbacks()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolIo;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolIo;
            Guard |= YY::Thunks::aways_null_try_get_StartThreadpoolIo;
            Guard |= YY::Thunks::aways_null_try_get_CancelThreadpoolIo;
            Guard |= YY::Thunks::aways_null_try_get_WaitForThreadpoolIoCallbacks;
        }

        TEST_METHOD(任务完成后可以收到信号)
        {
            auto _hFile = CreateFileW(LR"(C:\Windows\System32\ntdll.dll)", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

            struct TestInfo
            {
                DWORD uRef;
                PVOID Context;
                PVOID Overlapped;
                ULONG_PTR NumberOfBytesTransferred;
                PTP_IO Io;
            };

            TestInfo _TestData = {};

            auto _pIo = CreateThreadpoolIo(_hFile, [](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_opt_ PVOID                 Overlapped,
                _In_        ULONG                 IoResult,
                _In_        ULONG_PTR             NumberOfBytesTransferred,
                _Inout_     PTP_IO                Io)
                {
                    Sleep(500);
                    auto _pTestInfo = (TestInfo*)Context;
                    InterlockedIncrement(&_pTestInfo->uRef);
                    _pTestInfo->Context = Context;
                    _pTestInfo->Overlapped = Overlapped;
                    _pTestInfo->NumberOfBytesTransferred = NumberOfBytesTransferred;


                    _pTestInfo->Io = Io;
                }, &_TestData, nullptr);

            Assert::IsNotNull(_pIo);

            ::StartThreadpoolIo(_pIo);

            {
                char Buffer1[100];
                OVERLAPPED _Overlapped = {};
                if (!ReadFile(_hFile, Buffer1, sizeof(Buffer1), nullptr, &_Overlapped) && GetLastError() == ERROR_IO_PENDING)
                {
                    ::WaitForThreadpoolIoCallbacks(_pIo, FALSE);

                    Assert::AreEqual(_TestData.uRef, 1ul);

                    Assert::AreEqual(_TestData.Context, (PVOID)&_TestData);
                    Assert::AreEqual((void*)_TestData.Io, (void*)_pIo);
                    Assert::AreEqual(_TestData.NumberOfBytesTransferred, (ULONG_PTR)sizeof(Buffer1));
                    Assert::AreEqual(_TestData.Overlapped, (LPVOID)&_Overlapped);
                }
                else
                {
                    ::CancelThreadpoolIo(_pIo);
                }
            }
            

            ::CloseThreadpoolIo(_pIo);
        }

        TEST_METHOD(取消任务后回调没有信息)
        {
            auto _hFile = CreateFileW(LR"(C:\Windows\System32\ntdll.dll)", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

            struct TestInfo
            {
                DWORD uRef;
                PVOID Context;
                PVOID Overlapped;
                ULONG_PTR NumberOfBytesTransferred;
                PTP_IO Io;
                HANDLE hEvent;
            };

            TestInfo _TestData = {};
            _TestData.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

            auto _pIo = CreateThreadpoolIo(_hFile, [](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_opt_ PVOID                 Context,
                _Inout_opt_ PVOID                 Overlapped,
                _In_        ULONG                 IoResult,
                _In_        ULONG_PTR             NumberOfBytesTransferred,
                _Inout_     PTP_IO                Io)
                {
                    auto _pTestInfo = (TestInfo*)Context;
                    InterlockedIncrement(&_pTestInfo->uRef);
                    _pTestInfo->Context = Context;
                    _pTestInfo->Overlapped = Overlapped;
                    _pTestInfo->NumberOfBytesTransferred = NumberOfBytesTransferred;
                    _pTestInfo->Io = Io;
                    SetEvent(_pTestInfo->hEvent);
                }, &_TestData, nullptr);

            Assert::IsNotNull(_pIo);

            ::StartThreadpoolIo(_pIo);
            {
                char Buffer1[100];
                OVERLAPPED _Overlapped = {};
                if (!ReadFile(_hFile, Buffer1, sizeof(Buffer1), nullptr, &_Overlapped) && GetLastError() == ERROR_IO_PENDING)
                {
                    ::WaitForThreadpoolIoCallbacks(_pIo, TRUE);
                    WaitForSingleObject(_TestData.hEvent, 100);

                    Assert::AreEqual(_TestData.uRef, 0ul);
                }
                else
                {
                    ::CancelThreadpoolIo(_pIo);
                }
            }

            ::CloseThreadpoolIo(_pIo);
        }
    };


    TEST_CLASS(SetThreadpoolThreadMaximum)
    {
        AwaysNullGuard Guard;

    public:
        SetThreadpoolThreadMaximum()
        {
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpool;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpool;
            Guard |= YY::Thunks::aways_null_try_get_SetThreadpoolThreadMaximum;
            Guard |= YY::Thunks::aways_null_try_get_CreateThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_CloseThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_SubmitThreadpoolWork;
            Guard |= YY::Thunks::aways_null_try_get_WaitForThreadpoolWorkCallbacks;
        }

        TEST_METHOD(最大上限测试)
        {
            constexpr DWORD kThreadMaximum = 3;
            constexpr DWORD kWorkPostCount = 100;

            struct TestInfo
            {
                volatile DWORD uCount = 0;
                volatile DWORD uMax = 0;
                volatile DWORD uRunCount = 0;
            };

            TestInfo _TestInfo;

            auto _pPool = ::CreateThreadpool(nullptr);
            Assert::IsNotNull(_pPool);


            ::SetThreadpoolThreadMaximum(_pPool, kThreadMaximum);
            TP_CALLBACK_ENVIRON CallbackEnviron;
            ::InitializeThreadpoolEnvironment(&CallbackEnviron);
            ::SetThreadpoolCallbackPool(&CallbackEnviron, _pPool);

            auto _pWork = ::CreateThreadpoolWork([](_Inout_     PTP_CALLBACK_INSTANCE Instance,
                _Inout_ PVOID                 Context,
                _Inout_     PTP_WORK              Work)
                {
                    auto& _TestInfo = *(TestInfo*)Context;
                    InterlockedIncrement(&_TestInfo.uRunCount);
                    auto _uNew = InterlockedIncrement(&_TestInfo.uCount);

                    for (auto _uMax = _TestInfo.uMax;;)
                    {
                        if (_uMax >= _uNew)
                            break;

                        auto _uLast = InterlockedCompareExchange(&_TestInfo.uMax, _uNew, _uMax);
                        if (_uLast == _uMax)
                            break;
                        _uMax = _uLast;
                    }

                    Sleep(10);

                    InterlockedDecrement(&_TestInfo.uCount);

                }, & _TestInfo, & CallbackEnviron);

            Assert::IsNotNull(_pWork);

            for (int i = 0; i != kWorkPostCount; ++i)
            {
                ::SubmitThreadpoolWork(_pWork);
            }

            ::WaitForThreadpoolWorkCallbacks(_pWork, FALSE);

            ::CloseThreadpoolWork(_pWork);

            ::CloseThreadpool(_pPool);

            Assert::AreEqual((DWORD)_TestInfo.uCount, 0ul);
            Assert::AreEqual((DWORD)_TestInfo.uMax, 3ul);
            Assert::AreEqual((DWORD)_TestInfo.uRunCount, kWorkPostCount);
        }
    };
}
