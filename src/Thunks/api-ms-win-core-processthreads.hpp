#if (YY_Thunks_Target < __WindowsNT10_15063)
#include <processthreadsapi.h>
#endif

#ifdef YY_Thunks_Implemented

//结构体来源：http://www.rohitab.com/discuss/topic/38601-proc-thread-attribute-list-structure-documentation/


// This structure stores the value for each attribute
typedef struct _PROC_THREAD_ATTRIBUTE_ENTRY
{
    DWORD_PTR   Attribute;  // PROC_THREAD_ATTRIBUTE_xxx
    SIZE_T      cbSize;
    PVOID       lpValue;
} PROC_THREAD_ATTRIBUTE_ENTRY, * LPPROC_THREAD_ATTRIBUTE_ENTRY;

// This structure contains a list of attributes that have been added using UpdateProcThreadAttribute
typedef struct _PROC_THREAD_ATTRIBUTE_LIST
{
    DWORD                          dwFlags;
    ULONG                          Size;
    ULONG                          Count;
    ULONG                          Reserved;
    LPPROC_THREAD_ATTRIBUTE_ENTRY  lpExtendedFlags;
    PROC_THREAD_ATTRIBUTE_ENTRY    Entries[ANYSIZE_ARRAY];
} PROC_THREAD_ATTRIBUTE_LIST, * LPPROC_THREAD_ATTRIBUTE_LIST;

#endif

namespace YY::Thunks::Fallback {
#if defined(YY_Thunks_Implemented) && (YY_Thunks_Target < __WindowsNT6)
    namespace
    {
        static void __cdecl UninitPageVirtualProtect();

        static char* volatile* GetPageVirtualProtect()
        {
            //注册 m_pPageVirtualProtect 的反初始化工作
            __declspec(allocate(".YYThr$AAB")) static void* RunUninitPageVirtualProtect = reinterpret_cast<void*>(&Fallback::UninitPageVirtualProtect);

            __foreinclude(RunUninitPageVirtualProtect);

            static char* volatile m_pPageVirtualProtect = nullptr;

            return &m_pPageVirtualProtect;
        }

