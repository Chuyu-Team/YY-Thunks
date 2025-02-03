# YY-Thunks Thunks 清单

此表展示了YY-Thunks（鸭船）可以解决的函数不存在问题，欢迎大家扩充！

> 开头带`*`的函数并不建议使用，存在一些较大负面影响，仅用于编译通过处理，具体负面影响可参考注释内容。

## api-ms-win-core-handle-l1-1-0.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CompareObjectHandles                       | 调用NtQueryObject以及DuplicateHandle。

## api-ms-win-core-path-l1-1-0.dll
| 函数                                       | Fallback
| ----                                       | -----------
| PathIsUNCEx                                | 内部实现。
| PathCchIsRoot                              | 内部实现。
| PathCchAddBackslashEx                      | 内部实现。
| PathCchAddBackslash                        | 调用PathCchAddBackslashEx。
| PathCchRemoveBackslashEx                   | 内部实现。
| PathCchRemoveBackslash                     | 调用PathCchRemoveBackslashEx。
| PathCchSkipRoot                            | 内部实现。
| PathCchStripToRoot                         | 内部实现。
| PathCchRemoveFileSpec                      | 内部实现。
| PathCchFindExtension                       | 内部实现。
| PathCchAddExtension                        | 调用PathCchFindExtension。
| PathCchRenameExtension                     | 调用PathCchFindExtension。
| PathCchRemoveExtension                     | 调用PathCchFindExtension。
| PathCchCanonicalizeEx                      | 内部实现。
| PathCchCanonicalize                        | 调用PathCchCanonicalizeEx。
| PathCchCombineEx                           | 内部实现。
| PathCchCombine                             | 调用PathCchCombineEx。
| PathCchAppendEx                            | 调用PathCchCombineEx。
| PathCchAppend                              | 调用PathCchAppendEx。
| PathCchStripPrefix                         | 内部实现。
| PathAllocCombine                           | 调用PathCchCombineEx。
| PathAllocCanonicalize                      | 调用PathCchCanonicalizeEx。

## api-ms-win-core-realtime-l1-1-1.dll
| 函数                                       | Fallback
| ----                                       | -----------
| QueryUnbiasedInterruptTimePrecise          | 调用QueryUnbiasedInterruptTime。
| QueryInterruptTime                         | 读取KUSER_SHARED_DATA::InterruptTime值。
| QueryInterruptTimePrecise                  | 读取KUSER_SHARED_DATA::InterruptTime值。

## api-ms-win-core-threadpool-l1-2-0.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CreateThreadpoolWork                       | 内部实现。
| CloseThreadpoolWork                        | 内部实现。
| TrySubmitThreadpoolCallback                | 调用QueueUserWorkItem。
| SubmitThreadpoolWork                       | 调用QueueUserWorkItem。
| WaitForThreadpoolWorkCallbacks             | 内部实现。
| CreateThreadpoolTimer                      | 内部实现。
| CloseThreadpoolTimer                       | 调用DeleteTimerQueueTimer。
| SetThreadpoolTimer                         | 调用CreateTimerQueueTimer。
| WaitForThreadpoolTimerCallbacks            | 调用WaitForSingleObject。
| SetEventWhenCallbackReturns                | 内部实现。
| ReleaseSemaphoreWhenCallbackReturns        | 内部实现。
| ReleaseMutexWhenCallbackReturns            | 内部实现。
| LeaveCriticalSectionWhenCallbackReturns    | 内部实现。
| FreeLibraryWhenCallbackReturns             | 内部实现。
| CreateThreadpoolWait                       | 内部实现。
| CloseThreadpoolWait                        | 调用UnregisterWait。
| SetThreadpoolWait                          | 调用RegisterWaitForSingleObject。
| WaitForThreadpoolWaitCallbacks             | 调用WaitForSingleObject。
| CreateThreadpoolIo                         | 调用BindIoCompletionCallback。
| CloseThreadpoolIo                          | 内部实现。
| StartThreadpoolIo                          | 内部实现。
| CancelThreadpoolIo                         | 内部实现。
| WaitForThreadpoolIoCallbacks               | 调用WaitForSingleObject。
| CreateThreadpool                           | 内部实现。
| CloseThreadpool                            | 内部实现。
| SetThreadpoolThreadMaximum                 | 内部实现，自己控制最大并行数量。
| SetThreadpoolThreadMinimum                 | 忽略，并总是返回成功。
| CallbackMayRunLong                         | 自己估算系统剩余可用线程数。

## api-ms-win-core-winrt-l1-1-0.dll
| 函数                                       | Fallback
| ----                                       | -----------
| RoInitialize                               | 调用 CoInitializeEx。
| RoUninitialize                             | 调用 CoUninitialize。
| RoActivateInstance                         | 返回 E_NOTIMPL。
| RoRegisterActivationFactories              | 返回 E_NOTIMPL。
| RoRevokeActivationFactories                | 什么也不做。
| RoGetActivationFactory                     | 返回 CLASS_E_CLASSNOTAVAILABLE
| RoRegisterForApartmentShutdown             | 返回 E_NOTIMPL。
| RoUnregisterForApartmentShutdown           | 返回 E_NOTIMPL。
| RoGetApartmentIdentifier                   | 返回 E_NOTIMPL。

## api-ms-win-core-winrt-error-l1-1-0.dll
| 函数                                       | Fallback
| ----                                       | -----------
| RoFailFastWithErrorContext                 | 调用RaiseFailFastException。
| RoOriginateError                           | 根据error值返回TRUE或者FLASE。
| RoOriginateErrorW                          | 根据error值返回TRUE或者FLASE。
| RoOriginateLanguageException               | 调用RoOriginateError。
| RoTransformError                           | 根据oldError以及newError值返回TRUE或者FLASE。
| RoTransformErrorW                          | 根据oldError以及newError值返回TRUE或者FLASE。

## api-ms-win-core-winrt-string-l1-1-0.dll
| 函数                                       | Fallback
| ----                                       | -----------
| WindowsCreateString                        | 内部实现。
| WindowsCreateStringReference               | 内部实现。
| WindowsDeleteString                        | 内部实现。
| WindowsDuplicateString                     | 内部实现。
| WindowsGetStringLen                        | 内部实现。
| WindowsGetStringRawBuffer                  | 内部实现。
| WindowsIsStringEmpty                       | 内部实现。
| WindowsStringHasEmbeddedNull               | 内部实现。
| WindowsCompareStringOrdinal                | 内部实现。

