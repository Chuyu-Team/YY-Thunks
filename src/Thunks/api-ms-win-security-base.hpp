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
} // namespace YY::Thunks