        static void __cdecl UninitPageVirtualProtect()
        {
            auto pOrgPageVirtualProtect = (char*)InterlockedExchangePointer((PVOID volatile*)GetPageVirtualProtect(), INVALID_HANDLE_VALUE);

            if (pOrgPageVirtualProtect != nullptr && pOrgPageVirtualProtect != (char*)INVALID_HANDLE_VALUE)
            {
                VirtualFree(pOrgPageVirtualProtect, 0, MEM_RELEASE);
            }
        }
    }
#endif

#if defined(YY_Thunks_Implemented) && (YY_Thunks_Target < __WindowsNT10_10240)

namespace {
// 亲和性掩码的位宽就是旧系统的处理器上限：NT 早期用 KAFFINITY 表达亲和性，
// x86 最多 32 个处理器、x64 最多 64 个。Windows 7 才引入处理器组把 64 个/组扩展成多组。
// 所以模拟实现只覆盖「单组 + 掩码位宽内」的处理器。
static constexpr const ULONG kCpuSetIdBitCount = sizeof(KAFFINITY) * 8;

// Windows 支持的处理器组数上限是 64，这是内核硬上限（至今仍是 64 组 × 64 处理器）。
// 组号 >= 64 的 CpuSet Id 必然不属于本机。
static constexpr const WORD kMaxProcessorGroupCount = 64;

// 原子读取快照，直接返回结构体。
// CAS 用「期望值 = 当前值」把同一份数据写回，等价于一次原子读；成功即代表读到一致快照。
static YY_ThunksCpuSetRecord __fastcall LoadCpuSetRecord(
    _In_ volatile YY_ThunksCpuSetRecord* _pRecord)
{
    YY_ThunksCpuSetRecord _Expected = {};
#if defined(_WIN64)
    InterlockedCompareExchange128(
        _pRecord->Raw,
        0, 0,
        _Expected.Raw);
#else
    _Expected.Value64 = InterlockedCompareExchange64(&_pRecord->Value64, 0, 0);
#endif

    return _Expected;
}

// 整体替换。不关心旧值，用 CAS 循环实现无条件的原子交换。
static void __fastcall StoreCpuSetRecord(
    _In_ volatile YY_ThunksCpuSetRecord* _pRecord,
    _In_ const YY_ThunksCpuSetRecord& _Desired)
{
    YY_ThunksCpuSetRecord _Expected;
#if defined(_WIN64)
    _Expected.Raw[0] = _pRecord->Raw[0];
    _Expected.Raw[1] = _pRecord->Raw[1];
#else
    _Expected.Value64 = _pRecord->Value64;
#endif

    for (;;)
    {
#if defined(_WIN64)
        // InterlockedCompareExchange128 的最后一个参数是输入输出：比较失败时它会自动把
        // 内存当前值回填进 _Expected.Raw，因此这里无需像 x86 分支那样手动回填。
        if (InterlockedCompareExchange128(
            (volatile __int64*)_pRecord->Raw,
            (__int64)_Desired.Raw[1], (__int64)_Desired.Raw[0],
            (__int64*)_Expected.Raw) != 0)
        {
            break;
        }
#else
        // InterlockedCompareExchange64 不会回填期望值，必须手动用返回的旧值重试。
        const auto _iLast = InterlockedCompareExchange64(&_pRecord->Value64, _Desired.Value64, _Expected.Value64);
        if (_iLast == _Expected.Value64)
        {
            break;
        }

        _Expected.Value64 = _iLast;
#endif

        YieldProcessor();
    }
}

// 新线程的延迟应用：DLL_THREAD_ATTACH 时把进程默认 CpuSet 落到该线程的亲和性上。
//
// 为什么需要：SetProcessDefaultCpuSets 只记录状态，不会影响当前已存在的线程
// （与原生一致）。新建线程必须在 DLL_THREAD_ATTACH 时机应用该状态。
//
// 关于「为 DllMain 执行线程补发一次 THREAD_ATTACH」：
// 补发者是 **CRT 运行库**，不是加载器。加载器只负责对执行 DllMain 的当前线程发
// DLL_PROCESS_ATTACH，以及对真正的新线程发 DLL_THREAD_ATTACH。部分 CRT 的初始化
// 逻辑会自行构造一次 DLL_THREAD_ATTACH 调用，以便让依赖线程局部初始化的运行时库
// 在该线程上完成初始化。这是 CRT 的实现细节，不同 CRT 版本、不同 TlsMode
// （本项目的 DllMainCRTStartupForYY_Thunks / ByFirstCallback / None）行为可能不同。
//
// 因此：既不能假设补发一定存在，也不能假设一定不存在。
//
// 方案：一次性标记。任何一个 THREAD_ATTACH 到达时，先把标记取走（清零），
// 再判断这次是不是伪 ATTACH。这样两种前提下的行为都是安全的：
//   * 伪 ATTACH 会到来   → 第一个 THREAD_ATTACH 就是它 → 命中 → 跳过。
//                          之后标记已清零，后续线程正常处理。
//   * 伪 ATTACH 不会到来 → 第一个真实新线程被误认为伪 ATTACH 而跳过
//                          （该线程不继承进程默认，一次偏差），之后标记清零，
//                          后续线程正常处理。
// 对比「命中才清零」：前提不成立时标记永不清零，加载线程退出后 TID 被系统回收，
// 后续新线程拿到同一 TID 就会被永久误跳过 —— 在长驻进程里会反复发生。
// 一次性标记把最坏情况压缩成「每个模块最多误跳过 1 个线程」。
//
// 各模块的标记是模块级静态，互不干扰：模块 A 的 PROCESS_ATTACH 只写 A 的标记，
// A 的伪 THREAD_ATTACH 只消费 A 的标记。
//
// 静态库场景：无 DllMain，但 TLS 回调由链接器按 .CRT$XLY 注册，仍能工作。
// 此时不存在 CRT 补发的伪 ATTACH，标记恒为 0 → 所有 THREAD_ATTACH 都被正常处理。
//
// XP 场景：XP 的 TLS 回调经 DllMainCRTStartup.hpp 的 CallTlsCallback 派发，
// 而 DllMainCRTStartupForYY_Thunks 的 DLL_PROCESS_ATTACH 分支不调用 CallTlsCallback。
// 即本回调在 XP 上收不到 PROCESS_ATTACH，标记恒为 0 → 同样没有偏差。
static volatile LONG s_lAttachThreadId;

static VOID NTAPI CpuSetThreadAttachCallback(
    _In_opt_ PVOID _pModule,
    _In_ DWORD _uReason,
    _In_opt_ PVOID _pReserved)
{
    UNREFERENCED_PARAMETER(_pModule);
    UNREFERENCED_PARAMETER(_pReserved);

    if (_uReason == DLL_PROCESS_ATTACH)
    {
        // 顺带完成共享映射的惰性初始化，避免首个 THREAD_ATTACH 在 loader lock 内
        // 做 CreateFileMappingW / MapViewOfFile / CloseHandle。
        (void)GetYY_ThunksSharedData();

        InterlockedExchange(&s_lAttachThreadId, (LONG)::GetCurrentThreadId());
        return;
    }
    else if (_uReason == DLL_PROCESS_DETACH)
    {
        // 兜底：正常流程下标记已在伪 ATTACH 时清零，这里再清一次以防万一
        // （例如模块在 PROCESS_ATTACH 之后、伪 THREAD_ATTACH 之前被强行卸载）。
        InterlockedExchange(&s_lAttachThreadId, 0);
        return;
    }
    else if (_uReason == DLL_THREAD_ATTACH)
    {
        // 一次性取出标记。无论后面判断结果如何，标记都已失效。
        const LONG _lPendingThreadId = InterlockedExchange(&s_lAttachThreadId, 0);
        if (_lPendingThreadId != 0 && _lPendingThreadId == (LONG)::GetCurrentThreadId())
            return;                          // 伪 ATTACH（若存在）→ 跳过

        const auto _Default = Fallback::LoadCpuSetRecord(&GetYY_ThunksSharedData()->CpuSetDefaultRecord);

        // 掩码 0 = 无进程默认（从未设置或已清除）→ 不限制线程。
        // 这是最常见的路径：一次原子读 + 一次比较即返回。
        if (_Default.Group == 0 && _Default.Mask == 0)
            return;

        GROUP_AFFINITY _Current = {};
        if (!::GetThreadGroupAffinity(NtGetCurrentThread(), &_Current))
            return;

        if (_Current.Group == _Default.Group && _Current.Mask == _Default.Mask)
            return;

        // 已经设置了组号（Group != 0）说明线程已经被限制过，不能再覆盖。
        if (_Current.Group != 0)
            return;

        DWORD_PTR _uProcessAffinityMask = 0;
        DWORD_PTR _uSystemAffinityMask = 0;
        if (!GetProcessAffinityMask(NtGetCurrentProcess(), &_uProcessAffinityMask, &_uSystemAffinityMask))
            return;

        // 线程掩码必须等于「进程亲和性掩码」才说明它处于「未设置自有选择」的初始态。
        // 若线程掩码是进程掩码的真子集，说明调用方已经对该线程做过限制，不能覆盖。
        //
        // 注意方向：线程亲和性不可能大于进程亲和性，最大值就是进程亲和性，
        // 因此「等于进程掩码」正是新线程刚创建时的形态 —— 此时才**需要**应用进程默认。
        if (_Current.Mask != _uProcessAffinityMask)
            return;

        GROUP_AFFINITY _Target = {};
        _Target.Group = _Default.Group;
        _Target.Mask = _Default.Mask;
        ::SetThreadGroupAffinity(NtGetCurrentThread(), &_Target, nullptr);
    }
}

// 组内实际存在的处理器位掩码。返回 0 表示该组不存在或该组无处理器。
static KAFFINITY __fastcall ValidCpuSetMask(_In_ WORD _uGroup)
{
    if (_uGroup >= kMaxProcessorGroupCount)
        return 0;

    // 按组缓存处理器数量：0 = 尚未查询，UINT32_MAX = 查询失败（该组不存在）。
    // 本库已有 GetActiveProcessorCount 的 thunk（XP/Vista 退化为只认组 0），可直接调用。
    static volatile DWORD s_ProcessorCountCache[kMaxProcessorGroupCount] = {};

    // _uCount 必须可写：冷启动时缓存为空，需要把查询结果回填后才算得出掩码。
    // 若写成 const auto，冷启动路径下 _uCount 恒为 0，末尾 (1 << 0) - 1 得到 0，
    // 会被误判成「该组不存在」而拒绝所有合法 Id（进程内首个 CpuSet 调用必然失败）。
    DWORD _uCount = s_ProcessorCountCache[_uGroup];
    if (_uCount == 0)
    {
        _uCount = GetActiveProcessorCount(_uGroup);
        if (_uCount == 0)
        {
            s_ProcessorCountCache[_uGroup] = UINT32_MAX;
            return 0;
        }

        s_ProcessorCountCache[_uGroup] = _uCount;
    }
    else if (_uCount == UINT32_MAX)
    {
        // 已确认查询失败
        return 0;
    }

    if (_uCount >= kCpuSetIdBitCount)
        return ~(KAFFINITY)0;

    return ((KAFFINITY)1 << _uCount) - 1;
}

// 把 CPU Set Id 列表折算成「单组」的亲和性掩码。
//
// Id 编码：组号 * kCpuSetIdBitCount + 组内位序。该编码仅 Fallback 内部自洽；
// 原生 CpuSet Id 是不透明标识，跨进程也不保证稳定，因此确定性编码不构成兼容缺陷。
//
// 校验规则（用户裁决）：
//   * 有效性校验覆盖**全部**输入 Id，包括之后会被丢弃的其它组 Id；
//   * 只要有一个 Id 不属于本机处理器 → 返回 false（调用方报 813）；
//   * 全部合法时，以**第一个 Id 所属组**为准，其它组的 Id 直接丢弃（不报错）。
//
// 为什么丢弃而不是报错：原生在多组机器上支持跨组集合。fallback 无法表达多组，
// 但「尽力应用第一组」比「整体拒绝」更接近调用方意图。
static bool __fastcall TryGroupMaskFromCpuSetIds(
    _In_reads_opt_(_uCpuSetIdCount) const ULONG* _puCpuSetIds,
    _In_ ULONG _uCpuSetIdCount,
    _Out_ YY_ThunksCpuSetRecord* _pCpuRecord)
{
    if (_puCpuSetIds == nullptr || _uCpuSetIdCount == 0)
        return false;

    // 组号先用 ULONG 比较、最后才截断成 WORD。
    // 若直接 (WORD)(Id / 位宽)，组号超过 0xFFFF 的 Id（如 Id = 4194304，组号 65536）会被截断成 0，
    // 与真正位于组 0 的 Id（如 Id = 5）误判为同组，导致跨组集合被静默接受并丢弃部分 Id。
    const ULONG _uGroup = _puCpuSetIds[0] / kCpuSetIdBitCount;
    if (_uGroup > 0xFFFF)
        return false;

    if (ValidCpuSetMask((WORD)_uGroup) == 0)
        return false;

    KAFFINITY _uMask = 0;

    for (ULONG _uIdIndex = 0; _uIdIndex != _uCpuSetIdCount; ++_uIdIndex)
    {
        const ULONG _uCpuSetId = _puCpuSetIds[_uIdIndex];
        const ULONG _uIdGroup = _uCpuSetId / kCpuSetIdBitCount;
        if (_uIdGroup > 0xFFFF)
            return false;
        
        // 必须是 KAFFINITY：x64 上 KAFFINITY 是 64 位，若落到 ULONG（32 位）则
        // 「组内位序 >= 32」的合法 Id 会得到 0，被误判为无效 Id 而拒绝（单组 48/64 核机器必现）。
        const KAFFINITY _uMaskBit = ((KAFFINITY)1 << (_uCpuSetId % kCpuSetIdBitCount));
        if ((ValidCpuSetMask((WORD)_uIdGroup) & _uMaskBit) == 0)
            return false;

        // 其它组的 Id 丢弃，但**有效性同样要校验**：只要有一个 Id 不属于本机处理器就整体失败。
        if (_uIdGroup != _uGroup)
        {
            continue;
        }

        _uMask |= _uMaskBit;
    }

    _pCpuRecord->Group = (WORD)_uGroup;
    _pCpuRecord->Mask = _uMask;
    return true;
}

// 打开目标进程的 YY_ThunksSharedData 映射。
//
// 映射名由 GetYY_ThunksSharedData 生成，格式为
//   YY_ThunksSharedData_53302349-F6BE-49C4-AC98-DA275C0CE653_<PID>_<arch>
// 其中 <arch> 是 _x86 / _amd64 / _arm / _arm64。
//
// 只尝试**本进程位宽对应的架构族**：64 位进程试 _amd64 / _arm64，32 位进程试 _x86 / _arm。
// 跨 CPU 体系因此天然打不开对方映射（64 位进程永远打不开 32 位进程的 _x86），
// 不需要 IsWow64Process 探测，也就少一次 OpenProcess + IsWow64Process + CloseHandle。
//
// 直接复用调用方已有的进程句柄，不再按 PID 重新 OpenProcess。
// 失败返回 nullptr 并置 ERROR_CALL_NOT_IMPLEMENTED。失败原因包括：
//   * 目标进程与本进程位数不一致（跨 CPU 体系，无法解释记录宽度）
//   * 目标进程未加载 YY-Thunks（映射不存在）
static YY_ThunksSharedData* __fastcall OpenSharedDataForProcess(_In_ HANDLE _hProcess)
{
    const DWORD _uProcessId = ::GetProcessId(_hProcess);
    if (_uProcessId == 0)
        return nullptr;

    // 拼映射名。与 GetYY_ThunksSharedData 保持完全一致的格式。
    wchar_t _szYY_ThunksSharedDataMapNameBuffer[MAX_PATH] = {};
    YY::Thunks::internal::StringBuffer<wchar_t> _szBuffer(_szYY_ThunksSharedDataMapNameBuffer, _countof(_szYY_ThunksSharedDataMapNameBuffer));
    _szBuffer.AppendString(L"YY_ThunksSharedData_53302349-F6BE-49C4-AC98-DA275C0CE653_");

    _szBuffer.AppendUint32(_uProcessId);

    const auto _cchLength = _szBuffer.uLength;

#if defined(_AMD64_) || defined(_ARM64_)
    static const LPCWSTR s_szArchs[] = { L"_amd64", L"_arm64" };
#elif defined(_X86_) || defined(_ARM_)
    static const LPCWSTR s_szArchs[] = { L"_x86", L"_arm" };
#else
#error "未知CPU架构"
#endif
    for (auto _szArch : s_szArchs)
    {
        _szBuffer.SetLength(_cchLength);
        _szBuffer.AppendString(_szArch);

        const HANDLE _hMapping = ::OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, _szBuffer.GetC_String());
        if (_hMapping == nullptr)
            continue;

        auto _pData = (YY_ThunksSharedData*)::MapViewOfFile(_hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
        ::CloseHandle(_hMapping);

        if (!_pData)
            break;

        return _pData;
    }

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return nullptr;
}

// 读取目标进程的进程默认记录。
//
// 同进程直接读本地记录；跨进程内部打开并关闭映射，调用方无法遗漏释放。
// 跨 CPU 体系（x86↔x64）或目标进程未加载 YY-Thunks 时返回 false
// 并设置 ERROR_CALL_NOT_IMPLEMENTED。
//
// 直接复用调用方已有的进程句柄（GetProcessDefaultCpuSets 本来就有），
// 不再按 PID 重新 OpenProcess。
static bool __fastcall GetCpuSetDefault(
    _In_ HANDLE _hProcess,
    _Out_ YY_ThunksCpuSetRecord* _pRecord)
{
    if (_hProcess == NtGetCurrentProcess())
    {
        *_pRecord = LoadCpuSetRecord(&GetYY_ThunksSharedData()->CpuSetDefaultRecord);
        return true;
    }

    const auto _uProcessId = ::GetProcessId(_hProcess);
    if (_uProcessId == 0)
    {
        return false;
    }
    else if (_uProcessId == ::GetCurrentProcessId())
    {
        *_pRecord = LoadCpuSetRecord(&GetYY_ThunksSharedData()->CpuSetDefaultRecord);
        return true;
    }

    auto _pData = OpenSharedDataForProcess(_hProcess);
    if (_pData == nullptr)
    {
        return false;
    }

    *_pRecord = LoadCpuSetRecord(&_pData->CpuSetDefaultRecord);
    ::UnmapViewOfFile((LPCVOID)_pData);
    return true;
}

// 写入目标进程的进程默认记录。
//
// 同进程直接写本地记录；跨进程内部打开并关闭映射，调用方无法遗漏释放。
// 失败语义与 GetCpuSetDefault 一致。
static bool __fastcall SetCpuSetDefault(
    _In_ HANDLE _hProcess,
    _In_ const YY_ThunksCpuSetRecord& _Record)
{
    if (_hProcess == NtGetCurrentProcess())
    {
        StoreCpuSetRecord(&GetYY_ThunksSharedData()->CpuSetDefaultRecord, _Record);
        return true;
    }

    const auto _uProcessId = ::GetProcessId(_hProcess);
    if (_uProcessId == 0)
    {
        return false;
    }
    else if (_uProcessId == ::GetCurrentProcessId())
    {
        StoreCpuSetRecord(&GetYY_ThunksSharedData()->CpuSetDefaultRecord, _Record);
        return true;
    }

    auto _pData = OpenSharedDataForProcess(_hProcess);
    if (_pData == nullptr)
    {
        return false;
    }

    StoreCpuSetRecord(&_pData->CpuSetDefaultRecord, _Record);

    ::UnmapViewOfFile((LPCVOID)_pData);
    return true;
}

// 按掩码位序展开 CPU Set Id，返回实际写入个数。
// 掩码位数少于容量时提前停止，因此实际写入数可能小于容量。
static ULONG __fastcall CpuSetIdsFromGroupMask(
    _In_ KAFFINITY _uMask,
    _In_ WORD _uGroup,
    _Out_writes_to_(_uCpuSetIdCount, return) PULONG _puCpuSetIds,
    _In_ ULONG _uCpuSetIdCount)
{
    ULONG _cWritten = 0;

    for (ULONG _uBit = 0; _uBit != kCpuSetIdBitCount && _cWritten != _uCpuSetIdCount; ++_uBit)
    {
        if ((_uMask & ((KAFFINITY)1 << _uBit)) == 0)
            continue;

        *_puCpuSetIds++ = _uGroup * kCpuSetIdBitCount + _uBit;
        ++_cWritten;
    }

    return _cWritten;
    }

    } // namespace
    #endif // defined(YY_Thunks_Implemented) && (YY_Thunks_Target < __WindowsNT10_10240)
} // namespace YY::Thunks::Fallback

