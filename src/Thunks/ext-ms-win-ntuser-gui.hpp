namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN7)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    16,
    BOOL,
    WINAPI,
    ChangeWindowMessageFilterEx,
        _In_ HWND _hWnd,
        _In_ UINT _uMessage,
        _In_ DWORD _uAction,
        _Inout_opt_ PCHANGEFILTERSTRUCT _pChangeFilterStruct
        )
    {
        if (const auto _pfnChangeWindowMessageFilterEx = try_get_ChangeWindowMessageFilterEx())
        {
            return _pfnChangeWindowMessageFilterEx(_hWnd, _uMessage, _uAction, _pChangeFilterStruct);
        }

        if (_hWnd == NULL || (_pChangeFilterStruct && _pChangeFilterStruct->cbSize != sizeof(CHANGEFILTERSTRUCT)))
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        DWORD _fFlag;
        if (_uAction == MSGFLT_ALLOW || _uAction == MSGFLT_DISALLOW)
        {
            _fFlag = _uAction;
        }
        else if (_uAction == MSGFLT_RESET)
        {
            // 默认情况下大于WM_USER的全部阻止，这是系统规则。
            _fFlag = _uMessage >= WM_USER ? MSGFLT_REMOVE : MSGFLT_ADD;
        }
        else
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        auto _bRet = ChangeWindowMessageFilter(_uMessage, _fFlag);
        if (_bRet && _pChangeFilterStruct)
        {
            _pChangeFilterStruct->ExtStatus = MSGFLTINFO_NONE;
        }

        return _bRet;
    }
#endif
}
