#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_advapi32)
#define __Comment_Lib_advapi32
#pragma comment(lib, "Advapi32.lib")
#endif

namespace YY::Thunks {
namespace {
#if (YY_Thunks_Target < __WindowsNT6)

#if !defined(YY_Thunks_Implemented)
template<typename Char>
BOOL __fastcall IsLocalNamedPipePath(_In_opt_ const Char* _szFileName) noexcept
{
    if (_szFileName == nullptr)
        return FALSE;

    static const char kLocalNamedPipePrefix[] = "\\\\.\\pipe\\";
    return StringCompareIgnoreCaseByAscii(_szFileName, kLocalNamedPipePrefix, _countof(kLocalNamedPipePrefix) - 1) == 0;
}

void __fastcall RegisterPipeClientProcessId(_In_ HANDLE _hNamedPipe) noexcept;

#else // defined(YY_Thunks_Implemented)
// 头部结构：只放"池内全局代次计数器"，不放任何计数字段。
// 刻意不放 cbSize / 桶数 / 每桶槽数 / 版本号等元信息：这些都能由编译期常量推导，
// 写进共享内存只会引入"必须先初始化才能用"的依赖，并带来"初始化到一半崩溃"的不一致风险。
struct PipePeerPidPoolHeader    // 64 字节，恰一个缓存行
{
    // 全局代次计数器（32 位，与槽位内代次位宽一致）。
    // 每次 GetPipePeerPidSlot 调用原子递增；位于池内使不同进程写入的代次互相可比。
    ULONG uGeneration;
    // 保留，填满 64 字节；将来追加诊断计数不必更换共享实例名称
    ULONG uReserved[15];
};

// 槽位结构：uPack 把"实例索引（低 32 位）+ 代次时间戳（高 32 位）"打包为一个
// 原子单元，使"校验索引 + 刷新代次"能在同一次 64 位 CAS 内完成：
// 若两者分开存放，"先校验、再写代次"就是两步，期间槽位若被他人替换，
// 代次会被写到别人的槽位上（LRU 失真）。
// x86 的 cmpxchg8b 与 x64 的原生 64 位 cmpxchg 都支持该操作；
// 不能依赖 128 位 CAS（x86 无此指令，与 32 位兼容目标冲突）。
struct PipePeerPidSlot          // 16 字节
{
    union
    {
        ULONG64 uPack;          // 原子读写单位（InterlockedCompareExchange64）
        struct
        {
            volatile ULONG uInstanceIndex;   // 低 32 位：0 = 未使用；其他 = 实例索引
            volatile ULONG uGeneration;      // 高 32 位：最近一次登记/续命的时间戳
        };
    };
    volatile ULONG uServerProcessId;     // 服务端 PID，0 = 未发布
    volatile ULONG uClientProcessId;     // 客户端 PID，0 = 未发布
};

// 池总字节：640 KB
constexpr ULONG kPipePeerPidPoolBytes = 640 * 1024;
constexpr ULONG kPipePeerPidPoolHeaderBytes = sizeof(PipePeerPidPoolHeader);
// 每桶 12 槽：桶内线性扫描的固定上界
constexpr ULONG kPipePeerPidSlotsPerBucket = 12;
constexpr ULONG kPipePeerPidSlotBytes = sizeof(PipePeerPidSlot);
// 桶 192 字节 = 3 个缓存行；必须为 64 的整数倍，保证每桶起始落在缓存行边界（避免相邻桶伪共享）
constexpr ULONG kPipePeerPidBucketBytes = kPipePeerPidSlotsPerBucket * kPipePeerPidSlotBytes;


// 桶数：由池大小与结构体尺寸推导（整除无浪费：655,296 / 192 = 3,413）。
// 桶定位采用"实例索引直接取模"：索引由内核按序分配、单调递增，
// 对质数取模相当于以步长 1 遍历剩余类，连续索引必然落到互不相同的桶
// （免哈希即天然均匀）。
constexpr ULONG kPipePeerPidBucketCount = (kPipePeerPidPoolBytes - kPipePeerPidPoolHeaderBytes) / kPipePeerPidBucketBytes;

// 总槽位：3,413 × 12 = 40,956
constexpr ULONG kPipePeerPidSlotCount = kPipePeerPidBucketCount * kPipePeerPidSlotsPerBucket;

// 编译期质数判定（试除到 sqrt）：桶数若被调整为合数，取模分布的均匀性假设失效
// （例如偶数桶时连续索引会交替命中奇偶桶，实际可用桶数减半），
// 必须在此显式编译失败。
constexpr BOOL IsPipePeerPidPrime(ULONG _uValue, ULONG _uDivisor)
{
    return _uDivisor * _uDivisor > _uValue
        ? (_uValue >= 2 ? TRUE : FALSE)
        : (_uValue % _uDivisor == 0 ? FALSE : IsPipePeerPidPrime(_uValue, _uDivisor + 1));
}

constexpr BOOL IsPipePeerPidPrime(ULONG _uValue)
{
    return IsPipePeerPidPrime(_uValue, 2);
}

// 布局校验：任何布局改动必须在这里显式失败，避免后续调整时静默错位
static_assert(sizeof(PipePeerPidPoolHeader) == kPipePeerPidPoolHeaderBytes, "头部必须恰为一个缓存行（64 字节）");
static_assert(sizeof(PipePeerPidSlot) == kPipePeerPidSlotBytes, "槽位必须为 16 字节");
// uPack 必须位于槽位起始：x86 的 cmpxchg8b 依赖 8 字节对齐（SPEC §4.2）
static_assert(UFIELD_OFFSET(PipePeerPidSlot, uPack) == 0, "uPack 必须位于槽位起始以保证 8 字节对齐");
static_assert(kPipePeerPidBucketBytes % 64 == 0, "桶字节必须为 64 的整数倍（每桶起始落在缓存行边界）");
static_assert((kPipePeerPidPoolBytes - kPipePeerPidPoolHeaderBytes) % kPipePeerPidBucketBytes == 0, "槽位区必须被桶字节整除（无尾部浪费）");
static_assert(IsPipePeerPidPrime(kPipePeerPidBucketCount), "桶数必须为质数：取模定位依赖桶数为质数以获得均匀分布");

// ---- 宽松 DACL 构造（SPEC §4.1 "DACL 要求"）----
// 主池必须显式设置宽松 DACL（允许 Everyone 完整访问），否则其他会话的用户进程
// 无法打开，跨会话场景将静默失效。
// 缓冲区使用静态存储，不使用动态容器；构造池是低频一次性操作。
struct PipePeerPidLooseSecurity
{
    SECURITY_ATTRIBUTES SecurityAttributes;
    SECURITY_DESCRIPTOR SecurityDescriptor;
    // ACL 缓冲：ACL 头部 + 一个 ACCESS_ALLOWED_ACE 头部（不含 SidStart）+ 最大 SID 长度
    BYTE AclBuffer[sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + 128];
    BYTE SidBuffer[128];

