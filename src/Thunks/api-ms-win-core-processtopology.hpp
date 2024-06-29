#if (YY_Thunks_Target < __WindowsNT6_1)
#include <processtopologyapi.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_1)

    //Minimum supported client	Windows 7 [desktop apps only]
    //Minimum supported server	Windows Server 2008 R2 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    GetProcessGroupAffinity,
        _In_ HANDLE _hProcess,
        _Inout_ PUSHORT _pGroupCount,
        _Out_writes_(*_pGroupCount) PUSHORT _pGroupArray
        )
    {
        if (const auto _pfnGetProcessGroupAffinity = try_get_GetProcessGroupAffinity())
        {
            return _pfnGetProcessGroupAffinity(_hProcess, _pGroupCount, _pGroupArray);
        }

        // 更低版本系统不支持 CPU组，所以我们可以视为系统只有一组处理器。
        if (_pGroupCount == nullptr || _pGroupArray == nullptr)
        {
            SetLastError(ERROR_NOACCESS);
            return FALSE;
        }

        if (*_pGroupCount < 1)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        *_pGroupCount = 1;
        _pGroupArray[0] = 0;
        return TRUE;
    }
#endif
} // namespace YY::Thunks
