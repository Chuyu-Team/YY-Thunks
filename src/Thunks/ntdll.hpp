

#if defined(YY_Thunks_Implemented) && (YY_Thunks_Target < __WindowsNT6)
namespace YY::Thunks::Fallback
{
    static NTSTATUS NTAPI NtCancelIoFileEx(
        HANDLE _hHandle,
        IO_STATUS_BLOCK* _pIo,
        IO_STATUS_BLOCK* _pIoStatus
        )
    {
        // NtCancelIoFile无法模拟特定的IO取消工作，因此需要取消特定IO时立即失败处理
        if (_pIo != nullptr)
        {
            // Not supported
            return STATUS_NOT_FOUND;
        }

    #ifndef __USING_NTDLL_LIB
        const auto NtCancelIoFile = try_get_NtCancelIoFile();
        if (!NtCancelIoFile)
        {
            // 正常来说不应该走到这里
            return STATUS_NOT_SUPPORTED;
        }
    #endif

        auto _Status = NtCancelIoFile(_hHandle, _pIoStatus);
        if (_Status < 0)
            return _Status;

        internal::StringBuffer<char> _Buffer;
        auto _pProcessInfo = internal::GetCurrentProcessInfo(_Buffer);
        if (!_pProcessInfo)
            return _Status;

        const auto _uCurrentThreadId = GetCurrentThreadId();
        for (ULONG i = 0; i != _pProcessInfo->ThreadCount; ++i)
        {
            auto& _Thread = _pProcessInfo->Threads[i];

            if (_uCurrentThreadId == static_cast<DWORD>(reinterpret_cast<UINT_PTR>(_Thread.ClientId.UniqueThread)))
                continue;

            auto _hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, static_cast<DWORD>(reinterpret_cast<UINT_PTR>(_Thread.ClientId.UniqueThread)));
            if (!_hThread)
                continue;

            QueueUserAPC(
                [](ULONG_PTR _hHandle)
                {
    #ifndef __USING_NTDLL_LIB
                    const auto NtCancelIoFile = try_get_NtCancelIoFile();
    #endif
                    IO_STATUS_BLOCK _IoStatus;
                    // 故意不判断空指针，NtCancelIoFileEx开头已经判断过了。
                    // 如果真的崩溃，那么说明内存被改坏了。
                    NtCancelIoFile(reinterpret_cast<HANDLE>(_hHandle), &_IoStatus);
                }, _hThread, (ULONG_PTR)_hHandle);
            CloseHandle(_hThread);
        }

        return _Status;
    }
}
#endif


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
        HANDLE _hHandle,
        IO_STATUS_BLOCK* _pIo,
        IO_STATUS_BLOCK* _pIoStatus
        )
    {
        if (const auto _pfnNtCancelIoFileEx = try_get_NtCancelIoFileEx())
        {
            return _pfnNtCancelIoFileEx(_hHandle, _pIo, _pIoStatus);
        }

        return Fallback::NtCancelIoFileEx(_hHandle, _pIo, _pIoStatus);
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
