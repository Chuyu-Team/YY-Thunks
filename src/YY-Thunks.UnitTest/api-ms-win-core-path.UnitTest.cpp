#include "pch.h"
#include "CppUnitTest.h"

#include "Thunks/api-ms-win-core-path.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace api_ms_win_core_path
{
	TEST_CLASS(PathIsUNCEx)
	{
        AwaysNullGuard Guard;

	public:
		PathIsUNCEx()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathIsUNCEx;
		}

		TEST_METHOD(传统UNC路径分割)
		{
			PCWSTR pszServer;

			Assert::IsTrue(::PathIsUNCEx(LR"(\\Test)", &pszServer));

			Assert::AreEqual(pszServer, LR"(Test)");
		}

		TEST_METHOD(长命名UNC路径分割)
		{
			PCWSTR pszServer;

			Assert::IsTrue(::PathIsUNCEx(LR"(\\?\UNC\Test)", &pszServer));

			Assert::AreEqual(pszServer, LR"(Test)");
		}

		TEST_METHOD(Dos路径容错)
		{
			PCWSTR pszServer;
			Assert::IsFalse(::PathIsUNCEx(LR"(C:\123)", &pszServer));

			Assert::IsFalse(::PathIsUNCEx(LR"(\\?\C:\123)", &pszServer));
		}

		TEST_METHOD(空指针测试)
		{
			Assert::IsTrue(::PathIsUNCEx(LR"(\\?\UNC\Test)", nullptr));

			Assert::IsFalse(::PathIsUNCEx(LR"(C:\123)", nullptr));
		}
	};

	TEST_CLASS(PathCchIsRoot)
	{
        AwaysNullGuard Guard;

	public:
		PathCchIsRoot()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchIsRoot;
		}

		TEST_METHOD(Dos路径测试)
		{

			Assert::IsTrue(::PathCchIsRoot(LR"(c:\)"));
			Assert::IsTrue(::PathCchIsRoot(LR"(C:\)"));
			Assert::IsTrue(::PathCchIsRoot(LR"(A:\)"));
			Assert::IsTrue(::PathCchIsRoot(LR"(Z:\)"));
			Assert::IsFalse(::PathCchIsRoot(LR"(人:\)"));


			Assert::IsFalse(::PathCchIsRoot(LR"(c:)"));
			Assert::IsFalse(::PathCchIsRoot(LR"(C:)"));


			Assert::IsFalse(::PathCchIsRoot(LR"(c:\path1)"));
			Assert::IsFalse(::PathCchIsRoot(LR"(C:\path1)"));

			Assert::IsTrue(::PathCchIsRoot(LR"(\\?\c:\)"));
			Assert::IsTrue(::PathCchIsRoot(LR"(\\?\C:\)"));
			Assert::IsTrue(::PathCchIsRoot(LR"(\\?\A:\)"));
			Assert::IsTrue(::PathCchIsRoot(LR"(\\?\Z:\)"));
			Assert::IsFalse(::PathCchIsRoot(LR"(\\?\人:\)"));

			Assert::IsFalse(::PathCchIsRoot(LR"(\\?\c:)"));

			Assert::IsFalse(::PathCchIsRoot(LR"(\\?\c:\path1")"));

		}

		TEST_METHOD(UNC路径测试)
		{
			
			Assert::IsTrue(::PathCchIsRoot(LR"(\\path1\path2)"));
			Assert::IsFalse(::PathCchIsRoot(LR"(\\path1\path2\)"));

			Assert::IsFalse(::PathCchIsRoot(LR"(\\path1\path2\path3)"));

			Assert::IsTrue(::PathCchIsRoot(LR"(\\path1)"));

			Assert::IsFalse(::PathCchIsRoot(LR"(\\path1\)"));

			Assert::IsTrue(::PathCchIsRoot(LR"(\\)"));

			Assert::IsTrue(::PathCchIsRoot(LR"(\\?\UNC\)"));

			Assert::IsTrue(::PathCchIsRoot(LR"(\\?\UNC\path1\path2)"));

			Assert::IsFalse(::PathCchIsRoot(LR"(\\?\UNC\path1\path2\)"));

			Assert::IsFalse(::PathCchIsRoot(LR"(\\?\UNC\path1\path2\path3)"));

			Assert::IsTrue(::PathCchIsRoot(LR"(\\?\UNC\path1)"));

			Assert::IsFalse(::PathCchIsRoot(LR"(\\?\UNC\path1\)"));
		}

		TEST_METHOD(VolumeGUID路径测试)
		{
			Assert::IsTrue(::PathCchIsRoot(LR"(\\?\Volume{051aa677-8969-11e8-b1bf-000ec6fa25d4}\)"));
			Assert::IsFalse(::PathCchIsRoot(LR"(\\?\Volume{051aa677-8969-11e8-b1bf-000ec6fa25d4})"));

			Assert::IsFalse(::PathCchIsRoot(LR"(\\?\Volume{051aa677-8969-11e8-b1bf-000ec6fa25d4}\path1)"));
		}

		TEST_METHOD(相对路径测试)
		{
			Assert::IsFalse(::PathCchIsRoot(LR"(\path1)"));
			Assert::IsFalse(::PathCchIsRoot(LR"(path1)"));
		}

		TEST_METHOD(特殊值测试)
		{
			Assert::IsFalse(::PathCchIsRoot(NULL));
			Assert::IsFalse(::PathCchIsRoot(L""));
		}
	};

	TEST_CLASS(PathCchAddBackslashEx)
	{
        AwaysNullGuard Guard;

	public:
		PathCchAddBackslashEx()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchAddBackslashEx;
		}

		TEST_METHOD(基础功能测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(C:\123)";
				wchar_t Dst[] = LR"(C:\123\)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchAddBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(SUCCEEDED(hr));
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(C:\123\)";
				wchar_t Dst[] = LR"(C:\123\)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchAddBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}
		}

		TEST_METHOD(空指针测试)
		{
			wchar_t Src[MAX_PATH] = LR"(C:\123)";
			wchar_t Dst[] = LR"(C:\123\)";

			auto hr = ::PathCchAddBackslashEx(Src, _countof(Src), nullptr, nullptr);

			Assert::IsTrue(SUCCEEDED(hr));
			Assert::AreEqual(Src, Dst, false);
		}


	};

	TEST_CLASS(PathCchRemoveBackslashEx)
	{
        AwaysNullGuard Guard;

	public:
		PathCchRemoveBackslashEx()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchRemoveBackslashEx;
		}

		TEST_METHOD(Dos路径测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(C:\123)";
				wchar_t Dst[] = LR"(C:\123)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(C:\123\)";
				wchar_t Dst[] = LR"(C:\123)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_OK);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(C:\)";
				wchar_t Dst[] = LR"(C:\)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 2)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 2));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(C:)";
				wchar_t Dst[] = LR"(C:)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\C:\123)";
				wchar_t Dst[] = LR"(\\?\C:\123)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\C:\123\)";
				wchar_t Dst[] = LR"(\\?\C:\123)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_OK);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\C:\)";
				wchar_t Dst[] = LR"(\\?\C:\)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 2)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 2));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\C:)";
				wchar_t Dst[] = LR"(\\?\C:)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}
		}

		TEST_METHOD(UNC路径测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\path2\path3\)";
				wchar_t Dst[] = LR"(\\path1\path2\path3)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_OK);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\path2\path3)";
				wchar_t Dst[] = LR"(\\path1\path2\path3)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\path2)";
				wchar_t Dst[] = LR"(\\path1\path2)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\path1)";
				wchar_t Dst[] = LR"(\\path1)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\)";
				wchar_t Dst[] = LR"(\\)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 2)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 2));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\path2\path3\)";
				wchar_t Dst[] = LR"(\\?\UNC\path1\path2\path3)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_OK);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}


			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\path2\path3)";
				wchar_t Dst[] = LR"(\\?\UNC\path1\path2\path3)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}


			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\path2)";
				wchar_t Dst[] = LR"(\\?\UNC\path1\path2)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1)";
				wchar_t Dst[] = LR"(\\?\UNC\path1)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\)";
				wchar_t Dst[] = LR"(\\?\UNC\)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 2)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 2));
			}


			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC)";
				wchar_t Dst[] = LR"(\\?\UNC)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}
		}

		TEST_METHOD(VolumeGUID路径测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\path2\)";
				wchar_t Dst[] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\path2)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_OK);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\path2)";
				wchar_t Dst[] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\path2)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 1)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 1));
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\)";
				wchar_t Dst[] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\)";

				LPWSTR pEnd;
				size_t cchRemaining;

				auto hr = ::PathCchRemoveBackslashEx(Src, _countof(Src), &pEnd, &cchRemaining);

				Assert::IsTrue(hr == S_FALSE);
				Assert::AreEqual(Src, Dst, false);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + (_countof(Dst) - 2)));

				Assert::AreEqual(cchRemaining, _countof(Src) - (_countof(Dst) - 2));
			}
		}
	};

	TEST_CLASS(PathCchSkipRoot)
	{
        AwaysNullGuard Guard;

	public:
		PathCchSkipRoot()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchSkipRoot;
		}

		TEST_METHOD(Dos路径)
		{
			{
				LPCWSTR Src = LR"(C:\2125\21515)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 3));
			}

			{
				LPCWSTR Src = LR"(C:\)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 3));
			}


			{
				LPCWSTR Src = LR"(C:)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 2));
			}

			{
				LPCWSTR Src = LR"(\\?\C:\2125\21515)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 7));
			}

			{
				LPCWSTR Src = LR"(\\?\C:\)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 7));
			}


			{
				LPCWSTR Src = LR"(\\?\C:)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 6));
			}
		}

		TEST_METHOD(UNC目录测试)
		{
			{
				LPCWSTR Src = LR"(\\Path1\Path2\xxxx\sfsf)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 14));
			}

			{
				LPCWSTR Src = LR"(\\)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 2));
			}

			{
				LPCWSTR Src = LR"(\\Path1)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 7));
			}

			{
				LPCWSTR Src = LR"(\\Path1\)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 8));
			}

			
			{
				LPCWSTR Src = LR"(\\Path1\Path2)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 13));
			}

			{
				LPCWSTR Src = LR"(\\Path1\Path2\)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 14));
			}

			{
				LPCWSTR Src = LR"(\\?\UNC\Path1\Path2\5151\45154)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 20));
			}

			{
				LPCWSTR Src = LR"(\\?\UNC\)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 8));
			}

			{
				LPCWSTR Src = LR"(\\?\UNC\path1)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 13));
			}

			{
				LPCWSTR Src = LR"(\\?\UNC\path1\)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 14));
			}

			{
				LPCWSTR Src = LR"(\\?\UNC\path1\path2)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 19));
			}

			{
				LPCWSTR Src = LR"(\\?\UNC\path1\path2\)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 20));
			}
		}

		TEST_METHOD(VolumeGUID目录测试)
		{
			{
				LPCWSTR Src = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\1213131\232424)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 49));
			}

			{
				LPCWSTR Src = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4})";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 48));
			}

			{
				LPCWSTR Src = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 49));
			}
		}

		TEST_METHOD(相对路径目录测试)
		{
			{
				LPCWSTR Src = LR"(\path1\path2)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 1));
			}

			{
				LPCWSTR Src = LR"(\)";
				LPCWSTR pEnd;

				auto hr = ::PathCchSkipRoot(Src, &pEnd);

				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual((INT_PTR)pEnd, (INT_PTR)(Src + 1));
			}
		}

	};

	TEST_CLASS(PathCchStripToRoot)
	{
        AwaysNullGuard Guard;

	public:
		PathCchStripToRoot()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchStripToRoot;
		}

		TEST_METHOD(Dos路径测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(C:\123\215)";
				wchar_t Dst[] = LR"(C:\)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(C:\)";
				wchar_t Dst[] = LR"(C:\)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(C:)";
				wchar_t Dst[] = LR"(C:)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\C:\123\215)";
				wchar_t Dst[] = LR"(\\?\C:\)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\C:\)";
				wchar_t Dst[] = LR"(\\?\C:\)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\C:)";
				wchar_t Dst[] = LR"(\\?\C:)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}
		}

		TEST_METHOD(UNC路径测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\path2\path3)";
				wchar_t Dst[] = LR"(\\path1\path2)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}


			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\path2\)";
				wchar_t Dst[] = LR"(\\path1\path2)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\path2)";
				wchar_t Dst[] = LR"(\\path1\path2)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\)";
				wchar_t Dst[] = LR"(\\path1)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\path1)";
				wchar_t Dst[] = LR"(\\path1)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\)";
				wchar_t Dst[] = LR"(\\)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\path2\path3)";
				wchar_t Dst[] = LR"(\\?\UNC\path1\path2)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}


			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\path2\)";
				wchar_t Dst[] = LR"(\\?\UNC\path1\path2)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\path2)";
				wchar_t Dst[] = LR"(\\?\UNC\path1\path2)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\)";
				wchar_t Dst[] = LR"(\\?\UNC\path1)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1)";
				wchar_t Dst[] = LR"(\\?\UNC\path1)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\)";
				wchar_t Dst[] = LR"(\\?\UNC\)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}
		}

		TEST_METHOD(VolumeGUID路径测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\path1\path2)";
				wchar_t Dst[] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4})";
				wchar_t Dst[] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4})";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}
		}

		TEST_METHOD(相对路径测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(\path1)";
				wchar_t Dst[] = LR"(\)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\)";
				wchar_t Dst[] = LR"(\)";

				auto hr = ::PathCchStripToRoot(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}
		}
	};


	TEST_CLASS(PathCchRemoveFileSpec)
	{
        AwaysNullGuard Guard;

	public:
		PathCchRemoveFileSpec()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchRemoveFileSpec;
		}

		TEST_METHOD(Dos路径测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(C:\path1\)";
				wchar_t Dst[] = LR"(C:\path1)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(C:\path1)";
				wchar_t Dst[] = LR"(C:\)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(C:\)";
				wchar_t Dst[] = LR"(C:\)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(C:)";
				wchar_t Dst[] = LR"(C:)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\C:\path1\)";
				wchar_t Dst[] = LR"(\\?\C:\path1)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\C:\path1)";
				wchar_t Dst[] = LR"(\\?\C:\)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\C:\)";
				wchar_t Dst[] = LR"(\\?\C:\)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\C:)";
				wchar_t Dst[] = LR"(\\?\C:)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}
		}

		TEST_METHOD(UNC路径测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\path2\path3\)";
				wchar_t Dst[] = LR"(\\path1\path2\path3)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\path2\path3)";
				wchar_t Dst[] = LR"(\\path1\path2)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\path2\)";
				wchar_t Dst[] = LR"(\\path1\path2)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\path2)";
				wchar_t Dst[] = LR"(\\path1\path2)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\path1\)";
				wchar_t Dst[] = LR"(\\path1)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\path1)";
				wchar_t Dst[] = LR"(\\path1)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\)";
				wchar_t Dst[] = LR"(\\)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\path2\path3\)";
				wchar_t Dst[] = LR"(\\?\UNC\path1\path2\path3)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\path2\path3)";
				wchar_t Dst[] = LR"(\\?\UNC\path1\path2)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\path2\)";
				wchar_t Dst[] = LR"(\\?\UNC\path1\path2)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\path2)";
				wchar_t Dst[] = LR"(\\?\UNC\path1\path2)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1\)";
				wchar_t Dst[] = LR"(\\?\UNC\path1)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\path1)";
				wchar_t Dst[] = LR"(\\?\UNC\path1)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\UNC\)";
				wchar_t Dst[] = LR"(\\?\UNC\)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}
		}

		TEST_METHOD(VolumeGUID路径测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\path1\)";
				wchar_t Dst[] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\path1)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\path1)";
				wchar_t Dst[] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\)";
				wchar_t Dst[] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4}\)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4})";
				wchar_t Dst[] = LR"(\\?\Volume{1fd5cead-d314-11e7-a347-000ec6fa25d4})";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}
		}

		TEST_METHOD(相对路径目录测试)
		{
			{
				wchar_t Src[MAX_PATH] = LR"(\path1\path2\)";
				wchar_t Dst[] = LR"(\path1\path2)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\path1\path2)";
				wchar_t Dst[] = LR"(\path1)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\path1\)";
				wchar_t Dst[] = LR"(\path1)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\path1)";
				wchar_t Dst[] = LR"(\)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_OK);

				Assert::AreEqual(Src, Dst, false);
			}

			{
				wchar_t Src[MAX_PATH] = LR"(\)";
				wchar_t Dst[] = LR"(\)";

				auto hr = ::PathCchRemoveFileSpec(Src, _countof(Src));
				Assert::AreEqual(hr, S_FALSE);

				Assert::AreEqual(Src, Dst, false);
			}
		}
	};


	TEST_CLASS(PathCchFindExtension)
	{
        AwaysNullGuard Guard;

	public:
		PathCchFindExtension()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchFindExtension;
		}

		TEST_METHOD(常规后缀提取)
		{
			{
				wchar_t Src[] = LR"(C:\path1\file.test)";
				LPCWSTR Dst = L".test";
				LPCWSTR pExtension;

				auto hr = ::PathCchFindExtension(Src, _countof(Src), &pExtension);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(pExtension, Dst, false);
			}

			{
				wchar_t Src[] = LR"(C:\path1\.test)";
				LPCWSTR Dst = L".test";
				LPCWSTR pExtension;

				auto hr = ::PathCchFindExtension(Src, _countof(Src), &pExtension);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(pExtension, Dst, false);
			}

			{
				wchar_t Src[] = LR"(C:\path1\filetest)";
				LPCWSTR Dst = L"";
				LPCWSTR pExtension;

				auto hr = ::PathCchFindExtension(Src, _countof(Src), &pExtension);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(pExtension, Dst, false);
			}
		}

		TEST_METHOD(后缀带空格情况提取)
		{
			{
				wchar_t Src[] = LR"(C:\path1\file. test)";
				LPCWSTR Dst = L"";
				LPCWSTR pExtension;

				auto hr = ::PathCchFindExtension(Src, _countof(Src), &pExtension);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(pExtension, Dst, false);
			}
		}

		TEST_METHOD(不带任何斜杠情况提取)
		{
			{
				wchar_t Src[] = LR"(file.test)";
				LPCWSTR Dst = L".test";
				LPCWSTR pExtension;

				auto hr = ::PathCchFindExtension(Src, _countof(Src), &pExtension);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(pExtension, Dst, false);
			}

			{
				wchar_t Src[] = LR"(filetest)";
				LPCWSTR Dst = L"";
				LPCWSTR pExtension;

				auto hr = ::PathCchFindExtension(Src, _countof(Src), &pExtension);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(pExtension, Dst, false);
			}

		}
	};


	TEST_CLASS(PathCchAddExtension)
	{
        AwaysNullGuard Guard;

	public:
		PathCchAddExtension()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchAddExtension;
		}

		TEST_METHOD(常规的添加)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file)";
				LPCWSTR Output = LR"(C:\path1\file.test)";

				auto hr = ::PathCchAddExtension(Input, _countof(Input), L".test");

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file)";
				LPCWSTR Output = LR"(C:\path1\file.test)";

				auto hr = ::PathCchAddExtension(Input, _countof(Input), L"test");

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}

		TEST_METHOD(当文件名为空时添加)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\)";
				LPCWSTR Output = LR"(C:\path1\.test)";

				auto hr = ::PathCchAddExtension(Input, _countof(Input), L"test");

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}

		TEST_METHOD(向已经存在后缀的路径添加)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file.test)";
				LPCWSTR Output = LR"(C:\path1\file.test)";

				auto hr = ::PathCchAddExtension(Input, _countof(Input), L".123");

				Assert::AreEqual(hr, S_FALSE);
				Assert::AreEqual(Input, Output, false);
			}
		}

		TEST_METHOD(后缀本身是一个空字符串)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file)";
				LPCWSTR Output = LR"(C:\path1\file)";

				auto hr = ::PathCchAddExtension(Input, _countof(Input), L".");

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file)";
				LPCWSTR Output = LR"(C:\path1\file)";

				auto hr = ::PathCchAddExtension(Input, _countof(Input), L"");

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}

		TEST_METHOD(后缀格式错误验证)
		{
			//包含空格
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file)";
				LPCWSTR Output = LR"(C:\path1\file)";

				auto hr = ::PathCchAddExtension(Input, _countof(Input), LR"(. 215)");

				Assert::IsTrue(FAILED(hr));
				Assert::AreEqual(Input, Output, false);
			}
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file)";
				LPCWSTR Output = LR"(C:\path1\file)";

				auto hr = ::PathCchAddExtension(Input, _countof(Input), LR"( 215)");

				Assert::IsTrue(FAILED(hr));
				Assert::AreEqual(Input, Output, false);
			}

			//包含斜杠
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file)";
				LPCWSTR Output = LR"(C:\path1\file)";

				auto hr = ::PathCchAddExtension(Input, _countof(Input), LR"(.\215)");

				Assert::IsTrue(FAILED(hr));
				Assert::AreEqual(Input, Output, false);
			}
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file)";
				LPCWSTR Output = LR"(C:\path1\file)";

				auto hr = ::PathCchAddExtension(Input, _countof(Input), LR"(\215)");

				Assert::IsTrue(FAILED(hr));
				Assert::AreEqual(Input, Output, false);
			}

			//后缀太长，边界验证
			{
				wchar_t Input[512] = LR"(\\?\C:\path1\file)";

				auto hr = ::PathCchAddExtension(
					Input,
					_countof(Input),
					L".1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"111111111111"
					);
				Assert::IsTrue(FAILED(hr));

				hr = ::PathCchAddExtension(
					Input,
					_countof(Input),
					L".1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"11111111111"
					);

				Assert::IsTrue(SUCCEEDED(hr));
			}
			{
				wchar_t Input[512] = LR"(\\?\C:\path1\file)";

				auto hr = ::PathCchAddExtension(
					Input,
					_countof(Input),
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"111111111111"
					);
				Assert::IsTrue(FAILED(hr));

				hr = ::PathCchAddExtension(
					Input,
					_countof(Input),
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"1111111111111111111111111111111111111111111111111111111111111"
					L"11111111111"
					);
				Assert::IsTrue(SUCCEEDED(hr));
			}
		}
	};


	TEST_CLASS(PathCchRenameExtension)
	{
        AwaysNullGuard Guard;

	public:
		PathCchRenameExtension()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchRenameExtension;
		}

		TEST_METHOD(添加后缀)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file)";
				LPCWSTR Output = LR"(C:\path1\file.test)";

				auto hr = ::PathCchRenameExtension(Input, _countof(Input), L".test");

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file)";
				LPCWSTR Output = LR"(C:\path1\file.test)";

				auto hr = ::PathCchRenameExtension(Input, _countof(Input), L"test");

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}


		TEST_METHOD(重命名后缀)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file.123)";
				LPCWSTR Output = LR"(C:\path1\file.test)";

				auto hr = ::PathCchRenameExtension(Input, _countof(Input), L".test");

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file.123)";
				LPCWSTR Output = LR"(C:\path1\file.test)";

				auto hr = ::PathCchRenameExtension(Input, _countof(Input), L"test");

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}

		TEST_METHOD(删除后缀)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file.123)";
				LPCWSTR Output = LR"(C:\path1\file)";

				auto hr = ::PathCchRenameExtension(Input, _countof(Input), L".");

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file.123)";
				LPCWSTR Output = LR"(C:\path1\file)";

				auto hr = ::PathCchRenameExtension(Input, _countof(Input), L"");

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}
	};


	TEST_CLASS(PathCchRemoveExtension)
	{
        AwaysNullGuard Guard;

	public:
		PathCchRemoveExtension()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchRemoveExtension;
		}

		TEST_METHOD(基础功能验证)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file.123)";
				LPCWSTR Output = LR"(C:\path1\file)";

				auto hr = ::PathCchRemoveExtension(Input, _countof(Input));

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}


			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\file)";
				LPCWSTR Output = LR"(C:\path1\file)";

				auto hr = ::PathCchRemoveExtension(Input, _countof(Input));

				Assert::AreEqual(hr, S_FALSE);
				Assert::AreEqual(Input, Output, false);
			}
		}
	};


	TEST_CLASS(PathCchCanonicalizeEx)
	{
        AwaysNullGuard Guard;

	public:
		PathCchCanonicalizeEx()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchCanonicalizeEx;
		}

		TEST_METHOD(基础功能验证)
		{
			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(C:\path1\file.123)";

				auto hr = ::PathCchCanonicalizeEx(Input, _countof(Input), LR"(C:\path1\.\file.123)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(C:\file.123)";

				auto hr = ::PathCchCanonicalizeEx(Input, _countof(Input), LR"(C:\path1\..\file.123)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(C:\name_1\name_3)";

				auto hr = ::PathCchCanonicalizeEx(Input, _countof(Input), LR"(C:\name_1\.\name_2\..\name_3)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}

		TEST_METHOD(末尾多余点删除验证)
		{
			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(C:\name_1\*.)";

				auto hr = ::PathCchCanonicalizeEx(Input, _countof(Input), LR"(C:\name_1\*...)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}


		TEST_METHOD(根目录向上跳转验证)
		{
			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(C:\file.123)";

				auto hr = ::PathCchCanonicalizeEx(Input, _countof(Input), LR"(C:\..\file.123)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(\\path1\file.123)";

				auto hr = ::PathCchCanonicalizeEx(Input, _countof(Input), LR"(\\path1\path2\..\file.123)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(\\file.123)";

				auto hr = ::PathCchCanonicalizeEx(Input, _countof(Input), LR"(\\path1\..\file.123)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(\\file.123)";

				auto hr = ::PathCchCanonicalizeEx(Input, _countof(Input), LR"(\\..\file.123)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(C:\)";

				auto hr = ::PathCchCanonicalizeEx(Input, _countof(Input), LR"(C:\..)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}
	};


	TEST_CLASS(PathCchCombineEx)
	{
        AwaysNullGuard Guard;

	public:
		PathCchCombineEx()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchCombineEx;
		}

		TEST_METHOD(基础功能验证)
		{
			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(C:\path1\path2\path3)";

				auto hr = ::PathCchCombineEx(Input, _countof(Input), LR"(C:\path1\path2\)", LR"(path3)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(C:\path1\path2\path3)";

				auto hr = ::PathCchCombineEx(Input, _countof(Input), LR"(C:\path1\path2)", LR"(path3)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(C:\path1)";

				auto hr = ::PathCchCombineEx(Input, _countof(Input), LR"(C:\path1)", nullptr, 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(C:\path1)";

				auto hr = ::PathCchCombineEx(Input, _countof(Input), nullptr, LR"(C:\path1)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(\)";

				auto hr = ::PathCchCombineEx(Input, _countof(Input), LR"()", LR"()", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}

		TEST_METHOD(More参数以斜杠开头)
		{
			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(C:\path3)";

				auto hr = ::PathCchCombineEx(Input, _countof(Input), LR"(C:\path1\path2)", LR"(\path3)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}

		TEST_METHOD(More参数是一个完整路径)
		{
			{
				wchar_t Input[MAX_PATH];
				LPCWSTR Output = LR"(D:\path3)";

				auto hr = ::PathCchCombineEx(Input, _countof(Input), LR"(C:\path1\path2)", LR"(D:\path3)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}
	};


	TEST_CLASS(PathCchAppendEx)
	{
        AwaysNullGuard Guard;

	public:
		PathCchAppendEx()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchAppendEx;
		}

		TEST_METHOD(基础功能验证)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(D:\path1)";
				LPCWSTR Output = LR"(D:\path1\path2)";

				auto hr = ::PathCchAppendEx(Input, _countof(Input), LR"(path2)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH] = LR"(D:\path1\)";
				LPCWSTR Output = LR"(D:\path1\path2)";

				auto hr = ::PathCchAppendEx(Input, _countof(Input), LR"(path2)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH] = LR"(D:\path1\)";
				LPCWSTR Output = LR"(D:\path1\path2)";

				auto hr = ::PathCchAppendEx(Input, _countof(Input), LR"(\path2)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}


			{
				wchar_t Input[MAX_PATH] = LR"(D:\path1\)";
				LPCWSTR Output = LR"(C:\path2)";

				auto hr = ::PathCchAppendEx(Input, _countof(Input), LR"(\\?\C:\path2)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH] = LR"(D:\path1\)";
				LPCWSTR Output = LR"(\\path1\path2)";

				auto hr = ::PathCchAppendEx(Input, _countof(Input), LR"(\\path1\path2)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}

		TEST_METHOD(More参数是一个完整路径)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(D:\path1\)";
				LPCWSTR Output = LR"(C:\path2)";

				auto hr = ::PathCchAppendEx(Input, _countof(Input), LR"(C:\path2)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH] = {};
				LPCWSTR Output = LR"(C:\path2)";

				auto hr = ::PathCchAppendEx(Input, _countof(Input), LR"(C:\path2)", 0);

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}
		}
	};


	TEST_CLASS(PathCchStripPrefix)
	{
        AwaysNullGuard Guard;

	public:
		PathCchStripPrefix()
		{
            Guard |= YY::Thunks::aways_null_try_get_PathCchStripPrefix;
		}

		TEST_METHOD(Dos路径)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(\\?\C:\path1\)";
				LPCWSTR Output = LR"(C:\path1\)";

				auto hr = ::PathCchStripPrefix(Input, _countof(Input));

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH] = LR"(C:\path1\)";
				LPCWSTR Output = LR"(C:\path1\)";

				auto hr = ::PathCchStripPrefix(Input, _countof(Input));

				Assert::AreEqual(hr, S_FALSE);
				Assert::AreEqual(Input, Output, false);
			}
		}

		TEST_METHOD(UNC路径)
		{
			{
				wchar_t Input[MAX_PATH] = LR"(\\?\UNC\path1\path2\path3)";
				LPCWSTR Output = LR"(\\path1\path2\path3)";

				auto hr = ::PathCchStripPrefix(Input, _countof(Input));

				Assert::AreEqual(hr, S_OK);
				Assert::AreEqual(Input, Output, false);
			}

			{
				wchar_t Input[MAX_PATH] = LR"(\\path1\path2\path3)";
				LPCWSTR Output = LR"(\\path1\path2\path3)";

				auto hr = ::PathCchStripPrefix(Input, _countof(Input));

				Assert::AreEqual(hr, S_FALSE);
				Assert::AreEqual(Input, Output, false);
			}
		}
	};
}
	