## advapi32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| RegDeleteKeyExW(A)                         | 调用RegDeleteKeyW(A)。
| RegSetKeyValueW(A)                         | 调用RegCreateKeyExW(A)以及RegSetValueExW(A)。
| RegDeleteKeyValueW(A)                      | 调用RegOpenKeyExW(A)以及RegDeleteValueW(A)。
| RegDeleteTreeW(A)                          | 调用SHDeleteKeyW(A)。
| RegGetValueW(A)                            | 调用RegQueryValueExW(A)。
| RegCopyTreeW(A)                            | 调用SHCopyKeyW(A)。
| EventSetInformation                        | 返回ERROR_NOT_SUPPORTED。
| EventActivityIdControl                     | 返回ERROR_NOT_SUPPORTED。
| EventRegister                              | 返回ERROR_NOT_SUPPORTED。
| EventUnregister                            | 返回ERROR_NOT_SUPPORTED。
| EnumerateTraceGuidsEx                      | 返回ERROR_NOT_SUPPORTED。
| EventEnabled                               | 返回ERROR_NOT_SUPPORTED。
| EventWrite                                 | 返回ERROR_NOT_SUPPORTED。
| EventWriteTransfer                         | 返回ERROR_NOT_SUPPORTED。
| EventWriteEx                               | 调用EventWriteTransfer。
| EventWriteString                           | 返回ERROR_NOT_SUPPORTED。
| GetDynamicTimeZoneInformationEffectiveYears| 直接读取`Time Zones`注册表。
| AddMandatoryAce                            | 调用RtlCopySid。
| GetTokenInformation                        | 返回假装的 TokenVirtualizationAllowed、TokenAppContainerSid等。

## bcrypt.dll
| 函数                                       | Fallback
| ----                                       | -----------
| BCryptOpenAlgorithmProvider                | 调用CryptAcquireContextW。目前支持的算法有：RC2、RC4、AES、DES、3DES、3DES-112、MD2、MD4、MD5、SHA1、SHA256、SHA384、SHA512、RNG、FIPS186DSARNG、DUALECRNG。
| BCryptCloseAlgorithmProvider               | 调用CryptReleaseContext。
| BCryptGenRandom                            | 调用RtlGenRandom。
| BCryptGetProperty                          | 调用CryptGetKeyParam。
| BCryptSetProperty                          | 调用CryptSetKeyParam。
| BCryptCreateHash                           | 调用CryptCreateHash。
| BCryptDestroyHash                          | 调用CryptDestroyHash。
| BCryptHashData                             | 调用CryptHashData。
| BCryptFinishHash                           | 调用CryptGetHashParam。
| BCryptDeriveKeyPBKDF2                      | 调用BCryptHashData。
| BCryptDeriveKeyCapi                        | 调用BCryptHashData。
| BCryptEncrypt                              | 调用CryptEncrypt。
| BCryptDecrypt                              | 调用CryptDecrypt。
| BCryptGenerateSymmetricKey                 | 调用CryptImportKey。
| BCryptDestroyKey                           | 调用CryptDestroyKey。
| BCryptExportKey                            | 调用CryptExportKey。
| BCryptImportKey                            | 调用CryptImportKey。

## bcryptprimitives.dll
| 函数                                       | Fallback
| ----                                       | -----------
| ProcessPrng                                | 调用RtlGenRandom。

## bluetoothapis.dll
| 函数                                       | Fallback
| ----                                       | -----------
| BluetoothGATTGetCharacteristicValue        | 返回ERROR_NOT_SUPPORTED。
| BluetoothGATTGetCharacteristics            | 返回ERROR_NOT_SUPPORTED。
| BluetoothGATTGetDescriptors                | 返回ERROR_NOT_SUPPORTED。
| BluetoothGATTGetServices                   | 返回ERROR_NOT_SUPPORTED。
| BluetoothGATTRegisterEvent                 | 返回ERROR_NOT_SUPPORTED。
| BluetoothGATTSetCharacteristicValue        | 返回ERROR_NOT_SUPPORTED。
| BluetoothGATTSetDescriptorValue            | 返回ERROR_NOT_SUPPORTED。

## CfgMgr32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CM_Get_DevNode_Property_ExW                | 调用CM_Get_DevNode_Registry_PropertyW。
| CM_Set_DevNode_Property_ExW                | 调用CM_Set_DevNode_Registry_PropertyW。
| CM_Get_DevNode_PropertyW                   | 调用CM_Get_DevNode_Property_ExW。
| CM_Set_DevNode_PropertyW                   | 调用CM_Set_DevNode_Property_ExW。

## Crypt32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CryptProtectMemory                         | 返回TRUE。
| CryptUnprotectMemory                       | 返回TRUE。
| CryptBinaryToStringW(A)                    | 为Windows XP模拟 CRYPT_STRING_NOCRLF。

## d3d9.dll
| 函数                                       | Fallback
| ----                                       | -----------
| Direct3DCreate9Ex                          | 返回 `D3DERR_NOTAVAILABLE`。

## d3d11.dll
| 函数                                       | Fallback
| ----                                       | -----------
| D3D11CreateDevice                          | 返回 `E_NOINTERFACE`。
| CreateDirect3D11DeviceFromDXGIDevice       | 返回 `E_NOINTERFACE`。

## d3d12.dll
| 函数                                       | Fallback
| ----                                       | -----------
| D3D12CreateDevice                          | 返回 `E_NOINTERFACE`。

## DbgHelp.dll
| 函数                                       | Fallback
| ----                                       | -----------
| SymSetSearchPathW                          | 调用SymSetSearchPath。
| SymGetSearchPathW                          | 调用SymGetSearchPath。

## dwmapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| DwmEnableBlurBehindWindow                  | 返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmIsCompositionEnabled                    | 总是返回组合层已关闭。
| DwmEnableComposition                       | 如果尝试开启组合，那么返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用），其他情况返回 S_OK_。
| DwmExtendFrameIntoClientArea               | 返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmDefWindowProc                           | 返回 FALSE。
| DwmGetColorizationColor                    | 返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmGetWindowAttribute                      | 返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmSetWindowAttribute                      | 返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmFlush                                   | 返回 `S_OK_`。
| DwmGetCompositionTimingInfo                | 返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmInvalidateIconicBitmaps                 | 返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmSetIconicLivePreviewBitmap              | 返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmSetIconicThumbnail                      | 返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。


## dwrite.dll
| 函数                                       | Fallback
| ----                                       | -----------
| DWriteCreateFactory                        | 返回 `E_NOINTERFACE`。</br>此外NT6或者更高版本提供IDWriteFactory3模拟。

## dxgi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CreateDXGIFactory                          | 返回 `DXGI_ERROR_UNSUPPORTED`。
| CreateDXGIFactory1                         | 调用 CreateDXGIFactory。
| CreateDXGIFactory2                         | 调用 CreateDXGIFactory1。

## dxva2.dll
| 函数                                       | Fallback
| ----                                       | -----------
| DXVA2CreateVideoService                    | 返回 `E_NOINTERFACE`。

