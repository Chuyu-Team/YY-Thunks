#include "pch.h"
#include "Thunks/api-ms-win-core-processthreads.hpp"

namespace api_ms_win_core_processthreads
{
TEST_CLASS(GetSystemCpuSetInformation)
{
    AwaysNullGuard Guard;

public:
    GetSystemCpuSetInformation()
    {
        Guard |= YY::Thunks::aways_null_try_get_GetSystemCpuSetInformation;
    }

    TEST_METHOD(查询所需长度_报告不支持)
    {
        ULONG _uReturnedLength = MAXDWORD;

        Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 0, &_uReturnedLength, nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_NOT_SUPPORTED), ::GetLastError());
        Assert::AreEqual(0ul, _uReturnedLength);
    }

    TEST_METHOD(返回长度为nullptr_报告参数错误)
    {
        Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 0, nullptr, nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), ::GetLastError());
    }
};

TEST_CLASS(GetProcessDefaultCpuSets)
{
    AwaysNullGuard Guard;

public:
    GetProcessDefaultCpuSets()
    {
        Guard |= YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets;
    }

    TEST_METHOD(查询所需数量_报告不支持)
    {
        ULONG _uRequiredIdCount = MAXDWORD;

        Assert::IsFalse(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uRequiredIdCount));
        Assert::AreEqual(DWORD(ERROR_NOT_SUPPORTED), ::GetLastError());
        Assert::AreEqual(0ul, _uRequiredIdCount);
    }

    TEST_METHOD(所需数量为nullptr_报告参数错误)
    {
        Assert::IsFalse(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, nullptr));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), ::GetLastError());
    }
};

TEST_CLASS(SetProcessDefaultCpuSets)
{
    AwaysNullGuard Guard;

public:
    SetProcessDefaultCpuSets()
    {
        Guard |= YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets;
    }

    TEST_METHOD(清空进程默认CPU集合_报告不支持)
    {
        Assert::IsFalse(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_NOT_SUPPORTED), ::GetLastError());
    }
};

TEST_CLASS(GetThreadSelectedCpuSets)
{
    AwaysNullGuard Guard;

public:
    GetThreadSelectedCpuSets()
    {
        Guard |= YY::Thunks::aways_null_try_get_GetThreadSelectedCpuSets;
    }

    TEST_METHOD(查询所需数量_报告不支持)
    {
        ULONG _uRequiredIdCount = MAXDWORD;

        Assert::IsFalse(::GetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0, &_uRequiredIdCount));
        Assert::AreEqual(DWORD(ERROR_NOT_SUPPORTED), ::GetLastError());
        Assert::AreEqual(0ul, _uRequiredIdCount);
    }

    TEST_METHOD(所需数量为nullptr_报告参数错误)
    {
        Assert::IsFalse(::GetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0, nullptr));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), ::GetLastError());
    }
};

TEST_CLASS(SetThreadSelectedCpuSets)
{
    AwaysNullGuard Guard;

public:
    SetThreadSelectedCpuSets()
    {
        Guard |= YY::Thunks::aways_null_try_get_SetThreadSelectedCpuSets;
    }

    TEST_METHOD(清空线程CPU集合_报告不支持)
    {
        Assert::IsFalse(::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_NOT_SUPPORTED), ::GetLastError());
    }
};
}
