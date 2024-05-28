// SymbolBuildTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>

#include <Windows.h>

// DllMainCRTStartupForYY_Thunks是YY-Thunks中的一个内置符号，强制引用它如果链接成功则说明YY-Thunks肯定被依赖上了。
extern "C" BOOL WINAPI DllMainCRTStartupForYY_Thunks(
    HINSTANCE const _hInstance,
    DWORD     const _uReason,
    LPVOID    const _pReserved
    );

int main()
{
    printf("%p\n", &DllMainCRTStartupForYY_Thunks);
    return 0;
}
