# YY-Thunks - 让兼容 Windows 更轻松
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

#### 2.1. Vistual Studio C++项目如何使用？
1. 项目右键 “管理 NuGet 程序包”。NuGet搜索框中输入：`YY-Thunks`，搜索后点击安装。
2. 如果需要支持XP，请项目右键 - 属性 - YY-Thunks - 最小兼容系统版本，设置为5.1.2600.0。
3. 重新编译代码

#### 2.2. Vistual Studio .NET Native AOT项目如何使用？
1. 给`TargetFramework`添加`Windows`系统平台，比如修改为`net8.0-windows`或者`net9.0-windows`。
2. 项目右键 `管理 NuGet 程序包`。NuGet搜索框中输入：`YY-Thunks`，搜索后点击安装。
3. 如果需要支持XP，请将项目属性`WindowsSupportedOSPlatformVersion`调整为`5.1`。大致如下：
    ```xml
    <Project Sdk="Microsoft.NET.Sdk">
        <PropertyGroup>
            <!-- ... -->
            <TargetFramework>net8.0-windows</TargetFramework>
            <SupportedOSPlatformVersion>5.1</SupportedOSPlatformVersion>
            <!-- ... -->
        </PropertyGroup>
      <!--...-->
    </Project>
    ```
4. 重新编译代码

### 2.3. CMake项目如何使用？
1. 在代码根目录创建`Directory.Build.props`，内容如下：
    ```xml
    <?xml version="1.0" encoding="utf-8"?>
    <Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
      <ItemGroup Condition="'$(MSBuildProjectExtension)' == '.vcxproj'">
        <ProjectCapability Include="PackageReferences" />
      </ItemGroup>
      <PropertyGroup Condition="'$(MSBuildProjectExtension)' == '.vcxproj'">
        <NuGetTargetMoniker Condition="'$(NuGetTargetMoniker)' == ''">native,Version=v0.0</NuGetTargetMoniker>
        <RuntimeIdentifiers Condition="'$(RuntimeIdentifiers)' == ''">win;win-x86;win-x64;win-arm;win-arm64</RuntimeIdentifiers>

        <!--将项目最小支持到Windows XP，可根据自己的情况设置-->
        <WindowsTargetPlatformMinVersion>5.1.2600</WindowsTargetPlatformMinVersion>
      </PropertyGroup>
      <ItemGroup Condition="'$(MSBuildProjectExtension)' == '.vcxproj'">
        <PackageReference Include="YY-Thunks">
          <!--根据自己的情况选择YY-Thunks版本-->
          <Version>1.1.7-Beta3</Version>
        </PackageReference>      
      </ItemGroup>
      <!--从兼容性考虑，继续向上搜索 Directory.Build.props-->
      <PropertyGroup>
        <DirectoryBuildPropsPath>$([MSBuild]::GetPathOfFileAbove('Directory.Build.props', '$(MSBuildThisFileDirectory)../'))</DirectoryBuildPropsPath>
      </PropertyGroup>
      <Import Project="$(DirectoryBuildPropsPath)" Condition="'$(DirectoryBuildPropsPath)' != ''"/>
    </Project>
    ```
2. 将VS作为Gen启动编译过程，代码如下：
    ```
    # Gen必须选择Visual Studio系列，因为Visual Studio才支持nuget。
    # 假设输出目录为build\x86-Release，自己可根据情况修改。
    cmake -G "Visual Studio 17 2022" -A Win32 -DCMAKE_CONFIGURATION_TYPES:STRING=Release -DCMAKE_INSTALL_PREFIX:PATH=.\build\x86-Release .

    # 注意尾部的 `-- -r`，该命令是还原nuget包，此时会自动下载YY-Thunks，并且配置到工程。
    cmake --build .\build\x86-Release --config Release -- -r

    cmake --install .\build\x86-Release --config Release
    ```
3. 重新编译代码

### 2.4. 我不喜欢NuGet，如何纯手工配置链接器参数？
#### 2.4.1. obj方式（适合微软的链接器）
1. 下载 [YY-Thunks-Objs](https://github.com/Chuyu-Team/YY-Thunks/releases)，
   然后解压到你的工程目录。
2. 【链接器】-【输入】-【附加依赖项】，添加 
   `objs\$(PlatformShortName)\YY_Thunks_for_WinXP.obj`。
3. 【链接器】-【系统】-【所需的最低版本】，根据实际情况填写 `5.01`（WinXP 32位） 或者 `5.02`（WinXP x64、2003）。
4. 如果是编译DLL，那么【链接器】-【高级】-【自定义入口点】更改为`DllMainCRTStartupForYY_Thunks`（不这样做XP下使用thread_local可能崩溃！）
5. 重新编译代码。

> 温馨提示：如果需要兼容 Vista，【所需的最低版本】无需修改，但是【附加依赖项】请选择 
  `objs\$(PlatformShortName)\YY_Thunks_for_Vista.obj`。

#### 2.4.2. lib方式（适合LLD-Link链接器）
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

## 4. 其他
### 4.1 更新日志
请参阅 [releases 更新日志](https://github.com/Chuyu-Team/YY-Thunks/releases)

### 4.2. 第三方依赖
* [WinDepends](https://github.com/hfiref0x/WinDepends)
   - YY.Depends.Analyzer中使用上述部分代码扫描Windows API Set。
