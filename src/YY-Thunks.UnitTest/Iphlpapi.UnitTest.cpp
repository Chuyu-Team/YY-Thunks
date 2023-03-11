#include "pch.h"
#include "Thunks/Iphlpapi.hpp"


namespace Iphlpapi
{
    TEST_CLASS(GetIfTable2)
    {
    public:
        GetIfTable2()
        {
            YY::Thunks::aways_null_try_get_GetIfEntry2 = true;
            YY::Thunks::aways_null_try_get_GetIfTable2 = true;
            YY::Thunks::aways_null_try_get_FreeMibTable = true;
        }

        TEST_METHOD(GetIfTable2简单验证)
        {
            YY::Thunks::aways_null_try_get_GetIfTable2 = false;
            PMIB_IF_TABLE2 _pTable2_OS;
            auto _lStatus = ::GetIfTable2(&_pTable2_OS);
            Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);

            YY::Thunks::aways_null_try_get_GetIfTable2 = true;
            PMIB_IF_TABLE2 _pTable2_YY;
            _lStatus = ::GetIfTable2(&_pTable2_YY);
            Assert::AreEqual(_lStatus, (DWORD)NO_ERROR);


            Assert::AreEqual(_pTable2_YY->NumEntries, _pTable2_OS->NumEntries);

            for (DWORD i = 0; i != _pTable2_YY->NumEntries; ++i)
            {
                Assert::AreEqual(_pTable2_YY->Table[i].InterfaceIndex, _pTable2_OS->Table[i].InterfaceIndex);
                Assert::AreEqual(std::wstring(_pTable2_YY->Table[i].Description), std::wstring(_pTable2_OS->Table[i].Description));
                Assert::IsTrue(_pTable2_YY->Table[i].InterfaceGuid == _pTable2_OS->Table[i].InterfaceGuid);
            }

            ::FreeMibTable(_pTable2_YY);
        }
    };
}