    /// <summary>
    /// 构造允许 Everyone 完整访问（FILE_MAP_ALL_ACCESS）的 SECURITY_ATTRIBUTES。
    /// </summary>
    /// <returns>TRUE = 构造成功，SecurityAttributes 可用于创建共享内存；FALSE = 构造失败。</returns>
    /// <remarks>
    ///   使用 InitializeSecurityDescriptor → AllocateAndInitializeSid → InitializeAcl
    ///   → AddAccessAllowedAce → SetSecurityDescriptorDacl，均为 XP 可用 API。
    /// </remarks>
    BOOL Init() noexcept
    {
        if (!InitializeSecurityDescriptor(&SecurityDescriptor, SECURITY_DESCRIPTOR_REVISION))
            return FALSE;

        // Everyone（World）SID：S-1-1-0（SECURITY_WORLD_RID 为 0）
        SID_IDENTIFIER_AUTHORITY _WorldAuthority = SECURITY_WORLD_SID_AUTHORITY;
        PSID _pTempSid = nullptr;
        if (!AllocateAndInitializeSid(&_WorldAuthority, 1, SECURITY_WORLD_RID,
                                      0, 0, 0, 0, 0, 0, 0, &_pTempSid))
        {
            return FALSE;
        }

        const DWORD _cbSid = GetLengthSid(_pTempSid);
        if (_cbSid > sizeof(SidBuffer))
        {
            FreeSid(_pTempSid);
            return FALSE;
        }
        // 复制进本地缓冲后释放：使安全属性不再依赖堆分配的 SID
        CopyMemory(SidBuffer, _pTempSid, _cbSid);
        FreeSid(_pTempSid);

        const DWORD _cbAcl = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + _cbSid;
        if (!InitializeAcl((PACL)AclBuffer, _cbAcl, ACL_REVISION))
            return FALSE;

        if (!AddAccessAllowedAce((PACL)AclBuffer, ACL_REVISION, FILE_MAP_ALL_ACCESS, (PSID)SidBuffer))
            return FALSE;
        if (!SetSecurityDescriptorDacl(&SecurityDescriptor, TRUE, (PACL)AclBuffer, FALSE))
            return FALSE;

        SecurityAttributes.nLength = sizeof(SecurityAttributes);
        SecurityAttributes.lpSecurityDescriptor = &SecurityDescriptor;
        SecurityAttributes.bInheritHandle = FALSE;
        return TRUE;
    }
};

/// <summary>
/// 解析（打开；必要时创建）指定名称的槽位池，并把视图发布到进程内缓存。
/// </summary>
/// <param name="_pCache">该池对应的进程内缓存槽位。</param>
/// <param name="_szPoolName">池对象名（主池含 Global\ 前缀，备池为无前缀形式）。</param>
/// <param name="_bAllowCreate">TRUE = 池不存在时创建（仅服务端登记路径可传）；FALSE = 只打开既有池。</param>
/// <returns>
///   成功：池视图首地址（指向 64 字节头部），进程生命周期内有效。
///   失败：nullptr（对象不存在、无权限、内存不足等），调用方按"池不可用"降级。
/// </returns>
/// <remarks>
///   打开/创建失败的结果不缓存：下次调用会重新尝试，
///   以覆盖"服务端晚于客户端创建池"的时序。
///   本函数不设置 GetLastError（调用方处于被拦截 API 之后，其 LastError 必须保持）。
/// </remarks>
_Ret_maybenull_ PipePeerPidPoolHeader* __fastcall ResolvePipePeerPidPool(
    _Inout_ PipePeerPidPoolHeader** _ppPoolCache,
    _In_z_ const wchar_t* _szPoolName,
    _In_ BOOL _bAllowCreate) noexcept
{
    // 快路径：缓存命中（对齐指针读；写入由下方 InterlockedCompareExchangePointer 发布）
    if (auto _pCached = *_ppPoolCache)
        return _pCached;

    // 慢路径：先尝试打开既有池
    HANDLE _hMapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, _szPoolName);
    if (_hMapping == nullptr && _bAllowCreate)
    {
        // 池不存在（或无权限打开）时创建：
        // 创建 Global\ 主池需要 SeCreateGlobalPrivilege，无特权进程会在这里失败
        // （ERROR_ACCESS_DENIED），由调用方回退到备池（SPEC §4.1）
        PipePeerPidLooseSecurity _LooseSecurity;
        SECURITY_ATTRIBUTES* _pSecurityAttributes = nullptr;
        if (_LooseSecurity.Init())
            _pSecurityAttributes = &_LooseSecurity.SecurityAttributes;

        _hMapping = CreateFileMappingW(INVALID_HANDLE_VALUE, _pSecurityAttributes,
            PAGE_READWRITE, 0, kPipePeerPidPoolBytes, _szPoolName);
    }
    if (_hMapping == nullptr)
        return nullptr;

