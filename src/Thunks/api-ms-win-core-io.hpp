

namespace YY {
namespace Thunks {

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
// Windows Vista [desktop apps | UWP apps]
// Windows Server 2008 [desktop apps | UWP apps]

EXTERN_C
BOOL WINAPI CancelIoEx(_In_ HANDLE hFile, _In_opt_ LPOVERLAPPED lpOverlapped)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (auto pCancelIoEx = try_get_CancelIoEx())
    {
        return pCancelIoEx(hFile, lpOverlapped);
    }

    // downlevel�߼���Ѹ��ļ�����IO������ȡ�������պ��ðɡ�
    return CancelIo(hFile);
}
#endif

__YY_Thunks_Expand_Function(kernel32, CancelIoEx, 8);

#endif

} // namespace Thunks

} // namespace YY