## esent.dll
| 函数                                       | Fallback
| ----                                       | -----------
| JetAddColumnA                              | 调用JetAddColumn。
| JetAddColumnA                              | 调用JetAddColumn。
| JetAttachDatabaseA                         | 调用JetAttachDatabase。
| JetAttachDatabase2A                        | 调用JetAttachDatabase2。
| JetAttachDatabaseWithStreamingA            | 调用JetAttachDatabaseWithStreaming。
| JetBackupA                                 | 调用JetBackup。
| JetBackupInstanceA                         | 调用JetBackupInstance。
| JetBeginSessionA                           | 调用JetBeginSession。
| JetCompactA                                | 调用JetCompact。
| JetConvertDDLA                             | 调用JetConvertDDL。
| JetCreateDatabaseA                         | 调用JetCreateDatabase。
| JetCreateDatabase2A                        | 调用JetCreateDatabase2。
| JetCreateDatabaseWithStreamingA            | 调用JetCreateDatabaseWithStreaming。
| JetCreateIndexA                            | 调用JetCreateIndex。
| JetCreateIndex2A                           | 调用JetCreateIndex2。
| JetCreateInstanceA                         | 调用JetCreateInstance。
| JetCreateInstance2A                        | 调用JetCreateInstance2。
| JetCreateTableA                            | 调用JetCreateTable。
| JetCreateTableColumnIndexA                 | 调用JetCreateTableColumnIndex。
| JetCreateTableColumnIndex2A                | 调用JetCreateTableColumnIndex2。
| JetDBUtilitiesA                            | 调用JetDBUtilities。
| JetDefragmentA                             | 调用JetDefragment。
| JetDefragment2A                            | 调用JetDefragment2。
| JetDeleteColumnA                           | 调用JetDeleteColumn。
| JetDeleteColumn2A                          | 调用JetDeleteColumn2。
| JetDeleteIndexA                            | 调用JetDeleteIndex。
| JetDeleteTableA                            | 调用JetDeleteTable。
| JetDetachDatabaseA                         | 调用JetDetachDatabase。
| JetDetachDatabase2A                        | 调用JetDetachDatabase2。
| JetEnableMultiInstanceA                    | 调用JetEnableMultiInstance。
| JetExternalRestoreA                        | 调用JetExternalRestore。
| JetExternalRestore2A                       | 调用JetExternalRestore2。
| JetGetAttachInfoA                          | 调用JetGetAttachInfo。
| JetGetAttachInfoInstanceA                  | 调用JetGetAttachInfoInstance。
| JetGetColumnInfoA                          | 调用JetGetColumnInfo。
| JetGetCurrentIndexA                        | 调用JetGetCurrentIndex。
| JetGetDatabaseFileInfoA                    | 调用JetGetDatabaseFileInfo。
| JetGetDatabaseInfoA                        | 调用JetGetDatabaseInfo。
| JetGetIndexInfoA                           | 调用JetGetIndexInfo。
| JetGetInstanceInfoA                        | 调用JetGetInstanceInfo。
| JetGetLogInfoA                             | 调用JetGetLogInfo。
| JetGetLogInfoInstanceA                     | 调用JetGetLogInfoInstance。
| JetGetLogInfoInstance2A                    | 调用JetGetLogInfoInstance2。
| JetGetObjectInfoA                          | 调用JetGetObjectInfo。
| JetGetSystemParameterA                     | 调用JetGetSystemParameter。
| JetGetTableColumnInfoA                     | 调用JetGetTableColumnInfo。
| JetGetTableIndexInfoA                      | 调用JetGetTableIndexInfo。
| JetGetTableInfoA                           | 调用JetGetTableInfo。
| JetGetTruncateLogInfoInstanceA             | 调用JetGetTruncateLogInfoInstance。
| JetInit3A                                  | 调用JetInit3。
| JetOpenDatabaseA                           | 调用JetOpenDatabase。
| JetOpenFileA                               | 调用JetOpenFile。
| JetOpenFileInstanceA                       | 调用JetOpenFileInstance。
| JetOpenFileSectionInstanceA                | 调用JetOpenFileSectionInstance。
| JetOpenTableA                              | 调用JetOpenTable。
| JetOSSnapshotFreezeA                       | 调用JetOSSnapshotFreeze。
| JetRenameColumnA                           | 调用JetRenameColumn。
| JetRenameTableA                            | 调用JetRenameTable。
| JetRestoreA                                | 调用JetRestore。
| JetRestore2A                               | 调用JetRestore2。
| JetRestoreInstanceA                        | 调用JetRestoreInstance。
| JetSetColumnDefaultValueA                  | 调用JetSetColumnDefaultValue。
| JetSetCurrentIndexA                        | 调用JetSetCurrentIndex。
| JetSetCurrentIndex2A                       | 调用JetSetCurrentIndex2。
| JetSetCurrentIndex3A                       | 调用JetSetCurrentIndex3。
| JetSetCurrentIndex4A                       | 调用JetSetCurrentIndex4。
| JetSetDatabaseSizeA                        | 调用JetSetDatabaseSize。
| JetSetSystemParameterA                     | 调用JetSetSystemParameter。
| JetSnapshotStartA                          | 调用JetSnapshotStart。
| JetUpgradeDatabaseA                        | 调用JetUpgradeDatabase。
| JetAttachDatabase2W                        | 调用JetAttachDatabase2A。
| JetBeginSessionW                           | 调用JetBeginSessionA。
| JetCreateInstanceW                         | 调用JetCreateInstanceA。
| JetGetTableColumnInfoW                     | 调用JetGetTableColumnInfoA。
| JetOpenDatabaseW                           | 调用JetOpenDatabaseA。
| JetOpenTableW                              | 调用JetOpenTableA。
| JetSetSystemParameterW                     | 调用JetSetSystemParameterA。
| JetGetSystemParameterW                     | 调用JetGetSystemParameterA。

## iphlpapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| GetIfTable2                                | 调用GetIfTable，并使用HeapAlloc申请内存。
| GetIfTable2Ex                              | 调用GetIfTable，并使用HeapAlloc申请内存。
| GetIfEntry2                                | 调用GetIfEntry。
| GetIfEntry2Ex                              | 调用GetIfEntry2。
| FreeMibTable                               | 调用HeapFree。
| ConvertInterfaceIndexToLuid                | 调用GetIfEntry。
| ConvertInterfaceLuidToNameW(A)             | 内部实现。
| ConvertInterfaceNameToLuidW(A)             | 内部实现。
| if_nametoindex                             | 调用GetIfEntry。
| if_indextoname                             | 调用ConvertInterfaceIndexToLuid、ConvertInterfaceLuidToNameA。
| ConvertInterfaceLuidToGuid                 | 调用GetIfEntry。
| ConvertInterfaceLuidToIndex                | 内部实现。
| * NotifyIpInterfaceChange                  | 什么也不做，假装成功。
| CancelMibChangeNotify2                     | 什么也不做，假装成功。

