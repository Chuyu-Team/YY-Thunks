// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

#include <sal.h>

// 添加要在此处预编译的标头
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define UMDF_USING_NTSTATUS
#define _Disallow_YY_KM_Namespace
#include <km.h>

#include <Windows.h>

#include <atltime.h>

#include "CppUnitTest.h"
#include <process.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define __DEFINE_THUNK(_MODULE, _SIZE, _RETURN_, _CONVENTION_, _FUNCTION, ...)     \
    extern bool _CRT_CONCATENATE(aways_null_try_get_, _FUNCTION);                  \
	EXTERN_C _RETURN_ _CONVENTION_ _FUNCTION(__VA_ARGS__);                         \
	__if_not_exists(_FUNCTION)

#endif //PCH_H
