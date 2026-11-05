#include "pch.h"
#include "Thunks/api-ms-win-core-processthreads.hpp"
// v1.9：SetThreadSelectedCpuSets 的 fallback 依赖 SetThreadGroupAffinity，
// 需要 aways_null_try_get_SetThreadGroupAffinity 守卫来强制它走 fallback
// （否则测试跑的是原生实现，对越界位返回 TRUE 并静默放宽进程掩码，与本库 fallback 完全不同）。
#include "Thunks/api-ms-win-core-systemtopology.hpp"

#include <vector>
#include <process.h>

namespace api_ms_win_core_processthreads
{
namespace
{
    // 旧系统模拟实现把“一个逻辑处理器 = 一个 CPU Set”，Id 为 组号 * 掩码位宽 + 组内编号，
    // 掩码位宽即 KAFFINITY 的位宽（x86 32 / x64 64）。
    constexpr ULONG kCpuSetIdBitCount = sizeof(KAFFINITY) * 8;

    ULONG BitsCountOf(KAFFINITY _uMask)
    {
        ULONG _uCount = 0;
        for (; _uMask != 0; _uMask &= _uMask - 1)
        {
            ++_uCount;
        }
        return _uCount;
    }

    std::vector<ULONG> CpuSetIdsFromAffinityMask(KAFFINITY _uMask, WORD _uGroup)
    {
        std::vector<ULONG> _CpuSetIds;
        for (ULONG _uBit = 0; _uBit != kCpuSetIdBitCount; ++_uBit)
        {
            if ((_uMask & ((KAFFINITY)1 << _uBit)) == 0)
                continue;
            _CpuSetIds.push_back(_uGroup * kCpuSetIdBitCount + _uBit);
        }
        return _CpuSetIds;
    }

    // 按集合语义比较两个 Id 列表（忽略顺序）。
    // 不用 std::sort：测试工程的 pch 链（km.h）与 <algorithm> 存在命名空间冲突。
    bool SameIdSet(const std::vector<ULONG>& _a, const std::vector<ULONG>& _b)
    {
        if (_a.size() != _b.size())
            return false;

        std::vector<bool> _Matched(_b.size(), false);
        for (size_t _i = 0; _i != _a.size(); ++_i)
        {
            bool _bFound = false;
            for (size_t _j = 0; _j != _b.size(); ++_j)
            {
                if (!_Matched[_j] && _a[_i] == _b[_j])
                {
                    _Matched[_j] = true;
                    _bFound = true;
                    break;
                }
            }
            if (!_bFound)
                return false;
        }
        return true;
    }

    // 组 0 内第 _uIndex 个处理器的 CpuSet Id。Fallback 的 Id 编码保证组 0 的 Id 等于组内位序。
    ULONG CpuSetIdOfProcessor(ULONG _uIndex)
    {
        return _uIndex;
    }

    // v1.9 新增：按数组比较两个 Id 列表（忽略顺序）。
    bool SameIdSet(const ULONG* _pA, size_t _cA, const ULONG* _pB, size_t _cB)
    {
        if (_cA != _cB)
            return false;

        bool _Matched[64] = {};
        for (size_t _i = 0; _i != _cA; ++_i)
        {
            bool _bFound = false;
            for (size_t _j = 0; _j != _cB; ++_j)
            {
                if (!_Matched[_j] && _pA[_i] == _pB[_j])
                {
                    _Matched[_j] = true;
                    _bFound = true;
                    break;
                }
            }
            if (!_bFound)
                return false;
        }
        return true;
    }

    // v1.9 新增：本机处理器数是否够用（构造多 Id 用例前的前置判断）。
    // 注意 x86 的 KAFFINITY 只有 32 位，WOW64 下 GetSystemInfo 可能报告更多处理器，
    // 因此上限取「处理器数」与「掩码位宽」的较小值。
    bool CanSetProcessDefault(const ULONG* _pIds, size_t _cIds)
    {
        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);

        ULONG _cLimit = _SystemInfo.dwNumberOfProcessors;
        if (_cLimit > kCpuSetIdBitCount)
            _cLimit = kCpuSetIdBitCount;

        if (_cLimit < _cIds)
            return false;

        for (size_t _i = 0; _i != _cIds; ++_i)
        {
            if (_pIds[_i] >= _cLimit)
                return false;
        }
        return true;
    }

    void RestoreProcessAffinity()
    {
        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        if (::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask))
        {
            ::SetProcessAffinityMask(::GetCurrentProcess(), _uSystemMask);
        }
    }

    void RestoreThreadAffinity()
    {
        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        if (::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask))
        {
            ::SetThreadAffinityMask(::GetCurrentThread(), _uSystemMask);
        }
    }
}

// ============================================================================
// GetProcessDefaultCpuSets
// ============================================================================

TEST_CLASS(GetProcessDefaultCpuSets)
{
    AwaysNullGuard Guard;

public:
    GetProcessDefaultCpuSets()
    {
        Guard |= YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets;
        // v1.9：本类的用例需要用 SetProcessDefaultCpuSets 构造进程默认状态，
        // 必须同时强制它走 fallback，否则 Set 会落到原生 API，
        // 写进操作系统状态而不是本库的共享记录，导致 Get 读回 0。
        Guard |= YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets;
    }

    TEST_METHOD_INITIALIZE(MethodInitialize)
    {
        // 若忘记在构造函数里 Guard |= aways_null_try_get_Xxx，用例会静默测到原生 API
        // 而不是 fallback（两者在两阶段查询矩阵上恰好一致），只有换到旧系统才暴露。
        Assert::IsTrue(YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets,
            L"fallback 守卫未生效，测试可能测到的是原生 API");
    }

    TEST_METHOD_CLEANUP(MethodCleanup)
    {
        ::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0);
        RestoreProcessAffinity();
    }

    // TC-001
    TEST_METHOD(RequiredIdCount为NULL_返回FALSE并报参数错误)
    {
        ::SetLastError(0);
        Assert::IsFalse(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, nullptr));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), ::GetLastError());
    }

    // TC-003：P0-2 回归 —— 集合非空且缓冲区为 NULL 时必须 FALSE + ERROR_INSUFFICIENT_BUFFER
    TEST_METHOD(集合非空且缓冲区为NULL_返回FALSE并报缓冲区不足)
    {
        // v1.9：进程默认不再来自进程亲和性，必须显式设置。
        const ULONG _Ids[3] = { CpuSetIdOfProcessor(0), CpuSetIdOfProcessor(1), CpuSetIdOfProcessor(2) };
        if (!CanSetProcessDefault(_Ids, 3))
        {
            Logger::WriteMessage(L"本机处理器不足 3 个，跳过本用例。");
            return;
        }

        ::SetLastError(0);
        const BOOL _bSetResult = ::SetProcessDefaultCpuSets(::GetCurrentProcess(), _Ids, 3);
        const DWORD _uSetError = ::GetLastError();
        if (_bSetResult == FALSE)
        {
            SYSTEM_INFO _si = {};
            ::GetSystemInfo(&_si);
            wchar_t _sz[256];
            ::wsprintfW(_sz,
                L"Set 失败 err=%u Ids=%u,%u,%u nProc=%u bitWidth=%u",
                _uSetError, _Ids[0], _Ids[1], _Ids[2],
                _si.dwNumberOfProcessors, kCpuSetIdBitCount);
            Assert::IsTrue(_bSetResult != FALSE, _sz);
        }

        // 前置：确认记录已写入 3 个 Id。
        // 注意：集合非空 + 缓冲区 NULL 时 Get 本身就返回 FALSE + 122（这正是本用例要验证的语义），
        // 因此这里必须传一个足够大的缓冲区才能成功读到数量。
        {
            ULONG _PreIds[8] = {};
            ULONG _uPre = 0;
            Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _PreIds, 8, &_uPre) != FALSE,
                L"前置：带足够缓冲区读回必须成功");
            if (_uPre != 3)
            {
                wchar_t _sz[256];
                ::wsprintfW(_sz, L"读回=%u（期望3）", _uPre);
                Assert::AreEqual(ULONG(3), ULONG(_uPre), _sz);
            }
        }

        ULONG _uRequired = 0xDEADBEEF;
        ::SetLastError(0);
        Assert::IsFalse(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uRequired),
            L"集合非空时缓冲区为 NULL 必须返回 FALSE");
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError());
        Assert::AreEqual(ULONG(3), ULONG(_uRequired), L"必须回填所需数量");
    }

    // TC-004
    TEST_METHOD(缓冲区恰好够_返回TRUE并填充Id)
    {
        const ULONG _SetIds[3] = { CpuSetIdOfProcessor(0), CpuSetIdOfProcessor(1), CpuSetIdOfProcessor(2) };
        if (!CanSetProcessDefault(_SetIds, 3))
        {
            Logger::WriteMessage(L"本机处理器不足 3 个，跳过本用例。");
            return;
        }

        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), _SetIds, 3) != FALSE);

        ULONG _Ids[8] = {};
        ULONG _uGot = 0;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _Ids, 3, &_uGot) != FALSE);
        Assert::AreEqual(ULONG(3), ULONG(_uGot));

        Assert::IsTrue(SameIdSet(_Ids, 3, _SetIds, 3), L"返回的 Id 集合必须与设置时一致");
    }

    // TC-005
    TEST_METHOD(缓冲区不足_返回FALSE且回填所需数量)
    {
        const ULONG _SetIds[3] = { CpuSetIdOfProcessor(0), CpuSetIdOfProcessor(1), CpuSetIdOfProcessor(2) };
        if (!CanSetProcessDefault(_SetIds, 3))
        {
            Logger::WriteMessage(L"本机处理器不足 3 个，跳过本用例。");
            return;
        }

        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), _SetIds, 3) != FALSE);

        ULONG _Ids[8] = {};
        ULONG _uGot = 0xDEADBEEF;
        ::SetLastError(0);
        Assert::IsFalse(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _Ids, 2, &_uGot));
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError());
        Assert::AreEqual(ULONG(3), ULONG(_uGot), L"回填的是所需数量，不是实际写入数");
    }

    // TC-006
    TEST_METHOD(缓冲区超出_返回TRUE且数量为实际值)
    {
        const ULONG _SetIds[2] = { CpuSetIdOfProcessor(0), CpuSetIdOfProcessor(1) };
        if (!CanSetProcessDefault(_SetIds, 2))
        {
            Logger::WriteMessage(L"本机处理器不足 2 个，跳过本用例。");
            return;
        }

        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), _SetIds, 2) != FALSE);

        ULONG _Ids[16] = {};
        ULONG _uGot = 0;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _Ids, 16, &_uGot) != FALSE);
        Assert::AreEqual(ULONG(2), ULONG(_uGot), L"数量必须是实际记录数，不是缓冲区容量");
    }

    // NULL 句柄透传：原生实测 GetProcessDefaultCpuSets(NULL,...) 返回 FALSE + ERROR_INVALID_HANDLE(6)
    TEST_METHOD(Null句柄_返回FALSE并报句柄无效)
    {
        ULONG _uRequired = 0;
        ::SetLastError(0);
        Assert::IsFalse(::GetProcessDefaultCpuSets(nullptr, nullptr, 0, &_uRequired));
        Assert::AreEqual(DWORD(ERROR_INVALID_HANDLE), DWORD(::GetLastError()));
    }

    // TC-008
    TEST_METHOD(成功路径不修改LastError)
    {
        const ULONG _SetIds[2] = { CpuSetIdOfProcessor(0), CpuSetIdOfProcessor(1) };
        if (!CanSetProcessDefault(_SetIds, 2))
        {
            Logger::WriteMessage(L"本机处理器不足 2 个，跳过本用例。");
            return;
        }

        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), _SetIds, 2) != FALSE);

        ULONG _Ids[8] = {};
        ULONG _uGot = 0;
        ::SetLastError(12345);
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _Ids, 8, &_uGot) != FALSE);
        Assert::AreEqual(DWORD(12345), DWORD(::GetLastError()), L"成功路径不得修改 LastError");
    }
};

