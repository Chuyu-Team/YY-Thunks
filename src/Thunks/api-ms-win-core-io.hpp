

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		BOOL,
		WINAPI,
		CancelIoEx,
			_In_ HANDLE hFile,
			_In_opt_ LPOVERLAPPED lpOverlapped
			)
		{
			if (auto pCancelIoEx = try_get_CancelIoEx())
			{
				return pCancelIoEx(hFile, lpOverlapped);
			}

			//downlevel逻辑会把该文件所有IO动作给取消掉！凑合用吧。
			return CancelIo(hFile);
		}
#endif
		

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista [desktop apps | UWP apps]
		//Windows Server 2008 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		BOOL,
		WINAPI,
		CancelSynchronousIo,
			_In_ HANDLE _hThread
			)
		{
			if (const auto _pfnCancelSynchronousIo = try_get_CancelSynchronousIo())
			{
				return _pfnCancelSynchronousIo(_hThread);
			}

			SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
			return FALSE;
		}
#endif
	}//namespace Thunks

} //namespace YY