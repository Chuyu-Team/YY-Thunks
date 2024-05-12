#include "pch.h"
#include "Thunks/api-ms-win-core-fibers.hpp"


namespace api_ms_win_core_fibers
{
	__if_exists(YY::Thunks::aways_null_try_get_FlsAlloc)
	{
		TEST_CLASS(FlsGet_SetValue)
		{
            AwaysNullGuard Guard;

		public:
			FlsGet_SetValue()
			{
                Guard |= YY::Thunks::aways_null_try_get_FlsAlloc;
                Guard |= YY::Thunks::aways_null_try_get_FlsFree;
                Guard |= YY::Thunks::aways_null_try_get_FlsGetValue;
                Guard |= YY::Thunks::aways_null_try_get_FlsSetValue;
			}

			TEST_METHOD(单线程验证)
			{
				auto Index = ::FlsAlloc(nullptr);

				Assert::AreNotEqual(Index, FLS_OUT_OF_INDEXES);

				Assert::AreEqual((long)::FlsGetValue(Index), 0l);


				Assert::IsTrue(::FlsSetValue(Index, (void*)77));

				Assert::AreEqual((long)::FlsGetValue(Index), 77l);

				Assert::IsTrue(::FlsFree(Index));
			}

			TEST_METHOD(多线程验证)
			{
				auto Index = ::FlsAlloc(nullptr);

				Assert::AreNotEqual(Index, FLS_OUT_OF_INDEXES);

				Assert::IsTrue(::FlsSetValue(Index, (void*)-1));

				HANDLE hHandles[100];

				for (auto& hHandle : hHandles)
				{
					hHandle = (HANDLE)_beginthreadex(nullptr, 0, [](void* Index) -> unsigned
						{
							Assert::AreEqual((long)::FlsGetValue((DWORD)Index), 0l);


							Assert::IsTrue(::FlsSetValue((DWORD)Index, (void*)GetCurrentThreadId()));

							Sleep(100);

							Assert::AreEqual((long)::FlsGetValue((DWORD)Index), (long)GetCurrentThreadId());


							return 0;
						}, (void*)Index, 0, nullptr);

					Assert::IsNotNull(hHandle);
				}

				for (auto hHandle : hHandles)
				{
					Assert::AreEqual(WaitForSingleObject(hHandle, 1000), (DWORD)WAIT_OBJECT_0);

					CloseHandle(hHandle);
				}

				Assert::AreEqual((long)::FlsGetValue((DWORD)Index), -1l);

				Assert::IsTrue(::FlsFree(Index));
			}

			TEST_METHOD(Fls释放销毁回调验证)
			{
				auto Index = ::FlsAlloc([](void* pFlsData)
					{
						InterlockedIncrement((long*)pFlsData);

					});

				Assert::AreNotEqual(Index, FLS_OUT_OF_INDEXES);


				volatile long RunCount = 0;


				Assert::IsTrue(::FlsSetValue(Index, (void*)&RunCount));

				Assert::IsTrue(::FlsFree(Index));

				Assert::AreEqual((long)RunCount, 1l);
			}

			TEST_METHOD(线程退出销毁回调验证)
			{
				struct MyData
				{
					union
					{
						DWORD dwThreadId;
						DWORD Index;
					};
					volatile long* pRunCount;
				};

				auto Index = ::FlsAlloc([](void* pFlsData)
					{
						auto pData = (MyData*)pFlsData;

						Assert::AreEqual(GetCurrentThreadId(), pData->dwThreadId);

						InterlockedIncrement(pData->pRunCount);

					});

				Assert::AreNotEqual(Index, FLS_OUT_OF_INDEXES);


				volatile long RunCount = 0;


				MyData Data;
				Data.Index = Index;
				Data.pRunCount = &RunCount;

				

				HANDLE hHandles[100];

				for (auto& hHandle : hHandles)
				{
					hHandle = (HANDLE)_beginthreadex(nullptr, 0, [](void* pMyData) -> unsigned
						{
							auto& Data = *(MyData*)pMyData;

							Assert::AreEqual((long)::FlsGetValue(Data.Index), 0l);

							auto pFlsData = new MyData;

							pFlsData->dwThreadId = GetCurrentThreadId();
							pFlsData->pRunCount = Data.pRunCount;

							Assert::IsTrue(::FlsSetValue(Data.Index, pFlsData));

							Sleep(100);

							return 0;
						}, &Data, 0, nullptr);

					Assert::IsNotNull(hHandle);
				}


				for (auto hHandle : hHandles)
				{
					Assert::AreEqual(WaitForSingleObject(hHandle, 1000), (DWORD)WAIT_OBJECT_0);

					CloseHandle(hHandle);
				}


				Assert::AreEqual((long)RunCount, (long)_countof(hHandles));


				Assert::IsTrue(::FlsFree(Index));
			}



		};
	}
}

