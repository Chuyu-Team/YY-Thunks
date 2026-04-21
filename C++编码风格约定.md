# 1. 前言
我们知道，每个人都有自己的喜好，这并没有对错。但是当一个项目里出现五花八门的形式时，看起来总是那么的怪怪……

其次，C++很强大，某些用法可能产生一些负面影响。风格也有助于规避一些问题。这也是本文的重点，让我们一起坚守底线。


# 2. 命名约定
## 2.1. 基本约定
| 类型           | 风格                       | 例子                          | 注释
| -------------- | -------------------------- | ----------------------------- | -------
| 命名空间       | 帕斯卡                     | `namespace MegaUI;`           |
| 类/结构名      | 帕斯卡                     | `class Element;`              |
| 函数名称       | 帕斯卡                     | `void __fastcall Init();`     |
| 全局变量       | `g_` + 匈牙利Y前缀 + 帕斯卡| `uint32_t g_uSize;`           | 类或者函数中的static变量也认为是全局变量。
| 常量           | 帕斯卡                     | `constexpr uint32_t kSize = 0;`| 类或者函数中的static常量也这样处理。
| 参数/局部变量  | `_` + 匈牙利Y前缀 + 帕斯卡 | `uint32_t _uSize;`            |
| 成员变量       | 匈牙利Y前缀 + 帕斯卡       | `uint32_t uSize;`             |
| 枚举/枚举成员  | 帕斯卡                     | `enum ValueType { Element, };`|
| 宏             | `__` + 全_大_写            | `__MEGA_UI`, `__MEGA_UI__`, `_MEGA_UI` | 类似均可，最大的要求就是全大写。
| 文件名         | 尽量与其中的类名一致       | `Element.h`, `Element.cpp`    |

> 温馨提示：如果命名与CRT或者SDK中的宏重名，尽量避开此类命名，如果实在无法避开，那么末尾再添加 `_`。
举个例子，我们知道Windows SDK中的有一个宏`MoveFile`。那么我们可以改名为 `MoveFile_`。

## 2.2. 匈牙利Y前缀规范
> 注意，这里的匈牙利前缀与微软版本存在一些区别，所以我称之为匈牙利Y前缀规范。

| 语义               | 前缀      | 例子                  | 注释
| ------------------ | --------- | --------------------- | ---------
| bool               | b         | `bool bSuccess;`      |
| 字符               | ch        | `char chDriveLetter;` | char，wchar_t 统一。
| 带符号数字         | i         | `int64_t iSize;`      | int8_t、int16_t、int32_t。
| 无符号整形         | u         | `uint64_t uSize;`     | uint8_t、uint16_t、uint32_t均适用，不考虑长度。
| 浮点               | f         | `float fValue;`       | float、double均适用，不考虑长度。
| 位域组合           | 无        | `XXXStyles eXXXStyles;`  | 这类按位组合强烈定义位域，使用`enum class`，而不是使用宏。
| Buffer变量的字节数 | cb        | `uint32_t cbBuffer;`  | 必须为无符号数，不考虑长度。
| Buffer变量元素个数 | c         | `uint32_t cBuffer;`   | 必须为无符号数，不考虑长度。
| Buffer变量字符数   | cch       | `uint32_t cchBuffer;` | 与`c` 类似，只是更加明确为字符数量。
| HRESULT            | hr        | `HRESULT hrOpen;`     |
| LSTATUS            | ls        | `LSTATUS lsOpen;`     |
| NTSTATUS           | ns        | `NTSTATUS nsOpen;`    |
| 枚举值             | e         | `ValueType eType;`    |
| 指针               | p + Y前缀 | `int32_t* piData;`    | 以 `p` 开头，再组合上指针指向的对象的匈牙利Y前缀。
| 函数指针           | pfn       | `void (*pfnNotify)();`|
| 以 0 结尾的字符串  | sz        | `auto szName = "YY";` | `char*`，`wchar_t*` 统一。
| 句柄等内核对象     | h         | `HANDLE hFile;`       |
| 迭代器             | it        | `auto it = T.begin();`|
| 数组               | 根据Item确定 | `vector<int> iDataArray;` | 注意尾部，加上Array显式表面这是一片连续的空间。
| 其他类型（Other)   | o         | `Brush oBrush;`       | 当其他前置无法表示时可以考虑添加此前置。

