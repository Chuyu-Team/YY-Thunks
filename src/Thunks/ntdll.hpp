namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // 最低受支持的客户端    Windows Vista [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2008[桌面应用 | UWP 应用]
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


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

    // 最低受支持的客户端	在 Windows 7 和更高版本的 Windows 中可用。
    __DEFINE_THUNK(
    ntdll,
    16,
    NTSTATUS,
	NTAPI,
	NtOpenKeyEx,
		__out PHANDLE KeyHandle,
		__in ACCESS_MASK DesiredAccess,
		__in POBJECT_ATTRIBUTES ObjectAttributes,
		__in ULONG OpenOptions
        )
    {
        if (const auto _pfnNtOpenKeyEx = try_get_NtOpenKeyEx())
        {
            return _pfnNtOpenKeyEx(KeyHandle, DesiredAccess, ObjectAttributes, OpenOptions);
        }

        const auto _pfnNtOpenKey = try_get_NtOpenKey();
        if (!_pfnNtOpenKey)
        {
            // 正常来说不应该走到这里，XP存在此函数
            return STATUS_NOT_SUPPORTED;
        }

        if (OpenOptions == 0)
        {
            return _pfnNtOpenKey(KeyHandle, DesiredAccess, ObjectAttributes);
        }

        if (OpenOptions & (~(REG_OPTION_OPEN_LINK | REG_OPTION_BACKUP_RESTORE)))
        {
            return STATUS_INVALID_PARAMETER_4;
        }

        const auto _pfnNtCreateKey = try_get_NtCreateKey();
        const auto _pfnNtDeleteKey = try_get_NtDeleteKey();
        const auto _pfnNtClose = try_get_NtClose();
        if (_pfnNtCreateKey == nullptr || _pfnNtDeleteKey == nullptr || _pfnNtClose == nullptr)
        {
            return STATUS_NOT_SUPPORTED;
        }

        // 先测试一下 Key在不在
        HANDLE _hKey = NULL;
        LONG _Status = _pfnNtOpenKey(&_hKey, DesiredAccess, ObjectAttributes);
        if (_Status < 0 && _Status != STATUS_ACCESS_DENIED)
        {
            return _Status;
        }

        do
        {
            ULONG Disposition;
            _Status = _pfnNtCreateKey(KeyHandle, DesiredAccess, ObjectAttributes, 0, nullptr, OpenOptions, &Disposition);
            if (_Status < 0)
                break;

            if (Disposition == REG_CREATED_NEW_KEY)
            {
                // 新建的话就重新删除
                _pfnNtDeleteKey(*KeyHandle);
                _pfnNtClose(*KeyHandle);
                *KeyHandle = NULL;
                _Status = STATUS_OBJECT_NAME_NOT_FOUND;
            }
        } while (false);

        if(_hKey)
            _pfnNtClose(_hKey);

        return _Status;
    }
#endif
} // namespace YY::Thunks
