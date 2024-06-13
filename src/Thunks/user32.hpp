#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS2)
#include <shellscalingapi.h>
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1) && !defined(__Comment_Lib_user32)
#define __Comment_Lib_user32
#pragma comment(lib, "User32.lib")
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS2) && (YY_Thunks_Support_Version >= NTDDI_WINBLUE || __YY_Thunks_libs) && !defined(__Comment_Lib_shcore)
#define __Comment_Lib_shcore
#pragma comment(lib, "Shcore.lib")
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1) && !defined(__Comment_Lib_gdi32)
#define __Comment_Lib_gdi32
#pragma comment(lib, "Gdi32.lib")
#endif

#if defined(YY_Thunks_Implemented)
namespace YY::Thunks::internal
{
	namespace
	{
#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1)
		static UINT __fastcall GetDpiForSystemDownlevel()
		{
			static int nDPICache = 0;

			if (nDPICache)
			{
				return nDPICache;
			}

			int nDpiX = USER_DEFAULT_SCREEN_DPI;

			if (HDC hdc = GetDC(NULL))
			{
				nDpiX = GetDeviceCaps(hdc, LOGPIXELSX);
				ReleaseDC(NULL, hdc);
			}

			return nDPICache = nDpiX;
		}
#endif
	}
}
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WS03SP1)

	//Windows XP with SP2, Windows Server 2003 with SP1
	__DEFINE_THUNK(
	user32,
	0,
	BOOL,
	WINAPI,
	IsWow64Message,
		VOID
		)
	{
		if (auto const pIsWow64Message = try_get_IsWow64Message())
		{
			return pIsWow64Message();
		}

		return FALSE;
	}
