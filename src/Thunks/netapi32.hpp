#if (YY_Thunks_Target < __WindowsNT10_10240)
#include <lmjoin.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端    Windows 10 [仅限桌面应用]
    // 最低受支持的服务器    Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    netapi32,
    8,
    HRESULT,
    NET_API_FUNCTION,
    NetGetAadJoinInformation,
        _In_opt_ LPCWSTR _szTenantId,
        _Outptr_result_maybenull_ PDSREG_JOIN_INFO* _ppJoinInfo
        )
    {
        if (const auto _pfnNetGetAadJoinInformation = try_get_NetGetAadJoinInformation())
        {
            return _pfnNetGetAadJoinInformation(_szTenantId, _ppJoinInfo);
        }
        if (!_ppJoinInfo)
            return E_INVALIDARG;

        // 总是认为自己没有加入 Azure AD 帐户。
        *_ppJoinInfo = nullptr;
        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端    Windows 10 [仅限桌面应用]
    // 最低受支持的服务器    Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    netapi32,
    4,
    VOID,
    NET_API_FUNCTION,
    NetFreeAadJoinInformation,
        _In_opt_ PDSREG_JOIN_INFO _pJoinInfo
        )
    {
        if (const auto _pfnNetFreeAadJoinInformation = try_get_NetFreeAadJoinInformation())
        {
            return _pfnNetFreeAadJoinInformation(_pJoinInfo);
        }

        // 什么也不做，老版本系统不可能会拿到这个信息。
        UNREFERENCED_PARAMETER(_pJoinInfo);
    }
#endif
} // namespace YY::Thunks
