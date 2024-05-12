#include "pch.h"
#include "Thunks/api-ms-win-power-base.hpp"

namespace api_ms_win_power_base
{
	TEST_CLASS(PowerRegisterSuspendResumeNotification)
	{
        AwaysNullGuard Guard;

	public:
		PowerRegisterSuspendResumeNotification()
		{
            Guard |= YY::Thunks::aways_null_try_get_PowerRegisterSuspendResumeNotification;
            Guard |= YY::Thunks::aways_null_try_get_PowerUnregisterSuspendResumeNotification;
		}

		TEST_METHOD(创建然后关闭)
		{
			DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS Parameters = {};
			Parameters.Callback = [](_In_opt_ PVOID Context,
				_In_ ULONG Type,
				_In_ PVOID Setting) -> ULONG
			{
				return ERROR_SUCCESS;
			};

			HPOWERNOTIFY RegistrationHandle = nullptr;

			auto _lStatus = ::PowerRegisterSuspendResumeNotification(DEVICE_NOTIFY_CALLBACK, &Parameters, &RegistrationHandle);
			Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);
			Assert::IsNotNull(RegistrationHandle);

			Sleep(500);

			_lStatus = ::PowerUnregisterSuspendResumeNotification(RegistrationHandle);
			Assert::AreEqual(_lStatus, (DWORD)ERROR_SUCCESS);
		}
	};
}
