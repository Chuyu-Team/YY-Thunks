namespace YY::Thunks
{

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

        // �����֧�ֵĿͻ���    Windows Vista [����Ӧ��|UWP Ӧ��]
        // �����֧�ֵķ�����    Windows Server 2008[����Ӧ�� | UWP Ӧ��]
        __DEFINE_THUNK(
        ntdll,
        12,
        NTSTATUS,
        NTAPI,
        NtCancelIoFileEx,
            HANDLE handle,
            IO_STATUS_BLOCK* io,
            IO_STATUS_BLOCK* io_status
            )
        {
            if (const auto _pfnNtCancelIoFileEx = try_get_NtCancelIoFileEx())
            {
                return _pfnNtCancelIoFileEx(handle, io, io_status);
            }

            // ����㣬������еĵ���
            if (const auto _pfnNtCancelIoFile = try_get_NtCancelIoFile())
            {
                return _pfnNtCancelIoFile(handle, io_status);
            }

            // ������˵��Ӧ���ߵ�����
            return STATUS_NOT_SUPPORTED;
        }
#endif

} // namespace YY::Thunks
