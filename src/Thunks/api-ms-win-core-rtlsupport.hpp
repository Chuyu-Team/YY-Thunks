

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
			_In_ DWORD FramesToSkip,
			_In_ DWORD FramesToCapture,
			_Out_writes_to_(FramesToCapture, return) PVOID* BackTrace,
			_Out_opt_ PDWORD BackTraceHash
		)
		{
			if (const auto pRtlCaptureStackBackTrace = try_get_RtlCaptureStackBackTrace())
			{
				return pRtlCaptureStackBackTrace(FramesToSkip, FramesToCapture, BackTrace, BackTraceHash);
			}

			return 0;
		}
#endif
	}
}