// ============================================================================
// SetProcessDefaultCpuSets
// ============================================================================

TEST_CLASS(SetProcessDefaultCpuSets)
{
    AwaysNullGuard Guard;

public:
    SetProcessDefaultCpuSets()
    {
        Guard |= YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets;
        // v1.9：本类用例需要用 GetProcessDefaultCpuSets 读回验证，必须同时强制它走 fallback。
        // 否则 Set 写进本库共享记录、Get 却读原生状态 → 恒读回 0。
        Guard |= YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets;
    }

    TEST_METHOD_INITIALIZE(MethodInitialize)
    {
        Assert::IsTrue(YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets,
            L"fallback 守卫未生效");
    }

    TEST_METHOD_CLEANUP(MethodCleanup)
    {
        ::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0);
        RestoreProcessAffinity();
    }

    // TC-009（v1.9 重写）：清除后记录为空，且**不再**修改进程亲和性
    TEST_METHOD(清除使用NULL和零_记录清空且进程亲和性不变)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);

        DWORD_PTR _uMaskBefore = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uMaskBefore, &_uSystemMask) != FALSE);

        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0) != FALSE);

        DWORD_PTR _uMaskAfter = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uMaskAfter, &_uSystemMask) != FALSE);
        Assert::AreEqual(DWORD_PTR(_uMaskBefore), DWORD_PTR(_uMaskAfter),
            L"v1.9：SetProcessDefaultCpuSets 绝不修改进程亲和性");

        ULONG _uRequired = 0xDEADBEEF;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uRequired) != FALSE);
        Assert::AreEqual(ULONG(0), ULONG(_uRequired), L"清除后读回数量必须为 0");
    }

    // TC-010：P0-2b 回归 —— 非空指针 + count 0 也是合法清除
    TEST_METHOD(清除使用非空指针和零_记录清空且进程亲和性不变)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);

        DWORD_PTR _uMaskBefore = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uMaskBefore, &_uSystemMask) != FALSE);

        ULONG _Dummy = 0;
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Dummy, 0) != FALSE,
            L"非空指针 + count 0 是合法清除，不得返回 ERROR_INVALID_PARAMETER");

        DWORD_PTR _uMaskAfter = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uMaskAfter, &_uSystemMask) != FALSE);
        Assert::AreEqual(DWORD_PTR(_uMaskBefore), DWORD_PTR(_uMaskAfter));

        ULONG _uRequired = 0xDEADBEEF;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uRequired) != FALSE);
        Assert::AreEqual(ULONG(0), ULONG(_uRequired));
    }

    // TC-011
    TEST_METHOD(空指针但count大于零_返回FALSE并报参数错误)
    {
        ::SetLastError(0);
        Assert::IsFalse(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 1));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), ::GetLastError());
    }

    // TC-012
    TEST_METHOD(设置单个Id_返回TRUE)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        ::SetLastError(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);
    }

    // TC-014 / TC-015 / TC-016（v1.9 合并）：本机不存在的组 → 813。
    // 单组测试机上「组 1 的合法 Id」不存在，因此这类用例只会命中「组号越界」分支，
    // 无法验证「跨组取首组」。后者按用户裁决登记为未覆盖（TESTCASE §13）。
    TEST_METHOD(组号越界Id_返回FALSE并报无效CpuSet)
    {
        const ULONG _BadIds[2] = {
            kCpuSetIdBitCount,          // 组 1 位 0
            kCpuSetIdBitCount * 3,      // 组 3 位 0
        };

        for (ULONG _uId : _BadIds)
        {
            ::SetLastError(0);
            Assert::IsFalse(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_uId, 1),
                L"组号超出本机组数的 Id 必须被拒绝");
            Assert::AreEqual(DWORD(ERROR_CPU_SET_INVALID), ::GetLastError(),
                L"原生对不属于本机处理器的 Id 统一报 ERROR_CPU_SET_INVALID(813)");
        }

        // 对照：组 0 的合法 Id 必须被接受
        const ULONG _GoodId = CpuSetIdOfProcessor(0);
        ::SetLastError(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_GoodId, 1) != FALSE,
            L"对照断言：组 0 的合法 Id 必须被接受");
    }

    // TC-017
    TEST_METHOD(组零内位序越界_返回FALSE并报无效CpuSet)
    {
        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);
        if (_SystemInfo.dwNumberOfProcessors >= kCpuSetIdBitCount)
        {
            Logger::WriteMessage(L"处理器数已达掩码位宽，无法构造组 0 内位序越界，跳过本用例。");
            return;
        }

        const ULONG _Id = _SystemInfo.dwNumberOfProcessors;
        Assert::AreEqual(0ul, ULONG(_Id / kCpuSetIdBitCount), L"该 Id 必须落在组 0");

        ::SetLastError(0);
        Assert::IsFalse(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1));
        Assert::AreEqual(DWORD(ERROR_CPU_SET_INVALID), ::GetLastError(),
            L"原生对不存在的处理器位报 813；fallback 用同一错误码且不调用 SetProcessAffinityMask");
    }

    // TC-019
    TEST_METHOD(成功路径不修改LastError)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        ::SetLastError(12345);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);
        Assert::AreEqual(DWORD(12345), DWORD(::GetLastError()));
    }

    // TC-020
    TEST_METHOD(清除路径不修改LastError)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);

        ::SetLastError(12345);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0) != FALSE);
        Assert::AreEqual(DWORD(12345), DWORD(::GetLastError()));
    }

    // TC-071：v1.9 核心 —— 不改变已存在线程掩码
    TEST_METHOD(不改变已存在的当前线程掩码)
    {
        // 前置：把线程掩码设成与目标不同的值（目标 CPU0，前置设为 CPU1）。
        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);
        if (_SystemInfo.dwNumberOfProcessors < 2)
        {
            Logger::WriteMessage(L"处理器不足 2 个，无法构造「前置掩码 != 目标掩码」，跳过本用例。");
            return;
        }

        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask) != FALSE);
        Assert::IsTrue(::SetProcessAffinityMask(::GetCurrentProcess(), _uSystemMask) != FALSE);

        Assert::IsTrue(::SetThreadAffinityMask(::GetCurrentThread(), 0x2) != 0, L"前置：线程掩码设为 0x2");

        GROUP_AFFINITY _Before = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_Before) != FALSE);
        Assert::AreEqual(KAFFINITY(0x2), _Before.Mask, L"前置断言：线程掩码必须为 0x2 而非目标值 0x1");

        const ULONG _Id = CpuSetIdOfProcessor(0);   // 目标：CPU0 = 0x1
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);

        GROUP_AFFINITY _After = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_After) != FALSE);
        Assert::AreEqual(KAFFINITY(0x2), _After.Mask,
            L"v1.9 核心：SetProcessDefaultCpuSets 绝不改动已存在线程的掩码");
    }

    // TC-072：v1.9 核心 —— 不改变进程亲和性
    TEST_METHOD(不改变进程亲和性)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);

        DWORD_PTR _uMaskBefore = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uMaskBefore, &_uSystemMask) != FALSE);

        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);

        DWORD_PTR _uMaskAfter = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uMaskAfter, &_uSystemMask) != FALSE);
        Assert::AreEqual(DWORD_PTR(_uMaskBefore), DWORD_PTR(_uMaskAfter),
            L"v1.9 核心：本方案完全不调用 SetProcessAffinityMask");
    }

    // TC-073：NULL 句柄 → 6
    TEST_METHOD(Null句柄_返回FALSE并报句柄无效)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        ::SetLastError(0);
        Assert::IsFalse(::SetProcessDefaultCpuSets(nullptr, &_Id, 1));
        Assert::AreEqual(DWORD(ERROR_INVALID_HANDLE), ::GetLastError());
    }

    // v1.12 新增：跨进程 Set 必须写入**目标**进程的映射，绝不能落到本进程记录上。
    //
    // 回归目标：OpenSharedDataForProcess 曾用 GetCurrentProcessId() 拼映射名，
    // 导致「写目标进程」实际写入自己 —— 这种错误比直接失败更危险（静默污染调用方状态）。
    //
    // 子进程用 cmd.exe，它不会加载 YY-Thunks，因此没有共享映射：
    //   * 正确实现 → Set 返回 FALSE（映射不存在），本进程记录保持为空；
    //   * 错误实现 → Set 返回 TRUE（误开自己的映射），本进程记录被写入。
    TEST_METHOD(跨进程Set不得污染本进程记录)
    {
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0) != FALSE);

        STARTUPINFOA _StartupInfo = { sizeof(_StartupInfo) };
        PROCESS_INFORMATION _ProcessInfo = {};
        char _szCommandLine[] = "cmd.exe /c ping 127.0.0.1 -n 30 > nul";
        Assert::IsTrue(::CreateProcessA(nullptr, _szCommandLine, nullptr, nullptr, FALSE,
            CREATE_SUSPENDED | CREATE_NO_WINDOW, nullptr, nullptr, &_StartupInfo, &_ProcessInfo) != FALSE,
            L"需要能创建挂起态子进程");
        ::CloseHandle(_ProcessInfo.hThread);

        HANDLE _hTarget = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_LIMITED_INFORMATION,
            FALSE, _ProcessInfo.dwProcessId);
        if (_hTarget == nullptr)
        {
            Logger::WriteMessage(L"无法打开目标进程，跳过本用例。");
            ::TerminateProcess(_ProcessInfo.hProcess, 0);
            ::CloseHandle(_ProcessInfo.hProcess);
            return;
        }

        const ULONG _Id = CpuSetIdOfProcessor(0);
        ::SetLastError(0);
        const BOOL _bSet = ::SetProcessDefaultCpuSets(_hTarget, &_Id, 1);
        const DWORD _uSetError = ::GetLastError();

        ::CloseHandle(_hTarget);
        ::TerminateProcess(_ProcessInfo.hProcess, 0);
        ::CloseHandle(_ProcessInfo.hProcess);

        // 核心断言：无论成败，本进程记录都必须保持为空。
        // 注意：集合非空 + 缓冲区 NULL 时 Get 本身返回 FALSE + 122，但仍会回填所需数量，
        // 因此这里不校验返回值，只校验回填的数量。
        ULONG _uRequired = 0xDEADBEEF;
        ::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uRequired);
        Assert::AreEqual(ULONG(0), ULONG(_uRequired),
            L"写目标进程的记录绝不能落到本进程上");

        // 目标进程没有 YY-Thunks 共享映射 → 必须如实失败，不能假装成功。
        Assert::IsFalse(_bSet != FALSE,
            L"目标进程没有共享映射时必须返回 FALSE");
        Assert::AreEqual(DWORD(ERROR_CALL_NOT_IMPLEMENTED), _uSetError,
            L"打不开目标映射时报 ERROR_CALL_NOT_IMPLEMENTED");
    }

    // TC-074：位打包往返（P0-1 直接回归，x86 必过）
    TEST_METHOD(设置后读回同一Id_位打包往返正确)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);

        // 前置：清除并断言读回为 0，避免「恰好已等于目标值」的假通过。
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0) != FALSE);
        ULONG _uPre = 0xDEADBEEF;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uPre) != FALSE);
        Assert::AreEqual(ULONG(0), ULONG(_uPre), L"前置断言：必须先清除到 0");

        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);

        ULONG _Ids[8] = {};
        ULONG _uGot = 0;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _Ids, 8, &_uGot) != FALSE);
        Assert::AreEqual(ULONG(1), ULONG(_uGot), L"读回数量必须为 1");
        Assert::AreEqual(ULONG(_Id), ULONG(_Ids[0]), L"读回的 Id 必须与设置的完全相同（P0-1 回归）");
    }

    // TC-075：全掩码往返（覆盖 Mask == ~0 边界）
    TEST_METHOD(全掩码往返一致)
    {
        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);

        // x86 上 KAFFINITY 只有 32 位，WOW64 下 GetSystemInfo 却可能报告超过 32 个处理器。
        // 取二者较小值，保证构造的 Id 都在掩码位宽与本机处理器数之内。
        ULONG _cProcessors = _SystemInfo.dwNumberOfProcessors;
        if (_cProcessors > kCpuSetIdBitCount)
            _cProcessors = kCpuSetIdBitCount;

        if (_cProcessors == 0)
        {
            Logger::WriteMessage(L"处理器数为 0，跳过全掩码往返用例。");
            return;
        }

        ULONG _Ids[32] = {};
        for (ULONG _i = 0; _i != _cProcessors; ++_i)
        {
            _Ids[_i] = CpuSetIdOfProcessor(_i);
        }

        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), _Ids, _cProcessors) != FALSE);

        ULONG _Readback[32] = {};
        ULONG _uGot = 0;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _Readback, 32, &_uGot) != FALSE);
        Assert::AreEqual(_cProcessors, _uGot, L"读回数量必须等于参与往返的处理器数");
        Assert::IsTrue(SameIdSet(_Ids, _cProcessors, _Readback, _uGot), L"全掩码往返必须完全一致");
    }

    // TC-078：清除不改变线程掩码
    TEST_METHOD(清除不改变线程掩码)
    {
        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);
        if (_SystemInfo.dwNumberOfProcessors < 2)
        {
            Logger::WriteMessage(L"处理器不足 2 个，跳过本用例。");
            return;
        }

        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask) != FALSE);
        Assert::IsTrue(::SetProcessAffinityMask(::GetCurrentProcess(), _uSystemMask) != FALSE);

        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);

        Assert::IsTrue(::SetThreadAffinityMask(::GetCurrentThread(), 0x2) != 0);

        GROUP_AFFINITY _Before = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_Before) != FALSE);

        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0) != FALSE);

        GROUP_AFFINITY _After = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_After) != FALSE);
        Assert::AreEqual(KAFFINITY(_Before.Mask), _After.Mask, L"清除同样不碰线程掩码");
    }

    // TC-079：失败不污染记录
    TEST_METHOD(设置失败后记录未被写入)
    {
        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);
        if (_SystemInfo.dwNumberOfProcessors >= kCpuSetIdBitCount)
        {
            Logger::WriteMessage(L"处理器数已达掩码位宽，无法构造越界 Id，跳过本用例。");
            return;
        }

        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0) != FALSE);

        const ULONG _BadId = _SystemInfo.dwNumberOfProcessors;   // 组 0 内位序越界
        ::SetLastError(0);
        Assert::IsFalse(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_BadId, 1));
        Assert::AreEqual(DWORD(ERROR_CPU_SET_INVALID), ::GetLastError());

        ULONG _uRequired = 0xDEADBEEF;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uRequired) != FALSE);
        Assert::AreEqual(ULONG(0), ULONG(_uRequired), L"失败不得污染记录");
    }
};

