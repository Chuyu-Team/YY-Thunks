namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    advapi32,
    20,
    BOOL,
    WINAPI,
    AddMandatoryAce,
        _Inout_ PACL _pAcl,
        _In_ DWORD _uAceRevision,
        _In_ DWORD _fAceFlags,
        _In_ DWORD _uMandatoryPolicy,
        _In_ SID* _pLabelSid
        )
    {
        if (const auto _pfnAddMandatoryAce = try_get_AddMandatoryAce())
        {
            return _pfnAddMandatoryAce(_pAcl, _uAceRevision, _fAceFlags, _uMandatoryPolicy, _pLabelSid);
        }
        
        // 参考 ntdll.RtlAddMandatoryAce 实现

        if (!_pAcl)
        {
            // internal::BaseSetLastNTError(0xC0000077);
            SetLastError(ERROR_INVALID_ACL);
            return FALSE;
        }

#if !defined(__USING_NTDLL_LIB)
        const auto RtlValidSid = try_get_RtlValidSid();
        const auto RtlValidAcl = try_get_RtlValidAcl();
        const auto RtlFirstFreeAce = try_get_RtlFirstFreeAce();
        const auto RtlCopySid = try_get_RtlCopySid();
        if (RtlValidSid == nullptr || RtlValidAcl == nullptr || RtlFirstFreeAce == nullptr || RtlCopySid == nullptr)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return FALSE;
        }
