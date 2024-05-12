#include "pch.h"
#include "Thunks/shell32.hpp"

namespace shell32
{
    __if_exists(YY::Thunks::aways_null_try_get_SHGetKnownFolderPath)
    {
        TEST_CLASS(SHGetKnownFolderPath)
        {
            AwaysNullGuard Guard;

        public:
            SHGetKnownFolderPath()
            {
                Guard |= YY::Thunks::aways_null_try_get_SHGetKnownFolderPath;
            }

            TEST_METHOD(基本验证)
            {
                {
                    wchar_t* _szValue = nullptr;
                    auto hr = ::SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, nullptr, &_szValue);
                    Assert::IsTrue(SUCCEEDED(hr));
                    CoTaskMemFree(_szValue);
                }

                {
                    wchar_t* _szValue = nullptr;
                    auto hr = ::SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE | KF_FLAG_DEFAULT_PATH, nullptr, &_szValue);
                    Assert::IsTrue(SUCCEEDED(hr));
                    CoTaskMemFree(_szValue);
                }

                {
                    wchar_t* _szValue = nullptr;
                    auto hr = ::SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE | KF_FLAG_DEFAULT_PATH | KF_FLAG_NOT_PARENT_RELATIVE, nullptr, &_szValue);
                    Assert::IsTrue(SUCCEEDED(hr));
                    CoTaskMemFree(_szValue);
                }

                // KF_FLAG_NOT_PARENT_RELATIVE必须跟 KF_FLAG_DEFAULT_PATH一起使用
                {
                    wchar_t* _szValue = nullptr;
                    auto hr = ::SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE | KF_FLAG_NOT_PARENT_RELATIVE, nullptr, &_szValue);
                    Assert::AreEqual(hr, E_INVALIDARG);
                }
            }
        };
    }

    __if_exists(YY::Thunks::aways_null_try_get_SHGetKnownFolderIDList)
    {
        TEST_CLASS(SHGetKnownFolderIDList)
        {
            AwaysNullGuard Guard;

        public:
            SHGetKnownFolderIDList()
            {
                Guard |= YY::Thunks::aways_null_try_get_SHGetKnownFolderIDList;
            }

            TEST_METHOD(基本验证)
            {
                PIDLIST_ABSOLUTE _List = nullptr;
                auto hr = ::SHGetKnownFolderIDList(FOLDERID_Documents, KF_FLAG_CREATE, nullptr, &_List);
                Assert::IsTrue(SUCCEEDED(hr));
                ILFree(_List);
            }
        };
    }
}