# 3. 编码风格
## 3.1. 总体要求
### 3.1.1. 风格类要求
* 缩进采用 4个空格，而不是Tab。
* 代码尾部尽量不要出现无意义的多余空格。
* 左右花括号自个独立一行，花括号内的代码统一缩进（即：4个空格）。
* 代码块多余一行时不要省略`花括号`。
* 外部不可见的变量、函数等统一使用`static`修饰，不要使用匿名空间，它不利于阅读代码。
* 对于复杂的判断逻辑，不要使用`单目!`，应该完整的写出来，比如：`if(XXX == false || XXXX == nullptr)`。
* 不要过分依赖C++操作符优先级，放上括号逻辑更加清晰。
* 代码尽可能在自己的命名空间中。
* 对于数组尽量使用`_countof`，而不是`sizeof`，因为`_countof`拥有编译时检查，如果传入指针那么报错。
* 传入结构体大小时，尽量使用对象，比如`sizeof(VersionInformation)`，而不是`sizeof(OSVERSIONINFOW)`。
* 文件编码尽可能使用UTF8，RC文件则使用UTF16LE。

### 3.1.2. 安全类要求
* 所有变量、参数、返回值请严格验证。有问题则返回错误代码，如果无法返回才考虑抛出异常或者中断程序执行。
* 禁止使用不可控、或者存在资源泄露的全局变量、以及单例。
    - 多个文件之间，全局对象的构造与析构是不可控制的，这容易引发问题。
    - 如果它是可控的，也不会导致泄露那么，那么允许使用。
    - 常见的`vector`、`string`、`map`等均不可作为全局对象。
* 函数内`static变量`，避免不必要的动态初始化工作。
    - 新编译器默认开启静态对象安全初始化，但是依赖TLS，这将导致Windows XP发生兼容性问题。
    - 如果默认关闭，虽然不再依赖TLS，但是它不是线程安全的，如果多线程调用，那么它将发生问题。
* 函数内`static变量`，避免对存在析构的对象使用。
    - 首次调用会触发 `atexit`注册，如果运行环境是`MD`,那么进程退出时可能引起死锁。

### 3.1.3. 性能类要求
* 减少不必要的函数调用，与逻辑。
* 自增，自减尽可能的前置（`++i`, `--i`）,如果i的迭代器，那么这可以显著减少复制开销。
* 庞大的成员作为参数时，优先考虑引用，减少复制开销。
* 少用`std::string`，因为它没有写复制机制，容易引入额外的内存复制开销。
    - `ATL::CString`、`YY::String`、`QString`都是极好的。

## 3.2. 头文件
* 头文件不允许出现 `using namespace XXX:`。

### 3.2.1. 头文件守卫
头文件应该添加守卫，防止头文件重复include，导致编译失败。有2种方法供你选择：
**方法1：#pragma once**
* 优点：使用简单。
* 缺点：兼容性不好，相对路径变化、符号链接或者文件重复（物理上重复）会让机制失效。

```cpp
// 示例 Element.h
#pragma once

// 你的代码

```
**方法2：宏守卫**
* 优点：兼容性好，相对路径变化、符号链接或者文件重复（物理上重复）机制依然有效。
* 缺点：有人复制了你的代码片段或者多个版本共存时容易宏撞车。

```cpp
// 示例 Element.h
// 宏需要拥有足够的复杂度，一般推荐格式：__{作者}_{库名称}_{文件名}
#ifndef __YY_MEGA_UI_ELEMENT_H
#define __YY_MEGA_UI_ELEMENT_H

// 你的代码

#endif // __YY_MEGA_UI_ELEMENT_H
```

> 温馨提示：自己根据需要选择方法，但是不要把方法1与方法2同时出现在一个头文件中！这样没有任何好处，因为最终将受限与宏。

