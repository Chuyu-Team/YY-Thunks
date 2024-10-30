# YY-Thunks Thunks 清单

此表展示了YY-Thunks（鸭船）可以解决的函数不存在问题，欢迎大家扩充！

> 开头带`*`的函数并不建议使用，存在一些较大负面影响，仅用于编译通过处理，具体负面影响可参考注释内容。

## api-ms-win-core-handle-l1-1-0.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CompareObjectHandles                       | 不存在时，调用NtQueryObject以及DuplicateHandle。

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
| PathCchCanonicalizeEx                      | 不存在时，内部实现。
| PathCchCanonicalize                        | 调用PathCchCanonicalizeEx。
| PathCchCombineEx                           | 内部实现。
| PathCchCombine                             | 调用PathCchCombineEx。
| PathCchAppendEx                            | 调用PathCchCombineEx。
| PathCchAppend                              | 调用PathCchAppendEx。
| PathCchStripPrefix                         | 内部实现。
| PathAllocCombine                           | 不存在时，调用PathCchCombineEx。
| PathAllocCanonicalize                      | 不存在时，调用PathCchCanonicalizeEx。

## api-ms-win-core-realtime-l1-1-1.dll
| 函数                                       | Fallback
| ----                                       | -----------
| QueryUnbiasedInterruptTimePrecise          | 不存在时，调用QueryUnbiasedInterruptTime。
| QueryInterruptTime                         | 不存在时，读取KUSER_SHARED_DATA::InterruptTime值。
| QueryInterruptTimePrecise                  | 不存在时，读取KUSER_SHARED_DATA::InterruptTime值。

## api-ms-win-core-threadpool-l1-2-0.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CreateThreadpoolWork                       | 不存在时，内部实现。
| CloseThreadpoolWork                        | 不存在时，内部实现。
| TrySubmitThreadpoolCallback                | 不存在时，调用QueueUserWorkItem。
| SubmitThreadpoolWork                       | 不存在时，调用QueueUserWorkItem。
| WaitForThreadpoolWorkCallbacks             | 不存在时，内部实现。
| CreateThreadpoolTimer                      | 不存在时，内部实现。
| CloseThreadpoolTimer                       | 不存在时，调用DeleteTimerQueueTimer。
| SetThreadpoolTimer                         | 不存在时，调用CreateTimerQueueTimer。
| WaitForThreadpoolTimerCallbacks            | 不存在时，调用WaitForSingleObject。
| SetEventWhenCallbackReturns                | 不存在时，内部实现。
| ReleaseSemaphoreWhenCallbackReturns        | 不存在时，内部实现。
| ReleaseMutexWhenCallbackReturns            | 不存在时，内部实现。
| LeaveCriticalSectionWhenCallbackReturns    | 不存在时，内部实现。
| FreeLibraryWhenCallbackReturns             | 不存在时，内部实现。
| CreateThreadpoolWait                       | 不存在时，内部实现。
| CloseThreadpoolWait                        | 不存在时，调用UnregisterWait。
| SetThreadpoolWait                          | 不存在时，调用RegisterWaitForSingleObject。
| WaitForThreadpoolWaitCallbacks             | 不存在时，调用WaitForSingleObject。
| CreateThreadpoolIo                         | 不存在时，调用BindIoCompletionCallback。
| CloseThreadpoolIo                          | 不存在时，内部实现。
| StartThreadpoolIo                          | 不存在时，内部实现。
| CancelThreadpoolIo                         | 不存在时，内部实现。
| WaitForThreadpoolIoCallbacks               | 不存在时，调用WaitForSingleObject。
| CreateThreadpool                           | 不存在时，内部实现。
| CloseThreadpool                            | 不存在时，内部实现。
| SetThreadpoolThreadMaximum                 | 不存在时，内部实现，自己控制最大并行数量。
| SetThreadpoolThreadMinimum                 | 不存在时，忽略，并总是返回成功。
| CallbackMayRunLong                         | 不存在时，自己估算系统剩余可用线程数。

## api-ms-win-core-winrt-l1-1-0.dll
| 函数                                       | Fallback
| ----                                       | -----------
| RoInitialize                               | 不存在时，调用 CoInitializeEx。
| RoUninitialize                             | 不存在时，调用 CoUninitialize。
| RoActivateInstance                         | 不存在时，返回 E_NOTIMPL。
| RoRegisterActivationFactories              | 不存在时，返回 E_NOTIMPL。
| RoRevokeActivationFactories                | 不存在时，什么也不做。
| RoGetActivationFactory                     | 不存在时，返回 CLASS_E_CLASSNOTAVAILABLE
| RoRegisterForApartmentShutdown             | 不存在时，返回 E_NOTIMPL。
| RoUnregisterForApartmentShutdown           | 不存在时，返回 E_NOTIMPL。
| RoGetApartmentIdentifier                   | 不存在时，返回 E_NOTIMPL。

## api-ms-win-core-winrt-error-l1-1-0.dll
| 函数                                       | Fallback
| ----                                       | -----------
| RoOriginateError                           | 不存在时，返回 TRUE.
| RoOriginateErrorW                          | 不存在时，返回 TRUE.

## api-ms-win-core-winrt-string-l1-1-0.dll
| 函数                                       | Fallback
| ----                                       | -----------
| WindowsCreateString                        | 不存在时，内部实现。
| WindowsCreateStringReference               | 不存在时，内部实现。
| WindowsDeleteString                        | 不存在时，内部实现。
| WindowsDuplicateString                     | 不存在时，内部实现。
| WindowsGetStringLen                        | 不存在时，内部实现。
| WindowsGetStringRawBuffer                  | 不存在时，内部实现。
| WindowsIsStringEmpty                       | 不存在时，内部实现。
| WindowsStringHasEmbeddedNull               | 不存在时，内部实现。
| WindowsCompareStringOrdinal                | 不存在时，内部实现。

