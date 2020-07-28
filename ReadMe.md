
# YY-Thunks——让兼容Windows更轻松


## 1. 关于YY-Thunks
众所周知Windows XP每个SP都会新增大量API，而Windows 10每次更新又会新增大量API，这使得兼容不同版本的Windows需要大量的判断。

甚至大量开源代码已经不再兼容一些早期的Windows XP版本，比如 Windows XP RTM。难道就没有一种快速高效的方案解决无法定位程序输入点的问题吗？

YY-Thunks（鸭船），存在的目的就是抹平不同系统的差异，编译时单纯添加一个obj即可自动解决这些兼容性问题。让你兼容Windows更轻松！


[ [鸭船交流群 633710173](https://shang.qq.com/wpa/qunwpa?idkey=21d51d8ad1d77b99ea9544b399e080ec347ca6a1bc04267fb59cebf22644a42a) ]


### 1.1. 原理
使用`LoadLibrary`以及`GetProcAddress`动态加载API，不存在时做出补偿措施，最大限度模拟原始API行为，让你的程序正常运行。

### 1.2. 亮点
* 更快！更安全！`鸭船`内建2级缓存以及按需加载机制，同时自动加密所有函数指针，防止内存爆破攻击。最大程度减少不需要的、不必要的`LoadLibrary以及GetProcAddress`调用以及潜在安全风险。
* 轻松兼容Windows XP，让你安心专注于业务逻辑。
* 完全的开放代码，广泛的接受用户意见，希望大家能踊跃的 pull requests，为`鸭船`添砖加瓦。

## 2. 使用YY-Thunks
大家可以在以下方案中任选一种，但是我们优先推荐NuGet方案。

### 2.1. 通过NuGet（推荐）
1. 项目右键 “管理 NuGet 程序包”。
2. NuGet搜索框中输入：`YY-Thunks`，搜索后点击安装。
3. 项目右键 - 属性 - YY-Thunks 中，自行调整YY-Thunks等级，允许Windows 2000、Windows XP以及Windows Vista（默认）。
4. 重新编译代码

### 2.2. 手工低效配置
1. 下载[YY-Thunks-Binary](https://github.com/Chuyu-Team/YY-Thunks/releases)，然后解压到你的工程目录。<br/>
2. 【链接器】-【输入】-【附加依赖项】，添加`objs\$(PlatformShortName)\YY_Thunks_for_WinXP.obj`。<br/>
3. 重新编译代码。

> 温馨提示：如果需要兼容Vista，请选择`objs\$(PlatformShortName)\YY_Thunks_for_Vista.obj`。

## 3. YY-Thunks兼容性
### 3.1. 支持的编译器
全平台ABI兼容。
* 所有Visual Studio版本均支持（比如：VC6.0、VS2008、VS2010、VS2015、VS2017、VS2019等等）。
* 所有运行库模式均支持（比如：`/MD`、`/MT`、`/MDd`、`/MTd`）。

### 3.2. Thunks清单

请参阅 [ThunksList.md](ThunksList.md)

## Changes

### 1.0.0.1 - 第一版（2018-05-17 19:00）
* YY-Thunks第一版


### 1.0.0.2 - 补充API支持（2018-06-01 18:30）
* 添加RegSetKeyValueW(A)
* 添加RegDeleteKeyValueW(A)
* 添加RegDeleteTreeW(A)


### 1.0.0.3 - 让兼容Windows ARM64更轻松（2018-06-18 11:30）
* 添加IsWow64Process2
* 添加IsWow64GuestMachineSupported
* 添加GetTickCount64
* 添加GetSystemTimePreciseAsFileTime
* 添加InitializeCriticalSectionEx
* 添加InitOnceExecuteOnce
* 添加GetCurrentProcessorNumber
* 添加GetCurrentProcessorNumberEx
* 添加GetNumaNodeProcessorMask
* 添加GetNumaNodeProcessorMaskEx
* 添加SetThreadGroupAffinity


### 1.0.0.4 - 兼容性更新（2018-09-08 18:00）
* 解决Bug，将初始化时机推迟到`.CRT$XID`，避免VC2008下过早初始化导致atexit崩溃（感谢 死田鸡）。


### 1.0.0.7 - 功能更新（2019-01-02 15:10）
* 添加RegGetValueW(A)
* 添加LocaleNameToLCID
* 添加LCIDToLocaleName
* 添加GetLocaleInfoEx
* 添加GetDateFormatEx
* 添加GetTimeFormatEx
* 添加GetNumberFormatEx
* 添加GetCurrencyFormatEx
* 添加GetUserDefaultLocaleName
* 添加GetSystemDefaultLocaleName
* 重新编译，解决潜在符号修饰问题。


### 1.0.1.1 - 功能更新（2019-04-01 17:00）
* 添加EnumCalendarInfoExEx
* 添加EnumDateFormatsExEx
* 添加GetFileInformationByHandleEx
* 添加SetFileInformationByHandle
* 添加GetFinalPathNameByHandleW(A)
* 解决Bug，1.0.0.10版意外引入ntdll.lib问题（感谢 小古）。
* 添加GetLogicalProcessorInformation
* 添加GetLogicalProcessorInformationEx
* 添加InetPtonW(inet_pton)
* 添加InetNtopW(inet_ntop)
* 添加GetNumaHighestNodeNumber


### 1.0.1.5 - 功能更新（2019-05-26 19:00）
* 添加RaiseFailFastException（感谢 过客）
* 添加GetThreadId（感谢 过客）
* 添加GetProcessIdOfThread
* 添加QueryThreadCycleTime（感谢 过客）
* 添加QueryProcessCycleTime
* 添加K32EnumProcessModules
* 添加K32EnumProcessModulesEx
* 添加K32GetModuleBaseNameW(A)
* 添加K32GetModuleFileNameExW(A)
* 添加K32EmptyWorkingSet
* 添加K32QueryWorkingSet
* 添加K32QueryWorkingSetEx
* 添加K32InitializeProcessForWsWatch
* 添加K32GetWsChanges
* 添加K32GetWsChangesEx
* 添加K32GetMappedFileNameW(A)
* 添加K32EnumDeviceDrivers
* 添加K32GetDeviceDriverBaseNameW(A)
* 添加K32GetDeviceDriverFileNameW(A)
* 添加K32GetPerformanceInfo
* 添加K32EnumPageFilesW(A)
* 添加K32GetProcessImageFileNameW(A)
* 添加EnumProcessModulesEx
* 添加GetWsChangesEx
* 添加QueryFullProcessImageNameW(A)
* 添加CreateFile2
* 添加CreateEventExW(A)
* 添加CreateMutexExW(A)
* 添加CreateSemaphoreExW
* 添加CreateWaitableTimerExW
* 添加GetFileVersionInfoExW(A)
* 添加GetFileVersionInfoSizeExW(A)
* 添加QueryFullProcessImageNameW(A)
* 添加InterlockedCompareExchange64
* 添加GetProcessId
* 添加SetThreadErrorMode
* 添加GetThreadErrorMode
* 添加GetErrorMode
* 添加CancelIoEx


### 1.0.1.7 - 功能更新（2019-06-04 13:00）
* 添加InitializeSRWLock
* 添加AcquireSRWLockExclusive
* 添加ReleaseSRWLockExclusive
* 添加AcquireSRWLockShared
* 添加ReleaseSRWLockShared
* 添加TryAcquireSRWLockExclusive
* 添加TryAcquireSRWLockShared


### 1.0.1.9 - 优化实现（2019-06-23 13:00）
* 优化代码结构，减少不必要的全局对象引入。
* 内部函数采用__fastcall约定，减少栈操作。
* 增加YY-Thunks版本互斥。
* 改进与联想一键影音的兼容性，由于联想一键影音错会乱Hook，导致LoadLibraryExW行为异常。这样将导致YY-Thunks等在没有安装KB2533623的系统上无法正常使用问题。
* 改进Windows 7 RTM以及以下系统的兼容性，由于这些老版本系统由于在LoadLibraryExW期间不会恢复重定向，因此当目标线程关闭重定向时可能导致YY-Thunks无法正常工作。
* 添加QueryWorkingSetEx


### 1.0.1.17 - 优化实现 (2019-09-06 21:00) 
* 解决Bug，VS2010无法使用问题（感谢 柒零）。
* 改进 SRWLock 以及 One-Time Initialization调整为KeyedEvent实现。
* 添加WSAPoll
* 添加InitializeConditionVariable
* 添加SleepConditionVariableCS
* 添加SleepConditionVariableSRW
* 添加WakeConditionVariable
* 添加WakeAllConditionVariable
* 添加InitOnceInitialize
* 添加InitOnceBeginInitialize
* 添加InitOnceComplete
* 添加InitializeSynchronizationBarrier
* 添加EnterSynchronizationBarrier
* 添加DeleteSynchronizationBarrier
* 添加WaitOnAddress
* 添加WakeByAddressSingle
* 添加WakeByAddressAll
* 添加SHGetKnownFolderPath
* 添加SHSetKnownFolderPath
* 添加SHGetKnownFolderIDList


### 1.0.1.23 - 优化实现（2019-10-25 13:00）
* 解决 Bug，CreateFile2 dwSecurityQosFlags成员可能无法发挥作用问题（感谢 賈可）。
* 解决 Bug，KnownFoldersIdsMap缺少 FOLDERID_ProgramFilesCommonX86问题（感谢 賈可）。
* 解决 Bug，KnownFoldersIdsMap会生static静态对象初始化代码问题（感谢 Joe）。
* 行为调整，EnumCalendarInfoExEx、EnumDateFormatsExEx消除 TLS 依赖。
* 添加 SHBindToFolderIDListParent（感谢 賈可）
* 添加 SHBindToFolderIDListParentEx（感谢 賈可）
* 添加 SHBindToObject（感谢 賈可）
* 添加 SHCreateItemFromIDList（感谢 賈可）
* 添加 SHCreateItemWithParent（感谢 賈可）
* 添加 SHCreateItemFromRelativeName（感谢 賈可）
* 添加 SHGetNameFromIDList（感谢 賈可）
* 添加 SHCreateShellItem
* 添加 OpenFileById
* 添加 RegCopyTreeW(A)（感谢 賈可）
* 添加 CreateSymbolicLinkW(A)
* 添加 ReOpenFile

### 1.0.2.8 - 扩充实现（2020-07-12 18:00）
* 优化实现，改进YY-Thunks性能与体积
* 解决Bug，修复RtlpQueueWaitBlockToSRWLock与RtlpWakeSingle错误（感谢 清泠）
* 解决Bug，CLSID_ShellItem符号可能冲突问题（感谢 星期四）
* 解决Bug，与现代化stdio模式触发FAILIFMISMATCH链接错误。
* 添加 CompareStringEx
* 添加 CompareStringOrdinal
* 添加 SetFilePointerEx（Windows 2000模式）
* 添加 GetModuleHandleExW(A)（Windows 2000模式）
* 添加 WTSGetActiveConsoleSessionId（Windows 2000模式）
* 添加 StrToInt64ExW(A)（Windows 2000模式）
* 添加 GetNativeSystemInfo（Windows 2000模式）
* 添加 InitializeSListHead（Windows 2000模式）
* 添加 InterlockedFlushSList（Windows 2000模式）
* 添加 QueryDepthSList（Windows 2000模式）
* 添加 InterlockedPushEntrySList（Windows 2000模式）
* 添加 InterlockedPopEntrySList（Windows 2000模式）
* 添加 LoadLibraryExW(A) LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR、LOAD_LIBRARY_SEARCH_APPLICATION_DIR、LOAD_LIBRARY_SEARCH_SYSTEM32支持
* 添加 SetupDiGetDevicePropertyW
* 添加 SetupDiSetDevicePropertyW
* 添加 SetupDiGetClassPropertyW
* 添加 SetupDiGetClassPropertyExW
* 添加 SetupDiSetClassPropertyW
* 添加 SetupDiSetClassPropertyExW
