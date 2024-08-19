#if (YY_Thunks_Target < __WindowsNT6_2)
#define PATHCCH_NO_DEPRECATE
#include <pathcch.h>
#include <strsafe.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_2) && __YY_Thunks_libs && !defined(__Comment_Lib_pathcch)
#define __Comment_Lib_pathcch
#pragma comment(lib, "Pathcch.lib")
#endif

#ifdef YY_Thunks_Implemented
namespace YY::Thunks::internal
{
    namespace
    {
        static bool __fastcall IsHexDigitWorker(wchar_t ch)
        {
            return (ch >= L'0' && ch <= L'9') || (ch >= L'A' && ch <= L'F') || (ch >= L'a' && ch <= L'f');
        }

        static bool __fastcall StringIsGUIDWorker(LPCWSTR szStr)
        {
            return szStr[0] == L'{'
                && IsHexDigitWorker(szStr[1])
                && IsHexDigitWorker(szStr[2])
                && IsHexDigitWorker(szStr[3])
                && IsHexDigitWorker(szStr[4])
                && IsHexDigitWorker(szStr[5])
                && IsHexDigitWorker(szStr[6])
                && IsHexDigitWorker(szStr[7])
                && IsHexDigitWorker(szStr[8])
                && szStr[9] == L'-'
                && IsHexDigitWorker(szStr[10])
                && IsHexDigitWorker(szStr[11])
                && IsHexDigitWorker(szStr[12])
                && IsHexDigitWorker(szStr[13])
                && szStr[14] == L'-'
                && IsHexDigitWorker(szStr[15])
                && IsHexDigitWorker(szStr[16])
                && IsHexDigitWorker(szStr[17])
                && IsHexDigitWorker(szStr[18])
                && szStr[19] == L'-'
                && IsHexDigitWorker(szStr[20])
                && IsHexDigitWorker(szStr[21])
                && IsHexDigitWorker(szStr[22])
                && IsHexDigitWorker(szStr[23])
                && szStr[24] == L'-'
                && IsHexDigitWorker(szStr[25])
                && IsHexDigitWorker(szStr[26])
                && IsHexDigitWorker(szStr[27])
                && IsHexDigitWorker(szStr[28])
                && IsHexDigitWorker(szStr[29])
                && IsHexDigitWorker(szStr[30])
                && IsHexDigitWorker(szStr[31])
                && IsHexDigitWorker(szStr[32])
                && IsHexDigitWorker(szStr[33])
                && IsHexDigitWorker(szStr[34])
                && IsHexDigitWorker(szStr[35])
                && IsHexDigitWorker(szStr[36])
                && szStr[37] == L'}';
        }

        static bool __fastcall PathIsVolumeGUIDWorker(LPCWSTR pszPath)
        {
            return pszPath[0] == L'\\'
                && pszPath[1] == L'\\'
                && pszPath[2] == L'?'
                && pszPath[3] == L'\\'
                && (pszPath[4] == L'V' || pszPath[4] == L'v')
                && (pszPath[5] == L'O' || pszPath[5] == L'o')
                && (pszPath[6] == L'L' || pszPath[6] == L'l')
                && (pszPath[7] == L'U' || pszPath[7] == L'u')
                && (pszPath[8] == L'M' || pszPath[8] == L'm')
                && (pszPath[9] == L'E' || pszPath[9] == L'e')
                && internal::StringIsGUIDWorker(pszPath + 10);
        }

        /*
        以下后缀是有效的
        ""
        后缀内容最多255个字符
        不能包含 ' ' , '\' 以及 '.'

        */
        static bool __fastcall IsValidExtensionWorker(LPCWSTR pszExt)
        {
            if (pszExt == nullptr)
                return false;

            if (*pszExt == L'\0')
                return true;

            if (*pszExt == L'.')
                ++pszExt;


            //检查长度 以及非法字符检测
            const auto pszExtMax = pszExt + 256;

            for (;; ++pszExt)
            {
                //长度超过 256
                if (pszExt >= pszExtMax)
                {
                    return false;
                }

                if (*pszExt == L'\0')
                    break;

                if (*pszExt == L'.' || *pszExt == L' ' || *pszExt == L'\\')
                {
                    return false;
                }
            }

            return true;
        }

        //长命名？
        static bool __fastcall IsExtendedLengthDosDevicePath(LPCWSTR pszPath)
        {
            return pszPath[0] == L'\\'
                && pszPath[1] == L'\\'
                && pszPath[2] == L'?'
                && pszPath[3] == L'\\';
        }

        static LPCWSTR __fastcall FindPreviousBackslashWorker(LPCWSTR szPathStart, LPCWSTR szPathEnd)
        {
            for (; szPathStart < szPathEnd;)
            {
                --szPathEnd;

                if (*szPathEnd == L'\\')
                    return szPathEnd;
            }

            return nullptr;
        }