### 3.2.2. 头文件 include
主要按3段论走：
* #include "源文件自身对应的头文件"
* #include <公共库头文件>
  - CRT、STL相关头文件
  - 第三方公共库头文件（比如ZLIB、CURL等） 
  - OS平台相关头文件
* #include "项目内自身头文件"

```cpp
// 示例 Element.cpp
// 源文件自身对应的头文件
#include "Element.h"

// 公共库头文件
// CRT、STL相关头文件
#include <stdint.h>
#include <string>
// 第三方公共库头文件（比如ZLIB、CURL等）
#include <zlib.h>
// OS平台 SDK相关头文件
#include <Windows.h>

// 项目内自身头文件
#include "Value.h"
```
### 3.2.3. 显式指定对齐方式
如果头文件中存在结构体、类、联合体，那么强烈建议大家显式指定对齐方式，减少ABI风险。

为什么我们需要显式指定？
* 默认对齐方式系数是一个编译选项，假如某人把默认对齐方系数式改成 `1`，很显然内部成员的偏移、结构体的大小可能发生改变。
* 某些人的头文件可能会出现不保存现场，直接 `#pragma pack(1)`，然后再引用我们的头文件，如果我们不显式指定，那么就会受到影响。

**示例：**
```cpp
#include "XXXXXX"

// 开头显式指定对齐方式，一般可以指定 4 或者 8 或者 sizeof(void*)
#pragma pack(push, 4)

// 我的代码

// 头文件结束后恢复上次对齐方式
#pragma pack(pop)
```

## 3.3. 命名空间
* 采用帕斯卡命名规则。
* 少用匿名空间。
* 花括号各自独立一行。
* 内部代码，缩进一次（即：4个空格）。
* 尾巴上增加自己空间的名称注释。

```cpp
namespace MegaUI
{
    // 我的代码
} // MegaUI
```

## 3.4. 函数
* 需要对外暴露的请使用命名空间，不需要对外暴露的加上`static`，让外部不可见。
* 原则上，需要遵守垃圾进、垃圾出原则。
    - 如果函数可能失败，那么必须拥有`错误码返回值`，而不是`void`。
    - 如果函数可能失败，内部不可以不管，直接返回一个成功。
    - 某些特殊的函数无法使用错误码，那么再考虑使用`异常`或者`CRT机制中断程序运行`，绝对不可以什么都不做。
* 花括号独立一行，不要与参数的右括号放在一起。
* 函数内容需要缩进。
* 函数签名只有2种形式（效果可以看 `一般形式参考`）
    - 整个签名在一行。
    - 所有内容各自独立一行。

### 3.4.1. 注释风格
* 统一使用`summary`注释函数本体，内容包括，函数的作用、参数怎么使用、返回值的情况详细说明。
    - 如果函数非常简单，那么可以省略。 
    - 如果函数有声明，那么只给声明添加 `summary` 注释即可。

### 3.4.2. 调用约定
* 内部函数推荐显式的声明调用约定为 `__fastcall`，一般来说这样性能更好。
* 外部可见的函数必须显式的指定函数调用约定，避免使用者默认调用约定不一致导致链接失败。

### 3.4.3. 参数约定
* 统一对函数参数使用`SAL`注释。
    - 如果函数有声明，那么只给声明添加 `SAL` 注释即可。 
* 不建议使用引用，因为通过指针返回看起来更加直观。
* 对于参数需要做严格检查，`返回失败`。如果无法返回失败，那么再考虑抛出异常或者`CRT中断程序运行`，绝对不可以什么都不做。

### 3.4.4. 一般形式参考
```cpp
// 如果函数比较短，则可以考虑将签名写在一行内。


/// <summary>获取 XXXX 的数据。</summary>
/// <param name="piData_">用于接收XXX</param>
/// <returns>操作成功则返回 S_OK。</returns>
HRESULT __fastcall GetData(_Out_ int32_t* piData_);

// 实现
HRESULT __fastcall GetData(int32_t* piData_)
{
    // 参数检查
    if(!piData_)
        return E_POINT;

    if(IsXXX())
    {
        *piData_ = 5;
    }
    else if(IsXXX())
    {
        *piData_ = 6;
    }
    else
    {
        *piData_ = 0;
    }
    return S_OK;
}
```

