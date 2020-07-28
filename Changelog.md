# YY-Thunks 更新日志

### 1.0.2.8 - 扩充实现 (2020-07-12 18:00)

* 优化实现，改进 YY-Thunks 性能与体积
* 解决 Bug，修复 RtlpQueueWaitBlockToSRWLock 与 RtlpWakeSingle 错误 (感谢 清泠)
* 解决 Bug，CLSID_ShellItem符号可能冲突问题 (感谢 星期四)
* 解决 Bug，与现代化stdio模式触发FAILIFMISMATCH链接错误。
* 添加 CompareStringEx
* 添加 CompareStringOrdinal
* 添加 SetFilePointerEx (Windows 2000 模式)
* 添加 GetModuleHandleExW(A) (Windows 2000 模式)
* 添加 WTSGetActiveConsoleSessionId (Windows 2000 模式)
* 添加 StrToInt64ExW(A) (Windows 2000 模式)
* 添加 GetNativeSystemInfo (Windows 2000 模式)
* 添加 InitializeSListHead (Windows 2000 模式)
* 添加 InterlockedFlushSList (Windows 2000 模式)
* 添加 QueryDepthSList (Windows 2000 模式)
* 添加 InterlockedPushEntrySList (Windows 2000 模式)
* 添加 InterlockedPopEntrySList (Windows 2000 模式)
* 添加 LoadLibraryExW(A) LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR, 
  LOAD_LIBRARY_SEARCH_APPLICATION_DIR, LOAD_LIBRARY_SEARCH_SYSTEM32 支持
* 添加 SetupDiGetDevicePropertyW
* 添加 SetupDiSetDevicePropertyW
* 添加 SetupDiGetClassPropertyW
* 添加 SetupDiGetClassPropertyExW
* 添加 SetupDiSetClassPropertyW
* 添加 SetupDiSetClassPropertyExW

### 1.0.1.23 - 优化实现 (2019-10-25 13:00)

* 解决 Bug，CreateFile2 dwSecurityQosFlags成员可能无法发挥作用问题 (感谢 賈可)
* 解决 Bug，KnownFoldersIdsMap 缺少 FOLDERID_ProgramFilesCommonX86 
  问题 (感谢 賈可)
* 解决 Bug，KnownFoldersIdsMap 会生成 static 静态对象初始化代码问题 (感谢 Joe)
* 行为调整，EnumCalendarInfoExEx, EnumDateFormatsExEx 消除 TLS 依赖
* 添加 SHBindToFolderIDListParent (感谢 賈可)
* 添加 SHBindToFolderIDListParentEx (感谢 賈可)
* 添加 SHBindToObject (感谢 賈可)
* 添加 SHCreateItemFromIDList (感谢 賈可)
* 添加 SHCreateItemWithParent (感谢 賈可)
* 添加 SHCreateItemFromRelativeName (感谢 賈可)
* 添加 SHGetNameFromIDList (感谢 賈可)
* 添加 SHCreateShellItem
* 添加 OpenFileById
* 添加 RegCopyTreeW(A) (感谢 賈可)
* 添加 CreateSymbolicLinkW(A)
* 添加 ReOpenFile

### 1.0.1.17 - 优化实现 (2019-09-06 21:00) 

* 解决Bug，VS2010无法使用问题 (感谢 柒零)
* 改进 SRWLock 以及 One-Time Initialization 调整为 KeyedEvent 实现
* 添加 WSAPoll
* 添加 InitializeConditionVariable
* 添加 SleepConditionVariableCS
* 添加 SleepConditionVariableSRW
* 添加 WakeConditionVariable
* 添加 WakeAllConditionVariable
* 添加 InitOnceInitialize
* 添加 InitOnceBeginInitialize
* 添加 InitOnceComplete
* 添加 InitializeSynchronizationBarrier
* 添加 EnterSynchronizationBarrier
* 添加 DeleteSynchronizationBarrier
* 添加 WaitOnAddress
* 添加 WakeByAddressSingle
* 添加 WakeByAddressAll
* 添加 SHGetKnownFolderPath
* 添加 SHSetKnownFolderPath
* 添加 SHGetKnownFolderIDList

### 1.0.1.9 - 优化实现 (2019-06-23 13:00)

* 优化代码结构，减少不必要的全局对象引入
* 内部函数采用 __fastcall 约定，减少栈操作
* 增加 YY-Thunks 版本互斥
* 改进与联想一键影音的兼容性，由于联想一键影音错会乱 Hook，导致 LoadLibraryExW 
  行为异常。这样将导致 YY-Thunks 等在没有安装 KB2533623 的系统上无法正常使用
* 改进 Windows 7 RTM 以及以下系统的兼容性，由于这些老版本系统由于在 
  LoadLibraryExW 期间不会恢复重定向，因此当目标线程关闭重定向时可能导致 
  YY-Thunks 无法正常工作
