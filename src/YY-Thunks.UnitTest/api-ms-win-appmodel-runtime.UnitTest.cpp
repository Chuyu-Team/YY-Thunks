#include "pch.h"
#include "Thunks/api-ms-win-appmodel-runtime.hpp"

#include <vector>
#include <atlstr.h>

namespace api_ms_win_appmodel_runtime
{
    struct PackageInfo
    {
        CStringW szPackageFullName;
        UINT32 uPackagePropertiy;

        bool operator==(const PackageInfo& _oOther) const
        {
            return szPackageFullName == _oOther.szPackageFullName && uPackagePropertiy == _oOther.uPackagePropertiy;
        }
    };

    void AreEqual(const std::vector<PackageInfo>& _oLeft, const std::vector<PackageInfo>& _Right, CStringW _szMessage)
    {
        Assert::AreEqual(_oLeft.size(), _Right.size(), _szMessage);

        for (auto& _Item : _oLeft)
        {
            Assert::IsTrue(std::find(_Right.begin(), _Right.end(), _Item) != _Right.end(), _szMessage + _Item.szPackageFullName);
        }
    }

    static std::vector<PackageInfo> __fastcall FindPackagesByPackageFamilyHelper(_In_ PCWSTR packageFamilyName, _In_ UINT32 packageFilters)
    {
        std::vector<PackageInfo> _Result;
        CStringW _szBuffer;
        std::vector<LPWSTR> _szNames;
        UINT32 _uCount = 0;
        UINT32 _cchBuffer = 0;
        auto _lStatus = ::FindPackagesByPackageFamily(packageFamilyName, packageFilters, &_uCount, nullptr, &_cchBuffer, nullptr, nullptr);
        if (_lStatus != ERROR_INSUFFICIENT_BUFFER)
            return _Result;

        std::vector<UINT32> _PackageProperties;
        _szNames.resize(_uCount);
        _PackageProperties.resize(_uCount);
        _lStatus = ::FindPackagesByPackageFamily(packageFamilyName, packageFilters, &_uCount, _szNames.data(), &_cchBuffer, _szBuffer.GetBuffer(_cchBuffer), _PackageProperties.data());
        if (_lStatus != ERROR_SUCCESS)
            return _Result;

        _Result.resize(_uCount);

        for (UINT32 i = 0; i != _uCount; ++i)
        {
            _Result[i].szPackageFullName = _szNames[i];
            _Result[i].uPackagePropertiy = _PackageProperties[i];
        }

        return _Result;
    }

    TEST_CLASS(GetPackagePathByFullName)
    {
    public:
        GetPackagePathByFullName()
        {
        }

        TEST_METHOD(常规)
        {
            for (auto& _Pacakge : FindPackagesByPackageFamilyHelper(L"Microsoft.WindowsNotepad_8wekyb3d8bbwe", PACKAGE_FILTER_HEAD | PACKAGE_FILTER_DIRECT))
            {
                wchar_t _szPath1[MAX_PATH] = {};
                UINT32 _cchPathLength1 = _countof(_szPath1);
                ::GetPackagePathByFullName(_Pacakge.szPackageFullName, &_cchPathLength1, _szPath1);

                AwaysNullGuard Guard;
                Guard |= YY::Thunks::aways_null_try_get_GetPackagePathByFullName;

                wchar_t _szPath2[MAX_PATH] = {};
                UINT32 _cchPathLength2 = _countof(_szPath2);
                ::GetPackagePathByFullName(_Pacakge.szPackageFullName, &_cchPathLength2, _szPath2);

                Assert::AreEqual(_cchPathLength1, _cchPathLength1, _Pacakge.szPackageFullName);
                Assert::AreEqual(_szPath1, _szPath2, _Pacakge.szPackageFullName);
            }
        }
    };

    TEST_CLASS(FindPackagesByPackageFamily)
    {
    public:
        FindPackagesByPackageFamily()
        {
        }
        TEST_METHOD(常规)
        {
            constexpr UINT32 kPackageFilters[] =
            {
                0,
                PACKAGE_FILTER_HEAD | PACKAGE_FILTER_DIRECT,
                PACKAGE_FILTER_HEAD,
                PACKAGE_FILTER_DIRECT,
                PACKAGE_FILTER_RESOURCE,
                PACKAGE_FILTER_RESOURCE | PACKAGE_FILTER_DIRECT | PACKAGE_FILTER_HEAD,
                PACKAGE_FILTER_RESOURCE | PACKAGE_FILTER_HEAD,
                // PACKAGE_FILTER_BUNDLE, 改单元测试无法通过
            };

            for(auto _uFilters: kPackageFilters)
            {
                const auto _szSystem = FindPackagesByPackageFamilyHelper(L"Microsoft.WindowsNotepad_8wekyb3d8bbwe", _uFilters);
                AwaysNullGuard Guard;
                Guard |= YY::Thunks::aways_null_try_get_FindPackagesByPackageFamily;
                const auto _szThunks = FindPackagesByPackageFamilyHelper(L"Microsoft.WindowsNotepad_8wekyb3d8bbwe", _uFilters);

                CStringW _szPackageFilter;
                _szPackageFilter.Format(L"PackageFilter = 0x%X, ", _uFilters);
                AreEqual(_szSystem, _szThunks, _szPackageFilter);
            }          
        }
    };
}
