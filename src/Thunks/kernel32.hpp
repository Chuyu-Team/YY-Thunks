

namespace YY
{
	namespace Thunks
	{
#if (YY_Thunks_Support_Version < NTDDI_WINXP)
EXTERN_C
DWORD
WINAPI
WTSGetActiveConsoleSessionId(
	VOID
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (const auto pWTSGetActiveConsoleSessionId = try_get_WTSGetActiveConsoleSessionId())
	{
		return pWTSGetActiveConsoleSessionId();
	}


	//因为Windows 2000没有会话隔离，所有的进程始终在 0 会话中运行，因此直接返回 0 即可。
	return 0;
}
#endif

__YY_Thunks_Expand_Function(kernel32, WTSGetActiveConsoleSessionId, 0);

#endif
	}
}