namespace YY ::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_2)

    //Windows Vista, Windows Server 2003
    __DEFINE_THUNK(
    kernel32,
    0,
    DWORD,
    WINAPI,
    GetCurrentProcessorNumber,
        VOID
        )
    {
        if (const auto _pfnGetCurrentProcessorNumber = try_get_GetCurrentProcessorNumber())
        {
            return _pfnGetCurrentProcessorNumber();
        }
        else
        {
            // Reference: https://www.cs.tcd.ie/Jeremy.Jones/GetCurrentProcessorNumberXP.htm
            //
            // The GetCurrentProcessorNumber() function is not available in XP. 
            // Here is a VC++ version of the function that works with Windows XP on Intel x86 single, hyperthreaded, multicore 
            // and multi-socket systems. It makes use of the APIC ID returned by the CPUID instruction. 
            // This is in the range 0 .. N-1, where N is the number of logical CPUs.

            __asm
            {
                mov eax, 1
                cpuid
                shr ebx, 24
                // eax 返回值
                mov eax, ebx
            }
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    //Windows 7, Windows Server 2008 R2
    __DEFINE_THUNK(
    kernel32,
    4,
    VOID,
    WINAPI,
    GetCurrentProcessorNumberEx,
        _Out_ PPROCESSOR_NUMBER ProcNumber
        )
    {
        if (auto pGetCurrentProcessorNumberEx = try_get_GetCurrentProcessorNumberEx())
        {
            pGetCurrentProcessorNumberEx(ProcNumber);
        }
        else
        {
            //不支持GetCurrentProcessorNumberEx时假定用户只有一组CPU
            ProcNumber->Group = 0;
            ProcNumber->Number = static_cast<BYTE>(GetCurrentProcessorNumber());
            ProcNumber->Reserved = 0;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2)

    //Windows Vista [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    //感谢 过客 提供
    __DEFINE_THUNK(
    kernel32,
    4,
    DWORD,
    WINAPI,
    GetThreadId,
        _In_ HANDLE Thread
        )
    {
        if (const auto _pfnGetThreadId = try_get_GetThreadId())
        {
            return _pfnGetThreadId(Thread);
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryInformationThread = try_get_NtQueryInformationThread();
        if(!NtQueryInformationThread)
        {
            SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
            return 0;
        }
#endif

        THREAD_BASIC_INFORMATION ThreadBasicInfo;

        LONG Status = NtQueryInformationThread(Thread, ThreadBasicInformation, &ThreadBasicInfo, sizeof(ThreadBasicInfo), nullptr);

        if (Status < 0)
        {
            internal::BaseSetLastNTError(Status);
            return 0;
        }
        else
        {
            return (DWORD)ThreadBasicInfo.ClientId.UniqueThread;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2)

    //Windows Vista [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    4,
    DWORD,
    WINAPI,
    GetProcessIdOfThread,
        _In_ HANDLE Thread
        )
    {
        if (const auto _pfnGetProcessIdOfThread = try_get_GetProcessIdOfThread())
            return _pfnGetProcessIdOfThread(Thread);
        
#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryInformationThread = try_get_NtQueryInformationThread();
        if (!NtQueryInformationThread)
        {
            SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
            return 0;
        }
#endif

        THREAD_BASIC_INFORMATION ThreadBasicInfo;

        LONG Status = NtQueryInformationThread(Thread, ThreadBasicInformation, &ThreadBasicInfo, sizeof(ThreadBasicInfo), nullptr);

        if (Status < 0)
        {
            internal::BaseSetLastNTError(Status);
            return 0;
        }
        else
        {
            return (DWORD)ThreadBasicInfo.ClientId.UniqueProcess;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1_SP1)

    //Windows Vista, Windows XP with SP1 [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    4,
    DWORD,
    WINAPI,
    GetProcessId,
        _In_ HANDLE Process
        )
    {
        if (auto pGetProcessId = try_get_GetProcessId())
        {
            return pGetProcessId(Process);
        }
#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryInformationProcess = try_get_NtQueryInformationProcess();
        if (!NtQueryInformationProcess)
        {
            SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
            return 0;
        }
#endif

        PROCESS_BASIC_INFORMATION ProcessBasicInfo;

        LONG Status = NtQueryInformationProcess(Process, ProcessBasicInformation, &ProcessBasicInfo, sizeof(ProcessBasicInfo), nullptr);

        if (Status < 0)
        {
            internal::BaseSetLastNTError(Status);
            return 0;
        }
        else
        {
            return (DWORD)ProcessBasicInfo.UniqueProcessId;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    0,
    VOID,
    WINAPI,
    FlushProcessWriteBuffers,
        VOID
        )
    {
        if (auto pFlushProcessWriteBuffers = try_get_FlushProcessWriteBuffers())
        {
            return pFlushProcessWriteBuffers();
        }


        /*
        * 参考了 14.29.30037 crt\src\concrt\ResourceManager.cpp 实现
            
        */

        for (;;)
        {
            auto pOrgPageVirtualProtect = (char*)InterlockedCompareExchangePointer((PVOID volatile*)Fallback::GetPageVirtualProtect(), nullptr, nullptr);


            //内存存在异常，我们什么也不做。
            if (pOrgPageVirtualProtect == (char*)INVALID_HANDLE_VALUE)
            {
                break;
            }
            else if (pOrgPageVirtualProtect == nullptr)
            {
                //我们需要开辟一段内存

                auto pPageVirtualProtectTmp = (char*)VirtualAlloc(NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

                if (pPageVirtualProtectTmp == nullptr)
                {
                    //内存申请失败，直接设置为 -1，并且什么也不做
                    InterlockedCompareExchangePointer((PVOID volatile*)Fallback::GetPageVirtualProtect(), INVALID_HANDLE_VALUE, nullptr);
                    break;
                }

                *pPageVirtualProtectTmp = 1;

                pOrgPageVirtualProtect = (char*)InterlockedCompareExchangePointer((PVOID volatile*)Fallback::GetPageVirtualProtect(), pPageVirtualProtectTmp, nullptr);

                if (pOrgPageVirtualProtect != nullptr)
                {
                    //说明另外一个线程已经设置，我们释放这次申请的内存
                    VirtualFree(pPageVirtualProtectTmp, 0, MEM_RELEASE);
                }
                else
                {
                    pOrgPageVirtualProtect = pPageVirtualProtectTmp;
                }
            }


            // Note that the read of *m_pPageVirtualProtect is very important, as it makes it extremely likely that this memory will
            // be in the working set when we call VirtualProtect (see comments below).
            if (*pOrgPageVirtualProtect == 1)
            {
                //
                // VirtualProtect simulates FlushProcessWriteBuffers because it happens to send an inter-processor interrupt to all CPUs,
                // and inter-processor interrupts happen to cause the CPU's store buffers to be flushed.
                //
                // Unfortunately, VirtualProtect only does this if the page whose status is being changed is in the process' working set
                // (otherwise there's no need to tell the other CPUs that anything has changed).
                //
                // One way to do this is to lock the page into the process' working set. Unfortunately, it can fail if there are already too many
                // locked pages.
                //
                // We could increase the process' working set limit, using SetProcessWorkingSet, but that would be a) intrusive (the process may
                // have its own idea of what the limit should be), and b) race-prone (another thread may be trying to adjust the limit, to a
                // different value, at the same time).
                //
                // We could stop using *m_pPageVirtualProtect as the page we fiddle with, and instead use a page we know is already locked into
                // the working set. There's no way to enumerate such pages, so it'd have to be a well-known fixed location that we know is always
                // locked, and that can have its protection fiddled with without consequence.  We know of no such location, and if we did it would
                // undoubtedly be some internal Windows data structure that would be subject to changes in the way its memory is handled at any time.
                //
                // The VirtualProtect trick has worked for many years in the CLR, without the call to VirtualLock, without apparent problems.
                // Part of the reason is because of the equivalent of the check of *m_pPageVirtualProtect above.
                //
                DWORD oldProtect;

                // We have it on good authority from the kernel team that, although VirtualProtect is repeatedly called with the
                // same protection (PAGE_READONLY), the OS will not optimize out the flush buffers as a result.
                VirtualProtect(pOrgPageVirtualProtect, sizeof(BYTE), PAGE_READONLY, &oldProtect);
            }

            break;
        }
    }

#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps only]
    //Minimum supported server	Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    16,
    _Success_(return != FALSE)
    BOOL,
    WINAPI,
    InitializeProcThreadAttributeList,
        _Out_writes_bytes_to_opt_(*lpSize,*lpSize) LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
        _In_ DWORD dwAttributeCount,
        _Reserved_ DWORD dwFlags,
        _When_(lpAttributeList == nullptr,_Out_) _When_(lpAttributeList != nullptr,_Inout_) PSIZE_T lpSize
        )
    {
        if (const auto pInitializeProcThreadAttributeList = try_get_InitializeProcThreadAttributeList())
        {
            return pInitializeProcThreadAttributeList(lpAttributeList, dwAttributeCount, dwFlags, lpSize);
        }


        LSTATUS lStatus = ERROR_SUCCESS;

        do
        {
            //参数验证
            if (dwFlags)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            //Vista只支持 3个 Type，所以只有三个
            if (dwAttributeCount > 3)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }


            const auto cbSize = *lpSize;
            const auto cbSizeNeed = sizeof(PROC_THREAD_ATTRIBUTE_LIST) + dwAttributeCount * sizeof(PROC_THREAD_ATTRIBUTE_ENTRY);
            *lpSize = cbSizeNeed;

            if (lpAttributeList == nullptr || cbSize < cbSizeNeed)
            {
                lStatus = ERROR_INSUFFICIENT_BUFFER;
                break;
            }

            lpAttributeList->dwFlags = 0;
            lpAttributeList->lpExtendedFlags = nullptr;
            lpAttributeList->Size = dwAttributeCount;
            lpAttributeList->Count = 0;

            return TRUE;

        } while (false);


        SetLastError(lStatus);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps only]
    //Minimum supported server	Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    4,
    VOID,
    WINAPI,
    DeleteProcThreadAttributeList,
        _Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList
        )
    {
        if (const auto pDeleteProcThreadAttributeList = try_get_DeleteProcThreadAttributeList())
        {
            return pDeleteProcThreadAttributeList(lpAttributeList);
        }


        //Vista原版什么也没有做……
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps only]
    //Minimum supported server	Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    28,
    BOOL,
    WINAPI,
    UpdateProcThreadAttribute,
        _Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
        _In_ DWORD dwFlags,
        _In_ DWORD_PTR Attribute,
        _In_reads_bytes_opt_(cbSize) PVOID lpValue,
        _In_ SIZE_T cbSize,
        _Out_writes_bytes_opt_(cbSize) PVOID lpPreviousValue,
        _In_opt_ PSIZE_T lpReturnSize
        )
    {
        if (const auto pUpdateProcThreadAttribute = try_get_UpdateProcThreadAttribute())
        {
            return pUpdateProcThreadAttribute(lpAttributeList, dwFlags, Attribute, lpValue, cbSize, lpPreviousValue, lpReturnSize);
        }

        
        LSTATUS lStatus = ERROR_SUCCESS;

        do
        {
            auto AttributeMark = 1ul << Attribute;

            /////////////////////////////////////////////////////
            //
            // 参数检查
            //

            if (dwFlags & (~PROC_THREAD_ATTRIBUTE_REPLACE_VALUE))
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }


            if ((Attribute & PROC_THREAD_ATTRIBUTE_ADDITIVE) == 0)
            {
                if (lpAttributeList->Count == lpAttributeList->Size)
                {
                    //internal::BaseSetLastNTError(0xC0000001);
                    lStatus = ERROR_GEN_FAILURE;
                    break;
                }
                else if (AttributeMark & lpAttributeList->dwFlags)
                {
                    //internal::BaseSetLastNTError(0x40000000);
                    lStatus = ERROR_OBJECT_NAME_EXISTS;
                    break;
                }
                else if (lpPreviousValue)
                {
                    //internal::BaseSetLastNTError(0xC00000F4);
                    lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }
                else if (dwFlags & PROC_THREAD_ATTRIBUTE_REPLACE_VALUE)
                {
                    //internal::BaseSetLastNTError(0xC00000F0);
                    lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }
            }


            if ((PROC_THREAD_ATTRIBUTE_INPUT & Attribute) && lpReturnSize)
            {
                //internal::BaseSetLastNTError(0xC00000F5);
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            //
            //
            ////////////////////////////////////////////////////


            auto pAttribute = &lpAttributeList->Entries[lpAttributeList->Count];

            constexpr auto ProcThreadAttributeExtendedFlags = 1;
            //0x60001，文档没有公开
            constexpr auto PROC_THREAD_ATTRIBUTE_EXTENDED_FLAGS = ProcThreadAttributeValue(ProcThreadAttributeExtendedFlags, FALSE, TRUE, TRUE);


            if (Attribute == PROC_THREAD_ATTRIBUTE_PARENT_PROCESS) //0x20000
            {
                //WinXP不支持 UAC，没实现似乎也没什么的。

                if (cbSize != sizeof(HANDLE))
                {
                    //internal::BaseSetLastNTError(0xC0000004);
                    lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }
            }
            else if (Attribute == PROC_THREAD_ATTRIBUTE_EXTENDED_FLAGS) //0x60001
            {
                //系统没有公开这个含义，暂时让他允许通过把……

                if (cbSize != sizeof(DWORD))
                {
                    //internal::BaseSetLastNTError(0xC0000004);
                    lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }

                DWORD dwOrgFlags;

                if (lpAttributeList->lpExtendedFlags)
                {
                    pAttribute = lpAttributeList->lpExtendedFlags;
                    dwOrgFlags = (DWORD)lpAttributeList->lpExtendedFlags->lpValue;
                    AttributeMark = 0;
                }
                else
                {
                    lpAttributeList->lpExtendedFlags = pAttribute;
                    dwOrgFlags = 0;
                }


                auto dwNewFlags = *(DWORD*)lpValue;

                if (dwNewFlags & ~0x00000003ul)
                {
                    //internal::BaseSetLastNTError(0xC000000D);
                    lStatus = ERROR_BAD_LENGTH;
                    break;
                }

                if ((dwFlags & PROC_THREAD_ATTRIBUTE_REPLACE_VALUE) == 0 && dwOrgFlags)
                {
                    dwNewFlags |= dwOrgFlags;
                }

                if (lpPreviousValue)
                    *(DWORD*)lpPreviousValue = dwOrgFlags;

                lpValue = (PVOID)dwNewFlags;
            }
            else if (Attribute == PROC_THREAD_ATTRIBUTE_HANDLE_LIST) //0x20002
            {
                //WinXP也不支持指定句柄继承，他会直接继承所有可继承的句柄，所以没实现好像也没什么大不了的。

                if (cbSize == 0 || cbSize % sizeof(HANDLE) != 0)
                {
                    //internal::BaseSetLastNTError(0xC0000004);
                    lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }
            }
            else
            {
                //internal::BaseSetLastNTError(0xC00000BB);
                lStatus = ERROR_NOT_SUPPORTED;
                break;
            }

            //LABEL_17
            pAttribute->lpValue = lpValue;

            if (AttributeMark)
            {
                pAttribute->Attribute = Attribute;
                pAttribute->cbSize = cbSize;
                ++lpAttributeList->Count;
                lpAttributeList->dwFlags |= AttributeMark;
            }

            return TRUE;

        } while (false);

        SetLastError(lStatus);
            
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows Vista, Windows XP Professional x64 Edition [desktop apps only]
    //Windows Server 2008, Windows Server 2003 with SP1 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    SetThreadStackGuarantee,
        _Inout_ ULONG *StackSizeInBytes
        )
    {
        if (const auto pSetThreadStackGuarantee = try_get_SetThreadStackGuarantee())
        {
            return pSetThreadStackGuarantee(StackSizeInBytes);
        }

        // 以下内容来自：ucrt\misc\resetstk.cpp


        // 因为只面向 Windows XP以及之前的系统，我们可以硬编码的确定，页大小只可能是 4K。
        // 减少 GetSystemInfo 调用。
        constexpr auto PageSize = 4096u;
        constexpr auto MinStackRequest = 2u;

        auto pStack = (LPBYTE)_alloca(1);

        auto RegionSize = *StackSizeInBytes;

        // TEB没有 GuaranteedStackBytes，没法知道上一次的结果，我们仅仅返回一个假的数值。
        *StackSizeInBytes = PageSize;

        // 长度为 0 时，不需要额外申请
        if (RegionSize == 0)
            return TRUE;

        // 我们需要额外一块 PAGE_GUARD 来进行边界防御。
        RegionSize = (RegionSize + PageSize + (PageSize - 1)) & ~(PageSize - 1);
        RegionSize = max(RegionSize, PageSize * MinStackRequest);

        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(pStack, &mbi, sizeof mbi) == 0)
            return FALSE;

        auto pStackBase = (LPBYTE)mbi.AllocationBase;

        //
        // Find the page(s) just below where the stack pointer currently points.
        // This is the highest potential guard page.
        auto pMaxGuard = (LPBYTE)(((DWORD_PTR)pStack & ~(DWORD_PTR)(PageSize - 1)) - RegionSize);

        //
        // If the potential guard page is too close to the start of the stack
        // region, abandon the reset effort for lack of space.  Win9x has a
        // larger reserved stack requirement.
        auto pMinGuard = pStackBase + PageSize;

        if (pMaxGuard < pMinGuard)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        // Set the new guard page just below the current stack page.
        DWORD flOldProtect;
        if (VirtualAlloc(pMaxGuard, RegionSize, MEM_COMMIT, PAGE_READWRITE) == nullptr
            || VirtualProtect(pMaxGuard, RegionSize, PAGE_READWRITE | PAGE_GUARD, &flOldProtect) == 0)
        {
            return FALSE;
        }

        return TRUE;
    }
#endif
        
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    GetProcessMitigationPolicy,
        _In_ HANDLE _hProcess,
        _In_ PROCESS_MITIGATION_POLICY _eMitigationPolicy,
        _Out_writes_bytes_(_cbLength) PVOID _pBuffer,
        _In_ SIZE_T _cbLength
        )
    {
        if (const auto _pfnGetProcessMitigationPolicy = try_get_GetProcessMitigationPolicy())
        {
            return _pfnGetProcessMitigationPolicy(_hProcess, _eMitigationPolicy, _pBuffer, _cbLength);
        }

        if (!_pBuffer)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if ((DWORD)_eMitigationPolicy >= (DWORD)MaxProcessMitigationPolicy)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryInformationProcess = try_get_NtQueryInformationProcess();
        if (!NtQueryInformationProcess)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
#endif

        if (_eMitigationPolicy == ProcessDEPPolicy)
        {
            if (_cbLength != sizeof(PROCESS_MITIGATION_DEP_POLICY))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }
            KEXECUTE_OPTIONS _DepOptions = {};
            NTSTATUS _Status = NtQueryInformationProcess(_hProcess, ProcessExecuteFlags, &_DepOptions, sizeof(_DepOptions), nullptr);
            if (_Status >= 0)
            {
                auto _pDepPolicy = (PROCESS_MITIGATION_DEP_POLICY*)_pBuffer;
                _pDepPolicy->Enable = _DepOptions.ExecuteEnable ? 0 : 1;
                _pDepPolicy->DisableAtlThunkEmulation = _DepOptions.DisableThunkEmulation;
                _pDepPolicy->ReservedFlags = 0;
                _pDepPolicy->Permanent = _DepOptions.Permanent;
                return TRUE;
            }
            else if (STATUS_INVALID_INFO_CLASS == _Status || STATUS_NOT_SUPPORTED == _Status)
            {
                *(DWORD*)_pBuffer = 0;
                return TRUE;
            }
            else
            {
                internal::BaseSetLastNTError(_Status);
                return FALSE;
            }
        }
        else if (_eMitigationPolicy == ProcessMitigationOptionsMask)
        {
            if (_cbLength < sizeof(UINT64))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            memset(_pBuffer, 0, _cbLength);
            return TRUE;
        }
        else
        {
            if (_cbLength != sizeof(DWORD))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            YY_ProcessPolicyInfo _Info = { static_cast<DWORD>(_eMitigationPolicy) };
            NTSTATUS _Status = NtQueryInformationProcess(_hProcess, YY_ProcessPolicy, &_Info, sizeof(_Info), nullptr);
            if (_Status >= 0)
            {
                *(DWORD*)_pBuffer = _Info.Flags;
                return TRUE;
            }
            else if (STATUS_INVALID_INFO_CLASS == _Status || STATUS_NOT_SUPPORTED == _Status)
            {
                // 如果没有这个特性，那么统一设置为0，表示内部所有环境方案都处于关闭状态
                *(DWORD*)_pBuffer = 0;
                return TRUE;
            }
            else
            {
                internal::BaseSetLastNTError(_Status);
                return FALSE;
            }
        }
            
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    SetProcessMitigationPolicy,
        _In_ PROCESS_MITIGATION_POLICY _eMitigationPolicy,
        _In_reads_bytes_(_cbLength) PVOID _pBuffer,
        _In_ SIZE_T _cbLength
        )
    {
        if (const auto _pfnSetProcessMitigationPolicy = try_get_SetProcessMitigationPolicy())
        {
            return _pfnSetProcessMitigationPolicy(_eMitigationPolicy, _pBuffer, _cbLength);
        }

        if (!_pBuffer)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if ((DWORD)_eMitigationPolicy >= (DWORD)MaxProcessMitigationPolicy || _eMitigationPolicy == ProcessMitigationOptionsMask)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
#if !defined(__USING_NTDLL_LIB)
        const auto NtSetInformationProcess = try_get_NtSetInformationProcess();
        if (!NtSetInformationProcess)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
#endif

        NTSTATUS _Status;
        if (_eMitigationPolicy == ProcessDEPPolicy)
        {
            if (_cbLength != sizeof(PROCESS_MITIGATION_DEP_POLICY))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            auto& _DepPolicy = *(PROCESS_MITIGATION_DEP_POLICY*)_pBuffer;

            KEXECUTE_OPTIONS _DepOptions = {};
            if (_DepPolicy.Enable)
            {
                _DepOptions.ExecuteDisable = 1;
            }
            else
            {
                _DepOptions.ExecuteEnable = 1;
            }
            _DepOptions.DisableThunkEmulation = _DepPolicy.DisableAtlThunkEmulation;
            _DepOptions.Permanent = _DepPolicy.Permanent;

            _Status = NtSetInformationProcess(NtCurrentProcess(), YY_ProcessPolicy, &_DepOptions, sizeof(_DepOptions));
                
        }
        else
        {
            if (_cbLength != sizeof(DWORD))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            YY_ProcessPolicyInfo _Info = { static_cast<DWORD>(_eMitigationPolicy), *(DWORD*)_pBuffer };
            _Status = NtSetInformationProcess(NtCurrentProcess(), YY_ProcessPolicy, &_Info, sizeof(_Info));
        }

        if (_Status >= 0)
        {
            return TRUE;
        }
        else
        {
            internal::BaseSetLastNTError(_Status);
            return FALSE;
        }
    }
