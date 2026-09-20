#include "pch.h"
#include "CppUnitTest.h"

#include "Thunks/api-ms-win-core-namedpipe.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

EXTERN_C extern uint64_t g_uSystemVersion;

namespace api_ms_win_core_namedpipe
{
    namespace
    {
        /// <summary>
        /// 生成进程内唯一的本地命名管道名。
        /// </summary>
        /// <param name="_szBuffer">输出缓冲；调用方保证容量足够（128 个 wchar）。</param>
        /// <returns>以 NUL 结尾的管道路径（\\\\.\\pipe\\ 前缀）。</returns>
        PCWSTR MakeUniquePipeName(_Out_writes_z_(128) wchar_t (&_szBuffer)[128]) noexcept
        {
            // 进程内序号保证同一测试进程内多次调用不重名
            static volatile LONG s_uSequence = 0;
            swprintf_s(_szBuffer, L"\\\\.\\pipe\\YY-Thunks-UnitTest-%lu-%ld",
                GetCurrentProcessId(), InterlockedIncrement(&s_uSequence));
            return _szBuffer;
        }
    }

    TEST_CLASS(GetNamedPipeServerProcessId)
    {
        AwaysNullGuard Guard;

    public:
        GetNamedPipeServerProcessId()
        {
            g_uSystemVersion = __WindowsNT5;
            Guard |= YY::Thunks::aways_null_try_get_GetNamedPipeClientProcessId;
            Guard |= YY::Thunks::aways_null_try_get_GetNamedPipeServerProcessId;
        }

        ~GetNamedPipeServerProcessId()
        {
            g_uSystemVersion = 0;
        }

        TEST_METHOD(CreateNamedPipeW调用后应该可以立即查询ServerProcessId)
        {
            wchar_t _szPipeName[128] = {};

            auto _hServer = CreateNamedPipeW(MakeUniquePipeName(_szPipeName),
                PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                1, 0, 0, 0, nullptr);

            Assert::AreNotEqual((INVALID_HANDLE_VALUE), _hServer);

            ULONG _uServerProcessId = 0;
            Assert::IsTrue(::GetNamedPipeServerProcessId(_hServer, &_uServerProcessId));
            Assert::AreEqual((ULONG)GetCurrentProcessId(), _uServerProcessId);

            CloseHandle(_hServer);
        }

        TEST_METHOD(CreateFileW连接管道后应该可以获取ServerProcessId)
        {
            wchar_t _szPipeName[128] = {};

            auto _hServer = CreateNamedPipeW(MakeUniquePipeName(_szPipeName),
                PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                1, 0, 0, 0, nullptr);

            Assert::AreNotEqual(INVALID_HANDLE_VALUE, _hServer);

            auto _hClient = CreateFileW(_szPipeName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
            Assert::AreNotEqual(INVALID_HANDLE_VALUE, _hClient);

            {
                ULONG _uServerProcessId = 0;
                Assert::IsTrue(::GetNamedPipeServerProcessId(_hServer, &_uServerProcessId));
                Assert::AreEqual((ULONG)GetCurrentProcessId(), _uServerProcessId);
            }

            {
                ULONG _uServerProcessId = 0;
                Assert::IsTrue(::GetNamedPipeServerProcessId(_hClient, &_uServerProcessId));
                Assert::AreEqual((ULONG)GetCurrentProcessId(), _uServerProcessId);
            }

            CloseHandle(_hServer);
            CloseHandle(_hClient);
        }

        TEST_METHOD(文件句柄无法查询ServerProcessId)
        {
            wchar_t _szTempPath[MAX_PATH] = {};
            Assert::IsTrue(GetTempPathW(MAX_PATH, _szTempPath) != 0);
            wcscat_s(_szTempPath, L"YY-Thunks-UnitTest-Transparency.tmp");

            HANDLE _hFile = CreateFileW(_szTempPath, GENERIC_WRITE, 0, nullptr,
                CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, nullptr);
            Assert::IsTrue(_hFile != INVALID_HANDLE_VALUE);

            ULONG _uServerProcessId = 0;
            Assert::IsFalse(::GetNamedPipeServerProcessId(_hFile, &_uServerProcessId));
            Assert::AreEqual((DWORD)ERROR_INVALID_FUNCTION, GetLastError());

            CloseHandle(_hFile);
            DeleteFileW(_szTempPath);

        }
    };

    TEST_CLASS(GetNamedPipeClientProcessId)
    {
        AwaysNullGuard Guard;

    public:
        GetNamedPipeClientProcessId()
        {
            g_uSystemVersion = __WindowsNT5;
            Guard |= YY::Thunks::aways_null_try_get_GetNamedPipeClientProcessId;
            Guard |= YY::Thunks::aways_null_try_get_GetNamedPipeServerProcessId;
        }

        ~GetNamedPipeClientProcessId()
        {
            g_uSystemVersion = 0;
        }

        TEST_METHOD(CreateNamedPipeW调用后此时应该无法获取ClientProcessId)
        {
            wchar_t _szPipeName[128] = {};

            auto _hServer = CreateNamedPipeW(MakeUniquePipeName(_szPipeName),
                PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                1, 0, 0, 0, nullptr);

            Assert::AreNotEqual(INVALID_HANDLE_VALUE, _hServer);

            ULONG _uClientProcessId = 0;
            Assert::IsFalse(::GetNamedPipeClientProcessId(_hServer, &_uClientProcessId));
            Assert::AreEqual((DWORD)ERROR_NOT_FOUND, GetLastError());
            CloseHandle(_hServer);
        }

        TEST_METHOD(CreateFileW连接管道后应该可以获取ClientProcessId)
        {
            wchar_t _szPipeName[128] = {};

            auto _hServer = CreateNamedPipeW(MakeUniquePipeName(_szPipeName),
                PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                1, 0, 0, 0, nullptr);

            Assert::AreNotEqual(INVALID_HANDLE_VALUE, _hServer);

            auto _hClient = CreateFileW(_szPipeName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
            Assert::AreNotEqual(INVALID_HANDLE_VALUE, _hClient);

            {
                ULONG _uClientProcessId = 0;
                Assert::IsTrue(::GetNamedPipeClientProcessId(_hServer, &_uClientProcessId));
                Assert::AreEqual((ULONG)GetCurrentProcessId(), _uClientProcessId);
            }

            {
                ULONG _uClientProcessId = 0;
                Assert::IsTrue(::GetNamedPipeClientProcessId(_hClient, &_uClientProcessId));
                Assert::AreEqual((ULONG)GetCurrentProcessId(), _uClientProcessId);
            }

            CloseHandle(_hServer);
            CloseHandle(_hClient);
        }

        TEST_METHOD(CreateFileW_非管道路径应正常工作)
        {
            wchar_t _szTempPath[MAX_PATH] = {};
            Assert::IsTrue(GetTempPathW(MAX_PATH, _szTempPath) != 0);
            wcscat_s(_szTempPath, L"YY-Thunks-UnitTest-Transparency.tmp");

            HANDLE _hFile = CreateFileW(_szTempPath, GENERIC_WRITE, 0, nullptr,
                CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, nullptr);
            Assert::IsTrue(_hFile != INVALID_HANDLE_VALUE);

            const char _szContent[] = "yy-thunks";
            DWORD _cbWritten = 0;
            Assert::IsTrue(WriteFile(_hFile, _szContent, sizeof(_szContent), &_cbWritten, nullptr) != FALSE);
            Assert::AreEqual((DWORD)sizeof(_szContent), _cbWritten);

            CloseHandle(_hFile);
            DeleteFileW(_szTempPath);
        }
    };
}
