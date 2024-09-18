

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows XP Professional x64 Edition, Windows Server 2003 SP1
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    Wow64DisableWow64FsRedirection,
        _Out_ PVOID* OldValue
        )
    {
        if (auto const pWow64DisableWow64FsRedirection = try_get_Wow64DisableWow64FsRedirection())
        {
            return pWow64DisableWow64FsRedirection(OldValue);
        }


        SetLastError(ERROR_INVALID_FUNCTION);

        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows XP Professional x64 Edition, Windows Server 2003 SP1
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    Wow64RevertWow64FsRedirection,
        _In_ PVOID OlValue
        )
    {
        if (auto const pWow64RevertWow64FsRedirection = try_get_Wow64RevertWow64FsRedirection())
        {
            return pWow64RevertWow64FsRedirection(OlValue);
        }


        SetLastError(ERROR_INVALID_FUNCTION);

        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2)

    //Windows XP Professional x64 Edition, Windows Server 2003
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOLEAN,
    WINAPI,
    Wow64EnableWow64FsRedirection,
        _In_ BOOLEAN Wow64FsEnableRedirection
        )
    {
        if (auto const pWow64EnableWow64FsRedirection = try_get_Wow64EnableWow64FsRedirection())
        {
            return pWow64EnableWow64FsRedirection(Wow64FsEnableRedirection);
        }


        SetLastError(ERROR_INVALID_FUNCTION);

        return FALSE;

    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1)

    //Windows XP, Windows Server 2003
    __DEFINE_THUNK(
    kernel32,
    8,
    BOOL,
    WINAPI,
    IsWow64Process,
        _In_ HANDLE hProcess,
        _Out_ PBOOL Wow64Process
        )
    {
        if (auto const pIsWow64Process = try_get_IsWow64Process())
        {
            return pIsWow64Process(hProcess, Wow64Process);
        }


        *Wow64Process = FALSE;

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_16299)

    // 最低受支持的客户端	Windows 10版本 1709 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2016版本 1709[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    _Success_(return)
    BOOL,
    WINAPI,
    IsWow64Process2,
        _In_      HANDLE  hProcess,
        _Out_     USHORT* pProcessMachine,
        _Out_opt_ USHORT* pNativeMachine
        )
    {
        if (auto const pIsWow64Process2 = try_get_IsWow64Process2())
        {
            return pIsWow64Process2(hProcess, pProcessMachine, pNativeMachine);
        }


        //判断是否运行在Wow6432虚拟机
        BOOL bWow64Process;
        auto bRet = IsWow64Process(hProcess, &bWow64Process);

        if (bRet)
        {
            if (bWow64Process)
            {
                *pProcessMachine = IMAGE_FILE_MACHINE_I386;

                //IA64已经哭晕在厕所
                if (pNativeMachine)
                    *pNativeMachine = IMAGE_FILE_MACHINE_AMD64;
            }
            else
            {
                *pProcessMachine = IMAGE_FILE_MACHINE_UNKNOWN;

#if defined _X86_
                if (pNativeMachine)
                    *pNativeMachine = IMAGE_FILE_MACHINE_I386;
#elif defined _AMD64_
                if (pNativeMachine)
                    *pNativeMachine = IMAGE_FILE_MACHINE_AMD64;
#else
    #error 不支持此体系
#endif
            }
        }

        return bRet;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_16299)

    //Windows 10, version 1709
    __DEFINE_THUNK(
    kernel32,
    8,
    _Must_inspect_result_
    HRESULT,
    WINAPI,
    IsWow64GuestMachineSupported,
        _In_ USHORT WowGuestMachine,
        _Out_ BOOL* MachineIsSupported
        )
    {
        if (auto const pIsWow64GuestMachineSupported = try_get_IsWow64GuestMachineSupported())
        {
            return pIsWow64GuestMachineSupported(WowGuestMachine, MachineIsSupported);
        }

        if (IMAGE_FILE_MACHINE_I386 == WowGuestMachine)
        {
#ifdef _AMD64_
            *MachineIsSupported = TRUE;
#else
            SYSTEM_INFO SystemInfo;
            GetNativeSystemInfo(&SystemInfo);

            *MachineIsSupported = SystemInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL;
#endif

        }
        else
        {
            *MachineIsSupported = FALSE;
        }

        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Windows XP x64确认没有这个接口
    __DEFINE_THUNK(
    kernel32,
    8,
    BOOL,
    WINAPI,
    Wow64GetThreadContext,
        _In_ HANDLE         _hThread,
        _Out_ PWOW64_CONTEXT _pContext
        )
    {
        if (const auto _pfnWow64GetThreadContext = try_get_Wow64GetThreadContext())
        {
            return _pfnWow64GetThreadContext(_hThread, _pContext);
        }

        /*
        | 系统   | 线程类型 | 结果
        | ------ | -------- | ---
        | x86    | x86      | ERROR_INVALID_PARAMETER
        | x64    | x86      | ERROR_SUCCESS
        | x64    | x64      | ERROR_INVALID_PARAMETER
        */

#if defined(_X86_)
        // 如果系统也是 x86这必然不可能处于Wow64
        if (internal::GetNativeSystemInfo().wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        static_assert(sizeof(CONTEXT) == sizeof(*_pContext));
        return GetThreadContext(_hThread, (CONTEXT*)_pContext);
#elif defined(_AMD64_)
        if (!_pContext)
        {
            SetLastError(ERROR_NOACCESS);
            return FALSE;
        }

        if ((_pContext->ContextFlags & (~(WOW64_CONTEXT_ALL | WOW64_CONTEXT_XSTATE))) != 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        // 判断线程是否是 WOW64线程
        const auto _pfnNtQueryInformationThread = try_get_NtQueryInformationThread();
        if (!_pfnNtQueryInformationThread)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return FALSE;
        }

        THREAD_BASIC_INFORMATION ThreadBasicInfo;
        LONG Status = _pfnNtQueryInformationThread(_hThread, ThreadBasicInformation, &ThreadBasicInfo, sizeof(ThreadBasicInfo), nullptr);
        if (Status < 0)
        {
            internal::BaseSetLastNTError(Status);
            return FALSE;
        }

        // 当前进程肯定不是Wow64
        if (static_cast<DWORD>(reinterpret_cast<UINT_PTR>(ThreadBasicInfo.ClientId.UniqueProcess)) == GetCurrentProcessId())
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        union
        {
            struct
            {
                DWORD Unknow;
                WOW64_CONTEXT ThreadContext;
            } RemoteWow64ThreadContext;

            CONTEXT Context64;
        } ContextBuffer = {};

        CONTEXT& _Context64 = ContextBuffer.Context64;
        _Context64.ContextFlags = CONTEXT_CONTROL | (_pContext->ContextFlags & 0x1F /*WOW64_CONTEXT_CONTROL | WOW64_CONTEXT_INTEGER | WOW64_CONTEXT_SEGMENTS | WOW64_CONTEXT_FLOATING_POINT | WOW64_CONTEXT_DEBUG_REGISTERS*/);

        if (_pContext->ContextFlags & 0x20 /*WOW64_CONTEXT_EXTENDED_REGISTERS*/)
        {
            _Context64.ContextFlags |= CONTEXT_FLOATING_POINT;
        }

        // 这里忽略Windows XP必然不支持 WOW64_CONTEXT_XSTATE
        if (!GetThreadContext(_hThread, &_Context64))
        {
            return FALSE;
        }

        if (_Context64.SegCs == 0x23u)
        {
            // 当前正处于WOW64执行中，这时取到的上下文就是 WOW64上下文
            // 我们可以直接将 CONTEXT 转换到 WOW64_CONTEXT
            if ((_pContext->ContextFlags & WOW64_CONTEXT_DEBUG_REGISTERS) == WOW64_CONTEXT_DEBUG_REGISTERS)
            {
                _pContext->Dr0 = static_cast<DWORD>(_Context64.Dr0);
                _pContext->Dr1 = static_cast<DWORD>(_Context64.Dr1);
                _pContext->Dr2 = static_cast<DWORD>(_Context64.Dr2);
                _pContext->Dr3 = static_cast<DWORD>(_Context64.Dr3);
                _pContext->Dr6 = static_cast<DWORD>(_Context64.Dr6);
                _pContext->Dr7 = static_cast<DWORD>(_Context64.Dr7);
            }

            if ((_pContext->ContextFlags & WOW64_CONTEXT_FLOATING_POINT) == WOW64_CONTEXT_FLOATING_POINT)
            {
                _pContext->FloatSave.ControlWord = _Context64.FltSave.ControlWord;
                _pContext->FloatSave.StatusWord = _Context64.FltSave.StatusWord;
                _pContext->FloatSave.TagWord = _Context64.FltSave.TagWord;
                _pContext->FloatSave.ErrorOffset = _Context64.FltSave.ErrorOffset;
                _pContext->FloatSave.ErrorSelector = _Context64.FltSave.ErrorSelector;
                _pContext->FloatSave.DataOffset = _Context64.FltSave.DataOffset;
                _pContext->FloatSave.DataSelector = _Context64.FltSave.DataSelector;
                _pContext->FloatSave.Cr0NpxState = 0;

                constexpr auto kFloatRegisterByteSize = sizeof(_pContext->FloatSave.RegisterArea) / _countof(_Context64.FltSave.FloatRegisters);   
                for (size_t i = 0; i != _countof(_Context64.FltSave.FloatRegisters); ++i)
                {
                    memcpy(_pContext->FloatSave.RegisterArea + i * kFloatRegisterByteSize, &_Context64.FltSave.FloatRegisters[i], kFloatRegisterByteSize);
                }
            }

            if ((_pContext->ContextFlags & WOW64_CONTEXT_SEGMENTS) == WOW64_CONTEXT_SEGMENTS)
            {
                _pContext->SegGs = 0x2Bu;
                _pContext->SegFs = 0x53u;
                _pContext->SegEs = 0x2Bu;
                _pContext->SegDs = 0x2Bu;
            }

            if ((_pContext->ContextFlags & WOW64_CONTEXT_INTEGER) == WOW64_CONTEXT_INTEGER)
            {
                _pContext->Edi = static_cast<DWORD>(_Context64.Rdi);
                _pContext->Esi = static_cast<DWORD>(_Context64.Rsi);
                _pContext->Ebx = static_cast<DWORD>(_Context64.Rbx);
                _pContext->Edx = static_cast<DWORD>(_Context64.Rdx);
                _pContext->Ecx = static_cast<DWORD>(_Context64.Rcx);
                _pContext->Eax = static_cast<DWORD>(_Context64.Rax);
            }

            if ((_pContext->ContextFlags & WOW64_CONTEXT_CONTROL) == WOW64_CONTEXT_CONTROL)
            {
                _pContext->Ebp = static_cast<DWORD>(_Context64.Rbp);
                _pContext->Eip = static_cast<DWORD>(_Context64.Rip);
                _pContext->SegCs = 0x23u;
                _pContext->EFlags = _Context64.EFlags & 0x3F0DD5u | 0x202u;
                _pContext->Esp = static_cast<DWORD>(_Context64.Rsp);
                _pContext->SegSs = 0x2Bu;
            }

            if ((_pContext->ContextFlags & WOW64_CONTEXT_EXTENDED_REGISTERS) == WOW64_CONTEXT_EXTENDED_REGISTERS)
            {
                struct DECLSPEC_ALIGN(16) _XSAVE_FORMAT_WOW64
                {
                    WORD   ControlWord;
                    WORD   StatusWord;
                    BYTE  TagWord;
                    BYTE  Reserved1;
                    WORD   ErrorOpcode;
                    DWORD ErrorOffset;
                    WORD   ErrorSelector;
                    WORD   Reserved2;
                    DWORD DataOffset;
                    WORD   DataSelector;
                    WORD   Reserved3;
                    DWORD MxCsr;
                    DWORD MxCsr_Mask;
                    M128A FloatRegisters[8];

                    M128A XmmRegisters[8];
                };
                
                memcpy(_pContext->ExtendedRegisters, &_Context64.FltSave, sizeof(_XSAVE_FORMAT_WOW64));
            }

            return TRUE;
        }
        else if (_Context64.SegCs == 0x33u)
        {
            // 虽然大多数 x86程序地址最大只有2G，但是开启大地址时也可以达到接近4G
            // 如果RSP大于32位上限，那么这明显就是一个64位程序。
            if (_Context64.Rsp > 0xffff0000ull)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            auto _hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, static_cast<DWORD>(reinterpret_cast<UINT_PTR>(ThreadBasicInfo.ClientId.UniqueProcess)));
            if (_hProcess == NULL)
            {
                return FALSE;
            }

            LSTATUS _lStatus = ERROR_SUCCESS;

            do
            {
                void* _pRemoteThreadWow64Context = nullptr;

                auto _pRemoteThreadTeb = (TEB*)ThreadBasicInfo.TebBaseAddress;
                // TEB->TlsSlots[1](TEB + 0x1488)指向一个 WOW64_CONTEXT，具体可以参考 wow64cpu.dll!CpuGetContext
                static_assert((UFIELD_OFFSET(TEB, TlsSlots) + 1 * sizeof(TEB::TlsSlots[0])) == 0x1488, "");
                if (!ReadProcessMemory(_hProcess, &_pRemoteThreadTeb->TlsSlots[1], &_pRemoteThreadWow64Context, sizeof(_pRemoteThreadWow64Context), nullptr))
                {
                    _lStatus = ERROR_FUNCTION_FAILED;
                    break;
                }
                if (!_pRemoteThreadWow64Context)
                {
                    _lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }

                memset(&ContextBuffer.RemoteWow64ThreadContext, 0, sizeof(ContextBuffer.RemoteWow64ThreadContext));
                if (!ReadProcessMemory(_hProcess, _pRemoteThreadWow64Context, &ContextBuffer.RemoteWow64ThreadContext, sizeof(ContextBuffer.RemoteWow64ThreadContext), nullptr))
                {
                    _lStatus = ERROR_FUNCTION_FAILED;
                    break;
                }

                auto& _Wow64ThreadContext = ContextBuffer.RemoteWow64ThreadContext.ThreadContext;
                if ((_Wow64ThreadContext.ContextFlags & WOW64_CONTEXT_i386) == 0)
                {
                    _lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }

                if ((_pContext->ContextFlags & WOW64_CONTEXT_DEBUG_REGISTERS) == WOW64_CONTEXT_DEBUG_REGISTERS)
                {
                    _pContext->Dr0 = _Wow64ThreadContext.Dr0;
                    _pContext->Dr1 = _Wow64ThreadContext.Dr1;
                    _pContext->Dr2 = _Wow64ThreadContext.Dr2;
                    _pContext->Dr3 = _Wow64ThreadContext.Dr3;
                    _pContext->Dr6 = _Wow64ThreadContext.Dr6;
                    _pContext->Dr7 = _Wow64ThreadContext.Dr7;
                }

                if ((_pContext->ContextFlags & WOW64_CONTEXT_FLOATING_POINT) == WOW64_CONTEXT_FLOATING_POINT)
                {
                    memcpy(&_pContext->FloatSave, &_Wow64ThreadContext.FloatSave, sizeof(_Wow64ThreadContext.FloatSave));
                }

                if ((_pContext->ContextFlags & WOW64_CONTEXT_SEGMENTS) == WOW64_CONTEXT_SEGMENTS)
                {
                    _pContext->SegGs = 0x2Bu;
                    _pContext->SegFs = 0x53u;
                    _pContext->SegEs = 0x2Bu;
                    _pContext->SegDs = 0x2Bu;
                }

                if ((_pContext->ContextFlags & WOW64_CONTEXT_INTEGER) == WOW64_CONTEXT_INTEGER)
                {
                    _pContext->Edi = _Wow64ThreadContext.Edi;
                    _pContext->Esi = _Wow64ThreadContext.Esi;
                    _pContext->Ebx = _Wow64ThreadContext.Ebx;
                    _pContext->Edx = _Wow64ThreadContext.Edx;
                    _pContext->Ecx = _Wow64ThreadContext.Ecx;
                    _pContext->Eax = _Wow64ThreadContext.Eax;
                }

                if ((_pContext->ContextFlags & WOW64_CONTEXT_CONTROL) == WOW64_CONTEXT_CONTROL)
                {
                    _pContext->Ebp = _Wow64ThreadContext.Ebp;
                    _pContext->Eip = _Wow64ThreadContext.Eip;
                    _pContext->SegCs = 0x23u;
                    _pContext->EFlags = _Wow64ThreadContext.EFlags & 0x3F0DD5u | 0x202u;
                    _pContext->Esp = _Wow64ThreadContext.Esp;
                    _pContext->SegSs = 0x2Bu;
                }

                if ((_pContext->ContextFlags & WOW64_CONTEXT_EXTENDED_REGISTERS) == WOW64_CONTEXT_EXTENDED_REGISTERS)
                {
                    memcpy(_pContext->ExtendedRegisters, _Wow64ThreadContext.ExtendedRegisters, sizeof(_Wow64ThreadContext.ExtendedRegisters));
                }

            } while (false);
            CloseHandle(_hProcess);

            if (_lStatus)
            {
                SetLastError(_lStatus);
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
        else
        {
            // 非预期值。
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }       
#else
#error ????
#endif
    }
#endif
} //namespace YY::Thunks