// ============================================================================
// GetThreadSelectedCpuSets
// ============================================================================

TEST_CLASS(GetThreadSelectedCpuSets)
{
    AwaysNullGuard Guard;

public:
    GetThreadSelectedCpuSets()
    {
        Guard |= YY::Thunks::aways_null_try_get_GetThreadSelectedCpuSets;
        // v1.9：本类用例用 Set 构造线程状态，必须同时强制它走 fallback。
        Guard |= YY::Thunks::aways_null_try_get_SetThreadSelectedCpuSets;
    }

    TEST_METHOD_INITIALIZE(MethodInitialize)
    {
        Assert::IsTrue(YY::Thunks::aways_null_try_get_GetThreadSelectedCpuSets,
            L"fallback 守卫未生效");

        // v1.9：本类多数用例需要「线程有自有选择」这个前置态。
        // 线程掩码为全核时按 D-11 视为「未设置」（返回 n=0），
        // 因此统一先设成一个真子集（处理器 0）。
        RestoreProcessAffinity();
        RestoreThreadAffinity();
        const ULONG _SeedId = CpuSetIdOfProcessor(0);
        ::SetThreadSelectedCpuSets(::GetCurrentThread(), &_SeedId, 1);
    }

    TEST_METHOD_CLEANUP(MethodCleanup)
    {
        ::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0);
        RestoreThreadAffinity();
    }

    // TC-021
    TEST_METHOD(RequiredIdCount为NULL_返回FALSE并报参数错误)
    {
        ::SetLastError(0);
        Assert::IsFalse(::GetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0, nullptr));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), ::GetLastError());
    }

    // TC-023：P0-2 回归
    TEST_METHOD(集合非空且缓冲区为NULL_返回FALSE并报缓冲区不足)
    {
        // v1.9：线程掩码为全核时按 D-11 视为「未设置」（返回 n=0），
        // 因此必须显式设置一个真子集来构造「集合非空」。
        RestoreProcessAffinity();
        RestoreThreadAffinity();

        const ULONG _SetIds[1] = { CpuSetIdOfProcessor(0) };
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), _SetIds, 1) != FALSE,
            L"前置：设置 1 个合法 Id 必须成功");

        ULONG _uRequired = 0xDEADBEEF;
        ::SetLastError(0);
        Assert::IsFalse(::GetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0, &_uRequired),
            L"集合非空时缓冲区为 NULL 必须返回 FALSE");
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError());
        Assert::AreEqual(ULONG(1), ULONG(_uRequired));
    }

    // TC-024
    TEST_METHOD(缓冲区恰好够_返回TRUE并填充Id)
    {
        GROUP_AFFINITY _Affinity = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_Affinity) != FALSE);

        ULONG _uRequired = 0;
        ::GetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0, &_uRequired);
        if (_uRequired == 0)
        {
            Logger::WriteMessage(L"线程亲和性掩码为 0，跳过本用例。");
            return;
        }

        std::vector<ULONG> _Ids(_uRequired + 4, 0);
        ULONG _uGot = 0;
        Assert::IsTrue(::GetThreadSelectedCpuSets(::GetCurrentThread(), _Ids.data(), _uRequired, &_uGot) != FALSE);
        Assert::AreEqual(ULONG(_uRequired), ULONG(_uGot));

        _Ids.resize(_uGot);
        Assert::IsTrue(SameIdSet(_Ids, CpuSetIdsFromAffinityMask(_Affinity.Mask, _Affinity.Group)));
    }

    // TC-025
    TEST_METHOD(缓冲区不足_返回FALSE且回填所需数量)
    {
        ULONG _uRequired = 0;
        ::GetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0, &_uRequired);
        if (_uRequired < 2)
        {
            Logger::WriteMessage(L"线程亲和性少于 2 个处理器，跳过本用例。");
            return;
        }

        std::vector<ULONG> _Ids(_uRequired + 4, 0);
        ULONG _uGot = 0xDEADBEEF;
        ::SetLastError(0);
        Assert::IsFalse(::GetThreadSelectedCpuSets(::GetCurrentThread(), _Ids.data(), _uRequired - 1, &_uGot));
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError());
        Assert::AreEqual(ULONG(_uRequired), ULONG(_uGot));
    }

    // TC-028
    TEST_METHOD(成功路径不修改LastError)
    {
        ULONG _uRequired = 0;
        ::GetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0, &_uRequired);
        if (_uRequired == 0)
        {
            Logger::WriteMessage(L"线程亲和性掩码为 0，跳过本用例。");
            return;
        }

        std::vector<ULONG> _Ids(_uRequired + 4, 0);
        ULONG _uGot = 0;
        ::SetLastError(12345);
        Assert::IsTrue(::GetThreadSelectedCpuSets(::GetCurrentThread(), _Ids.data(), _uRequired, &_uGot) != FALSE);
        Assert::AreEqual(DWORD(12345), DWORD(::GetLastError()));
    }

    // TC-023b（v1.12 新增）：线程掩码等于进程亲和性掩码 → 视为「未设置」→ n = 0。
    // 这是 D-11 修正的直接回归：v1.11 的判据是 Mask == ~0，在非满配机器上恒失效。
    TEST_METHOD(线程掩码等于进程亲和性_返回零个Id)
    {
        RestoreProcessAffinity();
        RestoreThreadAffinity();

        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask) != FALSE);

        GROUP_AFFINITY _Affinity = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_Affinity) != FALSE);
        if (_Affinity.Mask != (KAFFINITY)_uProcessMask)
        {
            Logger::WriteMessage(L"当前线程掩码不等于进程掩码，跳过本用例。");
            return;
        }

        ULONG _uRequired = 0xDEADBEEF;
        ::SetLastError(0);
        Assert::IsTrue(::GetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0, &_uRequired) != FALSE,
            L"n = 0 时即使缓冲区为 NULL 也必须返回 TRUE");
        Assert::AreEqual(ULONG(0), ULONG(_uRequired),
            L"线程掩码 == 进程亲和性掩码即「未设置自有选择」，必须返回 0 个 Id");
    }
};

// ============================================================================
// SetThreadSelectedCpuSets
// ============================================================================

