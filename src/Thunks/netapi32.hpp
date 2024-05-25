#if (YY_Thunks_Support_Version < NTDDI_WIN10)
#include <lmjoin.h>
#endif

namespace YY::Thunks
{

#if (YY_Thunks_Support_Version < NTDDI_WIN10)

        // �����֧�ֵĿͻ���    Windows 10 [��������Ӧ��]
        // �����֧�ֵķ�����    Windows Server 2016[��������Ӧ��]
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

            // ������Ϊ�Լ�û�м��� Azure AD �ʻ���
            *_ppJoinInfo = nullptr;
            return S_OK;
        }
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10)

        // �����֧�ֵĿͻ���    Windows 10 [��������Ӧ��]
        // �����֧�ֵķ�����    Windows Server 2016[��������Ӧ��]
        __DEFINE_THUNK(
        netapi32,
        8,
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

            // ʲôҲ�������ϰ汾ϵͳ�����ܻ��õ������Ϣ��
            UNREFERENCED_PARAMETER(_pJoinInfo);
        }
#endif

} // namespace YY::Thunks
