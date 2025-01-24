namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)
#ifndef __C65AB19A_FFA2_4BB6_B2D2_508A6509AD7A__
#define __C65AB19A_FFA2_4BB6_B2D2_508A6509AD7A__
    struct INFO_EFB07CE8_C478_475C_9B6D_1862D6400474
    {
        HWND hwndNextViewer = NULL;
        HWND Listener = NULL;
    };
    auto KLASS_4420FB75_2931_459E_BA36_B2484F6DC9EE = TEXT("4420FB75_2931_459E_BA36_B2484F6DC9EE");
    auto PROP_5B7BE8DE_E87B_4FC7_8562_48E4E42880DB = TEXT("5B7BE8DE_E87B_4FC7_8562_48E4E42880DB");
    LRESULT CALLBACK WNDPROC_8DDD0332_D337_4F76_AF3C_D0CF23E94191(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_DRAWCLIPBOARD:
        case WM_CHANGECBCHAIN:
        case WM_DESTROY:
        {
            auto info = reinterpret_cast<INFO_EFB07CE8_C478_475C_9B6D_1862D6400474 *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            if (!info)
                return DefWindowProc(hwnd, msg, wParam, lParam);
            switch (msg)
            {
            case WM_DRAWCLIPBOARD:
            {
                if (info->hwndNextViewer)
                    SendMessage(info->hwndNextViewer, msg, wParam, lParam);
                PostMessage(info->Listener, WM_CLIPBOARDUPDATE, 0, 0);
            }
            break;
            case WM_CHANGECBCHAIN:
            {
                if (((HWND)wParam == info->hwndNextViewer))
                    info->hwndNextViewer = (HWND)lParam;
                else if (info->hwndNextViewer)
                    SendMessage(info->hwndNextViewer, msg, wParam, lParam);
            }
            break;
            case WM_DESTROY:
            {
                if (info->hwndNextViewer)
                    ChangeClipboardChain(hwnd, info->hwndNextViewer);
                delete info;
            }
            break;
            }
        }
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }
#endif
#endif
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

        WNDCLASS wc = {};
        ZeroMemory(&wc, sizeof(WNDCLASS));
        wc.lpfnWndProc = WNDPROC_8DDD0332_D337_4F76_AF3C_D0CF23E94191;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)wc.lpfnWndProc, &wc.hInstance);
        wc.lpszClassName = KLASS_4420FB75_2931_459E_BA36_B2484F6DC9EE;
        RegisterClass(&wc);
        auto hwnd = CreateWindowEx(0, KLASS_4420FB75_2931_459E_BA36_B2484F6DC9EE, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
        if (!hwnd)
            return FALSE;
        auto info = new INFO_EFB07CE8_C478_475C_9B6D_1862D6400474{};
        info->Listener = _hWnd;
        info->hwndNextViewer = SetClipboardViewer(hwnd); // WM_DRAWCLIPBOARD会立即通知一次，但AddClipboardFormatListener不会
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)info);
        SetProp(_hWnd, PROP_5B7BE8DE_E87B_4FC7_8562_48E4E42880DB, hwnd);
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

        auto hwnd = (HWND)GetProp(_hWnd, PROP_5B7BE8DE_E87B_4FC7_8562_48E4E42880DB);
        if (!hwnd)
            return TRUE;
        DestroyWindow(hwnd);
        RemoveProp(_hWnd, PROP_5B7BE8DE_E87B_4FC7_8562_48E4E42880DB);
        return TRUE;
    }
#endif
}