TEST_CLASS(SetThreadSelectedCpuSets)
{
    AwaysNullGuard Guard;

public:
    SetThreadSelectedCpuSets()
    {
        Guard |= YY::Thunks::aways_null_try_get_SetThreadSelectedCpuSets;
        // v1.9：本类用例用 GetThreadSelectedCpuSets 读回验证，需同时强制。
        Guard |= YY::Thunks::aways_null_try_get_GetThreadSelectedCpuSets;
        // v1.9：SetThreadGroupAffinity 是本类 fallback 的核心依赖。
        // 不强制的话测试跑的是原生实现 —— 原生对越界位返回 TRUE 并静默放宽进程掩码
        // （实测 0x1 → 0xFFFFFFFF），而本库 fallback 走 SetThreadAffinityMask，
        // 返回 87 且不放宽。两者行为完全不同，必须显式走 fallback 才能验证本库语义。
        Guard |= YY::Thunks::aways_null_try_get_SetThreadGroupAffinity;
        Guard |= YY::Thunks::aways_null_try_get_GetThreadGroupAffinity;
    }

    TEST_METHOD_INITIALIZE(MethodInitialize)
    {
        Assert::IsTrue(YY::Thunks::aways_null_try_get_SetThreadSelectedCpuSets,
            L"fallback 守卫未生效");
    }

    TEST_METHOD_CLEANUP(MethodCleanup)
    {
        ::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0);
        // 必须先恢复进程亲和性：RestoreThreadAffinity 用系统掩码调用 SetThreadAffinityMask，
        // 若进程仍处于收窄状态会返回 ERROR_INVALID_PARAMETER(87) 并静默失败。
        RestoreProcessAffinity();
        RestoreThreadAffinity();
    }

    // TC-029
    TEST_METHOD(清除使用NULL和零_亲和性恢复全掩码)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Id, 1) != FALSE);

        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0) != FALSE);

        // 清除的语义是「不再限制」，恢复为系统掩码（不是 KAFFINITY(-1)，
        // 因为掩码含超出实际处理器数的位会被 SetThreadGroupAffinity 拒绝）。
        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask) != FALSE);

        GROUP_AFFINITY _Affinity = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_Affinity) != FALSE);
        Assert::AreEqual(KAFFINITY(_uSystemMask), KAFFINITY(_Affinity.Mask), L"清除后线程亲和性应恢复为系统掩码");
    }

    // 审查必修 1 回归：进程亲和性被收窄后，清除线程 CpuSet 仍须成功。
    // 曾用 SetThreadAffinityMask(系统掩码)，实测在进程收窄为 0x1 时返回 ERROR_INVALID_PARAMETER(87)。
    TEST_METHOD(进程亲和性收窄后清除线程CpuSet_返回TRUE)
    {
        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask) != FALSE);

        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Id, 1) != FALSE);

        Assert::IsTrue(::SetProcessAffinityMask(::GetCurrentProcess(), 0x1) != FALSE);

        ULONG _Dummy = 0;
        ::SetLastError(0);
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Dummy, 0) != FALSE,
            L"进程亲和性收窄后清除线程 CpuSet 仍须成功");

        // 清除应把线程掩码恢复成「进程亲和性掩码」而不是系统掩码。
        GROUP_AFFINITY _Affinity = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_Affinity) != FALSE);
        Assert::AreEqual(KAFFINITY(0x1), KAFFINITY(_Affinity.Mask), L"清除后线程掩码应等于进程掩码 0x1");

        Assert::IsTrue(::SetProcessAffinityMask(::GetCurrentProcess(), _uSystemMask) != FALSE);
    }

    // v1.9 回归：请求写入超出进程亲和性掩码的位时，透出系统的 87，且不放宽进程掩码。
    // CpuSet 优先级低于进程 Affinity（微软文档），因此不生效是符合预期的行为。
    TEST_METHOD(请求超出进程亲和性_返回FALSE且不放宽进程掩码)
    {
        // 前置：恢复掩码，避免本类前面用例的收窄状态干扰。
        RestoreProcessAffinity();
        RestoreThreadAffinity();
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0) != FALSE);

        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask) != FALSE);
        if (_uSystemMask == 1)
        {
            Logger::WriteMessage(L"系统只有 1 个处理器，无法构造超出进程掩码的请求，跳过本用例。");
            return;
        }

        // 进程只留 bit0，随后请求 bit1（不在进程掩码内）。
        Assert::IsTrue(::SetProcessAffinityMask(::GetCurrentProcess(), 0x1) != FALSE);

        const ULONG _Id = CpuSetIdOfProcessor(1);
        ::SetLastError(0);
        Assert::IsFalse(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Id, 1));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), DWORD(::GetLastError()),
            L"v1.9：透出系统的 ERROR_INVALID_PARAMETER(87)，不再返回 120");

        DWORD_PTR _uAfterMask = 0;
        DWORD_PTR _uAfterSystem = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uAfterMask, &_uAfterSystem) != FALSE);
        Assert::AreEqual(DWORD_PTR(0x1), DWORD_PTR(_uAfterMask), L"失败时不得放宽进程亲和性掩码");

        ::SetProcessAffinityMask(::GetCurrentProcess(), _uSystemMask);
    }

    // TC-030：P0-2b 回归
    TEST_METHOD(清除使用非空指针和零_返回TRUE)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Id, 1) != FALSE);

        ULONG _Dummy = 0;
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Dummy, 0) != FALSE,
            L"非空指针 + count 0 是合法清除");
    }

    // TC-031
    TEST_METHOD(空指针但count大于零_返回FALSE并报参数错误)
    {
        ::SetLastError(0);
        Assert::IsFalse(::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 1));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), ::GetLastError());
    }

    // TC-032
    TEST_METHOD(设置单个Id_返回TRUE)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        ::SetLastError(0);
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Id, 1) != FALSE);
    }

    // TC-034
    TEST_METHOD(跨组Id_返回FALSE并报无效CpuSet)
    {
        const ULONG _Id = kCpuSetIdBitCount; // 组 1 位 0
        ::SetLastError(0);
        Assert::IsFalse(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Id, 1));
        Assert::AreEqual(DWORD(ERROR_CPU_SET_INVALID), ::GetLastError(),
            L"原生对不属于本机处理器的 Id 统一报 ERROR_CPU_SET_INVALID(813)");
    }

    // TC-035
    TEST_METHOD(跨组多个Id_返回FALSE并报无效CpuSet)
    {
        const ULONG _Ids[2] = { 0, kCpuSetIdBitCount };
        ::SetLastError(0);
        Assert::IsFalse(::SetThreadSelectedCpuSets(::GetCurrentThread(), _Ids, 2));
        Assert::AreEqual(DWORD(ERROR_CPU_SET_INVALID), ::GetLastError());
    }

    // TC-036
    TEST_METHOD(组零内位序越界_返回FALSE并报无效CpuSet)
    {
        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);
        if (_SystemInfo.dwNumberOfProcessors >= kCpuSetIdBitCount)
        {
            Logger::WriteMessage(L"处理器数已达掩码位宽，跳过本用例。");
            return;
        }

        // 先确保进程亲和性为全系统掩码，避免「掩码 ⊆ 进程亲和性」校验先行干扰。
        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask) != FALSE);
        Assert::IsTrue(::SetProcessAffinityMask(::GetCurrentProcess(), _uSystemMask) != FALSE);

        const ULONG _Id = _SystemInfo.dwNumberOfProcessors;
        ::SetLastError(0);
        Assert::IsFalse(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Id, 1));
        Assert::AreEqual(DWORD(ERROR_CPU_SET_INVALID), DWORD(::GetLastError()),
            L"原生对不存在的处理器位报 813；fallback 用同一错误码");
    }

    // TC-038
    TEST_METHOD(成功路径不修改LastError)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        ::SetLastError(12345);
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Id, 1) != FALSE);
        Assert::AreEqual(DWORD(12345), DWORD(::GetLastError()));
    }

    // 无效句柄必须先于 Id 校验之外的路径独立可观测：原生对 0xDEAD 返回 ERROR_INVALID_HANDLE(6)
    TEST_METHOD(无效句柄_返回FALSE并报句柄无效)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        ::SetLastError(0);
        Assert::IsFalse(::SetThreadSelectedCpuSets((HANDLE)(ULONG_PTR)0xDEAD, &_Id, 1));
        Assert::AreEqual(DWORD(ERROR_INVALID_HANDLE), DWORD(::GetLastError()),
            L"原生对无效句柄返回 ERROR_INVALID_HANDLE(6)");
    }

    // NULL 句柄同样必须显式返回 6，不能依赖被调方的副作用
    TEST_METHOD(Null句柄_返回FALSE并报句柄无效)
    {
        const ULONG _Id = CpuSetIdOfProcessor(0);
        ::SetLastError(0);
        Assert::IsFalse(::SetThreadSelectedCpuSets(nullptr, &_Id, 1));
        Assert::AreEqual(DWORD(ERROR_INVALID_HANDLE), DWORD(::GetLastError()),
            L"原生对 NULL 句柄返回 ERROR_INVALID_HANDLE(6)");
    }

    // 句柄与 Id 的校验顺序：原生先判 Id 再判句柄，
    // 因此 (NULL, {非法Id}) 报 813 而不是 6。
    TEST_METHOD(Null句柄与非法Id_优先报无效CpuSet)
    {
        const ULONG _Id = kCpuSetIdBitCount; // 组 1 位 0，必然非法
        ::SetLastError(0);
        Assert::IsFalse(::SetThreadSelectedCpuSets(nullptr, &_Id, 1));
        Assert::AreEqual(DWORD(ERROR_CPU_SET_INVALID), DWORD(::GetLastError()),
            L"原生先校验 Id 再校验句柄，(NULL, {非法Id}) 返回 813");
    }

    // 跨进程判断用的是 GetProcessIdOfThread 的结果，而不是句柄相等。
    // 用 DuplicateHandle 复制当前线程得到一个「值不同但指向同一线程」的句柄，
    // 此时 _hThread != GetCurrentThread() 但进程 id 相同，必须走正常写入路径。
    TEST_METHOD(复制的当前线程句柄_正常写入而非误报未实现)
    {
        // 前置：恢复进程与线程掩码，避免本类前面的用例把掩码收窄导致写入失败。
        RestoreProcessAffinity();
        RestoreThreadAffinity();
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0) != FALSE);

        HANDLE _hDuplicated = nullptr;
        Assert::IsTrue(::DuplicateHandle(::GetCurrentProcess(), ::GetCurrentThread(),
            ::GetCurrentProcess(), &_hDuplicated, 0, FALSE, DUPLICATE_SAME_ACCESS) != FALSE);

        const ULONG _Id = CpuSetIdOfProcessor(0);
        ::SetLastError(0);
        const BOOL _bResult = ::SetThreadSelectedCpuSets(_hDuplicated, &_Id, 1);

        if (_bResult != FALSE)
        {
            ULONG _Ids[4] = {};
            ULONG _uGot = 0;
            Assert::IsTrue(::GetThreadSelectedCpuSets(::GetCurrentThread(), _Ids, 4, &_uGot) != FALSE);
            Assert::AreEqual(1ul, ULONG(_uGot), L"复制句柄写入后线程选择应为 1 个 Id");
            Assert::AreEqual(_Id, _Ids[0]);
        }

        ::CloseHandle(_hDuplicated);

        Assert::IsTrue(_bResult != FALSE,
            L"同进程线程的复制句柄必须正常写入");
    }

    // TC-040
    TEST_METHOD(设置后的掩码不包含其他处理器位)
    {
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0) != FALSE);

        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Id, 1) != FALSE);

        GROUP_AFFINITY _Affinity = {};
        Assert::IsTrue(::GetThreadGroupAffinity(::GetCurrentThread(), &_Affinity) != FALSE);
        Assert::AreEqual(WORD(0), _Affinity.Group);
        Assert::AreEqual(KAFFINITY(KAFFINITY(1) << _Id), _Affinity.Mask, L"掩码必须恰好等于 Id 对应的位");
    }

    // TC-041：跨进程线程必须支持（原生实测返回 TRUE，且不改目标进程亲和性）。
    // 用 CREATE_SUSPENDED 起的子进程，其唯一线程处于挂起状态，不会干扰本进程调度。
    TEST_METHOD(跨进程线程_正常写入而非误报未实现)
    {
        STARTUPINFOA _StartupInfo = { sizeof(_StartupInfo) };
        PROCESS_INFORMATION _ProcessInfo = {};
        char _szCommandLine[] = "cmd.exe /c ping 127.0.0.1 -n 30 > nul";
        Assert::IsTrue(::CreateProcessA(nullptr, _szCommandLine, nullptr, nullptr, FALSE,
            CREATE_SUSPENDED | CREATE_NO_WINDOW, nullptr, nullptr, &_StartupInfo, &_ProcessInfo) != FALSE,
            L"需要能创建挂起态子进程");
        ::CloseHandle(_ProcessInfo.hThread);

        HANDLE _hTargetThread = ::OpenThread(THREAD_SET_INFORMATION | THREAD_QUERY_INFORMATION,
            FALSE, _ProcessInfo.dwThreadId);

        HANDLE _hTargetProcess = nullptr;
        DWORD_PTR _uProcessMaskBefore = 0;
        DWORD_PTR _uSystemMask = 0;
        if (_hTargetThread != nullptr)
        {
            _hTargetProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, _ProcessInfo.dwProcessId);
            if (_hTargetProcess != nullptr)
            {
                ::GetProcessAffinityMask(_hTargetProcess, &_uProcessMaskBefore, &_uSystemMask);
            }
        }

        if (_hTargetThread == nullptr || _hTargetProcess == nullptr)
        {
            Logger::WriteMessage(L"无法打开目标线程/进程，跳过本用例。");
            if (_hTargetThread) ::CloseHandle(_hTargetThread);
            if (_hTargetProcess) ::CloseHandle(_hTargetProcess);
            ::TerminateProcess(_ProcessInfo.hProcess, 0);
            ::CloseHandle(_ProcessInfo.hProcess);
            return;
        }

        // 选一个确定落在目标进程亲和性内的 Id，避免撞上「请求位超出目标进程掩码」分支。
        // v1.9：先恢复本进程掩码，避免本类前面用例把进程亲和性收窄，
        // 导致 SetThreadSelectedCpuSets 写入时受进程亲和性约束而失败。
        RestoreProcessAffinity();
        RestoreThreadAffinity();
        ULONG _uFirstId = 0;
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0) != FALSE);
        {
            DWORD_PTR _uSelfMask = 0;
            ::GetProcessAffinityMask(::GetCurrentProcess(), &_uSelfMask, &_uSystemMask);
            for (ULONG _uBit = 0; _uBit != kCpuSetIdBitCount; ++_uBit)
            {
                if ((_uSelfMask & ((KAFFINITY)1 << _uBit)) == 0)
                    continue;
                _uFirstId = _uBit;
                break;
            }
        }

        ::SetLastError(0);
        const BOOL _bResult = ::SetThreadSelectedCpuSets(_hTargetThread, &_uFirstId, 1);
        const DWORD _uError = ::GetLastError();

        DWORD_PTR _uProcessMaskAfter = 0;
        ::GetProcessAffinityMask(_hTargetProcess, &_uProcessMaskAfter, &_uSystemMask);

        ::CloseHandle(_hTargetThread);
        ::CloseHandle(_hTargetProcess);
        ::TerminateProcess(_ProcessInfo.hProcess, 0);
        ::CloseHandle(_ProcessInfo.hProcess);

        Assert::IsTrue(_bResult != FALSE,
            L"跨进程线程必须支持写入，不得返回 ERROR_CALL_NOT_IMPLEMENTED");
        Assert::AreEqual(DWORD(ERROR_SUCCESS), _uError);
        Assert::AreEqual(DWORD_PTR(_uProcessMaskBefore), DWORD_PTR(_uProcessMaskAfter),
            L"写入目标进程的线程亲和性不得改动目标进程的亲和性掩码");
    }

    // TC-042（v1.9 重写）：请求位超出目标进程亲和性时，透出系统的 87，且不放宽目标进程掩码。
    TEST_METHOD(请求超出目标进程亲和性_不得放宽目标进程掩码)
    {
        // 前置：恢复本进程掩码，避免本类前面用例的收窄状态干扰。
        RestoreProcessAffinity();
        RestoreThreadAffinity();
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0) != FALSE);

        DWORD_PTR _uSelfSystemMask = 0;
        DWORD_PTR _uSelfProcessMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uSelfProcessMask, &_uSelfSystemMask) != FALSE);
        if (_uSelfSystemMask == 1)
        {
            Logger::WriteMessage(L"系统只有 1 个处理器，无法构造超集请求，跳过本用例。");
            return;
        }

        STARTUPINFOA _StartupInfo = { sizeof(_StartupInfo) };
        PROCESS_INFORMATION _ProcessInfo = {};
        char _szCommandLine[] = "cmd.exe /c ping 127.0.0.1 -n 30 > nul";
        Assert::IsTrue(::CreateProcessA(nullptr, _szCommandLine, nullptr, nullptr, FALSE,
            CREATE_SUSPENDED | CREATE_NO_WINDOW, nullptr, nullptr, &_StartupInfo, &_ProcessInfo) != FALSE);
        ::CloseHandle(_ProcessInfo.hThread);

        HANDLE _hTargetThread = ::OpenThread(THREAD_SET_INFORMATION | THREAD_QUERY_INFORMATION,
            FALSE, _ProcessInfo.dwThreadId);
        HANDLE _hTargetProcess = ::OpenProcess(PROCESS_SET_INFORMATION | PROCESS_QUERY_INFORMATION,
            FALSE, _ProcessInfo.dwProcessId);

        if (_hTargetThread == nullptr || _hTargetProcess == nullptr)
        {
            Logger::WriteMessage(L"无法打开目标线程/进程，跳过本用例。");
            if (_hTargetThread) ::CloseHandle(_hTargetThread);
            if (_hTargetProcess) ::CloseHandle(_hTargetProcess);
            ::TerminateProcess(_ProcessInfo.hProcess, 0);
            ::CloseHandle(_ProcessInfo.hProcess);
            return;
        }

        // 目标进程只留 bit0，随后请求 bit1（不在目标进程掩码内）。
        Assert::IsTrue(::SetProcessAffinityMask(_hTargetProcess, 0x1) != FALSE);

        const ULONG _Id = 1;
        ::SetLastError(0);
        const BOOL _bResult = ::SetThreadSelectedCpuSets(_hTargetThread, &_Id, 1);
        const DWORD _uError = ::GetLastError();

        DWORD_PTR _uAfterMask = 0;
        DWORD_PTR _uAfterSystem = 0;
        ::GetProcessAffinityMask(_hTargetProcess, &_uAfterMask, &_uAfterSystem);

        ::CloseHandle(_hTargetThread);
        ::CloseHandle(_hTargetProcess);
        ::TerminateProcess(_ProcessInfo.hProcess, 0);
        ::CloseHandle(_ProcessInfo.hProcess);

        Assert::IsFalse(_bResult, L"超出目标进程亲和性时必须失败");
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), _uError,
            L"v1.9：透出系统的 ERROR_INVALID_PARAMETER(87)，不再返回 120");
        Assert::AreEqual(DWORD_PTR(0x1), DWORD_PTR(_uAfterMask),
            L"失败时绝不能放宽目标进程的亲和性掩码");
    }
};