* 添加 QueryWorkingSetEx

### 1.0.1.7 - 功能更新 (2019-06-04 13:00)

* 添加 InitializeSRWLock
* 添加 AcquireSRWLockExclusive
* 添加 ReleaseSRWLockExclusive
* 添加 AcquireSRWLockShared
* 添加 ReleaseSRWLockShared
* 添加 TryAcquireSRWLockExclusive
* 添加 TryAcquireSRWLockShared

### 1.0.1.5 - 功能更新 (2019-05-26 19:00)

* 添加 RaiseFailFastException (感谢 过客)
* 添加 GetThreadId (感谢 过客)
* 添加 GetProcessIdOfThread
* 添加 QueryThreadCycleTime (感谢 过客)
* 添加 QueryProcessCycleTime
* 添加 K32EnumProcessModules
* 添加 K32EnumProcessModulesEx
* 添加 K32GetModuleBaseNameW(A)
* 添加 K32GetModuleFileNameExW(A)
* 添加 K32EmptyWorkingSet
* 添加 K32QueryWorkingSet
* 添加 K32QueryWorkingSetEx
* 添加 K32InitializeProcessForWsWatch
* 添加 K32GetWsChanges
* 添加 K32GetWsChangesEx
* 添加 K32GetMappedFileNameW(A)
* 添加 K32EnumDeviceDrivers
* 添加 K32GetDeviceDriverBaseNameW(A)
* 添加 K32GetDeviceDriverFileNameW(A)
* 添加 K32GetPerformanceInfo
* 添加 K32EnumPageFilesW(A)
* 添加 K32GetProcessImageFileNameW(A)
* 添加 EnumProcessModulesEx
* 添加 GetWsChangesEx
* 添加 QueryFullProcessImageNameW(A)
* 添加 CreateFile2
* 添加 CreateEventExW(A)
* 添加 CreateMutexExW(A)
* 添加 CreateSemaphoreExW
* 添加 CreateWaitableTimerExW
* 添加 GetFileVersionInfoExW(A)
* 添加 GetFileVersionInfoSizeExW(A)
* 添加 QueryFullProcessImageNameW(A)
* 添加 InterlockedCompareExchange64
* 添加 GetProcessId
* 添加 SetThreadErrorMode
* 添加 GetThreadErrorMode
* 添加 GetErrorMode
* 添加 CancelIoEx

### 1.0.1.1 - 功能更新 (2019-04-01 17:00)

* 添加 EnumCalendarInfoExEx
* 添加 EnumDateFormatsExEx
* 添加 GetFileInformationByHandleEx
* 添加 SetFileInformationByHandle
* 添加 GetFinalPathNameByHandleW(A)
* 解决 Bug，1.0.0.10 版意外引入 ntdll.lib 问题 (感谢 小古)
* 添加 GetLogicalProcessorInformation
* 添加 GetLogicalProcessorInformationEx
* 添加 InetPtonW(inet_pton)
* 添加 InetNtopW(inet_ntop)
* 添加 GetNumaHighestNodeNumber

### 1.0.0.7 - 功能更新 (2019-01-02 15:10)

* 添加 RegGetValueW(A)
* 添加 LocaleNameToLCID
* 添加 LCIDToLocaleName
* 添加 GetLocaleInfoEx
* 添加 GetDateFormatEx
* 添加 GetTimeFormatEx
* 添加 GetNumberFormatEx
* 添加 GetCurrencyFormatEx
* 添加 GetUserDefaultLocaleName
* 添加 GetSystemDefaultLocaleName
* 重新编译，解决潜在符号修饰问题

### 1.0.0.4 - 兼容性更新 (2018-09-08 18:00)

* 解决Bug，将初始化时机推迟到 `.CRT$XID`，避免 VC2008 下过早初始化导致 atexit 
  崩溃 (感谢 死田鸡)

### 1.0.0.3 - 让兼容Windows ARM64更轻松 (2018-06-18 11:30)

* 添加 IsWow64Process2
* 添加 IsWow64GuestMachineSupported
* 添加 GetTickCount64
* 添加 GetSystemTimePreciseAsFileTime
* 添加 InitializeCriticalSectionEx
* 添加 InitOnceExecuteOnce
* 添加 GetCurrentProcessorNumber
* 添加 GetCurrentProcessorNumberEx
* 添加 GetNumaNodeProcessorMask
* 添加 GetNumaNodeProcessorMaskEx
* 添加 SetThreadGroupAffinity

### 1.0.0.2 - 补充API支持 (2018-06-01 18:30)

* 添加 RegSetKeyValueW(A)
* 添加 RegDeleteKeyValueW(A)
* 添加 RegDeleteTreeW(A)

### 1.0.0.1 - 第一版 (2018-05-17 19:00)

* YY-Thunks 第一版
