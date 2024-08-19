namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    AddClipboardFormatListener,
        _In_ HWND _hWnd
        )
    {
        if (const auto _pfnAddClipboardFormatListener = try_get_AddClipboardFormatListener())
        {
            return _pfnAddClipboardFormatListener(_hWnd);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    RemoveClipboardFormatListener,
        _In_ HWND _hWnd
        )
    {
        if (const auto _pfnRemoveClipboardFormatListener = try_get_RemoveClipboardFormatListener())
        {
            return _pfnRemoveClipboardFormatListener(_hWnd);
        }

        return TRUE;
    }
#endif
}