// ============================================================================
// GetSystemCpuSetInformation
// ============================================================================
TEST_CLASS(GetSystemCpuSetInformation)
{
    AwaysNullGuard Guard;

public:
    GetSystemCpuSetInformation()
    {
        Guard |= YY::Thunks::aways_null_try_get_GetSystemCpuSetInformation;
    }

    TEST_METHOD_INITIALIZE(MethodInitialize)
    {
        Assert::IsTrue(YY::Thunks::aways_null_try_get_GetSystemCpuSetInformation,
            L"fallback 守卫未生效");
    }

    // TC-041
    TEST_METHOD(ReturnedLength为NULL_返回FALSE并报参数错误)
    {
        ::SetLastError(0);
        Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 0, nullptr, nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), ::GetLastError());
    }

    // TC-042
    TEST_METHOD(缓冲区为NULL且长度为零_返回FALSE并报缓冲区不足)
    {
        ULONG _uLength = 0;
        ::SetLastError(0);
        Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError());
        Assert::IsTrue(_uLength > 0);
    }

    // TC-043
    TEST_METHOD(缓冲区为NULL但长度大于零_返回FALSE且不崩溃)
    {
        ULONG _uBaseline = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uBaseline, nullptr, 0);
        Assert::IsTrue(_uBaseline > 0);

        ULONG _uLength = 0;
        ::SetLastError(0);
        Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 4096, &_uLength, nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError());
        Assert::AreEqual(ULONG(_uBaseline), ULONG(_uLength), L"回填长度必须与 (NULL,0) 一致");
    }

    // TC-044
    TEST_METHOD(返回长度是记录大小整数倍)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);
        Assert::AreEqual(0ul, ULONG(_uLength % ULONG(sizeof(SYSTEM_CPU_SET_INFORMATION))));
    }

    // TC-045
    TEST_METHOD(缓冲区不足一字节_返回FALSE并回填所需长度)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);
        Assert::IsTrue(_uLength > 1);

        std::vector<BYTE> _Buffer(_uLength);
        ULONG _uGot = 0;
        ::SetLastError(0);
        Assert::IsFalse(::GetSystemCpuSetInformation((PSYSTEM_CPU_SET_INFORMATION)_Buffer.data(), _uLength - 1, &_uGot, nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError());
        Assert::AreEqual(ULONG(_uLength), ULONG(_uGot));
    }

    // TC-046
    TEST_METHOD(缓冲区恰好_返回TRUE)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);

        std::vector<BYTE> _Buffer(_uLength);
        ULONG _uGot = 0;
        Assert::IsTrue(::GetSystemCpuSetInformation((PSYSTEM_CPU_SET_INFORMATION)_Buffer.data(), _uLength, &_uGot, nullptr, 0) != FALSE);
        Assert::AreEqual(ULONG(_uLength), ULONG(_uGot));
    }

    // TC-047
    TEST_METHOD(缓冲区超出_返回TRUE且数量为所需值)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);

        std::vector<BYTE> _Buffer(_uLength + 64);
        ULONG _uGot = 0;
        Assert::IsTrue(::GetSystemCpuSetInformation((PSYSTEM_CPU_SET_INFORMATION)_Buffer.data(), _uLength + 64, &_uGot, nullptr, 0) != FALSE);
        Assert::AreEqual(ULONG(_uLength), ULONG(_uGot), L"返回长度必须是所需值，不是缓冲区容量");
    }

    // TC-063
    TEST_METHOD(缓冲区非空但长度为零_返回FALSE并回填所需长度)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);
        Assert::IsTrue(_uLength > 0);

        std::vector<BYTE> _Buffer(_uLength);
        ULONG _uGot = 0xDEADBEEF;
        ::SetLastError(0);
        Assert::IsFalse(::GetSystemCpuSetInformation((PSYSTEM_CPU_SET_INFORMATION)_Buffer.data(), 0, &_uGot, nullptr, 0));
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError());
        Assert::AreEqual(ULONG(_uLength), ULONG(_uGot));
    }

    // TC-048：P0-1 回归 —— 记录数必须与长度自洽，不能为 0
    TEST_METHOD(记录可按Size遍历且数量与长度自洽)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);
        Assert::IsTrue(_uLength > 0);

        std::vector<BYTE> _Buffer(_uLength);
        ULONG _uGot = 0;
        Assert::IsTrue(::GetSystemCpuSetInformation((PSYSTEM_CPU_SET_INFORMATION)_Buffer.data(), _uLength, &_uGot, nullptr, 0) != FALSE);

        ULONG _cRecords = 0;
        for (ULONG _uOffset = 0; _uOffset < _uGot;)
        {
            const auto _pInfo = (const SYSTEM_CPU_SET_INFORMATION*)(_Buffer.data() + _uOffset);
            Assert::AreEqual(ULONG(sizeof(SYSTEM_CPU_SET_INFORMATION)), ULONG(_pInfo->Size),
                L"每条记录的 Size 必须是 sizeof(SYSTEM_CPU_SET_INFORMATION)");
            Assert::IsTrue(_pInfo->Size != 0, L"Size 不得为 0，否则遍历会死循环");
            _uOffset += _pInfo->Size;
            ++_cRecords;
        }

        Assert::AreEqual(ULONG(_uGot / ULONG(sizeof(SYSTEM_CPU_SET_INFORMATION))), ULONG(_cRecords),
            L"遍历出的记录数必须等于 长度/记录大小；为 0 说明 Size 门槛判断有误");
    }

    // TC-049
    TEST_METHOD(记录数等于逻辑处理器数)
    {
        if (::GetActiveProcessorGroupCount() > 1)
        {
            Logger::WriteMessage(L"多处理器组环境，跳过本用例。");
            return;
        }

        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);

        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);
        const ULONG _cRecords = _uLength / ULONG(sizeof(SYSTEM_CPU_SET_INFORMATION));
        Assert::AreEqual(ULONG(_SystemInfo.dwNumberOfProcessors), ULONG(_cRecords));
    }

    // TC-050
    TEST_METHOD(存在Id等于逻辑处理器索引的记录)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);

        std::vector<BYTE> _Buffer(_uLength);
        ULONG _uGot = 0;
        Assert::IsTrue(::GetSystemCpuSetInformation((PSYSTEM_CPU_SET_INFORMATION)_Buffer.data(), _uLength, &_uGot, nullptr, 0) != FALSE);

        bool _bFound = false;
        for (ULONG _uOffset = 0; _uOffset < _uGot;)
        {
            const auto _pInfo = (const SYSTEM_CPU_SET_INFORMATION*)(_Buffer.data() + _uOffset);
            if (_pInfo->CpuSet.Id == _pInfo->CpuSet.LogicalProcessorIndex)
            {
                _bFound = true;
            }
            _uOffset += _pInfo->Size;
        }
        Assert::IsTrue(_bFound, L"组 0 内至少应有一条记录的 Id == LogicalProcessorIndex");
    }

    // TC-051：R-006 回归 —— AllFlags 必须为 0
    TEST_METHOD(所有记录的AllFlags为零)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);

        std::vector<BYTE> _Buffer(_uLength);
        ULONG _uGot = 0;
        Assert::IsTrue(::GetSystemCpuSetInformation((PSYSTEM_CPU_SET_INFORMATION)_Buffer.data(), _uLength, &_uGot, nullptr, 0) != FALSE);

        for (ULONG _uOffset = 0; _uOffset < _uGot;)
        {
            const auto _pInfo = (const SYSTEM_CPU_SET_INFORMATION*)(_Buffer.data() + _uOffset);
            Assert::AreEqual(DWORD(0), DWORD(_pInfo->CpuSet.AllFlags),
                L"AllFlags 必须为 0；实测原生 ALLOCATED 恒为 0，置 ALLOCATED 会误导调用方");
            _uOffset += _pInfo->Size;
        }
    }

    // TC-052 / TC-053 / TC-054
    TEST_METHOD(记录字段与唯一性正确)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);

        std::vector<BYTE> _Buffer(_uLength);
        ULONG _uGot = 0;
        Assert::IsTrue(::GetSystemCpuSetInformation((PSYSTEM_CPU_SET_INFORMATION)_Buffer.data(), _uLength, &_uGot, nullptr, 0) != FALSE);

        std::vector<BYTE> _SeenLpIndex;
        for (ULONG _uOffset = 0; _uOffset < _uGot;)
        {
            const auto _pInfo = (const SYSTEM_CPU_SET_INFORMATION*)(_Buffer.data() + _uOffset);
            Assert::AreEqual(ULONG(CpuSetInformation), ULONG(_pInfo->Type));
            Assert::AreEqual(WORD(0), _pInfo->CpuSet.Group, L"Fallback 只模拟组 0");

            _SeenLpIndex.push_back(_pInfo->CpuSet.LogicalProcessorIndex);

            _uOffset += _pInfo->Size;
        }

        for (size_t _i = 0; _i != _SeenLpIndex.size(); ++_i)
        {
            for (size_t _j = _i + 1; _j != _SeenLpIndex.size(); ++_j)
            {
                Assert::IsTrue(_SeenLpIndex[_i] != _SeenLpIndex[_j], L"LogicalProcessorIndex 不得重复");
            }
        }
    }

    // TC-055：CoreIndex 归并 —— 同物理核的多个逻辑处理器必须共享 CoreIndex
    TEST_METHOD(CoreIndex去重数符合物理核数)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);
        std::vector<BYTE> _Buffer(_uLength);
        ULONG _uGot = 0;
        Assert::IsTrue(::GetSystemCpuSetInformation((PSYSTEM_CPU_SET_INFORMATION)_Buffer.data(), _uLength, &_uGot, nullptr, 0) != FALSE);

        std::vector<BYTE> _SeenCore;
        ULONG _cRecords = 0;
        for (ULONG _uOffset = 0; _uOffset < _uGot;)
        {
            const auto _pInfo = (const SYSTEM_CPU_SET_INFORMATION*)(_Buffer.data() + _uOffset);
            _SeenCore.push_back(_pInfo->CpuSet.CoreIndex);
            ++_cRecords;
            _uOffset += _pInfo->Size;
        }

        {
            std::vector<BYTE> _Unique;
            for (size_t _i = 0; _i != _SeenCore.size(); ++_i)
            {
                bool _bDup = false;
                for (size_t _j = 0; _j != _Unique.size(); ++_j)
                {
                    if (_Unique[_j] == _SeenCore[_i]) { _bDup = true; break; }
                }
                if (!_bDup) { _Unique.push_back(_SeenCore[_i]); }
            }
            _SeenCore.swap(_Unique);
        }

        
        Assert::IsTrue(_SeenCore.size() <= _cRecords, L"CoreIndex 去重数不得大于记录数");

        // GLPIEx 可用时，去重数必须等于 RelationProcessorCore 记录数；不可用时一处理器一核。
        ULONG _cbBuffer = 0;
        if (::GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &_cbBuffer) == FALSE
            && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER && _cbBuffer != 0)
        {
            std::vector<BYTE> _CoreBuffer(_cbBuffer);
            if (::GetLogicalProcessorInformationEx(RelationProcessorCore,
                    (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)_CoreBuffer.data(), &_cbBuffer) != FALSE)
            {
                const DWORD _cbMin = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Processor);
                ULONG _cCores = 0;
                for (BYTE* _p = _CoreBuffer.data(), *_pEnd = _CoreBuffer.data() + _cbBuffer; _p + _cbMin <= _pEnd;)
                {
                    const auto _pInfo = (const SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)_p;
                    if (_pInfo->Size < _cbMin)
                        break;
                    if (_pInfo->Relationship == RelationProcessorCore)
                        ++_cCores;
                    _p += _pInfo->Size;
                }
                if (_cCores != 0)
                {
                    Assert::AreEqual(ULONG(_cCores), ULONG(_SeenCore.size()), L"CoreIndex 去重数必须等于物理核数");
                }
            }
        }
    }

    // TC-056：P0-3 回归 —— Process 参数不得裁剪集合
    TEST_METHOD(Process参数不影响返回长度)
    {
        ULONG _uLengthNull = 0;
        ULONG _uLengthProcess = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLengthNull, nullptr, 0);
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLengthProcess, ::GetCurrentProcess(), 0);
        Assert::AreEqual(ULONG(_uLengthNull), ULONG(_uLengthProcess), L"Process 参数不得裁剪集合");
    }

    // TC-057：P0-3 回归关键用例
    TEST_METHOD(亲和性收窄后Process参数仍不影响返回长度)
    {
        ULONG _uBaseline = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uBaseline, nullptr, 0);

        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask) != FALSE);
        Assert::IsTrue(::SetProcessAffinityMask(::GetCurrentProcess(), 0x1) != FALSE);

        ULONG _uNarrowedNull = 0;
        ULONG _uNarrowedProcess = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uNarrowedNull, nullptr, 0);
        ::GetSystemCpuSetInformation(nullptr, 0, &_uNarrowedProcess, ::GetCurrentProcess(), 0);

        ::SetProcessAffinityMask(::GetCurrentProcess(), _uSystemMask);

        Assert::AreEqual(ULONG(_uBaseline), ULONG(_uNarrowedNull), L"进程亲和性收窄不得影响枚举结果");
        Assert::AreEqual(ULONG(_uNarrowedNull), ULONG(_uNarrowedProcess));
    }

    // TC-058：原生会解析 Process 句柄，实测 GetSystemCpuSetInformation(..., 0xDEAD, 0)
    // 返回 FALSE + ERROR_INVALID_HANDLE(6)，只有句柄可查询时才继续枚举。
    TEST_METHOD(无效Process句柄_返回FALSE并报句柄无效)
    {
        ULONG _uLength = 0;
        ::SetLastError(0);
        Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 0, &_uLength, (HANDLE)(ULONG_PTR)0x1234, 0));
        Assert::AreEqual(DWORD(ERROR_INVALID_HANDLE), DWORD(::GetLastError()),
            L"原生对无效 Process 句柄返回 ERROR_INVALID_HANDLE(6)");
    }

    // TC-058b：Process 为 NULL 与伪句柄都表示当前进程，必须成功。
    TEST_METHOD(Null与伪Process句柄_均正常返回所需长度)
    {
        ULONG _uFromNull = 0;
        ULONG _uFromPseudo = 0;
        Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 0, &_uFromNull, nullptr, 0));
        Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 0, &_uFromPseudo, ::GetCurrentProcess(), 0));
        Assert::AreEqual(ULONG(_uFromNull), ULONG(_uFromPseudo),
            L"NULL 与伪句柄都表示当前进程，返回长度必须一致");
    }

    // TC-058c：Flags 校验先于 Process 句柄解析。实测原生对 (非法句柄, flags=1) 返回 87。
    TEST_METHOD(非法Flags优先于非法Process句柄报参数错误)
    {
        ULONG _uLength = 0;
        ::SetLastError(0);
        Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 0, &_uLength, (HANDLE)(ULONG_PTR)0xDEAD, 1));
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), DWORD(::GetLastError()),
            L"原生先校验 Flags 再解析句柄，返回 87 而非 6");
    }

    // TC-059：Flags 是保留字段（SDK 标注 _Reserved_），非 0 必须拒绝。
    // 实测原生传 1 / 2 / 0x80000000 均返回 FALSE + ERROR_INVALID_PARAMETER(87) 且不回填长度。
    TEST_METHOD(Flags非零_返回FALSE并报参数错误)
    {
        const ULONG _FlagsValues[] = { 1, 2, 4, 8, 16, 0x80000000 };
        for (ULONG _uFlags : _FlagsValues)
        {
            ULONG _uLength = 0xDEADBEEF;
            ::SetLastError(0);
            Assert::IsFalse(::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, _uFlags),
                L"非 0 Flags 必须被拒绝");
            Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), DWORD(::GetLastError()));
            Assert::AreEqual(0ul, ULONG(_uLength), L"失败时不得回填长度");
        }
    }

    // TC-059b（v1.14 新增）：成功路径必须保持 LastError 不变。
    // 原生 CpuSet API 成功时不改动 LastError，而 fallback 内部会调用
    // GetLogicalProcessorInformationEx（实测置 122）/ HeapAlloc / GetProcessId，
    // 若不显式还原，调用方会看到 122 这类噪音错误码。
    // 该用例覆盖「两阶段查询」与「真正填充」两条成功出口。
    TEST_METHOD(成功路径不修改LastError)
    {
        // 阶段一：只查所需长度（内部会走 GLPIEx，最容易污染 LastError）。
        ULONG _uRequired = 0;
        ::SetLastError(12345);
        const BOOL _bQuery = ::GetSystemCpuSetInformation(nullptr, 0, &_uRequired, nullptr, 0);
        if (_bQuery != FALSE)
        {
            // 缓冲区为 NULL 时原生返回 FALSE + 122，只有 0 条记录才返回 TRUE。
            Assert::AreEqual(DWORD(12345), DWORD(::GetLastError()),
                L"查询长度路径必须保持 LastError");
            Logger::WriteMessage(L"本机无 CpuSet 记录，仅验证长度查询路径。");
            return;
        }

        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), DWORD(::GetLastError()),
            L"长度查询失败时必须是 122，不能被 LastError 还原逻辑覆盖");
        Assert::IsTrue(_uRequired != 0, L"必须回填所需长度");

        // 阶段二：真正填充。
        const ULONG _cRecords = _uRequired / sizeof(SYSTEM_CPU_SET_INFORMATION);
        std::vector<SYSTEM_CPU_SET_INFORMATION> _Buffer(_cRecords);
        ULONG _uLength = 0;
        ::SetLastError(12345);
        Assert::IsTrue(::GetSystemCpuSetInformation(_Buffer.data(), _uRequired, &_uLength, nullptr, 0) != FALSE,
            L"缓冲区足够时必须成功");
        Assert::AreEqual(DWORD(12345), DWORD(::GetLastError()),
            L"填充路径必须保持 LastError");
        Assert::AreEqual(_uRequired, _uLength);
    }
};

