
# YY-Thunks——让兼容Windows XP更轻松


## 1. 关于YY-Thunks
Windows XP每个SP都会新增大量新接口，这使得兼容不同SP版本的Windows XP需要大量的判断。而现在大量开源代码已经不再兼容一些早期的Windows XP版本，比如 Windows XP RTM。

YY-Thunks（鸭船）,存在的目的就是抹平不同SP版本系统的差异，编译时单纯添加一个obj即可自动解决这些兼容性问题。让你兼容Windows XP更轻松！


[ [鸭船交流群 633710173](https://shang.qq.com/wpa/qunwpa?idkey=21d51d8ad1d77b99ea9544b399e080ec347ca6a1bc04267fb59cebf22644a42a) ]


### 1.1. 原理
使用`LoadLibrary`以及`GetProcAddress`动态加载API，不存在时做出补偿措施，最大限度模拟原始API行为，让你的程序正常运行。

### 1.2. 亮点
* 更快！更安全！`鸭船`内建2级缓存以及按需加载机制，同时自动加密所有函数指针，防止内存爆破攻击。最大程度减少不需要的、不必要的`LoadLibrary以及GetProcAddress`调用以及潜在安全风险。
* 轻松兼容Windows XP，让你安心专注于业务逻辑。
* 完全的开放代码，广泛的接受用户意见，希望大家能踊跃的 pull requests，为`鸭船`添砖加瓦。

## 2. 使用YY-Thunks
1：将`YY-Thunks\objs` 文件夹复制到你的工程目录。<br/>
2：【连接器】-【输入】-【附加依赖项】，添加`objs\$(PlatformShortName)\YY_Thunks_for_WinXP.obj`。<br/>
3：重新编译代码。

## 3. YY-Thunks兼容性
### 3.1. 支持的编译器
全平台ABI兼容。
* 所有Visual Studio版本均支持（比如：VC6.0、VS2008、VS2010、VS2015、VS2017等等）。
* 所有运行库模式都支持（比如：`/MD`、`/MT`、`/MDd`、`/MTd`）。

### 3.2. Thunks清单
此表展示了YY-Thunks（鸭船）可以解决的函数不存在问题，欢迎大家扩充！

| 函数                                                                                                         | Fallback
| ----                                                                                                         | -----------
| [DecodePointer](https://msdn.microsoft.com/en-us/library/bb432242.aspx)                                      | 不存在时，返回指针本身。
| [EncodePointer](https://msdn.microsoft.com/en-us/library/bb432254.aspx)                                      | 不存在时，返回指针本身。
| [RegDeleteKeyExW/RegDeleteKeyExA](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724847.aspx)    | 不存在时，调用RegDeleteKeyW/RegDeleteKeyA。
| [Wow64DisableWow64FsRedirection](https://msdn.microsoft.com/en-us/library/windows/desktop/aa365743.aspx)     | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [Wow64RevertWow64FsRedirection](https://msdn.microsoft.com/en-us/library/windows/desktop/aa365745.aspx)      | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [Wow64EnableWow64FsRedirection](https://msdn.microsoft.com/en-us/library/windows/desktop/aa365744.aspx)      | 不存在时，返回FALSE，并设置 LastError = ERROR_INVALID_FUNCTION。
| [IsWow64Process](https://msdn.microsoft.com/en-us/library/windows/desktop/ms684139.aspx)                     | 不存在时，返回TRUE，并设置 `*Wow64Process = FALSE`。
| [IsWow64Message](https://msdn.microsoft.com/en-us/library/windows/desktop/ms684136.aspx)                     | 不存在时，返回FALSE。


## Changes

### 1.0.0.1 2018-05-17 19:00
* YY-Thunks第一版
