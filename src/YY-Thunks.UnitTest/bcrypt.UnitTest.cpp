#include "pch.h"
#include "Thunks/bcrypt.hpp"


namespace bcrypt
{
	TEST_CLASS(BCryptGenRandom)
	{
	public:
		BCryptGenRandom()
		{
			YY::Thunks::aways_null_try_get_BCryptOpenAlgorithmProvider = true;
			YY::Thunks::aways_null_try_get_BCryptCloseAlgorithmProvider = true;
			YY::Thunks::aways_null_try_get_BCryptGenRandom = true;
		}

		TEST_METHOD(BCRYPT_USE_SYSTEM_PREFERRED_RNG模式)
		{
			UCHAR _Temp[15] = {};
			long _Status = ::BCryptGenRandom(nullptr, _Temp, sizeof(_Temp), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
			Assert::IsTrue(_Status >= 0);
			
			const UCHAR _Temp2[15] = {};
			Assert::IsFalse(memcpy(_Temp, _Temp2, sizeof(_Temp2)) == 0);
		}

		TEST_METHOD(BCryptOpen在关闭)
		{
			BCRYPT_ALG_HANDLE _hAlg = nullptr;
			auto _Status = ::BCryptOpenAlgorithmProvider(&_hAlg, BCRYPT_RNG_ALGORITHM, nullptr, 0);
			Assert::IsTrue(_Status >= 0);
			Assert::IsNotNull(_hAlg);

			UCHAR _Temp[15] = {};
			_Status = ::BCryptGenRandom(_hAlg, _Temp, sizeof(_Temp), 0);
			Assert::IsTrue(_Status >= 0);

			const UCHAR _Temp2[15] = {};
			Assert::IsFalse(memcpy(_Temp, _Temp2, sizeof(_Temp2)) == 0);

			_Status = ::BCryptCloseAlgorithmProvider(_hAlg, 0);
			Assert::IsTrue(_Status >= 0);
		}
	};

}
