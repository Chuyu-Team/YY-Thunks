# YY-Thunks 更新日志

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

