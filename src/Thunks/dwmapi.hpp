#include <dwmapi.h>

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
	}
}
