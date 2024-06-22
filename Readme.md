# YY-Thunks - It's easier to make your apps compatible with older versions of Windows!
[![license](https://img.shields.io/github/license/Chuyu-Team/YY-Thunks)](https://github.com/Chuyu-Team/YY-Thunks/blob/master/LICENSE)
[![downloads](https://img.shields.io/github/downloads/Chuyu-Team/YY-Thunks/total)](https://github.com/Chuyu-Team/YY-Thunks/releases)
[![contributors](https://img.shields.io/github/contributors-anon/Chuyu-Team/YY-Thunks)](https://github.com/Chuyu-Team/YY-Thunks/graphs/contributors)
[![release](https://img.shields.io/github/v/release/Chuyu-Team/YY-Thunks?include_prereleases)](https://github.com/Chuyu-Team/YY-Thunks/releases)
[![nuget](https://img.shields.io/nuget/vpre/YY-Thunks)](https://www.nuget.org/packages/YY-Thunks)
[![Build&Test](https://github.com/Chuyu-Team/YY-Thunks/actions/workflows/Build&Test.yml/badge.svg)](https://github.com/Chuyu-Team/YY-Thunks/actions/workflows/Build&Test.yml)

- [简体中文](Readme.osc.md)

## 1. About YY-Thunks

With each new version of Windows, a large number of APIs are added,
and it often takes a lot of effort to develop Windows applications
that are compatible with older systems. And some open source projects
are no longer compatible with some earlier versions of Windows, such as Windows XP, Windows 7...

Isn't there a solution to quickly deal with the problem that the old system can't find the API?

YY-Thunks can solve these kinds of problems quickly and without changing the code.
These compatibility issues can be resolved automatically by tweaking the linker.
It's easier to make your apps compatible with older versions of Windows!

[ [YY-Thunks QQ Group 633710173](https://shang.qq.com/wpa/qunwpa?idkey=21d51d8ad1d77b99ea9544b399e080ec347ca6a1bc04267fb59cebf22644a42a) ]

### 1.1. The principle of YY-Thunks

Use `LoadLibrary` and `GetProcAddress` to dynamically load the API.
If the API doesn't exist, then implement its behavior and get your program running properly.

As follows: GetTickCount64

```cpp
// Windows XP not support GetTickCount64.
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

### 1.2. Highlight

* Faster! Safer! Built-in L2 cache and encrypt all function pointers to prevent brute-force memory searches.
    Minimize unwanted, unnecessary calls to `LoadLibrary` and `GetProcAddress`.
* Make your application easily compatible with Windows XP so you can focus on your business logic.
* The code is completely open source, and everyone is welcome to create PRs to contribute to the YY-Thunks.

## 2. How to used?

You can choose one of the following options, but it is recommended to use NuGet first,
as NuGet is designed to be foolproof and easier to use.

### 2.1. NuGet (Recommend)
#### 2.1.1. C++ Project
1. Right-click on the `Project` and select `Manage NuGet Packages`, then search for `YY-Thunks` and choose the version that suits you, and finally click Install.
2. In Project Right-Click - `Properties` - `YY-Thunks`, adjust the YY-Thunks level,
    which can be set to: Windows 2000, Windows XP, and Windows Vista (default).
3. Rebuild the solution.

#### 2.1.2. .NET Native AOT Project
1. modify the value of `TargetFramework` to `net8.0-windows` or `net9.0-windows`。
2. Right-click on the `Project` and select `Manage NuGet Packages`, then search for `YY-Thunks` and choose the version that suits you, and finally click Install.
3. Optional, if you need run on Windows XP, please modify the value of `SupportedOSPlatformVersion` to `5.1`

### 2.2. Manual
#### 2.2.1. using obj file (MSVC Link)
1. Download and unzip [YY-Thunks-Objs](https://github.com/Chuyu-Team/YY-Thunks/releases) to project directory.
2. `Linker` - `Input` - `Additional Dependencies`, add `objs\$(PlatformShortName)\YY_Thunks_for_WinXP.obj`.
3. `Linker` - `System` - `Minimum Required Version`, set to `5.1` (WinXP 32-bit) or `5.2` (WinXP x64 or Win2003).
4. If the project is a `Dynamic Link Library`, then change `Linker` - `Advanced` - `Custom Entry Point` to `DllMainCRTStartupForYY_Thunks`
    (If you ignore this step, the XP system may crash with `thread_local`).
5. Rebuild the solution.

> Note: If your app needs to be compatible with Vista or later, please set `Additional Dependencies` to 
  `objs\$(PlatformShortName)\YY_Thunks_for_Vista.obj`。

#### 2.2.2. using lib files (LLD-Link)
> LLD-Link linkers using obj files will encounter duplicate symbol errors.

1. Download and unzip [YY-Thunks-Lib](https://github.com/Chuyu-Team/YY-Thunks/releases) to project directory.
2. Add a parameter like `-LIBPATH:YY-Thunks_Root_Dir/lib/5.1.2600.0/x86` to the linker and make sure the order is higher than WinSDK.
3. Please add parameter `-SUBSYSTEM:WINDOWS",5.1"` or `-SUBSYSTEM:CONSOLE",5.1"` or `-SUBSYSTEM:WINDOWS",5.2"` or `-SUBSYSTEM:CONSOLE",5.2"` to linker.
4. If the project is a `Dynamic Link Library`, please add parameter `-ENTRY:DllMainCRTStartupForYY_Thunks` to linker
    (If you ignore this step, the XP system may crash with `thread_local`).
5. Rebuild the solution.

## 3. Compatibility

### 3.1. Supported Compiler

Compatible with all platforms.

* It is supported by all Visual Studio versions (such as: VC6.0, VS2008, VS2010, VS2015, VS2017, VS2019, VS2022...).
* All runtime modes are supported (such as: `/MD`, `/MT`, `/MDd`, `/MTd`).

### 3.2. Windows SDK Version Requirements
| Thunks Target      | At Least Windows SDK Version is required
| ------------------ | -----------
| Windows 2000       | SDK 6.0 (VS2008 built-in)
| Windows XP(2003)   | SDK 6.0 (VS2008 built-in)
| Windows Vista      | SDK 6.0 (VS2008 built-in)
| Windows 7          | SDK 7.0
| Windows 8          | SDK 8.0
| Windows 8.1        | SDK 8.1
| Windows 10 10240   | SDK 10.0.10240
| Windows 10 19041   | SDK 10.0.19041

At least Windows SDK 6.0 is required.

> Note: VC6.0 and VS2005 users should note that the SDK version that comes with these compilers by default is too low.
Please upgrade the SDK to version 6.0 or later, and then use YY-Thunks, otherwise the link will not be successful!
The different SDK version don't affect your app compatibility with the old system.

### 3.3. Thunks API List

See the [ThunksList.md](ThunksList.md)

## 4. Changelog

See the [releases Changelog](https://github.com/Chuyu-Team/YY-Thunks/releases)