#endif
        
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    SetProcessInformation,
        _In_ HANDLE _hProcess,
        _In_ PROCESS_INFORMATION_CLASS _eProcessInformationClass,
        _In_reads_bytes_(_cbProcessInformationSize) LPVOID _pProcessInformation,
        _In_ DWORD _cbProcessInformationSize
        )
    {
        if (const auto _pfnSetProcessInformation = try_get_SetProcessInformation())
        {
            return _pfnSetProcessInformation(_hProcess, _eProcessInformationClass, _pProcessInformation, _cbProcessInformationSize);
        }

        if (_pProcessInformation == nullptr || (DWORD)_eProcessInformationClass >= (DWORD)ProcessInformationClassMax)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtSetInformationProcess = try_get_NtSetInformationProcess();
        if (!NtSetInformationProcess)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
#endif

        NTSTATUS _Status;
        if (_eProcessInformationClass == ProcessMemoryPriority)
        {
            if (_cbProcessInformationSize != sizeof(MEMORY_PRIORITY_INFORMATION))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }
            // PAGE_PRIORITY_INFORMATION
            _Status = NtSetInformationProcess(_hProcess, ProcessPagePriority, _pProcessInformation, sizeof(DWORD));
        }
        else
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }

        if (_Status >= 0)
            return TRUE;

        internal::BaseSetLastNTError(_Status);
        return FALSE;
    }
