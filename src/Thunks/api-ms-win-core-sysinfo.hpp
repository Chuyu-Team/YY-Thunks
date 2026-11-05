#if (YY_Thunks_Target < __WindowsNT6_SP1)
#include <Shared/SmBios.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista, Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    0,
    ULONGLONG,
    WINAPI,
    GetTickCount64,
        VOID
        )
    {
        if (auto const pGetTickCount64 = try_get_GetTickCount64())
        {
            return pGetTickCount64();
        }

        __pragma(warning(suppress:28159))
        return GetTickCount();
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8, Windows Server 2012
    __DEFINE_THUNK(
    kernel32,
    4,
    VOID,
    WINAPI,
    GetSystemTimePreciseAsFileTime,
        _Out_ LPFILETIME lpSystemTimeAsFileTime
        )
    {
        if (auto const pGetSystemTimePreciseAsFileTime = try_get_GetSystemTimePreciseAsFileTime())
        {
            return pGetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
        }

        return GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1_SP3)

    //Windows Vista, Windows XP Professional x64 Edition, Windows XP with SP3 [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    8,
    _Success_(return)
    BOOL,
    WINAPI,
    GetLogicalProcessorInformation,
        _Out_writes_bytes_to_opt_(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
        _Inout_ PDWORD ReturnedLength
        )
    {
        if (auto pGetLogicalProcessorInformation = try_get_GetLogicalProcessorInformation())
        {
            return pGetLogicalProcessorInformation(Buffer, ReturnedLength);
        }

        // 成功路径需原样恢复 LastError（原生实现不改写它）
        const auto _uLastError = GetLastError();

        // [阶段 1] 参数校验。顺序不可调整：(NULL, 100) 必须得到 ERROR_NOACCESS 而不是 ERROR_INSUFFICIENT_BUFFER。
        if (ReturnedLength == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if (Buffer == nullptr && *ReturnedLength != 0)
        {
            SetLastError(ERROR_NOACCESS);
            return FALSE;
        }

        struct LogicalProcessorSample
        {
            DWORD_PTR ProcessorMask;    // 该处理器在系统亲和性掩码中的单 bit
            DWORD uApicId;              // x2APIC ID；不可用时为 0xFFFFFFFF
            DWORD uCoreKey;             // 归并键
            bool bSampled;              // 是否成功采样
        };

        struct ProcessorCoreInfo
        {
            DWORD_PTR ProcessorMask;    // 该核包含的逻辑处理器位掩码
            DWORD uCoreKey;             // 归并键；kNoCoreKey 表示不与任何处理器合并
            BYTE Flags;                 // 1 = 含多个逻辑处理器（SMT），0 = 否则
        };

        // 编译期上限，避免动态分配。本 fallback 的守卫是 YY_Thunks_Target < __WindowsNT5_1_SP3，而 x64 / ARM64 的
        // __WindowsMinTarget 均高于该阈值（见 SharedDefs.h），故只有 x86-32 会编译到这里，亲和性掩码最多 32 位。
        const DWORD kMaxLogicalProcessors = 32;


        // [阶段 2] 采样。GetSystemInfo 自 Windows 2000 起原生存在、无 thunk，WOW64 下返回进程视图。
        SYSTEM_INFO _SystemInfo;
        GetSystemInfo(&_SystemInfo);

        DWORD _cProcessorLimit = _SystemInfo.dwNumberOfProcessors;
        if (_cProcessorLimit > kMaxLogicalProcessors)
        {
            _cProcessorLimit = kMaxLogicalProcessors;
        }

        LogicalProcessorSample _Samples[kMaxLogicalProcessors];
        DWORD _cSamples = 0;

        for (DWORD _iBit = 0; _iBit != kMaxLogicalProcessors && _cSamples != _cProcessorLimit; ++_iBit)
        {
            const DWORD_PTR _uBit = ((DWORD_PTR)1) << _iBit;
            if ((_SystemInfo.dwActiveProcessorMask & _uBit) == 0)
            {
                continue;
            }

            auto& _Sample = _Samples[_cSamples];
            _Sample.ProcessorMask = _uBit;
            _Sample.uApicId = 0xFFFFFFFF;
            _Sample.uCoreKey = _iBit;
            _Sample.bSampled = false;
            ++_cSamples;
        }

        if (_cSamples == 0)
        {
            // GetSystemInfo 返回了空掩码（理论上不可达）：保底 1 个逻辑处理器，保证 cbRequired 恒不为 0。
            _Samples[0].ProcessorMask = 1;
            _Samples[0].uApicId = 0xFFFFFFFF;
            _Samples[0].uCoreKey = 0;
            _Samples[0].bSampled = false;
            _cSamples = 1;
        }


        // CPUID 读取：本 fallback 的守卫是 YY_Thunks_Target < __WindowsNT5_1_SP3，而 x64 / ARM64 的 __WindowsMinTarget
        // 均高于该阈值（见 SharedDefs.h），故只有 x86-32 会编译到这里，直接用内联汇编即可（VC6 起可用，且能指定子叶）。
        struct CpuidReader
        {
            /// <summary>
            /// 读取指定 CPUID 叶 / 子叶的 4 个 32 位返回值。
            /// </summary>
            /// <param name="uLeaf">CPUID 叶号，如 0 / 1 / 0x0B / 0x1F / 4 / 0x8000001D。</param>
            /// <param name="uSubLeaf">子叶号；叶不支持子叶时传 0。</param>
            /// <param name="pRegs">输出缓冲区，必须指向至少 4 个连续 DWORD；依次写入 EAX / EBX / ECX / EDX。</param>
            void Read(DWORD uLeaf, DWORD uSubLeaf, DWORD* pRegs) const
            {
                __asm
                {
                    push ebx
                    push esi
                    mov eax, uLeaf
                    mov ecx, uSubLeaf
                    cpuid
                    mov esi, pRegs
                    mov [esi], eax
                    mov [esi + 4], ebx
                    mov [esi + 8], ecx
                    mov [esi + 12], edx
                    pop esi
                    pop ebx
                }
            }
        } _Cpuid;

        DWORD _Regs[4];

        // 叶 0：EAX 为最大基础叶号，EBX/EDX/ECX 依次拼出 12 字节厂商串。
        _Cpuid.Read(0, 0, _Regs);

        const DWORD _cMaxBasicLeaf = _Regs[0];
        const bool _bVendorIntel = _Regs[1] == 0x756E6547 && _Regs[3] == 0x49656E69 && _Regs[2] == 0x6C65746E;
        const bool _bVendorAmd = _Regs[1] == 0x68747541 && _Regs[3] == 0x69746E65 && _Regs[2] == 0x444D4163;

        // 叶 1：EDX[28] 为 HTT 能力位，EBX[23:16] 为每封装可寻址逻辑处理器数（不是每核线程数）。
        bool _bHttCapable = false;
        DWORD _cLogicalPerPackage = 0;

        if (_cMaxBasicLeaf >= 1)
        {
            _Cpuid.Read(1, 0, _Regs);
            _bHttCapable = (_Regs[3] & (1u << 28)) != 0;
            _cLogicalPerPackage = (_Regs[1] >> 16) & 0xFF;
        }

        // x2APIC 拓扑叶：先试 0x0B（兼容面更广），子叶 0 的 ECX[15:8] 为 0 表示该叶不可用，再试 0x1F。
        // 硬件不支持时 _uTopologyLeaf 保持 0，退化为 T2。
        DWORD _uTopologyLeaf = 0;

        if (_cMaxBasicLeaf >= 0x0B)
        {
            _Cpuid.Read(0x0B, 0, _Regs);
            if (((_Regs[2] >> 8) & 0xFF) != 0)
            {
                _uTopologyLeaf = 0x0B;
            }
        }

        if (_uTopologyLeaf == 0 && _cMaxBasicLeaf >= 0x1F)
        {
            _Cpuid.Read(0x1F, 0, _Regs);
            if (((_Regs[2] >> 8) & 0xFF) != 0)
            {
                _uTopologyLeaf = 0x1F;
            }
        }

        // 「每核线程数」只在无 x2APIC 拓扑叶时用于 T2 归并。默认 1：退化形态只缺失 SMT 关系，不会虚报核数。
        DWORD _cThreadsPerCore = 1;
        bool _bThreadsPerCoreKnown = false;

        if (_uTopologyLeaf == 0 && _bHttCapable)
        {
            if (_bVendorIntel && _cMaxBasicLeaf >= 4)
            {
                // 叶 4 子叶 0：EAX[25:14]+1 为共享该缓存的逻辑处理器数；L1/L2 每核独占，故即为每核线程数。
                _Cpuid.Read(4, 0, _Regs);

                const DWORD _uCacheType = _Regs[0] & 0x1F;
                const DWORD _uCacheLevel = (_Regs[0] >> 5) & 0x7;
                if (_uCacheType != 0 && _uCacheLevel <= 2)
                {
                    _cThreadsPerCore = ((_Regs[0] >> 14) & 0xFFF) + 1;
                    _bThreadsPerCoreKnown = true;
                }
            }
            else if (_bVendorAmd)
            {
                _Cpuid.Read(0x80000000, 0, _Regs);
                const DWORD _cMaxExtLeaf = _Regs[0];

                if (_cMaxExtLeaf >= 0x8000001D)
                {
                    _Cpuid.Read(0x8000001D, 0, _Regs);

                    const DWORD _uCacheType = _Regs[0] & 0x1F;
                    const DWORD _uCacheLevel = (_Regs[0] >> 5) & 0x7;
                    if (_uCacheType != 0 && _uCacheLevel <= 2)
                    {
                        _cThreadsPerCore = ((_Regs[0] >> 14) & 0xFFF) + 1;
                        _bThreadsPerCoreKnown = true;
                    }
                }

                if (!_bThreadsPerCoreKnown && _cMaxExtLeaf >= 0x80000008 && _cLogicalPerPackage != 0)
                {
                    // 叶 0x80000008 的 ECX[7:0]+1 为物理核数，用它反推每核线程数。
                    _Cpuid.Read(0x80000008, 0, _Regs);

                    const DWORD _cPhysicalCores = (_Regs[2] & 0xFF) + 1;
                    if (_cPhysicalCores != 0 && _cLogicalPerPackage >= _cPhysicalCores)
                    {
                        _cThreadsPerCore = _cLogicalPerPackage / _cPhysicalCores;
                        _bThreadsPerCoreKnown = true;
                    }
                }
            }

            if (_cThreadsPerCore == 0)
            {
                _cThreadsPerCore = 1;
            }
        }

        // 整段逐处理器采样共用一个绑定守卫：构造时切到第一个处理器，扫描过程中用 Retarget() 逐个前进，
        // 析构时统一恢复真正的原掩码，任何 break 都不会泄漏绑定，调用前后线程亲和性保持一致。
        struct AffinityGuard
        {
            HANDLE _hThread;
            DWORD_PTR _uOldMask;

            AffinityGuard(HANDLE hThread, DWORD_PTR uNewMask)
                : _hThread(hThread)
                , _uOldMask(SetThreadAffinityMask(hThread, uNewMask))
            {
            }

            ~AffinityGuard()
            {
                if (_uOldMask != 0)
                {
                    SetThreadAffinityMask(_hThread, _uOldMask);
                }
            }

            bool IsBound() const
            {
                return _uOldMask != 0;
            }

            // 把线程前进到下一个处理器。返回 false 表示系统不允许修改本线程的亲和性。
            bool Retarget(DWORD_PTR uNewMask) const
            {
                return SetThreadAffinityMask(_hThread, uNewMask) != 0;
            }
        };

        // 拓扑叶的子叶结构（级类型、各级共享计数）对所有逻辑处理器一致，只有 EDX 的 x2APIC ID 逐处理器不同。
        // 因此子叶枚举只在循环外做一次，循环内每个处理器只读「最末有效子叶」拿 x2APIC ID 这一次 CPUID。
        // 实测纯 CPUID 几乎免费（32 处理器各 100 轮共 3200 次为 0 ms），减少次数对总耗时影响可忽略，
        // 此处的主要价值是消除每处理器重复探测子叶结构的冗余，而不是节省 CPUID 本身。
        DWORD _cSmtsPerCore = 0;
        DWORD _cThreadsPerPackage = 0;
        DWORD _uApicIdSubLeaf = 0xFFFFFFFF;

        if (_uTopologyLeaf != 0)
        {
            for (DWORD _uSubLeaf = 0; _uSubLeaf != 32; ++_uSubLeaf)
            {
                _Cpuid.Read(_uTopologyLeaf, _uSubLeaf, _Regs);

                const DWORD _uLevelType = (_Regs[2] >> 8) & 0xFF;
                if (_uLevelType == 0)
                {
                    break;
                }

                // 每级子叶的 EDX 都是从位 0 起编码的 x2APIC ID，级越高有效位越多，故取最后一个有效子叶。
                _uApicIdSubLeaf = _uSubLeaf;

                if (_uLevelType == 1)
                {
                    // EBX[15:0] 为共享同一核的逻辑处理器数
                    _cSmtsPerCore = _Regs[1] & 0xFFFF;
                }
                else if (_uLevelType == 2)
                {
                    // EBX[15:0] 为每封装逻辑处理器数，用于校验 SMT 计数
                    _cThreadsPerPackage = _Regs[1] & 0xFFFF;
                }
            }
        }

        // 只有存在 SMT 时才需要逐处理器 APIC ID：每核单线程时 T1 的商恒等于 APIC ID、T2 的商恒等于位序号，
        // 归并结果与 T3「每位一核」完全一致，APIC ID 取值不影响任何输出。跳过扫描可把无 SMT 平台的
        // 成本从 _cSamples + 1 次跨 CPU 迁移降到 0 次（迁移是本函数唯一的可观测开销来源）。
        const bool _bNeedApicId = _cSmtsPerCore > 1 || _cThreadsPerCore > 1;

        if (_cMaxBasicLeaf >= 1 && _bNeedApicId)
        {
            const HANDLE _hThread = GetCurrentThread();

            // 顺序扫描而不是「逐个切过去再切回」：本实现共 _cSamples + 1 次 SetThreadAffinityMask，
            // 后者需 2 * _cSamples 次；而实测该调用的耗时几乎全部来自真实跨 CPU 迁移，
            // 故往返切换的迁移次数是顺序扫描的两倍。32 逻辑处理器各扫 100 轮实测：125 ms vs 266 ms。
            AffinityGuard _Affinity(_hThread, _Samples[0].ProcessorMask);
            if (_Affinity.IsBound())
            {
                for (DWORD _i = 0; _i != _cSamples; ++_i)
                {
                    if (_i != 0 && !_Affinity.Retarget(_Samples[_i].ProcessorMask))
                    {
                        // 不允许继续修改线程亲和性：放弃剩余处理器的采样，已采样的仍按 T1 / T2 归并。
                        break;
                    }

                    if (_uApicIdSubLeaf != 0xFFFFFFFF)
                    {
                        _Cpuid.Read(_uTopologyLeaf, _uApicIdSubLeaf, _Regs);
                        _Samples[_i].uApicId = _Regs[3];
                    }

                    _Samples[_i].bSampled = true;
                }
            }
            // 首个处理器都绑不上去时一个都不采样，由 _Affinity 析构保证亲和性不变，阶段 3 自动降级到 T3。
        }

        // SMT 计数不可能超过每封装逻辑处理器数；越界说明拓扑叶不可信，按 T2 处理。
        if (_cThreadsPerPackage != 0 && _cSmtsPerCore > _cThreadsPerPackage)
        {
            _cSmtsPerCore = 0;
        }


        // [阶段 3] 归并。三路降级（SPEC §6.2）：T1 用「APIC ID / 每核线程数」推导核键；T2 在无拓扑叶的旧平台上
        // 按活跃位序号分组；T3 每位一核。恒输出单 NUMA 节点。
        const DWORD kNoCoreKey = 0xFFFFFFFF;

        ProcessorCoreInfo _Cores[kMaxLogicalProcessors];
        DWORD _cCores = 0;
        DWORD_PTR _NumaMask = 0;

        for (DWORD _i = 0; _i != _cSamples; ++_i)
        {
            LogicalProcessorSample& _Sample = _Samples[_i];
            _NumaMask |= _Sample.ProcessorMask;

            if (!_Sample.bSampled)
            {
                // 采样失败时拿不到任何拓扑线索，独立成核并保持 Flags = 0，宁可多报核也不误并。
                _Sample.uCoreKey = kNoCoreKey;
            }
            else if (_Sample.uApicId != 0xFFFFFFFF && _cSmtsPerCore != 0)
            {
                // T1：同一核内的逻辑处理器共享「APIC ID / 每核线程数」这个商。
                _Sample.uCoreKey = _Sample.uApicId / _cSmtsPerCore;
            }
            else if (_cThreadsPerCore > 1)
            {
                // T2：没有可用 APIC ID 时只能按活跃位序号分组——旧平台按物理顺序给逻辑处理器编号，
                // 连续的 _cThreadsPerCore 个活跃位同属一核。此处必须是「活跃位序号」而不是位掩码位号，
                // 否则掩码不从 bit0 开始时会把同一核的线程拆到两个核里。
                _Sample.uCoreKey = _i / _cThreadsPerCore;
            }
            else
            {
                _Sample.uCoreKey = kNoCoreKey;
            }

            DWORD _j = 0;
            for (; _j != _cCores; ++_j)
            {
                if (_Sample.uCoreKey != kNoCoreKey && _Cores[_j].uCoreKey == _Sample.uCoreKey)
                {
                    _Cores[_j].ProcessorMask |= _Sample.ProcessorMask;
                    break;
                }
            }

            if (_j == _cCores)
            {
                _Cores[_cCores].ProcessorMask = _Sample.ProcessorMask;
                _Cores[_cCores].uCoreKey = _Sample.uCoreKey;
                _Cores[_cCores].Flags = 0;
                ++_cCores;
            }
        }

        // 按掩码最低置位升序排列。条目数 <= kMaxLogicalProcessors，插入排序足够且全程无堆分配。
        for (DWORD _i = 1; _i < _cCores; ++_i)
        {
            const ProcessorCoreInfo _Pending = _Cores[_i];
            // _uMask & (0 - _uMask) 取出最低置位，用它的数值比较等价于比较位号，且无需按位宽分支。
            const DWORD_PTR _uPendingLow = _Pending.ProcessorMask & ((DWORD_PTR)0 - _Pending.ProcessorMask);

            DWORD _j = _i;
            for (; _j != 0; --_j)
            {
                const DWORD_PTR _uPrevLow = _Cores[_j - 1].ProcessorMask & ((DWORD_PTR)0 - _Cores[_j - 1].ProcessorMask);
                if (_uPrevLow <= _uPendingLow)
                {
                    break;
                }

                _Cores[_j] = _Cores[_j - 1];
            }

            _Cores[_j] = _Pending;
        }

        // Flags 语义：该核含多个逻辑处理器（SMT）时为 1。掩码必然非空，故「清掉最低位后是否仍有位」即可判定。
        for (DWORD _i = 0; _i != _cCores; ++_i)
        {
            const DWORD_PTR _uRest = _Cores[_i].ProcessorMask & (_Cores[_i].ProcessorMask - 1);
            _Cores[_i].Flags = (_uRest != 0) ? 1 : 0;
        }


        // [阶段 4] 落盘。顺序固定为「所有 Core 在前，单条 NumaNode 在后」。
        const DWORD _cbRequired = (_cCores + 1) * sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);

        if (*ReturnedLength < _cbRequired)
        {
            *ReturnedLength = _cbRequired;
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        // 此处 Buffer 必然非空：Buffer == nullptr 且 *ReturnedLength == 0 已落入上面的「长度不足」分支。
        auto _pInfo = Buffer;

        for (DWORD _i = 0; _i != _cCores; ++_i, ++_pInfo)
        {
            memset(_pInfo, 0, sizeof(*_pInfo));
            _pInfo->Relationship = RelationProcessorCore;
            _pInfo->ProcessorMask = _Cores[_i].ProcessorMask;
            // Flags 位于匿名联合体的 ProcessorCore 命名成员内，不能写成 _pInfo->Flags。
            _pInfo->ProcessorCore.Flags = _Cores[_i].Flags;
        }

        memset(_pInfo, 0, sizeof(*_pInfo));
        _pInfo->Relationship = RelationNumaNode;
        _pInfo->ProcessorMask = _NumaMask;
        _pInfo->NumaNode.NodeNumber = 0;

        *ReturnedLength = _cbRequired;
        SetLastError(_uLastError);
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    //Windows 7, Windows Server 2008 R2
    __DEFINE_THUNK(
    kernel32,
    12,
    _Success_(return)
    BOOL,
    WINAPI,
    GetLogicalProcessorInformationEx,
        _In_ LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
        _Out_writes_bytes_to_opt_(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Buffer,
        _Inout_ PDWORD ReturnedLength
        )
    {
        if (auto pGetLogicalProcessorInformationEx = try_get_GetLogicalProcessorInformationEx())
        {
            return pGetLogicalProcessorInformationEx(RelationshipType, Buffer, ReturnedLength);
        }


        if (!ReturnedLength)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
    
        const auto cbBuffer = *ReturnedLength;

        if (cbBuffer != 0 && Buffer == nullptr)
        {
            SetLastError(ERROR_NOACCESS);
            return FALSE;
        }
    
        const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
        LSTATUS lStatus = ERROR_SUCCESS;

        SYSTEM_LOGICAL_PROCESSOR_INFORMATION* pProcessorInfo = nullptr;
        DWORD cbLogicalProcessorInformation = 0;

    
        for (; GetLogicalProcessorInformation(pProcessorInfo, &cbLogicalProcessorInformation) == FALSE;)
        {
            lStatus = GetLastError();

            if (ERROR_INSUFFICIENT_BUFFER == lStatus)
            {
                if (pProcessorInfo)
                {
                    auto pNewBuffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)HeapReAlloc(ProcessHeap, 0, pProcessorInfo, cbLogicalProcessorInformation);
                    if (pNewBuffer)
                    {
                        pProcessorInfo = pNewBuffer;
                        continue;
                    }
                }
                else
                {
                    pProcessorInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)HeapAlloc(ProcessHeap, 0, cbLogicalProcessorInformation);

                    if (pProcessorInfo)
                        continue;
                }

                lStatus = ERROR_NOT_ENOUGH_MEMORY;
            }

            goto __End;
        }

        lStatus = ERROR_SUCCESS;

        {
            const auto pProcessorInfoLastItem = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)((byte*)pProcessorInfo + cbLogicalProcessorInformation - sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

            DWORD cbBufferUsed = 0;



            for (auto pProcessorInfoStart = pProcessorInfo; pProcessorInfoStart <= pProcessorInfoLastItem; ++pProcessorInfoStart)
            {
                if (RelationshipType == LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll
                    || pProcessorInfoStart->Relationship == RelationshipType)
                {
                    DWORD cbInfoNeed;

                    switch (pProcessorInfoStart->Relationship)
                    {
                    case RelationProcessorCore:
                    case RelationProcessorPackage:
                        cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Processor);
                        break;
                    case RelationNumaNode:
                        cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, NumaNode);
                        break;
                    case RelationCache:
                        cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Cache);
                        break;
                    default:
                        cbInfoNeed = 0;
                        break;
                    }

                    //不支持传输此类型
                    if (0 == cbInfoNeed)
                        continue;

                    auto pInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)((byte*)Buffer + cbBufferUsed);
                    cbBufferUsed += cbInfoNeed;

                    if (cbBuffer >= cbBufferUsed)
                    {
                        memset(pInfo, 0, cbInfoNeed);

                        pInfo->Size = cbInfoNeed;
                        pInfo->Relationship = pProcessorInfoStart->Relationship;

                        switch (pProcessorInfoStart->Relationship)
                        {
                        case RelationProcessorCore:
                        case RelationProcessorPackage:
                            pInfo->Processor.Flags = pProcessorInfoStart->ProcessorCore.Flags;
                            pInfo->Processor.GroupCount = 1;
                            pInfo->Processor.GroupMask->Mask = pProcessorInfoStart->ProcessorMask;
                            break;
                        case RelationNumaNode:
                            pInfo->NumaNode.NodeNumber = pProcessorInfoStart->NumaNode.NodeNumber;
                            pInfo->NumaNode.GroupMask.Mask = pProcessorInfoStart->ProcessorMask;
                            break;
                        case RelationCache:
                            pInfo->Cache.Level = pProcessorInfoStart->Cache.Level;
                            pInfo->Cache.Associativity = pProcessorInfoStart->Cache.Associativity;
                            pInfo->Cache.LineSize = pProcessorInfoStart->Cache.LineSize;
                            pInfo->Cache.CacheSize = pProcessorInfoStart->Cache.Size;
                            pInfo->Cache.Type = pProcessorInfoStart->Cache.Type;
                            pInfo->Cache.GroupMask.Mask= pProcessorInfoStart->ProcessorMask;
                            break;
                        }
                    }
                }
            }


            //传输 RelationGroup 信息，这里只能假设只有一组CPU
            if (RelationshipType == LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll
                || RelationGroup == RelationshipType)
            {
                auto pInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)((byte*)Buffer + cbBufferUsed);
                const auto cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Group);
                cbBufferUsed += cbInfoNeed;

                if (cbBuffer >= cbBufferUsed)
                {
                    memset(pInfo, 0, cbInfoNeed);

                    pInfo->Size = cbInfoNeed;
                    pInfo->Relationship = RelationGroup;

                    pInfo->Group.ActiveGroupCount = 1;
                    pInfo->Group.MaximumGroupCount = 1;

                    SYSTEM_INFO SystemInfo;

                    GetSystemInfo(&SystemInfo);

                    pInfo->Group.GroupInfo->ActiveProcessorMask = SystemInfo.dwActiveProcessorMask;
                    pInfo->Group.GroupInfo->ActiveProcessorCount = internal::BitsCount(static_cast<size_t>(SystemInfo.dwActiveProcessorMask));
                    pInfo->Group.GroupInfo->MaximumProcessorCount = static_cast<BYTE>(SystemInfo.dwNumberOfProcessors);
                }
            }

            *ReturnedLength = cbBufferUsed;

            if (cbBufferUsed > cbBuffer)
            {
                //缓冲区不足
                lStatus = ERROR_INSUFFICIENT_BUFFER;
            }
        }

    __End:
        if (pProcessorInfo)
            HeapFree(ProcessHeap, 0, pProcessorInfo);

        if (lStatus != ERROR_SUCCESS)
        {
            SetLastError(lStatus);

            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1)

    //Windows XP [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    4,
    VOID,
    WINAPI,
    GetNativeSystemInfo,
        _Out_ LPSYSTEM_INFO lpSystemInfo
        )
    {
        if (const auto pGetNativeSystemInfo = try_get_GetNativeSystemInfo())
        {
            return pGetNativeSystemInfo(lpSystemInfo);
        }

        GetSystemInfo(lpSystemInfo);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    20,
    BOOL,
    WINAPI,
    GetProductInfo,
        _In_ DWORD dwOSMajorVersion,
        _In_ DWORD dwOSMinorVersion,
        _In_ DWORD dwSpMajorVersion,
        _In_ DWORD dwSpMinorVersion,
        _Out_ PDWORD pdwReturnedProductType
        )
    {
        if (const auto pGetProductInfo = try_get_GetProductInfo())
        {
            return pGetProductInfo(dwOSMajorVersion, dwOSMinorVersion, dwSpMajorVersion, dwSpMinorVersion, pdwReturnedProductType);
        }

        if (!pdwReturnedProductType)
            return FALSE;

        //故意不检测版本号，能凑合就尽可能的凑合吧……

        DWORD dwReturnedProductType = PRODUCT_UNLICENSED;

        //由于不支持这个接口，所以系统版本必然是XP或者以下
        OSVERSIONINFOEXW VersionInfo = { sizeof(VersionInfo) };
        #pragma warning(suppress:4996 28159)
        if (GetVersionExW((LPOSVERSIONINFOW)&VersionInfo))
        {
            if (VersionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
            {
                dwReturnedProductType = PRODUCT_ENTERPRISE_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_SMALLBUSINESS_RESTRICTED) // VER_SUITE_SMALLBUSINESS 与 VER_SUITE_SMALLBUSINESS_RESTRICTED
            {
                dwReturnedProductType = PRODUCT_SMALLBUSINESS_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_EMBEDDED_RESTRICTED) // VER_SUITE_EMBEDDEDNT 与 VER_SUITE_EMBEDDED_RESTRICTED
            {
                dwReturnedProductType = PRODUCT_EMBEDDED;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_DATACENTER)
            {
                dwReturnedProductType = PRODUCT_DATACENTER_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_PERSONAL)
            {
                dwReturnedProductType = PRODUCT_HOME_BASIC;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_BLADE)
            {
                dwReturnedProductType = PRODUCT_WEB_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_STORAGE_SERVER)
            {
                dwReturnedProductType = PRODUCT_STORAGE_STANDARD_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
            {
                dwReturnedProductType = PRODUCT_CLUSTER_SERVER;
            }
            else if (VersionInfo.wSuiteMask & VER_SUITE_WH_SERVER)
            {
                dwReturnedProductType = PRODUCT_HOME_SERVER;
            }
            else
            {
                if (VersionInfo.wProductType == VER_NT_WORKSTATION)
                {
                    //如果目标系统是5.1，因为SuiteMask中检测了Home，所以这里只可能是专业版
                    // 
                    //如果目标系统是5.0、5.2，只可能是专业版。

                    dwReturnedProductType = PRODUCT_PROFESSIONAL;
                }
                else if (VersionInfo.wProductType == VER_NT_DOMAIN_CONTROLLER || VersionInfo.wProductType == VER_NT_SERVER)
                {
                    //如果目标系统是5.1、5.2，因为SuiteMask中检测了Small Business Server、Web Edition、Enterprise Edition、
                    //Datacenter Edition、Compute Cluster Server、Storage Server，所以这里只可能是Standard Edition
                    //
                    //如果目标系统是5.0，因为SuiteMask中检测了Datacenter Server，这里只能是 Advanced Server或者 Server，但是我没有找到 Advanced，姑且用 PRODUCT_STANDARD_SERVER 凑合吧。
                    dwReturnedProductType = PRODUCT_STANDARD_SERVER;
                }
                else
                {
                    //这是什么鬼？
                    dwReturnedProductType = PRODUCT_UNDEFINED;
                }
            }
        }

        *pdwReturnedProductType = dwReturnedProductType;

        return dwReturnedProductType != PRODUCT_UNDEFINED;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    // 最低受支持的客户端	Windows Vista、Windows XP Professional x64 版本 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008、Windows Server 2003 SP1[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    UINT,
    WINAPI,
    GetSystemFirmwareTable,
        _In_ DWORD _uFirmwareTableProviderSignature,
        _In_ DWORD _uFirmwareTableID,
        _Out_writes_bytes_to_opt_(_cbBufferSize,return) PVOID _pFirmwareTableBuffer,
        _In_ DWORD _cbBufferSize
        )
    {
        if (const auto _pfnGetSystemFirmwareTable = try_get_GetSystemFirmwareTable())
        {
            return _pfnGetSystemFirmwareTable(_uFirmwareTableProviderSignature, _uFirmwareTableID, _pFirmwareTableBuffer, _cbBufferSize);
        }

        UINT _cbBufferNeed = 0;
        LSTATUS _lStatus = ERROR_INVALID_FUNCTION;

        if (_uFirmwareTableProviderSignature == 'RSMB')
        {
#if !defined(__USING_NTDLL_LIB)
            const auto NtOpenSection = try_get_NtOpenSection();
            if (!NtOpenSection)
            {
                SetLastError(ERROR_INVALID_FUNCTION);
                return 0ul;
            }
#endif

            HANDLE _hMap = nullptr;
            UNICODE_STRING _szPhysicalMemory = internal::MakeStaticUnicodeString(L"\\Device\\PhysicalMemory");
            OBJECT_ATTRIBUTES _ObjectAttributes = { sizeof(_ObjectAttributes), nullptr, &_szPhysicalMemory ,OBJ_CASE_INSENSITIVE };
            LONG _Status = NtOpenSection(&_hMap, SECTION_MAP_READ, &_ObjectAttributes);
            if (_Status < 0)
            {
                internal::BaseSetLastNTError(_Status);
                return 0ul;
            }

            constexpr auto kMaxSize = 64ul * 1024;
            LPBYTE _pData = (LPBYTE)MapViewOfFile(_hMap, FILE_MAP_READ, 0, 0xF0000, kMaxSize);
            CloseHandle(_hMap);

            do
            {
                if (!_pData)
                {
                    break;
                }

                const auto _pRawSMBIOSDataEnd = _pData + kMaxSize;
                for (auto _pStart = _pData; _pStart + sizeof(SMBEntryPoint32) < _pRawSMBIOSDataEnd; _pStart += 16)
                {
                    if (*(uint32_t*)_pStart == kSMBIOS32)
                    {
                        auto _pSMBEntryPoint32 = (SMBEntryPoint32*)_pStart;
                        const auto _cbData = _pSMBEntryPoint32->entryPointLength;
                        if (_pStart + _cbData > _pRawSMBIOSDataEnd)
                            continue;

                        uint8_t _uChecksum = 0;
                        for (size_t i = 0; i != _cbData; ++i)
                        {
                            _uChecksum += _pStart[i];
                        }

                        if (_uChecksum != 0)
                            continue;

                        if (LPBYTE(_pSMBEntryPoint32->dmi.tableAddress + _pSMBEntryPoint32->dmi.tableLength) > _pRawSMBIOSDataEnd)
                            continue;

                        _cbBufferNeed = sizeof(RawSMBIOSData) + _pSMBEntryPoint32->dmi.tableLength;
                        if (_cbBufferNeed <= _cbBufferSize)
                        {
                            if (_pFirmwareTableBuffer)
                            {
                                auto _pRawSMBIOSData = (RawSMBIOSData*)_pFirmwareTableBuffer;
                                _pRawSMBIOSData->Used20CallingMethod = 0;
                                _pRawSMBIOSData->SMBIOSMajorVersion = _pSMBEntryPoint32->majorVersion;
                                _pRawSMBIOSData->SMBIOSMinorVersion = _pSMBEntryPoint32->minorVersion;
                                _pRawSMBIOSData->DmiRevision = _pSMBEntryPoint32->entryPointRevision;
                                _pRawSMBIOSData->Length = _pSMBEntryPoint32->dmi.tableLength;
                                memcpy(_pRawSMBIOSData->SMBIOSTableData, LPBYTE(_pSMBEntryPoint32->dmi.tableAddress), _pSMBEntryPoint32->dmi.tableLength);
                            }

                            _lStatus = ERROR_SUCCESS;
                        }
                        else
                        {
                            _lStatus = ERROR_INSUFFICIENT_BUFFER;
                        }
                        break;
                    }
                }
            } while (false);

            if (_pData)
            {
                UnmapViewOfFile(_pData);
            }
        }
        else if (_uFirmwareTableProviderSignature == 'ACPI')
        {
            if (!_uFirmwareTableID)
                _uFirmwareTableID = 'PCAF';

            HKEY _hKey = nullptr;
            BYTE _StaticBuffer[2048];
            auto _pData = _StaticBuffer;

            do
            {
                // Windows XP SP2
                if (RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\mssmbios\\Data", &_hKey) == ERROR_SUCCESS)
                {
                    DWORD _cbData = sizeof(_StaticBuffer);
                    _lStatus = RegQueryValueExW(_hKey, L"AcpiData", nullptr, nullptr, _pData, &_cbData);
                    if (_lStatus == ERROR_MORE_DATA)
                    {
                        _pData = (BYTE*)internal::Alloc(_cbData);
                        if (!_pData)
                        {
                            _lStatus = ERROR_OUTOFMEMORY;
                            break;
                        }
                        _lStatus = RegQueryValueExW(_hKey, L"AcpiData", nullptr, nullptr, _pData, &_cbData);
                    }

                    if (_lStatus != ERROR_SUCCESS)
                    {
                        break;
                    }

#pragma pack(push, 1)
                    struct ACPI_DESCRIPTION_HEADER
                    {
                        UINT32  Signature;
                        UINT32  Length;
                        //UINT8   Revision;
                        //UINT8   Checksum;
                        //UINT8   OemId[6];
                        //UINT64  OemTableId;
                        //UINT32  OemRevision;
                        //UINT32  CreatorId;
                        //UINT32  CreatorRevision;
                    };
#pragma pack(pop)
                    const auto _pDataEnd = _pData + _cbData;
                    for (auto _pItem = _pData; _pItem + sizeof(ACPI_DESCRIPTION_HEADER) < _pDataEnd;)
                    {
                        auto _pHeader = (ACPI_DESCRIPTION_HEADER*)_pItem;
                        if (_pHeader->Length < sizeof(ACPI_DESCRIPTION_HEADER))
                            break;

                        _pItem += _pHeader->Length;
                        if (_pItem > _pDataEnd)
                            break;

                        if (_pHeader->Signature == _uFirmwareTableID)
                        {
                            _cbBufferNeed = _pHeader->Length;
                            if (_cbBufferNeed <= _cbBufferSize)
                            {
                                if (_pFirmwareTableBuffer)
                                {
                                    memcpy(_pFirmwareTableBuffer, _pHeader, _cbBufferNeed);
                                }

                                _lStatus = ERROR_SUCCESS;
                            }
                            else
                            {
                                _lStatus = ERROR_INSUFFICIENT_BUFFER;
                            }
                            break;
                        }
                    }
                }
            } while (false);

            if (_pData && _pData != _StaticBuffer)
            {
                internal::Free(_pData);
            }

            if (_hKey)
            {
                ::RegCloseKey(_hKey);
            }
        }

        if (_lStatus)
        {
            SetLastError(_lStatus);
        }

        return _cbBufferNeed;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_SP1)

    // 最低受支持的客户端	Windows Vista SP1 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    GetPhysicallyInstalledSystemMemory,
        _Out_ PULONGLONG _puTotalMemoryInKilobytes
        )
    {
        if (const auto _pfnGetPhysicallyInstalledSystemMemory = try_get_GetPhysicallyInstalledSystemMemory())
        {
            return _pfnGetPhysicallyInstalledSystemMemory(_puTotalMemoryInKilobytes);
        }

        if (!_puTotalMemoryInKilobytes)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        auto _cbRawSMBIOSData = GetSystemFirmwareTable('RSMB', 0, 0, 0);
        if (_cbRawSMBIOSData == 0)
        {
            return FALSE;
        }

        internal::StringBuffer<BYTE> _oSMBIOSDataBuffer;
        auto _pRawSMBIOSData = (RawSMBIOSData*)_oSMBIOSDataBuffer.GetBuffer(_cbRawSMBIOSData);
        if (!_pRawSMBIOSData)
        {
            SetLastError(ERROR_OUTOFMEMORY);
            return FALSE;
        }

        _cbRawSMBIOSData = GetSystemFirmwareTable('RSMB', 0, _pRawSMBIOSData, _cbRawSMBIOSData);
        if (_cbRawSMBIOSData == 0)
        {
            return FALSE;
        }

        if (_cbRawSMBIOSData < sizeof(RawSMBIOSData) || sizeof(RawSMBIOSData) + _pRawSMBIOSData->Length > _cbRawSMBIOSData)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        // 对于早期系统，应该不太可能同时插上256条内存
        // 简化内存申请，先将内存条上限设置在256
        struct PhysicalMemoryInfo
        {
            uint64_t uTotalSize = 0;
            WORD Handle = 0;
            bool bUsed = false;
        };

        PhysicalMemoryInfo _arrPhysicalMemoryInfo[256];
        uint32_t _cPhysicalMemoryInfo = 0;

        const auto _pSMBIOSTableDataEnd = _pRawSMBIOSData->SMBIOSTableData + _pRawSMBIOSData->Length;
        for (auto _pSMBIOSTableData = _pRawSMBIOSData->SMBIOSTableData; _pSMBIOSTableData + sizeof(SMBIOSHEADER) < _pSMBIOSTableDataEnd;)
        {
            auto _pSMBIOSHeader = (SMBIOSHEADER*)_pSMBIOSTableData;
            if (_pSMBIOSHeader->Type == 127 && _pSMBIOSHeader->Length == sizeof(SMBIOSHEADER))
                break;

            auto _pNextSMBIOSTableData = _pSMBIOSTableData + _pSMBIOSHeader->Length;
            if (_pNextSMBIOSTableData > _pSMBIOSTableDataEnd)
                break;

            if (_pSMBIOSHeader->Type == 16)
            {
                if (_pSMBIOSHeader->Length >= RTL_SIZEOF_THROUGH_FIELD(PhysicalMemoryArray, NumberOfMemoryDevices))
                {
                    auto _pPhysicalMemoryArray = (PhysicalMemoryArray*)_pSMBIOSHeader;
                    if (_pPhysicalMemoryArray->Location == 3)
                    {
                        uint32_t i = 0;
                        for (; i != _cPhysicalMemoryInfo && _arrPhysicalMemoryInfo[i].Handle == _pPhysicalMemoryArray->Handle; ++i);

                        if (i == _cPhysicalMemoryInfo)
                        {
                            if (_cPhysicalMemoryInfo >= _countof(_arrPhysicalMemoryInfo))
                                break;

                            ++_cPhysicalMemoryInfo;
                            _arrPhysicalMemoryInfo[i].Handle = _pPhysicalMemoryArray->Handle;
                        }

                        _arrPhysicalMemoryInfo[i].bUsed = true;
                    }
                }
            }
            else if (_pSMBIOSHeader->Type == 17)
            {
                if (_pSMBIOSHeader->Length >= RTL_SIZEOF_THROUGH_FIELD(MemoryDevice, PartNumber))
                {
                    auto _pMemoryDevice = (MemoryDevice*)_pSMBIOSHeader;
                    if (_pMemoryDevice->Size == UINT16_MAX)
                    {
                        SetLastError(ERROR_INVALID_DATA);
                        return FALSE;
                    }

                    uint32_t i = 0;
                    for (; i != _cPhysicalMemoryInfo && _arrPhysicalMemoryInfo[i].Handle == _pMemoryDevice->MemoryArrayHandle; ++i);

                    if (i == _cPhysicalMemoryInfo)
                    {
                        if (_cPhysicalMemoryInfo >= _countof(_arrPhysicalMemoryInfo))
                            break;

                        ++_cPhysicalMemoryInfo;
                        _arrPhysicalMemoryInfo[i].Handle = _pMemoryDevice->MemoryArrayHandle;
                    }

                    // Windows 7的代码也没考虑 ExtendedSize，所以我们就不考虑32GB大内存的情况了。
                    if (_pMemoryDevice->Size & 0x8000ul)
                    {
                        // KB
                        _arrPhysicalMemoryInfo[i].uTotalSize += _pMemoryDevice->Size & 0x7FFFul;
                    }
                    else
                    {
                        // MB
                        _arrPhysicalMemoryInfo[i].uTotalSize += uint64_t(_pMemoryDevice->Size) * 1024ul;
                    }
                }
            }

            for (; _pNextSMBIOSTableData < _pSMBIOSTableDataEnd; ++_pNextSMBIOSTableData)
            {
                if (!*(WORD*)_pNextSMBIOSTableData)
                    break;
            }
            _pNextSMBIOSTableData += 2;
            _pSMBIOSTableData = _pNextSMBIOSTableData;
        }

        if (_cPhysicalMemoryInfo == 0)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        // 单位KB
        uint64_t _uSize = 0;
        for (uint32_t i = 0; i != _cPhysicalMemoryInfo; ++i)
        {
            if (_arrPhysicalMemoryInfo[i].bUsed)
            {
                _uSize += _arrPhysicalMemoryInfo[i].uTotalSize;
            }
        }

        if (_uSize == 0ull)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        MEMORYSTATUSEX _oMemoryStatus = { sizeof(_oMemoryStatus) };
        if (!GlobalMemoryStatusEx(&_oMemoryStatus))
        {
            return FALSE;
        }

        if (_oMemoryStatus.ullTotalPhys / 1024ul > _uSize)
        {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        *_puTotalMemoryInKilobytes = _uSize;
        return TRUE;
    }
#endif
} //namespace YY::Thunks
