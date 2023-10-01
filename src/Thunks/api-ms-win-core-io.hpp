

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

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
		// 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		kernel32,
		24,
		BOOL,
		WINAPI,
		GetQueuedCompletionStatusEx,
			_In_ HANDLE CompletionPort,
			_Out_writes_to_(ulCount,*ulNumEntriesRemoved) LPOVERLAPPED_ENTRY lpCompletionPortEntries,
			_In_ ULONG ulCount,
			_Out_ PULONG ulNumEntriesRemoved,
			_In_ DWORD dwMilliseconds,
			_In_ BOOL fAlertable
			)
		{
			if (const auto _pfnGetQueuedCompletionStatusEx = try_get_GetQueuedCompletionStatusEx())
			{
				return _pfnGetQueuedCompletionStatusEx(CompletionPort, lpCompletionPortEntries, ulCount, ulNumEntriesRemoved, dwMilliseconds, fAlertable);
			}

			if (ulCount == 0 || lpCompletionPortEntries == nullptr || ulNumEntriesRemoved == nullptr)
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}

			*ulNumEntriesRemoved = 0;

			auto& _Entry = lpCompletionPortEntries[0];
			
			// TODO: 已知问题：可警报状态丢失！（fAlertable）
			auto _bRet = GetQueuedCompletionStatus(CompletionPort, &_Entry.dwNumberOfBytesTransferred, &_Entry.lpCompletionKey, &_Entry.lpOverlapped, dwMilliseconds);
			if (_bRet)
			{
				*ulNumEntriesRemoved = 1;
			}
			return _bRet;
		}
#endif
	}//namespace Thunks

} //namespace YY