#endif
        
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    SetThreadInformation,
        _In_ HANDLE _hThread,
        _In_ THREAD_INFORMATION_CLASS _eThreadInformationClass,
        _In_reads_bytes_(_cbThreadInformationSize) LPVOID _pThreadInformation,
        _In_ DWORD _cbThreadInformationSize
        )
    {
        if (const auto _pfnSetThreadInformation = try_get_SetThreadInformation())
        {
            return _pfnSetThreadInformation(_hThread, _eThreadInformationClass, _pThreadInformation, _cbThreadInformationSize);
        }

        if (_pThreadInformation == nullptr || (DWORD)_eThreadInformationClass >= (DWORD)ThreadInformationClassMax)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
#if !defined(__USING_NTDLL_LIB)
        const auto NtSetInformationThread = try_get_NtSetInformationThread();
        if (!NtSetInformationThread)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
#endif

        NTSTATUS _Status;
        if (_eThreadInformationClass == ThreadMemoryPriority)
        {
            if (_cbThreadInformationSize != sizeof(MEMORY_PRIORITY_INFORMATION))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }
            _Status = NtSetInformationThread(_hThread, ThreadPagePriority, _pThreadInformation, sizeof(DWORD));
        }
        else if (_eThreadInformationClass == ThreadAbsoluteCpuPriority)
        {
            if (_cbThreadInformationSize != sizeof(DWORD))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }
            _Status = NtSetInformationThread(_hThread, ThreadActualBasePriority, _pThreadInformation, sizeof(DWORD));
        }
        else
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }

        if (_Status >= 0)
            return TRUE;

        internal::BaseSetLastNTError(_Status);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    GetThreadInformation,
        _In_ HANDLE _hThread,
        _In_ THREAD_INFORMATION_CLASS _eThreadInformationClass,
        _Out_writes_bytes_(_cbThreadInformationSize) LPVOID _pThreadInformation,
        _In_ DWORD _cbThreadInformationSize
        )
    {
        if (const auto _pfnGetThreadInformation = try_get_GetThreadInformation())
        {
            return _pfnGetThreadInformation(_hThread, _eThreadInformationClass, _pThreadInformation, _cbThreadInformationSize);
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryInformationThread = try_get_NtQueryInformationThread();
        if (!NtQueryInformationThread)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
#endif

        long _Status;
        if (_eThreadInformationClass == ThreadMemoryPriority)
        {
            _Status = NtQueryInformationThread(_hThread, ThreadPagePriority, _pThreadInformation, _cbThreadInformationSize, nullptr);
        }
        else if (_eThreadInformationClass == ThreadAbsoluteCpuPriority)
        {
            _Status = NtQueryInformationThread(_hThread, ThreadActualBasePriority, _pThreadInformation, _cbThreadInformationSize, nullptr);
        }
        else
        {
            _Status = STATUS_INVALID_PARAMETER;
        }

        if (_Status < 0)
        {
            internal::BaseSetLastNTError(_Status);
            return FALSE;
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    8,
    VOID,
    WINAPI,
    GetCurrentThreadStackLimits,
        _Out_ PULONG_PTR _pLowLimit,
        _Out_ PULONG_PTR _pHighLimit
        )
    {
        if (const auto _pfnGetCurrentThreadStackLimits = try_get_GetCurrentThreadStackLimits())
        {
            return _pfnGetCurrentThreadStackLimits(_pLowLimit, _pHighLimit);
        }

        const auto _pTeb = NtCurrentTeb();
        *_pLowLimit = reinterpret_cast<ULONG_PTR>(_pTeb->DeallocationStack);
        *_pHighLimit = reinterpret_cast<ULONG_PTR>(_pTeb->NtTib.StackBase);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    SetThreadIdealProcessorEx,
        _In_ HANDLE _hThread,
        _In_ PPROCESSOR_NUMBER _pIdealProcessor,
        _Out_opt_ PPROCESSOR_NUMBER _pPreviousIdealProcessor
        )
    {
        if (const auto _pfnSetThreadIdealProcessorEx = try_get_SetThreadIdealProcessorEx())
        {
            return _pfnSetThreadIdealProcessorEx(_hThread, _pIdealProcessor, _pPreviousIdealProcessor);
        }

        // 不支持的平台统一认为只有一组处理器
        // 微软这里就没有检查 _pIdealProcessor
        if (_pIdealProcessor->Group != 0 || _pIdealProcessor->Number >= MAXIMUM_PROCESSORS)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        const auto _uPreviousIdealProcessor = SetThreadIdealProcessor(_hThread, _pIdealProcessor->Number);
        if (_uPreviousIdealProcessor == static_cast<DWORD>(-1))
        {
            return FALSE;
        }

        if (_pPreviousIdealProcessor)
        {
            _pPreviousIdealProcessor->Group = 0;
            _pPreviousIdealProcessor->Number = static_cast<BYTE>(_uPreviousIdealProcessor);
            _pPreviousIdealProcessor->Reserved = 0;
        }
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    8,
    BOOL,
    WINAPI,
    GetThreadIdealProcessorEx,
        _In_ HANDLE _hThread,
        _Out_ PPROCESSOR_NUMBER _pIdealProcessor
        )
    {
        if (const auto _pfnGetThreadIdealProcessorEx = try_get_GetThreadIdealProcessorEx())
        {
            return _pfnGetThreadIdealProcessorEx(_hThread, _pIdealProcessor);
        }
            
        // 不支持的平台统一认为只有一组处理器
        const auto _uPreviousIdealProcessor = SetThreadIdealProcessor(_hThread, MAXIMUM_PROCESSORS);
        if (_uPreviousIdealProcessor == static_cast<DWORD>(-1))
        {
            return FALSE;
        }

        _pIdealProcessor->Group = 0;
        _pIdealProcessor->Number = static_cast<BYTE>(_uPreviousIdealProcessor);
        _pIdealProcessor->Reserved = 0;
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    32,
    HANDLE,
    WINAPI,
    CreateRemoteThreadEx,
        _In_  HANDLE                       _hProcess,
        _In_  LPSECURITY_ATTRIBUTES        _pThreadAttributes,
        _In_  SIZE_T                       _uStackSize,
        _In_  LPTHREAD_START_ROUTINE       _pStartAddress,
        _In_  LPVOID                       _pParameter,
        _In_  DWORD                        _fCreationFlags,
        _In_  LPPROC_THREAD_ATTRIBUTE_LIST _pAttributeList,
        _Out_ LPDWORD                      _pThreadId
        )
    {
        if (const auto _pfnCreateRemoteThreadEx = try_get_CreateRemoteThreadEx())
        {
            return _pfnCreateRemoteThreadEx(_hProcess, _pThreadAttributes, _uStackSize, _pStartAddress, _pParameter, _fCreationFlags, _pAttributeList, _pThreadId);
        }

        return CreateRemoteThread(_hProcess, _pThreadAttributes, _uStackSize, _pStartAddress, _pParameter, _fCreationFlags, _pThreadId);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    // Windows XP无法支持 PROCESS_QUERY_LIMITED_INFORMATION等权限。
    __DEFINE_THUNK(
    kernel32,
    12,
    HANDLE,
    WINAPI,
    OpenProcess,
        _In_ DWORD _fDesiredAccess,
        _In_ BOOL _bInheritHandle,
        _In_ DWORD _uProcessId
        )
    {
        const auto _pfnOpenProcess = try_get_OpenProcess();
        if (!_pfnOpenProcess)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return nullptr;
        }

        if (internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            if (PROCESS_QUERY_LIMITED_INFORMATION & _fDesiredAccess)
            {
                _fDesiredAccess |= PROCESS_QUERY_INFORMATION;
            }

            if (PROCESS_SET_LIMITED_INFORMATION & _fDesiredAccess)
            {
                _fDesiredAccess |= PROCESS_SET_INFORMATION;
            }

            // Vista以上最高为    0xFFFF
            // Windows XP最高只有 0x0FFF
            _fDesiredAccess &= ~(0xFFFFu ^ 0x0FFF);
        }

        return _pfnOpenProcess(_fDesiredAccess, _bInheritHandle, _uProcessId);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    // Windows XP无法支持 PROCESS_QUERY_LIMITED_INFORMATION等权限。
    __DEFINE_THUNK(
    kernel32,
    12,
    HANDLE,
    WINAPI,
    OpenThread,
        _In_ DWORD _fDesiredAccess,
        _In_ BOOL _bInheritHandle,
        _In_ DWORD _uThreadId
        )
    {
        const auto _pfnOpenThread = try_get_OpenThread();
        if (!_pfnOpenThread)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return nullptr;
        }

        if (internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            if (THREAD_QUERY_LIMITED_INFORMATION & _fDesiredAccess)
            {
                _fDesiredAccess |= THREAD_QUERY_INFORMATION;
            }

            if (THREAD_SET_LIMITED_INFORMATION & _fDesiredAccess)
            {
                _fDesiredAccess |= THREAD_SET_INFORMATION;
            }

            // Vista以上最高为    0xFFFF
            // Windows XP最高只有 0x03FF
            _fDesiredAccess &= ~(0xFFFFu ^ 0x03FFu);
        }

        return _pfnOpenThread(_fDesiredAccess, _bInheritHandle, _uThreadId);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_15063)

    // 最低受支持的客户端	Windows 10版本 1607 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2016[桌面应用 | UWP 应用]
    // 虽然14393就有这个接口，但是14393必须从 kernelbase加载，因此也需要修正。
    __DEFINE_THUNK(
    kernelbase,
    8,
    HRESULT,
    WINAPI,
    GetThreadDescription,
        _In_ HANDLE _hThread,
        _Outptr_result_z_ PWSTR* _pszThreadDescription
        )
    {
        if (const auto _pfnGetThreadDescription = try_get_GetThreadDescription())
        {
            return _pfnGetThreadDescription(_hThread, _pszThreadDescription);
        }

        // 假装自己是一个空的 ThreadDescription

        *_pszThreadDescription = nullptr;
        auto _szThreadDescription = (wchar_t*)LocalAlloc(LMEM_FIXED, sizeof(wchar_t));
        if (!_szThreadDescription)
            return E_OUTOFMEMORY;

        *_szThreadDescription = L'\0';
        *_pszThreadDescription = _szThreadDescription;
        return 0x10000000;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_15063)

    // 最低受支持的客户端	Windows 10版本 1607 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2016[桌面应用 | UWP 应用]
    // 虽然14393就有这个接口，但是14393必须从 kernelbase加载，因此也需要修正。
    __DEFINE_THUNK(
    kernelbase,
    8,
    HRESULT,
    WINAPI,
    SetThreadDescription,
        _In_ HANDLE _hThread,
        _In_ PCWSTR _szThreadDescription
        )
    {
        if (const auto _pfnSetThreadDescription = try_get_SetThreadDescription())
        {
            return _pfnSetThreadDescription(_hThread, _szThreadDescription);
        }

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端	Windows 10 [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2016 [桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    GetProcessDefaultCpuSets,
        _In_ HANDLE _hProcess,
        _Out_writes_to_opt_(_uCpuSetIdCount, *_puRequiredIdCount) PULONG _puCpuSetIds,
        _In_ ULONG _uCpuSetIdCount,
        _Out_ PULONG _puRequiredIdCount
        )
    {
        if (const auto _pfnGetProcessDefaultCpuSets = try_get_GetProcessDefaultCpuSets())
        {
            return _pfnGetProcessDefaultCpuSets(_hProcess, _puCpuSetIds, _uCpuSetIdCount, _puRequiredIdCount);
        }

        if (_puRequiredIdCount == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        const auto _lStatus = GetLastError();

        // 取目标进程的 CpuSet 记录。同进程直读；跨进程内部开/关映射；
        // 跨位数无法等价表达（记录宽度与掩码位宽都不同），返回 ERROR_CALL_NOT_IMPLEMENTED。
        YY_ThunksCpuSetRecord _Default = {};
        if (Fallback::GetCpuSetDefault(_hProcess, &_Default) == false)
            return FALSE;

        // 掩码 0 = 从未设置或已清除。两者对外表现一致（都返回 n = 0，都不限制新线程），
        // 因此不需要额外的 generation 计数器。
        if (_Default.Mask == 0)
        {
            *_puRequiredIdCount = 0;
            SetLastError(_lStatus);
            return TRUE;
        }

        const ULONG _cCpuSetIds = internal::BitsCount(static_cast<size_t>(_Default.Mask));

        *_puRequiredIdCount = _cCpuSetIds;

        // 与原生实测语义一致：集合为空时无论缓冲区是否为空都返回 TRUE；
        // 集合非空且缓冲区为空时返回 FALSE + ERROR_INSUFFICIENT_BUFFER，
        // 调用方据此分配缓冲区再调用（标准两阶段查询模式）。
        if (_puCpuSetIds == nullptr)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        if (_uCpuSetIdCount < _cCpuSetIds)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        Fallback::CpuSetIdsFromGroupMask(_Default.Mask, _Default.Group, _puCpuSetIds, _cCpuSetIds);
        SetLastError(_lStatus);
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端	Windows 10 [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2016 [桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    SetProcessDefaultCpuSets,
        _In_ HANDLE _hProcess,
        _In_reads_opt_(_uCpuSetIdCount) const ULONG* _puCpuSetIds,
        _In_ ULONG _uCpuSetIdCount
        )
    {
        if (const auto _pfnSetProcessDefaultCpuSets = try_get_SetProcessDefaultCpuSets())
        {
            return _pfnSetProcessDefaultCpuSets(_hProcess, _puCpuSetIds, _uCpuSetIdCount);
        }

        const auto _lStatus = GetLastError();
        YY_ThunksCpuSetRecord _Default = {};

        // 句柄与 Id 的校验顺序必须与原生一致：实测 SetProcessDefaultCpuSets(NULL, {合法Id}, 1)
        // 返回 ERROR_INVALID_HANDLE(6)，而 SetProcessDefaultCpuSets(0xDEAD, {非法Id}, 1)
        // 返回 ERROR_CPU_SET_INVALID(813)，说明 Id 校验在前、句柄解析在后。
        // 清除（count == 0）则必须先解析句柄，原生的 Set*(NULL, NULL, 0) 就返回 6。
        if (_uCpuSetIdCount!= 0)
        {
            // 只有 ids 为空但 count > 0 才是参数错误。原生在此会直接崩溃，这里不复刻。
            if (_puCpuSetIds == nullptr)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            // Id → 单个（组号, 掩码）。
            // 校验全部 Id 的有效性；跨组时只取第一组，其它组的 Id 丢弃（不报错）。
            if (!Fallback::TryGroupMaskFromCpuSetIds(_puCpuSetIds, _uCpuSetIdCount, &_Default))
            {
                SetLastError(ERROR_CPU_SET_INVALID);
                return FALSE;
            }
        }

        if (_hProcess == nullptr)
        {
            SetLastError(ERROR_INVALID_HANDLE);
            return FALSE;
        }

        if (!Fallback::SetCpuSetDefault(_hProcess, _Default))
            return FALSE;

        SetLastError(_lStatus);
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端	Windows 10 [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2016 [桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    GetThreadSelectedCpuSets,
        _In_ HANDLE _hThread,
        _Out_writes_to_opt_(_uCpuSetIdCount, *_puRequiredIdCount) PULONG _puCpuSetIds,
        _In_ ULONG _uCpuSetIdCount,
        _Out_ PULONG _puRequiredIdCount
        )
    {
        if (const auto _pfnGetThreadSelectedCpuSets = try_get_GetThreadSelectedCpuSets())
        {
            return _pfnGetThreadSelectedCpuSets(_hThread, _puCpuSetIds, _uCpuSetIdCount, _puRequiredIdCount);
        }

        if (_puRequiredIdCount == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        // 原生 CpuSet API 成功时不修改 LastError（实测预置 12345，调用后仍是 12345），
        // 而内部调用的 GetThreadGroupAffinity / GetProcessIdOfThread / OpenProcess /
        // GetProcessAffinityMask 都可能改写它，因此成功出口必须还原。
        // 快照点必须在自身 SetLastError 的参数校验之后，否则会把失败码覆盖掉。
        const auto _lStatus = GetLastError();

        GROUP_AFFINITY _ThreadGroupAffinity = {};
        if (::GetThreadGroupAffinity(_hThread, &_ThreadGroupAffinity) == FALSE)
            return FALSE;

        // 取目标线程所属进程（跨进程线程也要对）。
        const DWORD _uProcessId = ::GetProcessIdOfThread(_hThread);
        if (_uProcessId == 0)
            return FALSE;

        // 本进程直接用伪句柄，避免多余的 OpenProcess；跨进程才打开一次。
        HANDLE _hOwnerProcess = NtGetCurrentProcess();
        if (_uProcessId != ::GetCurrentProcessId())
        {
            _hOwnerProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, _uProcessId);
            if (_hOwnerProcess == nullptr)
            {
                return FALSE;
            }
        }

        DWORD_PTR _uProcessMask = 0;
        DWORD_PTR _uSystemMask  = 0;
        const bool _bGotAffinity = ::GetProcessAffinityMask(_hOwnerProcess, &_uProcessMask, &_uSystemMask);

        if (_hOwnerProcess != NtGetCurrentProcess())
            ::CloseHandle(_hOwnerProcess);

        if (_bGotAffinity == false)
            return FALSE;

        // 线程亲和性不可能大于进程亲和性，最大值就是进程亲和性。
        // 因此「线程掩码 == 进程亲和性掩码」正是「未设置自有选择」的形态。
        // 这正是新线程刚创建时的状态，也是调用方从未限制过该线程时的状态。
        const bool _bUnset = (_ThreadGroupAffinity.Group == 0
            && _ThreadGroupAffinity.Mask == (KAFFINITY)_uProcessMask);

        const ULONG _cCpuSetIds = _bUnset ? 0 : internal::BitsCount(static_cast<size_t>(_ThreadGroupAffinity.Mask));

        *_puRequiredIdCount = _cCpuSetIds;

        if (_cCpuSetIds == 0)
        {
            SetLastError(_lStatus);
            return TRUE;
        }

        if (_puCpuSetIds == nullptr)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        if (_uCpuSetIdCount < _cCpuSetIds)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        Fallback::CpuSetIdsFromGroupMask(_ThreadGroupAffinity.Mask, _ThreadGroupAffinity.Group, _puCpuSetIds, _cCpuSetIds);

        SetLastError(_lStatus);
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端	Windows 10 [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2016 [桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    SetThreadSelectedCpuSets,
        _In_ HANDLE _hThread,
        _In_reads_opt_(_uCpuSetIdCount) const ULONG* _puCpuSetIds,
        _In_ ULONG _uCpuSetIdCount
    )
    {
        if (const auto _pfnSetThreadSelectedCpuSets = try_get_SetThreadSelectedCpuSets())
        {
            return _pfnSetThreadSelectedCpuSets(_hThread, _puCpuSetIds, _uCpuSetIdCount);
        }

        const auto _lStatus = GetLastError();

        // _hThread 一律原样使用（含 NULL），不做归一化。
        // 校验顺序必须与原生一致：先校验 Id，再解析句柄。实测
        //   SetThreadSelectedCpuSets(NULL,  {非法Id}, 1) → ERROR_CPU_SET_INVALID(813)
        //   SetThreadSelectedCpuSets(0xDEAD, {合法Id}, 1) → ERROR_INVALID_HANDLE(6)
        // 跨进程线程是支持的：原生实测对其它进程的线程返回 TRUE 且不改目标进程掩码。

        // 原生 Set*(h, ids, 0) 与 Set*(h, NULL, 0) 都是合法清除，此时唯一要解析的是句柄。
        GROUP_AFFINITY _NewThreadGroupAffinity = {};

        if (_uCpuSetIdCount != 0)
        {
            if (_puCpuSetIds == nullptr)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            // Id → 单个（组号, 掩码）。
            // 校验全部 Id 的有效性；跨组时只取第一组，其它组的 Id 丢弃（不报错）。
            YY_ThunksCpuSetRecord _oRecord = {};
            if (Fallback::TryGroupMaskFromCpuSetIds(_puCpuSetIds, _uCpuSetIdCount, &_oRecord) == false)
            {
                SetLastError(ERROR_CPU_SET_INVALID);
                return FALSE;
            }

            _NewThreadGroupAffinity.Mask = _oRecord.Mask;
            _NewThreadGroupAffinity.Group = _oRecord.Group;
        }

        if (_hThread == nullptr)
        {
            SetLastError(ERROR_INVALID_HANDLE);
            return FALSE;
        }

        // 直接写线程掩码。失败时透出系统错误码（含 87 = 超出进程亲和性）。
        // 依据：微软文档明确 CpuSet 优先级低于进程 Affinity，因此此时不生效是符合预期的行为。
        if (!::SetThreadGroupAffinity(_hThread, &_NewThreadGroupAffinity, nullptr))
        {
            return FALSE;
        }

        SetLastError(_lStatus);
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端	Windows 10 [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2016 [桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    20,
    BOOL,
    WINAPI,
    GetSystemCpuSetInformation,
        _Out_writes_bytes_to_opt_(_cbInformation, *_puReturnedLength) PSYSTEM_CPU_SET_INFORMATION _pInformation,
        _In_ ULONG _cbInformation,
        _Out_ PULONG _puReturnedLength,
        _In_opt_ HANDLE _hProcess,
        _In_ ULONG _uFlags
        )
    {
        if (const auto _pfnGetSystemCpuSetInformation = try_get_GetSystemCpuSetInformation())
        {
            return _pfnGetSystemCpuSetInformation(_pInformation, _cbInformation, _puReturnedLength, _hProcess, _uFlags);
        }

        if (_puReturnedLength == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        *_puReturnedLength = 0;

        // SDK 原型把 Flags 标注为 _Reserved_，必须为 0。实测原生传任意非 0 值
        // （1 / 2 / 0x80000000）均返回 FALSE + ERROR_INVALID_PARAMETER(87) 且不回填长度。
        // 它不是过滤器位，不能静默忽略。
        if (_uFlags != 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        // 原生 CpuSet API 成功时不修改 LastError，而内部调用的
        // GetLogicalProcessorInformationEx / HeapAlloc / GetProcessId 都会改写它，
        // 因此成功出口必须还原。快照点必须在自身 SetLastError 的参数校验之后。
        const auto _lStatus = GetLastError();

        // 旧系统没有 CPU Set 调度，这里按「一个逻辑处理器 = 一个 CPU Set」模拟，
        // 让按 CPU Set 规划亲和性的调用方仍能拿到真实的组号与组内处理器编号，
        // 后续用 SetThreadSelectedCpuSets（本文件同样已模拟）即可真正落到亲和性上。
        //
        // _hProcess 只影响 ALLOCATED_TO_TARGET_PROCESS 标志，不影响返回的集合与长度，
        // 原生对无效句柄仍然报错，因此这里必须校验句柄本身。
        //
        // 实测（Windows 11 26100）：
        //   * NULL / GetCurrentProcess() / 任意可查询进程句柄 → 成功（NULL 与伪句柄都表示当前进程）
        //   * 0xDEAD → GetLastError() == ERROR_INVALID_HANDLE(6)
        //   * 只有 SYNCHRONIZE 权限的句柄 → ERROR_ACCESS_DENIED(5)
        // 用 GetProcessId 同时覆盖「句柄有效」与「可查询」两种判定，无需额外权限位组合。
        if (_hProcess != nullptr && ::GetProcessId(_hProcess) == 0)
            return FALSE;

        // 拓扑直接取自 GetLogicalProcessorInformationEx：它在本库中也有 thunk，
        // XP/2003 上会自行退化到 GetLogicalProcessorInformation / GetSystemInfo，
        // 因此这里只调用一次，不重复做退化。
        //
        // 一条 RelationProcessorCore 记录约 48 字节，8KB 栈缓冲可容纳约 170 个物理核，
        // 覆盖绝大多数机器；核更多时系统会回填所需长度并报 ERROR_INSUFFICIENT_BUFFER，
        // 此时改从堆上取。拓扑在进程生命周期内不变，重试一次足够，无需循环。
        const auto _hProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;

        BYTE _StackBuffer[8192];
        DWORD _cbBuffer = sizeof(_StackBuffer);

        PVOID _pHeapBuffer = nullptr;
        BYTE* _pBuffer = _StackBuffer;

        if (::GetLogicalProcessorInformationEx(RelationProcessorCore, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)_pBuffer, &_cbBuffer) == FALSE)
        {
            // GetLogicalProcessorInformationEx 在参数合法时只会返回 ERROR_INSUFFICIENT_BUFFER。
            // 其它错误（实际不可达；理论上只有 XP/Vista fallback 的 HeapAlloc 失败）
            // 必须原样透传，绝不改写成 ERROR_CALL_NOT_IMPLEMENTED —— 那会伪造一个
            // 原生根本不产生的错误码，把「内存不足」误报成「系统不支持 CPU Set」。
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                return FALSE;

            _pHeapBuffer = ::HeapAlloc(_hProcessHeap, 0, _cbBuffer);
            if (_pHeapBuffer == nullptr)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                return FALSE;
            }

            _pBuffer = static_cast<BYTE*>(_pHeapBuffer);

            if (::GetLogicalProcessorInformationEx(RelationProcessorCore, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)_pBuffer, &_cbBuffer) == FALSE)
            {
                ::HeapFree(_hProcessHeap, 0, _pHeapBuffer);
                return FALSE;   // 透传 LastError
            }
        }

        // SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX 是变长结构，sizeof 给出的是最大成员（x64=80 / x86=76），
        // 而 RelationProcessorCore 记录实际只有「到 Processor 字段末尾」这么长（x64=48 / x86=44）。
        // 用 sizeof 作门槛会让首条记录就被判为过短而 break，枚举恒返回 0 条。
        const DWORD _cbMinimumRecordSize = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Processor);

        // CoreIndex 的语义是「组内物理核序号」，必须按组独立计数。
        // 若用单个全局累加器，组 1 的 CoreIndex 会接在组 0 之后形成空洞。
        // 数组容量同时充当组号上限（Windows 最多 64 组），两遍循环必须用同一判据，
        // 否则回填长度会大于实际写入条数，调用方按长度遍历会读到未初始化数据。
        BYTE _uGroupCoreIndex[Fallback::kMaxProcessorGroupCount] = {};

        ULONG _cCpuSetInformation = 0;
        for (BYTE* _pCurrent = _pBuffer, *_pBufferEnd = _pBuffer + _cbBuffer; _pCurrent + _cbMinimumRecordSize <= _pBufferEnd;)
        {
            const auto _pInfo = (const SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)_pCurrent;
            if (_pInfo->Size < _cbMinimumRecordSize)
                break;

            if (_pInfo->Relationship == RelationProcessorCore)
            {
                const PROCESSOR_RELATIONSHIP& _Relation = _pInfo->Processor;
                for (WORD _uGroupIndex = 0; _uGroupIndex != _Relation.GroupCount; ++_uGroupIndex)
                {
                    if (_Relation.GroupMask[_uGroupIndex].Group >= _countof(_uGroupCoreIndex))
                        continue;

                    _cCpuSetInformation += internal::BitsCount(static_cast<size_t>(_Relation.GroupMask[_uGroupIndex].Mask));
                }
            }

            _pCurrent += _pInfo->Size;
        }

        // 一个处理器都没有时不存在可模拟的 CpuSet。
        // 该分支实际不可达（系统恒从组 0 开始填充处理器），保留为防御性检查。
        if (_cCpuSetInformation == 0)
        {
            if (_pHeapBuffer != nullptr)
                ::HeapFree(_hProcessHeap, 0, _pHeapBuffer);

            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return FALSE;
        }

        const ULONG _cbRequired = _cCpuSetInformation * sizeof(SYSTEM_CPU_SET_INFORMATION);
        *_puReturnedLength = _cbRequired;

        // 与系统语义一致：缓冲区为空或空间不足时只回填所需长度。
        if (_pInformation == nullptr || _cbInformation < _cbRequired)
        {
            if (_pHeapBuffer != nullptr)
                ::HeapFree(_hProcessHeap, 0, _pHeapBuffer);

            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        // 第二遍落盘：调用方已保证缓冲区能容纳 _cCpuSetInformation 条，不会截断。
        for (BYTE* _pCurrent = _pBuffer, *_pBufferEnd = _pBuffer + _cbBuffer; _pCurrent + _cbMinimumRecordSize <= _pBufferEnd;)
        {
            const auto _pInfo = (const SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)_pCurrent;
            if (_pInfo->Size < _cbMinimumRecordSize)
                break;

            if (_pInfo->Relationship == RelationProcessorCore)
            {
                const PROCESSOR_RELATIONSHIP& _Relation = _pInfo->Processor;
                for (WORD _uGroupIndex = 0; _uGroupIndex != _Relation.GroupCount; ++_uGroupIndex)
                {
                    const GROUP_AFFINITY& _GroupAffinity = _Relation.GroupMask[_uGroupIndex];

                    // 组号必须落在 _uGroupCoreIndex 的容量内。
                    if (_GroupAffinity.Group >= _countof(_uGroupCoreIndex))
                        continue;

                    const BYTE _uCoreIndex = _uGroupCoreIndex[_GroupAffinity.Group];

                    for (ULONG _uBit = 0; _uBit != Fallback::kCpuSetIdBitCount; ++_uBit)
                    {
                        if ((_GroupAffinity.Mask & ((KAFFINITY)1 << _uBit)) == 0)
                            continue;

                        const auto _pCpuSetInformation = _pInformation++;
                        _pCpuSetInformation->Size = sizeof(SYSTEM_CPU_SET_INFORMATION);
                        _pCpuSetInformation->Type = CpuSetInformation;
                        _pCpuSetInformation->CpuSet.Id = _GroupAffinity.Group * Fallback::kCpuSetIdBitCount + _uBit;
                        _pCpuSetInformation->CpuSet.Group = _GroupAffinity.Group;
                        _pCpuSetInformation->CpuSet.LogicalProcessorIndex = (BYTE)_uBit;
                        _pCpuSetInformation->CpuSet.CoreIndex = _uCoreIndex;
                        _pCpuSetInformation->CpuSet.LastLevelCacheIndex = 0;
                        _pCpuSetInformation->CpuSet.NumaNodeIndex = 0;
                        _pCpuSetInformation->CpuSet.EfficiencyClass = _Relation.EfficiencyClass;
                        // 实测原生在 32 个 CpuSet 上 ALLOCATED 计数恒为 0、PARKED 计数为 28。
                        // ALLOCATED 表示「被某个进程独占分配」，旧系统没有该机制，置位会误导调用方跳过该 CpuSet；
                        // PARKED 需要内核驻留状态，fallback 无法获知。故一律填 0（普通、可用、非独占）。
                        _pCpuSetInformation->CpuSet.AllFlags = 0;
                        _pCpuSetInformation->CpuSet.SchedulingClass = 0;
                        _pCpuSetInformation->CpuSet.AllocationTag = 0;
                    }

                    ++_uGroupCoreIndex[_GroupAffinity.Group];
                }
            }

            _pCurrent += _pInfo->Size;
        }

        if (_pHeapBuffer != nullptr)
            ::HeapFree(_hProcessHeap, 0, _pHeapBuffer);

        SetLastError(_lStatus);
        return TRUE;
    }
#endif

} //namespace YY::Thunks
