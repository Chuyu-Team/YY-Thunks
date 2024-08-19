﻿# YY-Thunks - 让兼容 Windows 更轻松
[![license](https://img.shields.io/github/license/Chuyu-Team/YY-Thunks)](https://github.com/Chuyu-Team/YY-Thunks/blob/master/LICENSE)
[![downloads](https://img.shields.io/github/downloads/Chuyu-Team/YY-Thunks/total)](https://github.com/Chuyu-Team/YY-Thunks/releases)
[![contributors](https://img.shields.io/github/contributors-anon/Chuyu-Team/YY-Thunks)](https://github.com/Chuyu-Team/YY-Thunks/graphs/contributors)
[![release](https://img.shields.io/github/v/release/Chuyu-Team/YY-Thunks?include_prereleases)](https://github.com/Chuyu-Team/YY-Thunks/releases)
[![nuget](https://img.shields.io/nuget/vpre/YY-Thunks)](https://www.nuget.org/packages/YY-Thunks)
[![Build&Test](https://github.com/Chuyu-Team/YY-Thunks/actions/workflows/Build&Test.yml/badge.svg)](https://github.com/Chuyu-Team/YY-Thunks/actions/workflows/Build&Test.yml)

- [英语](Readme.md)

## 1. 关于 YY-Thunks

众所周知，从 Windows 的每次更新又会新增大量 API，这使得兼容不同版本的 Windows 
需要花费很大精力。导致现在大量开源项目已经不再兼容一些早期的 Windows 版本，比如
Windows XP RTM。

难道就没有一种快速高效的方案解决无法定位程序输入点的问题吗？

YY-Thunks（鸭船），存在的目的就是抹平不同系统的差异，编译时单纯添加一个 obj 
即可自动解决这些兼容性问题。让你兼容旧版本 Windows 更轻松！

[ [鸭船交流群 633710173](https://shang.qq.com/wpa/qunwpa?idkey=21d51d8ad1d77b99ea9544b399e080ec347ca6a1bc04267fb59cebf22644a42a) ]

### 1.1. 原理

使用 `LoadLibrary` 以及 `GetProcAddress` 动态加载 API，不存在时做出补偿措施，
最大限度模拟原始 API 行为，让你的程序正常运行。大概就像下面这个`GetTickCount64`函数：

```cpp
// 注意：Windows XP 不支持 GetTickCount64
ULONGLONG WINAPI GetTickCount64(VOID)
{
    if (auto const _pfnGetTickCount64 = try_get_GetTickCount64())
    {
        return _pfnGetTickCount64();
    }
    // Fallback
    return GetTickCount();
}
```
### 1.2. 亮点

* 更快！更安全！`鸭船`内建2级缓存以及按需加载机制，同时自动加密所有函数指针，
  防止内存爆破攻击。最大程度减少不需要和不必要的 `LoadLibrary` 以及 
  `GetProcAddress` 调用以及潜在安全风险。
* 轻松兼容 Windows XP，让你安心专注于业务逻辑。
* 完全开源且广泛接受用户意见，希望大家能踊跃的创建 PR，为`鸭船`添砖加瓦。

## 2. 使用方法

大家可以在以下方案中任选一种，但是我们优先推荐 NuGet 方案，因为NuGet采用傻瓜式设计，使用更便捷。

### 2.1. NuGet（推荐）
#### 2.1.1. C++项目
1. 项目右键 “管理 NuGet 程序包”。
2. NuGet搜索框中输入：`YY-Thunks`，搜索后点击安装。
3. 项目右键 - 属性 - YY-Thunks 中，自行调整YY-Thunks等级，允许 Windows 2000, 
   Windows XP 以及 Windows Vista（默认）。
4. 重新编译代码

#### 2.1.2. .NET Native AOT项目
1. 给`TargetFramework`添加`Windows`系统平台，比如修改为`net8.0-windows`或者`net9.0-windows`。
2. 项目右键 `管理 NuGet 程序包`。
3. NuGet搜索框中输入：`YY-Thunks`，搜索后点击安装。
4. 默认兼容到Windows Vista（默认）,如果需要兼容Windows XP可以将`WindowsSupportedOSPlatformVersion`调整为`5.1`。

### 2.2. 手工配置
#### 2.2.1. obj方式（适合微软的链接器）
1. 下载 [YY-Thunks-Objs](https://github.com/Chuyu-Team/YY-Thunks/releases)，
   然后解压到你的工程目录。
2. 【链接器】-【输入】-【附加依赖项】，添加 
   `objs\$(PlatformShortName)\YY_Thunks_for_WinXP.obj`。
3. 【链接器】-【系统】-【所需的最低版本】，根据实际情况填写 `5.01`（WinXP 32位） 或者 `5.02`（WinXP x64、2003）。
4. 如果是编译DLL，那么【链接器】-【高级】-【自定义入口点】更改为`DllMainCRTStartupForYY_Thunks`（不这样做XP下使用thread_local可能崩溃！）
5. 重新编译代码。

> 温馨提示：如果需要兼容 Vista，【所需的最低版本】无需修改，但是【附加依赖项】请选择 
  `objs\$(PlatformShortName)\YY_Thunks_for_Vista.obj`。

6. 如果您使用的是 **CMake**+MSVC 构建方式，需要添加以下代码，并分别设置 `YY_THUNKS_DIR` `YY_THUNKS_WIN_VERSION` `YY_THUNKS_WIN_VER_STR` 三个变量, 以指定路径和目标系统版本，请酌情修改：

    ```cmake
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE) 	   # /MTd in Debug, /MT in Release
    # set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:DebugDLL>" CACHE STRING "" FORCE) # /MDd in Debug, /MT in Release
    # set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE STRING "" FORCE) # /MDd in Debug, /MD in Release (默认)
    
    
    
    #* yy-thunks (MSVC only)
    set(YY_THUNKS_DIR "D:/3rdlibs/YY-Thunks")	#* yy-thunks (obj) 包 根目录
    set(YY_THUNKS_WIN_VERSION "WinXP")	#* 用于匹配文件名, 可用值: `WinXP` `Vista` `Win7` `Win8` `Win10.0.10240` `Win10.0.19041`
    set(YY_THUNKS_WIN_VER_STR "5.1")	#* Windows 子系统版本, 5.1:XP, 5.2:2003, 6.0:Vista, 6.1:Win7, 6.2:Win8, 6.3:Win8.1, 10.0:Win10/11, 留空则默认
    set(YY_THUNKS_ARCH	"x64")
    if(CMAKE_SIZEOF_VOID_P EQUAL 4) # 32 位
    	set(YY_THUNKS_ARCH "x86")
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC") #* MSVC, 抢在系统库 lib 前挂入 yy-thunks 的 obj 文件
    	set(YY_THUNKS_OBJ_NAME "${YY_THUNKS_DIR}/objs/${YY_THUNKS_ARCH}/YY_Thunks_for_${YY_THUNKS_WIN_VERSION}.obj") # 查找 obj 文件
    	set(CMAKE_CXX_STANDARD_LIBRARIES "\"${YY_THUNKS_OBJ_NAME}\" ${CMAKE_CXX_STANDARD_LIBRARIES}")
    	set(CMAKE_C_STANDARD_LIBRARIES   "\"${YY_THUNKS_OBJ_NAME}\" ${CMAKE_C_STANDARD_LIBRARIES}")
    	if(YY_THUNKS_WIN_VER_STR)
    		add_link_options("-SUBSYSTEM:$<IF:$<BOOL:${CMAKE_WIN32_EXECUTABLE}>,WINDOWS,CONSOLE>,${YY_THUNKS_WIN_VER_STR}")
    	endif()
    else()
    	message(WARNING "yy-thunks obj files only support MSVC linker, this operation will be ignored...")
    endif()
    ```

    **如需运行在 Windows XP 中, 需要在每个 DLL 项目中额外增加:** (将 `{PROJECT_NAME}` 更改为自己的 DLL target 名称)

    ```cmake
    target_link_options(${PROJECT_NAME} PRIVATE "/ENTRY:DllMainCRTStartupForYY_Thunks")
    ```

    

#### 2.2.2. lib方式（适合LLD-Link链接器）

> LLD-Link链接器无法使用obj方式，因为遇到重复符号会报告错误。

1. 下载 [YY-Thunks-Lib](https://github.com/Chuyu-Team/YY-Thunks/releases)，
   然后解压到你的工程目录。
2. 将 `-LIBPATH:YY-Thunks根目录/Lib/5.1.2600.0/x86` 类似的参数添加到链接器参数中，并确保顺序比系统SDK靠前。
3. 链接器额外传递 `-SUBSYSTEM:WINDOWS",5.1"` 或者 `-SUBSYSTEM:CONSOLE",5.1"` 或者 `-SUBSYSTEM:WINDOWS",5.2"` 或者 `-SUBSYSTEM:CONSOLE",5.2"`。
4. 如果是编译DLL，额外给链接器传递 `-ENTRY:DllMainCRTStartupForYY_Thunks`，修改DLL入口点（不这样做XP下使用thread_local可能崩溃！）
5. 重新编译代码。

> 温馨提示：如果您看不懂链接器的配置指引那么请使用NuGet版！NuGet对新手友好。


## 3. 兼容性

### 3.1. 支持的编译器

全平台ABI兼容。

* 所有Visual Studio版本均支持
  （比如：VC6.0、VS2008、VS2010、VS2015、VS2017、VS2019、VS2022等等）。
* 所有运行库模式均支持（比如：`/MD`、`/MT`、`/MDd`、`/MTd`）。

### 3.2. SDK版本要求

| Thunks等级         | 最低SDK要求
| ------------------ | -----------
| Windows 2000       | SDK 6.0（VS2008默认附带）
| Windows XP(2003)   | SDK 6.0（VS2008默认附带）
| Windows Vista      | SDK 6.0（VS2008默认附带）
| Windows 7          | SDK 7.0
| Windows 8          | SDK 8.0
| Windows 8.1        | SDK 8.1
| Windows 10 10240   | SDK 10.0.10240
| Windows 10 19041   | SDK 10.0.19041

> 温馨提示：VC6.0、VS2005用户请注意，由于这些编译器默认附带的SDK版本太低。请先将SDK升级到6.0或者更高版本，然后再使用YY-Thunks，否则将发生链接失败！
高版本的SDK不影响对老系统的兼容性，请坐和放宽，安心升级。

### 3.3. Thunks 清单

请参阅 [ThunksList.md](ThunksList.md)

## 4. 更新日志

请参阅 [releases 更新日志](https://github.com/Chuyu-Team/YY-Thunks/releases)
