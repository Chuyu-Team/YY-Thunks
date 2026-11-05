#include "pch.h"
#include "Thunks/api-ms-win-core-sysinfo.hpp"

namespace api_ms_win_core_sysinfo
{
TEST_CLASS(GetLogicalProcessorInformationEx)
{
    AwaysNullGuard Guard;

public:
    GetLogicalProcessorInformationEx()
    {
        Guard |= YY::Thunks::aways_null_try_get_GetLogicalProcessorInformationEx;
    }

    TEST_METHOD(常规验证)
    {
        DWORD len = 0;
        const auto _lStatus =  ::GetLogicalProcessorInformationEx(RelationAll, nullptr, &len) ? ERROR_SUCCESS : GetLastError();
        Assert::AreEqual(_lStatus, DWORD(ERROR_INSUFFICIENT_BUFFER));
        Assert::AreNotEqual(len, 0ul);

        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buf = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)malloc(len);
        Assert::IsTrue(::GetLogicalProcessorInformationEx(RelationAll, buf, &len));
        free(buf);
    }
};

#if (YY_Thunks_Target < __WindowsNT5_1_SP3)

// 匿名 union 无法用 offsetof 求偏移，按字段宽度与 8 字节对齐推导：x86 下 union 起于 8（sizeof = 24），x64 下起于 16（sizeof = 32，偏移 12-15 为 padding）。对应 SPEC §13.3。
static_assert(((sizeof(DWORD_PTR) + sizeof(LOGICAL_PROCESSOR_RELATIONSHIP) + 7) & ~size_t(7)) + 16 == sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION),
    "SYSTEM_LOGICAL_PROCESSOR_INFORMATION 布局与 SPEC §13.3 不一致");

