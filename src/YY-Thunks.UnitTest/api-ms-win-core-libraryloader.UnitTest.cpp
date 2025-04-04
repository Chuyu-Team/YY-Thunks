#include "pch.h"
#include "Thunks/api-ms-win-core-libraryloader.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace api_ms_win_core_libraryloader
{
#if (YY_Thunks_Target < __WindowsNT5_1_SP1)
    TEST_CLASS(SetDllDirectoryW_A)
    {
        AwaysNullGuard Guard;

    public:
        SetDllDirectoryW_A()
        {
            Guard |= YY::Thunks::aways_null_try_get_SetDllDirectoryW;
            Guard |= YY::Thunks::aways_null_try_get_GetDllDirectoryW;
            Guard |= YY::Thunks::aways_null_try_get_SetDllDirectoryA;
            Guard |= YY::Thunks::aways_null_try_get_GetDllDirectoryA;
        }

        TEST_METHOD(常规)
        {
            {
                wchar_t _szBuffer[MAX_PATH];
                static LPCWSTR s_szPathList[] =
                {
                    L"C:\\123",
                    L"C:\\456",
                    L"C:\\789",
                    nullptr,
                };

                for (auto _szPath : s_szPathList)
                {
                    Assert::IsTrue(::SetDllDirectoryW(_szPath));

                    const auto _uResult = ::GetDllDirectoryW(_countof(_szBuffer), _szBuffer);
                    if (_szPath)
                    {
                        Assert::AreEqual(size_t(_uResult), wcslen(_szPath));
                        Assert::AreEqual(_szBuffer, _szPath);
                    }
                    else
                    {
                        Assert::AreEqual(_uResult, 0ul);
                    }
                }
            }

            {
                char _szBuffer[MAX_PATH];
                static LPCSTR s_szPathList[] =
                {
                    "C:\\123",
                    "C:\\456",
                    "C:\\789",
                    nullptr,
                };

                for (auto _szPath : s_szPathList)
                {
                    Assert::IsTrue(::SetDllDirectoryA(_szPath));

                    const auto _uResult = ::GetDllDirectoryA(_countof(_szBuffer), _szBuffer);
                    if (_szPath)
                    {
                        Assert::AreEqual(size_t(_uResult), strlen(_szPath));
                        Assert::AreEqual(_szBuffer, _szPath);
                    }
                    else
                    {
                        Assert::AreEqual(_uResult, 0ul);
                    }
                }
            }
        }

        TEST_METHOD(W_A互调验证)
        {
            {
                Assert::IsTrue(::SetDllDirectoryA("C:\\123"));
                wchar_t _szBuffer[MAX_PATH];
                const auto _uResult = ::GetDllDirectoryW(_countof(_szBuffer), _szBuffer);
                Assert::AreEqual(size_t(_uResult), strlen("C:\\123"));
                Assert::AreEqual(_szBuffer, L"C:\\123");
            }

            {
                Assert::IsTrue(::SetDllDirectoryW(L"C:\\456"));
                char _szBuffer[MAX_PATH];
                const auto _uResult = ::GetDllDirectoryA(_countof(_szBuffer), _szBuffer);
                Assert::AreEqual(size_t(_uResult), strlen("C:\\456"));
                Assert::AreEqual(_szBuffer, "C:\\456");
            }

            Assert::IsTrue(::SetDllDirectoryW(nullptr));
        }
    };

    TEST_CLASS(GetDllDirectoryW_A)
    {
        AwaysNullGuard Guard;

    public:
        GetDllDirectoryW_A()
        {
            Guard |= YY::Thunks::aways_null_try_get_SetDllDirectoryW;
            Guard |= YY::Thunks::aways_null_try_get_GetDllDirectoryW;
            Guard |= YY::Thunks::aways_null_try_get_SetDllDirectoryA;
            Guard |= YY::Thunks::aways_null_try_get_GetDllDirectoryA;
        }

        TEST_METHOD(常规)
        {
            {
                Assert::IsTrue(::SetDllDirectoryA("C:\\123"));
                char _szBuffer[MAX_PATH];
                const auto _uResult = ::GetDllDirectoryA(_countof(_szBuffer), _szBuffer);
                Assert::AreEqual(size_t(_uResult), strlen("C:\\123"));
                Assert::AreEqual(_szBuffer, "C:\\123");
            }

            {
                Assert::IsTrue(::SetDllDirectoryW(L"C:\\456"));
                wchar_t _szBuffer[MAX_PATH];
                const auto _uResult = ::GetDllDirectoryW(_countof(_szBuffer), _szBuffer);
                Assert::AreEqual(size_t(_uResult), strlen("C:\\456"));
                Assert::AreEqual(_szBuffer, L"C:\\456");
            }

            Assert::IsTrue(::SetDllDirectoryW(nullptr));
        }

        TEST_METHOD(缓冲区不足验证)
        {
            Assert::IsTrue(::SetDllDirectoryW(L"C:\\456"));

            {
                wchar_t _szBuffer[6];
                const auto _uResult = ::GetDllDirectoryW(_countof(_szBuffer), _szBuffer);
                Assert::AreEqual(size_t(_uResult),  _countof(L"C:\\456"));
                Assert::AreEqual(_szBuffer, L"");
            }

            {
                wchar_t _szBuffer[7];
                const auto _uResult = ::GetDllDirectoryW(_countof(_szBuffer), _szBuffer);
                Assert::AreEqual(size_t(_uResult), wcslen(L"C:\\456"));
                Assert::AreEqual(_szBuffer, L"C:\\456");
            }

            {
                char _szBuffer[6];
                const auto _uResult = ::GetDllDirectoryA(_countof(_szBuffer), _szBuffer);
                Assert::AreEqual(size_t(_uResult), _countof("C:\\456"));
                Assert::AreEqual(_szBuffer, "");
            }

            {
                char _szBuffer[7];
                const auto _uResult = ::GetDllDirectoryA(_countof(_szBuffer), _szBuffer);
                Assert::AreEqual(size_t(_uResult), strlen("C:\\456"));
                Assert::AreEqual(_szBuffer, "C:\\456");
            }

            Assert::IsTrue(::SetDllDirectoryW(nullptr));
        }
    };
#endif

    TEST_CLASS(AddDllDirectory_RemoveDllDirectory)
    {
        AwaysNullGuard Guard;

    public:
        AddDllDirectory_RemoveDllDirectory()
        {
            Guard |= YY::Thunks::aways_null_try_get_AddDllDirectory;
            Guard |= YY::Thunks::aways_null_try_get_RemoveDllDirectory;
        }

        TEST_METHOD(常规)
        {
            auto _pCookie = AddDllDirectory(L"C:\\123");
            Assert::IsNotNull(_pCookie);
            Assert::IsTrue(RemoveDllDirectory(_pCookie));
        }

        TEST_METHOD(多次调用)
        {
            auto _pCookie1 = AddDllDirectory(L"C:\\123");
            Assert::IsNotNull(_pCookie1);

            auto _pCookie2 = AddDllDirectory(L"C:\\456");
            Assert::IsNotNull(_pCookie2);

            Assert::IsTrue(RemoveDllDirectory(_pCookie2));
            Assert::IsTrue(RemoveDllDirectory(_pCookie1));
        }
    };

    TEST_CLASS(LoadLibraryExW)
    {
        AwaysNullGuard Guard;

    public:
        LoadLibraryExW()
        {
            Guard |= YY::Thunks::aways_null_try_get_AddDllDirectory;
        }

        TEST_METHOD(LOAD_WITH_ALTERED_SEARCH_PATH验证)
        {
            auto _hModlule = ::LoadLibraryExW(LR"(C:\Windows\System32\advapi32)", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
            Assert::IsNotNull(_hModlule);
            FreeLibrary(_hModlule);
        }

        TEST_METHOD(参数合法性校验检查)
        {
            {
                // LOAD_WITH_ALTERED_SEARCH_PATH 预期无法与LOAD_LIBRARY_SEARCH_* 等一起使用
                auto _hModlule = ::LoadLibraryExW(LR"(C:\Windows\System32\advapi32)", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH | LOAD_LIBRARY_SEARCH_SYSTEM32);
                auto _lStatus = GetLastError();
                Assert::IsNull(_hModlule);
                Assert::AreEqual(_lStatus, DWORD(ERROR_INVALID_PARAMETER));
            }
        }

    };
}
