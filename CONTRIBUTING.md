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
- 默认流程：先运行新增单元测试，通过后再扩展到其他受影响测试。
- 测试覆盖：新增 fallback 需要同时覆盖可链接性/导出存在性与核心行为路径。
- **强制 fallback 守卫**：单测工程定义 `__YY_Thunks_Unit_Test`，为每个 thunk 生成 `bool YY::Thunks::aways_null_try_get_<ApiName>`。测试类声明成员 `AwaysNullGuard Guard;` 并在构造函数中 `Guard |= YY::Thunks::aways_null_try_get_<ApiName>;`，即可让 `try_get_<ApiName>()` 恒返回 `nullptr`，从而在现代 Windows 上真实执行 fallback 分支。
  - 新增 thunk 必须使用该守卫编写 fallback 测试，不得依赖“当前系统版本恰好低于门槛”这种环境条件。
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

## 7. 其他约定
- Thunks 核心实现为兼容 VC6-2026，避免在该层使用 `std::vector`、`strcpy_s` 等受限设施，优先复用项目内部设施。
- 单元测试与辅助代码不受上述标准库限制，可按测试便利性使用 STL。
- 当行为无法完全兼容时，优先保持可预测失败语义，并在设计/实现文档中说明差异。
- NuGet 配置型能力若同时覆盖 native 与 .NET，两侧应优先共用同一个 Property 名，并保持与各自现有 targets 风格一致。
- **允许通过 `__declspec(dllimport)` 声明直接调用系统 API**：YY-Thunks 自身就是**链接时 Hook**——`YY_Thunks.cpp` 作为唯一 TU，通过 `__DEFINE_THUNK` / `_LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME` 在链接期为缺失的 IAT 符号提供本地实现。因此 thunk 内部直接写 `::XxxApi(...)`（编译器按 SDK 声明生成 `__declspec(dllimport)` 引用）是安全的：只要该 API 在 YY-Thunks 中有对应 thunk，链接器就会把引用解析到 YY-Thunks 自己的实现，而**不会**把导入写入静态导入表，也就不会在缺少该导出的旧系统上加载失败。现有代码中的 `bcrypt.hpp`、`api-ms-win-core-winrt-string.hpp` 等大量使用该写法。
  - 边界一：目标 API 若**完全未被 YY-Thunks 覆盖**（既无 thunk，也无同名的本地实现可见），则必须用 `try_get_XxxApi()` 动态解析后调用，否则链接期没有可解析的符号。
  - 边界二：目标 API **有 thunk 但当前 `YY_Thunks_Target` 配置下该 thunk 不参与编译**（调用方守卫区间未被被调用方守卫区间包含），链接期同样没有可解析符号，`dllimport` 会退化为真实静态导入。因此仍必须遵守 §3「守卫包含关系」：调用方守卫区间要被被调用方守卫区间包含，或改用守卫更宽的等价 API。
  - 同文件内可用未限定名调用前面已定义的 thunk（如 `api-ms-win-core-sysinfo.hpp` 的 GLPIEx fallback 调用 GLPI）；反之若目标 thunk 定义在本文件之后，需在文件前部声明后再调用。
