namespace YY
{
    namespace Thunks
    {
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
		// 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
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

			// 最坏打算，清除所有的调用
			if (const auto _pfnNtCancelIoFile = try_get_NtCancelIoFile())
			{
				return _pfnNtCancelIoFile(handle, io_status);
			}
			
			// 正常来说不应该走到这里
			return STATUS_NOT_SUPPORTED;
		}
#endif
    }
}
