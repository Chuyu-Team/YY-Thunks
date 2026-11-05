# 编码规范 - YY-Thunks

 > 最后更新：2026-11-04 18:16:00 +08:00

## 1. 命名规范
请参考仓库根目录的[C++编码风格约定.md](./C++编码风格约定.md)

## 2. 注释规范
- 公开接口与 thunk 声明：新增代码需遵循仓库 SDD 规则，补充完整函数契约注释；C/C++ 声明/定义按要求补充 SAL。
- 实现内部：对非直觉分支、兼容性 workaround、魔数来源写 why 注释，不写复读式注释。
- 兼容性说明：当 fallback 无法完全模拟系统行为时，优先写明限制或补充 `__WarningMessage__`。

## 3. 头文件与包含顺序
- 优先按模块需要包含最小头文件集合。
- WinRT / COM 相关实现通常先包含对应 SDK 头，再进入 thunk 宏定义。
- 项目内测试工程使用 `pch.h` 汇总系统头、测试框架头与公共定义。

## 4. 文件组织约定
- 核心 thunk 实现在 `src/Thunks`，按 API 集族拆分头文件。
- 若 API 已明确归属某个 API Set，则优先放入对应的 `api-ms-win-*.hpp` 文件，避免大量函数在一个文件中堆积。
- 共享定义放在 `src/Shared`。
- **构建入口**：必须构建解决方案 `src/YY_Thunks.sln`，不能只构建单个 `.vcxproj`。各工程通过 `IncludePath = $(SolutionDir)Thunks;$(SolutionDir);$(SolutionDir)Shared;...` 引入共享头文件，`$(SolutionDir)` 在单项目构建时不会指向 `src`，会因找不到 `km.h` / `SharedDefs.h` 而失败。开发期局部验证也应加 `-t:<ProjectName>` 或用解决方案级构建。
- 单元测试放在 `src/YY-Thunks.UnitTest`，新增 API fallback 后同步补测试。
- 新增 Windows API fallback 后同步更新仓库根目录 `ThunksList.md`。
- NuGet native 属性页配置位于 `NuGet/build/native/*.xml`，native/.NET 的配置注入逻辑分别位于各自 `YY-Thunks.targets`；涉及 NuGet 配置开关时，优先保持两个 targets 的语义一致。

## 4.1 Thunk 布局约定
- 每个 API thunk 逻辑单独使用对应的 `#if (YY_Thunks_Target < ...)` 包裹。
- 每个 thunk 函数与下一个 thunk 函数之间保持连续两个空行分隔。
- 新增 thunk 采用当前新风格：`namespace YY::Thunks` 内部条目不再额外做整体缩进；老文件保留原有缩进，不整体重排以免制造大范围 diff。

## 4.2 Thunk 参数命名约定
- 参数命名遵循项目既有前缀规范。
- 字符串参数优先使用 `_szXxx`；例如 `SetCurrentProcessExplicitAppUserModelID` 的 `AppID` 参数命名为 `_szAppID`。
- 指针 / 输出参数延续 `_pXxx`、`_ppXxx` 等项目既有命名方式。

## 4.3 API Set 归属约定
- 判定接口归属的 API Set 时以其真实所属 DLL 为准，例如 `SetCurrentProcessExplicitAppUserModelID` 属于 `api-ms-win-shcore-sysinfo-l1-1-0.dll`，实现须放入 `api-ms-win-shcore-sysinfo.hpp`。
- 不得依据宿主 DLL 名（如 `shell32`）反推 API Set 名，两者可能并不一致。
- **查询 API Set 归属的权威来源是本仓库的 `src/APISet.txt`**，其中按导出清单记录了每个 API Set 导出的符号（格式为 `<序号>.<API Set DLL 名> <符号名>`）。判定新增 API 归属时应优先检索该文件。
- 注意同一函数的 A/W 版本可能分属不同 API Set。例如 `CreateNamedPipeA` 属 `api-ms-win-core-kernel32-legacy-l1-1-0.dll`，而 `CreateNamedPipeW` 属 `api-ms-win-core-namedpipe-l1-1-0.dll`。
- **功能族聚合（优先于按 API Set 拆分）**：同一功能族的 API 优先聚合到族内最相关的 `api-ms-win-*.hpp` 单一文件，便于集中管理与查阅，即使这些 API 分属不同 API Set。例如管道族（`GetNamedPipeClientProcessId`、`GetNamedPipeServerProcessId`、`CreateNamedPipeA`、`CreateNamedPipeW`）统一放入 `api-ms-win-core-namedpipe.hpp`。
  - 聚合时 `__DEFINE_THUNK` 的模块名仍取该 API 的真实导入库（通常为 `kernel32`），不受文件名影响。
  - 仅当某 API 是跨族的通用入口时（如 `CreateFileW`、`CloseHandle`）才保留在其原属通用文件中，不迁入功能族文件。
  - 聚合文件需在文件头或以注释注明各 API 的真实 API Set 归属，避免后续维护误解。
