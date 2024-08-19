
extern "C" BOOL WINAPI ProcessPrng(
    _Out_writes_bytes_(_cbBuffer) PUCHAR _pbBuffer,
    _In_                          ULONG _cbBuffer
    );

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 用户反馈Windows 7也会报告此API不存在。
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
} // namespace YY::Thunks
