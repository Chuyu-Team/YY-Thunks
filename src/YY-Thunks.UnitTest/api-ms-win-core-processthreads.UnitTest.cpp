#include "pch.h"
#include "Thunks/api-ms-win-core-processthreads.hpp"

#include <algorithm>
#include <vector>

namespace api_ms_win_core_processthreads
{
namespace
{
    // 旧系统模拟实现把“一个逻辑处理器 = 一个 CPU Set”，Id 为 组号 * 掩码位宽 + 组内编号，
    // 掩码位宽即 KAFFINITY 的位宽（x86 32 / x64 64）。
    constexpr ULONG _uCpuSetIdBitCount = sizeof(KAFFINITY) * 8;

    ULONG __CountAffinityMask(KAFFINITY _uMask)
    {
        ULONG _uCount = 0;

        for (; _uMask != 0; _uMask &= _uMask - 1)
        {
            ++_uCount;
        }

        return _uCount;
    }

    std::vector<ULONG> __CpuSetIdsFromAffinityMask(KAFFINITY _uMask, WORD _uGroup)
    {
        std::vector<ULONG> _CpuSetIds;

        for (ULONG _uBit = 0; _uBit != _uCpuSetIdBitCount; ++_uBit)
        {
            if ((_uMask & ((KAFFINITY)1 << _uBit)) == 0)
                continue;

            _CpuSetIds.push_back(_uGroup * _uCpuSetIdBitCount + _uBit);
        }

        return _CpuSetIds;
    }
}

TEST_CLASS(GetSystemCpuSetInformation)
{
    AwaysNullGuard Guard;

public:
    GetSystemCpuSetInformation()
    {
        Guard |= YY::Thunks::aways_null_try_get_GetSystemCpuSetInformation;
    }

    TEST_METHOD(查询所需长度_返回缓冲区不足)
    {
        ULONG _uReturnedLength = 0;

        Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 0, &_uReturnedLength, nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError());
        Assert::IsTrue(_uReturnedLength != 0);
        Assert::AreEqual(0ul, ULONG(_uReturnedLength % sizeof(SYSTEM_CPU_SET_INFORMATION)));
    }

