#include "pch.h"
#include "Thunks/api-ms-win-core-sysinfo.hpp"

namespace api_ms_win_core_sysinfo
{
TEST_CLASS(GetLogicalProcessorInformationEx)
{
    AwaysNullGuard Guard;

public:
    GetLogicalProcessorInformationEx()
    {
        Guard |= YY::Thunks::aways_null_try_get_GetLogicalProcessorInformationEx;
    }

    TEST_METHOD(常规验证)
    {
        DWORD len = 0;
        const auto _lStatus =  ::GetLogicalProcessorInformationEx(RelationAll, nullptr, &len) ? ERROR_SUCCESS : GetLastError();
        Assert::AreEqual(_lStatus, DWORD(ERROR_INSUFFICIENT_BUFFER));
        Assert::AreNotEqual(len, 0ul);

        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buf = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)malloc(len);
        Assert::IsTrue(::GetLogicalProcessorInformationEx(RelationAll, buf, &len));
        free(buf);
    }
};
}