## advapi32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| RegDeleteKeyExW(A)                         | 不存在时，调用RegDeleteKeyW(A)。
| RegSetKeyValueW(A)                         | 调用RegCreateKeyExW(A)以及RegSetValueExW(A)。
| RegDeleteKeyValueW(A)                      | 调用RegOpenKeyExW(A)以及RegDeleteValueW(A)。
| RegDeleteTreeW(A)                          | 调用SHDeleteKeyW(A)。
| RegGetValueW(A)                            | 不存在时，调用RegQueryValueExW(A)。
| RegCopyTreeW(A)                            | 不存在时，调用SHCopyKeyW(A)。
| EventSetInformation                        | 不存在时，返回ERROR_NOT_SUPPORTED。
| EventActivityIdControl                     | 不存在时，返回ERROR_NOT_SUPPORTED。
| EventRegister                              | 不存在时，返回ERROR_NOT_SUPPORTED。
| EventUnregister                            | 不存在时，返回ERROR_NOT_SUPPORTED。
| EnumerateTraceGuidsEx                      | 不存在时，返回ERROR_NOT_SUPPORTED。
| EventEnabled                               | 不存在时，返回ERROR_NOT_SUPPORTED。
| EventWrite                                 | 不存在时，返回ERROR_NOT_SUPPORTED。
| EventWriteTransfer                         | 不存在时，返回ERROR_NOT_SUPPORTED。
| EventWriteEx                               | 不存在时，调用EventWriteTransfer。
| EventWriteString                           | 不存在时，返回ERROR_NOT_SUPPORTED。
| GetDynamicTimeZoneInformationEffectiveYears| 不存在时，直接读取`Time Zones`注册表。
| AddMandatoryAce                            | 不存在时，调用RtlCopySid。
| GetTokenInformation                        | 返回假装的 TokenVirtualizationAllowed、TokenAppContainerSid等。

## bcrypt.dll
| 函数                                       | Fallback
| ----                                       | -----------
| BCryptOpenAlgorithmProvider                | 不存在时，调用CryptAcquireContextW。目前支持的算法有：RC2、RC4、AES、DES、3DES、3DES-112、MD2、MD4、MD5、SHA1、SHA256、SHA384、SHA512、RNG、FIPS186DSARNG、DUALECRNG。
| BCryptCloseAlgorithmProvider               | 不存在时，调用CryptReleaseContext。
| BCryptGenRandom                            | 不存在时，调用RtlGenRandom。
| BCryptGetProperty                          | 不存在时，调用CryptGetKeyParam。
| BCryptSetProperty                          | 不存在时，调用CryptSetKeyParam。
| BCryptCreateHash                           | 不存在时，调用CryptCreateHash。
| BCryptDestroyHash                          | 不存在时，调用CryptDestroyHash。
| BCryptHashData                             | 不存在时，调用CryptHashData。
| BCryptFinishHash                           | 不存在时，调用CryptGetHashParam。
| BCryptDeriveKeyPBKDF2                      | 不存在时，调用BCryptHashData。
| BCryptDeriveKeyCapi                        | 不存在时，调用BCryptHashData。
| BCryptEncrypt                              | 不存在时，调用CryptEncrypt。
| BCryptDecrypt                              | 不存在时，调用CryptDecrypt。
| BCryptGenerateSymmetricKey                 | 不存在时，调用CryptImportKey。
| BCryptDestroyKey                           | 不存在时，调用CryptDestroyKey。
| BCryptExportKey                            | 不存在时，调用CryptExportKey。
| BCryptImportKey                            | 不存在时，调用CryptImportKey。

## bcryptprimitives.dll
| 函数                                       | Fallback
| ----                                       | -----------
| ProcessPrng                                | 不存在时调用，RtlGenRandom。

## bluetoothapis.dll
| 函数                                       | Fallback
| ----                                       | -----------
| BluetoothGATTGetCharacteristicValue        | 不存在时，返回ERROR_NOT_SUPPORTED。
| BluetoothGATTGetCharacteristics            | 不存在时，返回ERROR_NOT_SUPPORTED。
| BluetoothGATTGetDescriptors                | 不存在时，返回ERROR_NOT_SUPPORTED。
| BluetoothGATTGetServices                   | 不存在时，返回ERROR_NOT_SUPPORTED。
| BluetoothGATTRegisterEvent                 | 不存在时，返回ERROR_NOT_SUPPORTED。
| BluetoothGATTSetCharacteristicValue        | 不存在时，返回ERROR_NOT_SUPPORTED。
| BluetoothGATTSetDescriptorValue            | 不存在时，返回ERROR_NOT_SUPPORTED。

## CfgMgr32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CM_Get_DevNode_Property_ExW                | 不存在时，调用CM_Get_DevNode_Registry_PropertyW。
| CM_Set_DevNode_Property_ExW                | 不存在时，调用CM_Set_DevNode_Registry_PropertyW。
| CM_Get_DevNode_PropertyW                   | 不存在时，调用CM_Get_DevNode_Property_ExW。
| CM_Set_DevNode_PropertyW                   | 不存在时，调用CM_Set_DevNode_Property_ExW。

## Crypt32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CryptProtectMemory                         | 不存在时，返回TRUE。
| CryptUnprotectMemory                       | 不存在时，返回TRUE。
| CryptBinaryToStringW(A)                    | 为Windows XP模拟 CRYPT_STRING_NOCRLF。

## d3d9.dll
| 函数                                       | Fallback
| ----                                       | -----------
| Direct3DCreate9Ex                          | 不存在时，返回 `D3DERR_NOTAVAILABLE`。

## d3d11.dll
| 函数                                       | Fallback
| ----                                       | -----------
| D3D11CreateDevice                          | 不存在时，返回 `E_NOINTERFACE`。

## d3d12.dll
| 函数                                       | Fallback
| ----                                       | -----------
| D3D12CreateDevice                          | 不存在时，返回 `E_NOINTERFACE`。

## DbgHelp.dll
| 函数                                       | Fallback
| ----                                       | -----------
| SymSetSearchPathW                          | 不存在时，调用SymSetSearchPath。
| SymGetSearchPathW                          | 不存在时，调用SymGetSearchPath。

## dwmapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| DwmEnableBlurBehindWindow                  | 不存在时，返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmIsCompositionEnabled                    | 不存在时，总是返回组合层已关闭。
| DwmEnableComposition                       | 不存在时，如果尝试开启组合，那么返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用），其他情况返回 S_OK_。
| DwmExtendFrameIntoClientArea               | 不存在时，返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmDefWindowProc                           | 不存在时，返回 FALSE。
| DwmGetColorizationColor                    | 不存在时，返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmGetWindowAttribute                      | 不存在时，返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmSetWindowAttribute                      | 不存在时，返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmFlush                                   | 不存在时，返回 `S_OK_`。
| DwmGetCompositionTimingInfo                | 不存在时，返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmInvalidateIconicBitmaps                 | 不存在时，返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmSetIconicLivePreviewBitmap              | 不存在时，返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。
| DwmSetIconicThumbnail                      | 不存在时，返回 `DWM_E_COMPOSITIONDISABLED`（表示DWM已禁用）。


## dwrite.dll
| 函数                                       | Fallback
| ----                                       | -----------
| DWriteCreateFactory                        | 不存在时，返回 `E_NOINTERFACE`。</br>此外NT6或者更高版本提供IDWriteFactory3模拟。

## dxgi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CreateDXGIFactory                          | 返回 `DXGI_ERROR_UNSUPPORTED`。
| CreateDXGIFactory1                         | 调用 CreateDXGIFactory。
| CreateDXGIFactory2                         | 调用 CreateDXGIFactory1。

## dxva2.dll
| 函数                                       | Fallback
| ----                                       | -----------
| DXVA2CreateVideoService                    | 不存在时，返回 `E_NOINTERFACE`。

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
| JetAttachDatabase2W                        | 不存在时，调用JetAttachDatabase2A。
| JetBeginSessionW                           | 不存在时，调用JetBeginSessionA。
| JetCreateInstanceW                         | 不存在时，调用JetCreateInstanceA。
| JetGetTableColumnInfoW                     | 不存在时，调用JetGetTableColumnInfoA。
| JetOpenDatabaseW                           | 不存在时，调用JetOpenDatabaseA。
| JetOpenTableW                              | 不存在时，调用JetOpenTableA。
| JetSetSystemParameterW                     | 不存在时，调用JetSetSystemParameterA。
| JetGetSystemParameterW                     | 不存在时，调用JetGetSystemParameterA。

## iphlpapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| GetIfTable2                                | 不存在时，调用GetIfTable，并使用HeapAlloc申请内存。
| GetIfTable2Ex                              | 不存在时，调用GetIfTable，并使用HeapAlloc申请内存。
| GetIfEntry2                                | 不存在时，调用GetIfEntry。
| GetIfEntry2Ex                              | 不存在时，调用GetIfEntry2。
| FreeMibTable                               | 不存在时，调用HeapFree。
| ConvertInterfaceIndexToLuid                | 不存在时，调用GetIfEntry。
| ConvertInterfaceLuidToNameW(A)             | 不存在时，内部实现。
| ConvertInterfaceNameToLuidW(A)             | 不存在时，内部实现。
| if_nametoindex                             | 不存在时，调用GetIfEntry。
| if_indextoname                             | 不存在时，调用ConvertInterfaceIndexToLuid、ConvertInterfaceLuidToNameA。
| ConvertInterfaceLuidToGuid                 | 不存在时，调用GetIfEntry。
| ConvertInterfaceLuidToIndex                | 不存在时，内部实现。

