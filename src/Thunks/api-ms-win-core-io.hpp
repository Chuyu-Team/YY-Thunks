

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

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

        IO_STATUS_BLOCK _IoStatusBlock;
        // 没有CancelIoEx那么也必然没有NtCancelIoFileEx，所以直接调用 Fallback系列即可。
        auto _Status = Fallback::NtCancelIoFileEx(hFile, reinterpret_cast<IO_STATUS_BLOCK*>(lpOverlapped), &_IoStatusBlock);
        if (_Status < 0)
        {
            SetLastError(internal::NtStatusToDosError(_Status));
            return FALSE;
        }
        return TRUE;
    }
#endif
        

#if (YY_Thunks_Target < __WindowsNT6)

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


#if (YY_Thunks_Target < __WindowsNT6)

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
        DWORD _cEntry = 0;
        if (fAlertable)
        {
            constexpr DWORD kMaxSleepTime = 6;
            // 使用 SleepEx 进行等待触发 APC
            if (dwMilliseconds == INFINITE)
            {
                for (;;)
                {
                    for (; _cEntry != ulCount; ++_cEntry)
                    {
                        auto& _Entry = lpCompletionPortEntries[_cEntry];
                        auto _bRet = GetQueuedCompletionStatus(CompletionPort, &_Entry.dwNumberOfBytesTransferred, &_Entry.lpCompletionKey, &_Entry.lpOverlapped, 0);
                        if (!_bRet)
                        {
                            break;
                        }
                    }

                    if (_cEntry)
                        break;

                    if (GetLastError() != WAIT_TIMEOUT)
                    {
                        return FALSE;
                    }

                    if (SleepEx(kMaxSleepTime, TRUE) == WAIT_IO_COMPLETION)
                    {
                        SetLastError(WAIT_IO_COMPLETION);
                        return FALSE;
                    }
                }
            }
            else
            {
                const auto _uStartTick = GetTickCount();
                for (;;)
                {
                    for (; _cEntry != ulCount; ++_cEntry)
                    {
                        auto& _Entry = lpCompletionPortEntries[_cEntry];
                        auto _bRet = GetQueuedCompletionStatus(CompletionPort, &_Entry.dwNumberOfBytesTransferred, &_Entry.lpCompletionKey, &_Entry.lpOverlapped, 0);
                        if (!_bRet)
                        {
                            break;
                        }
                    }

                    if (_cEntry)
                        break;

                    if (GetLastError() != WAIT_TIMEOUT)
                    {
                        return FALSE;
                    }

                    // 计算剩余等待时间，如果剩余等待时间归零则返回
                    const auto _uTickSpan = GetTickCount() - _uStartTick;
                    if (dwMilliseconds > _uTickSpan)
                    {
                        if (SleepEx(min(kMaxSleepTime, dwMilliseconds - _uTickSpan), TRUE) == WAIT_IO_COMPLETION)
                        {
                            SetLastError(WAIT_IO_COMPLETION);
                            return FALSE;
                        }
                    }
                    else
                    {
                        if (SleepEx(0, TRUE) == WAIT_IO_COMPLETION)
                        {
                            SetLastError(WAIT_IO_COMPLETION);
                            return FALSE;
                        }

                        SetLastError(WAIT_TIMEOUT);
                        return FALSE;
                    }
                }
            }
        }
        else
        {
            auto& _FirstEntry = lpCompletionPortEntries[_cEntry];
            if (!GetQueuedCompletionStatus(CompletionPort, &_FirstEntry.dwNumberOfBytesTransferred, &_FirstEntry.lpCompletionKey, &_FirstEntry.lpOverlapped, dwMilliseconds))
                return FALSE;

            ++_cEntry;
            for (; _cEntry != ulCount; ++_cEntry)
            {
                auto& _Entry = lpCompletionPortEntries[_cEntry];
                auto _bRet = GetQueuedCompletionStatus(CompletionPort, &_Entry.dwNumberOfBytesTransferred, &_Entry.lpCompletionKey, &_Entry.lpOverlapped, 0);
                if (!_bRet)
                {
                    break;
                }
            }
        }

        *ulNumEntriesRemoved = _cEntry;
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    20,
    BOOL,
    WINAPI,
    GetOverlappedResultEx,
        _In_  HANDLE       _hFile,
        _In_  LPOVERLAPPED _pOverlapped,
        _Out_ LPDWORD      _puNumberOfBytesTransferred,
        _In_  DWORD        _uMilliseconds,
        _In_  BOOL         _bAlertable
        )
    {
        if (const auto _pfnGetOverlappedResultEx = try_get_GetOverlappedResultEx())
        {
            return _pfnGetOverlappedResultEx(_hFile, _pOverlapped, _puNumberOfBytesTransferred, _uMilliseconds, _bAlertable);
        }

        auto _Status = (NTSTATUS)_pOverlapped->Internal;
        if (_Status == STATUS_PENDING)
        {
            if (_uMilliseconds == 0)
            {
                SetLastError(ERROR_IO_INCOMPLETE);
                return FALSE;
            }

            const auto _uResult = WaitForSingleObjectEx(
                _pOverlapped->hEvent ? _pOverlapped->hEvent : _hFile,
                _uMilliseconds,
                _bAlertable);

            if (_uResult == WAIT_FAILED)
            {
                return FALSE;
            }
            else if (_uResult != WAIT_OBJECT_0)
            {
                SetLastError(_uResult);
                return FALSE;
            }

            _Status = (NTSTATUS)_pOverlapped->Internal;
            if (_Status == STATUS_PENDING)
            {
                _Status = STATUS_SUCCESS;
            }
        }

        *_puNumberOfBytesTransferred = (DWORD)_pOverlapped->InternalHigh;

        if (_Status != STATUS_SUCCESS)
        {
            internal::BaseSetLastNTError(_Status);
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
#endif // (YY_Thunks_Target < __WindowsNT6_2)
} //namespace YY::Thunks
