
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
1. 下载[YY-Thunks-Binary](https://github.com/Chuyu-Team/YY-Thunks/releases)，然后解压到你的工程目录。<br/>
2. 【链接器】-【输入】-【附加依赖项】，添加`objs\$(PlatformShortName)\YY_Thunks_for_WinXP.obj`。<br/>
3. 重新编译代码。

## 3. YY-Thunks兼容性
### 3.1. 支持的编译器
全平台ABI兼容。
* 所有Visual Studio版本均支持（比如：VC6.0、VS2008、VS2010、VS2015、VS2017、VS2019等等）。
* 所有运行库模式均支持（比如：`/MD`、`/MT`、`/MDd`、`/MTd`）。

### 3.2. Thunks清单
此表展示了YY-Thunks（鸭船）可以解决的函数不存在问题，欢迎大家扩充！

> 开头带`*`的函数并不建议使用，仅用于编译通过处理，如果使用可能导致老版本系统无法充分发挥性能。

| 函数                                                                                                                           | Fallback
| ----                                                                                                                           | -----------
| [DecodePointer](https://msdn.microsoft.com/library/bb432242.aspx)                                                              | 不存在时，返回指针本身。
| [EncodePointer](https://msdn.microsoft.com/library/bb432254.aspx)                                                              | 不存在时，返回指针本身。
| [RegDeleteKeyExW(A)](https://msdn.microsoft.com/library/windows/desktop/ms724847.aspx)                                         | 不存在时，调用RegDeleteKeyW(A)。
| [Wow64DisableWow64FsRedirection](https://msdn.microsoft.com/library/windows/desktop/aa365743.aspx)                             | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [Wow64RevertWow64FsRedirection](https://msdn.microsoft.com/library/windows/desktop/aa365745.aspx)                              | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [Wow64EnableWow64FsRedirection](https://msdn.microsoft.com/library/windows/desktop/aa365744.aspx)                              | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [IsWow64Process](https://msdn.microsoft.com/library/windows/desktop/ms684139.aspx)                                             | 不存在时，返回TRUE，并设置 `*Wow64Process = FALSE`。
| [IsWow64Message](https://msdn.microsoft.com/library/windows/desktop/ms684136.aspx)                                             | 不存在时，返回FALSE。
| [RegSetKeyValueW(A)](https://msdn.microsoft.com/library/ms724921.aspx)                                                         | 调用RegCreateKeyExW(A)以及RegSetValueExW(A)。
| [RegDeleteKeyValueW(A)](https://msdn.microsoft.com/library/ms724848.aspx)                                                      | 调用RegOpenKeyExW(A)以及RegDeleteValueW(A)。
| [RegDeleteTreeW(A)](https://msdn.microsoft.com/library/aa379776.aspx)                                                          | 调用SHDeleteKeyW(A)。
| [RegGetValueW(A)](https://msdn.microsoft.com/library/aa379776.aspx)                                                            | 不存在时，调用RegQueryValueExW(A)。
| [RegCopyTreeW(A)](https://docs.microsoft.com/windows/win32/api/winreg/nf-winreg-regcopytreew)                                  | 不存在时，调用SHCopyKeyW(A)。
| [IsWow64Process2](https://msdn.microsoft.com/library/windows/desktop/mt804318.aspx)                                            | 不存在时，调用IsWow64Process。
| [IsWow64GuestMachineSupported](https://msdn.microsoft.com/library/windows/desktop/mt804321.aspx)                               | 不存在时，调用GetNativeSystemInfo。
| [GetTickCount64](https://msdn.microsoft.com/library/windows/desktop/ms724411.aspx)                                             | 不存在时，调用GetTickCount。
| [GetSystemTimePreciseAsFileTime](https://msdn.microsoft.com/library/windows/desktop/hh706895.aspx)                             | 不存在时，调用GetSystemTimeAsFileTime。
| [InitializeCriticalSectionEx](https://msdn.microsoft.com/library/ms683477.aspx)                                                | 不存在时，调用InitializeCriticalSectionAndSpinCount。
| [InitOnceInitialize](https://msdn.microsoft.com/library/ms683495.aspx)                                                         | 初始化为 INIT_ONCE_STATIC_INIT。
| [InitOnceBeginInitialize](https://msdn.microsoft.com/library/ms683487.aspx)                                                    | 不存在时，调用NtWaitForKeyedEvent。
| [InitOnceComplete](https://msdn.microsoft.com/library/ms683491.aspx)                                                           | 不存在时，调用NtReleaseKeyedEvent。
| [InitOnceExecuteOnce](https://msdn.microsoft.com/library/windows/desktop/ms683493.aspx)                                        | 不存在时，调用NtWaitForKeyedEvent以及NtReleaseKeyedEvent。
| [LocaleNameToLCID](https://docs.microsoft.com/windows/desktop/api/winnls/nf-winnls-localenametolcid)                           | 不存在时，查LocaleNameToLcidTable。
| [LCIDToLocaleName](https://docs.microsoft.com/windows/desktop/api/winnls/nf-winnls-lcidtolocalename)                           | 不存在时，查LcidToLocaleNameTable。
| [GetLocaleInfoEx](https://docs.microsoft.com/windows/desktop/api/winnls/nf-winnls-getlocaleinfoex)                             | 不存在时，调用GetLocaleInfoW。
| [GetDateFormatEx](https://docs.microsoft.com/windows/desktop/api/datetimeapi/nf-datetimeapi-getdateformatex)                   | 不存在时，调用GetDateFormatW。
| [GetTimeFormatEx](https://docs.microsoft.com/windows/desktop/api/datetimeapi/nf-datetimeapi-gettimeformatex)                   | 不存在时，调用GetTimeFormatW。
| [GetNumberFormatEx](https://docs.microsoft.com/windows/desktop/api/winnls/nf-winnls-getnumberformatex)                         | 不存在时，调用GetNumberFormatW。
| [GetCurrencyFormatEx](https://docs.microsoft.com/windows/desktop/api/winnls/nf-winnls-getcurrencyformatex)                     | 不存在时，调用GetCurrencyFormatW。
| [GetUserDefaultLocaleName](https://docs.microsoft.com/windows/desktop/api/winnls/nf-winnls-getuserdefaultlocalename)           | 不存在时，调用LCIDToLocaleName。
| [GetSystemDefaultLocaleName](https://docs.microsoft.com/windows/desktop/api/winnls/nf-winnls-getsystemdefaultlocalename)       | 不存在时，调用LCIDToLocaleName。
| [EnumCalendarInfoExEx](https://docs.microsoft.com/windows/desktop/api/winnls/nf-winnls-enumcalendarinfoexex)                   | 不存在时，调用EnumCalendarInfoExW。
| [EnumDateFormatsExEx](https://docs.microsoft.com/windows/desktop/api/winnls/nf-winnls-enumdateformatsexex)                     | 不存在时，调用EnumDateFormatsExW。
| [GetFileInformationByHandleEx](https://docs.microsoft.com/windows/desktop/api/winbase/nf-winbase-getfileinformationbyhandleex) | 不存在时，调用NtQueryInformationFile 或者 NtQueryDirectoryFile。
| [SetFileInformationByHandle](https://docs.microsoft.com/windows/desktop/api/winbase/nf-winbase-getfileinformationbyhandleex)   | 不存在时，调用NtSetInformationFile。
| [GetFinalPathNameByHandleW(A)](https://docs.microsoft.com/windows/desktop/api/fileapi/nf-fileapi-getfinalpathnamebyhandlew)    | 不存在时，调用NtQueryObject以及NtQueryInformationFile。
| [GetLogicalProcessorInformation](https://msdn.microsoft.com/library/ms683194.aspx)                                             | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [GetLogicalProcessorInformationEx](https://msdn.microsoft.com/library/dd405488.aspx)                                           | 不存在时，调用GetLogicalProcessorInformation。
| [InetPtonW(inet_pton)](https://docs.microsoft.com/windows/desktop/api/ws2tcpip/nf-ws2tcpip-inetptonw)                          | 不存在时，类似于sscanf手工分析字符串。
| [InetNtopW(inet_ntop)](https://docs.microsoft.com/windows/desktop/api/ws2tcpip/nf-ws2tcpip-inetntopw)                          | 不存在时，类似于sprintf手工生成字符串。
| [WSAPoll](https://docs.microsoft.com/windows/win32/api/winsock2/nf-winsock2-wsapoll)                                           | 不存在时，调用select。
| [GetNumaHighestNodeNumber](https://msdn.microsoft.com/library/windows/desktop/ms683203.aspx)                                   | 不存在时，返回0。
| [RaiseFailFastException](https://msdn.microsoft.com/library/windows/desktop/dd941688.aspx)                                     | 不存在时，调用TerminateProcess。
| [GetThreadId](https://docs.microsoft.com/windows/desktop/api/processthreadsapi/nf-processthreadsapi-getthreadid)               | 不存在时，调用NtQueryInformationThread。
| [GetProcessIdOfThread](https://msdn.microsoft.com/library/ms683216.aspx)                                                       | 不存在时，调用NtQueryInformationThread。
| [GetProcessId](https://docs.microsoft.com/windows/desktop/api/processthreadsapi/nf-processthreadsapi-getprocessid)             | 不存在时，调用NtQueryInformationProcess。
| [QueryThreadCycleTime](https://docs.microsoft.com/windows/desktop/api/realtimeapiset/nf-realtimeapiset-querythreadcycletime)   | 不存在时，调用GetThreadTimes。
| [QueryProcessCycleTime](https://docs.microsoft.com/windows/desktop/api/realtimeapiset/nf-realtimeapiset-queryprocesscycletime) | 不存在时，调用GetProcessTimes。
| [K32EnumProcessModules](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-enumprocessmodules)                      | 调用EnumProcessModules。
| [K32EnumProcessModulesEx](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-enumprocessmodulesex)                  | 调用EnumProcessModulesEx。
| [K32GetModuleBaseNameW(A)](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-getmodulebasenamea)                   | 调用GetModuleBaseNameW(A)。
| [K32GetModuleFileNameExW(A)](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-getmodulefilenameexw)               | 调用K32GetModuleFileNameExW(A)。
| [K32EmptyWorkingSet](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-emptyworkingset)                            | 调用EmptyWorkingSet。
| [K32QueryWorkingSet](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-queryworkingset)                            | 调用QueryWorkingSet。
| [K32QueryWorkingSetEx](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-queryworkingsetex)                        | 调用QueryWorkingSetEx。
| [K32InitializeProcessForWsWatch](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-initializeprocessforwswatch)    | 调用InitializeProcessForWsWatch。
| [K32GetWsChanges](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-getwschanges)                                  | 调用GetWsChanges。
| [K32GetWsChangesEx](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-getwschangesex)                              | 调用GetWsChangesEx。
| [K32GetMappedFileNameW(A)](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-getmappedfilenamew)                   | 调用GetMappedFileNameW(A)。
| [K32EnumDeviceDrivers](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-enumdevicedrivers)                        | 调用EnumDeviceDrivers。
| [K32GetDeviceDriverBaseNameW(A)](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-getdevicedriverbasenamew)       | 调用GetDeviceDriverBaseNameW(A)。
| [K32GetDeviceDriverFileNameW(A)](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-getdevicedriverfilenamew)       | 调用GetDeviceDriverFileNameW(A)。
| [K32GetPerformanceInfo](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-getperformanceinfo)                      | 调用GetPerformanceInfo。
| [K32EnumPageFilesW(A)](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-enumpagefilesw)                           | 调用EnumPageFilesW(A)。
| [K32GetProcessImageFileNameW(A)](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-enumpagefilesw)                 | 调用GetProcessImageFileNameW(A)。
| [EnumProcessModulesEx](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-enumprocessmodulesex)                     | 不存在时，调用EnumProcessModules。
| [GetWsChangesEx](https://docs.microsoft.com/windows/desktop/api/psapi/nf-psapi-getwschangesex)                                 | 不存在时，调用GetWsChanges。
| [QueryFullProcessImageNameW(A)](https://docs.microsoft.com/windows/desktop/api/winbase/nf-winbase-queryfullprocessimagenamew)  | 不存在时，调用GetProcessImageFileNameW(A) 或者 GetModuleFileNameExW(A)。
| [CreateFile2](https://docs.microsoft.com/windows/desktop/api/fileapi/nf-fileapi-createfile2)                                   | 不存在时，调用CreateFileW。
| [CreateEventExW(A)](https://docs.microsoft.com/windows/desktop/api/synchapi/nf-synchapi-createeventexw)                        | 不存在时，调用CreateEventW(A)。
| [CreateMutexExW(A)](https://docs.microsoft.com/windows/desktop/api/synchapi/nf-synchapi-createmutexexw)                        | 不存在时，调用CreateMutexW(A)。
| [CreateSemaphoreExW](https://docs.microsoft.com/windows/desktop/api/synchapi/nf-synchapi-createsemaphoreexw)                   | 不存在时，调用CreateSemaphoreW。
| [CreateWaitableTimerExW](https://docs.microsoft.com/windows/desktop/api/synchapi/nf-synchapi-createwaitabletimerexw)           | 不存在时，调用CreateWaitableTimerW。
| [GetFileVersionInfoExW(A)](https://docs.microsoft.com/windows/desktop/api/winver/nf-winver-getfileversioninfosizeexw)          | 不存在时，调用GetFileVersionInfoW(A)。
| [GetFileVersionInfoSizeExW(A)](https://docs.microsoft.com/windows/desktop/api/winver/nf-winver-getfileversioninfosizeexw)      | 不存在时，调用GetFileVersionInfoSizeW(A)。
| [InterlockedCompareExchange64](https://docs.microsoft.com/windows/desktop/api/winnt/nf-winnt-interlockedcompareexchange64)     | 调用内部函数_InterlockedCompareExchange64。
| [SetThreadErrorMode](https://docs.microsoft.com/windows/desktop/api/errhandlingapi/nf-errhandlingapi-setthreaderrormode)       | 不存在时，调用SetErrorMode。
| [GetThreadErrorMode](https://docs.microsoft.com/windows/desktop/api/errhandlingapi/nf-errhandlingapi-getthreaderrormode)       | 不存在时，调用GetErrorMode。
| [GetErrorMode](https://docs.microsoft.com/windows/desktop/api/errhandlingapi/nf-errhandlingapi-geterrormode)                   | 不存在时，调用NtQueryInformationProcess。
| [InitializeSRWLock](https://docs.microsoft.com/windows/desktop/api/synchapi/nf-synchapi-initializesrwlock)                     | 初始化为 RTL_SRWLOCK_INIT。
| [AcquireSRWLockExclusive](https://docs.microsoft.com/windows/desktop/api/synchapi/nf-synchapi-acquiresrwlockexclusive)         | 不存在时，调用NtWaitForKeyedEvent。
| [TryAcquireSRWLockExclusive](https://msdn.microsoft.com/library/Dd405523.aspx)                                                 | 不存在时，调用InterlockedBitTestAndSet(64)。
| [ReleaseSRWLockExclusive](https://msdn.microsoft.com/library/ms685076.aspx)                                                    | 不存在时，调用NtReleaseKeyedEvent。
| [AcquireSRWLockShared](https://msdn.microsoft.com/library/ms681934.aspx)                                                       | 不存在时，调用NtWaitForKeyedEvent。
| [TryAcquireSRWLockShared](https://msdn.microsoft.com/library/Dd405524.aspx)                                                    | 不存在时，调用InterlockedCompareExchange。
| [ReleaseSRWLockShared](https://msdn.microsoft.com/library/ms685080.aspx)                                                       | 不存在时，调用NtReleaseKeyedEvent。
| [InitializeConditionVariable](https://docs.microsoft.com/windows/win32/api/synchapi/nf-synchapi-initializeconditionvariable)   | 初始化为 CONDITION_VARIABLE_INIT。
| [SleepConditionVariableCS](https://docs.microsoft.com/windows/win32/api/synchapi/nf-synchapi-sleepconditionvariablecs)         | 不存在时，调用NtWaitForKeyedEvent。
| [SleepConditionVariableSRW](https://docs.microsoft.com/windows/win32/api/synchapi/nf-synchapi-sleepconditionvariablesrw)       | 不存在时，调用NtWaitForKeyedEvent。
| [WakeConditionVariable](https://docs.microsoft.com/windows/win32/api/synchapi/nf-synchapi-wakeconditionvariable)               | 不存在时，调用NtReleaseKeyedEvent。
| [WakeAllConditionVariable](https://docs.microsoft.com/windows/win32/api/synchapi/nf-synchapi-wakeallconditionvariable)         | 不存在时，调用NtReleaseKeyedEvent。
| [InitializeSynchronizationBarrier](https://msdn.microsoft.com/library/hh706890.aspx)                                           | 不存在时，调用CreateEvent。
| [EnterSynchronizationBarrier](https://msdn.microsoft.com/library/hh706889.aspx)                                                | 不存在时，调用WaitForSingleObject。
| [DeleteSynchronizationBarrier](https://msdn.microsoft.com/library/hh706887.aspx)                                               | 不存在时，调用CloseHandle。
| [WaitOnAddress](https://docs.microsoft.com/windows/desktop/api/SynchAPI/nf-synchapi-waitonaddress)                             | 不存在时，调用NtWaitForKeyedEvent。警告，此函数请勿跨模块使用！！！
| [WakeByAddressSingle](https://docs.microsoft.com/windows/win32/api/synchapi/nf-synchapi-wakebyaddresssingle)                   | 不存在时，调用NtReleaseKeyedEvent。警告，此函数请勿跨模块使用！！！
| [WakeByAddressAll](https://docs.microsoft.com/windows/win32/api/synchapi/nf-synchapi-wakebyaddressall)                         | 不存在时，调用NtReleaseKeyedEvent。警告，此函数请勿跨模块使用！！！
| *[GetCurrentProcessorNumber](https://msdn.microsoft.com/library/windows/desktop/ms683181.aspx)                                 | 不存在时，返回0。
| *[GetCurrentProcessorNumberEx](https://msdn.microsoft.com/library/windows/desktop/dd405487.aspx)                               | 不存在时，调用GetCurrentProcessorNumber。
| *[GetNumaNodeProcessorMask](https://msdn.microsoft.com/library/windows/desktop/ms683204.aspx)                                  | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_PARAMETER。
| *[GetNumaNodeProcessorMaskEx](https://msdn.microsoft.com/library/windows/desktop/dd405493.aspx)                                | 不存在时，调用GetNumaNodeProcessorMask。
| *[SetThreadGroupAffinity](https://msdn.microsoft.com/library/windows/desktop/dd405516.aspx)                                    | 不存在时，调用SetThreadAffinityMask。
| *[CancelIoEx](https://docs.microsoft.com/windows/desktop/FileIO/cancelioex-func)                                               | 不存在时，调用CancelIo（会把此句柄的所有IO操作取消掉！）。
| *[QueryWorkingSetEx](https://docs.microsoft.com/windows/desktop/api/Psapi/nf-psapi-getwschangesex)                             | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [SHGetKnownFolderPath](https://docs.microsoft.com/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath)           | 不存在时，调用SHGetFolderPathW。
| [SHSetKnownFolderPath](https://docs.microsoft.com/windows/win32/api/shlobj_core/nf-shlobj_core-shsetknownfolderpath)           | 不存在时，调用SHSetFolderPathW。
| [SHGetKnownFolderIDList](https://docs.microsoft.com/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderidlist)       | 不存在时，调用SHGetFolderLocation。
| [SHBindToFolderIDListParent](https://docs.microsoft.com/windows/win32/api/shlobj_core/nf-shlobj_core-shbindtofolderidlistparent)      | 不存在时，调用IShellFolder。
| [SHBindToFolderIDListParentEx](https://docs.microsoft.com//windows/win32/api/shlobj_core/nf-shlobj_core-shbindtofolderidlistparentex) | 不存在时，调用IShellFolder。
| [SHBindToObject](https://docs.microsoft.com/windows/win32/api/shlobj_core/nf-shlobj_core-shbindtoobject)                        | 不存在时，调用IShellFolder。
| [SHCreateItemFromIDList](https://docs.microsoft.com/windows/win32/api/shobjidl_core/nf-shobjidl_core-shcreateitemfromidlist)    | 不存在时，调用IShellItem。
| [SHCreateItemWithParent](https://docs.microsoft.com/windows/win32/api/shobjidl_core/nf-shobjidl_core-shcreateitemwithparent)    | 不存在时，调用IShellItem。
| [SHCreateItemFromRelativeName](https://docs.microsoft.com/windows/win32/api/shobjidl_core/nf-shobjidl_core-shcreateitemfromrelativename) | 不存在时，调用IShellItem。
| [SHGetNameFromIDList](https://docs.microsoft.com/windows/win32/api/shobjidl_core/nf-shobjidl_core-shgetnamefromidlist)          | 不存在时，调用IShellItem。
| [SHCreateShellItem](https://docs.microsoft.com/windows/win32/api/shlobj_core/nf-shlobj_core-shcreateshellitem)                  | 不存在时，调用IShellItem。
| [OpenFileById](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-openfilebyid)                                    | 不存在时，调用NtCreateFile。
| [CreateSymbolicLinkW(A)](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-createsymboliclinkw)                   | 不存在时，调用DeviceIoControl。
| [ReOpenFile](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-reopenfile)                                        | 不存在时，调用NtCreateFile。
| [CompareStringEx](https://docs.microsoft.com/windows/win32/api/stringapiset/nf-stringapiset-comparestringex)                    | 不存在时，调用CompareStringW。
| [CompareStringOrdinal](https://docs.microsoft.com/windows/win32/api/stringapiset/nf-stringapiset-comparestringordinal)          | 不存在时，使用内置UnicodeCaseTableData实现。
| [SetFilePointerEx](https://docs.microsoft.com/windows/win32/api/fileapi/nf-fileapi-setfilepointerex)                            | 不存在时，调用SetFilePointer。
| [GetModuleHandleExW(A)](https://docs.microsoft.com/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandleexw)           | 不存在时，调用GetModuleHandleW(A)。
| [WTSGetActiveConsoleSessionId](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-wtsgetactiveconsolesessionid)    | 不存在时，直接返回 0。
| [StrToInt64ExW(A)](https://docs.microsoft.com/windows/win32/api/shlwapi/nf-shlwapi-strtoint64exw)                               | 不存在时，手工解析字符串。
| [GetNativeSystemInfo](https://docs.microsoft.com/windows/win32/api/sysinfoapi/nf-sysinfoapi-getnativesysteminfo)                | 不存在时，调用GetSystemInfo。
| [InitializeSListHead](https://docs.microsoft.com/windows/win32/api/interlockedapi/nf-interlockedapi-initializeslisthead)        | 直接初始化为 0。
| [InterlockedFlushSList](https://docs.microsoft.com/windows/win32/api/interlockedapi/nf-interlockedapi-interlockedflushslist)    | 不存在时，调用lock cmpxchg8b指令。
| [QueryDepthSList](https://docs.microsoft.com/windows/win32/api/interlockedapi/nf-interlockedapi-querydepthslist)                | 不存在时，直接返回Depth。
| [InterlockedPushEntrySList](https://docs.microsoft.com/windows/win32/api/interlockedapi/nf-interlockedapi-interlockedpushentryslist) | 不存在时，调用lock cmpxchg8b指令。
| [InterlockedPopEntrySList](https://docs.microsoft.com/windows/win32/api/interlockedapi/nf-interlockedapi-interlockedpopentryslist) | 不存在时，调用lock cmpxchg8b指令。


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

### 1.0.2.4 - 扩充实现（2020-03-03 19:00）
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
* 优化实现，改进YY-Thunks性能与体积