- 新增 API Set 头文件后，需要在测试工程 `YY-Thunks.UnitTest.vcxproj` 与其 `filters` 中登记，`YY_Thunks_List.hpp` 由构建系统自动重新生成，无需手工修改。
- `YY_Thunks_List.hpp` 的生成顺序与 `vcxproj` 中 `ClInclude` 的登记顺序一致；存在跨头文件调用时，被调用方必须排在调用方之前，或在使用点之前的公共头文件（如 `YY_Thunks.h`）中声明。
- 头文件内非 thunk 的类型/常量定义（如共享数据结构与布局常量）须限定在 `YY_Thunks_Implemented` 块内并以防重入宏保护：`YY_Thunks.cpp` 会在同一翻译单元内两次包含清单头文件（声明遍与实现遍），条件过宽会触发 C2011/C2086 重复定义编译错误。
  - 若该类型/常量**仅服务于单个 thunk**，优先改为**函数内局部定义**（局部 struct / 局部 `const` 变量），天然规避两轮 include 重复编译，无需 `YY_Thunks_Implemented` 守卫，也不会污染命名空间作用域。只有当定义需要被多个 thunk 共享时，才使用 `YY_Thunks_Implemented` 块 + 防重入宏。
- 清单登记约定：仅"提供 Fallback 实现"的 API 登记到 `ThunksList.md`；仅为支撑其他 Thunk 而被拦截、但本机已存在的 API（如 `CloseHandle`、`DuplicateHandle`、`CreateFileW`）不单独登记条目。`ThunksList.md` 中的分组按导入库/宿主 DLL 名（如 `## kernel32.dll`），而非 API Set 名。
- `ThunksList.md` 描述写法约定：
  - 第一列函数名，第二列 `Fallback` 列直接写实现方式，如 `内部实现。`、`调用Xxx。`、`返回E_NOTIMPL。`、`报告ERROR_NOT_SUPPORTED。`
  - 行为与系统实现存在差异时，在实现方式后用 `注意：` 追加用户可见的已知限制摘要（如不输出的条目类型、顺序差异、字段可能为 0、规模假设），不要只写"内部实现"而隐藏差异。
  - 存在较大负面影响、不建议直接使用时，函数名前加 `*` 前缀（现有文件星号后有/无空格不统一，新增条目统一写 `| *ApiName`，星号后不加空格）。
  - 描述允许较长（现有最长约 188 字符），但只写调用者需要知道的信息，不复制设计文档细节。

## 5. 测试规范
- 测试框架：MSTest Native Unit Test。
- 测试文件位置：与被测 Thunks 头文件同名，放在 `src/YY-Thunks.UnitTest/<header>.UnitTest.cpp`，新用例追加到既有文件，不为单个 API 新建测试文件。
- 命名空间与测试类：文件内用 `namespace api_ms_win_core_<module>`（由文件名推导）包裹，每个 API 一个 `TEST_CLASS(<ApiName>)`。
- `TEST_METHOD` 命名：`<场景>_<预期结果>`，中文，与测试计划/测试用例名称逐字一致，便于失败时直接定位用例。
- 断言统一使用 MSTest 宏（`Assert::AreEqual`、`Assert::IsTrue`、`Assert::AreNotEqual` 等），不使用 `assert`，保证失败信息可读且 Release 下同样生效。
  - **Microsoft 原生 C++ MSTest 不提供 `Assert::Inconclusive`**（那是 .NET 侧 API）。需要表达「环境不满足，跳过本用例」时，用 `Logger::WriteMessage(L"..."); return;` 并配 `TEST_METHOD_CLEANUP` 复位状态。
  - `Assert::AreEqual` 对类型敏感，缺少对应重载时会报 `C2665`。比较不同宽度的整数（`ULONG` / `DWORD` / `DWORD_PTR` / `KAFFINITY`）时必须显式转型到同一类型，例如 `Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), DWORD(::GetLastError()))`。
