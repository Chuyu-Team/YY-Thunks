
extern "C" BOOL WINAPI ProcessPrng(
    _Out_writes_bytes_(_cbBuffer) PUCHAR _pbBuffer,
    _In_                          ULONG _cbBuffer
    );

namespace YY
{
    namespace Thunks
    {
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// 最低受支持的客户端	Windows Vista [桌面应用|UWP 应用]
		// 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		bcryptprimitives,
		8,
		BOOL,
		WINAPI,
		ProcessPrng,
            _Out_writes_bytes_(_cbBuffer) PUCHAR _pbBuffer,
            _In_                          ULONG _cbBuffer
            )
		{
			if (auto _pfnProcessPrng = try_get_ProcessPrng())
			{
				return _pfnProcessPrng(_pbBuffer, _cbBuffer);
			}

            const auto _pfnRtlGenRandom = try_get_SystemFunction036();
            if (!_pfnRtlGenRandom)
            {
                SetLastError(ERROR_FUNCTION_FAILED);
                return FALSE;
            }

            return _pfnRtlGenRandom(_pbBuffer, _cbBuffer);
		}
#endif
    } // namespace Thunks
} // namespace YY
