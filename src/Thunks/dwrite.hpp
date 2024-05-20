#include <dwrite.h>

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN6SP2)

    // 最低受支持的客户端	Windows 7、带 SP2 的 Windows Vista 和适用于 Windows Vista 的平台更新 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2、Windows Server 2008 SP2 和适用于 Windows Server 2008 的平台更新[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    dwrite,
    12,
    HRESULT,
    WINAPI,
    DWriteCreateFactory,
        _In_ DWRITE_FACTORY_TYPE factoryType,
        _In_ REFIID iid,
        _COM_Outptr_ IUnknown **factory
        )
    {
        if (const auto _pfnDWriteCreateFactory = try_get_DWriteCreateFactory())
        {
            return _pfnDWriteCreateFactory(factoryType, iid, factory);
        }
        if (factory)
            *factory = nullptr;
        return E_NOINTERFACE;
    }
#endif
}