- 默认流程：先运行新增单元测试，通过后再扩展到其他受影响测试。
- 测试覆盖：新增 fallback 需要同时覆盖可链接性/导出存在性与核心行为路径。
- **强制 fallback 守卫**：单测工程定义 `__YY_Thunks_Unit_Test`，为每个 thunk 生成 `bool YY::Thunks::aways_null_try_get_<ApiName>`。测试类声明成员 `AwaysNullGuard Guard;` 并在构造函数中 `Guard |= YY::Thunks::aways_null_try_get_<ApiName>;`，即可让 `try_get_<ApiName>()` 恒返回 `nullptr`，从而在现代 Windows 上真实执行 fallback 分支。
  - 新增 thunk 必须使用该守卫编写 fallback 测试，不得依赖“当前系统版本恰好低于门槛”这种环境条件。
  - **必须显式断言守卫生效**：在每个 `TEST_CLASS` 的 `TEST_METHOD_INITIALIZE` 中断言 `YY::Thunks::aways_null_try_get_<ApiName> == true`。若某用例遗漏 `Guard |= ...`，测试会**静默测到原生 API**：在 Win11 上原生行为可能与 fallback 期望恰好一致，用例假通过；只有换到旧系统才会失败。此断言把这种风险转为即时失败。
  - **`TEST_METHOD_CLEANUP` 恢复顺序**：若用例改过进程亲和性，必须**先** `SetProcessAffinityMask(系统掩码)` 恢复进程，**再**恢复线程亲和性。反序会导致 `SetThreadAffinityMask(系统掩码)` 因「线程掩码必须是进程掩码子集」而返回 `ERROR_INVALID_PARAMETER(87)` 并静默失败，污染后续用例。
  - 涉及全局状态（亲和性、CpuSet 选择）的用例，除 `TEST_METHOD_CLEANUP` 兜底外，还应在用例体末尾显式恢复到**进入时**的值，不依赖 cleanup 的执行时机。
  - 由此带来的硬约束：fallback 的入口条件只能是 `try_get_*` 解析失败，不得再用运行时版本号判断作为唯一开关，否则该分支永远无法被单测覆盖。
- 若 fallback 仅为假装成功且无额外行为分支，可在需求已明确接受的前提下不补单元测试，但需要在需求/交付说明中明确原因。
- NCrypt 相关接口：尽量按“每个 API 单独一个 `TEST_CLASS`”拆分，便于定位失败接口。
- 单元测试代码可使用标准库；Thunks 核心实现需优先兼容更宽的 VC / Windows 版本范围。
- 对 NuGet 属性页、MSBuild targets、链接参数注入这类配置型功能，若缺少稳定自动化验证入口，可按特性文档明确声明“以人工测试为主”，但测试计划必须给出可执行的人工验证矩阵与观察点。

## 6. Code Review 准则
- 阻塞级问题：破坏旧系统兼容性、导出签名不匹配、错误码/行为明显偏离系统语义、遗漏新增测试、引入不兼容的现代库设施到 Thunks 核心层。
- 建议级问题：why 注释不足、边界条件解释不清、测试命名或场景表达不够直接。
- 安全检查点：参数校验、COM 指针输出初始化、路径/模块加载安全、资源释放、整数与缓冲区边界、静态分析可接受性。
- **`LastError` 语义审查**：fallback 若需“成功路径保持调用前的 `LastError`、失败路径返回错误码”。
- **fallback 可用性审查（重要）**：验证 fallback 时，必须把「功能在目标旧系统上是否真的可用」与「是否引入静态导入」分开核对，不能只确认后者。
  - 凡 fallback 内出现 `GetProcAddress` 解析的目标若在本仓库存在同名 thunk，一律视为**重复造轮子 + 可能的功能缺陷**，必须要求作者说明理由。
  - 原因：`GetProcAddress` 只查 DLL 真实导出，看不到本项目的 fallback 实现；结果是「为了避免一个不存在的加载失败风险，反而让功能在旧系统上彻底不可用」。CpuSet thunk 曾因此让 `Get/SetThreadSelectedCpuSets` 在 XP~2008 恒返回 `ERROR_CALL_NOT_IMPLEMENTED`。
