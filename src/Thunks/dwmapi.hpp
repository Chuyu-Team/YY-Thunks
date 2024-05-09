#include <dwmapi.h>

// 忽略deprecated，因为是Thunks项目肯定是需要调用老接口的。
#pragma warning(disable:4995)

namespace YY
{
	namespace Thunks
	{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

        // 最低受支持的客户端	Windows Vista [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
        __DEFINE_THUNK(
        dwmapi,
        8,
		HRESULT,
		STDAPICALLTYPE,
		DwmEnableBlurBehindWindow,
			HWND _hWnd,
			_In_ const DWM_BLURBEHIND* _pBlurBehind
			)
		{
			if (const auto _pfnDwmEnableBlurBehindWindow = try_get_DwmEnableBlurBehindWindow())
			{
				return _pfnDwmEnableBlurBehindWindow(_hWnd, _pBlurBehind);
			}
            
            // 老版本系统不支持Dwm,因此假装此功能被禁用
			return DWM_E_COMPOSITIONDISABLED;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

        // 最低受支持的客户端	Windows Vista [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
        __DEFINE_THUNK(
        dwmapi,
        4,
        HRESULT,
        STDAPICALLTYPE,
        DwmIsCompositionEnabled,
            _Out_ BOOL* _pbEnabled
            )
        {
            if (const auto _pfnDwmIsCompositionEnabled = try_get_DwmIsCompositionEnabled())
            {
                return _pfnDwmIsCompositionEnabled(_pbEnabled);
            }

            if (!_pbEnabled)
            {
                return E_INVALIDARG;
            }

            *_pbEnabled = FALSE;
            return S_OK;
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

        // 最低受支持的客户端	Windows Vista [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
        __DEFINE_THUNK(
        dwmapi,
        4,
        HRESULT,
        STDAPICALLTYPE,
        DwmEnableComposition,
            UINT _uCompositionAction
            )
        {
            if (const auto _pfnDwmEnableComposition = try_get_DwmEnableComposition())
            {
                return _pfnDwmEnableComposition(_uCompositionAction);
            }

            if (DWM_EC_DISABLECOMPOSITION == _uCompositionAction)
            {
                return S_OK;
            }
            else if (DWM_EC_ENABLECOMPOSITION == _uCompositionAction)
            {
                return DWM_E_COMPOSITIONDISABLED;
            }
            else
            {
                return E_INVALIDARG;
            }
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

        // 最低受支持的客户端	Windows Vista [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
        __DEFINE_THUNK(
        dwmapi,
        8,
        HRESULT,
        STDAPICALLTYPE,
        DwmExtendFrameIntoClientArea,
            HWND _hWnd,
            _In_ const MARGINS* _pMarInset
            )
        {
            if (const auto _pfnDwmExtendFrameIntoClientArea = try_get_DwmExtendFrameIntoClientArea())
            {
                return _pfnDwmExtendFrameIntoClientArea(_hWnd, _pMarInset);
            }

            if (!_pMarInset)
            {
                return E_INVALIDARG;
            }
            return DWM_E_COMPOSITIONDISABLED;
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

        // 最低受支持的客户端	Windows Vista [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
        __DEFINE_THUNK(
        dwmapi,
        20,
        BOOL,
        STDAPICALLTYPE,
        DwmDefWindowProc,
            _In_ HWND _hWnd,
            UINT _uMsg,
            WPARAM _wParam,
            LPARAM _lParam,
            _Out_ LRESULT* _plResult
            )
        {
            if (const auto _pfnDwmDefWindowProc = try_get_DwmDefWindowProc())
            {
                return _pfnDwmDefWindowProc(_hWnd, _uMsg, _wParam, _lParam, _plResult);
            }

            return FALSE;
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

        // 最低受支持的客户端	Windows Vista [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
        __DEFINE_THUNK(
        dwmapi,
        8,
        HRESULT,
        STDAPICALLTYPE,
        DwmGetColorizationColor,
            _Out_ DWORD* _pcrColorization,
            _Out_ BOOL* _pbOpaqueBlend
            )
        {
            if (const auto _pfnDwmGetColorizationColor = try_get_DwmGetColorizationColor())
            {
                return _pfnDwmGetColorizationColor(_pcrColorization, _pbOpaqueBlend);
            }

            if (_pcrColorization == nullptr || _pbOpaqueBlend == nullptr)
            {
                return E_INVALIDARG;
            }

            return DWM_E_COMPOSITIONDISABLED;
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

        // 最低受支持的客户端	Windows Vista [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
        __DEFINE_THUNK(
        dwmapi,
        16,
        HRESULT,
        STDAPICALLTYPE,
        DwmGetWindowAttribute,
            HWND _hWnd,
            DWORD _uAttribute,
            _Out_writes_bytes_(_cbAttribute) PVOID _pvAttribute,
            DWORD _cbAttribute
            )
        {
            if (const auto _pfnDwmGetWindowAttribute = try_get_DwmGetWindowAttribute())
            {
                return _pfnDwmGetWindowAttribute(_hWnd, _uAttribute, _pvAttribute, _cbAttribute);
            }
            
            if (!_pvAttribute)
            {
                return E_INVALIDARG;
            }
            return DWM_E_COMPOSITIONDISABLED;
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

        // 最低受支持的客户端	Windows Vista [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
        __DEFINE_THUNK(
        dwmapi,
        16,
        HRESULT,
        STDAPICALLTYPE,
        DwmSetWindowAttribute,
            HWND _hWnd,
            DWORD _uAttribute,
            _In_reads_bytes_(_cbAttribute) LPCVOID _pvAttribute,
            DWORD _cbAttribute
            )
        {
            if (const auto _pfnDwmSetWindowAttribute = try_get_DwmSetWindowAttribute())
            {
                return _pfnDwmSetWindowAttribute(_hWnd, _uAttribute, _pvAttribute, _cbAttribute);
            }

            if (!_pvAttribute)
            {
                return E_INVALIDARG;
            }
            
            return DWM_E_COMPOSITIONDISABLED;
        }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

        // 最低受支持的客户端	Windows Vista [仅限桌面应用]
        // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
        __DEFINE_THUNK(
        dwmapi,
        0,
        HRESULT,
        STDAPICALLTYPE,
        DwmFlush,
            )
        {
            if (const auto _pfnDwmFlush = try_get_DwmFlush())
            {
                return _pfnDwmFlush();
            }
            
            return S_OK;
        }
#endif
    } // namespace Thunks
} // namespace YY