## kernel32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| DecodePointer                              | 不存在时，返回指针本身。
| EncodePointer                              | 不存在时，返回指针本身。
| Wow64DisableWow64FsRedirection             | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| Wow64RevertWow64FsRedirection              | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| Wow64EnableWow64FsRedirection              | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| IsWow64Process2                            | 不存在时，调用IsWow64Process。
| IsWow64GuestMachineSupported               | 不存在时，调用GetNativeSystemInfo。
| GetTickCount64                             | 不存在时，调用GetTickCount。
| GetSystemTimePreciseAsFileTime             | 不存在时，调用GetSystemTimeAsFileTime。
| InitializeCriticalSectionEx                | 不存在时，调用InitializeCriticalSectionAndSpinCount。
| InitOnceInitialize                         | 初始化为 INIT_ONCE_STATIC_INIT。
| InitOnceBeginInitialize                    | 不存在时，调用NtWaitForKeyedEvent。
| InitOnceComplete                           | 不存在时，调用NtReleaseKeyedEvent。
| InitOnceExecuteOnce                        | 不存在时，调用NtWaitForKeyedEvent以及NtReleaseKeyedEvent。
| LocaleNameToLCID                           | 不存在时，查LocaleNameToLcidTable。
| LCIDToLocaleName                           | 不存在时，查LcidToLocaleNameTable。
| GetLocaleInfoEx                            | 不存在时，调用GetLocaleInfoW。
| GetDateFormatEx                            | 不存在时，调用GetDateFormatW。
| GetTimeFormatEx                            | 不存在时，调用GetTimeFormatW。
| GetNumberFormatEx                          | 不存在时，调用GetNumberFormatW。
| GetCurrencyFormatEx                        | 不存在时，调用GetCurrencyFormatW。
| GetUserDefaultLocaleName                   | 不存在时，调用LCIDToLocaleName。
| GetSystemDefaultLocaleName                 | 不存在时，调用LCIDToLocaleName。
| EnumCalendarInfoExEx                       | 不存在时，调用EnumCalendarInfoExW。
| EnumDateFormatsExEx                        | 不存在时，调用EnumDateFormatsExW。
| LCMapStringEx                              | 不存在时，调用LCMapStringW。
| GetFileInformationByHandleEx               | 不存在时，调用NtQueryInformationFile 或者 NtQueryDirectoryFile。
| SetFileInformationByHandle                 | 不存在时，调用NtSetInformationFile。
| GetFinalPathNameByHandleW(A)               | 不存在时，调用NtQueryObject以及NtQueryInformationFile。
| GetLogicalProcessorInformation             | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| GetLogicalProcessorInformationEx           | 不存在时，调用GetLogicalProcessorInformation。
| GetNumaHighestNodeNumber                   | 不存在时，返回0。
| RaiseFailFastException                     | 不存在时，调用TerminateProcess。
| GetThreadId                                | 不存在时，调用NtQueryInformationThread。
| GetProcessIdOfThread                       | 不存在时，调用NtQueryInformationThread。
| GetProcessId                               | 不存在时，调用NtQueryInformationProcess。
| QueryThreadCycleTime                       | 不存在时，调用GetThreadTimes。
| QueryProcessCycleTime                      | 不存在时，调用GetProcessTimes。
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
| QueryFullProcessImageNameW(A)              | 不存在时，调用GetProcessImageFileNameW(A) 或者 GetModuleFileNameExW(A)。
| CreateFile2                                | 不存在时，调用CreateFileW。
| CreateEventExW(A)                          | 不存在时，调用CreateEventW(A)。
| CreateMutexExW(A)                          | 不存在时，调用CreateMutexW(A)。
| CreateSemaphoreExW                         | 不存在时，调用CreateSemaphoreW。
| CreateWaitableTimerExW                     | 不存在时，调用CreateWaitableTimerW。
| InterlockedCompareExchange64               | 调用内部函数_InterlockedCompareExchange64。
| SetThreadErrorMode                         | 不存在时，调用SetErrorMode。
| GetThreadErrorMode                         | 不存在时，调用GetErrorMode。
| GetErrorMode                               | 不存在时，调用NtQueryInformationProcess。
| InitializeSRWLock                          | 初始化为 RTL_SRWLOCK_INIT。
| AcquireSRWLockExclusive                    | 不存在时，调用NtWaitForKeyedEvent。
| TryAcquireSRWLockExclusive                 | 不存在时，调用InterlockedBitTestAndSet(64)。
| ReleaseSRWLockExclusive                    | 不存在时，调用NtReleaseKeyedEvent。
| AcquireSRWLockShared                       | 不存在时，调用NtWaitForKeyedEvent。
| TryAcquireSRWLockShared                    | 不存在时，调用InterlockedCompareExchange。
| ReleaseSRWLockShared                       | 不存在时，调用NtReleaseKeyedEvent。
| InitializeConditionVariable                | 初始化为 CONDITION_VARIABLE_INIT。
| SleepConditionVariableCS                   | 不存在时，调用NtWaitForKeyedEvent。
| SleepConditionVariableSRW                  | 不存在时，调用NtWaitForKeyedEvent。
| WakeConditionVariable                      | 不存在时，调用NtReleaseKeyedEvent。
| WakeAllConditionVariable                   | 不存在时，调用NtReleaseKeyedEvent。
| InitializeSynchronizationBarrier           | 不存在时，调用CreateEvent。
| EnterSynchronizationBarrier                | 不存在时，调用WaitForSingleObject。
| DeleteSynchronizationBarrier               | 不存在时，调用CloseHandle。
| WaitOnAddress                              | 不存在时，调用NtWaitForKeyedEvent。警告，此函数请勿跨模块使用！！！
| WakeByAddressSingle                        | 不存在时，调用NtReleaseKeyedEvent。警告，此函数请勿跨模块使用！！！
| WakeByAddressAll                           | 不存在时，调用NtReleaseKeyedEvent。警告，此函数请勿跨模块使用！！！
| GetCurrentProcessorNumber                  | 不存在时，调用cpuid。
| GetCurrentProcessorNumberEx                | 不存在时，调用GetCurrentProcessorNumber。
| GetNumaNodeProcessorMask                   | 不存在时，假定所有CPU都在当前Numa。
| GetNumaNodeProcessorMaskEx                 | 不存在时，调用GetNumaNodeProcessorMask。
| GetThreadGroupAffinity                     | 不存在时，调用NtQueryInformationThread。
| SetThreadGroupAffinity                     | 不存在时，调用SetThreadAffinityMask。
| *CancelIoEx                                | 不存在时，调用CancelIo。警告，会把此句柄的所有IO操作取消掉！
| *CancelSynchronousIo                       | 不存在时，仅返回失败。警告，实际无法取消！
| OpenFileById                               | 不存在时，调用NtCreateFile。
| CreateSymbolicLinkW(A)                     | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| ReOpenFile                                 | 不存在时，调用NtCreateFile。
| CompareStringEx                            | 不存在时，调用CompareStringW。
| CompareStringOrdinal                       | 不存在时，使用内置UnicodeCaseTableData实现。
| SetFilePointerEx                           | 不存在时，调用SetFilePointer。
| GetModuleHandleExW(A)                      | 不存在时，调用GetModuleHandleW(A)。
| WTSGetActiveConsoleSessionId               | 不存在时，直接返回 0。
| GetNativeSystemInfo                        | 不存在时，调用GetSystemInfo。
| InitializeSListHead                        | 直接初始化为 0。
| InterlockedFlushSList                      | 不存在时，调用lock cmpxchg8b指令。
| QueryDepthSList                            | 不存在时，直接返回Depth。
| InterlockedPushEntrySList                  | 不存在时，调用lock cmpxchg8b指令。
| InterlockedPopEntrySList                   | 不存在时，调用lock cmpxchg8b指令。
| GetNumaProximityNodeEx                     | 不存在时，调用GetNumaProximityNode。
| GetNumaProcessorNode                       | 不存在时，假定所有CPU都在节点 0。
| GetNumaNodeNumberFromHandle                | 不存在时，假定所有CPU都在节点 0。
| GetNumaProcessorNodeEx                     | 不存在时，调用 GetNumaProcessorNode 。
| GetNumaAvailableMemoryNode                 | 不存在时，假定所有内存都属于节点0 。
| GetNumaAvailableMemoryNodeEx               | 不存在时，调用 GetNumaAvailableMemoryNode 。
| GetNumaProximityNode                       | 不存在时，假定都是节点0 。
| AllocateUserPhysicalPagesNuma              | 不存在时，调用 AllocateUserPhysicalPages 。
| MapViewOfFileExNuma                        | 不存在时，调用 MapViewOfFileEx。
| VirtualAllocExNuma                         | 不存在时，调用 VirtualAllocEx 。
| CreateFileMappingNumaW(A)                  | 不存在时，调用 CreateFileMappingW(A) 。
| GetMaximumProcessorCount                   | 不存在时，调用 GetSystemInfo 。
| GetActiveProcessorCount                    | 不存在时，调用 GetSystemInfo 。
| GetActiveProcessorGroupCount               | 不存在时，假定为1 。
| GetMaximumProcessorGroupCount              | 不存在时，假定为1 。
| GetMemoryErrorHandlingCapabilities         | 不存在时，直接报告不支持任何特性。
| VirtualAllocFromApp                        | 不存在时，调用 VirtualAlloc 。
| VirtualAlloc2                              | 不存在时，调用 VirtualAllocExNuma 以及 VirtualAllocEx 。
| VirtualAlloc2FromApp                       | 不存在时，调用 VirtualAllocExNuma 以及 VirtualAllocEx 。
| CreateFileMappingFromApp                   | 不存在时，调用 CreateFileMappingW 。
| CreateFileMapping2                         | 不存在时，调用 CreateFileMappingNumaW 以及 CreateFileMappingW 。
| MapViewOfFileFromApp                       | 不存在时，调用 MapViewOfFile 。
| UnmapViewOfFileEx                          | 不存在时，调用 UnmapViewOfFile 。
| VirtualProtectFromApp                      | 不存在时，调用 VirtualProtect 。
| OpenFileMappingFromApp                     | 不存在时，调用 OpenFileMappingW 。
| FlushProcessWriteBuffers                   | 不存在时，调用VirtualProtect。
| FlsAlloc                                   | 不存在时，使用Tls实现。警告，此函数请勿跨模块使用！！！
| FlsFree                                    | 不存在时，使用Tls实现。警告，此函数请勿跨模块使用！！！
| FlsGetValue                                | 不存在时，使用Tls实现。警告，此函数请勿跨模块使用！！！
| FlsSetValue                                | 不存在时，使用Tls实现。警告，此函数请勿跨模块使用！！！
| IsThreadAFiber                             | 不存在时，调用 GetCurrentFiber。
| ConvertThreadToFiberEx                     | 不存在时，调用 ConvertThreadToFiber。
| GetDynamicTimeZoneInformation              | 不存在时，调用 GetTimeZoneInformation。
| SetDynamicTimeZoneInformation              | 不存在时，调用 SetTimeZoneInformation。
| GetProductInfo                             | 不存在时，调用 GetVersionExW。
| EnumSystemLocalesEx                        | 不存在时，调用 EnumSystemLocalesW。
| GetThreadPreferredUILanguages              | 不存在时，调用 GetThreadLocale、GetUserDefaultLangID以及GetSystemDefaultLangID。
| GetThreadUILanguage                        | 不存在时，调用 GetThreadLocale。
| ResolveLocaleName                          | 不存在时，调用 LocaleNameToLCID以及LCIDToLocaleName。
| InitializeProcThreadAttributeList          | 不存在时，内部实现。
| DeleteProcThreadAttributeList              | 不存在时，内部实现。
| UpdateProcThreadAttribute                  | 不存在时，内部实现。PROC_THREAD_ATTRIBUTE_PARENT_PROCESS与PROC_THREAD_ATTRIBUTE_HANDLE_LIST特性会被忽略处理。
| GetLargePageMinimum                        | 不存在时，假定为0 。
| SetThreadStackGuarantee                    | 不存在时，调用VirtualAlloc。
| SetCoalescableTimer                        | 不存在时，调用SetTimer。
| SetWaitableTimerEx                         | 不存在时，调用SetWaitableTimer。
| EnumResourceLanguagesExW(A)                | 不存在时，调用EnumResourceLanguagesW(A)。
| DiscardVirtualMemory                       | 不存在时，调用VirtualAlloc MEM_RESET。
| OfferVirtualMemory                         | 不存在时，返回ERROR_SUCCESS。
| ReclaimVirtualMemory                       | 不存在时，返回ERROR_SUCCESS。
| PrefetchVirtualMemory                      | 不存在时，返回ERROR_SUCCESS。
| GetProcessMitigationPolicy                 | 不存在时，调用NtQueryInformationProcess。
| SetProcessMitigationPolicy                 | 不存在时，调用NtSetInformationProcess。
| SetProcessInformation                      | 不存在时，调用NtSetInformationProcess。
| GetThreadInformation                       | 不存在时，调用NtQueryInformationThread。
| SetThreadInformation                       | 不存在时，调用NtSetInformationThread。
| PowerCreateRequest                         | 不存在时，内部实现。
| PowerSetRequest                            | 不存在时，调用 SetThreadExecutionState。
| PowerClearRequest                          | 不存在时，调用 SetThreadExecutionState。
| TzSpecificLocalTimeToSystemTime            | 不存在时，内部实现。
| TzSpecificLocalTimeToSystemTimeEx          | 不存在时，内部实现。
| GetFirmwareType                            | 不存在时，调用NtQuerySystemInformation。
| IsNativeVhdBoot                            | 不存在时，调用NtQuerySystemInformation。
| RtlCaptureStackBackTrace                   | 调用ntdll.RtlCaptureStackBackTrace。
| SetFileCompletionNotificationModes         | 不存在时，什么也不做。
| GetQueuedCompletionStatusEx                | 不存在时，调用 GetQueuedCompletionStatus。
| FindFirstFileEx(W/A)                       | Windows XP、Vista兼容 FIND_FIRST_EX_LARGE_FETCH、FindExInfoStandard参数。
| GetProcessGroupAffinity                    | 不存在时，始终认为只有一组CPU。
| QueryUnbiasedInterruptTime                 | 不存在时，读取KUSER_SHARED_DATA::InterruptTime值模拟UnbiasedInterruptTime。
| FindStringOrdinal                          | 不存在时，调用CompareStringOrdinal。
| GetEnabledXStateFeatures                   | 不存在时，调用IsProcessorFeaturePresent。
| SetXStateFeaturesMask                      | 不存在时，内部实现。
| InitializeContext                          | 不存在时，内部实现。
| InitializeContext2                         | 不存在时，调用InitializeContext。
| LocateXStateFeature                        | 不存在时，内部实现。
| CopyContext                                | 不存在时，内部实现。
| QueryIdleProcessorCycleTimeEx              | 不存在时，调用QueryIdleProcessorCycleTime。
| QueryIdleProcessorCycleTime                | 不存在时，随便返回一组数字代表当前进程空闲时间。
| SetThreadIdealProcessorEx                  | 不存在时，调用SetThreadIdealProcessor。
| GetThreadIdealProcessorEx                  | 不存在时，调用SetThreadIdealProcessor。
| GetUserPreferredUILanguages                | 不存在时，调用GetThreadPreferredUILanguages。
| EnumTimeFormatsEx                          | 不存在时，调用EnumTimeFormatsW。
| GetCalendarInfoEx                          | 不存在时，调用GetCalendarInfoW。
| GetNLSVersionEx                            | 不存在时，返回一个假版本。
| IsNLSDefinedString                         | 不存在时，调用GetStringTypeW。
| FindNLSStringEx                            | 调用 CompareStringW。
| SetProcessWorkingSetSizeEx                 | 不存在时，调用SetProcessWorkingSetSize。
| GetProcessWorkingSetSizeEx                 | 不存在时，调用GetProcessWorkingSetSize。
| GetTimeZoneInformationForYear              | 不存在时，直接读取`Time Zones`注册表。
| SetProcessDEPPolicy                        | 不存在时，调用NtSetInformationProcess。
| GetSystemDEPPolicy                         | 不存在时，返回总是关闭。
| DisableThreadLibraryCalls                  | 不存在时，始终返回成功。
| CreateRemoteThreadEx                       | 不存在时，调用CreateRemoteThread。
| WerRegisterRuntimeExceptionModule          | 不存在时，返回S_OK。
| WerUnregisterRuntimeExceptionModule        | 不存在时，返回S_OK。
| Wow64GetThreadContext                      | 不存在时，调用GetThreadContext或者返回ERROR_INVALID_PARAMETER。
| SetDefaultDllDirectories                   | 不存在时，手工控制LoadLibrary加载顺序。
| GetCurrentPackageFullName                  | 返回 APPMODEL_ERROR_NO_PACKAGE。
| OpenProcess                                | 额外处理 PROCESS_QUERY_LIMITED_INFORMATION、PROCESS_SET_LIMITED_INFORMATION。
| GetThreadDescription                       | 返回空字符串。
| SetThreadDescription                       | 返回 `E_NOTIMPL`。

