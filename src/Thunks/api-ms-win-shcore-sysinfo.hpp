#if (YY_Thunks_Target < __WindowsNT6_1)
#include <shellapi.h>
#endif

namespace YY::Thunks {

#if (YY_Thunks_Target < __WindowsNT6_1)

// 最低受支持的客户端	Windows 7 [仅限桌面应用]
// 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
__DEFINE_THUNK(
shell32,
4,
HRESULT,
STDAPICALLTYPE,
SetCurrentProcessExplicitAppUserModelID,
    _In_ PCWSTR _szAppID
    )
{
    if (auto const _pfnSetCurrentProcessExplicitAppUserModelID = try_get_SetCurrentProcessExplicitAppUserModelID())
    {
        return _pfnSetCurrentProcessExplicitAppUserModelID(_szAppID);
    }

    // Windows 7 之前没有任务栏分组与显式 AppUserModelID 语义，兼容层只需假装设置成功即可。
    return S_OK;
}
#endif
} //namespace YY::Thunks
