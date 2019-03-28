
# YY-Thunks——让兼容Windows更轻松


## 1. 关于YY-Thunks
总众所周知Windows XP每个SP都会新增大量API，而Windows 10每次更新又会新增大量API，这使得兼容不同版本的Windows需要大量的判断。

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
* 所有Visual Studio版本均支持（比如：VC6.0、VS2008、VS2010、VS2015、VS2017等等）。
* 所有运行库模式均支持（比如：`/MD`、`/MT`、`/MDd`、`/MTd`）。

### 3.2. Thunks清单
此表展示了YY-Thunks（鸭船）可以解决的函数不存在问题，欢迎大家扩充！

> 开头带`*`的函数并不建议使用，仅用于编译通过处理，如果使用可能导致老版本系统无法充分发挥性能。

| 函数                                                                                                                           | Fallback
| ----                                                                                                                           | -----------
| [DecodePointer](https://msdn.microsoft.com/en-us/library/bb432242.aspx)                                                        | 不存在时，返回指针本身。
| [EncodePointer](https://msdn.microsoft.com/en-us/library/bb432254.aspx)                                                        | 不存在时，返回指针本身。
| [RegDeleteKeyExW(A)](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724847.aspx)                                   | 不存在时，调用RegDeleteKeyW(A)。
| [Wow64DisableWow64FsRedirection](https://msdn.microsoft.com/en-us/library/windows/desktop/aa365743.aspx)                       | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [Wow64RevertWow64FsRedirection](https://msdn.microsoft.com/en-us/library/windows/desktop/aa365745.aspx)                        | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [Wow64EnableWow64FsRedirection](https://msdn.microsoft.com/en-us/library/windows/desktop/aa365744.aspx)                        | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [IsWow64Process](https://msdn.microsoft.com/en-us/library/windows/desktop/ms684139.aspx)                                       | 不存在时，返回TRUE，并设置 `*Wow64Process = FALSE`。
| [IsWow64Message](https://msdn.microsoft.com/en-us/library/windows/desktop/ms684136.aspx)                                       | 不存在时，返回FALSE。
| [RegSetKeyValueW(A)](https://msdn.microsoft.com/en-us/library/ms724921.aspx)                                                   | 调用RegCreateKeyExW(A)以及RegSetValueExW(A)实现。
| [RegDeleteKeyValueW(A)](https://msdn.microsoft.com/en-us/library/ms724848.aspx)                                                | 调用RegOpenKeyExW(A)以及RegDeleteValueW(A)实现。
| [RegDeleteTreeW(A)](https://msdn.microsoft.com/en-us/library/aa379776.aspx)                                                    | 调用SHDeleteKeyW(A)实现。
| [RegGetValueW(A)](https://msdn.microsoft.com/en-us/library/aa379776.aspx)                                                      | 不存在时，调用RegQueryValueExW(A)实现。
| [IsWow64Process2](https://msdn.microsoft.com/en-us/library/windows/desktop/mt804318.aspx)                                      | 不存在时，调用IsWow64Process。
| [IsWow64GuestMachineSupported](https://msdn.microsoft.com/en-us/library/windows/desktop/mt804321.aspx)                         | 不存在时，调用GetNativeSystemInfo。
| [GetTickCount64](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724411.aspx)                                       | 不存在时，调用GetTickCount。
| [GetSystemTimePreciseAsFileTime](https://msdn.microsoft.com/en-us/library/windows/desktop/hh706895.aspx)                       | 不存在时，调用GetSystemTimeAsFileTime。
| [InitializeCriticalSectionEx](https://msdn.microsoft.com/en-us/library/ms683477.aspx)                                          | 不存在时，调用InitializeCriticalSectionAndSpinCount。
| [InitOnceExecuteOnce](https://msdn.microsoft.com/en-us/library/windows/desktop/ms683493.aspx)                                  | 不存在时，调用自旋锁（InterlockedCompareExchange）。
| [LocaleNameToLCID](https://docs.microsoft.com/en-us/windows/desktop/api/winnls/nf-winnls-localenametolcid)                     | 不存在时，查LocaleNameToLcidTable。
| [LCIDToLocaleName](https://docs.microsoft.com/en-us/windows/desktop/api/winnls/nf-winnls-lcidtolocalename)                     | 不存在时，查LcidToLocaleNameTable。
| [GetLocaleInfoEx](https://docs.microsoft.com/en-us/windows/desktop/api/winnls/nf-winnls-getlocaleinfoex)                       | 不存在时，调用GetLocaleInfoW。
| [GetDateFormatEx](https://docs.microsoft.com/en-us/windows/desktop/api/datetimeapi/nf-datetimeapi-getdateformatex)             | 不存在时，调用GetDateFormatW。
| [GetTimeFormatEx](https://docs.microsoft.com/en-us/windows/desktop/api/datetimeapi/nf-datetimeapi-gettimeformatex)             | 不存在时，调用GetTimeFormatW。
| [GetNumberFormatEx](https://docs.microsoft.com/en-us/windows/desktop/api/winnls/nf-winnls-getnumberformatex)                   | 不存在时，调用GetNumberFormatW。
| [GetCurrencyFormatEx](https://docs.microsoft.com/en-us/windows/desktop/api/winnls/nf-winnls-getcurrencyformatex)               | 不存在时，调用GetCurrencyFormatW。
| [GetUserDefaultLocaleName](https://docs.microsoft.com/en-us/windows/desktop/api/winnls/nf-winnls-getuserdefaultlocalename)     | 不存在时，调用LCIDToLocaleName。
| [GetSystemDefaultLocaleName](https://docs.microsoft.com/en-us/windows/desktop/api/winnls/nf-winnls-getsystemdefaultlocalename) | 不存在时，调用LCIDToLocaleName。
| [EnumCalendarInfoExEx](https://docs.microsoft.com/zh-cn/windows/desktop/api/winnls/nf-winnls-enumcalendarinfoexex)             | 不存在时，调用EnumCalendarInfoExW。
| [EnumDateFormatsExEx](https://docs.microsoft.com/zh-cn/windows/desktop/api/winnls/nf-winnls-enumdateformatsexex)               | 不存在时，调用EnumDateFormatsExW。
| [GetFileInformationByHandleEx](https://docs.microsoft.com/zh-cn/windows/desktop/api/winbase/nf-winbase-getfileinformationbyhandleex)| 不存在时，调用NtQueryInformationFile/NtQueryDirectoryFile。
| [SetFileInformationByHandle](https://docs.microsoft.com/zh-cn/windows/desktop/api/winbase/nf-winbase-getfileinformationbyhandleex)| 不存在时，调用NtSetInformationFile。
| [GetFinalPathNameByHandleW(A)](https://docs.microsoft.com/en-us/windows/desktop/api/fileapi/nf-fileapi-getfinalpathnamebyhandlew)| 不存在时，调用NtQueryObject以及NtQueryInformationFile。
| [GetLogicalProcessorInformation](https://docs.microsoft.com/zh-cn/windows/desktop/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation)| 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [GetLogicalProcessorInformationEx](https://docs.microsoft.com/zh-cn/windows/desktop/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformationex)| 不存在时，调用GetLogicalProcessorInformation。
| *[GetCurrentProcessorNumber](https://msdn.microsoft.com/en-us/library/windows/desktop/ms683181.aspx)                           | 不存在时，返回0。
| *[GetCurrentProcessorNumberEx](https://msdn.microsoft.com/en-us/library/windows/desktop/dd405487.aspx)                         | 不存在时，调用GetCurrentProcessorNumber。
| *[GetNumaNodeProcessorMask](https://msdn.microsoft.com/en-us/library/windows/desktop/ms683204.aspx)                            | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_PARAMETER。
| *[GetNumaNodeProcessorMaskEx](https://msdn.microsoft.com/en-us/library/windows/desktop/dd405493.aspx)                          | 不存在时，调用GetNumaNodeProcessorMask。
| *[SetThreadGroupAffinity](https://msdn.microsoft.com/en-us/library/windows/desktop/dd405516.aspx)                              | 不存在时，调用SetThreadAffinityMask。


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


### 1.0.0.12 - 功能更新（2019-03-28 12:30）
* 添加EnumCalendarInfoExEx
* 添加EnumDateFormatsExEx
* 添加GetFileInformationByHandleEx
* 添加SetFileInformationByHandle
* 添加GetFinalPathNameByHandleW(A)
* 解决Bug，1.0.0.10版意外引入ntdll.lib问题（感谢 小古）。
* 添加GetLogicalProcessorInformation
* 添加GetLogicalProcessorInformationEx
