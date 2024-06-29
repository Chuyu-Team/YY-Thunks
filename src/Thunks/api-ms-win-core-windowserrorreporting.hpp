#if (YY_Thunks_Target < __WindowsNT6_1)
#include <werapi.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    8,
    HRESULT,
    WINAPI,
    WerRegisterRuntimeExceptionModule,
        _In_ PCWSTR _szOutOfProcessCallbackDll,
        _In_ PVOID  _pContext
        )
    {
        if (const auto _pfnWerRegisterRuntimeExceptionModule = try_get_WerRegisterRuntimeExceptionModule())
        {
            return _pfnWerRegisterRuntimeExceptionModule(_szOutOfProcessCallbackDll, _pContext);
        }

        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    8,
    HRESULT,
    WINAPI,
    WerUnregisterRuntimeExceptionModule,
        _In_ PCWSTR _szOutOfProcessCallbackDll,
        _In_ PVOID _pContext
        )
    {
        if (const auto _pfnWerUnregisterRuntimeExceptionModule = try_get_WerUnregisterRuntimeExceptionModule())
        {
            return _pfnWerUnregisterRuntimeExceptionModule(_szOutOfProcessCallbackDll, _pContext);
        }
        
        return S_OK;
    }
#endif
}
