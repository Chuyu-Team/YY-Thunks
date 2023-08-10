

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WINXP)

		// Available in starting with Windows XP.
		// Windows 2000时这个函数就已经存在了，只是从XP开始才从kernel32.dll中导出
		__DEFINE_THUNK(
		ntdll,
		16,
		WORD,
		NTAPI,
		RtlCaptureStackBackTrace,
			_In_ DWORD _uFramesToSkip,
			_In_ DWORD _uFramesToCapture,
			_Out_writes_to_(_uFramesToCapture, return) PVOID* _pBackTrace,
			_Out_opt_ PDWORD _puBackTraceHash
			)
		{
			if (const auto _pfnRtlCaptureStackBackTrace = try_get_RtlCaptureStackBackTrace())
			{
				return _pfnRtlCaptureStackBackTrace(_uFramesToSkip, _uFramesToCapture, _pBackTrace, _puBackTraceHash);
			}

			return 0;
		}
#endif
	}
}
