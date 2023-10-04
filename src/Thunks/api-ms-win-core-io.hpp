

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
			
            if (fAlertable)
            {
                // 使用 WaitForSingleObjectEx 进行等待触发 APC
                auto _uStartTick = GetTickCount();
                for (;;)
                {
                    const auto _uResult = WaitForSingleObjectEx(CompletionPort, dwMilliseconds, TRUE);
                    if (_uResult == WAIT_OBJECT_0)
                    {
                        // 完成端口有数据了
                        auto _bRet = GetQueuedCompletionStatus(CompletionPort, &_Entry.dwNumberOfBytesTransferred, &_Entry.lpCompletionKey, &_Entry.lpOverlapped, 0);
                        if (_bRet)
                        {
                            *ulNumEntriesRemoved = 1;
                            break;
                        }

                        if (GetLastError() != WAIT_TIMEOUT)
                        {
                            return FALSE;
                        }

                        // 无限等待时无脑继续等即可。
                        if (dwMilliseconds == INFINITE)
                        {
                            continue;
                        }

                        // 计算剩余等待时间，如果剩余等待时间归零则返回
                        const DWORD _uTickSpan = GetTickCount() - _uStartTick;
                        if (_uTickSpan >= dwMilliseconds)
                        {
                            SetLastError(WAIT_TIMEOUT);
                            return FALSE;
                        }
                        dwMilliseconds -= _uTickSpan;
                        _uStartTick += _uTickSpan;
                        continue;
                    }
                    else if (_uResult == WAIT_IO_COMPLETION || _uResult == WAIT_TIMEOUT)
                    {
                        // 很奇怪，微软原版遇到 APC唤醒直接会设置 LastError WAIT_IO_COMPLETION
                        // 遇到超时，LastError WAIT_TIMEOUT（注意不是预期的 ERROR_TIMEOUT）不知道是故意还是有意。
                        SetLastError(_uResult);
                        return FALSE;
                    }
                    else if (_uResult == WAIT_ABANDONED)
                    {
                        SetLastError(ERROR_ABANDONED_WAIT_0);
                        return FALSE;
                    }
                    else if (_uResult == WAIT_FAILED)
                    {
                        // LastError
                        return FALSE;
                    }
                    else
                    {
                        // LastError ???
                        return FALSE;
                    }
                }

                return TRUE;
            }
            else
            {
                auto _bRet = GetQueuedCompletionStatus(CompletionPort, &_Entry.dwNumberOfBytesTransferred, &_Entry.lpCompletionKey, &_Entry.lpOverlapped, dwMilliseconds);
                if (_bRet)
                {
                    *ulNumEntriesRemoved = 1;
                }
                return _bRet;
            }
		}
#endif
	}//namespace Thunks

} //namespace YY
