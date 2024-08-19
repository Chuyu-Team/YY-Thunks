namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

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

#ifndef __USING_NTDLL_LIB
        const auto NtCancelIoFile = try_get_NtCancelIoFile();
        if(!NtCancelIoFile)
        {
            // 正常来说不应该走到这里
            return STATUS_NOT_SUPPORTED;
        }
#endif
        // 最坏打算，清除所有的调用
        return NtCancelIoFile(handle, io_status);    
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	在 Windows 7 和更高版本的 Windows 中可用。
    __DEFINE_THUNK(
    ntdll,
    16,
    NTSTATUS,
    NTAPI,
    NtOpenKeyEx,
        __out PHANDLE _phKeyHandle,
        __in ACCESS_MASK _fDesiredAccess,
        __in POBJECT_ATTRIBUTES _pObjectAttributes,
        __in ULONG _fOpenOptions
        )
    {
        if (const auto _pfnNtOpenKeyEx = try_get_NtOpenKeyEx())
        {
            return _pfnNtOpenKeyEx(_phKeyHandle, _fDesiredAccess, _pObjectAttributes, _fOpenOptions);
        }

#ifndef __USING_NTDLL_LIB
        const auto NtOpenKey = try_get_NtOpenKey();
        if (!NtOpenKey)
        {
            // 正常来说不应该走到这里，XP存在此函数
            return STATUS_NOT_SUPPORTED;
        }
#endif

        if (_fOpenOptions & (~(REG_OPTION_OPEN_LINK | REG_OPTION_BACKUP_RESTORE)))
            return STATUS_INVALID_PARAMETER_4;

        OBJECT_ATTRIBUTES _ObjectAttributes;
        if (_fOpenOptions & REG_OPTION_OPEN_LINK)
        {
            __try
            {
                // 避免 ObjectAttributes 指针非法
                // 我们抓异常为了模拟原版函数行为，内存无效时可以返回错误代码。
                _ObjectAttributes = *_pObjectAttributes;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return GetExceptionCode();
            }

            _ObjectAttributes.Attributes |= OBJ_OPENLINK;
            _pObjectAttributes = &_ObjectAttributes;
        }

        if ((_fOpenOptions & REG_OPTION_BACKUP_RESTORE) == 0)
            return NtOpenKey(_phKeyHandle, _fDesiredAccess, _pObjectAttributes);

#ifndef __USING_NTDLL_LIB
        const auto NtCreateKey = try_get_NtCreateKey();
        const auto NtDeleteKey = try_get_NtDeleteKey();
        const auto NtClose = try_get_NtClose();
        if (NtCreateKey == nullptr || NtDeleteKey == nullptr || NtClose == nullptr)
            return STATUS_NOT_SUPPORTED;
#endif

        // 先测试一下 Key在不在，如果确定不在就没有必要继续了。
        HANDLE _hKey = NULL;
        LONG _Status = NtOpenKey(&_hKey, STANDARD_RIGHTS_READ, _pObjectAttributes);
        if (_Status < 0 && _Status != STATUS_ACCESS_DENIED)
            return _Status;

        do
        {
            ULONG Disposition;
            _Status = NtCreateKey(_phKeyHandle, _fDesiredAccess, _pObjectAttributes, 0, nullptr, REG_OPTION_BACKUP_RESTORE, &Disposition);
            if (_Status < 0)
                break;

            if (Disposition == REG_CREATED_NEW_KEY)
            {
                // 新建的话就重新删除，毕竟NtOpenKeyEx是没有创建功能的。
                NtDeleteKey(*_phKeyHandle);
                NtClose(*_phKeyHandle);
                *_phKeyHandle = NULL;
                _Status = STATUS_OBJECT_NAME_NOT_FOUND;
            }
        } while (false);

        if(_hKey)
            NtClose(_hKey);

        return _Status;
    }
#endif
} // namespace YY::Thunks