## mfplat.dll
| 函数                                       | Fallback
| ----                                       | -----------
| MFCreateDeviceSource                       | 不存在时，返回E_NOTIMPL。
| MFEnumDeviceSources                        | 不存在时，返回E_NOTIMPL。

## mfplat.dll
| 函数                                       | Fallback
| ----                                       | -----------
| MFCreateDXGIDeviceManager                  | 不存在时，返回E_NOTIMPL。
| MFCreateDXGISurfaceBuffer                  | 不存在时，返回E_NOTIMPL。
| MFLockDXGIDeviceManager                    | 不存在时，返回E_NOTIMPL。
| MFUnlockDXGIDeviceManager                  | 不存在时，返回E_NOTIMPL。
| MFCreateAlignedMemoryBuffer                | 不存在时，返回E_NOTIMPL。
| MFCreateAsyncResult                        | 不存在时，返回E_NOTIMPL。
| MFCreateAttributes                         | 不存在时，返回E_NOTIMPL。
| MFCreateEventQueue                         | 不存在时，返回E_NOTIMPL。
| MFCreateMediaBufferWrapper                 | 不存在时，返回E_NOTIMPL。
| MFCreateMediaEvent                         | 不存在时，返回E_NOTIMPL。
| MFCreateMediaType                          | 不存在时，返回E_NOTIMPL。
| MFCreateMemoryBuffer                       | 不存在时，返回E_NOTIMPL。
| MFCreatePresentationDescriptor             | 不存在时，返回E_NOTIMPL。
| MFCreateSample                             | 不存在时，返回E_NOTIMPL。
| MFCreateStreamDescriptor                   | 不存在时，返回E_NOTIMPL。
| MFCreateWaveFormatExFromMFMediaType        | 不存在时，返回E_NOTIMPL。
| MFFrameRateToAverageTimePerFrame           | 不存在时，返回E_NOTIMPL。
| MFGetSystemTime                            | 不存在时，调用GetSystemTimeAsFileTime。
| MFInitMediaTypeFromWaveFormatEx            | 不存在时，返回E_NOTIMPL。
| MFShutdown                                 | 不存在时，返回E_NOTIMPL。
| MFStartup                                  | 不存在时，返回E_NOTIMPL。
| MFTEnumEx                                  | 不存在时，返回E_NOTIMPL。

