#if (YY_Thunks_Target < __WindowsNT6)
#include <strsafe.h>
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps | UWP apps]
    //Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    36,
    int,
    WINAPI,
    CompareStringEx,
        _In_opt_ LPCWSTR lpLocaleName,
        _In_ DWORD dwCmpFlags,
        _In_NLS_string_(cchCount1) LPCWCH lpString1,
        _In_ int cchCount1,
        _In_NLS_string_(cchCount2) LPCWCH lpString2,
        _In_ int cchCount2,
        _Reserved_ LPNLSVERSIONINFO lpVersionInformation,
        _Reserved_ LPVOID lpReserved,
        _Reserved_ LPARAM lParam
        )
    {
        if (auto const pCompareStringEx = try_get_CompareStringEx())
        {
            return pCompareStringEx(lpLocaleName, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2, lpVersionInformation, lpReserved, lParam);
        }

        const auto Locale = LocaleNameToLCID(lpLocaleName, 0);
        if (Locale == 0)
        {
            return 0;
        }

        return CompareStringW(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps | UWP apps]
    //Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    20,
    int,
    WINAPI,
    CompareStringOrdinal,
        _In_NLS_string_(cchCount1) LPCWCH lpString1,
        _In_ int cchCount1,
        _In_NLS_string_(cchCount2) LPCWCH lpString2,
        _In_ int cchCount2,
        _In_ BOOL bIgnoreCase
        )
    {
        if (auto const pCompareStringOrdinal = try_get_CompareStringOrdinal())
        {
            return pCompareStringOrdinal(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);
        }



        auto ToLowerCase = [](unsigned short ch) -> wchar_t
        {
            static constexpr wchar_t UnicodeCaseTableData[] =
            {
                0x0110, 0x0120, 0x0130, 0x0140, 0x0150, 0x0160, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0170, 0x0180, 0x0190,
                0x0100, 0x01A0, 0x0100, 0x0100, 0x01B0, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x01C0, 0x01D0, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x01E0, 0x01F0,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100,
                0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0200,
            };

            return (unsigned int)ch + UnicodeCaseTableData[(ch & 0xF) + UnicodeCaseTableData[((ch >> 4) & 0xF) + UnicodeCaseTableData[(unsigned short)(ch >> 8)]]];
        };



        if ((bIgnoreCase == TRUE || bIgnoreCase == FALSE)
            && lpString1 && lpString2)
        {
            if (cchCount1 >= 0 || cchCount2 >= 0)
            {
                //CompareStringOrdinalCounted

                size_t __cchCount1;
                if (cchCount1 >= 0)
                {
                    __cchCount1 = cchCount1;
                }
                else if (cchCount1 == -1)
                {
                    if (FAILED(StringCchLengthW(lpString1, cchCount2 + 1, &__cchCount1)))
                    {
                        __cchCount1 = cchCount2 + 1;
                    }
                }
                else
                {
                    SetLastError(ERROR_INVALID_PARAMETER);
                    return 0;
                }

                size_t __cchCount2;
                if (cchCount2 >= 0)
                {
                    __cchCount2 = cchCount2;
                }
                else if (cchCount2 == -1)
                {
                    if (FAILED(StringCchLengthW(lpString2, cchCount1 + 1, &__cchCount2)))
                    {
                        __cchCount2 = cchCount1 + 1;
                    }
                }
                else
                {
                    SetLastError(ERROR_INVALID_PARAMETER);
                    return 0;
                }

                size_t cchLength = min(__cchCount1, __cchCount2);

                if (bIgnoreCase)
                {
                    for (size_t Index = 0; Index != cchLength; ++Index)
                    {
                        auto ch1 = lpString1[Index];
                        auto ch2 = lpString2[Index];

                        if (ch1 != ch2)
                        {
                            ch1 = ToLowerCase(ch1);
                            ch2 = ToLowerCase(ch2);

                            if (ch1 > ch2)
                            {
                                return CSTR_GREATER_THAN;
                            }
                            else if (ch1 < ch2)
                            {
                                return CSTR_LESS_THAN;
                            }
                        }
                    }
                }
                else
                {
                    for (size_t Index = 0; Index != cchLength; ++Index)
                    {
                        auto ch1 = lpString1[Index];
                        auto ch2 = lpString2[Index];

                        if (ch1 > ch2)
                        {
                            return CSTR_GREATER_THAN;
                        }
                        else if (ch1 < ch2)
                        {
                            return CSTR_LESS_THAN;
                        }
                    }
                }

                if (__cchCount1 == __cchCount2)
                    return CSTR_EQUAL;
                else if (__cchCount1 > __cchCount2)
                    return CSTR_GREATER_THAN;
                else
                    return CSTR_LESS_THAN;
            }
        
            if(cchCount1 == -1 && cchCount2 == -1)
            {
                //CompareStringOrdinalNullTerminated
                if (bIgnoreCase)
                {
                    for (;;++lpString1, ++lpString2)
                    {
                        auto ch1 = *lpString1;
                        auto ch2 = *lpString2;

                        if (ch1 != ch2)
                        {
                            ch1 = ToLowerCase(ch1);
                            ch2 = ToLowerCase(ch2);

                            if (ch1 > ch2)
                            {
                                return CSTR_GREATER_THAN;
                            }
                            else if (ch1 < ch2)
                            {
                                return CSTR_LESS_THAN;
                            }
                        }

                        if (ch1 == '\0')
                        {
                            return CSTR_EQUAL;
                        }
                    }
                }
                else
                {
                    for (;;++lpString1, ++lpString2)
                    {
                        auto ch1 = *lpString1;
                        auto ch2 = *lpString2;
                        if (ch1 > ch2)
                        {
                            return CSTR_GREATER_THAN;
                        }
                        else if (ch1 < ch2)
                        {
                            return CSTR_LESS_THAN;
                        }
                        else
                        {
                            if (ch1 == '\0')
                            {
                                return CSTR_EQUAL;
                            }
                        }
                    }
                }
            }
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows 2000 Professional [desktop apps only]
    // Minimum supported server	Windows 2000 Server [desktop apps only]
    // 虽然Windows 2000已经支持，但是几个新标记老系统不支持，特殊处理一下。
    __DEFINE_THUNK(
    kernel32,
    24,
    int,
    WINAPI,
    CompareStringA,
        _In_ LCID     Locale,
        _In_ DWORD    dwCmpFlags,
        _In_reads_(cchCount1) PCNZCH lpString1,
        _In_ int      cchCount1,
        _In_reads_(cchCount2) PCNZCH  lpString2,
        _In_ int      cchCount2)
    {
        // 网友 海好蓝 提供
        const auto _pfnCompareStringA = try_get_CompareStringA();
        if (!_pfnCompareStringA)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return 0;
        }

#if (YY_Thunks_Target < __WindowsNT5_1)
        // Windows 2000 不支持 LOCALE_INVARIANT 参数特殊处理一下。
        // 网友 海好蓝 提供
        if (Locale == LOCALE_INVARIANT && internal::GetSystemVersion() < internal::MakeVersion(5, 1))
        {
            Locale = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
        }
#endif

#if (YY_Thunks_Target < __WindowsNT6)
        // Windows XP不支持 NORM_LINGUISTIC_CASING
        // 网友 海好蓝 反馈
        if (internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            dwCmpFlags &= ~NORM_LINGUISTIC_CASING;
        }
#endif
        return _pfnCompareStringA(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows 2000 Professional [desktop apps only]
    // Minimum supported server	Windows 2000 Server [desktop apps only]
    // 虽然Windows 2000已经支持，但是几个新标记老系统不支持，特殊处理一下。
    __DEFINE_THUNK(
    kernel32,
    24,
    int,
    WINAPI,
    CompareStringW,
        _In_ LCID Locale,
        _In_ DWORD dwCmpFlags,
        _In_NLS_string_(cchCount1) PCNZWCH lpString1,
        _In_ int cchCount1,
        _In_NLS_string_(cchCount2) PCNZWCH lpString2,
        _In_ int cchCount2
        )
    {
        const auto _pfnCompareStringW = try_get_CompareStringW();
        if (!_pfnCompareStringW)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return 0;
        }

#if (YY_Thunks_Target < __WindowsNT5_1)
        // Windows 2000 不支持 LOCALE_INVARIANT 参数特殊处理一下。
        // 网友 海好蓝 提供
        if (Locale == LOCALE_INVARIANT && internal::GetSystemVersion() < internal::MakeVersion(5, 1))
        {
            Locale = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
        }
#endif

#if (YY_Thunks_Target < __WindowsNT6)
        // Windows XP不支持 NORM_LINGUISTIC_CASING
        // 网友 海好蓝 反馈
        if (internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            dwCmpFlags &= ~NORM_LINGUISTIC_CASING;
        }
#endif

        return _pfnCompareStringW(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2);
    }
#endif
}