## kernel32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| DecodePointer                              | 返回指针本身。
| EncodePointer                              | 返回指针本身。
| Wow64DisableWow64FsRedirection             | 返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| Wow64RevertWow64FsRedirection              | 返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| Wow64EnableWow64FsRedirection              | 返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| IsWow64Process2                            | 调用IsWow64Process。
| IsWow64GuestMachineSupported               | 调用GetNativeSystemInfo。
| GetTickCount64                             | 调用GetTickCount。
| GetSystemTimePreciseAsFileTime             | 调用GetSystemTimeAsFileTime。
| InitializeCriticalSectionEx                | 调用InitializeCriticalSectionAndSpinCount。
| InitOnceInitialize                         | 初始化为 INIT_ONCE_STATIC_INIT。
| InitOnceBeginInitialize                    | 调用NtWaitForKeyedEvent。
| InitOnceComplete                           | 调用NtReleaseKeyedEvent。
| InitOnceExecuteOnce                        | 调用NtWaitForKeyedEvent以及NtReleaseKeyedEvent。
| LocaleNameToLCID                           | 查LocaleNameToLcidTable。
| LCIDToLocaleName                           | 查LcidToLocaleNameTable。
| GetLocaleInfoEx                            | 调用GetLocaleInfoW。
| GetDateFormatEx                            | 调用GetDateFormatW。
| GetTimeFormatEx                            | 调用GetTimeFormatW。
| GetNumberFormatEx                          | 调用GetNumberFormatW。
| GetCurrencyFormatEx                        | 调用GetCurrencyFormatW。
| GetUserDefaultLocaleName                   | 调用LCIDToLocaleName。
| GetSystemDefaultLocaleName                 | 调用LCIDToLocaleName。
| EnumCalendarInfoExEx                       | 调用EnumCalendarInfoExW。
| EnumDateFormatsExEx                        | 调用EnumDateFormatsExW。
| LCMapStringEx                              | 调用LCMapStringW。
| GetFileInformationByHandleEx               | 调用NtQueryInformationFile 或者 NtQueryDirectoryFile。
| SetFileInformationByHandle                 | 调用NtSetInformationFile。
| GetFinalPathNameByHandleW(A)               | 调用NtQueryObject以及NtQueryInformationFile。
| GetLogicalProcessorInformation             | 返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| GetLogicalProcessorInformationEx           | 调用GetLogicalProcessorInformation。
| GetNumaHighestNodeNumber                   | 返回0。
| RaiseFailFastException                     | 调用TerminateProcess。
| GetThreadId                                | 调用NtQueryInformationThread。
| GetProcessIdOfThread                       | 调用NtQueryInformationThread。
| GetProcessId                               | 调用NtQueryInformationProcess。
| QueryThreadCycleTime                       | 调用GetThreadTimes。
| QueryProcessCycleTime                      | 调用GetProcessTimes。
| K32EnumProcessModules                      | 调用EnumProcessModules。
| K32EnumProcessModulesEx                    | 调用EnumProcessModulesEx。
| K32GetModuleBaseNameW(A)                   | 调用GetModuleBaseNameW(A)。
| K32GetModuleFileNameExW(A)                 | 调用K32GetModuleFileNameExW(A)。
| K32EmptyWorkingSet                         | 调用EmptyWorkingSet。
| K32QueryWorkingSet                         | 调用QueryWorkingSet。
| K32QueryWorkingSetEx                       | 调用QueryWorkingSetEx。
| K32InitializeProcessForWsWatch             | 调用InitializeProcessForWsWatch。
| K32GetWsChanges                            | 调用GetWsChanges。
| K32GetWsChangesEx                          | 调用GetWsChangesEx。
| K32GetMappedFileNameW(A)                   | 调用GetMappedFileNameW(A)。
| K32EnumDeviceDrivers                       | 调用EnumDeviceDrivers。
| K32GetDeviceDriverBaseNameW(A)             | 调用GetDeviceDriverBaseNameW(A)。
| K32GetDeviceDriverFileNameW(A)             | 调用GetDeviceDriverFileNameW(A)。
| K32GetPerformanceInfo                      | 调用GetPerformanceInfo。
| K32EnumPageFilesW(A)                       | 调用EnumPageFilesW(A)。
| K32GetProcessImageFileNameW(A)             | 调用GetProcessImageFileNameW(A)。
| K32GetProcessMemoryInfo                    | 调用GetProcessMemoryInfo。
| K32EnumProcesses                           | 调用EnumProcesses。
| K32GetModuleInformation                    | 调用GetModuleInformation。
| QueryFullProcessImageNameW(A)              | 调用GetProcessImageFileNameW(A) 或者 GetModuleFileNameExW(A)。
| CreateFile2                                | 调用CreateFileW。
| CreateEventExW(A)                          | 调用CreateEventW(A)。
| CreateMutexExW(A)                          | 调用CreateMutexW(A)。
| CreateSemaphoreExW                         | 调用CreateSemaphoreW。
| CreateWaitableTimerExW                     | 调用CreateWaitableTimerW。
| InterlockedCompareExchange64               | 调用内部函数_InterlockedCompareExchange64。
| SetThreadErrorMode                         | 调用SetErrorMode。
| GetThreadErrorMode                         | 调用GetErrorMode。
| GetErrorMode                               | 调用NtQueryInformationProcess。
| InitializeSRWLock                          | 初始化为 RTL_SRWLOCK_INIT。
| AcquireSRWLockExclusive                    | 调用NtWaitForKeyedEvent。
| TryAcquireSRWLockExclusive                 | 调用InterlockedBitTestAndSet(64)。
| ReleaseSRWLockExclusive                    | 调用NtReleaseKeyedEvent。
| AcquireSRWLockShared                       | 调用NtWaitForKeyedEvent。
| TryAcquireSRWLockShared                    | 调用InterlockedCompareExchange。
| ReleaseSRWLockShared                       | 调用NtReleaseKeyedEvent。
| InitializeConditionVariable                | 初始化为 CONDITION_VARIABLE_INIT。
| SleepConditionVariableCS                   | 调用NtWaitForKeyedEvent。
| SleepConditionVariableSRW                  | 调用NtWaitForKeyedEvent。
| WakeConditionVariable                      | 调用NtReleaseKeyedEvent。
| WakeAllConditionVariable                   | 调用NtReleaseKeyedEvent。
| InitializeSynchronizationBarrier           | 调用CreateEvent。
| EnterSynchronizationBarrier                | 调用WaitForSingleObject。
| DeleteSynchronizationBarrier               | 调用CloseHandle。
| WaitOnAddress                              | 调用NtWaitForKeyedEvent。警告，此函数请勿跨模块使用！！！
| WakeByAddressSingle                        | 调用NtReleaseKeyedEvent。警告，此函数请勿跨模块使用！！！
| WakeByAddressAll                           | 调用NtReleaseKeyedEvent。警告，此函数请勿跨模块使用！！！
| GetCurrentProcessorNumber                  | 调用cpuid。
| GetCurrentProcessorNumberEx                | 调用GetCurrentProcessorNumber。
| GetNumaNodeProcessorMask                   | 假定所有CPU都在当前Numa。
| GetNumaNodeProcessorMaskEx                 | 调用GetNumaNodeProcessorMask。
| GetThreadGroupAffinity                     | 调用NtQueryInformationThread。
| SetThreadGroupAffinity                     | 调用SetThreadAffinityMask。
| *CancelIoEx                                | 调用CancelIo。警告，会把此句柄的所有IO操作取消掉！
| *CancelSynchronousIo                       | 仅返回失败。警告，实际无法取消！
| OpenFileById                               | 调用NtCreateFile。
| CreateSymbolicLinkW(A)                     | 返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| ReOpenFile                                 | 调用NtCreateFile。
| CompareStringEx                            | 调用CompareStringW。
| CompareStringOrdinal                       | 使用内置UnicodeCaseTableData实现。
| SetFilePointerEx                           | 调用SetFilePointer。
| GetModuleHandleExW(A)                      | 调用GetModuleHandleW(A)。
| WTSGetActiveConsoleSessionId               | 直接返回 0。
| GetNativeSystemInfo                        | 调用GetSystemInfo。
| InitializeSListHead                        | 直接初始化为 0。
| InterlockedFlushSList                      | 调用lock cmpxchg8b指令。
| QueryDepthSList                            | 直接返回Depth。
| InterlockedPushEntrySList                  | 调用lock cmpxchg8b指令。
| InterlockedPopEntrySList                   | 调用lock cmpxchg8b指令。
| GetNumaProximityNodeEx                     | 调用GetNumaProximityNode。
| GetNumaProcessorNode                       | 假定所有CPU都在节点 0。
| GetNumaNodeNumberFromHandle                | 假定所有CPU都在节点 0。
| GetNumaProcessorNodeEx                     | 调用 GetNumaProcessorNode。
| GetNumaAvailableMemoryNode                 | 假定所有内存都属于节点0。
| GetNumaAvailableMemoryNodeEx               | 调用 GetNumaAvailableMemoryNode。
| GetNumaProximityNode                       | 假定都是节点0。
| AllocateUserPhysicalPagesNuma              | 调用 AllocateUserPhysicalPages。
| MapViewOfFileExNuma                        | 调用 MapViewOfFileEx。
| VirtualAllocExNuma                         | 调用 VirtualAllocEx。
| CreateFileMappingNumaW(A)                  | 调用 CreateFileMappingW(A)。
| GetMaximumProcessorCount                   | 调用 GetSystemInfo。
| GetActiveProcessorCount                    | 调用 GetSystemInfo。
| GetActiveProcessorGroupCount               | 假定为1。
| GetMaximumProcessorGroupCount              | 假定为1。
| GetMemoryErrorHandlingCapabilities         | 直接报告不支持任何特性。
| VirtualAllocFromApp                        | 调用 VirtualAlloc。
| VirtualAlloc2                              | 调用 VirtualAllocExNuma 以及 VirtualAllocEx。
| VirtualAlloc2FromApp                       | 调用 VirtualAllocExNuma 以及 VirtualAllocEx。
| CreateFileMappingFromApp                   | 调用 CreateFileMappingW。
| CreateFileMapping2                         | 调用 CreateFileMappingNumaW 以及 CreateFileMappingW。
| MapViewOfFileFromApp                       | 调用 MapViewOfFile。
| UnmapViewOfFileEx                          | 调用 UnmapViewOfFile。
| VirtualProtectFromApp                      | 调用 VirtualProtect。
| OpenFileMappingFromApp                     | 调用 OpenFileMappingW。
| FlushProcessWriteBuffers                   | 调用VirtualProtect。
| FlsAlloc                                   | 使用Tls实现。警告，此函数请勿跨模块使用！！！
| FlsFree                                    | 使用Tls实现。警告，此函数请勿跨模块使用！！！
| FlsGetValue                                | 使用Tls实现。警告，此函数请勿跨模块使用！！！
| FlsSetValue                                | 使用Tls实现。警告，此函数请勿跨模块使用！！！
| IsThreadAFiber                             | 调用 GetCurrentFiber。
| ConvertThreadToFiberEx                     | 调用 ConvertThreadToFiber。
| GetDynamicTimeZoneInformation              | 调用 GetTimeZoneInformation。
| SetDynamicTimeZoneInformation              | 调用 SetTimeZoneInformation。
| GetProductInfo                             | 调用 GetVersionExW。
| EnumSystemLocalesEx                        | 调用 EnumSystemLocalesW。
| GetThreadPreferredUILanguages              | 调用 GetThreadLocale、GetUserDefaultLangID以及GetSystemDefaultLangID。
| GetThreadUILanguage                        | 调用 GetThreadLocale。
| ResolveLocaleName                          | 调用 LocaleNameToLCID以及LCIDToLocaleName。
| InitializeProcThreadAttributeList          | 内部实现。
| DeleteProcThreadAttributeList              | 内部实现。
| UpdateProcThreadAttribute                  | 内部实现。PROC_THREAD_ATTRIBUTE_PARENT_PROCESS与PROC_THREAD_ATTRIBUTE_HANDLE_LIST特性会被忽略处理。
| GetLargePageMinimum                        | 假定为0。
| SetThreadStackGuarantee                    | 调用VirtualAlloc。
| SetCoalescableTimer                        | 调用SetTimer。
| SetWaitableTimerEx                         | 调用SetWaitableTimer。
| EnumResourceLanguagesExW(A)                | 调用EnumResourceLanguagesW(A)。
| DiscardVirtualMemory                       | 调用VirtualAlloc MEM_RESET。
| OfferVirtualMemory                         | 返回ERROR_SUCCESS。
| ReclaimVirtualMemory                       | 返回ERROR_SUCCESS。
| PrefetchVirtualMemory                      | 返回ERROR_SUCCESS。
| GetProcessMitigationPolicy                 | 调用NtQueryInformationProcess。
| SetProcessMitigationPolicy                 | 调用NtSetInformationProcess。
| SetProcessInformation                      | 调用NtSetInformationProcess。
| GetThreadInformation                       | 调用NtQueryInformationThread。
| SetThreadInformation                       | 调用NtSetInformationThread。
| PowerCreateRequest                         | 内部实现。
| PowerSetRequest                            | 调用 SetThreadExecutionState。
| PowerClearRequest                          | 调用 SetThreadExecutionState。
| TzSpecificLocalTimeToSystemTime            | 内部实现。
| TzSpecificLocalTimeToSystemTimeEx          | 内部实现。
| GetFirmwareType                            | 调用NtQuerySystemInformation。
| IsNativeVhdBoot                            | 调用NtQuerySystemInformation。
| RtlCaptureStackBackTrace                   | 调用ntdll.RtlCaptureStackBackTrace。
| SetFileCompletionNotificationModes         | 什么也不做。
| GetQueuedCompletionStatusEx                | 调用 GetQueuedCompletionStatus。
| FindFirstFileEx(W/A)                       | Windows XP、Vista兼容 FIND_FIRST_EX_LARGE_FETCH、FindExInfoStandard参数。
| GetProcessGroupAffinity                    | 始终认为只有一组CPU。
| QueryUnbiasedInterruptTime                 | 读取KUSER_SHARED_DATA::InterruptTime值模拟UnbiasedInterruptTime。
| FindStringOrdinal                          | 调用CompareStringOrdinal。
| GetEnabledXStateFeatures                   | 调用IsProcessorFeaturePresent。
| SetXStateFeaturesMask                      | 内部实现。
| InitializeContext                          | 内部实现。
| InitializeContext2                         | 调用InitializeContext。
| LocateXStateFeature                        | 内部实现。
| CopyContext                                | 内部实现。
| QueryIdleProcessorCycleTimeEx              | 调用QueryIdleProcessorCycleTime。
| QueryIdleProcessorCycleTime                | 随便返回一组数字代表当前进程空闲时间。
| SetThreadIdealProcessorEx                  | 调用SetThreadIdealProcessor。
| GetThreadIdealProcessorEx                  | 调用SetThreadIdealProcessor。
| GetUserPreferredUILanguages                | 调用GetThreadPreferredUILanguages。
| EnumTimeFormatsEx                          | 调用EnumTimeFormatsW。
| GetCalendarInfoEx                          | 调用GetCalendarInfoW。
| GetNLSVersionEx                            | 返回一个假版本。
| IsNLSDefinedString                         | 调用GetStringTypeW。
| FindNLSStringEx                            | 调用 CompareStringW。
| SetProcessWorkingSetSizeEx                 | 调用SetProcessWorkingSetSize。
| GetProcessWorkingSetSizeEx                 | 调用GetProcessWorkingSetSize。
| GetTimeZoneInformationForYear              | 直接读取`Time Zones`注册表。
| SetProcessDEPPolicy                        | 调用NtSetInformationProcess。
| GetSystemDEPPolicy                         | 返回总是关闭。
| DisableThreadLibraryCalls                  | 始终返回成功。
| CreateRemoteThreadEx                       | 调用CreateRemoteThread。
| WerRegisterRuntimeExceptionModule          | 返回S_OK。
| WerUnregisterRuntimeExceptionModule        | 返回S_OK。
| Wow64GetThreadContext                      | 调用GetThreadContext或者返回ERROR_INVALID_PARAMETER。
| SetDefaultDllDirectories                   | 手工控制LoadLibrary加载顺序。
| SetDllDirectoryW(A)                        | 内部保存路径，并且控制LoadLibrary加载顺序。
| GetDllDirectoryW(A)                        | 获取内部保存的路径。
| AddDllDirectory                            | 内部保存路径，并且控制LoadLibrary加载顺序。
| RemoveDllDirectory                         | 内部移除路径。
| GetCurrentPackageFullName                  | 返回 APPMODEL_ERROR_NO_PACKAGE。
| OpenProcess                                | 额外处理 PROCESS_QUERY_LIMITED_INFORMATION、PROCESS_SET_LIMITED_INFORMATION。
| GetThreadDescription                       | 返回空字符串。
| SetThreadDescription                       | 返回 `E_NOTIMPL`。
| GetSystemFirmwareTable                     | 读取PhysicalMemory或者注册表。注意：目前仅支持'RSMB'、'ACPI'。
| GetPhysicallyInstalledSystemMemory         | 调用GetSystemFirmwareTable。