## mfreadwrite.dll
| 函数                                       | Fallback
| ----                                       | -----------
| MFCreateSourceReaderFromMediaSource        | 不存在时，返回E_NOTIMPL。

## ndfapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| NdfCreateWebIncident                       | 不存在时，返回一个伪句柄假装成功。
| NdfCloseIncident                           | 不存在时，假装成功关闭句柄。
| NdfExecuteDiagnosis                        | 不存在时，假装什么问题也没有发现。

## netapi32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| NetGetAadJoinInformation                   | 不存在时，始终认为没有加入 Azure AD 帐户 账号。
| NetFreeAadJoinInformation                  | 不存在时，什么也不做。

## ntdll.dll
| 函数                                       | Fallback
| ----                                       | -----------
| NtCancelIoFileEx                           | 不存在时，调用 NtCancelIoFile。注意：将取消此文件的所有IO请求。
| NtOpenKeyEx                                | 不存在时，调用 NtOpenKey 或者 NtCreateKey。

## ole32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| CoGetApartmentType                         | 不存在时，调用IComThreadingInfo。

## pdh.dll
| 函数                                       | Fallback
| ----                                       | -----------
| PdhAddEnglishCounterW(A)                   | 不存在时，调用PdhAddCounterW(A)。

## powrprof.dll
| 函数                                       | Fallback
| ----                                       | -----------
| PowerDeterminePlatformRole                 | 不存在时，返回PlatformRoleDesktop。
| PowerDeterminePlatformRoleEx               | 不存在时，调用PowerDeterminePlatformRole。
| PowerRegisterSuspendResumeNotification     | 不存在时，使用窗口模拟。
| PowerUnregisterSuspendResumeNotification   | 内部实现。
| PowerGetActiveScheme                       | 不存在时，始终认为是平衡模式。
| PowerReadACValue                           | 不存在时，读取注册表。（目前仅支持ConsoleLock）
| PowerReadDCValue                           | 不存在时，读取注册表。（目前仅支持ConsoleLock）

