# YY-Thunks Thunks 清单

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
| [LCMapStringEx](https://docs.microsoft.com/windows/win32/api/winnls/nf-winnls-lcmapstringex)                                   | 不存在时，调用LCMapStringW。
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
| *[GetNumaNodeProcessorMask](https://msdn.microsoft.com/library/windows/desktop/ms683204.aspx)                                  | 不存在时，假定所有CPU都在当前Numa。
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
| [CreateSymbolicLinkW(A)](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-createsymboliclinkw)                   | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
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
| [InterlockedPopEntrySList](https://docs.microsoft.com/windows/win32/api/interlockedapi/nf-interlockedapi-interlockedpopentryslist)   | 不存在时，调用lock cmpxchg8b指令。
| [SetupDiGetDevicePropertyW](https://docs.microsoft.com/windows/win32/api/setupapi/nf-setupapi-setupdigetdevicepropertyw)        | 不存在时，调用SetupDiGetDeviceRegistryPropertyW。
| [SetupDiSetDevicePropertyW](https://docs.microsoft.com/windows/win32/api/setupapi/nf-setupapi-setupdisetdevicepropertyw)        | 不存在时，调用SetupDiSetDeviceRegistryPropertyW。
| [SetupDiGetClassPropertyW](https://docs.microsoft.com/windows/win32/api/setupapi/nf-setupapi-setupdigetclasspropertyw)          | 不存在时，调用SetupDiGetClassRegistryPropertyW。
| [SetupDiGetClassPropertyExW](https://docs.microsoft.com/windows/win32/api/setupapi/nf-setupapi-setupdigetclasspropertyexw)      | 不存在时，调用SetupDiGetClassRegistryPropertyW。
| [SetupDiSetClassPropertyW](https://docs.microsoft.com/windows/win32/api/setupapi/nf-setupapi-setupdisetclasspropertyw)          | 不存在时，调用SetupDiSetClassRegistryPropertyW。
| [SetupDiSetClassPropertyExW](https://docs.microsoft.com/windows/win32/api/setupapi/nf-setupapi-setupdisetclasspropertyexw)      | 不存在时，调用SetupDiSetClassRegistryPropertyW。
| [GetDpiForMonitor](https://docs.microsoft.com/windows/win32/api/shellscalingapi/nf-shellscalingapi-getdpiformonitor)            | 不存在时，调用GetDeviceCaps。
| [SetProcessDpiAwareness](https://docs.microsoft.com/windows/win32/api/shellscalingapi/nf-shellscalingapi-setprocessdpiawareness)| 不存在时，调用SetProcessDPIAware。
| [SetProcessDPIAware](https://docs.microsoft.com/windows/win32/api/shellscalingapi/nf-shellscalingapi-setprocessdpiawareness)    | 不存在时，直接返回 TRUE。
| [SetProcessDpiAwarenessContext](https://docs.microsoft.com/windows/win32/api/winuser/nf-winuser-setprocessdpiawarenesscontext)  | 不存在时，调用SetProcessDpiAwareness。
| [GetDpiForSystem](https://docs.microsoft.com/windows/win32/api/winuser/nf-winuser-getdpiforsystem)                              | 不存在时，调用GetDeviceCaps。
| [GetDpiForWindow](https://docs.microsoft.com/windows/win32/api/winuser/nf-winuser-getdpiforwindow)                              | 不存在时，调用GetDpiForMonitor。
| [GetSystemMetricsForDpi](https://docs.microsoft.com/windows/win32/api/winuser/nf-winuser-getsystemmetricsfordpi)                | 不存在时，调用GetSystemMetrics。
| [AdjustWindowRectExForDpi](https://docs.microsoft.com/windows/win32/api/winuser/nf-winuser-adjustwindowrectexfordpi)            | 不存在时，调用AdjustWindowRectEx。
| [SystemParametersInfoForDpi](https://docs.microsoft.com/windows/win32/api/winuser/nf-winuser-systemparametersinfofordpi)        | 不存在时，调用SystemParametersInfoW。
| [PathIsUNCEx](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathisuncex)                                      | 内部实现。
| [PathCchIsRoot](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchisroot)                                  | 内部实现。
| [PathCchAddBackslashEx](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchaddbackslashex)                  | 内部实现。
| [PathCchAddBackslash](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchaddbackslash)                      | 调用PathCchAddBackslashEx。
| [PathCchRemoveBackslashEx](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchremovebackslashex)            | 内部实现。
| [PathCchRemoveBackslash](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchaddbackslash)                   | 调用PathCchRemoveBackslashEx。
| [PathCchSkipRoot](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchskiproot)                              | 内部实现。
| [PathCchStripToRoot](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchstriptoroot)                        | 内部实现。
| [PathCchRemoveFileSpec](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchremovefilespec)                  | 内部实现。
| [PathCchFindExtension](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchfindextension)                    | 内部实现。
| [PathCchAddExtension](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchaddextension)                      | 调用PathCchFindExtension。
| [PathCchRenameExtension](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchrenameextension)                | 调用PathCchFindExtension。
| [PathCchRemoveExtension](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchremoveextension)                | 调用PathCchFindExtension。
| [PathCchCanonicalizeEx](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchcanonicalizeex)                  | 不存在时，内部实现。
| [PathCchCanonicalize](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchcanonicalize)                      | 调用PathCchCanonicalizeEx。
| [PathCchCombineEx](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchcombineex)                            | 内部实现。
| [PathCchCombine](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchcombineex)                              | 调用PathCchCombineEx。
| [PathCchAppendEx](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchappendex)                              | 调用PathCchCombineEx。
| [PathCchAppend](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchappendex)                                | 调用PathCchAppendEx。
| [PathCchStripPrefix](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathcchstripprefix)                        | 内部实现。
| [PathAllocCombine](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathalloccombine)                            | 不存在时，调用PathCchCombineEx。
| [PathAllocCanonicalize](https://docs.microsoft.com/windows/win32/api/pathcch/nf-pathcch-pathalloccanonicalize)                  | 不存在时，调用PathCchCanonicalizeEx。
| [GetNumaProximityNodeEx](https://docs.microsoft.com/windows/win32/api/systemtopologyapi/nf-systemtopologyapi-getnumaproximitynodeex)   | 不存在时，调用GetNumaProximityNode。
| [GetNumaProcessorNode](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-getnumaprocessornode)                    | 不存在时，假定所有CPU都在节点 0。
| [GetNumaNodeNumberFromHandle](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-getnumanodenumberfromhandle)      | 不存在时，假定所有CPU都在节点 0。
| [GetNumaProcessorNodeEx](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-getnumaprocessornodeex)                | 不存在时，调用 GetNumaProcessorNode 。
| [GetNumaAvailableMemoryNode](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-getnumaavailablememorynode)        | 不存在时，假定所有内存都属于节点0 。
| [GetNumaAvailableMemoryNodeEx](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-getnumaavailablememorynodeex)    | 不存在时，调用 GetNumaAvailableMemoryNode 。
| [GetNumaProximityNode](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-getnumaproximitynode)                    | 不存在时，假定都是节点0 。
| [MapViewOfFileExNuma](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-mapviewoffileexnuma)                      | 不存在时，调用 MapViewOfFileEx。
| [AllocateUserPhysicalPagesNuma](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-allocateuserphysicalpagesnuma)     | 不存在时，调用 AllocateUserPhysicalPages 。
| [VirtualAllocExNuma](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-virtualallocexnuma)                    | 不存在时，调用 VirtualAllocEx 。
| [CreateFileMappingNumaW(A)](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-createfilemappingnumaw)         | 不存在时，调用 CreateFileMappingW(A) 。
| [GetMaximumProcessorCount](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-getmaximumprocessorcount)            | 不存在时，调用 GetSystemInfo 。
| [GetActiveProcessorCount](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-getactiveprocessorcount)              | 不存在时，调用 GetSystemInfo 。
| [GetActiveProcessorGroupCount](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-getactiveprocessorgroupcount)    | 不存在时，假定为1 。
| [GetMaximumProcessorGroupCount](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-getmaximumprocessorgroupcount)  | 不存在时，假定为1 。
| [GetMemoryErrorHandlingCapabilities](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-getmemoryerrorhandlingcapabilities) | 不存在时，直接报告不支持任何特性。
| [VirtualAllocFromApp](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-virtualallocfromapp)                  | 不存在时，调用 VirtualAlloc 。
| [VirtualAlloc2](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc2)                              | 不存在时，调用 VirtualAllocExNuma 以及 VirtualAllocEx 。
| [VirtualAlloc2FromApp](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc2fromapp)                | 不存在时，调用 VirtualAllocExNuma 以及 VirtualAllocEx 。
| [CreateFileMappingFromApp](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-createfilemappingfromapp)        | 不存在时，调用 CreateFileMappingW 。
| [CreateFileMapping2](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-createfilemapping2)                    | 不存在时，调用 CreateFileMappingNumaW 以及 CreateFileMappingW 。
| [MapViewOfFileFromApp](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffilefromapp)                | 不存在时，调用 MapViewOfFile 。
| [UnmapViewOfFileEx](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-unmapviewoffileex)                      | 不存在时，调用 UnmapViewOfFile 。
| [VirtualProtectFromApp](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-virtualprotectfromapp)              | 不存在时，调用 VirtualProtect 。
| [OpenFileMappingFromApp](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-openfilemappingfromapp)            | 不存在时，调用 OpenFileMappingW 。
| [CreateThreadpoolWork](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-createthreadpoolwork)  | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [CloseThreadpoolWork](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-closethreadpoolwork)    | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [SubmitThreadpoolWork](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-submitthreadpoolwork)  | 不存在时，调用QueueUserWorkItem。警告，此函数请勿跨模块使用！！！
| [WaitForThreadpoolWorkCallbacks](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-waitforthreadpoolworkcallbacks)                   | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [CreateThreadpoolTimer](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-createthreadpooltimer)                                     | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [CloseThreadpoolTimer](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-closethreadpooltimer)                                       | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [SetThreadpoolTimer](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-setthreadpooltimer)                                           | 不存在时，调用QueueTimer。警告，此函数请勿跨模块使用！！！
| [SetEventWhenCallbackReturns](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-seteventwhencallbackreturns)                         | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [ReleaseSemaphoreWhenCallbackReturns](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-releasesemaphorewhencallbackreturns)         | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [ReleaseMutexWhenCallbackReturns](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-releasemutexwhencallbackreturns)                 | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [LeaveCriticalSectionWhenCallbackReturns](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-leavecriticalsectionwhencallbackreturns) | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [FreeLibraryWhenCallbackReturns](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-freelibrarywhencallbackreturns)                   | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [WaitForThreadpoolTimerCallbacks](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-waitforthreadpooltimercallbacks)                 | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [TrySubmitThreadpoolCallback](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-trysubmitthreadpoolcallback)                         | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [CreateThreadpoolWait](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-createthreadpoolwait)                                       | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [CloseThreadpoolWait](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-closethreadpoolwait)                                         | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [SetThreadpoolWait](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-setthreadpoolwait)                                             | 不存在时，调用RegisterWaitForSingleObject。警告，此函数请勿跨模块使用！！！
| [WaitForThreadpoolWaitCallbacks](https://docs.microsoft.com/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-waitforthreadpoolwaitcallbacks)                   | 不存在时，内部实现。警告，此函数请勿跨模块使用！！！
| [FlushProcessWriteBuffers](https://docs.microsoft.com/windows/win32/api/processthreadsapi/nf-processthreadsapi-flushprocesswritebuffers)                             | 不存在时，调用VirtualProtect。
| [FlsAlloc](https://docs.microsoft.com/windows/win32/api/fibersapi/nf-fibersapi-flsalloc)                                       | 不存在时，使用Tls实现。警告，此函数请勿跨模块使用！！！
| [FlsFree](https://docs.microsoft.com/windows/win32/api/fibersapi/nf-fibersapi-flsfree)                                         | 不存在时，使用Tls实现。警告，此函数请勿跨模块使用！！！
| [FlsGetValue](https://docs.microsoft.com/windows/win32/api/fibersapi/nf-fibersapi-flsgetvalue)                                 | 不存在时，使用Tls实现。警告，此函数请勿跨模块使用！！！
| [FlsSetValue](https://docs.microsoft.com/windows/win32/api/fibersapi/nf-fibersapi-flssetvalue)                                 | 不存在时，使用Tls实现。警告，此函数请勿跨模块使用！！！
| [IsThreadAFiber](https://docs.microsoft.com/windows/win32/api/fibersapi/nf-fibersapi-isthreadafiber)                           | 不存在时，调用 GetCurrentFiber。
| [ConvertThreadToFiberEx](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-convertthreadtofiberex)               | 不存在时，调用 ConvertThreadToFiber。
| [RoInitialize](https://docs.microsoft.com/windows/win32/api/roapi/nf-roapi-roinitialize)                                       | 不存在时，调用 CoInitializeEx。
| [RoUninitialize](https://docs.microsoft.com/windows/win32/api/roapi/nf-roapi-rouninitialize)                                   | 不存在时，调用 CoUninitialize。
| [GetDynamicTimeZoneInformation](https://docs.microsoft.com/windows/win32/api/timezoneapi/nf-timezoneapi-getdynamictimezoneinformation)   | 不存在时，调用 GetTimeZoneInformation。
| [SetDynamicTimeZoneInformation](https://docs.microsoft.com/windows/win32/api/timezoneapi/nf-timezoneapi-setdynamictimezoneinformation)   | 不存在时，调用 SetTimeZoneInformation。
| [GetProductInfo](https://docs.microsoft.com/windows/win32/api/sysinfoapi/nf-sysinfoapi-getproductinfo)                         | 不存在时，调用 GetVersionExW。
| [EnumSystemLocalesEx](https://docs.microsoft.com/windows/win32/api/winnls/nf-winnls-enumsystemlocalesex)                       | 不存在时，调用 EnumSystemLocalesW。
| [GetThreadPreferredUILanguages](https://docs.microsoft.com/windows/win32/api/winnls/nf-winnls-getthreadpreferreduilanguages)   | 不存在时，调用 GetThreadLocale、GetUserDefaultLangID以及GetSystemDefaultLangID。
| [GetThreadUILanguage](https://docs.microsoft.com/windows/win32/api/winnls/nf-winnls-getthreaduilanguage)                       | 不存在时，调用 GetThreadLocale。
| [ResolveLocaleName](https://docs.microsoft.com/windows/win32/api/winnls/nf-winnls-resolvelocalename)                           | 不存在时，调用 LocaleNameToLCID以及LCIDToLocaleName。
| [InitializeProcThreadAttributeList](https://docs.microsoft.com/windows/win32/api/processthreadsapi/nf-processthreadsapi-initializeprocthreadattributelist)          | 不存在时，内部实现。
| [DeleteProcThreadAttributeList](https://docs.microsoft.com/windows/win32/api/processthreadsapi/nf-processthreadsapi-deleteprocthreadattributelist)                  | 不存在时，内部实现。
| [UpdateProcThreadAttribute](https://docs.microsoft.com/windows/win32/api/processthreadsapi/nf-processthreadsapi-updateprocthreadattribute)                          | 不存在时，内部实现。PROC_THREAD_ATTRIBUTE_PARENT_PROCESS与PROC_THREAD_ATTRIBUTE_HANDLE_LIST特性会被忽略处理。
| [GetLargePageMinimum](https://docs.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-getlargepageminimum)    | 不存在时，假定为0 。
| [SHCreateItemFromParsingName](https://docs.microsoft.com/windows/win32/api/shobjidl_core/nf-shobjidl_core-shcreateitemfromparsingname)    | 不存在时，调用SHParseDisplayName。