    auto _pPool = (PipePeerPidPoolHeader*)MapViewOfFile(_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, kPipePeerPidPoolBytes);
    if (_pPool == nullptr)
    {
        CloseHandle(_hMapping);
        return nullptr;
    }

    // CAS 发布视图：并发初始化时只有一个线程胜出，
    // 其余线程清理本次多余的映射并复用胜出者的视图（两者指向同一 section）
    if (auto _pPublished = (PipePeerPidPoolHeader*)InterlockedCompareExchangePointer(
        (PVOID volatile*)_ppPoolCache, _pPool, nullptr))
    {
        UnmapViewOfFile(_pPool);
        CloseHandle(_hMapping);
        return _pPublished;
    }

    // 如果发布成功：长期持有映射句柄，保证进程生命周期内 section 不被销毁
    return _pPool;
}

/// <summary>
/// 在指定池中查找（必要时认领空槽或替换最旧槽位）实例索引对应的槽位。
/// </summary>
/// <param name="_pPool">已解析的池视图。</param>
/// <param name="_uInstanceIndex">管道实例索引（必须非 0，由调用方保证）。</param>
/// <param name="_bAllowCreate">TRUE = 未命中时认领空槽或替换桶内代次最旧者；FALSE = 只查找。</param>
/// <returns>
///   成功：槽位指针，调用方直接读写 uServerProcessId / uClientProcessId。
///   失败：nullptr —— 未命中且 _bAllowCreate 为 FALSE。
/// </returns>
/// <remarks>
///   命中时执行"续命"：以一次 64 位 CAS 完成"校验索引 + 刷新代次"，
///   只改高 32 位代次，不改索引与 PID —— 这正是"同一槽位可被服务端与客户端
///   先后使用"的基础（服务端创建槽位并写 uServerProcessId，
///   客户端随后命中同一槽位并只写 uClientProcessId）。
///   全部状态变更只用 uPack 的 64 位 CAS 与 PID 字段的 32 位交换，无锁；
///   CAS 失败时重新扫描桶，不做忙等。本函数不设置 GetLastError。
/// </remarks>
_Ret_maybenull_ PipePeerPidSlot* __fastcall FindOrCreatePipePeerPidSlot(
    _In_ PipePeerPidPoolHeader* _pPool,
    _In_ ULONG _uInstanceIndex,
    _In_ BOOL _bAllowCreate) noexcept
{
    // 本次调用的时间戳取自池内全局代次计数器（32 位）：
    // 只有全局单调递增值才能让不同进程写入的时间戳互相可比
    const ULONG _uGeneration = (ULONG)InterlockedIncrement((volatile LONG*)&_pPool->uGeneration);
    const ULONG64 _uNewPack = ((ULONG64)_uGeneration << 32) | _uInstanceIndex;

    // 桶定位：直接取模（依赖桶数为质数，见 kPipePeerPidBucketCount 处的 static_assert）。
    // 槽位区起始于头部之后；每桶 kPipePeerPidBucketBytes 字节。
    auto _pBucket = (PipePeerPidSlot*)((BYTE*)(_pPool + 1) +
        (SIZE_T)(_uInstanceIndex % kPipePeerPidBucketCount) * kPipePeerPidBucketBytes);

    for (;;)
    {
        // 扫描桶：命中优先；同时记录一个空槽与代次最旧的槽位，供新增使用
        PipePeerPidSlot* _pMatch = nullptr;
        ULONG64 _uMatchPack = 0;
        PipePeerPidSlot* _pEmpty = nullptr;
        PipePeerPidSlot* _pOldest = nullptr;
        ULONG _uOldestGeneration = 0;

        for (ULONG _iSlot = 0; _iSlot != kPipePeerPidSlotsPerBucket; ++_iSlot)
        {
            auto& _Slot = _pBucket[_iSlot];
            const ULONG64 _uPack = _Slot.uPack;

            if ((ULONG)_uPack == _uInstanceIndex)
            {
                // 命中：直接比较低 32 位索引，不必比较整个 uPack
                _pMatch = &_Slot;
                _uMatchPack = _uPack;
                break;
            }

            if (_uPack == 0)
            {
                // 未使用：仅当整个 uPack 为 0 才可直接认领
                //（认领 CAS 期望的是整个 64 位为 0，两者判据必须一致；SPEC §4.2.3）
                if (_pEmpty == nullptr)
                    _pEmpty = &_Slot;
            }
            else
            {
                // 已用或"写入中"中间态：参与最旧槽位评选。
                const ULONG _uSlotGeneration = (ULONG)(_uPack >> 32);
                if (_pOldest == nullptr || (_uSlotGeneration - _uOldestGeneration) < 0)
                {
                    _pOldest = &_Slot;
                    _uOldestGeneration = _uSlotGeneration;
                }
            }
        }

        if (_pMatch != nullptr)
        {
            // 命中续命：校验索引与刷新代次在同一次 CAS 内完成。
            // 若分开执行，期间槽位被他人替换时，代次会被写到别人的槽位上（LRU 失真）
            if (_InterlockedCompareExchange64((volatile LONG64*)&_pMatch->uPack, (LONG64)_uNewPack, (LONG64)_uMatchPack) == (LONG64)_uMatchPack)
                return _pMatch;
            // CAS 失败（期间被替换）→ 重新扫描
            continue;
        }

        if (!_bAllowCreate)
            return nullptr;     // 查询与客户端路径从不创建或替换槽位（权限收敛到服务端）

        if (_pEmpty != nullptr)
        {
            // 认领空槽：直接发布（期望整个 uPack 为 0）
            if (_InterlockedCompareExchange64((volatile LONG64*)&_pEmpty->uPack, (LONG64)_uNewPack, 0) == 0)
                return _pEmpty;
            // 空槽被并发抢占 → 重新扫描
            continue;
        }

        // 桶满：替换代次最旧的槽位（三步协议）。
        // 中间态低 32 位为 0（对读侧不可见）、高 32 位非 0（不会被误判为"未使用"而被直接认领）
        const LONG64 _uMidPack = (LONG64)((ULONG64)_uGeneration << 32);
        const LONG64 _uOldPack = (LONG64)_pOldest->uPack;
        if (_InterlockedCompareExchange64((volatile LONG64*)&_pOldest->uPack, _uMidPack, _uOldPack) != _uOldPack)
            continue;           // 期间被他人变更 → 重新扫描

        // 进入中间态后清零两个 PID：即使随后崩溃，
        // 也不会出现"索引已换新、PID 仍是上一实例"的组合（避免返回错误 PID）
        InterlockedExchange((volatile LONG*)&_pOldest->uServerProcessId, 0);
        InterlockedExchange((volatile LONG*)&_pOldest->uClientProcessId, 0);

        // 发布新索引（中间态只能被本流程的发布改变，失败仅作兜底）
        if (_InterlockedCompareExchange64((volatile LONG64*)&_pOldest->uPack, (LONG64)_uNewPack, _uMidPack) != _uMidPack)
            continue;
        return _pOldest;
    }
}

/// <summary>
/// 取得实例索引对应的槽位（槽位池的唯一访问入口）。
/// 先主池（Global\）后备池（无前缀）：任一池命中即返回；
/// 主池未命中且本次允许新增时，只在主池新增（槽位新增权限收敛到服务端）。
/// </summary>
/// <param name="_uInstanceIndex">
///   管道实例索引（对端身份键），由 QueryPipeInstanceIndex 取得；传入 0 视为无效输入。
/// </param>
/// <param name="_bAllowCreate">
///   TRUE：命中则续命；未命中时优先认领空槽，桶内无空槽则替换代次最旧的槽位（仅服务端传 TRUE）。
///   FALSE：只查找；命中则续命后返回，未命中返回 nullptr，不创建也不替换槽位。
/// </param>
/// <returns>
///   成功：槽位指针（生命周期与池的进程内缓存一致，进程退出前一直有效）。
///   失败：nullptr —— 索引为 0 / 池不可用 / 未命中且 _bAllowCreate 为 FALSE。
/// </returns>
/// <remarks>
///   每次调用都会递增池内的全局代次计数器作为本次调用的时间戳；
///   命中既有槽位时以一次 64 位 CAS 完成"校验索引 + 刷新代次"（续命），
///   因此查询路径也会产生共享内存写入（跨进程场景下有缓存行同步开销），
///   这是为"长期活跃的连接不被误替换"而付的代价。线程安全（Interlocked 系列），
///   不设置 GetLastError。
/// </remarks>
_Ret_maybenull_ PipePeerPidSlot* __fastcall GetPipePeerPidSlot(
    _In_ ULONG _uInstanceIndex,
    _In_ BOOL _bAllowCreate) noexcept
{
    if (_uInstanceIndex == 0)
        return nullptr;

    auto _pSharedData = GetYY_ThunksSharedData();

    // 先主池
    if (auto _pPool = ResolvePipePeerPidPool((PipePeerPidPoolHeader**)& _pSharedData->pPipePeerPidPoolv1[0], L"Global\\YY-Thunks-PipePeerPid-v1", _bAllowCreate))
    {
        if (auto _pSlot = FindOrCreatePipePeerPidSlot(_pPool, _uInstanceIndex, _bAllowCreate))
            return _pSlot;
    }

    // 主池不可用或未命中 → 备池
    if (auto _pPool = ResolvePipePeerPidPool((PipePeerPidPoolHeader**)&_pSharedData->pPipePeerPidPoolv1[1], L"YY-Thunks-PipePeerPid-v1", _bAllowCreate))
    {
        return FindOrCreatePipePeerPidSlot(_pPool, _uInstanceIndex, _bAllowCreate);
    }

    return nullptr;
}

/// <summary>
/// 取命名管道实例的索引，作为对端之间的身份键（同一实例的两端取值相同）。
/// </summary>
/// <param name="_hNamedPipe">命名管道实例句柄（服务端端或客户端端均可）。</param>
/// <param name="_puInstanceIndex">输出：实例索引（32 位）。</param>
/// <returns>
///   ERROR_SUCCESS：成功。
///   其他值：失败 —— ntdll 无法解析、NtQueryInformationFile 调用失败，
///          或索引的高 32 位非 0（见 remarks）。
/// </returns>
/// <remarks>
///   实现为 NtQueryInformationFile(_hNamedPipe, FileInternalInformation)，
///   读取 FILE_INTERNAL_INFORMATION.IndexNumber（LARGE_INTEGER）；与
///   GetFileInformationByHandle 得到的 nFileIndexHigh:Low 等值，但只需一次内核往返。
///
///   槽位池把索引与代次打包为 64 位，索引只能占低 32 位：高 32 位非 0 时本函数
///   返回失败（由调用方映射为 ERROR_NOT_SUPPORTED），**绝不截断后当键使用**
///   —— 截断会让两个不同实例映射到同一键，直接导致返回错误 PID。
/// </remarks>
_Check_return_ LSTATUS __fastcall QueryPipeInstanceIndex(
    _In_ HANDLE _hNamedPipe,
    _Out_ ULONG* _puInstanceIndex) noexcept
{
    DWORD _fFlags = 0;
    if (!GetNamedPipeInfo(_hNamedPipe, &_fFlags, nullptr, nullptr, nullptr))
        return GetLastError();

#if !defined(__USING_NTDLL_LIB)
    const auto NtQueryInformationFile = try_get_NtQueryInformationFile();
    if (NtQueryInformationFile == nullptr)
        return ERROR_NOT_SUPPORTED;
#endif

    FILE_INTERNAL_INFORMATION _InternalInformation = {};
    IO_STATUS_BLOCK _IoStatusBlock = {};
    const auto _Status = NtQueryInformationFile(_hNamedPipe, &_IoStatusBlock,
        &_InternalInformation, sizeof(_InternalInformation), FileInternalInformation);
    if (_Status < 0)
        return ERROR_INVALID_HANDLE;

    // 32 位约束：高 32 位非 0 必须拒绝（绝不截断）；
    // 索引 0 是槽位"未使用/写入中"的保留编码，同样视为失败
    if (_InternalInformation.IndexNumber.HighPart != 0 || _InternalInformation.IndexNumber.LowPart == 0)
        return ERROR_NOT_SUPPORTED;

    *_puInstanceIndex = _InternalInformation.IndexNumber.LowPart;
    return ERROR_SUCCESS;
}


/// <summary>
/// 查询管道对端 PID（两个查询 API 的共享 fallback 实现）。
/// </summary>
/// <param name="_hNamedPipe">命名管道句柄，且调用方已确认其端点与查询目标相反。</param>
/// <param name="_bQueryServerProcessId">TRUE = 查询服务端 PID（调用方为客户端）；FALSE = 查询客户端 PID。</param>
/// <param name="_pProcessId">输出：对端 PID；失败时不写入。</param>
/// <returns>TRUE = 成功；FALSE = 失败（GetLastError = ERROR_NOT_SUPPORTED）。</returns>
_Check_return_ BOOL __fastcall QueryPipePeerProcessId(
    _In_ HANDLE _hNamedPipe,
    _In_ BOOL _bQueryServerProcessId,
    _Out_ PULONG _pProcessId) noexcept
{
    ULONG _uInstanceIndex = 0;
    auto _lStatus = QueryPipeInstanceIndex(_hNamedPipe, &_uInstanceIndex);
    if (_lStatus != ERROR_SUCCESS)
    {
        if (_lStatus == ERROR_INVALID_PARAMETER)
            _lStatus = ERROR_INVALID_FUNCTION;

        SetLastError(_lStatus);
        return FALSE;
    }

    if (_bQueryServerProcessId)
    {
        // 查询服务端 PID 时，服务端可能还未登记，所以多次尝试几次GetPipePeerPidSlot
        for (int i = 0; i != 2; ++i)
        {
            auto _pSlot = GetPipePeerPidSlot(_uInstanceIndex, FALSE);
            if (_pSlot)
            {
                const ULONG _uProcessId = _pSlot->uServerProcessId;
                if (_pSlot->uInstanceIndex != _uInstanceIndex)
                {
                    break;  // 槽位已被替换 → 失败
                }

                if (_uProcessId)
                {
                    *_pProcessId = _uProcessId;
                    return TRUE;
                }
            }

            Sleep(10);
        }
    }
    else
    {
        // 查询客户端 PID 时，服务端肯定已经登记了PID
        auto _pSlot = GetPipePeerPidSlot(_uInstanceIndex, FALSE);
        if (_pSlot)
        {
            for (int i = 0; i != 2; ++i)
            {

                const ULONG _uProcessId = _pSlot->uClientProcessId;
                if (_pSlot->uInstanceIndex != _uInstanceIndex)
                {
                    break;  // 槽位已被替换 → 失败
                }

                if (_uProcessId)
                {
                    *_pProcessId = _uProcessId;
                    return TRUE;
                }

                Sleep(10);
            }
        }
    }
    
    SetLastError(ERROR_NOT_FOUND);
    return FALSE;
}


/// <summary>
/// 登记命名管道实例的服务端 PID（供 CreateNamedPipeA/W 在原生调用成功后调用）。
/// </summary>
/// <param name="_hNamedPipe">新建的命名管道服务端句柄。</param>
/// <returns>
///   无返回值。一切失败静默降级：仅导致后续 PID 查询返回 ERROR_NOT_SUPPORTED，
///   不得影响调用方（被拦截 API）的返回值。
/// </returns>
/// <remarks>
///   原生查询 API 存在（Vista+）时不触碰槽位池：系统自行维护 PID 关系。
///   仅服务端传 _bAllowCreate=TRUE —— 槽位新增权限收敛到服务端。
///   本函数可能改变 GetLastError（对象打开/创建均会设置），由调用方负责保存与恢复。
/// </remarks>
void __fastcall RegisterPipeServerProcessId(_In_ HANDLE _hNamedPipe) noexcept
{
    ULONG _uInstanceIndex = 0;
    auto _lStatus = QueryPipeInstanceIndex(_hNamedPipe, &_uInstanceIndex);
    if (_lStatus != ERROR_SUCCESS)
        return;

    auto _pSlot = GetPipePeerPidSlot(_uInstanceIndex, TRUE);
    if (_pSlot == nullptr)
        return;

    InterlockedExchange((volatile LONG*)&_pSlot->uServerProcessId, (LONG)GetCurrentProcessId());
}

void __fastcall RegisterPipeClientProcessId(_In_ HANDLE _hNamedPipe) noexcept
{
    ULONG _uInstanceIndex = 0;
    auto _lStatus = QueryPipeInstanceIndex(_hNamedPipe, &_uInstanceIndex);
    if (_lStatus != ERROR_SUCCESS)
        return;

    auto _pSlot = GetPipePeerPidSlot(_uInstanceIndex, FALSE);
    if (_pSlot == nullptr)
        return;

    InterlockedExchange((volatile LONG*)&_pSlot->uClientProcessId, (LONG)GetCurrentProcessId());
}
#endif // YY_Thunks_Implemented

#endif // YY_Thunks_Target < __WindowsNT6
} // namespace
} // namespace YY::Thunks

