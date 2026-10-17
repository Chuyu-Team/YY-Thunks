#include "pch.h"
#include "Thunks/api-ms-win-core-string.hpp"

extern bool aways_null_try_get_RtlUpcaseUnicodeChar;

namespace api_ms_win_core_string
{
    TEST_CLASS(CompareStringOrdinal)
    {
        AwaysNullGuard Guard;

    public:
        CompareStringOrdinal()
        {
            Guard |= YY::Thunks::aways_null_try_get_CompareStringOrdinal;
        }

        TEST_METHOD(IgnoreCaseUsesTheSystemUppercaseMapping)
        {
            struct CompareCase
            {
                LPCWCH String1;
                LPCWCH String2;
                int Expected;
            };

            const CompareCase Cases[] =
            {
                { L"A", L"a", CSTR_EQUAL },
                { L"a", L"A", CSTR_EQUAL },
                { L"AbC", L"aBc", CSTR_EQUAL },
                { L"\u00c4", L"\u00e4", CSTR_EQUAL },
                { L"\u0391", L"\u03b1", CSTR_EQUAL },
                { L"\u0410", L"\u0430", CSTR_EQUAL },
                // Uppercasing before ordinal ordering places 'a' before '['.
                { L"a", L"[", CSTR_LESS_THAN },
                { L"[", L"a", CSTR_GREATER_THAN },
                { L"z", L"b", CSTR_GREATER_THAN },
                { L"b", L"z", CSTR_LESS_THAN },
                { L"abc", L"abc", CSTR_EQUAL },
                { L"", L"", CSTR_EQUAL },
                { L"a", L"ab", CSTR_LESS_THAN },
                { L"ab", L"a", CSTR_GREATER_THAN },
            };

            for (const auto& Case : Cases)
            {
                Assert::AreEqual(Case.Expected,
                    ::CompareStringOrdinal(Case.String1, -1, Case.String2, -1, TRUE));
                Assert::AreEqual(Case.Expected,
                    ::CompareStringOrdinal(Case.String1, int(wcslen(Case.String1)),
                        Case.String2, int(wcslen(Case.String2)), TRUE));
            }
        }

        TEST_METHOD(ExplicitAndMixedLengthsPreserveOrdinalSemantics)
        {
            struct CompareCase
            {
                LPCWCH String1;
                int Count1;
                LPCWCH String2;
                int Count2;
                BOOL IgnoreCase;
                int Expected;
            };

            const CompareCase Cases[] =
            {
                { L"a\0B", 3, L"A\0b", 3, TRUE, CSTR_EQUAL },
                { L"a\0b", 3, L"a\0c", 3, FALSE, CSTR_LESS_THAN },
                { L"a\0B", -1, L"a\0c", -1, TRUE, CSTR_EQUAL },
                { L"abcX", 3, L"abcY", 3, FALSE, CSTR_EQUAL },
                { L"abc", -1, L"abc", 3, TRUE, CSTR_EQUAL },
                { L"abc", 3, L"abc", -1, TRUE, CSTR_EQUAL },
                { L"abcdef", -1, L"abc", 3, TRUE, CSTR_GREATER_THAN },
                { L"abc", 3, L"abcdef", -1, TRUE, CSTR_LESS_THAN },
                { L"a", -1, L"abc", 3, TRUE, CSTR_LESS_THAN },
                { L"abc", 3, L"a", -1, TRUE, CSTR_GREATER_THAN },
                { L"abc", 0, L"xyz", 0, TRUE, CSTR_EQUAL },
                { L"abc", 0, L"x", 1, TRUE, CSTR_LESS_THAN },
                { L"x", 1, L"abc", 0, TRUE, CSTR_GREATER_THAN },
                { L"abc", 3, L"abc", 3, FALSE, CSTR_EQUAL },
                { L"abc", -1, L"abc", -1, FALSE, CSTR_EQUAL },
                { L"A", 1, L"a", 1, FALSE, CSTR_LESS_THAN },
                { L"a", 1, L"A", 1, FALSE, CSTR_GREATER_THAN },
                { L"A", -1, L"a", -1, FALSE, CSTR_LESS_THAN },
                { L"a", -1, L"A", -1, FALSE, CSTR_GREATER_THAN },
            };

            for (const auto& Case : Cases)
            {
                Assert::AreEqual(Case.Expected,
                    ::CompareStringOrdinal(Case.String1, Case.Count1,
                        Case.String2, Case.Count2, Case.IgnoreCase));
            }
        }

        TEST_METHOD(InvalidParametersFailBeforeCaseMapping)
        {
            struct InvalidCase
            {
                LPCWCH String1;
                int Count1;
                LPCWCH String2;
                int Count2;
                BOOL IgnoreCase;
            };

            const InvalidCase Cases[] =
            {
                { nullptr, 1, L"a", 1, TRUE },
                { L"a", 1, nullptr, 1, TRUE },
                { L"a", 1, L"a", 1, 2 },
                { L"a", 1, L"a", 1, -1 },
                { L"a", -2, L"a", 1, TRUE },
                { L"a", 1, L"a", -2, TRUE },
                { L"a", -2, L"a", -2, TRUE },
                { L"a", -2, L"a", -1, TRUE },
                { L"a", -1, L"a", -2, TRUE },
            };

            for (const auto& Case : Cases)
            {
                SetLastError(ERROR_SUCCESS);
                Assert::AreEqual(0, ::CompareStringOrdinal(Case.String1, Case.Count1,
                    Case.String2, Case.Count2, Case.IgnoreCase));
                Assert::AreEqual(DWORD(ERROR_INVALID_PARAMETER), GetLastError());
            }
        }

        TEST_METHOD(MissingUppercaseHelperFailsOnlyIgnoreCaseFallback)
        {
            AwaysNullGuard UppercaseGuard;
            UppercaseGuard |= ::aways_null_try_get_RtlUpcaseUnicodeChar;

            for (int Count : { 1, -1 })
            {
                SetLastError(ERROR_SUCCESS);
                Assert::AreEqual(0, ::CompareStringOrdinal(L"A", Count, L"a", Count, TRUE));
                Assert::AreEqual(DWORD(ERROR_CALL_NOT_IMPLEMENTED), GetLastError());
                Assert::AreEqual(int(CSTR_LESS_THAN),
                    ::CompareStringOrdinal(L"A", Count, L"a", Count, FALSE));
            }
        }

        TEST_METHOD(NativeFunctionTakesPrecedenceOverTheUppercaseHelper)
        {
            YY::Thunks::aways_null_try_get_CompareStringOrdinal = false;
            AwaysNullGuard UppercaseGuard;
            UppercaseGuard |= ::aways_null_try_get_RtlUpcaseUnicodeChar;

            Assert::AreEqual(int(CSTR_EQUAL), ::CompareStringOrdinal(L"A", 1, L"a", 1, TRUE));
        }
    };
}
