
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

## 更新日志

请参阅 [Changelog.md](Changelog.md)