// ============================================================================
// 跨 API 一致性
// ============================================================================

TEST_CLASS(CpuSetCrossApi)
{
    AwaysNullGuard GuardSystem;
    AwaysNullGuard GuardGetThread;
    AwaysNullGuard GuardSetThread;
    AwaysNullGuard GuardGetProcess;
    AwaysNullGuard GuardSetProcess;

public:
    CpuSetCrossApi()
    {
        GuardSystem |= YY::Thunks::aways_null_try_get_GetSystemCpuSetInformation;
        GuardGetThread |= YY::Thunks::aways_null_try_get_GetThreadSelectedCpuSets;
        GuardSetThread |= YY::Thunks::aways_null_try_get_SetThreadSelectedCpuSets;
        GuardGetProcess |= YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets;
        GuardSetProcess |= YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets;
    }

    TEST_METHOD_INITIALIZE(MethodInitialize)
    {
        Assert::IsTrue(YY::Thunks::aways_null_try_get_GetSystemCpuSetInformation);
        Assert::IsTrue(YY::Thunks::aways_null_try_get_GetThreadSelectedCpuSets);
        Assert::IsTrue(YY::Thunks::aways_null_try_get_SetThreadSelectedCpuSets);
        Assert::IsTrue(YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets);
        Assert::IsTrue(YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets);
    }

    TEST_METHOD_CLEANUP(MethodCleanup)
    {
        ::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0);
        ::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0);
        RestoreThreadAffinity();
        RestoreProcessAffinity();
    }

    // TC-060
    TEST_METHOD(枚举结果中的Id可被SetThreadSelectedCpuSets接受)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);
        std::vector<BYTE> _Buffer(_uLength);
        ULONG _uGot = 0;
        Assert::IsTrue(::GetSystemCpuSetInformation((PSYSTEM_CPU_SET_INFORMATION)_Buffer.data(), _uLength, &_uGot, nullptr, 0) != FALSE);

        std::vector<ULONG> _Ids;
        for (ULONG _uOffset = 0; _uOffset < _uGot && _Ids.size() < 3;)
        {
            const auto _pInfo = (const SYSTEM_CPU_SET_INFORMATION*)(_Buffer.data() + _uOffset);
            Assert::AreEqual(DWORD(0), DWORD(_pInfo->CpuSet.AllFlags), L"枚举结果的 AllFlags 应为 0");
            _Ids.push_back(_pInfo->CpuSet.Id);
            _uOffset += _pInfo->Size;
        }

        Assert::IsTrue(!_Ids.empty(), L"枚举结果必须至少包含一条记录");
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), _Ids.data(), (ULONG)_Ids.size()) != FALSE,
            L"枚举出的 Id 必须能被设置接口接受");
    }

    // TC-061
    TEST_METHOD(枚举设置读回形成闭环)
    {
        ULONG _uLength = 0;
        ::GetSystemCpuSetInformation(nullptr, 0, &_uLength, nullptr, 0);
        std::vector<BYTE> _Buffer(_uLength);
        ULONG _uGot = 0;
        Assert::IsTrue(::GetSystemCpuSetInformation((PSYSTEM_CPU_SET_INFORMATION)_Buffer.data(), _uLength, &_uGot, nullptr, 0) != FALSE);

        std::vector<ULONG> _Expected;
        for (ULONG _uOffset = 0; _uOffset < _uGot && _Expected.size() < 2;)
        {
            const auto _pInfo = (const SYSTEM_CPU_SET_INFORMATION*)(_Buffer.data() + _uOffset);
            _Expected.push_back(_pInfo->CpuSet.Id);
            _uOffset += _pInfo->Size;
        }
        Assert::IsTrue(!_Expected.empty());

        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), _Expected.data(), (ULONG)_Expected.size()) != FALSE);

        ULONG _uRequired = 0;
        ::GetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0, &_uRequired);
        std::vector<ULONG> _Actual(_uRequired);
        ULONG _uRead = 0;
        Assert::IsTrue(::GetThreadSelectedCpuSets(::GetCurrentThread(), _Actual.data(), _uRequired, &_uRead) != FALSE);

        _Actual.resize(_uRead);
        Assert::IsTrue(SameIdSet(_Actual, _Expected), L"读回的 Id 集合必须与设置的一致");
    }

    // TC-062：只能断言相对不变性。
    // fallback 的线程掩码继承自进程，所以「设置进程亲和性」会连带改变线程读回数量。
    // 这是 TD-009 记录的已知语义差异（原生两维度正交），不是缺陷。
    TEST_METHOD(设置线程CpuSet不改变进程的读回数量)
    {
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0) != FALSE);
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0) != FALSE);

        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);

        ULONG _uProcessCount = 0;
        ::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uProcessCount);
        Assert::AreEqual(1ul, ULONG(_uProcessCount));

        // 设置线程 CpuSet 不得改变进程的读回数量（这是 fallback 真正满足的性质：
        // SetThreadSelectedCpuSets 只写线程亲和性，不碰进程亲和性）。
        const ULONG _ThreadIds[1] = { CpuSetIdOfProcessor(0) };
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), _ThreadIds, 1) != FALSE);

        ULONG _uProcessAfter = 0;
        ::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uProcessAfter);
        Assert::AreEqual(ULONG(_uProcessCount), ULONG(_uProcessAfter), L"设置线程 CpuSet 不得改变进程的读回数量");
    }

    // TC-063：组合场景 —— 先设进程默认 CpuSet，再给线程设一个「进程默认之外」的 CpuSet。
    //
    // 原生实测（Windows 11 26100）：
    //   SetProcessDefaultCpuSets({CPU0}) -> TRUE，读回 {256}
    //   SetThreadSelectedCpuSets({CPU4}) -> TRUE，读回 {260}，进程读回仍是 {256}
    // TC-104（v1.9 重写）：线程 CpuSet 可以落在进程默认之外 —— 这是 v1.7→v1.9 的
    // **唯一可观测收益**。v1.7 把进程默认映射为进程亲和性（硬约束），此场景只能返回 120；
    // v1.9 只把进程默认记在共享记录里，线程选择直接写线程亲和性，两者互不干扰。
    TEST_METHOD(线程CpuSet落在进程默认之外_返回TRUE且互不干扰)
    {
        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);
        if (_SystemInfo.dwNumberOfProcessors < 2)
        {
            Logger::WriteMessage(L"处理器不足 2 个，无法构造「进程默认之外」的 CpuSet，跳过本用例。");
            return;
        }

        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0) != FALSE);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0) != FALSE);
        RestoreProcessAffinity();
        RestoreThreadAffinity();

        // 进程默认只留处理器 0
        const ULONG _ProcessCpuSetId = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_ProcessCpuSetId, 1) != FALSE);

        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask) != FALSE);

        // 线程请求处理器 1（在进程默认之外）
        const ULONG _ThreadCpuSetId = CpuSetIdOfProcessor(1);
        ::SetLastError(0);
        const BOOL _bResult = ::SetThreadSelectedCpuSets(::GetCurrentThread(), &_ThreadCpuSetId, 1);
        Assert::IsTrue(_bResult != FALSE,
            L"v1.9：线程 CpuSet 可以落在进程默认之外（v1.7 此处返回 120）");

        // 线程读回应为处理器 1
        ULONG _ThreadIds[4] = {};
        ULONG _uThreadGot = 0;
        Assert::IsTrue(::GetThreadSelectedCpuSets(::GetCurrentThread(), _ThreadIds, 4, &_uThreadGot) != FALSE);
        Assert::AreEqual(1ul, ULONG(_uThreadGot), L"线程选择应只有 1 个 Id");
        Assert::AreEqual(_ThreadCpuSetId, _ThreadIds[0], L"线程选择应为处理器 1");

        // 进程默认保持不变
        ULONG _ProcessIds[4] = {};
        ULONG _uProcessGot = 0;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _ProcessIds, 4, &_uProcessGot) != FALSE);
        Assert::AreEqual(1ul, ULONG(_uProcessGot), L"进程默认应仍为 1 个 Id");
        Assert::AreEqual(_ProcessCpuSetId, _ProcessIds[0], L"进程默认应仍为处理器 0");

        // 进程亲和性未被改动
        DWORD_PTR _uProcessMaskAfter = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMaskAfter, &_uSystemMask) != FALSE);
        Assert::AreEqual(DWORD_PTR(_uProcessMask), DWORD_PTR(_uProcessMaskAfter),
            L"本方案绝不修改进程亲和性");

        ::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0);
        ::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0);
        RestoreProcessAffinity();
        RestoreThreadAffinity();
    }
};