- **跨 thunk 调用的三条验证手段**（判断「直接 `::` 调用是否安全」不能靠读 `#if` 推演）：
  1. **链接期导入表比对**：同一份 `YY_Thunks.cpp` 在「低于该 API 引入版本」与「等于该版本」两档分别编译并链接探针 EXE，比对 `dumpbin /imports`。前者不应出现被 thunk 化的 API，后者应出现且必须能在目标系统真实导出表中找到（可查 `src/YY.Depends.Analyzer/Config/<arch>/<ver>.txt`）。这同时验证「不误绑」与「不漏绑」。
  2. **`dumpbin /symbols` 看 `__imp_*` 的 DEFINED/UNDEF**：`selectany` IAT 重定向生效时该符号在 obj 内 DEFINED；退化为真实导入时为 UNDEF。
  3. **反汇编关键路径**：确认 fallback 实际生成的指令，可发现编译器内建引入的指令集依赖（如 `__popcnt` → `popcnt`）。
- **原生语义探针要按参数矩阵穷举**：尤其要区分「句柄非法」与「Id 非法」的优先级，以及「缓冲区非法」与「句柄非法」的优先级——单点测试很容易把顺序判反。
- **不要为了「对齐错误码」而自造常量**：先确认该错误码是否已在 Windows SDK 中定义（正确的宏名可能与自己设想的不同，例如 `ERROR_CPU_SET_INVALID` 而非 `ERROR_INVALID_CPU_SET`）。本项目已把 `WindowsTargetPlatformVersion` 固定为 Windows 10 SDK，并在 thunk 代码中无条件使用 `SYSTEM_CPU_SET_INFORMATION` 等 Win10-only 类型，因此**优先直接使用 SDK 宏，不要加 `#ifndef` 兜底**。
- **不要假定「系统调用会替我校验参数」**：同一族系统 API 对同一种非法输入的错误码与副作用可能完全不同。实测 `SetProcessAffinityMask` 对越界位返回 `87`，而 `SetThreadGroupAffinity` 返回 `TRUE` 并静默放宽进程亲和性。凡是「委托系统调用校验」的设计，必须先按参数矩阵实测，不能靠 API 名称推断。
- **必须按原生实测顺序实现校验**：校验顺序本身是可观测行为。CpuSet 族的实测顺序是「`Flags` → Id → 句柄」，其中 `count == 0` 的清除路径例外（先解析句柄）。发现顺序不一致时，应以实测为准，不能以「反正都失败」为由放过。
- **不要凭 API 名称推断「跨进程能不能用」**：同一族 API 的跨进程能力差异很大。CpuSet 族实测全部支持跨进程：`SetThreadSelectedCpuSets(crossThread, ...)` 返回 `TRUE`、`GetProcessAffinityMask(跨进程句柄)` 成功、`Get/SetThreadGroupAffinity` 也能跨进程。曾被误判为「需要 `GetProcessIdOfThread`（Vista SP1+）因此不支持」，实际 `GetProcessIdOfThread` 在本项目有 thunk，且取目标进程掩码只需 `OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION)`。**判断跨进程可行性必须实测，不能看 API 版本号推断。**