namespace YY::Thunks {
#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
kernel32,
8,
BOOL,
WINAPI,
GetNamedPipeClientProcessId,
    _In_ HANDLE _hNamedPipe,
    _Out_ PULONG _pClientProcessId
    )
{
    if (auto const _pfnGetNamedPipeClientProcessId = try_get_GetNamedPipeClientProcessId())
    {
        return _pfnGetNamedPipeClientProcessId(_hNamedPipe, _pClientProcessId);
    }

    if (_pClientProcessId == nullptr)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    return QueryPipePeerProcessId(_hNamedPipe, FALSE, _pClientProcessId);
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
// 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
__DEFINE_THUNK(
kernel32,
8,
BOOL,
WINAPI,
GetNamedPipeServerProcessId,
    _In_ HANDLE _hNamedPipe,
    _Out_ PULONG _pServerProcessId
    )
{
    if (auto const _pfnGetNamedPipeServerProcessId = try_get_GetNamedPipeServerProcessId())
    {
        return _pfnGetNamedPipeServerProcessId(_hNamedPipe, _pServerProcessId);
    }

    if (_pServerProcessId == nullptr)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    return QueryPipePeerProcessId(_hNamedPipe, TRUE, _pServerProcessId);
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows NT 4.0 [桌面应用 | UWP 应用]
// 最低受支持的服务器	Windows NT 4.0 [桌面应用 | UWP 应用]
__DEFINE_THUNK(
kernel32,
32,
HANDLE,
WINAPI,
CreateNamedPipeW,
    _In_ LPCWSTR _szPipeName,
    _In_ DWORD _uOpenMode,
    _In_ DWORD _uPipeMode,
    _In_ DWORD _uMaxInstances,
    _In_ DWORD _uOutBufferSize,
    _In_ DWORD _uInBufferSize,
    _In_ DWORD _uDefaultTimeout,
    _In_opt_ LPSECURITY_ATTRIBUTES _pSecurityAttributes
    )
{
    const auto _pfnCreateNamedPipeW = try_get_CreateNamedPipeW();
    if (_pfnCreateNamedPipeW == nullptr)
    {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return INVALID_HANDLE_VALUE;
    }

    const auto _hNamedPipe = _pfnCreateNamedPipeW(_szPipeName, _uOpenMode, _uPipeMode,
        _uMaxInstances, _uOutBufferSize, _uInBufferSize, _uDefaultTimeout, _pSecurityAttributes);

    // 登记 GetNamedPipeServerProcessId所需的 ServerProcessId
    if (_hNamedPipe != INVALID_HANDLE_VALUE && YY::Thunks::internal::GetSystemVersion() < __WindowsNT6)
    {
        const auto _ulLastError = GetLastError();

        RegisterPipeServerProcessId(_hNamedPipe);

        SetLastError(_ulLastError);
    }

    return _hNamedPipe;
}
#endif


#if (YY_Thunks_Target < __WindowsNT6)

// 最低受支持的客户端	Windows NT 4.0 [桌面应用 | UWP 应用]
// 最低受支持的服务器	Windows NT 4.0 [桌面应用 | UWP 应用]
__DEFINE_THUNK(
kernel32,
32,
HANDLE,
WINAPI,
CreateNamedPipeA,
    _In_ LPCSTR _szPipeName,
    _In_ DWORD _uOpenMode,
    _In_ DWORD _uPipeMode,
    _In_ DWORD _uMaxInstances,
    _In_ DWORD _uOutBufferSize,
    _In_ DWORD _uInBufferSize,
    _In_ DWORD _uDefaultTimeout,
    _In_opt_ LPSECURITY_ATTRIBUTES _pSecurityAttributes
    )
{
    const auto _pfnCreateNamedPipeA = try_get_CreateNamedPipeA();
    if (_pfnCreateNamedPipeA == nullptr)
    {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return INVALID_HANDLE_VALUE;
    }

    const auto _hNamedPipe = _pfnCreateNamedPipeA(_szPipeName, _uOpenMode, _uPipeMode,
        _uMaxInstances, _uOutBufferSize, _uInBufferSize, _uDefaultTimeout, _pSecurityAttributes);

    // 登记 GetNamedPipeServerProcessId所需的 ServerProcessId
    if (_hNamedPipe != INVALID_HANDLE_VALUE && YY::Thunks::internal::GetSystemVersion() < __WindowsNT6)
    {
        const auto _ulLastError = GetLastError();

        RegisterPipeServerProcessId(_hNamedPipe);

        SetLastError(_ulLastError);
    }

    return _hNamedPipe;
}
#endif
} // namespace YY::Thunks