    TEST_METHOD(填充缓冲区_处理器编号与组有效)
    {
        ULONG _uReturnedLength = 0;
        Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 0, &_uReturnedLength, nullptr, 0));
        Assert::IsTrue(_uReturnedLength != 0);

        std::vector<BYTE> _Buffer(_uReturnedLength);
        ULONG _uFilledLength = 0;
        Assert::IsTrue(::GetSystemCpuSetInformation(
            reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(_Buffer.data()), _uReturnedLength, &_uFilledLength, nullptr, 0));

        // 模拟实现只覆盖组 0 与掩码位宽内的处理器
        Assert::AreEqual(_uReturnedLength, _uFilledLength);
        const auto _cRecords = _uFilledLength / sizeof(SYSTEM_CPU_SET_INFORMATION);
        Assert::IsTrue(_cRecords != 0);
        Assert::IsTrue(_cRecords <= _uCpuSetIdBitCount);

        std::vector<ULONG> _CpuSetIds;
        for (ULONG _uOffset = 0; _uOffset != _uFilledLength; _uOffset += sizeof(SYSTEM_CPU_SET_INFORMATION))
        {
            const auto _pInformation = reinterpret_cast<const SYSTEM_CPU_SET_INFORMATION*>(_Buffer.data() + _uOffset);

            Assert::AreEqual(DWORD(sizeof(SYSTEM_CPU_SET_INFORMATION)), _pInformation->Size);
            Assert::AreEqual(int(CpuSetInformation), int(_pInformation->Type));
            Assert::AreEqual(WORD(0), _pInformation->CpuSet.Group);
            Assert::AreEqual(
                ULONG(_pInformation->CpuSet.Group * _uCpuSetIdBitCount + _pInformation->CpuSet.LogicalProcessorIndex),
                _pInformation->CpuSet.Id);
            Assert::IsTrue((_pInformation->CpuSet.AllFlags & SYSTEM_CPU_SET_INFORMATION_ALLOCATED) != 0);

            _CpuSetIds.push_back(_pInformation->CpuSet.Id);
        }

        // Id 不能重复，否则调用方按 Id 匹配处理器会串台
        std::sort(_CpuSetIds.begin(), _CpuSetIds.end());
        Assert::IsTrue(std::adjacent_find(_CpuSetIds.begin(), _CpuSetIds.end()) == _CpuSetIds.end());
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

    TEST_METHOD(查询所需数量_与进程亲和性一致)
    {
        DWORD_PTR _uProcessAffinityMask = 0;
        DWORD_PTR _uSystemAffinityMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessAffinityMask, &_uSystemAffinityMask));
        const auto _cExpectedIdCount = __CountAffinityMask((KAFFINITY)_uProcessAffinityMask);
        Assert::IsTrue(_cExpectedIdCount != 0);

        ULONG _uRequiredIdCount = 0;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uRequiredIdCount));
        Assert::AreEqual(_cExpectedIdCount, _uRequiredIdCount);
    }

    TEST_METHOD(缓冲区不足_报告缓冲区不足)
    {
        ULONG _uRequiredIdCount = 0;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uRequiredIdCount));
        if (_uRequiredIdCount < 2)
        {
            // 单处理器机器无法构造“缓冲区不足”的场景
            return;
        }

        std::vector<ULONG> _CpuSetIds(_uRequiredIdCount - 1, MAXDWORD);
        Assert::IsFalse(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _CpuSetIds.data(), _uRequiredIdCount - 1, &_uRequiredIdCount));
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError());
    }

    TEST_METHOD(填充Id列表_与亲和性掩码一致)
    {
        DWORD_PTR _uProcessAffinityMask = 0;
        DWORD_PTR _uSystemAffinityMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessAffinityMask, &_uSystemAffinityMask));
        const auto _ExpectedCpuSetIds = __CpuSetIdsFromAffinityMask((KAFFINITY)_uProcessAffinityMask, 0);

        ULONG _uRequiredIdCount = 0;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uRequiredIdCount));

        std::vector<ULONG> _CpuSetIds(_uRequiredIdCount, MAXDWORD);
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _CpuSetIds.data(), _uRequiredIdCount, &_uRequiredIdCount));

        Assert::AreEqual(_uRequiredIdCount, (ULONG)_ExpectedCpuSetIds.size());
        for (ULONG _uIdIndex = 0; _uIdIndex != _uRequiredIdCount; ++_uIdIndex)
        {
            Assert::AreEqual(_ExpectedCpuSetIds[_uIdIndex], _CpuSetIds[_uIdIndex]);
        }
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

    TEST_METHOD(空列表_报告参数错误)
    {
        Assert::IsFalse(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), ::GetLastError());
    }

    TEST_METHOD(跨组Id_报告不支持)
    {
        // 旧接口只能表达一个处理器组，跨组集合必须如实失败
        const ULONG _CpuSetIds[] = {0, _uCpuSetIdBitCount};

        Assert::IsFalse(::SetProcessDefaultCpuSets(::GetCurrentProcess(), _CpuSetIds, _countof(_CpuSetIds)));
        Assert::AreEqual(DWORD(ERROR_NOT_SUPPORTED), ::GetLastError());
    }

    TEST_METHOD(设置当前亲和性_成功)
    {
        DWORD_PTR _uProcessAffinityMask = 0;
        DWORD_PTR _uSystemAffinityMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessAffinityMask, &_uSystemAffinityMask));
        const auto _CpuSetIds = __CpuSetIdsFromAffinityMask((KAFFINITY)_uProcessAffinityMask, 0);

        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), _CpuSetIds.data(), (ULONG)_CpuSetIds.size()));

        // 回设同一份亲和性后，掩码应保持不变
        DWORD_PTR _uNewProcessAffinityMask = 0;
        DWORD_PTR _uNewSystemAffinityMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uNewProcessAffinityMask, &_uNewSystemAffinityMask));
        Assert::AreEqual(_uProcessAffinityMask, _uNewProcessAffinityMask);
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

    TEST_METHOD(查询所需数量_与线程亲和性一致)
    {
        GROUP_AFFINITY _GroupAffinity = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_GroupAffinity));
        const auto _cExpectedIdCount = __CountAffinityMask(_GroupAffinity.Mask);
        Assert::IsTrue(_cExpectedIdCount != 0);

        ULONG _uRequiredIdCount = 0;
        Assert::IsTrue(::GetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0, &_uRequiredIdCount));
        Assert::AreEqual(_cExpectedIdCount, _uRequiredIdCount);
    }

    TEST_METHOD(填充Id列表_与线程亲和性一致)
    {
        GROUP_AFFINITY _GroupAffinity = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_GroupAffinity));
        const auto _ExpectedCpuSetIds = __CpuSetIdsFromAffinityMask(_GroupAffinity.Mask, _GroupAffinity.Group);

        ULONG _uRequiredIdCount = 0;
        Assert::IsTrue(::GetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0, &_uRequiredIdCount));

        std::vector<ULONG> _CpuSetIds(_uRequiredIdCount, MAXDWORD);
        Assert::IsTrue(::GetThreadSelectedCpuSets(::GetCurrentThread(), _CpuSetIds.data(), _uRequiredIdCount, &_uRequiredIdCount));

        Assert::AreEqual(_uRequiredIdCount, (ULONG)_ExpectedCpuSetIds.size());
        for (ULONG _uIdIndex = 0; _uIdIndex != _uRequiredIdCount; ++_uIdIndex)
        {
            Assert::AreEqual(_ExpectedCpuSetIds[_uIdIndex], _CpuSetIds[_uIdIndex]);
        }
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

    TEST_METHOD(空列表_报告参数错误)
    {
        Assert::IsFalse(::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), ::GetLastError());
    }

    TEST_METHOD(跨组Id_报告不支持)
    {
        const ULONG _CpuSetIds[] = {0, _uCpuSetIdBitCount};

        Assert::IsFalse(::SetThreadSelectedCpuSets(::GetCurrentThread(), _CpuSetIds, _countof(_CpuSetIds)));
        Assert::AreEqual(DWORD(ERROR_NOT_SUPPORTED), ::GetLastError());
    }

    TEST_METHOD(回设当前亲和性_成功)
    {
        GROUP_AFFINITY _GroupAffinity = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_GroupAffinity));
        const auto _CpuSetIds = __CpuSetIdsFromAffinityMask(_GroupAffinity.Mask, _GroupAffinity.Group);

        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), _CpuSetIds.data(), (ULONG)_CpuSetIds.size()));
    }
};
}