#endif
		

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	// 最低受支持的客户端	Windows Vista [仅限桌面应用]
	// 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
	__DEFINE_THUNK(
	user32,
	0,
	BOOL,
	WINAPI,
	IsProcessDPIAware,
		void)
	{
		if (const auto _pfnIsProcessDPIAware = try_get_IsProcessDPIAware())
		{
			return _pfnIsProcessDPIAware();
		}

		// XP 无法感知DPI，返回的Rect始终被系统缩放了。
		return FALSE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Windows Vista [desktop apps only]
	//Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	user32,
	0,
	BOOL,
	WINAPI,
	SetProcessDPIAware,
		VOID
		)
	{
		if (auto const pSetProcessDPIAware = try_get_SetProcessDPIAware())
		{
			return pSetProcessDPIAware();
		}

		// 假装成功，其实我们都知道，我们没有成功……
		// XP系统无法感知 DPI
		return TRUE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS2)

	//Windows 10, version 1703 [desktop apps only]
	//Windows Server 2016 [desktop apps only]
	__DEFINE_THUNK(
	user32,
	4,
	BOOL,
	WINAPI,
	SetProcessDpiAwarenessContext,
		_In_ DPI_AWARENESS_CONTEXT value
		)
	{
		if (auto const pSetProcessDpiAwarenessContext = try_get_SetProcessDpiAwarenessContext())
		{
			return pSetProcessDpiAwarenessContext(value);
		}

		LSTATUS lStatus;

		do
		{
			PROCESS_DPI_AWARENESS DpiAwareness;

			if (DPI_AWARENESS_CONTEXT_UNAWARE == value
				|| DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED == value)
			{
				DpiAwareness = PROCESS_DPI_UNAWARE;
			}
			else if (DPI_AWARENESS_CONTEXT_SYSTEM_AWARE == value)
			{
				DpiAwareness = PROCESS_SYSTEM_DPI_AWARE;
			}
			else if (DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE == value
				|| DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 == value)
			{
				DpiAwareness = PROCESS_PER_MONITOR_DPI_AWARE;
			}
			else
			{
				lStatus = ERROR_INVALID_PARAMETER;
				break;
			}

			auto hr = SetProcessDpiAwareness(DpiAwareness);

			if (SUCCEEDED(hr))
			{
				return TRUE;
			}

			//将 HRESULT 错误代码转换到 LSTATUS
			if (hr & 0xFFFF0000)
			{
				if (HRESULT_FACILITY(hr) == FACILITY_WIN32)
				{
					lStatus = HRESULT_CODE(hr);
				}
				else
				{
					lStatus = ERROR_FUNCTION_FAILED;
				}
			}
			else
			{
				lStatus = hr;
			}

		} while (false);

			
		SetLastError(lStatus);
		return FALSE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1)

	//Windows 10, version 1607 [desktop apps only]
	__DEFINE_THUNK(
	user32,
	0,
	UINT,
	WINAPI,
	GetDpiForSystem,
		VOID
		)
	{
		if (auto const pGetDpiForSystem = try_get_GetDpiForSystem())
		{
			return pGetDpiForSystem();
		}

		return internal::GetDpiForSystemDownlevel();
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1)

	//Windows 10, version 1607 [desktop apps only]
	__DEFINE_THUNK(
	user32,
	4,
	UINT,
	WINAPI,
	GetDpiForWindow,
		_In_ HWND hwnd
		)
	{
		if (auto const pGetDpiForWindow = try_get_GetDpiForWindow())
		{
			return pGetDpiForWindow(hwnd);
		}

		if (HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY))
		{
			UINT nDpiX, nDpiY;
			if (SUCCEEDED(GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &nDpiX, &nDpiY)))
			{
				return nDpiX;
			}
		}

		return internal::GetDpiForSystemDownlevel();
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1)

	//Windows 10, version 1607 [desktop apps only]
	__DEFINE_THUNK(
	user32,
	8,
	int,
	WINAPI,
	GetSystemMetricsForDpi,
		_In_ int nIndex,
		_In_ UINT dpi
		)
	{
		if (auto const pGetSystemMetricsForDpi = try_get_GetSystemMetricsForDpi())
		{
			return pGetSystemMetricsForDpi(nIndex, dpi);
		}
	
		auto nValue = GetSystemMetrics(nIndex);

		if (nValue != 0)
		{
			switch (nIndex)
			{
			case SM_CYSIZE:
			case SM_CXVSCROLL:
			case SM_CYSMICON:
			case SM_CYVSCROLL:
			case SM_CXPADDEDBORDER:
			case SM_CXSMICON:
			case SM_CYSMSIZE:
			case SM_CYICON:
			case SM_CYHSCROLL:
			case SM_CYMENUCHECK:
			case SM_CYCAPTION:
			case SM_CXHSCROLL:
			case SM_CXFRAME:
			case SM_CYMENUSIZE:
			case SM_CYFRAME:
			case SM_CYMENU:
			case SM_CXICON:
			case SM_CXICONSPACING:
			case SM_CYICONSPACING:
			case SM_CYVTHUMB:
			case SM_CXHTHUMB:
			case SM_CXCURSOR:
			case SM_CYCURSOR:
			case SM_CXMIN:
			case SM_CXMINTRACK:
			case SM_CYMIN:
			case SM_CYMINTRACK:
			case SM_CXSIZE:
			case SM_CYSMCAPTION:
			case SM_CXSMSIZE:
			case SM_CXMENUSIZE:
			case SM_CXMENUCHECK:
			{
				auto nDpiX = internal::GetDpiForSystemDownlevel();
				if (nDpiX != dpi)
				{
					nValue *= dpi;
					nValue += nDpiX / 2;
					nValue /= nDpiX;
				}

				break;
			}
			default:
				break;
			}
		}

		return nValue;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1)

	//Windows 10, version 1607 [desktop apps only]
	__DEFINE_THUNK(
	user32,
	20,
	BOOL,
	WINAPI,
	AdjustWindowRectExForDpi,
		_Inout_ LPRECT lpRect,
		_In_ DWORD dwStyle,
		_In_ BOOL bMenu,
		_In_ DWORD dwExStyle,
		_In_ UINT dpi
		)
	{
		if (auto const pAdjustWindowRectExForDpi = try_get_AdjustWindowRectExForDpi())
		{
			return pAdjustWindowRectExForDpi(lpRect, dwStyle, bMenu, dwExStyle, dpi);
		}

		RECT FrameRect = {};

		if (!AdjustWindowRectEx(&FrameRect, dwStyle, bMenu, dwExStyle))
		{
			return FALSE;
		}

		auto nDpiX = internal::GetDpiForSystemDownlevel();

		if (nDpiX != dpi)
		{
			FrameRect.left = MulDiv(FrameRect.left, dpi, nDpiX);
			FrameRect.top = MulDiv(FrameRect.top, dpi, nDpiX);
			FrameRect.right = MulDiv(FrameRect.right, dpi, nDpiX);
			FrameRect.bottom = MulDiv(FrameRect.bottom, dpi, nDpiX);
		}

		lpRect->left += FrameRect.left;
		lpRect->top += FrameRect.top;
		lpRect->right += FrameRect.right;
		lpRect->bottom += FrameRect.bottom;

		return TRUE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS1)

	//Windows 10, version 1607 [desktop apps only]
	__DEFINE_THUNK(
	user32,
	20,
	BOOL,
	WINAPI,
	SystemParametersInfoForDpi,
		_In_ UINT uiAction,
		_In_ UINT uiParam,
		_Pre_maybenull_ _Post_valid_ PVOID pvParam,
		_In_ UINT fWinIni,
		_In_ UINT dpi
		)
	{
		if (auto const pSystemParametersInfoForDpi = try_get_SystemParametersInfoForDpi())
		{
			return pSystemParametersInfoForDpi(uiAction, uiParam, pvParam, fWinIni, dpi);
		}

		if (!SystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni))
			return FALSE;

		if (SPI_GETICONTITLELOGFONT == uiAction
			|| SPI_GETICONMETRICS == uiAction
			|| SPI_GETNONCLIENTMETRICS == uiAction)
		{
			auto nDpiX = internal::GetDpiForSystemDownlevel();

			if (nDpiX != dpi)
			{
				if (SPI_GETICONTITLELOGFONT == uiAction)
				{
					if (auto pInfo = (LOGFONTW*)pvParam)
					{
						pInfo->lfHeight = MulDiv(pInfo->lfHeight, dpi, nDpiX);
					}
				}
				else if (SPI_GETICONMETRICS == uiAction)
				{
					if (auto pInfo = (ICONMETRICSW*)pvParam)
					{
						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iHorzSpacing))
							pInfo->iHorzSpacing = MulDiv(pInfo->iHorzSpacing, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iVertSpacing))
							pInfo->iVertSpacing = MulDiv(pInfo->iVertSpacing, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfFont))
							pInfo->lfFont.lfHeight = MulDiv(pInfo->lfFont.lfHeight, dpi, nDpiX);
					}
				}
				else if (SPI_GETNONCLIENTMETRICS == uiAction)
				{
					if (auto pInfo = (NONCLIENTMETRICSW*)pvParam)
					{
						if(RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iBorderWidth))
							pInfo->iBorderWidth = MulDiv(pInfo->iBorderWidth, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iScrollWidth))
							pInfo->iScrollWidth = MulDiv(pInfo->iScrollWidth, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iScrollHeight))
							pInfo->iScrollHeight = MulDiv(pInfo->iScrollHeight, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iCaptionWidth))
							pInfo->iCaptionWidth= MulDiv(pInfo->iCaptionWidth, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iCaptionHeight))
							pInfo->iCaptionHeight= MulDiv(pInfo->iCaptionHeight, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfCaptionFont))
							pInfo->lfCaptionFont.lfHeight = MulDiv(pInfo->lfCaptionFont.lfHeight, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iSmCaptionWidth))
							pInfo->iSmCaptionWidth = MulDiv(pInfo->iSmCaptionWidth, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iSmCaptionHeight))
							pInfo->iSmCaptionHeight = MulDiv(pInfo->iSmCaptionHeight, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfSmCaptionFont))
							pInfo->lfSmCaptionFont.lfHeight = MulDiv(pInfo->lfSmCaptionFont.lfHeight, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iMenuWidth))
							pInfo->iMenuWidth = MulDiv(pInfo->iMenuWidth, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iMenuHeight))
							pInfo->iMenuHeight = MulDiv(pInfo->iMenuHeight, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfMenuFont))
							pInfo->lfMenuFont.lfHeight = MulDiv(pInfo->lfMenuFont.lfHeight, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfStatusFont))
							pInfo->lfStatusFont.lfHeight = MulDiv(pInfo->lfStatusFont.lfHeight, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, lfMessageFont))
							pInfo->lfMessageFont.lfHeight = MulDiv(pInfo->lfMessageFont.lfHeight, dpi, nDpiX);

						if (RTL_CONTAINS_FIELD(pInfo, pInfo->cbSize, iPaddedBorderWidth))
							pInfo->iPaddedBorderWidth = MulDiv(pInfo->iPaddedBorderWidth, dpi, nDpiX);
					}
				}
			}
		}

		return TRUE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

	// 最低受支持的客户端	Windows 7 [仅限桌面应用]
	// 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
	__DEFINE_THUNK(
	user32,
	8,
	BOOL,
	WINAPI,
	GetWindowDisplayAffinity,
		_In_ HWND _hWnd,
		_Out_ DWORD* _pdwAffinity)
	{
		if (auto const _pfnGetWindowDisplayAffinity = try_get_GetWindowDisplayAffinity())
		{
			return _pfnGetWindowDisplayAffinity(_hWnd, _pdwAffinity);
		}

		if (!_pdwAffinity)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		// 系统不支持，假装自己不需要任何保护
		*_pdwAffinity = WDA_NONE;
		return TRUE;
	}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN7)

	// 最低受支持的客户端	Windows 7 [仅限桌面应用]
	// 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
	__DEFINE_THUNK(
	user32,
	8,
	BOOL,
	WINAPI,
	SetWindowDisplayAffinity,
		_In_ HWND _hWnd,
		_In_ DWORD _dwAffinity)
	{
		if (auto const _pfnSetWindowDisplayAffinity = try_get_SetWindowDisplayAffinity())
		{
			return _pfnSetWindowDisplayAffinity(_hWnd, _dwAffinity);
		}

		// 系统不支持，假装自己成功……
		return TRUE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

	// 最低受支持的客户端	Windows 7 [仅限桌面应用]
	// 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
	__DEFINE_THUNK(
	user32,
	8,
	BOOL,
	WINAPI,
	RegisterTouchWindow,
		_In_ HWND _hWnd,
		_In_ ULONG _ulFlags)
	{
		if (auto const _pfnRegisterTouchWindow = try_get_RegisterTouchWindow())
		{
			return _pfnRegisterTouchWindow(_hWnd, _ulFlags);
		}

		return TRUE;
	}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN7)

	// 最低受支持的客户端	Windows 7 [仅限桌面应用]
	// 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
	__DEFINE_THUNK(
	user32,
	4,
	BOOL,
	WINAPI,
	UnregisterTouchWindow,
		_In_ HWND _hWnd)
	{
		if (auto const _pfnUnregisterTouchWindow = try_get_UnregisterTouchWindow())
		{
			return _pfnUnregisterTouchWindow(_hWnd);
		}

		return TRUE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

	// 最低受支持的客户端	Windows 7 [仅限桌面应用]
	// 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
	__DEFINE_THUNK(
	user32,
	8,
	BOOL,
	WINAPI,
	IsTouchWindow,
		_In_ HWND _hWnd,
		_Out_opt_ PULONG _puFlags)
	{
		if (auto const _pfnIsTouchWindow = try_get_IsTouchWindow())
		{
			return _pfnIsTouchWindow(_hWnd, _puFlags);
		}

		if (_puFlags)
			*_puFlags = 0;

		return TRUE;
	}
#endif
		

#if (YY_Thunks_Support_Version < NTDDI_WIN7)

	// 最低受支持的客户端	Windows 7 [仅限桌面应用]
	// 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
	__DEFINE_THUNK(
	user32,
	16,
	BOOL,
	WINAPI,
	GetTouchInputInfo,
		_In_ HTOUCHINPUT _hTouchInput,
		_In_ UINT _uInputs,
		_Out_writes_(_uInputs) PTOUCHINPUT _pInputs,
		_In_ int _cbSize)
	{
		if (auto const _pfnGetTouchInputInfo = try_get_GetTouchInputInfo())
		{
			return _pfnGetTouchInputInfo(_hTouchInput, _uInputs, _pInputs, _cbSize);
		}

		// 老版本系统没有触摸消息，_hTouchInput 必然无效
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN7)

	// 最低受支持的客户端	Windows 7 [仅限桌面应用]
	// 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
	__DEFINE_THUNK(
	user32,
	4,
	BOOL,
	WINAPI,
	CloseTouchInputHandle,
		_In_ HTOUCHINPUT _hTouchInput)
	{
		if (auto const _pfnCloseTouchInputHandle = try_get_CloseTouchInputHandle())
		{
			return _pfnCloseTouchInputHandle(_hTouchInput);
		}

		// 老版本系统没有触摸消息，_hTouchInput 必然无效
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

// 最低受支持的客户端	Windows Vista [仅限桌面应用]
// 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
__DEFINE_THUNK(
user32,
8,
BOOL,
WINAPI,
ChangeWindowMessageFilter,
    _In_ UINT _uMessage,
    _In_ DWORD _fFlag
    )
{
    if (const auto _pfnChangeWindowMessageFilter = try_get_ChangeWindowMessageFilter())
    {
        return _pfnChangeWindowMessageFilter(_uMessage, _fFlag);
    }

    return TRUE;
}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

// 最低受支持的客户端	Windows Vista [仅限桌面应用]
// 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
__DEFINE_THUNK(
user32,
8,
BOOL,
WINAPI,
UpdateLayeredWindowIndirect,
    _In_ HWND hWnd,
    _In_ const UPDATELAYEREDWINDOWINFO* pULWInfo
    )
{
    if (const auto _pfnUpdateLayeredWindowIndirect = try_get_UpdateLayeredWindowIndirect())
    {
        return _pfnUpdateLayeredWindowIndirect(hWnd, pULWInfo);
    }

    return UpdateLayeredWindow(
        hWnd,
        pULWInfo->hdcDst,
        const_cast<POINT*>(pULWInfo->pptDst),
        const_cast<SIZE*>(pULWInfo->psize),
        pULWInfo->hdcSrc,
        const_cast<POINT*>(pULWInfo->pptSrc),
        pULWInfo->crKey,
        const_cast<BLENDFUNCTION*>(pULWInfo->pblend), pULWInfo->dwFlags);
}
#endif
} //namespace YY::Thunks