## 7. 其他约定
- Thunks 核心实现为兼容 VC6-2026，避免在该层使用 `std::vector`、`strcpy_s` 等受限设施，优先复用项目内部设施。
- 单元测试与辅助代码不受上述标准库限制，可按测试便利性使用 STL。
  - **但测试文件的 STL 头必须在 `#include "pch.h"` 之后的第一段统一引入，且不能在引入顺序中间插入项目头**。原因：`pch.h` 会把 `__DEFINE_THUNK` 重定义为「`extern bool aways_null_try_get_<Fn>` + `EXTERN_C` 声明 + `__if_not_exists`」，该宏对后续所有 `#include` 都生效；一旦 STL 头被排到项目 thunk 头之后，或 include 顺序被打乱，会触发 `__msvc_int128.hpp` / `__msvc_heap_algorithms.hpp` 一类的模板解析错误（表现为 `_Iter_diff_t` / `enable_if_t` 未声明）。当前工程还启用了 `/fsanitize=address`，对顺序更敏感。
  - `src/Thunks/Thunks` 是指向 `src/Thunks` 的自引用符号链接（既有环境遗留）。它会让 `#include "Thunks/xxx.hpp"` 与 `#include "../Thunks/xxx.hpp"` 指向同一物理文件的两个不同路径，破坏 `__if_not_exists` 与 include guard 的一致性。新增测试文件时优先沿用现有文件的 include 写法，不要另创路径风格。
- 当行为无法完全兼容时，优先保持可预测失败语义，并在设计/实现文档中说明差异。
- **`ThunksList.md` 条目约定**：
  - 单行长度控制在 188 字符以内（含 Markdown 表格标记），超出时精简为「实现手段 + 关键限制 + 最需警示的差异」，细节留给 Specs。
  - 若 fallback 存在会显著改变调用方行为、且调用方难以自行规避的负面影响，函数名**必须加 `*` 前缀**（文件头已声明 `*` 的含义为"不建议使用"）。典型场景：`*` 语义错位（如用亲和性模拟 CpuSet 这类正交维度）、全局副作用（如 `*CancelIoEx` 取消句柄全部 IO）、无法真正生效。
  - 若存在跨进程/跨实例不一致的标识符（如本地编码的 Id），必须在条目中显式标注「跨进程不一致」。
  - 若 fallback 的错误码与原生不同（如原生 `813` vs fallback `87`），应在条目或 Specs 中说明，不得省略。
- NuGet 配置型能力若同时覆盖 native 与 .NET，两侧应优先共用同一个 Property 名，并保持与各自现有 targets 风格一致。
- **允许通过 `__declspec(dllimport)` 声明直接调用系统 API**：YY-Thunks 自身就是**链接时 Hook**——`YY_Thunks.cpp` 作为唯一 TU，通过 `__DEFINE_THUNK` / `_LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME` 在链接期为缺失的 IAT 符号提供本地实现。因此 thunk 内部直接写 `::XxxApi(...)`（编译器按 SDK 声明生成 `__declspec(dllimport)` 引用）是安全的：只要该 API 在 YY-Thunks 中有对应 thunk，链接器就会把引用解析到 YY-Thunks 自己的实现，而**不会**把导入写入静态导入表，也就不会在缺少该导出的旧系统上加载失败。现有代码中的 `bcrypt.hpp`、`api-ms-win-core-winrt-string.hpp` 等大量使用该写法。
  - 边界一：目标 API 若**完全未被 YY-Thunks 覆盖**（既无 thunk，也无同名的本地实现可见），则必须用 `try_get_XxxApi()` 动态解析后调用，否则链接期没有可解析的符号。
  - 边界二（**已修正**）：`__DEFINE_THUNK` 的声明与实现共用同一 `#if` 守卫，因此**不存在**「thunk 不参与编译且系统也没有该导出」的真空区间——`Target >= 该 API 引入版本` 时，最低目标系统已保证该 API 存在，`::ApiName()` 是有效的静态导入。所以只要目标 API 有 thunk，直接 `::ApiName()` 一律安全，**不需要**判断守卫包含关系。
  - **禁止**用 `GetProcAddress` 自行解析已有 thunk 的 API：`GetProcAddress` 只能看到 DLL 的真实导出，看不到 YY-Thunks 的 fallback 实现，在 `Target < 该 API 引入版本` 的旧系统上会返回 `NULL`，把「本项目已提供 fallback」误判为「系统不支持」。该误用曾导致 CpuSet thunk 在 XP~2008 上失效（详见 `ARCHITECTURE.md` ADR-005）。
  - 同文件内可用未限定名调用前面已定义的 thunk（如 `api-ms-win-core-sysinfo.hpp` 的 GLPIEx fallback 调用 GLPI）；反之若目标 thunk 定义在本文件之后，需在文件前部声明后再调用。
