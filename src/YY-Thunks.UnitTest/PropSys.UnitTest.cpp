#include "pch.h"
#include "Thunks/PropSys.hpp"

namespace PropSys
{
    TEST_CLASS(VariantCompare)
    {
        AwaysNullGuard Guard;

    public:
        VariantCompare()
        {
            Guard |= YY::Thunks::aways_null_try_get_VariantCompare;
        }

        TEST_METHOD(一般)
        {
            {
                VARIANT _Left = {};
                VARIANT _Rigth = {};

                _Left.vt = VT_I1;
                _Left.bVal = 8;

                _Rigth.vt = VT_I1;
                _Rigth.bVal = 9;

                Assert::IsTrue(::VariantCompare(_Left, _Rigth) < 0);
            }

            {
                VARIANT _Left = {};
                VARIANT _Rigth = {};

                _Left.vt = VT_R4;
                _Left.fltVal = 8;

                _Rigth.vt = VT_R4;
                _Rigth.fltVal = 8;

                Assert::IsTrue(::VariantCompare(_Left, _Rigth) == 0);
            }

            {
                VARIANT _Left = {};
                VARIANT _Rigth = {};

                _Left.vt = VT_R4;
                _Left.fltVal = 9;

                _Rigth.vt = VT_R4;
                _Rigth.fltVal = 8;

                Assert::IsTrue(::VariantCompare(_Left, _Rigth) > 0);
            }

            {
                VARIANT _Left = {};
                VARIANT _Rigth = {};

                _Left.vt = VT_R4;
                _Left.fltVal = 8.1f;

                _Rigth.vt = VT_R4;
                _Rigth.fltVal = 8;

                Assert::IsTrue(::VariantCompare(_Left, _Rigth) > 0);
            }
        }
    };
}