## mfplat.dll
| 函数                                       | Fallback
| ----                                       | -----------
| MFCreateDeviceSource                       | 返回E_NOTIMPL。
| MFEnumDeviceSources                        | 返回E_NOTIMPL。
| MFCreateDXGIDeviceManager                  | 返回E_NOTIMPL。
| MFCreateDXGISurfaceBuffer                  | 返回E_NOTIMPL。
| MFLockDXGIDeviceManager                    | 返回E_NOTIMPL。
| MFUnlockDXGIDeviceManager                  | 返回E_NOTIMPL。
| MFCreateAlignedMemoryBuffer                | 返回E_NOTIMPL。
| MFCreateAsyncResult                        | 返回E_NOTIMPL。
| MFCreateAttributes                         | 返回E_NOTIMPL。
| MFCreateEventQueue                         | 返回E_NOTIMPL。
| MFCreateMediaBufferWrapper                 | 返回E_NOTIMPL。
| MFCreateMediaEvent                         | 返回E_NOTIMPL。
| MFCreateMediaType                          | 返回E_NOTIMPL。
| MFCreateMemoryBuffer                       | 返回E_NOTIMPL。
| MFCreatePresentationDescriptor             | 返回E_NOTIMPL。
| MFCreateSample                             | 返回E_NOTIMPL。
| MFCreateStreamDescriptor                   | 返回E_NOTIMPL。
| MFCreateWaveFormatExFromMFMediaType        | 返回E_NOTIMPL。
| MFFrameRateToAverageTimePerFrame           | 返回E_NOTIMPL。
| MFGetSystemTime                            | 调用GetSystemTimeAsFileTime。
| MFInitMediaTypeFromWaveFormatEx            | 返回E_NOTIMPL。
| MFShutdown                                 | 返回E_NOTIMPL。
| MFStartup                                  | 返回E_NOTIMPL。
| MFTEnumEx                                  | 返回E_NOTIMPL。
| MFCancelWorkItem                           | 返回E_NOTIMPL。
| MFLockSharedWorkQueue                      | 返回E_NOTIMPL。
| MFPutWorkItem                              | 返回E_NOTIMPL。
| MFPutWorkItem2                             | 调用MFPutWorkItem。
| MFPutWaitingWorkItem                       | 返回E_NOTIMPL。
| MFUnlockWorkQueue                          | 返回E_NOTIMPL。

