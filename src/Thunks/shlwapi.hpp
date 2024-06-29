#if (YY_Thunks_Target < __WindowsNT5_1)
#include <Shlwapi.h>
#endif

#ifdef YY_Thunks_Implemented
namespace YY::Thunks::internal
{
    namespace
    {
        template<class Char>
        static
            _Success_(return)
            BOOL
            WINAPI
            StrToInt64ExT(
                _In_ const Char* pszString,
                _In_ STIF_FLAGS dwFlags,
                _Out_ LONGLONG* pllRet
            )
        {
            if (!pszString)
                return FALSE;


            for (; ; ++pszString)
            {
                const auto ch = *pszString;

                if (ch == Char(' ') || ch == Char('\n') || ch == Char('\t'))
                {
                    continue;
                }

                break;
            }

            bool Sign = true;
            if (*pszString == Char('+'))
            {
                ++pszString;
            }
            else if (*pszString == Char('-'))
            {
                ++pszString;
                Sign = false;
            }

            ULONGLONG result = 0;

            if (dwFlags & STIF_SUPPORT_HEX)
            {
                //二种形式0x????, 0X?????
                if (*pszString == Char('0') && (pszString[1] == Char('x') || pszString[1] == Char('X')))
                {
                    pszString += 2;

                    for (auto String = pszString; ; ++String)
                    {
                        const auto ch = *String;
                        if (ch >= Char('0') && ch <= Char('9'))
                        {
                            result = (result << 4) | ((ULONGLONG)ch - Char('0'));
                        }
                        else if (ch >= Char('a') && ch <= Char('f'))
                        {
                            result = (result << 4) | ((ULONGLONG)ch - Char('a') + 10);
                        }
                        else if (ch >= Char('A') && ch <= Char('F'))
                        {
                            result = (result << 4) | ((ULONGLONG)ch - Char('A') + 10);
                        }
                        else
                        {
                            if (String == pszString)
                            {
                                return FALSE;
                            }

                            if (pllRet)
                            {
                                *pllRet = result;
                            }

                            return TRUE;
                        }
                    }
                }
            }

            for (auto String = pszString; ; ++String)
            {
                const auto ch = *String;
                if (ch >= Char('0') && ch <= ('9'))
                {
                    result = (result * 10) + ((ULONGLONG)ch - Char('0'));
                }
                else
                {
                    if (String == pszString)
                    {
                        return FALSE;
                    }

                    if (pllRet)
                    {
                        if (!Sign)
                        {
                            result = ~result + 1;
                        }

                        *pllRet = result;
                    }

                    return TRUE;
                }
            }

            return FALSE;
        }
    }
}
#endif


namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_1)

    // Windows 2000 Professional, Windows XP [desktop apps only] 
    // Windows 2000 Server [desktop apps only]
    // 虽然Windows 2000支持，但是在IE6里面而IE6不一定所有Windows 2000都有，因此低于5.1我们就静态引入。
    __DEFINE_THUNK(
    shlwapi,
    12,
    BOOL,
    WINAPI,
    StrToInt64ExA,
        _In_ PCSTR pszString,
        STIF_FLAGS dwFlags,
        _Out_ LONGLONG* pllRet
        )
    {
        if (const auto pStrToInt64ExA = try_get_StrToInt64ExA())
        {
            return pStrToInt64ExA(pszString, dwFlags, pllRet);
        }

        return internal::StrToInt64ExT(pszString, dwFlags, pllRet);
    }
#endif //YY_Thunks_Target < __WindowsNT5_1


#if (YY_Thunks_Target < __WindowsNT5_1)

    // Windows 2000 Professional, Windows XP [desktop apps only] 
    // Windows 2000 Server [desktop apps only]
    // 虽然Windows 2000支持，但是在IE6里面而IE6不一定所有Windows 2000都有，因此低于5.1我们就静态引入。
    __DEFINE_THUNK(
    shlwapi,
    12,
    BOOL,
    WINAPI,
    StrToInt64ExW,
        _In_ PCWSTR pszString,
        STIF_FLAGS dwFlags,
        _Out_ LONGLONG* pllRet
        )
    {
        if (const auto pStrToInt64ExW = try_get_StrToInt64ExW())
        {
            return pStrToInt64ExW(pszString, dwFlags, pllRet);
        }

        return internal::StrToInt64ExT(pszString, dwFlags, pllRet);
    }
#endif //YY_Thunks_Target < __WindowsNT5_1
}
