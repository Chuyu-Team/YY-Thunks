#include "pch.h"
#include "Thunks/api-ms-win-core-handle.hpp"

#pragma comment(lib, "KtmW32.lib")
#pragma comment(lib, "ntdll.lib")

namespace api_ms_win_core_handle
{
    TEST_CLASS(CompareObjectHandles)
    {
        AwaysNullGuard Guard;

    public:
        CompareObjectHandles()
        {
            Guard |= YY::Thunks::aways_null_try_get_CompareObjectHandles;
        }

        TEST_METHOD(常规比较)
        {
            {
                HANDLE _hHandle1 = CreateTransaction(nullptr, 0, 0, 0, 0, 0, nullptr);
                Assert::IsNotNull(_hHandle1);

                HANDLE _hHandle2 = CreateTransaction(nullptr, 0, 0, 0, 0, 0, nullptr);
                Assert::IsNotNull(_hHandle2);

                Assert::IsFalse(::CompareObjectHandles(_hHandle1, _hHandle2));

                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
            }

            if(ImpersonateSelf(SECURITY_IMPERSONATION_LEVEL::SecurityImpersonation))
            {
                HANDLE _hHandle1;
                Assert::IsTrue(OpenThreadToken(NtCurrentThread(), TOKEN_QUERY_SOURCE, FALSE, &_hHandle1));

                HANDLE _hHandle2;
                Assert::IsTrue(OpenThreadToken(NtCurrentThread(), TOKEN_QUERY_SOURCE, FALSE, &_hHandle2));

                bool _bRet = false;
                auto _hThread = CreateThread(nullptr, 0,
                    [](void* _pUserData) -> DWORD
                    {
                        if (ImpersonateSelf(SECURITY_IMPERSONATION_LEVEL::SecurityImpersonation))
                        {
                            for (;;)
                            {
                                if (*(volatile bool*)_pUserData)
                                    break;

                                Sleep(10);
                            }
                            RevertToSelf();
                        }

                        return 0;
                    }, & _bRet, 0, nullptr);

                Assert::IsNotNull(_hThread);

                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));

                Sleep(500);
                HANDLE _hHandle3;
                Assert::IsTrue(OpenThreadToken(_hThread, TOKEN_QUERY_SOURCE, FALSE, &_hHandle3));

                HANDLE _hHandle4;
                Assert::IsTrue(OpenThreadToken(_hThread, TOKEN_QUERY, FALSE, &_hHandle4));

                Assert::IsTrue(::CompareObjectHandles(_hHandle3, _hHandle4));
                Assert::IsFalse(::CompareObjectHandles(_hHandle1, _hHandle4));

                _bRet = true;
                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
                CloseHandle(_hHandle3);
                CloseHandle(_hHandle4);

                WaitForSingleObject(_hThread, INFINITE);
                CloseHandle(_hThread);

                RevertToSelf();
            }