## mfreadwrite.dll
| 函数                                       | Fallback
| ----                                       | -----------
| MFCreateSourceReaderFromMediaSource        | 返回E_NOTIMPL。

## mmdevapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| ActivateAudioInterfaceAsync                | 返回E_NOTIMPL。

## ndfapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| NdfCreateWebIncident                       | 返回一个伪句柄假装成功。
| NdfCloseIncident                           | 假装成功关闭句柄。
| NdfExecuteDiagnosis                        | 假装什么问题也没有发现。

## netapi32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| NetGetAadJoinInformation                   | 始终认为没有加入 Azure AD 帐户 账号。
| NetFreeAadJoinInformation                  | 什么也不做。

## ntdll.dll
| 函数                                       | Fallback
| ----                                       | -----------
| * NtCancelIoFileEx                         | 尝试对所有线程调用 NtCancelIoFile。警告：将取消此文件的所有IO请求。
| NtOpenKeyEx                                | 调用 NtOpenKey 或者 NtCreateKey。

## ole32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CoGetApartmentType                         | 调用IComThreadingInfo。
| RoGetAgileReference                        | 返回E_NOINTERFACE。

## pdh.dll
| 函数                                       | Fallback
| ----                                       | -----------
| PdhAddEnglishCounterW(A)                   | 调用PdhAddCounterW(A)。

