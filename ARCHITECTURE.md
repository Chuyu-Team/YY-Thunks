# YY-Thunks 架构总览

> 置信度说明：🔍 自动探测 | 🟡 规划中 | ✅ 已确认
> 最后更新：2026-09-03 17:17:28 +08:00

## 1. 项目概述

- 项目简介：为 Windows 应用提供缺失系统 API 的动态加载与兼容性 Fallback，帮助应用运行在较旧版本的 Windows 上。
- 核心能力：通过动态加载 API；当目标系统不存在 API 时提供兼容实现或安全的占位行为。
- 目标平台与兼容性：Windows 原生应用；工程配置覆盖 x86、x64，并以可配置的 Windows 最低版本作为兼容性基线。🔍 自动探测

## 2. 技术栈与工具链

| 类别 | 选型 | 版本 | 说明 | 置信度 |
| --- | --- | --- | --- | --- |
| 语言 | C++ | C++17（单元测试工程当前配置） | Thunks 代码需要兼容较早的编译器与运行环境 | 🔍 |
| 构建系统 | MSBuild / Visual Studio | Visual Studio 解决方案格式 12.00 | 解决方案位于 `src/YY_Thunks.sln` | 🔍 |
| 目标架构 | x86、x64 | Debug/Release | 解决方案配置为 `Debug|x86`、`Debug|x64`、`Release|x86`、`Release|x64` | 🔍 |
| 测试工程 | `YY-Thunks.UnitTest` | 原生 C++ 单元测试 | 位于 `src/YY-Thunks.UnitTest` | 🔍 |
| 辅助工具 | `MinimumRequiredVersionHelper` | 原生 C++ 工程 | 用于最低系统版本相关处理 | 🔍 |
| 分析工具 | `YY.Depends.Analyzer` | 原生 C++ 工程 | 用于依赖/API 信息分析 | 🔍 |
| 主要依赖 | Windows SDK / Visual C++ 工具链 | 由工程环境提供 | 头文件与系统库由 Visual Studio/Windows SDK 提供 | 🔍 |

## 3. 系统架构

项目采用以 API 兼容层为核心的原生 C++ 组织方式。业务应用通过链接 YY-Thunks 的目标文件或 NuGet 集成获得兼容能力；兼容层优先尝试调用系统原生 API，不存在时进入对应 Fallback。

```mermaid
graph TD
    A[Windows 应用] --> B[YY-Thunks 兼容层]
    B --> C{系统 API 是否存在}
    C -->|存在| D[LoadLibrary / GetProcAddress 获取的原生 API]
    C -->|不存在| E[API Fallback 实现]
    B --> F[Thunks 清单与兼容性文档]
```

关键文字总结：YY-Thunks 在应用与 Windows API 之间提供动态解析和按 API 划分的兼容实现，测试工程和分析工具围绕该兼容层提供验证与辅助能力。

## 4. 模块设计

| 模块 | 路径 | 职责 | 置信度 |
| --- | --- | --- | --- |
| Thunks 兼容实现 | `src/Thunks` | 按系统 DLL/API 提供动态解析和 Fallback 实现；例如 `user32.hpp` | 🔍 |
| 共享基础设施 | `src/Shared` | 提供多个 Thunks 模块复用的基础设施 | 🔍 |
| 原生单元测试 | `src/YY-Thunks.UnitTest` | 验证兼容层行为和回归场景 | 🔍 |
| 最低版本辅助工具 | `src/MinimumRequiredVersionHelper` | 辅助处理最低系统版本相关信息 | 🔍 |
| 依赖分析器 | `src/YY.Depends.Analyzer` | 分析 Windows API 与依赖信息 | 🔍 |
| 构建/定义输入 | `src/def`、`src/Directory.Build.props`、`src/Build.cmd` | 提供 API 定义、共享构建属性和构建脚本 | 🔍 |
| 兼容性产物 | `src/objs` | 保存或生成面向不同目标平台的对象文件产物 | 🔍 |

模块间依赖关系：

```mermaid
graph LR
    UnitTest[YY-Thunks.UnitTest] --> Thunks[Thunks]
    Thunks --> Shared[Shared]
    Analyzer[YY.Depends.Analyzer] --> Definitions[def]
    Build[Build.cmd / Directory.Build.props] --> Thunks
    Build --> UnitTest
```

## 5. 非功能性约束

| 类别 | 约束 | 置信度 |
| --- | --- | --- |
| 兼容性 | Thunks 实现需要面向较旧 Windows 系统和较早 C++ 编译器保持可用；实现代码避免依赖现代标准库设施 | 🔍 |
| 平台 | 至少维护 x86 与 x64 构建配置 | 🔍 |
| 性能 | 原生 API 存在时优先直接转发，减少不必要的兼容层开销 | 🔍 |
| 可靠性 | 对不具备对应系统能力的旧系统，Fallback 应提供明确的成功、失败或占位语义 | 🔍 |
| 可追溯性 | 新增 Windows API Fallback 需要同步维护 `ThunksList.md` | 🔍 |

## 6. 关键设计决策（ADR 摘要）

| 编号 | 决策 | 原因 | 影响范围 | 置信度 |
| --- | --- | --- | --- | --- |
| ADR-001 | 优先动态解析原生 API，不存在时执行 Fallback | 避免旧系统因导入表缺少 API 而无法启动，同时在新系统保留原生行为 | `src/Thunks` 全部 API 兼容模块 | 🔍 |
| ADR-002 | 按 Windows DLL 和 API 划分 Thunks 实现 | 与系统 API 边界对应，便于维护、定位和生成兼容性产物 | `src/Thunks` | 🔍 |

## 7. 跨模块契约与公共接口

- API 解析契约：每个 Fallback 优先调用对应的 `try_get_<ApiName>()` 解析结果；解析成功时转发到系统 API，解析失败时执行模块定义的兼容行为。🔍
- 工程契约：`src/YY_Thunks.sln` 统一组织单元测试、最低版本辅助工具和依赖分析器工程。🔍
- 清单契约：`ThunksList.md` 记录可用的 API Fallback 及其行为摘要。🔍

## 8. 已知技术债务与限制

| 编号 | 问题 | 影响 | 计划 | 置信度 |
| --- | --- | --- | --- | --- |
| TD-001 | 部分旧系统 Fallback 只能提供占位成功或固定失败语义，无法模拟不存在的硬件/系统能力 | 调用方只能获得兼容返回值，不能获得真实系统能力 | 在 API 级文档和测试中明确行为边界 | 🔍 |
| TD-002 | 完整构建依赖正确配置的 Windows SDK/WDK 头文件环境 | 缺少系统头文件时无法构建测试工程 | 在开发环境中固定并验证所需 SDK/WDK 组件 | 🔍 |
