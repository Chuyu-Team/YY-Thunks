#include "pch.h"
#include "Thunks/api-ms-win-core-synch.hpp"


namespace api_ms_win_core_synch
{
	TEST_CLASS(WaitOnAddress)
	{
	public:
		WaitOnAddress()
		{
			YY::Thunks::aways_null_try_get_WaitOnAddress = true;
			YY::Thunks::aways_null_try_get_WakeByAddressSingle = true;
		}

		TEST_METHOD(结果本身不同)
		{
			//本身不同时，数据应该立即返回

			ULONG TargetValue = 0x2;
			ULONG UndesiredValue = 0;

			auto bRet = ::WaitOnAddress(&TargetValue, &UndesiredValue, sizeof(UndesiredValue), 500);

			Assert::IsTrue(bRet);
		}

		TEST_METHOD(结果本身相同)
		{
			ULONG TargetValue = 0x2;
			ULONG UndesiredValue = 0x2;

			auto bRet = ::WaitOnAddress(&TargetValue, &UndesiredValue, sizeof(UndesiredValue), 500);

			Assert::IsFalse(bRet);
		}
	};


	TEST_CLASS(WakeByAddressSingle)
	{
	public:
		WakeByAddressSingle()
		{
			YY::Thunks::aways_null_try_get_WaitOnAddress = true;
			YY::Thunks::aways_null_try_get_WakeByAddressSingle = true;
		}

		TEST_METHOD(只唤醒了一个线程)
		{
			
			struct MyData
			{
				ULONG TargetValue;
				volatile long RunCount;
			};

			MyData Data = { 0x2,0 };


			HANDLE hThreadHandles[100];

			for (auto& hThreadHandle : hThreadHandles)
			{
				hThreadHandle = (HANDLE)_beginthreadex(nullptr, 0, [](void* pMyData) -> unsigned
					{
						auto& Data = *(MyData*)pMyData;

						ULONG UndesiredValue = 0x2;
						auto bRet = ::WaitOnAddress(&Data.TargetValue, &UndesiredValue, sizeof(UndesiredValue), INFINITE);

						Assert::IsTrue(bRet);

						InterlockedIncrement(&Data.RunCount);

						return 0;
					}, & Data, 0, nullptr);

				Assert::IsNotNull(hThreadHandle);
			}

			Sleep(200);

			Assert::AreEqual(0l, (long)Data.RunCount);

			Data.TargetValue = 0;
			::WakeByAddressSingle(&Data.TargetValue);

			Sleep(200);

			Assert::AreEqual(1l, (long)Data.RunCount);


			for (int i = 0; i != 99; ++i)
			{
				::WakeByAddressSingle(&Data.TargetValue);
			}

			for (auto hThreadHandle : hThreadHandles)
			{
				Assert::AreEqual(WaitForSingleObject(hThreadHandle, 500), WAIT_OBJECT_0);
			}

			Assert::AreEqual(100l, (long)Data.RunCount);

		}
	};


	TEST_CLASS(WakeByAddressAll)
	{
	public:
		WakeByAddressAll()
		{
			YY::Thunks::aways_null_try_get_WaitOnAddress = true;
			YY::Thunks::aways_null_try_get_WakeByAddressAll = true;
		}

		TEST_METHOD(唤醒所有线程)
		{
			struct MyData
			{
				ULONG TargetValue;
				volatile long RunCount;
			};

			MyData Data = { 0x2,0 };


			HANDLE hThreadHandles[100];

			for (auto& hThreadHandle : hThreadHandles)
			{
				hThreadHandle = (HANDLE)_beginthreadex(nullptr, 0, [](void* pMyData) -> unsigned
					{
						auto& Data = *(MyData*)pMyData;

						ULONG UndesiredValue = 0x2;
						auto bRet = ::WaitOnAddress(&Data.TargetValue, &UndesiredValue, sizeof(UndesiredValue), INFINITE);

						Assert::IsTrue(bRet);

						InterlockedIncrement(&Data.RunCount);

						return 0;
					}, &Data, 0, nullptr);

				Assert::IsNotNull(hThreadHandle);
			}

			Sleep(200);

			Assert::AreEqual(0l, (long)Data.RunCount);

			Data.TargetValue = 0;
			::WakeByAddressAll(&Data.TargetValue);

			for (auto hThreadHandle : hThreadHandles)
			{
				Assert::AreEqual(WaitForSingleObject(hThreadHandle, 500), WAIT_OBJECT_0);
			}

			Assert::AreEqual(100l, (long)Data.RunCount);


		}
	};
}