## powrprof.dll
| 函数                                       | Fallback
| ----                                       | -----------
| PowerDeterminePlatformRole                 | 返回PlatformRoleDesktop。
| PowerDeterminePlatformRoleEx               | 调用PowerDeterminePlatformRole。
| PowerRegisterSuspendResumeNotification     | 使用窗口模拟。
| PowerUnregisterSuspendResumeNotification   | 内部实现。
| PowerGetActiveScheme                       | 始终认为是平衡模式。
| PowerReadACValue                           | 读取注册表。（目前仅支持ConsoleLock）
| PowerReadDCValue                           | 读取注册表。（目前仅支持ConsoleLock）

## PropSys.dll
| 函数                                       | Fallback
| ----                                       | -----------
| InitPropVariantFromCLSID                   | CoTaskMemAlloc分配内存。
| PSGetPropertyKeyFromName                   | 返回 TYPE_E_ELEMENTNOTFOUND（属性不存在）。
| PSCreateMemoryPropertyStore                | 返回 E_NOTIMPL。
| VariantCompare                             | 内部实现。

## psapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| EnumProcessModulesEx                       | 调用EnumProcessModules。
| GetWsChangesEx                             | 调用GetWsChanges。
| *QueryWorkingSetEx                         | 返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。

## setupapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| SetupDiGetDevicePropertyW                  | 调用SetupDiGetDeviceRegistryPropertyW。
| SetupDiSetDevicePropertyW                  | 调用SetupDiSetDeviceRegistryPropertyW。
| SetupDiGetClassPropertyW                   | 调用SetupDiGetClassRegistryPropertyW。
| SetupDiGetClassPropertyExW                 | 调用SetupDiGetClassRegistryPropertyW。
| SetupDiSetClassPropertyW                   | 调用SetupDiSetClassRegistryPropertyW。
| SetupDiSetClassPropertyExW                 | 调用SetupDiSetClassRegistryPropertyW。

## shcore.dll
| 函数                                       | Fallback
| ----                                       | -----------
| GetDpiForMonitor                           | 调用GetDeviceCaps。
| GetProcessDpiAwareness                     | 调用 IsProcessDPIAware。
| SetProcessDpiAwareness                     | 调用SetProcessDPIAware。
| SetProcessDPIAware                         | 直接返回 TRUE。
| CreateRandomAccessStreamOnFile             | 返回 E_NOTIMPL。
| CreateRandomAccessStreamOverStream         | 返回 E_NOTIMPL。
| CreateStreamOverRandomAccessStream         | 返回 E_NOTIMPL。

## shell32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| SHGetKnownFolderPath                       | 调用SHGetFolderPathW。
| SHSetKnownFolderPath                       | 调用SHSetFolderPathW。
| SHGetKnownFolderIDList                     | 调用SHGetFolderLocation。
| SHBindToFolderIDListParent                 | 调用IShellFolder。
| SHBindToFolderIDListParentEx               | 调用IShellFolder。
| SHBindToObject                             | 调用IShellFolder。
| SHCreateItemFromIDList                     | 调用IShellItem。
| SHCreateItemWithParent                     | 调用IShellItem。
| SHCreateItemFromRelativeName               | 调用IShellItem。
| SHGetNameFromIDList                        | 调用IShellItem。
| SHCreateShellItem                          | 调用IShellItem。
| SHCreateItemFromParsingName                | 调用SHParseDisplayName。
| Shell_NotifyIconGetRect                    | 调用SendMessageW（可能不适用于Vista系统）。
| SHGetStockIconInfo                         | 调用LoadImageW。
| SHGetPropertyStoreForWindow                | 报告错误 E_NOTIMPL。
| SHOpenWithDialog                           | 报告错误 E_NOTIMPL。

## shlwapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| StrToInt64ExW(A)                           | 手工解析字符串。

## UIAutomationCore.dll
| 函数                                       | Fallback
| ----                                       | -----------
| UiaClientsAreListening                     | 假装没有人在监听。
| UiaHostProviderFromHwnd                    | 报告错误 E_NOTIMPL。
| UiaRaiseAutomationEvent                    | 报告错误 E_NOTIMPL。
| UiaRaiseAutomationPropertyChangedEvent     | 报告错误 E_NOTIMPL。
| UiaReturnRawElementProvider                | 报告错误 E_NOTIMPL。
| UiaGetReservedMixedAttributeValue          | 报告错误 E_NOTIMPL。
| UiaGetReservedNotSupportedValue            | 报告错误 E_NOTIMPL。
| UiaRaiseStructureChangedEvent              | 报告错误 E_NOTIMPL。
| UiaRaiseNotificationEvent                  | 假装成功。
| UiaLookupId                                | 始终返回 0。