            {
                HANDLE _hHandle1;
                Assert::IsTrue(OpenProcessToken(NtCurrentProcess(), TOKEN_QUERY, &_hHandle1));

                HANDLE _hHandle2;
                Assert::IsTrue(OpenProcessToken(NtCurrentProcess(), TOKEN_QUERY, &_hHandle2));

                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));

                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
            }

            {
                constexpr const wchar_t Name[] = L"\\KernelObjects\\CritSecOutOfMemoryEvent";
                UNICODE_STRING ObjectName = { sizeof(Name) - sizeof(wchar_t),sizeof(Name) - sizeof(wchar_t) ,(PWSTR)Name };
                OBJECT_ATTRIBUTES attr = { sizeof(attr),nullptr,&ObjectName };

                HANDLE _hHandle1;
                NtOpenKeyedEvent(&_hHandle1, MAXIMUM_ALLOWED, &attr);

                HANDLE _hHandle2;
                NtOpenKeyedEvent(&_hHandle2, MAXIMUM_ALLOWED, &attr);

                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));

                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
            }

            {
                HANDLE _hHandle1 = CreateWaitableTimerW(nullptr, FALSE, L"Local\\TestWaitableTimer");
                Assert::IsNotNull(_hHandle1);

                HANDLE _hHandle2 = OpenWaitableTimerW(SYNCHRONIZE, FALSE, L"Local\\TestWaitableTimer");
                Assert::IsNotNull(_hHandle2);

                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));

                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
            }

            {
                auto _hHandle1 = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 4);
                HANDLE _hHandle2;
                Assert::IsTrue(DuplicateHandle(NtGetCurrentProcess(), _hHandle1, NtGetCurrentProcess(), &_hHandle2, 0, FALSE, DUPLICATE_SAME_ACCESS));
                auto _hFile1 = CreateFileW(LR"(C:\Windows\System32\ntdll.dll)", GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));
                auto _hHandle3 = CreateIoCompletionPort(_hFile1, _hHandle1, 0, 0);
                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));

                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
                CloseHandle(_hFile1);
            }

            {
                auto _hHandle1 = OpenDesktopW(L"Default", 0, FALSE, STANDARD_RIGHTS_READ);
                auto _hHandle2 = OpenDesktopW(L"Default", 0, FALSE, STANDARD_RIGHTS_READ);

                // CI可能没有桌面
                if (_hHandle1 || _hHandle2)
                {
                    Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));
                }

                if(_hHandle1)
                    CloseHandle(_hHandle1);
                if(_hHandle2)
                    CloseHandle(_hHandle2);
            }

            {
                auto _hHandle1 = OpenWindowStationW(nullptr, FALSE, STANDARD_RIGHTS_READ);
                auto _hHandle2 = OpenWindowStationW(nullptr, FALSE, STANDARD_RIGHTS_READ);

                // CI可能没有桌面
                if (_hHandle1 || _hHandle2)
                {
                    Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));
                }

                if(_hHandle1)
                    CloseHandle(_hHandle1);

                if(_hHandle2)
                    CloseHandle(_hHandle2);
            }

            {
                auto _hHandle1 = CreateFileW(L"C:\\Windows", 0, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
                Assert::AreNotEqual(_hHandle1, INVALID_HANDLE_VALUE);

                auto _hHandle2 = CreateFileW(L"C:\\Windows", 0, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
                Assert::AreNotEqual(_hHandle2, INVALID_HANDLE_VALUE);

                HANDLE _hHandle3;
                Assert::IsTrue(DuplicateHandle(NtGetCurrentProcess(), _hHandle1, NtGetCurrentProcess(), &_hHandle3, 0, FALSE, DUPLICATE_SAME_ACCESS));

                Assert::IsFalse(::CompareObjectHandles(_hHandle1, _hHandle2));
                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle3));

                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
                CloseHandle(_hHandle3);
            }

            {
                auto _hHandle1 = CreateSemaphoreW(nullptr, 0, 2, L"Local\\TestSemaphore");
                auto _hHandle2 = OpenSemaphoreW(SYNCHRONIZE, FALSE, L"Local\\TestSemaphore");

                Assert::IsNotNull(_hHandle1);
                Assert::IsNotNull(_hHandle2);

                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));
                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
            }

            {
                auto _hHandle1 = CreateMutexW(nullptr, FALSE, L"Local\\TestMutex");
                auto _hHandle2 = OpenMutexW(SYNCHRONIZE, FALSE, L"Local\\TestMutex");

                Assert::IsNotNull(_hHandle1);
                Assert::IsNotNull(_hHandle2);

                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));
                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
            }

            {
                auto _hHandle1 = CreateNamedPipeW(
                    L"\\\\.\\pipe\\TestNamedPipe",
                    PIPE_ACCESS_DUPLEX,
                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                    PIPE_UNLIMITED_INSTANCES,
                    512,
                    512,
                    0,
                    NULL);

                auto _hHandle2 = CreateFileW(L"\\\\.\\pipe\\TestNamedPipe",GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, NULL);
                HANDLE _hHandle3;
                Assert::IsTrue(DuplicateHandle(NtGetCurrentProcess(), _hHandle1, NtGetCurrentProcess(), &_hHandle3, 0, FALSE, DUPLICATE_SAME_ACCESS));

                //auto _hHandle1 = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READONLY, 0, 4096, L"Local\\TestFileMap");
                //auto _hHandle2 = OpenFileMappingW(FILE_MAP_READ, FALSE, L"Local\\TestFileMap");

                Assert::IsNotNull(_hHandle1);
                Assert::IsNotNull(_hHandle2);

                Assert::IsFalse(::CompareObjectHandles(_hHandle1, _hHandle2));
                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle3));
                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
            }

            {
                auto _hHandle1 = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READONLY, 0, 4096, L"Local\\TestFileMap");
                auto _hHandle2 = OpenFileMappingW(FILE_MAP_READ, FALSE, L"Local\\TestFileMap");

                Assert::IsNotNull(_hHandle1);
                Assert::IsNotNull(_hHandle2);

                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));
                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
            }

            {
                HKEY _hHandle1;
                auto _lStatus = RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE", &_hHandle1);
                Assert::AreEqual(_lStatus, ERROR_SUCCESS);

                HKEY _hHandle2;
                _lStatus = RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE", &_hHandle2);
                Assert::AreEqual(_lStatus, ERROR_SUCCESS);

                Assert::IsFalse(::CompareObjectHandles(_hHandle1, _hHandle2));

                HKEY _hHandle3;
                Assert::IsTrue(DuplicateHandle(NtGetCurrentProcess(), _hHandle1, NtGetCurrentProcess(), (HANDLE*)&_hHandle3, 0, FALSE, DUPLICATE_SAME_ACCESS));
                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle3));

                RegCloseKey(_hHandle1);
                RegCloseKey(_hHandle2);
                RegCloseKey(_hHandle3);
            }

            {
                auto _hHandle1 = OpenThread(SYNCHRONIZE, FALSE, GetCurrentThreadId());
                auto _hHandle2 = OpenThread(SYNCHRONIZE, FALSE, GetCurrentThreadId());

                Assert::IsNotNull(_hHandle1);
                Assert::IsNotNull(_hHandle2);

                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));
                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
            }

            {
                auto _hHandle1 = OpenProcess(SYNCHRONIZE, FALSE, GetCurrentProcessId());
                auto _hHandle2 = OpenProcess(SYNCHRONIZE, FALSE, GetCurrentProcessId());

                Assert::IsNotNull(_hHandle1);
                Assert::IsNotNull(_hHandle2);

                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));
                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
            }

            {
                auto _hHandle1 = CreateEventW(nullptr, FALSE, FALSE, L"Local\\TestEvent");
                auto _hHandle2 = OpenEventW(SYNCHRONIZE, FALSE, L"Local\\TestEvent");
                auto _hHandle3 = CreateEventW(nullptr, FALSE, FALSE, L"Local\\TestEvend");

                Assert::IsNotNull(_hHandle1);
                Assert::IsNotNull(_hHandle2);
                Assert::IsNotNull(_hHandle3);

                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle2));
                Assert::IsFalse(::CompareObjectHandles(_hHandle1, _hHandle3));
                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
                CloseHandle(_hHandle3);
            }

            {
                auto _hFile1 = CreateFileW(LR"(C:\Windows\System32\ntdll.dll)", 0, FILE_SHARE_DELETE | FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, NULL);
                auto _hFile2 = CreateFileW(LR"(C:\Windows\System32\ntdll.dll)", 0, FILE_SHARE_DELETE | FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, NULL);
                HANDLE _hFile3;
                Assert::IsTrue(DuplicateHandle(NtGetCurrentProcess(), _hFile1, NtGetCurrentProcess(), &_hFile3, 0, FALSE, DUPLICATE_SAME_ACCESS));

                Assert::IsNotNull(_hFile1);
                Assert::IsNotNull(_hFile2);
                Assert::IsNotNull(_hFile3);

                Assert::IsFalse(::CompareObjectHandles(_hFile1, _hFile2));
                Assert::IsTrue(::CompareObjectHandles(_hFile1, _hFile3));
                CloseHandle(_hFile1);
                CloseHandle(_hFile2);
                CloseHandle(_hFile3);
            }
        }

        TEST_METHOD(匿名对象)
        {
            {
                auto _hHandle1 = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                Assert::IsNotNull(_hHandle1);
                auto _hHandle2 = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                Assert::IsNotNull(_hHandle2);
                HANDLE _hHandle3;
                HANDLE _hHandle4;
                Assert::IsTrue(DuplicateHandle(NtGetCurrentProcess(), _hHandle1, NtGetCurrentProcess(), &_hHandle3, 0, FALSE, DUPLICATE_SAME_ACCESS));
                Assert::IsTrue(DuplicateHandle(NtGetCurrentProcess(), _hHandle2, NtGetCurrentProcess(), &_hHandle4, 0, FALSE, DUPLICATE_SAME_ACCESS));

                Assert::IsFalse(::CompareObjectHandles(_hHandle1, _hHandle2));
                Assert::IsTrue(::CompareObjectHandles(_hHandle1, _hHandle3));

                CloseHandle(_hHandle1);
                CloseHandle(_hHandle2);
                CloseHandle(_hHandle3);
                CloseHandle(_hHandle4);
            }
        }
    };
}