## PropSys.dll
| 函数                                       | Fallback
| ----                                       | -----------
| InitPropVariantFromCLSID                   | 不存在时，CoTaskMemAlloc分配内存。
| PSGetPropertyKeyFromName                   | 不存在时，返回 TYPE_E_ELEMENTNOTFOUND（属性不存在）。
| PSCreateMemoryPropertyStore                | 不存在时，返回 E_NOTIMPL。
| VariantCompare                             | 不存在时，内部实现。

## psapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| EnumProcessModulesEx                       | 不存在时，调用EnumProcessModules。
| GetWsChangesEx                             | 不存在时，调用GetWsChanges。
| *QueryWorkingSetEx                         | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。

## setupapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| SetupDiGetDevicePropertyW                  | 不存在时，调用SetupDiGetDeviceRegistryPropertyW。
| SetupDiSetDevicePropertyW                  | 不存在时，调用SetupDiSetDeviceRegistryPropertyW。
| SetupDiGetClassPropertyW                   | 不存在时，调用SetupDiGetClassRegistryPropertyW。
| SetupDiGetClassPropertyExW                 | 不存在时，调用SetupDiGetClassRegistryPropertyW。
| SetupDiSetClassPropertyW                   | 不存在时，调用SetupDiSetClassRegistryPropertyW。
| SetupDiSetClassPropertyExW                 | 不存在时，调用SetupDiSetClassRegistryPropertyW。

## shcore.dll
| 函数                                       | Fallback
| ----                                       | -----------
| GetDpiForMonitor                           | 不存在时，调用GetDeviceCaps。
| GetProcessDpiAwareness                     | 调用 IsProcessDPIAware。
| SetProcessDpiAwareness                     | 不存在时，调用SetProcessDPIAware。
| SetProcessDPIAware                         | 不存在时，直接返回 TRUE。

## shell32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| SHGetKnownFolderPath                       | 不存在时，调用SHGetFolderPathW。
| SHSetKnownFolderPath                       | 不存在时，调用SHSetFolderPathW。
| SHGetKnownFolderIDList                     | 不存在时，调用SHGetFolderLocation。
| SHBindToFolderIDListParent                 | 不存在时，调用IShellFolder。
| SHBindToFolderIDListParentEx               | 不存在时，调用IShellFolder。
| SHBindToObject                             | 不存在时，调用IShellFolder。
| SHCreateItemFromIDList                     | 不存在时，调用IShellItem。
| SHCreateItemWithParent                     | 不存在时，调用IShellItem。
| SHCreateItemFromRelativeName               | 不存在时，调用IShellItem。
| SHGetNameFromIDList                        | 不存在时，调用IShellItem。
| SHCreateShellItem                          | 不存在时，调用IShellItem。
| SHCreateItemFromParsingName                | 不存在时，调用SHParseDisplayName。
| Shell_NotifyIconGetRect                    | 不存在时，调用SendMessageW（可能不适用于Vista系统）。
| SHGetStockIconInfo                         | 不存在时，调用LoadImageW。
| SHGetPropertyStoreForWindow                | 不存在时，报告错误 E_NOTIMPL。
| SHOpenWithDialog                           | 不存在时，报告错误 E_NOTIMPL。

## shlwapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| StrToInt64ExW(A)                           | 不存在时，手工解析字符串。

## UIAutomationCore.dll
| 函数                                       | Fallback
| ----                                       | -----------
| UiaClientsAreListening                     | 不存在时，假装没有人在监听。
| UiaHostProviderFromHwnd                    | 不存在时，报告错误 E_NOTIMPL。
| UiaRaiseAutomationEvent                    | 不存在时，报告错误 E_NOTIMPL。
| UiaRaiseAutomationPropertyChangedEvent     | 不存在时，报告错误 E_NOTIMPL。
| UiaReturnRawElementProvider                | 不存在时，报告错误 E_NOTIMPL。
| UiaGetReservedMixedAttributeValue          | 不存在时，报告错误 E_NOTIMPL。
| UiaGetReservedNotSupportedValue            | 不存在时，报告错误 E_NOTIMPL。
| UiaRaiseStructureChangedEvent              | 不存在时，报告错误 E_NOTIMPL。
| UiaRaiseNotificationEvent                  | 不存在时，假装成功。

