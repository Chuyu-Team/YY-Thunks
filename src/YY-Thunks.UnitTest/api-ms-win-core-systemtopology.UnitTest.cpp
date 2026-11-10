#include "pch.h"
#include "Thunks/api-ms-win-core-systemtopology.hpp"

namespace api_ms_win_core_systemtopology
{
TEST_CLASS(SetThreadGroupAffinity)
{
    AwaysNullGuard Guard;

    /// <summary>
    /// 测试期间临时修改当前线程的亲和性，析构时恢复原值，避免污染同一线程上的后续测试。
    /// </summary>
    class ThreadAffinityGuard
    {
    private:
        GROUP_AFFINITY _Original = {};
        bool _bValid = false;

    public:
        ThreadAffinityGuard()
        {
            _bValid = ::GetThreadGroupAffinity(GetCurrentThread(), &_Original) != FALSE;
        }

        ~ThreadAffinityGuard()
        {
            if (_bValid)
            {
                ::SetThreadGroupAffinity(GetCurrentThread(), &_Original, nullptr);
            }
        }

        ThreadAffinityGuard(const ThreadAffinityGuard&) = delete;
        ThreadAffinityGuard& operator=(const ThreadAffinityGuard&) = delete;

        bool IsValid() const
        {
            return _bValid;
        }

        const GROUP_AFFINITY& GetOriginal() const
        {
            return _Original;
        }
    };

    /// <summary>
    /// 取掩码中最低的置位位，用于构造一个一定属于进程亲和性掩码的单核掩码。
    /// </summary>
    static DWORD_PTR LowBit(_In_ DWORD_PTR _Mask)
    {
        return _Mask & (DWORD_PTR(0) - _Mask);
    }

    static DWORD_PTR GetProcessMask()
    {
        DWORD_PTR _ProcessAffinityMask = 0;
        DWORD_PTR _SystemAffinityMask = 0;
        if (!GetProcessAffinityMask(GetCurrentProcess(), &_ProcessAffinityMask, &_SystemAffinityMask))
        {
            return 0;
        }

        return _ProcessAffinityMask;
    }

public:
    SetThreadGroupAffinity()
    {
        Guard |= YY::Thunks::aways_null_try_get_SetThreadGroupAffinity;
        Guard |= YY::Thunks::aways_null_try_get_GetThreadGroupAffinity;
    }

    TEST_METHOD(常规验证)
    {
        const auto _ProcessAffinityMask = GetProcessMask();
        Assert::AreNotEqual(_ProcessAffinityMask, DWORD_PTR(0));

        ThreadAffinityGuard _Guard;
        Assert::IsTrue(_Guard.IsValid());

        GROUP_AFFINITY _Affinity = {};
        _Affinity.Group = 0;
        _Affinity.Mask = static_cast<KAFFINITY>(LowBit(_ProcessAffinityMask));

        GROUP_AFFINITY _Previous = {};
        Assert::IsTrue(::SetThreadGroupAffinity(GetCurrentThread(), &_Affinity, &_Previous) != FALSE);
        Assert::AreEqual(static_cast<DWORD_PTR>(_Guard.GetOriginal().Mask), static_cast<DWORD_PTR>(_Previous.Mask));
        Assert::AreEqual(WORD(0), _Previous.Group);

        GROUP_AFFINITY _Current = {};
        Assert::IsTrue(::GetThreadGroupAffinity(GetCurrentThread(), &_Current) != FALSE);
        Assert::AreEqual(static_cast<DWORD_PTR>(_Affinity.Mask), static_cast<DWORD_PTR>(_Current.Mask));
        Assert::AreEqual(WORD(0), _Current.Group);
    }

    TEST_METHOD(Mask为0时使用所属进程的亲和性掩码)
    {
        const auto _ProcessAffinityMask = GetProcessMask();
        Assert::AreNotEqual(_ProcessAffinityMask, DWORD_PTR(0));

        ThreadAffinityGuard _Guard;
        Assert::IsTrue(_Guard.IsValid());

        //先把当前线程限制到单个处理器，这样“线程当前的亲和性掩码”与“进程的亲和性掩码”不再相同。
        const auto _SingleMask = LowBit(_ProcessAffinityMask);

        GROUP_AFFINITY _Affinity = {};
        _Affinity.Group = 0;
        _Affinity.Mask = static_cast<KAFFINITY>(_SingleMask);
        Assert::IsTrue(::SetThreadGroupAffinity(GetCurrentThread(), &_Affinity, nullptr) != FALSE);

        GROUP_AFFINITY _Current = {};
        Assert::IsTrue(::GetThreadGroupAffinity(GetCurrentThread(), &_Current) != FALSE);
        Assert::AreEqual(static_cast<DWORD_PTR>(_SingleMask), static_cast<DWORD_PTR>(_Current.Mask));

        //关键回归点：Mask为0时应该恢复成进程的亲和性掩码，而不是保持线程当前的亲和性掩码。
        GROUP_AFFINITY _ZeroAffinity = {};
        GROUP_AFFINITY _Previous = {};
        Assert::IsTrue(::SetThreadGroupAffinity(GetCurrentThread(), &_ZeroAffinity, &_Previous) != FALSE);
        Assert::AreEqual(static_cast<DWORD_PTR>(_SingleMask), static_cast<DWORD_PTR>(_Previous.Mask));

        Assert::IsTrue(::GetThreadGroupAffinity(GetCurrentThread(), &_Current) != FALSE);
        Assert::AreEqual(_ProcessAffinityMask, static_cast<DWORD_PTR>(_Current.Mask));
    }

    TEST_METHOD(无效参数)
    {
        GROUP_AFFINITY _Affinity = {};
        _Affinity.Group = 1;
        _Affinity.Mask = 1;

        SetLastError(ERROR_SUCCESS);
        Assert::IsFalse(::SetThreadGroupAffinity(GetCurrentThread(), &_Affinity, nullptr) != FALSE);
        Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), GetLastError());

        SetLastError(ERROR_SUCCESS);
        Assert::IsFalse(::SetThreadGroupAffinity(GetCurrentThread(), nullptr, nullptr) != FALSE);
        Assert::AreEqual(DWORD(ERROR_NOACCESS), GetLastError());
    }
};
}