        static bool __fastcall PathIsRelativeWorker(LPCWSTR szPath)
        {
            return szPath[0] != L'\\'
                && (((L'A' <= szPath[0] && szPath[0] <= L'Z') || (L'a' <= szPath[0] && szPath[0] <= L'z')) && szPath[1] == L':') == false;
        }
    }
}
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    8,
    _Success_(return)
    BOOL,
    WINAPI,
    PathIsUNCEx,
        _In_ PCWSTR pszPath,
        _Outptr_opt_ PCWSTR* ppszServer
        )
    {
        if (ppszServer)
            *ppszServer = nullptr;

        if (pszPath[0] == L'\\' && pszPath[1] == L'\\')
        {
            if (pszPath[2] == L'?')
            {
                if (pszPath[3] == L'\\'
                    && (pszPath[4] == L'U' || pszPath[4] == L'u')
                    && (pszPath[5] == L'N' || pszPath[5] == L'n')
                    && (pszPath[6] == L'C' || pszPath[6] == L'c')
                    && pszPath[7] == L'\\')
                {
                    pszPath += 8;
                }
                else
                {
                    return FALSE;
                }
            }
            else
            {
                //PathIsVolumeGUIDWorker 判断是多余的，因为 "\\\\?\\Volume{GUID}" 这样的形式走不到这个分支

                pszPath += 2;
            }

            if (ppszServer)
                *ppszServer = pszPath;

            return TRUE;
        }

        return FALSE;

    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    4,
    BOOL,
    WINAPI,
    PathCchIsRoot,
        _In_opt_ PCWSTR pszPath
        )
    {
        // NULL or ""
        if (pszPath == nullptr || pszPath[0] == L'\0')
            return FALSE;


        // "C:\\" 形式
        if (((L'A' <= pszPath[0] && pszPath[0] <= L'Z') || (L'a' <= pszPath[0] && pszPath[0] <= L'z')) // iswalpha(pszPath[0])
            && pszPath[1] == L':'
            && pszPath[2] == L'\\'
            && pszPath[3] == L'\0')
        {
            return TRUE;
        }

        // "\\" 形式
        if (pszPath[0] == L'\\' && pszPath[1] == L'\0')
        {
            return TRUE;
        }


        /*
        * "\\\path1\\path2"
        * "\\\\path1"
        * "\\\\?\\UNC\\path1\\path2"
        * "\\\\?\\UNC\\path1"
        */
        PCWSTR pszServer;
        if (PathIsUNCEx(pszPath, &pszServer))
        {
            DWORD nSeparatorsCount = 0;

            for (; *pszServer;++pszServer)
            {
                if (*pszServer == L'\\')
                {
                    ++nSeparatorsCount;

                    if (nSeparatorsCount > 1 || pszServer[1] == L'\0')
                        return FALSE;
                }
            }

            return TRUE;
        }

        /*
        * "\\\\?\\C:\\"
        * "\\\\?\\Volume{guid}\\"
        */

        if (pszPath[0] == L'\\'
            && pszPath[1] == L'\\'
            && pszPath[2] == L'?'
            && pszPath[3] == L'\\')
        {
            // "\\\\?\\C:\\"
            if (((L'A' <= pszPath[4] && pszPath[4] <= L'Z') || (L'a' <= pszPath[4] && pszPath[4] <= L'z')) // iswalpha(pszPath[4])
                && pszPath[5] == L':'
                && pszPath[6] == L'\\'
                && pszPath[7] == L'\0')
            {
                return TRUE;
            }

            // "\\\\?\\Volume{guid}\\"
            if (internal::PathIsVolumeGUIDWorker(pszPath)
                && pszPath[48] == L'\\'
                && pszPath[49] == L'\0')
            {
                return TRUE;
            }

        }

        return FALSE;

    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    16,
    HRESULT,
    WINAPI,
    PathCchAddBackslashEx,
        _Inout_updates_(cchPath) PWSTR pszPath,
        _In_ size_t cchPath,
        _Outptr_opt_result_buffer_(*pcchRemaining) PWSTR* ppszEnd,
        _Out_opt_ size_t* pcchRemaining
        )
    {
        if (ppszEnd)
            *ppszEnd = nullptr;

        if (pcchRemaining)
            *pcchRemaining = 0;

        auto nLength = wcslen(pszPath);

        HRESULT hr;

        do
        {
            if (nLength >= cchPath)
            {
                hr = __HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
                break;
            }

            if (nLength && pszPath[nLength - 1] != L'\\')
            {
                //末尾不是 '\\' 所以我们添加 一个 '\\' 微软原版这里用了 StringCchCopyExW，但是我觉得这没有必要。

                ++nLength;
                if (nLength >= cchPath)
                {
                    hr = __HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
                    break;
                }

                pszPath[nLength - 1] = L'\\';
                pszPath[nLength] = L'\0';

                hr = S_OK;
            }
            else
            {
                hr = S_FALSE;
            }

            if (ppszEnd)
                *ppszEnd = pszPath + nLength;

            if (pcchRemaining)
                *pcchRemaining = cchPath - nLength;

        } while (false);

        return hr;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    8,
    HRESULT,
    WINAPI,
    PathCchAddBackslash,
        _Inout_updates_(cchPath) PWSTR pszPath,
        _In_ size_t cchPath
        )
    {
        return PathCchAddBackslashEx(pszPath, cchPath, nullptr, nullptr);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    16,
    HRESULT,
    WINAPI,
    PathCchRemoveBackslashEx,
        _Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
        _In_ size_t cchPath,
        _Outptr_opt_result_buffer_(*pcchRemaining) PWSTR* ppszEnd,
        _Out_opt_ size_t* pcchRemaining
        )
    {
        if (ppszEnd)
            *ppszEnd = nullptr;

        if (pcchRemaining)
            *pcchRemaining = 0;

        auto nLength = wcslen(pszPath);

        HRESULT hr = S_FALSE;


        do
        {
            if (nLength >= cchPath)
            {
                hr = E_INVALIDARG;
                break;
            }

            if (nLength && pszPath[nLength - 1] == L'\\')
            {
                --nLength;

                if (!PathCchIsRoot(pszPath))
                {
                    pszPath[nLength] = L'\0';
                    hr = S_OK;
                }
            }

            if (ppszEnd)
                *ppszEnd = pszPath + nLength;

            if (pcchRemaining)
                *pcchRemaining = cchPath - nLength;


        } while (false);

        return hr;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    8,
    HRESULT,
    WINAPI,
    PathCchRemoveBackslash,
        _Inout_updates_(cchPath) PWSTR pszPath,
        _In_ size_t cchPath
        )
    {
        return PathCchRemoveBackslashEx(pszPath, cchPath, nullptr, nullptr);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    8,
    HRESULT,
    WINAPI,
    PathCchSkipRoot,
        _In_ PCWSTR pszPath,
        _Outptr_ PCWSTR* ppszRootEnd
        )
    {
        if (pszPath == nullptr || *pszPath == L'\0' || ppszRootEnd == nullptr)
            return E_INVALIDARG;

        *ppszRootEnd = nullptr;

        PCWSTR szRootEnd;
        if (PathIsUNCEx(pszPath, &szRootEnd))
        {
            //如果是UNC路径，则尝试跳过 2个 '\\'
            DWORD nSeparate = 0;

            for (; *szRootEnd && nSeparate < 2; ++szRootEnd)
            {
                if (*szRootEnd == L'\\')
                {
                    ++nSeparate;
                }
            }
        }
        else if (pszPath[0] == L'\\' && pszPath[1] != L'\\')
        {
            // "\\XXXXXX" 一个相对路径
            szRootEnd = pszPath + 1;
        }
        else
        {
            if (internal::PathIsVolumeGUIDWorker(pszPath))
            {
                szRootEnd = pszPath + 48;
            }
            else if(pszPath[0] == L'\\'
                && pszPath[1] == L'\\'
                && pszPath[2] == L'?'
                && pszPath[3] == L'\\'
                && ((L'A' <= pszPath[4] && pszPath[4] <= L'Z') || (L'a' <= pszPath[4] && pszPath[4] <= L'z')) // iswalpha(pszPath[4])
                && pszPath[5] == L':')
            {
                // "\\\\?\\C:\\"
                szRootEnd = pszPath + 6;
            }
            else if(((L'A' <= pszPath[0] && pszPath[0] <= L'Z') || (L'a' <= pszPath[0] && pszPath[0] <= L'z')) // iswalpha(pszPath[0])
                && pszPath[1] == L':')
            {
                // "C:\\"
                szRootEnd = pszPath + 2;
            }
            else
            {
                return E_INVALIDARG;
            }

            if (*szRootEnd == L'\\')
                ++szRootEnd;
        }

        *ppszRootEnd = szRootEnd;

        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    8,
    HRESULT,
    WINAPI,
    PathCchStripToRoot,
        _Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
        _In_ size_t cchPath
        )
    {
        if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH)
            return E_INVALIDARG;

        PWSTR szRootEnd;
        auto hr = PathCchSkipRoot(pszPath, (PCWSTR*)&szRootEnd);

        do
        {
            if (FAILED(hr))
                break;

            if (szRootEnd >= pszPath + cchPath)
            {
                hr = E_INVALIDARG;
                break;
            }

            if (*szRootEnd)
            {
                *szRootEnd = L'\0';
                PathCchRemoveBackslash(pszPath, cchPath);
                hr = S_OK;
            }
            else
            {
                hr = PathCchRemoveBackslash(pszPath, cchPath);
                if (FAILED(hr))
                    break;
            }

            return hr;

        } while (false);


        *pszPath = L'\0';

        return hr;

    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    8,
    HRESULT,
    WINAPI,
    PathCchRemoveFileSpec,
        _Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
        _In_ size_t cchPath
        )
    {
        if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH)
            return E_INVALIDARG;

        PWSTR pszRootEnd;

        if (FAILED(PathCchSkipRoot(pszPath, (PCWSTR*)&pszRootEnd)))
        {
            pszRootEnd = pszPath;
        }

        const auto pszPathBufferEnd = pszPath + cchPath;

        if (pszRootEnd >= pszPathBufferEnd)
            return E_INVALIDARG;

        //搜索最后一个 ‘\\’

        LPWSTR szLast = pszRootEnd;

        for (; *pszRootEnd; ++pszRootEnd)
        {
            if (pszRootEnd >= pszPathBufferEnd)
                return E_INVALIDARG;

            if (*pszRootEnd == L'\\')
                szLast = pszRootEnd;
        }

        if (*szLast)
        {
            *szLast = L'\0';
            PathCchRemoveBackslash(pszPath, cchPath);

            return S_OK;
        }
        else
        {
            return PathCchRemoveBackslash(pszPath, cchPath);
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    12,
    HRESULT,
    WINAPI,
    PathCchFindExtension,
        _In_reads_(_Inexpressible_(cchPath)) PCWSTR pszPath,
        _In_ size_t cchPath,
        _Outptr_ PCWSTR* ppszExt
        )
    {
        if(ppszExt == nullptr)
            return E_INVALIDARG;

        *ppszExt = nullptr;

        if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH)
        {
            return E_INVALIDARG;
        }

        const auto szPathBufferEnd = pszPath + cchPath;
        PCWSTR pszExt = nullptr;

        for (;*pszPath;++pszPath)
        {
            if (pszPath >= szPathBufferEnd)
                return E_INVALIDARG;

            switch (*pszPath)
            {
            case L'\\':
            case L' ':
                pszExt = nullptr;
                break;
            case L'.':
                pszExt = pszPath;
                break;
            default:
                break;
            }
        }

        *ppszExt = pszExt ? pszExt : pszPath;

        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    12,
    HRESULT,
    WINAPI,
    PathCchAddExtension,
        _Inout_updates_(cchPath) PWSTR pszPath,
        _In_ size_t cchPath,
        _In_ PCWSTR pszExt
        )
    {
        if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH || internal::IsValidExtensionWorker(pszExt) == false)
        {
            //loc_1009895F
            return E_INVALIDARG;
        }


        //检测Unicode Path，如果不是，那么最多允许 260
        const auto bExtendedLengthDosDevicePath = internal::IsExtendedLengthDosDevicePath(pszPath);
        if (!bExtendedLengthDosDevicePath)
        {
            cchPath = min(MAX_PATH, cchPath);
        }

        PWSTR pszPathExt;

        auto hr = PathCchFindExtension(pszPath, cchPath, (LPCWSTR*)&pszPathExt);

        if (FAILED(hr))
        {
            return hr;
        }

        if (*pszPathExt != L'\0')
        {
            //已经存在后缀，所以我们什么也不做。
            return S_FALSE;
        }


        if (*pszExt == L'.')
            ++pszExt;

        //后缀是 ""
        if (*pszExt == L'\0')
        {
            return S_OK;
        }

        do
        {
            const auto szPathBufferEnd = pszPath + cchPath;
            auto szDest = pszPathExt;

            if (szDest >= szPathBufferEnd)
                break;

            *szDest = L'.';
            ++szDest;

            for (; szDest < szPathBufferEnd; ++szDest, ++pszExt)
            {
                if ((*szDest = *pszExt) == L'\0')
                {
                    return S_OK;
                }
            }

        } while (false);

        *pszPathExt = L'\0';

        if (bExtendedLengthDosDevicePath)
        {
            if (cchPath != PATHCCH_MAX_CCH)
            {
                return __HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            }
        }
        else
        {
            if (cchPath != MAX_PATH)
            {
                return __HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            }
        }


        return __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);

    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    12,
    HRESULT,
    WINAPI,
    PathCchRenameExtension,
        _Inout_updates_(cchPath) PWSTR pszPath,
        _In_ size_t cchPath,
        _In_ PCWSTR pszExt
        )
    {
        if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH || internal::IsValidExtensionWorker(pszExt) == false)
            return E_INVALIDARG;

        //检测Unicode Path，如果不是，那么最多允许 260
        const auto bExtendedLengthDosDevicePath = internal::IsExtendedLengthDosDevicePath(pszPath);
        if (!bExtendedLengthDosDevicePath)
        {
            cchPath = min(MAX_PATH, cchPath);
        }

        PWSTR pszPathExt;

        auto hr = PathCchFindExtension(pszPath, cchPath, (LPCWSTR*)&pszPathExt);

        if (FAILED(hr))
        {
            return hr;
        }

        if (*pszExt == L'.')
            ++pszExt;

        //如果调用者提供的 后缀为空，那么删除现有的，然后立即返回 S_OK
        if (*pszExt == L'\0')
        {
            *pszPathExt = L'\0';
            return S_OK;
        }

        //故意不用微软的方法，微软的方法在缓存区不足时会破坏原始值。
        //覆盖现有文件后缀
        const auto szPathBufferEnd = pszPath + cchPath;
        const auto cchExt = wcslen(pszExt);

        // . ext < szPathBufferEnd
        if (pszPathExt + 1 + cchExt + 1 <= szPathBufferEnd)
        {
            *pszPathExt = L'.';
            ++pszPathExt;

            memcpy(pszPathExt, pszExt, (cchExt + 1) * sizeof(pszExt[0]));

            return S_OK;
        }

            
        if (bExtendedLengthDosDevicePath)
        {
            if (cchPath != PATHCCH_MAX_CCH)
            {
                return __HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            }
        }
        else
        {
            if (cchPath != MAX_PATH)
            {
                return __HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            }
        }


        return __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    8,
    HRESULT,
    WINAPI,
    PathCchRemoveExtension,
        _Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
        _In_ size_t cchPath
        )
    {
        if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH)
            return E_INVALIDARG;

        const auto bExtendedLengthDosDevicePath = internal::IsExtendedLengthDosDevicePath(pszPath);
        if (!bExtendedLengthDosDevicePath)
        {
            cchPath = min(MAX_PATH, cchPath);
        }

        PWSTR pszPathExt;

        auto hr = PathCchFindExtension(pszPath, cchPath, (LPCWSTR*)&pszPathExt);

        if (FAILED(hr))
        {
            return hr;
        }


        if (*pszPathExt)
        {
            *pszPathExt = L'\0';
            return S_OK;
        }
        else
        {
            return S_FALSE;
        }
    }
