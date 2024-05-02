namespace YY
{
    namespace Thunks
    {
#if (YY_Thunks_Support_Version < NTDDI_WIN6SP1)

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


#if (YY_Thunks_Support_Version < NTDDI_WIN6SP1)

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

        
#if (YY_Thunks_Support_Version < NTDDI_WIN6SP1)

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

#if (YY_Thunks_Support_Version < NTDDI_WIN6SP1)

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


#if (YY_Thunks_Support_Version < NTDDI_WIN10_FE)

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
    }
}