## user32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| IsWow64Process                             | 返回TRUE，并设置 `*Wow64Process = FALSE`。
| SetProcessDpiAwarenessContext              | 调用SetProcessDpiAwareness。
| GetDpiForSystem                            | 调用GetDeviceCaps。
| GetDpiForWindow                            | 调用GetDpiForMonitor。
| GetSystemMetricsForDpi                     | 调用GetSystemMetrics。
| AdjustWindowRectExForDpi                   | 调用AdjustWindowRectEx。
| SystemParametersInfoW(A)                   | SPI_GETNONCLIENTMETRICS修正。
| SystemParametersInfoForDpi                 | 调用SystemParametersInfoW。
| RegisterSuspendResumeNotification          | 使用窗口模拟。
| UnregisterSuspendResumeNotification        | 内部实现。
| IsProcessDPIAware                          | 返回 FALSE。
| SetProcessDPIAware                         | 什么都不做，假装成功。
| GetWindowDisplayAffinity                   | TRUE，并报告窗口没有任何保护`WDA_NONE`。
| SetWindowDisplayAffinity                   | 什么都不做，假装成功。
| RegisterTouchWindow                        | 什么都不做，假装成功。
| UnregisterTouchWindow                      | 什么都不做，假装成功。
| IsTouchWindow                              | 始终报告非触摸窗口。
| GetTouchInputInfo                          | 报告错误 ERROR_INVALID_HANDLE。
| CloseTouchInputHandle                      | 报告错误 ERROR_INVALID_HANDLE。
| *ChangeWindowMessageFilterEx               | 调用ChangeWindowMessageFilter。温馨提示：将影响该进程的所有窗口。
| ChangeWindowMessageFilter                  | 什么都不做，假装成功。
| UpdateLayeredWindowIndirect                | 调用UpdateLayeredWindow。
| AddClipboardFormatListener                 | 什么都不做，假装成功。
| RemoveClipboardFormatListener              | 什么都不做，假装成功。
| RegisterPowerSettingNotification           | 什么都不做，假装成功。
| UnregisterPowerSettingNotification         | 什么都不做，假装成功。
| DisplayConfigGetDeviceInfo                 | 报告没有安装驱动。
| GetDisplayConfigBufferSizes                | 报告没有安装驱动。
| QueryDisplayConfig                         | 报告没有安装驱动。
| RegisterPointerDeviceNotifications         | 假装成功。
| GetPointerDevices                          | 假装没有触摸设备。
| GetPointerDevice                           | 假装没有触摸设备。
| GetPointerPenInfo                          | 假装没有触摸设备。
| GetPointerType                             | 假装没有触摸设备。
| InitializeTouchInjection                   | 报告错误 ERROR_INVALID_PARAMETER。
| InjectTouchInput                           | 报告错误 ERROR_INVALID_PARAMETER。
| GetAwarenessFromDpiAwarenessContext        | 内部实现。
| AreDpiAwarenessContextsEqual               | 内部实现。
| EnableNonClientDpiScaling                  | 假装成功。
| GetPointerFrameTouchInfo                   | 报告错误 ERROR_INVALID_PARAMETER。
| GetPointerFrameTouchInfoHistory            | 报告错误 ERROR_INVALID_PARAMETER。
| GetPointerInfo                             | 报告错误 ERROR_INVALID_PARAMETER。
| GetPointerPenInfoHistory                   | 报告错误 ERROR_INVALID_PARAMETER。
| SkipPointerFrameMessages                   | 假装成功。
| GetThreadDpiAwarenessContext               | 调用 GetProcessDpiAwareness。
| GetWindowDpiAwarenessContext               | 调用 GetProcessDpiAwareness。
| GetDisplayAutoRotationPreferences          | 返回 ORIENTATION_PREFERENCE_NONE。
| SetDisplayAutoRotationPreferences          | 假装成功。
| GetPointerInfoHistory                      | 报告错误 ERROR_INVALID_PARAMETER。
| GetPointerTouchInfo                        | 报告错误 ERROR_INVALID_PARAMETER。
| GetPointerTouchInfoHistory                 | 报告错误 ERROR_INVALID_PARAMETER。
| IsMouseInPointerEnabled                    | 返回关闭。
| EnableMouseInPointer                       | 假装处于关闭状态。
| GetPointerDeviceRects                      | 报告错误 ERROR_INVALID_PARAMETER。

## userenv.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CreateAppContainerProfile                  | 返回E_NOTIMPL。
| DeleteAppContainerProfile                  | 返回E_NOTIMPL。
| DeriveAppContainerSidFromAppContainerName  | 返回E_NOTIMPL。
| GetAppContainerFolderPath                  | 返回E_NOTIMPL。
| GetAppContainerRegistryLocation            | 返回E_NOTIMPL。

## uxtheme.dll
| 函数                                       | Fallback
| ----                                       | -----------
| DrawThemeTextEx                            | 尝试获取非导出DrawThemeTextEx。如果任然获取失败则调用DrawThemeText。
| GetThemeTransitionDuration                 | 返回E_NOTIMPL。
| SetWindowThemeAttribute                    | 返回E_NOTIMPL。

## version.dll
| 函数                                       | Fallback
| ----                                       | -----------
| GetFileVersionInfoExW(A)                   | 调用GetFileVersionInfoW(A)。
| GetFileVersionInfoSizeExW(A)               | 调用GetFileVersionInfoSizeW(A)。

## wevtapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| EvtClose                                   | 返回TRUE。
| EvtCreateRenderContext                     | 报告ERROR_NOT_SUPPORTED。
| EvtNext                                    | 报告ERROR_NOT_SUPPORTED。
| EvtQuery                                   | 报告ERROR_NOT_SUPPORTED。
| EvtRender                                  | 报告ERROR_NOT_SUPPORTED。

## WinHttp.dll
| 函数                                       | Fallback
| ----                                       | -----------
| WinHttpCreateProxyResolver                 | 内部实现。
| WinHttpGetProxyForUrlEx                    | 异步调用WinHttpGetProxyForUrl。
| WinHttpGetProxyResult                      | 内部实现。
| WinHttpFreeProxyResult                     | 内部实现。

## WinUsb.dll
| 函数                                       | Fallback
| ----                                       | -----------
| WinUsb_Free                                | 报告ERROR_INVALID_HANDLE。
| WinUsb_GetAssociatedInterface              | 报告ERROR_INVALID_HANDLE。
| WinUsb_GetOverlappedResult                 | 报告ERROR_INVALID_HANDLE。
| WinUsb_Initialize                          | 报告ERROR_INVALID_HANDLE。
| WinUsb_ReadPipe                            | 报告ERROR_INVALID_HANDLE。
| WinUsb_ResetPipe                           | 报告ERROR_INVALID_HANDLE。
| WinUsb_SetCurrentAlternateSetting          | 报告ERROR_INVALID_HANDLE。
| WinUsb_WritePipe                           | 报告ERROR_INVALID_HANDLE。

## ws2_32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| InetPtonW(inet_pton)                       | 类似于sscanf手工分析字符串。
| InetNtopW(inet_ntop)                       | 类似于sprintf手工生成字符串。
| WSAPoll                                    | 调用select。
| GetAddrInfoExCancel                        | 使用线程模拟。
| GetAddrInfoExW(A)                          | 调用GetAddrInfoW(A)。
| GetAddrInfoExOverlappedResult              | 内部实现。
| FreeAddrInfoEx(W)                          | 内部实现。
| GetAddrInfoW                               | 调用getaddrinfo。
| FreeAddrInfoW                              | 内部实现。
| WSASocketW(A)                              | 低于6.1.7601时自动去除 `WSA_FLAG_NO_HANDLE_INHERIT`。
| WSAIoctl                                   | 低于6.0时，`SIO_BASE_HANDLE` 代码返回SOCKET自身。
