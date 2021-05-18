

namespace YY {
namespace Thunks {
#if (YY_Thunks_Support_Version < NTDDI_WINXP)
EXTERN_C
DWORD
WINAPI
WTSGetActiveConsoleSessionId(VOID)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (const auto pWTSGetActiveConsoleSessionId = try_get_WTSGetActiveConsoleSessionId())
    {
        return pWTSGetActiveConsoleSessionId();
    }

    //��ΪWindows 2000û�лỰ���룬���еĽ���ʼ���� 0 �Ự�����У����ֱ�ӷ��� 0 ���ɡ�
    return 0;
}
#endif

__YY_Thunks_Expand_Function(kernel32, WTSGetActiveConsoleSessionId, 0);

#endif
} // namespace Thunks
} // namespace YY