#include "pch.h"
#include "CppUnitTest.h"

#include "Thunks/api-ms-win-core-kernel32-legacy.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace api_ms_win_core_kernel32_legacy
{
	TEST_CLASS(PowerRequest)
	{
        AwaysNullGuard Guard;

	public:
		PowerRequest()
		{
            Guard |= YY::Thunks::aways_null_try_get_PowerCreateRequest;
            Guard |= YY::Thunks::aways_null_try_get_PowerSetRequest;
            Guard |= YY::Thunks::aways_null_try_get_PowerClearRequest;
		}

		TEST_METHOD(Set然后Clear)
		{
			REASON_CONTEXT _Context = { POWER_REQUEST_CONTEXT_VERSION };
			auto _hPower = ::PowerCreateRequest(&_Context);
			Assert::AreNotEqual(_hPower, INVALID_HANDLE_VALUE);

			auto _bRet = PowerSetRequest(_hPower, POWER_REQUEST_TYPE::PowerRequestAwayModeRequired);
			Assert::IsTrue(_bRet);

			_bRet = PowerClearRequest(_hPower, POWER_REQUEST_TYPE::PowerRequestAwayModeRequired);
			Assert::IsTrue(_bRet);

			CloseHandle(_hPower);
		}
	};
}