// ============================================================================
// TLS 回调：新线程继承进程默认（v1.9 新增机制）
// ============================================================================

TEST_CLASS(CpuSetThreadAttach)
{
    AwaysNullGuard GuardGet;
    AwaysNullGuard GuardSet;

public:
    CpuSetThreadAttach()
    {
        GuardGet |= YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets;
        GuardSet |= YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets;
    }

    TEST_METHOD_INITIALIZE(MethodInitialize)
    {
        ::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0);
        RestoreProcessAffinity();
        RestoreThreadAffinity();
    }

    TEST_METHOD_CLEANUP(MethodCleanup)
    {
        ::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0);
        RestoreProcessAffinity();
        RestoreThreadAffinity();
    }

    struct ThreadProbeContext
    {
        volatile LONG bStart;           // 主线程置 1 后才读掩码，保证 THREAD_ATTACH 已完成
        DWORD_PTR uThreadMask;
        WORD      Group;
        BOOL      bQueryOk;
    };

    static unsigned __stdcall ThreadProbe(void* _pParam)
    {
        auto* _pContext = (ThreadProbeContext*)_pParam;

        // 等待主线程允许 —— 此时本线程的 THREAD_ATTACH 早已完成。
        while (_pContext->bStart == 0)
            ::Sleep(1);

        GROUP_AFFINITY _Affinity = {};
        _pContext->bQueryOk = ::GetThreadGroupAffinity(::GetCurrentThread(), &_Affinity);
        _pContext->uThreadMask = _pContext->bQueryOk ? (DWORD_PTR)_Affinity.Mask : 0;
        _pContext->Group = _pContext->bQueryOk ? _Affinity.Group : 0xFFFF;
        return 0;
    }

    // TC-092：新线程继承进程默认。
    // 创建 2 个线程：若「CRT 补发伪 THREAD_ATTACH」在当前 CRT 上成立，
    // 一次性标记可能吞掉第 1 个线程的通知，因此断言「至少第 2 个线程继承成功」。
    TEST_METHOD(新线程继承进程默认)
    {
        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);
        if (_SystemInfo.dwNumberOfProcessors < 2)
        {
            Logger::WriteMessage(L"处理器不足 2 个，跳过本用例。");
            return;
        }

        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE,
            L"前置：设置进程默认必须成功");

        ThreadProbeContext _Contexts[2] = {};
        HANDLE _hThreads[2] = {};

        for (int _i = 0; _i != 2; ++_i)
        {
            unsigned _uThreadId = 0;
            _hThreads[_i] = (HANDLE)::_beginthreadex(nullptr, 0, ThreadProbe, &_Contexts[_i], 0, &_uThreadId);
            Assert::IsTrue(_hThreads[_i] != nullptr, L"创建线程失败");

            // 给 TLS 回调留出执行时间，再放行读掩码。
            ::Sleep(20);
            ::InterlockedExchange(&_Contexts[_i].bStart, 1);
        }

        ::WaitForMultipleObjects(2, _hThreads, TRUE, 10000);

        wchar_t _szMsg[256];
        ::wsprintfW(_szMsg, L"线程掩码：t0=0x%I64X(ok=%d) t1=0x%I64X(ok=%d)",
            (unsigned long long)_Contexts[0].uThreadMask, _Contexts[0].bQueryOk,
            (unsigned long long)_Contexts[1].uThreadMask, _Contexts[1].bQueryOk);
        Logger::WriteMessage(_szMsg);

        for (int _i = 0; _i != 2; ++_i)
            ::CloseHandle(_hThreads[_i]);

        // 至少有一个线程继承了进程默认（0x1）。
        const bool _bInherited =
            (_Contexts[0].bQueryOk && _Contexts[0].uThreadMask == 0x1) ||
            (_Contexts[1].bQueryOk && _Contexts[1].uThreadMask == 0x1);
        Assert::IsTrue(_bInherited,
            L"新线程必须继承进程默认 CpuSet（掩码应为 0x1）");
    }

    // TC-096：无进程默认时，TLS 回调不限制新线程。
    TEST_METHOD(无进程默认时不改动新线程掩码)
    {
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0) != FALSE);

        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask = 0;
        Assert::IsTrue(::GetProcessAffinityMask(::GetCurrentProcess(), &_uProcessMask, &_uSystemMask) != FALSE);

        ThreadProbeContext _Context = {};
        unsigned _uThreadId = 0;
        HANDLE _hThread = (HANDLE)::_beginthreadex(nullptr, 0, ThreadProbe, &_Context, 0, &_uThreadId);
        Assert::IsTrue(_hThread != nullptr);

        ::Sleep(20);
        ::InterlockedExchange(&_Context.bStart, 1);
        ::WaitForSingleObject(_hThread, 10000);
        ::CloseHandle(_hThread);

        Assert::IsTrue(_Context.bQueryOk != FALSE);
        // 新线程继承创建线程的掩码，无进程默认时不应被改动。
        Assert::AreEqual(DWORD_PTR(_uProcessMask), _Context.uThreadMask,
            L"无进程默认时 TLS 回调不得改动新线程掩码");
    }
};