```cpp
// 如果函数比较长，那么可以折行排布，注意新的一行缩进 4 个空格。 花括号也是单独一行。
// SAL注释 类型 参数名称注意对齐。

/// <summary>获取 XXXX 的数据。</summary>
/// <param name="hFile_">数据库句柄</param>
/// <param name="piData_">用于接收XXX</param>
/// <param name="pbTest_">表示 XXXX</param>
/// <returns>操作成功则返回 S_OK。</returns>
HRESULT
__fastcall
GetData(
    _In_  HANDLE   hFile_,
    _Out_ int32_t* piData_,
    _Out_ bool*    pbTest_
    );

// 实现
HRESULT
__fastcall
GetData(
    HANDLE   hFile_,
    int32_t* piData_,
    bool*    pbTest_
    )
{
    // 参数检查
    // ...
    return S_OK;
}
```

```cpp
// 如果是一个内部函数，没有声明，那么函数体本身需要提供`SAL`注释以及`summary`函数注释。

/// <summary>获取 XXXX 的数据。</summary>
/// <param name="hFile_">数据库句柄</param>
/// <param name="piData_">用于接收XXX</param>
/// <param name="pbTest_">表示 XXXX</param>
/// <returns>操作成功则返回 S_OK。</returns>
static
HRESULT
__fastcall
GetDataInternal(
    HANDLE   hFile_,
    int32_t* piData_,
    bool*    pbTest_
    )
{
    // 参数检查
    // ...
    return S_OK;
}
```

# 3.5. 结构体/联合体
* 应该放在自己的命名空间中，减少冲突概率。
* 一般不应该存在函数，只存在数据成员。
* 花括号独立一行，花括号内部需要缩进。
* 自己显式指定对齐方式，减少ABI潜在风险。

# 3.5.1. 结构体示例
```cpp
// 显式指定为 4 对齐
#pragma pack(push, 4)

struct Value
{
    int32_t iData;
};

struct Value2
{
    int32_t iData2;
};

struct Value3 : public Value
{
    int32_t iData3;
};

// 注意 `:` 跟 `,` 
struct Value4
    : public Value
    , public Value2
{
    int32_t iData4;
};

union Value5
{
    int32_t iData;
    int64_t iData2;
};

#pragma pack(pop)

```

# 3.6. 类
* 函数重写必须添加`override`。
* 虚函数必须添加`virtual`修饰。
* 对于类，尽量不要暴露成员变量。
* 成员变量靠前，成员函数靠后。
* 自己显式指定对齐方式，减少ABI潜在风险。

# 3.5.2. 类示例

```cpp
// 显式指定为 4 对齐
#pragma pack(push, 4)

class Value
{
public:
    virtual ~Value();

    virtual int32_t __fastcall GetData() = 0;
};

class Value2
{
public:
    virtual ~Value2();

    /// <summary>获取 XXXX 的数据。</summary>
    /// <param name="piData2_">用于接收XXX</param>
    /// <returns>操作成功则返回 S_OK。</returns>
    virtual HRESULT __fastcall GetData2(_Out_ int32_t* piData2_) = 0;
};

class Value3
    : public Value
    , public Value2
{
private:
    int32_t iData1;
    int32_t iData2;

public:
    Value3()
        : iData1(0)
        , iData2(1)
    {
    }

    virtual ~Value3()
    {
    }

    virtual int32_t __fastcall GetData() override
    {
        return iData1;
    }

    virtual
    HRESULT
    __fastcall
    GetData2(
        _Out_ int32_t* piData2_
        ) override
    {
        if(!piData2_)
            return E_POINTER;
        *piData2_ = iData2;
        return S_OK;
    }
};

#pragma pack(pop)

```

## 3.7. 枚举
* 优先考虑 `enum class`;
* 花括号独立一行，花括号内代码缩进一次。
* 尽量放置在自己的命名空间中。
* 最开始的枚举成员需要拥有初始值，因为大多数编译器初始值是 0，而某些可能是 -1。消除潜在不确定因素。

```cpp
enum class ValueType
{
    Element = 0,
    String,
};
```