#endif
        

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    16,
    HRESULT,
    APIENTRY,
    PathCchCanonicalizeEx,
        _Out_writes_(cchPathOut) PWSTR pszPathOut,
        _In_ size_t cchPathOut,
        _In_ PCWSTR pszPathIn,
        _In_ ULONG dwFlags
        )
    {
        if (const auto pPathCchCanonicalizeEx = try_get_PathCchCanonicalizeEx())
        {
            return pPathCchCanonicalizeEx(pszPathOut, cchPathOut, pszPathIn, dwFlags);
        }

        if (pszPathOut && cchPathOut)
            *pszPathOut = L'\0';

        if (cchPathOut == 0 || cchPathOut > PATHCCH_MAX_CCH)
        {
            return E_INVALIDARG;
        }

        // MAX_PATH 长度的路径重新禁止长命名
        if (cchPathOut <= MAX_PATH)
            dwFlags &= ~PATHCCH_ALLOW_LONG_PATHS;
        else if ((dwFlags & PATHCCH_ALLOW_LONG_PATHS) == 0)
            cchPathOut = MAX_PATH;

        DWORD ulReservedSize = 0;

        auto pTempOut = pszPathOut;
        const auto pszPathOutEnd = pszPathOut + cchPathOut;

        HRESULT hr = S_OK;

        do
        {
            PCWSTR pszServer;
            const auto bUNC = PathIsUNCEx(pszPathIn, &pszServer);
            if (bUNC)
            {
                if (dwFlags & PATHCCH_ALLOW_LONG_PATHS)
                {
                    ulReservedSize = 6;

                    if (pTempOut + 8 >= pszPathOutEnd)
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                    else
                    {
                        *pTempOut++ = L'\\';
                        *pTempOut++ = L'\\';
                        *pTempOut++ = L'?';
                        *pTempOut++ = L'\\';
                        *pTempOut++ = L'U';
                        *pTempOut++ = L'N';
                        *pTempOut++ = L'C';
                        *pTempOut++ = L'\\';
                        *pTempOut = L'\0';
                    }
                }
                else
                {
                    if (pTempOut + 2 >= pszPathOutEnd)
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                    else
                    {
                        *pTempOut++ = L'\\';
                        *pTempOut++ = L'\\';
                        *pTempOut = L'\0';
                    }
                }
            }
            else
            {
                pszServer = pszPathIn;

                auto bSupportDosDevicePath = false;

                if (internal::IsExtendedLengthDosDevicePath(pszPathIn))
                {
                    if ((L'A' <= pszPathIn[4] && pszPathIn[4] <= L'Z') || (L'a' <= pszPathIn[4] && pszPathIn[4] <= L'z')
                        && pszPathIn[5] == L':')
                    {
                        pszServer += 4;
                        bSupportDosDevicePath = true;
                    }
                }
                else if ((L'A' <= pszPathIn[0] && pszPathIn[0] <= L'Z') || (L'a' <= pszPathIn[0] && pszPathIn[0] <= L'z')
                    && pszPathIn[1] == L':')
                {
                    bSupportDosDevicePath = true;
                }

                if (bSupportDosDevicePath && (dwFlags & PATHCCH_ALLOW_LONG_PATHS))
                {
                    ulReservedSize = 4;

                    if (pTempOut + 4 >= pszPathOutEnd)
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                    else
                    {
                        *pTempOut++ = L'\\';
                        *pTempOut++ = L'\\';
                        *pTempOut++ = L'?';
                        *pTempOut++ = L'\\';
                        *pTempOut = L'\0';
                    }
                }
            }


            //LABEL_49
            //剩余部分是否 <= MAX_PATH，如果满足条件，那么应该重新开始。并且将路径限制在 MAX_PATH
            if ((dwFlags & PATHCCH_ALLOW_LONG_PATHS) && ulReservedSize && cchPathOut <= ulReservedSize + MAX_PATH)
            {
                if (cchPathOut > MAX_PATH)
                    cchPathOut = MAX_PATH;

                ulReservedSize = 0;
                pszServer = pszPathIn;

                pTempOut = pszPathOut;

                //缓冲区必然 >= 1 ,所以我们可以直接设置。
                *pTempOut = L'\0';
                // 清除错误，方便我们重新开始。
                hr = S_OK;
            }


            if (FAILED(hr))
                break;

            for (;*pszServer;)
            {
                /* 获取下一个文件名，故意不用微软的方案，而且效率低……
                * 遇到 '\\' 则说明文件名结束
                * 遇到 '\0' 则说明路径也结束了
                */
                unsigned cchFileName = 0;
                for (; pszServer[cchFileName] && pszServer[cchFileName] != L'\\'; ++cchFileName)
                {
                    /* 安全检查
                    * 1. 普通路径中，文件名最多 256个字符
                    * 2. 路径最大处理长度为 PATHCCH_MAX_CCH
                    */

                    if (((dwFlags & PATHCCH_ALLOW_LONG_PATHS) == 0 && cchFileName > MAXIMUM_FILENAME_LENGTH)
                        || cchFileName >= PATHCCH_MAX_CCH)
                    {
                        hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
                        goto OnFailedLabel;
                    }
                }

                if (cchFileName == 1 && pszServer[0] == L'.')
                {
                    //表示当前目录

                    ++pszServer;

                    if (*pszServer)
                    {
                        //说明这里是 '\\'，继续跳过
                        ++pszServer;
                    }
                    else
                    {
                        //这里是 L'\0'

                        if (pTempOut > pszPathOut && PathCchIsRoot(pszPathOut) == FALSE)
                        {
                            //不是跟目录，而且结束，那么需要删除最后的 '\\'
                            --pTempOut;
                            *pTempOut = L'\0';
                        }
                    }

                    continue;
                }
                else if (cchFileName == 2
                    && pszServer[0] == L'.'
                    && pszServer[1] == L'.')
                {
                    //表示上级目录

                    pszServer += 2;

                    if (pTempOut <= pszPathOut || PathCchIsRoot(pszPathOut))
                    {
                        //文件是根目录，或者开始位置时，遇到 ".." 则直接忽略即可。

                        if (*pszServer == L'\\')
                            ++pszServer;
                    }
                    else
                    {
                        auto szPrevious = (wchar_t*)internal::FindPreviousBackslashWorker(pszPathOut, pTempOut - 1);

                        pTempOut = szPrevious ? szPrevious : pszPathOut;

                        *pTempOut = L'\0';
                    }

                    continue;
                }
                else
                {
                    if (cchFileName == 0
                        && pszServer[0] == L'\\')
                    {
                        cchFileName = 1;
                    }


                    if (pTempOut + cchFileName >= pszPathOutEnd)
                    {
                        //空间不足，特殊判断一下，某些特定的形式是可以处理的。
                        if (cchFileName == 1 && pszServer[0] == L'\\')
                        {
                            if (pszServer[1] == L'\0' || (pszServer[1] == L'.' && pszServer[2] == L'\0'))
                            {
                                //可以确认，路径已经结束，所以无需再添加 '\\'
                                break;
                            }
                            else if (pTempOut + 1 == pszPathOutEnd
                                && pszServer[1] == L'.'
                                && pszServer[2] == L'.')
                            {
                                //下一个项目是返回上一个项目，所以可以伪造一下，直接设置为 L'\0' 然后往下移动
                                ++pszServer;
                                *pTempOut++ = L'\0';
                                continue;
                            }
                        }

                        hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
                        goto OnFailedLabel;
                    }

                    //复制文件名
                    memcpy(pTempOut, pszServer, cchFileName * sizeof(pszServer[0]));
                    pszServer += cchFileName;
                    pTempOut += cchFileName;
                    *pTempOut = L'\0';
                }
            }
                
            /* 删除 路径末尾的 .
            * "*." 则保留 
            */
            if (pTempOut > pszPathOut)
            {
                auto Str = pTempOut;
                --Str;

                if (*Str == L'.')
                {
                    for (;;)
                    {
                        if (Str == pszPathOut)
                        {
                            *Str = L'\0';
                            pTempOut = Str;
                            break;
                        }

                        --Str;

                        if (*Str != L'*')
                        {
                            pTempOut = Str + 1;
                            *pTempOut = L'\0';
                                
                            if (*Str == L'.')
                                continue;
                        }

                        //goto LABEL_29;
                        break;
                    }

                        
                }
            }

            //LABEL_29:

            constexpr int nCountOfData = _countof(L"::$DATA") - 1;

            if (pszPathOut + nCountOfData <= pTempOut
                && pTempOut[-7] == L':'
                && pTempOut[-6] == L':'
                && pTempOut[-5] == L'$'
                && (pTempOut[-4] == L'D' || pTempOut[-3] == L'd')
                && (pTempOut[-3] == L'A' || pTempOut[-3] == L'a')
                && (pTempOut[-2] == L'T' || pTempOut[-1] == L't')
                && (pTempOut[-1] == L'A' || pTempOut[-1] == L'a'))
            {
                pTempOut -= nCountOfData;
                *pTempOut = L'\0';
            }


            //处于长命名模式中，如果规范化后的路径有效部分小于 MAX_PATH，那么删除长命名前缀
            if (ulReservedSize)
            {
                if (size_t(pTempOut - pszPathOut) <= ulReservedSize + MAX_PATH)
                {
                    if (bUNC)
                    {
                        // "\\\\?\\UNC\\"
                        memmove(pszPathOut + 2, pszPathOut + 8, (pTempOut - (pszPathOut + 8) + 1) * sizeof(pTempOut[0]));
                    }
                    else
                    {
                        memmove(pszPathOut, pszPathOut + 4, (pTempOut - (pszPathOut + 4) + 1) * sizeof(pTempOut[0]));
                    }
                }
            }

                
            if (cchPathOut > 1 && *pszPathOut == L'\0')
            {
                //如果路径为 空，则转换为 "\\"
                pszPathOut[0] = L'\\';
                pszPathOut[1] = L'\0';
            }
            else if(cchPathOut > 3 && pszPathOut[1] == L':' && pszPathOut[2] == L'\0')
            {
                // 如果路径为 "X:"，则转换为 "X:\\"
                pszPathOut[2] = L'\\';
                pszPathOut[3] = L'\0';
            }

            return S_OK;

        } while (false);


        //LABEL_96 失败
    OnFailedLabel:

        *pszPathOut = L'\0';

        if (hr == STRSAFE_E_INSUFFICIENT_BUFFER)
        {
            if (dwFlags & PATHCCH_ALLOW_LONG_PATHS)
            {
                if(cchPathOut == PATHCCH_MAX_CCH)
                    hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
            }
            else
            {
                if (cchPathOut == MAX_PATH)
                    hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
            }
        }

        return hr;

    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    12,
    HRESULT,
    WINAPI,
    PathCchCanonicalize,
        _Out_writes_(cchPathOut) PWSTR pszPathOut,
        _In_ size_t cchPathOut,
        _In_ PCWSTR pszPathIn
        )
    {
        return PathCchCanonicalizeEx(pszPathOut, cchPathOut, pszPathIn, 0);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    20,
    HRESULT,
    WINAPI,
    PathCchCombineEx,
        _Out_writes_(cchPathOut) PWSTR pszPathOut,
        _In_ size_t cchPathOut,
        _In_opt_ PCWSTR pszPathIn,
        _In_opt_ PCWSTR pszMore,
        _In_ ULONG dwFlags
        )
    {
        if (pszPathOut == nullptr
            || cchPathOut == 0
            || cchPathOut > PATHCCH_MAX_CCH)
        {
            return E_INVALIDARG;
        }


        wchar_t PathOutStaticBuffer[MAX_PATH];
        PWSTR pszPathOutTmp = nullptr;
        size_t cchPathOutTmp = 0;

        HRESULT hr = S_OK;

        do
        {
            if (pszPathIn == nullptr && pszMore == nullptr)
            {
                hr = E_INVALIDARG;
                break;
            }

            size_t cchPathIn = pszPathIn ? wcslen(pszPathIn) : 0;
            if (cchPathIn >= PATHCCH_MAX_CCH)
            {
                hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
                break;
            }

            size_t cchMore = pszMore ? wcslen(pszMore) : 0;
            if (cchMore >= PATHCCH_MAX_CCH)
            {
                hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
                break;
            }

            //极端情况可能需要额外添加 '\\' 连接，简化处理，直接 + 1。改逻辑与微软不同。
            //字符串末尾需要 '\0' 结束
            const auto cchPathInNeed = cchPathIn + cchMore + 1 + 1;


            if (cchPathInNeed < _countof(PathOutStaticBuffer))
            {
                //静态缓冲区可以容纳，我们直接使用静态缓冲区
                pszPathOutTmp = PathOutStaticBuffer;
            }
            else
            {
                //好吧，我们真的需要开辟临时缓冲区
                pszPathOutTmp = (PWSTR)LocalAlloc(LMEM_ZEROINIT, cchPathInNeed * sizeof(wchar_t));
                if (!pszPathOutTmp)
                {
                    hr = E_OUTOFMEMORY;
                    break;
                }
            }

            if (cchPathIn == 0)
            {
                memcpy(pszPathOutTmp, pszMore, cchMore * sizeof(wchar_t));
                cchPathOutTmp = cchMore;
            }
            else if (cchMore == 0)
            {
                memcpy(pszPathOutTmp, pszPathIn, cchPathIn * sizeof(wchar_t));
                cchPathOutTmp = cchPathIn;
            }
            else
            {
                if (internal::PathIsRelativeWorker(pszMore))
                {
                    memcpy(pszPathOutTmp, pszPathIn, cchPathIn * sizeof(wchar_t));
                    cchPathOutTmp = cchPathIn;
                }
                else
                {
                    if (pszMore[0] == L'\\' && pszMore[1] != L'\\')
                    {
                        //路径是 '\\' 开头，那么 它与 pszPathIn 的根目录进行合并
                        ++pszMore;
                        --cchMore;


                        PCWSTR pszRootEnd;
                        hr = PathCchSkipRoot(pszPathIn, &pszRootEnd);
                        if (FAILED(hr))
                        {
                            break;
                        }

                        cchPathOutTmp = pszRootEnd - pszPathIn;
                        memcpy(pszPathOutTmp, pszPathIn, cchPathOutTmp * sizeof(wchar_t));
                    }
                    else
                    {
                        //这是一个绝对的路径，我们只需要把 pszMore 复制到 pszPathOutTmp
                        memcpy(pszPathOutTmp, pszMore, cchMore * sizeof(wchar_t));
                        cchPathOutTmp = cchMore;
                        break;
                    }
                }

                /*
                * 不想调用它……只是追加一个 '\' 而已
                PWSTR pszEnd;
                hr = PathCchAddBackslashEx(pszPathOutTmp, cchPathOutTmp, &pszEnd, &cchPathOutTmp);
                if (FAILED(hr))
                {
                    break;
                }
                */
                if (cchPathOutTmp && pszPathOutTmp[cchPathOutTmp - 1] != L'\\')
                {
                    pszPathOutTmp[cchPathOutTmp] = L'\\';
                    ++cchPathOutTmp;
                }

                memcpy(pszPathOutTmp + cchPathOutTmp, pszMore, cchMore * sizeof(wchar_t));
                cchPathOutTmp += cchMore;
            }

        } while (false);

        if (FAILED(hr))
        {
            *pszPathOut = L'\0';
        }
        else
        {
            //保证字符串 '\0' 截断
            pszPathOutTmp[cchPathOutTmp] = L'\0';
            hr = PathCchCanonicalizeEx(pszPathOut, cchPathOut, pszPathOutTmp, dwFlags);
        }

        if (pszPathOutTmp && pszPathOutTmp != PathOutStaticBuffer)
            LocalFree(pszPathOutTmp);

        return hr;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    16,
    HRESULT,
    WINAPI,
    PathCchCombine,
        _Out_writes_(cchPathOut) PWSTR pszPathOut,
        _In_ size_t cchPathOut,
        _In_opt_ PCWSTR pszPathIn,
        _In_opt_ PCWSTR pszMore
        )
    {
        return PathCchCombineEx(pszPathOut, cchPathOut, pszPathIn, pszMore, 0);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    16,
    HRESULT,
    WINAPI,
    PathCchAppendEx,
        _Inout_updates_(cchPath) PWSTR pszPath,
        _In_ size_t cchPath,
        _In_opt_ PCWSTR pszMore,
        _In_ ULONG dwFlags
        )
    {
        if (pszMore && PathIsUNCEx(pszMore, nullptr) == false && internal::IsExtendedLengthDosDevicePath(pszMore) == false)
        {
            //跳过开头的所有 '\\'
            for (;*pszMore == L'\\';++pszMore);
        }

        return PathCchCombineEx(pszPath, cchPath, pszPath, pszMore, dwFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    12,
    HRESULT,
    WINAPI,
    PathCchAppend,
        _Inout_updates_(cchPath) PWSTR pszPath,
        _In_ size_t cchPath,
        _In_opt_ PCWSTR pszMore
        )
    {
        return PathCchAppendEx(pszPath, cchPath, pszMore, 0);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    8,
    HRESULT,
    WINAPI,
    PathCchStripPrefix,
        _Inout_updates_(cchPath) PWSTR pszPath,
        _In_ size_t cchPath
        )
    {
        if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH)
            return E_INVALIDARG;

        if (!internal::IsExtendedLengthDosDevicePath(pszPath))
            return S_FALSE;

        if (cchPath < 4)
            return E_INVALIDARG;

        if ((pszPath[4] == L'U' || pszPath[4] == L'u')
            && (pszPath[5] == L'N' || pszPath[5] == L'n')
            && (pszPath[6] == L'C' || pszPath[6] == L'c')
            && pszPath[7] == L'\\')
        {
            if (cchPath < 8)
                return E_INVALIDARG;

            return StringCchCopyW(pszPath + 2, cchPath - 2, pszPath + 8);
        }
        else if ((L'A' <= pszPath[4] && pszPath[4] <= L'Z' || L'a' <= pszPath[4] && pszPath[4] <= L'z')
            && pszPath[5] == L':')
        {
            return StringCchCopyW(pszPath, cchPath, pszPath + 4);
        }
        else
        {
            return S_FALSE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    16,
    HRESULT,
    WINAPI,
    PathAllocCombine,
        _In_opt_ PCWSTR pszPathIn,
        _In_opt_ PCWSTR pszMore,
        _In_ ULONG dwFlags,
        _Outptr_ PWSTR* ppszPathOut
        )
    {
        if (const auto pPathAllocCombine = try_get_PathAllocCombine())
        {
            return pPathAllocCombine(pszPathIn, pszMore, dwFlags, ppszPathOut);
        }

        //参数检查
        if (ppszPathOut == nullptr)
            return E_INVALIDARG;

        *ppszPathOut = nullptr;

        //不能同时为 null
        if (pszPathIn == nullptr && pszMore == nullptr)
            return E_INVALIDARG;

        size_t cchPathIn = pszPathIn ? wcslen(pszPathIn) : 0;

        if(cchPathIn >= PATHCCH_MAX_CCH)
            return __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);

        if (cchPathIn)
            ++cchPathIn;
            
        size_t cchMore = pszMore ? wcslen(pszMore) : 0;

        if (cchMore >= PATHCCH_MAX_CCH)
            return __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);

        if (cchMore)
            ++cchMore;


        auto cchBufferSize = cchPathIn + cchMore;

        if (cchBufferSize == 0)
        {
            //"\\\0"
            cchBufferSize = 2;
        }
        else if (cchBufferSize > MAX_PATH && (dwFlags & PATHCCH_ALLOW_LONG_PATHS))
        {
            //"\\\\?\\UNC\0"
            cchBufferSize += 6;
        }

        //保证边界依然在 最大值内
        const unsigned cchMaxPath = (dwFlags & PATHCCH_ALLOW_LONG_PATHS) ? PATHCCH_MAX_CCH : MAX_PATH;
        if (cchBufferSize > cchMaxPath)
            cchBufferSize = cchMaxPath;

        auto pBuffer = (wchar_t*)LocalAlloc(LMEM_ZEROINIT, sizeof(wchar_t) * cchBufferSize);

        if (pBuffer == nullptr)
            return E_OUTOFMEMORY;

        auto hr = PathCchCombineEx(pBuffer, cchBufferSize, pszPathIn, pszMore, dwFlags);

        if (FAILED(hr))
        {
            LocalFree(pBuffer);
            return hr;
        }

        *ppszPathOut = pBuffer;

        return hr;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Minimum supported client	Windows 8 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_path_l1_1_0,
    12,
    HRESULT,
    WINAPI,
    PathAllocCanonicalize,
        _In_ PCWSTR pszPathIn,
        _In_ ULONG dwFlags,
        _Outptr_ PWSTR* ppszPathOut
        )
    {
        if (const auto pPathAllocCanonicalize = try_get_PathAllocCanonicalize())
        {
            return pPathAllocCanonicalize(pszPathIn, dwFlags, ppszPathOut);
        }


        //参数检查
        if (ppszPathOut == nullptr)
            return E_INVALIDARG;

        *ppszPathOut = nullptr;

        auto cchBufferSize = pszPathIn ? wcslen(pszPathIn) : 0;

        if(cchBufferSize >= PATHCCH_MAX_CCH)
            return __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);


        if (cchBufferSize)
        {
            ++cchBufferSize;
        }
        else
        {
            // "\\\0"
            cchBufferSize = 2;
        }

        if (cchBufferSize > MAX_PATH && (dwFlags & PATHCCH_ALLOW_LONG_PATHS))
        {
            //"\\\\?\\UNC\0"
            cchBufferSize += 6;
        }

        //保证边界依然在 最大值内
        const unsigned cchMaxPath = (dwFlags & PATHCCH_ALLOW_LONG_PATHS) ? PATHCCH_MAX_CCH : MAX_PATH;
        if (cchBufferSize > cchMaxPath)
            cchBufferSize = cchMaxPath;


        auto pBuffer = (wchar_t*)LocalAlloc(LMEM_ZEROINIT, sizeof(wchar_t) * cchBufferSize);

        if (pBuffer == nullptr)
            return E_OUTOFMEMORY;

        auto hr = PathCchCanonicalizeEx(pBuffer, cchBufferSize, pszPathIn, dwFlags);

        if (FAILED(hr))
        {
            LocalFree(pBuffer);
            return hr;
        }

        *ppszPathOut = pBuffer;

        return hr;
    }
#endif
}
