namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_1_SP1)

    // 最低受支持的客户端	Windows 7 SP1[桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2 SP1[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    0,
    DWORD64,
    WINAPI,
    GetEnabledXStateFeatures,
        VOID
        )
    {
        if (auto const _pfnGetEnabledXStateFeatures = try_get_GetEnabledXStateFeatures())
        {
            return _pfnGetEnabledXStateFeatures();
        }

        static DWORD64 s_uXState = 0;
        if (s_uXState)
        {
            return s_uXState;
        }

        DWORD64 _uXState = XSTATE_MASK_LEGACY_FLOATING_POINT;
        if (IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE))
        {
            _uXState |= XSTATE_MASK_LEGACY_SSE;
        }

        s_uXState = _uXState;
        return _uXState;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1_SP1)

    // 最低受支持的客户端	Windows 7 SP1 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2 SP1[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    SetXStateFeaturesMask,
        _Inout_ PCONTEXT Context,
        _In_ DWORD64 FeatureMask
        )
    {
        if (auto const _pfnSetXStateFeaturesMask = try_get_SetXStateFeaturesMask())
        {
            return _pfnSetXStateFeaturesMask(Context, FeatureMask);
        }
        const auto _uSupportFeatures =  GetEnabledXStateFeatures();
            
        if (FeatureMask & (~_uSupportFeatures))
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
#endif

        
#if (YY_Thunks_Target < __WindowsNT6_1_SP1)

    // 最低受支持的客户端	Windows 7 SP1 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2 SP1[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    PVOID,
    WINAPI,
    LocateXStateFeature,
        _In_ PCONTEXT Context,
        _In_ DWORD FeatureId,
        _Out_opt_ PDWORD Length
        )
    {
        if (auto const _pfnLocateXStateFeature = try_get_LocateXStateFeature())
        {
            return _pfnLocateXStateFeature(Context, FeatureId, Length);
        }

#if defined(_X86_)
        auto& _XSave = *(XSAVE_FORMAT*)Context->ExtendedRegisters;
        constexpr auto kThreadContext = CONTEXT_i386;
            
#elif defined(_AMD64_)
        auto& _XSave = Context->FltSave;
        constexpr auto kThreadContext = CONTEXT_AMD64;
#else
#error unknow OS.
#endif
        if (kThreadContext & Context->ContextFlags)
        {
            if (XSTATE_LEGACY_FLOATING_POINT == FeatureId)
            {
                if (Length)
                {
                    *Length = UFIELD_OFFSET(XSAVE_FORMAT, XmmRegisters);
                }
                return &_XSave;
            }
            else if (XSTATE_LEGACY_SSE == FeatureId)
            {
                if (IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE))
                {
                    if (Length)
                    {
                        *Length = sizeof(_XSave.XmmRegisters);
                    }
                    return &_XSave.XmmRegisters;
                }
            }
        }

        if (Length)
            *Length = 0;
        return nullptr;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_1_SP1)

    // 最低受支持的客户端	Windows 7 SP1 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2 SP1[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    InitializeContext,
        _Out_writes_bytes_opt_(*ContextLength) PVOID Buffer,
        _In_ DWORD ContextFlags,
        _Out_ PCONTEXT* Context,
        _Inout_ PDWORD ContextLength
        )
    {
        if (auto const _pfnInitializeContext = try_get_InitializeContext())
        {
            return _pfnInitializeContext(Buffer, ContextFlags, Context, ContextLength);
        }

        constexpr auto kSupportContextFlags = CONTEXT_ALL | CONTEXT_EXCEPTION_ACTIVE | CONTEXT_SERVICE_ACTIVE | CONTEXT_EXCEPTION_REQUEST | CONTEXT_EXCEPTION_REPORTING;

        if (ContextFlags & (~kSupportContextFlags))
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if (Buffer == nullptr || *ContextLength < sizeof(CONTEXT))
        {
            *ContextLength = sizeof(CONTEXT);
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        auto _pContext = reinterpret_cast<PCONTEXT>(Buffer);
        _pContext->ContextFlags = ContextFlags;
        *ContextLength = sizeof(CONTEXT);
        *Context = _pContext;
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_20348)

    // 最低受支持的客户端	Windows 10内部版本 20348
    // 最低受支持的服务器	Windows 10内部版本 20348
    __DEFINE_THUNK(
    kernel32,
    24,
    BOOL,
    WINAPI,
    InitializeContext2,
        _Out_writes_bytes_opt_(*ContextLength) PVOID Buffer,
        _In_ DWORD ContextFlags,
        _Out_ PCONTEXT* Context,
        _Inout_ PDWORD ContextLength,
        _In_ ULONG64 XStateCompactionMask
        )
    {
        if (auto const _pfnInitializeContext2 = try_get_InitializeContext2())
        {
            return _pfnInitializeContext2(Buffer, ContextFlags, Context, ContextLength, XStateCompactionMask);
        }

        return InitializeContext(Buffer, ContextFlags, Context, ContextLength);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1_SP1)

    // 最低受支持的客户端	Windows 7 SP1 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2 SP1[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    CopyContext,
        _Inout_ PCONTEXT _pDestination,
        _In_ DWORD _uContextFlags,
        _In_ PCONTEXT _pSource
        )
    {
        if (auto const _pfnCopyContext = try_get_CopyContext())
        {
            return _pfnCopyContext(_pDestination, _uContextFlags, _pSource);
        }
            
#if defined(_X86_)
        constexpr auto kThreadContext = CONTEXT_i386;

#elif defined(_AMD64_)
        constexpr auto kThreadContext = CONTEXT_AMD64;
#else
#error unknow OS.
#endif
        if ((_uContextFlags & kThreadContext) == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if ((CONTEXT_XSTATE & _uContextFlags) == CONTEXT_XSTATE)
        {
            // internal::BaseSetLastNTError(0xC00000BB);
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }

        if ((_pSource->ContextFlags & kThreadContext) == 0 || (_pDestination->ContextFlags & kThreadContext) == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if ((CONTEXT_DEBUG_REGISTERS & _uContextFlags) == CONTEXT_DEBUG_REGISTERS && (CONTEXT_DEBUG_REGISTERS & _pSource->ContextFlags) == CONTEXT_DEBUG_REGISTERS)
        {
            _pDestination->ContextFlags |= CONTEXT_DEBUG_REGISTERS;
            _pDestination->Dr0 = _pSource->Dr0;
            _pDestination->Dr1 = _pSource->Dr1;
            _pDestination->Dr2 = _pSource->Dr2;
            _pDestination->Dr3 = _pSource->Dr3;
            _pDestination->Dr6 = _pSource->Dr6;
            _pDestination->Dr7 = _pSource->Dr7;
#if defined(_AMD64_)
            _pDestination->LastBranchToRip = _pSource->LastBranchToRip;
            _pDestination->LastBranchFromRip = _pSource->LastBranchFromRip;
            _pDestination->LastExceptionToRip = _pSource->LastExceptionToRip;
            _pDestination->LastExceptionFromRip = _pSource->LastExceptionFromRip;
#endif
        }

        if ((CONTEXT_FLOATING_POINT & _uContextFlags) == CONTEXT_FLOATING_POINT && (CONTEXT_FLOATING_POINT & _pSource->ContextFlags) == CONTEXT_FLOATING_POINT)
        {
            _pDestination->ContextFlags |= CONTEXT_FLOATING_POINT;
#if defined(_X86_)
            memcpy(&_pDestination->FloatSave, &_pSource->FloatSave, sizeof(_pSource->FloatSave));
#elif defined(_AMD64_)
            _pDestination->MxCsr = _pSource->MxCsr;
            memcpy(&_pDestination->FltSave, &_pSource->FltSave, sizeof(_pSource->FltSave));
#else
#error unknow OS.
#endif
        }

        if ((CONTEXT_SEGMENTS & _uContextFlags) == CONTEXT_SEGMENTS && (CONTEXT_SEGMENTS & _pSource->ContextFlags) == CONTEXT_SEGMENTS)
        {
            _pDestination->ContextFlags |= CONTEXT_SEGMENTS;
            _pDestination->SegGs = _pSource->SegGs;
            _pDestination->SegFs = _pSource->SegFs;
            _pDestination->SegEs = _pSource->SegEs;
            _pDestination->SegDs = _pSource->SegDs;
        }

        if ((CONTEXT_INTEGER & _uContextFlags) == CONTEXT_INTEGER && (CONTEXT_INTEGER & _pSource->ContextFlags) == CONTEXT_INTEGER)
        {
            _pDestination->ContextFlags |= CONTEXT_INTEGER;
#if defined(_X86_)
            _pDestination->Edi = _pSource->Edi;
            _pDestination->Esi = _pSource->Esi;
            _pDestination->Ebx = _pSource->Ebx;
            _pDestination->Edx = _pSource->Edx;
            _pDestination->Ecx = _pSource->Ecx;
            _pDestination->Eax = _pSource->Eax;
#elif defined(_AMD64_)
            _pDestination->Rax = _pSource->Rax;
            _pDestination->Rcx = _pSource->Rcx;
            _pDestination->Rdx = _pSource->Rdx;
            _pDestination->Rbx = _pSource->Rbx;
            _pDestination->Rbp = _pSource->Rbp;
            _pDestination->Rsi = _pSource->Rsi;
            _pDestination->Rdi = _pSource->Rdi;
            _pDestination->R8 = _pSource->R8;
            _pDestination->R9 = _pSource->R9;
            _pDestination->R10 = _pSource->R10;
            _pDestination->R11 = _pSource->R11;
            _pDestination->R12 = _pSource->R12;
            _pDestination->R13 = _pSource->R13;
            _pDestination->R14 = _pSource->R14;
            _pDestination->R15 = _pSource->R15;
#else
#error unknow OS.
#endif
        }

        if ((CONTEXT_CONTROL & _uContextFlags) == CONTEXT_CONTROL && (CONTEXT_CONTROL & _pSource->ContextFlags) == CONTEXT_CONTROL)
        {
            _pDestination->ContextFlags |= CONTEXT_CONTROL;
#if defined(_X86_)
            _pDestination->Ebp = _pSource->Ebp;
            _pDestination->Eip = _pSource->Eip;
            _pDestination->SegCs = _pSource->SegCs;
            _pDestination->EFlags = _pSource->EFlags;
            _pDestination->Esp = _pSource->Esp;
            _pDestination->SegSs = _pSource->SegSs;
#elif defined(_AMD64_)
            _pDestination->SegCs = _pSource->SegCs;
            _pDestination->SegSs = _pSource->SegSs;
            _pDestination->EFlags = _pSource->EFlags;
            _pDestination->Rsp = _pSource->Rsp;
            _pDestination->Rip = _pSource->Rip;
#else
#error unknow OS.
#endif
        }
 
#if defined(CONTEXT_EXTENDED_REGISTERS)
        if ((CONTEXT_EXTENDED_REGISTERS & _uContextFlags) == CONTEXT_EXTENDED_REGISTERS && (CONTEXT_EXTENDED_REGISTERS & _pSource->ContextFlags) == CONTEXT_EXTENDED_REGISTERS)
        {
            memcpy(_pDestination->ExtendedRegisters, _pSource->ExtendedRegisters, sizeof(_pSource->ExtendedRegisters));
        }
#endif
        return TRUE;
    }
#endif
}