#endif

        if (!RtlValidSid(_pLabelSid))
        {
            // internal::BaseSetLastNTError(0xC0000078);
            SetLastError(ERROR_INVALID_SID);
            return FALSE;
        }

        static const SID_IDENTIFIER_AUTHORITY s_TargetIdentifierAuthority = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x10} };      
        static_assert(sizeof(s_TargetIdentifierAuthority) == sizeof(SID::IdentifierAuthority));

        if (memcmp(_pLabelSid->IdentifierAuthority.Value, s_TargetIdentifierAuthority.Value, sizeof(s_TargetIdentifierAuthority.Value)) != 0)
        {
            // internal::BaseSetLastNTError(0xC000000D);
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if (_pAcl->AclRevision > ACL_REVISION_DS || _uAceRevision > ACL_REVISION_DS)
        {
            // internal::BaseSetLastNTError(0xC0000059);
            SetLastError(ERROR_REVISION_MISMATCH);
            return FALSE;
        }

        _uAceRevision = max(_pAcl->AclRevision, _uAceRevision);

        if ((_fAceFlags & ~VALID_INHERIT_FLAGS) || (_uMandatoryPolicy & ~SYSTEM_MANDATORY_LABEL_VALID_MASK))
        {
            // internal::BaseSetLastNTError(0xC000000D);
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        _SYSTEM_MANDATORY_LABEL_ACE* _pFirstFree = nullptr;
        if (RtlValidAcl(_pAcl) == FALSE || RtlFirstFreeAce(_pAcl,(PVOID*)& _pFirstFree) == FALSE)
        {
            // internal::BaseSetLastNTError(0xC0000077);
            SetLastError(ERROR_INVALID_ACL);
            return FALSE;
        }

        const WORD _cbData =  (_pLabelSid->SubAuthorityCount + 4) * sizeof(DWORD);
        if (_pFirstFree == nullptr || PBYTE(_pFirstFree) + _cbData > PBYTE(_pAcl) + _pAcl->AclSize)
        {
            // internal::BaseSetLastNTError(0xC0000099);
            SetLastError(ERROR_ALLOTTED_SPACE_EXCEEDED);
            return FALSE;
        }

        _pFirstFree->Header.AceType = 0x11;
        _pFirstFree->Header.AceFlags = (BYTE)_fAceFlags;
        _pFirstFree->Header.AceSize = _cbData;
        _pFirstFree->Mask = _uMandatoryPolicy;
        RtlCopySid(sizeof(DWORD) * _pLabelSid->SubAuthorityCount + 8, &_pFirstFree->SidStart, _pLabelSid);
        _pAcl->AceCount++;
        _pAcl->AclRevision = (BYTE)_uAceRevision;

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    advapi32,
    20,
    BOOL,
    WINAPI,
    GetTokenInformation,
        _In_ HANDLE _hTokenHandle,
        _In_ TOKEN_INFORMATION_CLASS _eTokenInformationClass,
        _Out_writes_bytes_to_opt_(_cbTokenInformationLength,*_puReturnLength) LPVOID _pTokenInformation,
        _In_ DWORD _cbTokenInformationLength,
        _Out_ PDWORD _puReturnLength
        )
    {
        const auto _pfnGetTokenInformation = try_get_GetTokenInformation();
        if (!_pfnGetTokenInformation)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return FALSE;
        }

        if (internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            switch (_eTokenInformationClass)
            {
            case TokenVirtualizationAllowed:
            {
                // 如果令牌允许虚拟化，则缓冲区会收到一个非零的 DWORD 值。
                // 假装不允许虚拟化。
                using ValueType = DWORD;
                auto _pValue = (ValueType*)_pTokenInformation;
                if (_puReturnLength)
                {
                    *_puReturnLength = sizeof(ValueType);
                }

                if (_cbTokenInformationLength < sizeof(ValueType))
                {
                    SetLastError(ERROR_INSUFFICIENT_BUFFER);
                    return FALSE;
                }

                if (!_pValue)
                {
                    SetLastError(ERROR_NOACCESS);
                    return FALSE;
                }

                *_pValue = 0;
                return TRUE;
            }
            case TokenVirtualizationEnabled:
            {
                // 如果为令牌启用了虚拟化，则缓冲区会收到一个非零的 DWORD 值。
                // 假装没有启动虚拟化。
                using ValueType = DWORD;
                auto _pValue = (ValueType*)_pTokenInformation;
                if (_puReturnLength)
                {
                    *_puReturnLength = sizeof(ValueType);
                }

                if (_cbTokenInformationLength < sizeof(ValueType))
                {
                    SetLastError(ERROR_INSUFFICIENT_BUFFER);
                    return FALSE;
                }

                if (!_pValue)
                {
                    SetLastError(ERROR_NOACCESS);
                    return FALSE;
                }

                *_pValue = 0;
                return TRUE;
            }
            case TokenIntegrityLevel:
            {
                // 缓冲区接收指定令牌完整性级别的 TOKEN_MANDATORY_LABEL 结构。
                // 我们假装当前进程是高完整性。
                struct ValueType : public TOKEN_MANDATORY_LABEL
                {
                    SID SidBuffer;
                };
                auto _pValue = (ValueType*)_pTokenInformation;
                if (_puReturnLength)
                {
                    *_puReturnLength = sizeof(ValueType);
                }

                if (_cbTokenInformationLength < sizeof(ValueType))
                {
                    SetLastError(ERROR_INSUFFICIENT_BUFFER);
                    return FALSE;
                }

                if (!_pValue)
                {
                    SetLastError(ERROR_NOACCESS);
                    return FALSE;
                }

                static constexpr SID kHighLevel =
                {
                    SID_REVISION, 1, {SECURITY_MANDATORY_LABEL_AUTHORITY}, {SECURITY_MANDATORY_HIGH_RID}
                };

                _pValue->SidBuffer = kHighLevel;
                _pValue->Label.Sid = &_pValue->SidBuffer;
                _pValue->Label.Attributes = SE_GROUP_INTEGRITY | SE_GROUP_INTEGRITY_ENABLED;
                return TRUE;
            }
            case TokenUIAccess:
            {
                // 如果令牌设置了 UIAccess 标志，缓冲区将收到非零的 DWORD 值。
                // XP没有 TokenUIAccess的说法，假装自己没有这个权限。
                using ValueType = DWORD;
                auto _pValue = (ValueType*)_pTokenInformation;
                if (_puReturnLength)
                {
                    *_puReturnLength = sizeof(ValueType);
                }

                if (_cbTokenInformationLength < sizeof(ValueType))
                {
                    SetLastError(ERROR_INSUFFICIENT_BUFFER);
                    return FALSE;
                }

                if (!_pValue)
                {
                    SetLastError(ERROR_NOACCESS);
                    return FALSE;
                }
                *_pValue = 0;
                return TRUE;
            }
            case TokenAppContainerSid:
            {
                // 缓冲区接收包含与令牌关联的 AppContainerSid 的 TOKEN_APPCONTAINER_INFORMATION 结构。
                // 如果令牌未与应用容器关联，则TOKEN_APPCONTAINER_INFORMATION结构的 TokenAppContainer 成员指向 NULL。
                // 假装自己不是AppContainer

                using ValueType = TOKEN_APPCONTAINER_INFORMATION;
                auto _pValue = (ValueType*)_pTokenInformation;
                if (_puReturnLength)
                {
                    *_puReturnLength = sizeof(ValueType);
                }

                if (_cbTokenInformationLength < sizeof(ValueType))
                {
                    SetLastError(ERROR_INSUFFICIENT_BUFFER);
                    return FALSE;
                }

                if (!_pValue)
                {
                    SetLastError(ERROR_NOACCESS);
                    return FALSE;
                }

                _pValue->TokenAppContainer = nullptr;
                return TRUE;
            }
            case TokenIsAppContainer:
            {
                // 如果令牌是应用容器令牌，则缓冲区接收非零 的 DWORD 值。 
                // 检查 TokenIsAppContainer 并使其返回 0 的任何调用方还应验证调用方令牌是否不是标识级别模拟令牌。 
                // 如果当前令牌不是应用容器，而是标识级令牌，则应返回 AccessDenied。
                // 假装自己不是 AppContainer
                using ValueType = DWORD;
                auto _pValue = (ValueType*)_pTokenInformation;
                if (_puReturnLength)
                {
                    *_puReturnLength = sizeof(ValueType);
                }

                if (_cbTokenInformationLength < sizeof(ValueType))
                {
                    SetLastError(ERROR_INSUFFICIENT_BUFFER);
                    return FALSE;
                }

                if (!_pValue)
                {
                    SetLastError(ERROR_NOACCESS);
                    return FALSE;
                }

                *_pValue = 0;
                return TRUE;
            }
            case TokenAppContainerNumber:
            {
                // 缓冲区接收包含令牌的应用容器号的 DWORD 值。 对于不是应用容器令牌的令牌，此值为零。
                // 假装自己不是AppContainer
                using ValueType = DWORD;
                auto _pValue = (ValueType*)_pTokenInformation;
                if (_puReturnLength)
                {
                    *_puReturnLength = sizeof(ValueType);
                }

                if (_cbTokenInformationLength < sizeof(ValueType))
                {
                    SetLastError(ERROR_INSUFFICIENT_BUFFER);
                    return FALSE;
                }

                if (!_pValue)
                {
                    SetLastError(ERROR_NOACCESS);
                    return FALSE;
                }

                *_pValue = 0;
                return TRUE;
            }
            }
        }

        return _pfnGetTokenInformation(_hTokenHandle, _eTokenInformationClass, _pTokenInformation, _cbTokenInformationLength, _puReturnLength);
    }
#endif

} // namespace YY::Thunks
