# 编码规范 - YY-Thunks

 > 最后更新：2026-09-16 11:18:46 +08:00

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
- 新增 API Set 头文件后，需要在测试工程 `YY-Thunks.UnitTest.vcxproj` 与其 `filters` 中登记，`YY_Thunks_List.hpp` 由构建系统自动重新生成，无需手工修改。

## 5. 测试规范
- 测试框架：MSTest Native Unit Test。
- 默认流程：先运行新增单元测试，通过后再扩展到其他受影响测试。
- 测试覆盖：新增 fallback 需要同时覆盖可链接性/导出存在性与核心行为路径。
- 若 fallback 仅为假装成功且无额外行为分支，可在需求已明确接受的前提下不补单元测试，但需要在需求/交付说明中明确原因。
- NCrypt 相关接口：尽量按“每个 API 单独一个 `TEST_CLASS`”拆分，便于定位失败接口。
- 单元测试代码可使用标准库；Thunks 核心实现需优先兼容更宽的 VC / Windows 版本范围。
- 对 NuGet 属性页、MSBuild targets、链接参数注入这类配置型功能，若缺少稳定自动化验证入口，可按特性文档明确声明“以人工测试为主”，但测试计划必须给出可执行的人工验证矩阵与观察点。

## 6. Code Review 准则
- 阻塞级问题：破坏旧系统兼容性、导出签名不匹配、错误码/行为明显偏离系统语义、遗漏新增测试、引入不兼容的现代库设施到 Thunks 核心层。
- 建议级问题：why 注释不足、边界条件解释不清、测试命名或场景表达不够直接。
- 安全检查点：参数校验、COM 指针输出初始化、路径/模块加载安全、资源释放、整数与缓冲区边界、静态分析可接受性。

## 7. 其他约定
- Thunks 核心实现为兼容 VC6-2026，避免在该层使用 `std::vector`、`strcpy_s` 等受限设施，优先复用项目内部设施。
- 单元测试与辅助代码不受上述标准库限制，可按测试便利性使用 STL。
- 当行为无法完全兼容时，优先保持可预测失败语义，并在设计/实现文档中说明差异。
- NuGet 配置型能力若同时覆盖 native 与 .NET，两侧应优先共用同一个 Property 名，并保持与各自现有 targets 风格一致。