## user32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| IsWow64Process                             | 不存在时，返回TRUE，并设置 `*Wow64Process = FALSE`。
| SetProcessDpiAwarenessContext              | 不存在时，调用SetProcessDpiAwareness。
| GetDpiForSystem                            | 不存在时，调用GetDeviceCaps。
| GetDpiForWindow                            | 不存在时，调用GetDpiForMonitor。
| GetSystemMetricsForDpi                     | 不存在时，调用GetSystemMetrics。
| AdjustWindowRectExForDpi                   | 不存在时，调用AdjustWindowRectEx。
| SystemParametersInfoW(A)                   | SPI_GETNONCLIENTMETRICS修正。
| SystemParametersInfoForDpi                 | 不存在时，调用SystemParametersInfoW。
| RegisterSuspendResumeNotification          | 不存在时，使用窗口模拟。
| UnregisterSuspendResumeNotification        | 不存在时，内部实现。
| IsProcessDPIAware                          | 不存在时，返回 FALSE。
| SetProcessDPIAware                         | 不存在时，什么都不做，假装成功。
| GetWindowDisplayAffinity                   | 不存在时，TRUE，并报告窗口没有任何保护`WDA_NONE`。
| SetWindowDisplayAffinity                   | 不存在时，什么都不做，假装成功。
| RegisterTouchWindow                        | 不存在时，什么都不做，假装成功。
| UnregisterTouchWindow                      | 不存在时，什么都不做，假装成功。
| IsTouchWindow                              | 不存在时，始终报告非触摸窗口。
| GetTouchInputInfo                          | 不存在时，报告错误 ERROR_INVALID_HANDLE。
| CloseTouchInputHandle                      | 不存在时，报告错误 ERROR_INVALID_HANDLE。
| *ChangeWindowMessageFilterEx               | 不存在时，调用ChangeWindowMessageFilter。温馨提示：将影响该进程的所有窗口。
| ChangeWindowMessageFilter                  | 不存在时，什么都不做，假装成功。
| UpdateLayeredWindowIndirect                | 不存在时，调用UpdateLayeredWindow。
| AddClipboardFormatListener                 | 不存在时，什么都不做，假装成功。
| RemoveClipboardFormatListener              | 不存在时，什么都不做，假装成功。
| RegisterPowerSettingNotification           | 不存在时，什么都不做，假装成功。
| UnregisterPowerSettingNotification         | 不存在时，什么都不做，假装成功。
| DisplayConfigGetDeviceInfo                 | 不存在时，报告没有安装驱动。
| GetDisplayConfigBufferSizes                | 不存在时，报告没有安装驱动。
| QueryDisplayConfig                         | 不存在时，报告没有安装驱动。
| RegisterPointerDeviceNotifications         | 不存在时，假装成功。
| GetPointerDevices                          | 不存在时，假装没有触摸设备。
| GetPointerDevice                           | 不存在时，假装没有触摸设备。
| GetPointerPenInfo                          | 不存在时，假装没有触摸设备。
| GetPointerType                             | 不存在时，假装没有触摸设备。
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
| CreateAppContainerProfile                  | 不存在时，返回E_NOTIMPL。
| DeleteAppContainerProfile                  | 不存在时，返回E_NOTIMPL。
| DeriveAppContainerSidFromAppContainerName  | 不存在时，返回E_NOTIMPL。
| GetAppContainerFolderPath                  | 不存在时，返回E_NOTIMPL。
| GetAppContainerRegistryLocation            | 不存在时，返回E_NOTIMPL。

## uxtheme.dll
| 函数                                       | Fallback
| ----                                       | -----------
| DrawThemeTextEx                            | 不存在时，尝试获取非导出DrawThemeTextEx。如果任然获取失败则调用DrawThemeText。
| GetThemeTransitionDuration                 | 不存在时，返回E_NOTIMPL。
| SetWindowThemeAttribute                    | 不存在时，返回E_NOTIMPL。

## version.dll
| 函数                                       | Fallback
| ----                                       | -----------
| GetFileVersionInfoExW(A)                   | 不存在时，调用GetFileVersionInfoW(A)。
| GetFileVersionInfoSizeExW(A)               | 不存在时，调用GetFileVersionInfoSizeW(A)。

## wevtapi.dll
| 函数                                       | Fallback
| ----                                       | -----------
| EvtClose                                   | 不存在时，返回TRUE。
| EvtCreateRenderContext                     | 不存在时，报告ERROR_NOT_SUPPORTED。
| EvtNext                                    | 不存在时，报告ERROR_NOT_SUPPORTED。
| EvtQuery                                   | 不存在时，报告ERROR_NOT_SUPPORTED。
| EvtRender                                  | 不存在时，报告ERROR_NOT_SUPPORTED。

## WinHttp.dll
| 函数                                       | Fallback
| ----                                       | -----------
| WinHttpCreateProxyResolver                 | 不存在时，内部实现。
| WinHttpGetProxyForUrlEx                    | 不存在时，异步调用WinHttpGetProxyForUrl。
| WinHttpGetProxyResult                      | 不存在时，内部实现。
| WinHttpFreeProxyResult                     | 不存在时，内部实现。

## WinUsb.dll
| 函数                                       | Fallback
| ----                                       | -----------
| WinUsb_Free                                | 不存在时，报告ERROR_INVALID_HANDLE。
| WinUsb_GetAssociatedInterface              | 不存在时，报告ERROR_INVALID_HANDLE。
| WinUsb_GetOverlappedResult                 | 不存在时，报告ERROR_INVALID_HANDLE。
| WinUsb_Initialize                          | 不存在时，报告ERROR_INVALID_HANDLE。
| WinUsb_ReadPipe                            | 不存在时，报告ERROR_INVALID_HANDLE。
| WinUsb_ResetPipe                           | 不存在时，报告ERROR_INVALID_HANDLE。
| WinUsb_SetCurrentAlternateSetting          | 不存在时，报告ERROR_INVALID_HANDLE。
| WinUsb_WritePipe                           | 不存在时，报告ERROR_INVALID_HANDLE。

## ws2_32.dll
| 函数                                       | Fallback
| ----                                       | -----------
| InetPtonW(inet_pton)                       | 不存在时，类似于sscanf手工分析字符串。
| InetNtopW(inet_ntop)                       | 不存在时，类似于sprintf手工生成字符串。
| WSAPoll                                    | 不存在时，调用select。
| GetAddrInfoExCancel                        | 不存在时，使用线程模拟。
| GetAddrInfoExW(A)                          | 不存在时，调用GetAddrInfoW(A)。
| GetAddrInfoExOverlappedResult              | 不存在时，内部实现。
| FreeAddrInfoEx(W)                          | 不存在时，内部实现。
| GetAddrInfoW                               | 不存在时，调用getaddrinfo。
| FreeAddrInfoW                              | 不存在时，内部实现。
| WSASocketW(A)                              | 低于6.1.7601时自动去除 `WSA_FLAG_NO_HANDLE_INHERIT`。
| WSAIoctl                                   | 低于6.0时，`SIO_BASE_HANDLE` 代码返回SOCKET自身。