TEST_CLASS(GetLogicalProcessorInformation)
{
    AwaysNullGuard Guard;

    /// <summary>
    /// 分配一块按 SYSTEM_LOGICAL_PROCESSOR_INFORMATION 对齐、字节容量不小于 _cbBytes 的缓冲区。
    /// </summary>
    /// <param name="_cbBytes">所需的最小字节容量，必须大于 0。</param>
    /// <returns>元素个数已向上取整到 DWORD_PTR 宽度的缓冲区。</returns>
    /// <remarks>
    /// 不用 std::vector&lt;BYTE&gt;，因为它只保证 1 字节对齐，把拓扑条目写入其中属于未定义行为。
    /// </remarks>
    static std::vector<DWORD_PTR> MakeBuffer(_In_ DWORD _cbBytes)
    {
        return std::vector<DWORD_PTR>((_cbBytes + sizeof(DWORD_PTR) - 1) / sizeof(DWORD_PTR));
    }

    /// <summary>
    /// 统计掩码中置位的位数。
    /// </summary>
    /// <param name="_Mask">待统计的处理器亲和性掩码，允许为 0（返回 0）。</param>
    /// <returns>掩码中置位的位数。</returns>
    static DWORD PopCount(_In_ DWORD_PTR _Mask)
    {
        DWORD _cBits = 0;
        for (; _Mask != 0; _Mask &= _Mask - 1)
        {
            ++_cBits;
        }
        return _cBits;
    }

    /// <summary>
    /// 取掩码中最低的置位位，作为核掩码的排序键。
    /// </summary>
    /// <param name="_Mask">处理器亲和性掩码，必须非 0。</param>
    /// <returns>只保留最低置位位的掩码。</returns>
    static DWORD_PTR LowBit(_In_ DWORD_PTR _Mask)
    {
        // 用 0 - _Mask 取补码而不是 ~_Mask，避免一元取反经整型提升后高位符号扩展。
        return _Mask & (DWORD_PTR(0) - _Mask);
    }

    /// <summary>
    /// 执行一次两段式调用的第一段（零长度探测），并校验其返回值语义。
    /// </summary>
    /// <returns>系统所需的缓冲区字节数，恒大于 0 且为条目大小的整数倍。</returns>
    static DWORD QueryRequiredLength()
    {
        DWORD _cbLength = 0;
        const BOOL _bRet = ::GetLogicalProcessorInformation(nullptr, &_cbLength);
        const DWORD _uError = _bRet != FALSE ? ERROR_SUCCESS : ::GetLastError();

        Assert::IsFalse(_bRet != FALSE, L"零长度探测必须返回 FALSE");
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), _uError, L"零长度探测必须返回 ERROR_INSUFFICIENT_BUFFER");
        Assert::IsTrue(_cbLength > 0, L"零长度探测必须回填大于 0 的所需长度");
        Assert::AreEqual(DWORD(0), _cbLength % DWORD(sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)), L"所需长度必须是条目大小的整数倍");

        return _cbLength;
    }

    /// <summary>
    /// 执行一次完整的两段式调用，并把结果复制成条目数组。
    /// </summary>
    /// <returns>按系统返回顺序排列的逻辑处理器拓扑条目。</returns>
    static std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> QueryAll()
    {
        DWORD _cbLength = QueryRequiredLength();
        std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> _Infos(_cbLength / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

        Assert::IsTrue(::GetLogicalProcessorInformation(_Infos.data(), &_cbLength) != FALSE, L"按所需长度填充缓冲区必须成功");
        Assert::AreEqual(DWORD(0), _cbLength % DWORD(sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)), L"回填长度必须是条目大小的整数倍");

        _Infos.resize(_cbLength / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        return _Infos;
    }

public:
    GetLogicalProcessorInformation()
    {
        Guard |= YY::Thunks::aways_null_try_get_GetLogicalProcessorInformation;
        // TC-018 经 GetLogicalProcessorInformationEx 的 fallback 反向调用 GLPI，两个 thunk 都必须被强制进入 fallback。
        Guard |= YY::Thunks::aways_null_try_get_GetLogicalProcessorInformationEx;
    }

    TEST_METHOD(ReturnedLength为空指针_返回ERROR_INVALID_PARAMETER)
    {
        ::SetLastError(ERROR_SUCCESS);
            // 本用例的目的正是验证 ReturnedLength == nullptr 时的参数校验分支，与 SAL 声明的 _Inout_ 冲突属预期。
    #pragma warning(suppress: 6387)
            Assert::IsFalse(::GetLogicalProcessorInformation(nullptr, nullptr) != FALSE, L"ReturnedLength 为空指针时必须返回 FALSE");
            Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), ::GetLastError(), L"ReturnedLength 为空指针时必须返回 ERROR_INVALID_PARAMETER");
        }

    TEST_METHOD(零长度查询_返回ERROR_INSUFFICIENT_BUFFER并回填所需长度)
    {
        DWORD _cbLength = 0;
        Assert::IsFalse(::GetLogicalProcessorInformation(nullptr, &_cbLength) != FALSE, L"零长度查询必须返回 FALSE");
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError(), L"零长度查询必须返回 ERROR_INSUFFICIENT_BUFFER");
        Assert::IsTrue(_cbLength > 0, L"零长度查询必须回填所需长度");
        Assert::AreEqual(DWORD(0), _cbLength % DWORD(sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)), L"所需长度必须是条目大小的整数倍");
        // 至少容纳一个 RelationProcessorCore 加一个 RelationNumaNode。
        Assert::IsTrue(_cbLength >= 2 * DWORD(sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)), L"所需长度必须至少容纳 Core 与 NumaNode 两个条目");

        std::vector<DWORD_PTR> _Buffer = MakeBuffer(_cbLength);
        memset(_Buffer.data(), 0xCC, _Buffer.size() * sizeof(DWORD_PTR));
        const std::vector<DWORD_PTR> _Snapshot = _Buffer;

        DWORD _cbLength2 = 0;
        Assert::IsFalse(::GetLogicalProcessorInformation(reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(_Buffer.data()), &_cbLength2) != FALSE,
            L"零长度 + 非空缓冲区必须返回 FALSE");
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError(), L"零长度 + 非空缓冲区必须返回 ERROR_INSUFFICIENT_BUFFER");
        Assert::AreEqual(_cbLength, _cbLength2, L"零长度 + 非空缓冲区必须回填与零长度探测一致的所需长度");
        Assert::IsTrue(memcmp(_Buffer.data(), _Snapshot.data(), _Buffer.size() * sizeof(DWORD_PTR)) == 0, L"零长度 + 非空缓冲区不得写入任何字节");
    }

    TEST_METHOD(空缓冲区但长度非零_返回ERROR_NOACCESS且不改写长度)
    {
        // 长度取 1 在任意拓扑下必然小于所需长度，一旦校验顺序颠倒就会走进「缓冲不足」分支而暴露问题。
        DWORD _cbLength = 1;
        Assert::IsFalse(::GetLogicalProcessorInformation(nullptr, &_cbLength) != FALSE, L"空缓冲区 + 非零长度必须返回 FALSE");
        Assert::AreEqual(DWORD(ERROR_NOACCESS), ::GetLastError(), L"空缓冲区 + 非零长度必须返回 ERROR_NOACCESS");
        Assert::AreEqual(DWORD(1), _cbLength, L"空缓冲区 + 非零长度不得改写 ReturnedLength");
    }

    TEST_METHOD(缓冲区差一字节_返回ERROR_INSUFFICIENT_BUFFER并回填所需长度)
    {
        const DWORD _cbRequired = QueryRequiredLength();
        std::vector<DWORD_PTR> _Buffer = MakeBuffer(_cbRequired);
        DWORD _cbLength = _cbRequired - 1;

        Assert::IsFalse(::GetLogicalProcessorInformation(reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(_Buffer.data()), &_cbLength) != FALSE,
            L"缓冲区不足时必须返回 FALSE");
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError(), L"缓冲区不足时必须返回 ERROR_INSUFFICIENT_BUFFER");
        Assert::AreEqual(_cbRequired, _cbLength, L"缓冲区不足时必须回填所需长度");
    }

    TEST_METHOD(缓冲区恰好等于所需长度_返回TRUE且长度不变)
    {
        const DWORD _cbRequired = QueryRequiredLength();
        std::vector<DWORD_PTR> _Buffer = MakeBuffer(_cbRequired);
        DWORD _cbLength = _cbRequired;

        ::SetLastError(0xDEADBEEF);
        Assert::IsTrue(::GetLogicalProcessorInformation(reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(_Buffer.data()), &_cbLength) != FALSE,
            L"缓冲区恰好足够时必须返回 TRUE");
        Assert::AreEqual(_cbRequired, _cbLength, L"缓冲区恰好足够时 ReturnedLength 必须保持为所需长度");
        Assert::AreEqual(DWORD(0xDEADBEEF), ::GetLastError(), L"成功路径不得改写 LastError");
    }

    TEST_METHOD(缓冲区大于所需长度_返回TRUE且长度回填为实际写入值)
    {
        const DWORD _cbRequired = QueryRequiredLength();
        std::vector<DWORD_PTR> _Buffer = MakeBuffer(_cbRequired + 4 * DWORD(sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)));
        DWORD _cbLength = static_cast<DWORD>(_Buffer.size() * sizeof(DWORD_PTR));

        Assert::IsTrue(::GetLogicalProcessorInformation(reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(_Buffer.data()), &_cbLength) != FALSE,
            L"缓冲区超额时必须返回 TRUE");
        Assert::AreEqual(_cbRequired, _cbLength, L"缓冲区超额时必须回填实际写入长度");
    }

    TEST_METHOD(返回长度按条目大小对齐)
    {
        const DWORD _cbRequired = QueryRequiredLength();
        Assert::AreEqual(DWORD(0), _cbRequired % DWORD(sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)), L"所需长度必须按条目大小对齐");
    }

    TEST_METHOD(条目顺序与关系类型集合_仅含Core与NumaNode且Core在前)
    {
        const auto _Infos = QueryAll();
        Assert::IsTrue(!_Infos.empty(), L"必须至少输出一个拓扑条目");

        bool _bNumaSeen = false;
        bool _bCoreKeyAscending = true;
        bool _bPaddingZero = true;
        DWORD_PTR _LastCoreKey = 0;
        for (const auto& _Info : _Infos)
        {
            const int _iRelationship = static_cast<int>(_Info.Relationship);
            Assert::IsTrue(_iRelationship == static_cast<int>(RelationProcessorCore) || _iRelationship == static_cast<int>(RelationNumaNode),
                L"条目关系类型只允许 RelationProcessorCore 或 RelationNumaNode");

            if (_Info.Relationship == RelationNumaNode)
            {
                _bNumaSeen = true;
                continue;
            }

            Assert::IsFalse(_bNumaSeen, L"RelationProcessorCore 条目必须全部排在 RelationNumaNode 之前");

            const DWORD_PTR _CoreKey = LowBit(_Info.ProcessorMask);
            if (_LastCoreKey != 0 && _CoreKey <= _LastCoreKey)
            {
                _bCoreKeyAscending = false;
            }
            _LastCoreKey = _CoreKey;

            // Relationship 与 union 之间的空隙：x86 下为空区间（循环体不执行），x64 下为偏移 12-15 的 padding。
                        // 用 while 而不是 for，避免分析器把 x86 下的空区间误判为 C6294「定义错误的 for-loop」。
                        const BYTE* _pRaw = reinterpret_cast<const BYTE*>(&_Info);
                        size_t _iPadding = sizeof(DWORD_PTR) + sizeof(LOGICAL_PROCESSOR_RELATIONSHIP);
                        while (_iPadding != sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) - 16)
                        {
                            if (_pRaw[_iPadding] != 0)
                            {
                                _bPaddingZero = false;
                            }
                            ++_iPadding;
                        }
        }

        Assert::IsTrue(_bCoreKeyAscending, L"RelationProcessorCore 条目必须按核掩码最低位升序排列");
        Assert::IsTrue(_bPaddingZero, L"结构体 padding 必须被确定性清零");
    }

    TEST_METHOD(输出不含RelationCache条目)
    {
        for (const auto& _Info : QueryAll())
        {
            Assert::AreNotEqual(static_cast<int>(RelationCache), static_cast<int>(_Info.Relationship), L"输出不得包含 RelationCache 条目");
        }
    }

    TEST_METHOD(输出不含RelationProcessorPackage条目)
    {
        for (const auto& _Info : QueryAll())
        {
            Assert::AreNotEqual(static_cast<int>(RelationProcessorPackage), static_cast<int>(_Info.Relationship), L"输出不得包含 RelationProcessorPackage 条目");
        }
    }

    TEST_METHOD(核掩码互不相交且并集等于系统亲和性掩码)
    {
        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);
        const DWORD_PTR _SystemMask = _SystemInfo.dwActiveProcessorMask;

        DWORD_PTR _Union = 0;
        std::vector<DWORD_PTR> _Masks;
        for (const auto& _Info : QueryAll())
        {
            if (_Info.Relationship != RelationProcessorCore)
            {
                continue;
            }

            Assert::IsTrue(_Info.ProcessorMask != 0, L"核掩码不得为 0");
            _Masks.push_back(_Info.ProcessorMask);
            _Union |= _Info.ProcessorMask;
        }

        Assert::IsTrue(!_Masks.empty(), L"必须至少输出一个 RelationProcessorCore 条目");

        for (size_t _i = 0; _i != _Masks.size(); ++_i)
        {
            for (size_t _j = _i + 1; _j != _Masks.size(); ++_j)
            {
                Assert::IsTrue((_Masks[_i] & _Masks[_j]) == 0, L"不同核掩码必须互不相交");
            }
        }

        const DWORD _cMaskBits = DWORD(sizeof(DWORD_PTR) * 8);
        if (_SystemInfo.dwNumberOfProcessors > _cMaskBits)
        {
            // 逻辑处理器数超过单个掩码位宽时 GetSystemInfo 只回报当前处理器组，故降级为包含关系 + 位宽校验。
            Assert::IsTrue((_Union & ~_SystemMask) == 0, L"核掩码并集必须落在系统亲和性掩码内");
            Assert::AreEqual(_cMaskBits, PopCount(_Union), L"核掩码并集必须覆盖掩码全部位宽");
        }
        else
        {
            Assert::IsTrue(_Union == _SystemMask, L"核掩码并集必须等于系统亲和性掩码");
        }
    }

    TEST_METHOD(核掩码全部落在系统亲和性掩码内)
    {
        SYSTEM_INFO _SystemInfo = {};
        ::GetSystemInfo(&_SystemInfo);
        const DWORD_PTR _SystemMask = _SystemInfo.dwActiveProcessorMask;

        for (const auto& _Info : QueryAll())
        {
            if (_Info.Relationship != RelationProcessorCore)
            {
                continue;
            }

            Assert::IsTrue((_Info.ProcessorMask & ~_SystemMask) == 0, L"核掩码不得超出系统亲和性掩码");
        }
    }

    TEST_METHOD(SMT标志与核内逻辑处理器数量一致)
    {
        DWORD _cCores = 0;
        for (const auto& _Info : QueryAll())
        {
            if (_Info.Relationship != RelationProcessorCore)
            {
                continue;
            }

            ++_cCores;
            const BYTE _bExpected = (PopCount(_Info.ProcessorMask) > 1) ? BYTE(1) : BYTE(0);
            Assert::AreEqual(_bExpected, _Info.ProcessorCore.Flags, L"ProcessorCore.Flags 必须与核内逻辑处理器数量一致");
        }

        Assert::IsTrue(_cCores != 0, L"必须至少输出一个 RelationProcessorCore 条目");
    }

    TEST_METHOD(NUMA节点唯一且NodeNumber为0)
    {
        DWORD _cNumaNodes = 0;
        for (const auto& _Info : QueryAll())
        {
            if (_Info.Relationship != RelationNumaNode)
            {
                continue;
            }

            ++_cNumaNodes;
            Assert::AreEqual(DWORD(0), _Info.NumaNode.NodeNumber, L"单节点合成下 NodeNumber 必须为 0");
        }

        Assert::AreEqual(DWORD(1), _cNumaNodes, L"必须恰好输出一个 RelationNumaNode 条目");
    }

    TEST_METHOD(NUMA掩码等于所有核掩码并集)
    {
        DWORD_PTR _CoreUnion = 0;
        DWORD_PTR _NumaMask = 0;
        DWORD _cNumaNodes = 0;
        for (const auto& _Info : QueryAll())
        {
            if (_Info.Relationship == RelationProcessorCore)
            {
                _CoreUnion |= _Info.ProcessorMask;
            }
            else if (_Info.Relationship == RelationNumaNode)
            {
                ++_cNumaNodes;
                _NumaMask = _Info.ProcessorMask;
            }
        }

        Assert::AreEqual(DWORD(1), _cNumaNodes, L"必须恰好输出一个 RelationNumaNode 条目");
        Assert::IsTrue(_NumaMask == _CoreUnion, L"NUMA 节点掩码必须等于所有核掩码的并集");
    }

    TEST_METHOD(调用前后线程亲和性掩码一致)
    {
        const HANDLE _hThread = ::GetCurrentThread();

        const DWORD_PTR _Mask0 = ::SetThreadAffinityMask(_hThread, 1);
        Assert::IsTrue(_Mask0 != 0, L"无法把当前线程绑定到 0 号处理器");
        Assert::IsTrue(::SetThreadAffinityMask(_hThread, _Mask0) != 0, L"无法恢复当前线程亲和性掩码");

        (void)QueryAll();

        const DWORD_PTR _Mask1 = ::SetThreadAffinityMask(_hThread, 1);
        Assert::IsTrue(_Mask1 != 0, L"无法再次把当前线程绑定到 0 号处理器");
        Assert::IsTrue(::SetThreadAffinityMask(_hThread, _Mask1) != 0, L"无法再次恢复当前线程亲和性掩码");

        Assert::IsTrue(_Mask0 == _Mask1, L"GetLogicalProcessorInformation 调用前后线程亲和性掩码必须一致");
    }

    TEST_METHOD(连续两次零长度查询返回相同所需长度)
    {
        DWORD _cbFirst = 0;
        DWORD _cbSecond = 0;

        Assert::IsFalse(::GetLogicalProcessorInformation(nullptr, &_cbFirst) != FALSE, L"首次零长度查询必须返回 FALSE");
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError(), L"首次零长度查询必须返回 ERROR_INSUFFICIENT_BUFFER");

        Assert::IsFalse(::GetLogicalProcessorInformation(nullptr, &_cbSecond) != FALSE, L"二次零长度查询必须返回 FALSE");
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError(), L"二次零长度查询必须返回 ERROR_INSUFFICIENT_BUFFER");

        Assert::AreEqual(_cbFirst, _cbSecond, L"连续两次零长度查询必须返回相同所需长度");
    }

    TEST_METHOD(GLPIEx在GLPI的Fallback基础上可用)
    {
        DWORD _cbLength = 0;
        Assert::IsFalse(::GetLogicalProcessorInformationEx(RelationNumaNode, nullptr, &_cbLength) != FALSE, L"GLPIEx 零长度探测必须返回 FALSE");
        Assert::AreEqual(DWORD(ERROR_INSUFFICIENT_BUFFER), ::GetLastError(), L"GLPIEx 零长度探测必须返回 ERROR_INSUFFICIENT_BUFFER");
        Assert::IsTrue(_cbLength > 0, L"GLPIEx 零长度探测必须回填所需长度");

        std::vector<DWORD_PTR> _Buffer = MakeBuffer(_cbLength);
        Assert::IsTrue(::GetLogicalProcessorInformationEx(RelationNumaNode, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(_Buffer.data()), &_cbLength) != FALSE,
            L"GLPIEx 按所需长度填充必须成功");
    }
};
#endif // (YY_Thunks_Target < __WindowsNT5_1_SP3)
}