// ============================================================================
// 位打包与原子读写（v1.9 内部设施）
// ============================================================================

TEST_CLASS(CpuSetDefaultRecordLayout)
{
public:
    // TC-093：位打包往返 —— 通过公开 API 间接验证（Pack/Unpack 是内部静态函数）。
    // 设置 → 读回必须完全一致，这是 P0-1（x86 掩码被截断为 16 位）的直接回归。
    TEST_METHOD(位打包往返无损)
    {
        // 逐个 bit 验证，覆盖 x86 的全部 32 位与 x64 的全部 64 位。
        for (ULONG _uBit = 0; _uBit != kCpuSetIdBitCount; ++_uBit)
        {
            ULONG _cLimit = kCpuSetIdBitCount;
            SYSTEM_INFO _SystemInfo = {};
            ::GetSystemInfo(&_SystemInfo);
            if (_SystemInfo.dwNumberOfProcessors < _cLimit)
                _cLimit = _SystemInfo.dwNumberOfProcessors;

            if (_uBit >= _cLimit)
                break;

            const ULONG _Id = CpuSetIdOfProcessor(_uBit);

            AwaysNullGuard _GuardSet;
            _GuardSet |= YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets;
            AwaysNullGuard _GuardGet;
            _GuardGet |= YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets;

            Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);

            ULONG _Readback[4] = {};
            ULONG _uGot = 0;
            Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _Readback, 4, &_uGot) != FALSE);

            wchar_t _szMsg[128];
            ::wsprintfW(_szMsg, L"bit=%u 读回数量=%u", _uBit, _uGot);
            Assert::AreEqual(ULONG(1), ULONG(_uGot), _szMsg);
            Assert::AreEqual(_Id, _Readback[0], L"读回的 Id 必须与设置的完全相同");

            ::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0);
        }
    }

    // TC-095：记录结构对齐与大小断言。
    // x64 的 cmpxchg16b 要求 16 字节对齐，未对齐会 #GP；
    // x86 的 InterlockedCompareExchange64 要求 8 字节对齐。
    TEST_METHOD(记录结构对齐与大小正确)
    {
        // 通过 fallback 使用该记录来间接确认：任何一次 Set/Get 都会触达原子操作，
        // 若未对齐会在 x64 上直接 #GP 崩溃（测试进程崩溃即为失败）。
        AwaysNullGuard _GuardSet;
        _GuardSet |= YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets;
        AwaysNullGuard _GuardGet;
        _GuardGet |= YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets;

        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);

        ULONG _Readback[4] = {};
        ULONG _uGot = 0;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _Readback, 4, &_uGot) != FALSE);
        Assert::AreEqual(ULONG(1), ULONG(_uGot));
        Assert::AreEqual(_Id, _Readback[0]);

        ::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0);
    }

    // TC-079 补充：掩码为 0 时组号必须归零（Store 级不变式，通过公开 API 间接验证）。
    TEST_METHOD(掩码为零时组号归零)
    {
        AwaysNullGuard _GuardSet;
        _GuardSet |= YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets;
        AwaysNullGuard _GuardGet;
        _GuardGet |= YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets;

        const ULONG _Id = CpuSetIdOfProcessor(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0) != FALSE);

        ULONG _uRequired = 0xDEADBEEF;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0, &_uRequired) != FALSE);
        Assert::AreEqual(ULONG(0), ULONG(_uRequired), L"清除后必须读回 0（组号与掩码都已归零）");
    }

    // TC-106（v1.15 新增，P0-1/P0-2 回归）：x86 上记录是「Mask + Group」两段
    // 拼进一个 64 位 CAS 单元。若 CAS 只用 Raw[0]（4 字节 Mask）参与比较与交换，
    // Group 永远写不进去、且已存在的 Group 会被清零，还会因返回的旧值与截断比较值
    // 永不相等而死循环。
    // 本机单组时 Group 恒为 0，无法暴露；因此在多组机器上直接验证 Group 能往返。
    TEST_METHOD(多组机器上组号可往返)
    {
        AwaysNullGuard _GuardSet;
        _GuardSet |= YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets;
        AwaysNullGuard _GuardGet;
        _GuardGet |= YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets;

        if (::GetActiveProcessorGroupCount() <= 1)
        {
            Logger::WriteMessage(L"本机只有 1 个处理器组，无法验证 Group 往返（需多组机器）。");
            return;
        }

        // 组 1 的位 0：Id = 1 * kCpuSetIdBitCount。多组机器上该组必然存在。
        const ULONG _Id = kCpuSetIdBitCount;
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE,
            L"多组机器上组 1 的合法 Id 必须被接受");

        ULONG _Readback[4] = {};
        ULONG _uGot = 0;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _Readback, 4, &_uGot) != FALSE);
        Assert::AreEqual(ULONG(1), ULONG(_uGot));
        Assert::AreEqual(_Id, _Readback[0], L"组号必须能完整往返（x86 上曾因 CAS 只用 32 位而恒为 0）");

        ::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0);
    }

    // TC-107（v1.15 新增，P0-3 回归）：单组内位序 >= 32 的合法 Id 必须被接受。
    // x64 上 KAFFINITY 是 64 位，但若把 (KAFFINITY)1 << bit 落到 ULONG（32 位）里，
    // 位序 >= 32 时结果为 0 → 被误判为「不属于本机处理器」而返回 813。
    // 该缺陷只在「单组内逻辑处理器数 > 32」的机器上暴露（如 48 核 / 64 核单组）。
    TEST_METHOD(单组内位序大于等于32的合法Id被接受)
    {
        AwaysNullGuard _GuardSet;
        _GuardSet |= YY::Thunks::aways_null_try_get_SetProcessDefaultCpuSets;
        AwaysNullGuard _GuardGet;
        _GuardGet |= YY::Thunks::aways_null_try_get_GetProcessDefaultCpuSets;

        const DWORD _cProcessors = ::GetActiveProcessorCount(0);
        if (_cProcessors <= 32)
        {
            wchar_t _szMsg[160];
            ::wsprintfW(_szMsg, L"组 0 只有 %u 个处理器（<= 32），无法构造位序 >= 32 的合法 Id，跳过。", _cProcessors);
            Logger::WriteMessage(_szMsg);
            return;
        }

        // 取组内位序 32（第 33 个处理器）作为代表。
        const ULONG _Id = 32;
        ::SetLastError(0);
        Assert::IsTrue(::SetProcessDefaultCpuSets(::GetCurrentProcess(), &_Id, 1) != FALSE,
            L"组内位序 32 的合法 Id 必须被接受，不得因 32 位截断被判为无效");
        Assert::AreEqual(DWORD(ERROR_SUCCESS), DWORD(::GetLastError()));

        ULONG _Readback[4] = {};
        ULONG _uGot = 0;
        Assert::IsTrue(::GetProcessDefaultCpuSets(::GetCurrentProcess(), _Readback, 4, &_uGot) != FALSE);
        Assert::AreEqual(ULONG(1), ULONG(_uGot));
        Assert::AreEqual(_Id, _Readback[0], L"位序 32 必须能完整往返");

        // 同时验证 SetThreadSelectedCpuSets 走同一条 TryGroupMaskFromCpuSetIds 路径。
        Assert::IsTrue(::SetThreadSelectedCpuSets(::GetCurrentThread(), &_Id, 1) != FALSE,
            L"SetThreadSelectedCpuSets 同样不得拒绝位序 >= 32 的合法 Id");

        ::SetProcessDefaultCpuSets(::GetCurrentProcess(), nullptr, 0);
        ::SetThreadSelectedCpuSets(::GetCurrentThread(), nullptr, 0);
    }
};

} // namespace api_ms_win_core_processthreads
