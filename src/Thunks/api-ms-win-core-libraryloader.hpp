
namespace YY {
namespace Thunks {
#ifndef YY_Thunks_Defined
namespace internal {
/*LSTATUS __fastcall BasepGetModuleHandleExParameterValidation(
    _In_ DWORD dwFlags,
    _In_opt_ LPCSTR lpModuleName,
    _Out_ HMODULE* phModule
    )*/
#define BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule)                 \
    for (;;)                                                                                       \
    {                                                                                              \
        if (phModule)                                                                              \
        {                                                                                          \
            *phModule = nullptr;                                                                   \
            if ((dwFlags &                                                                         \
                 ~(GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT |  \
                   GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)) == 0 ||                                \
                (dwFlags & (GET_MODULE_HANDLE_EX_FLAG_PIN |                                        \
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)) !=                      \
                    (GET_MODULE_HANDLE_EX_FLAG_PIN |                                               \
                     GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) ||                              \
                (lpModuleName || (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS) == 0))         \
            {                                                                                      \
                if (lpModuleName == nullptr)                                                       \
                {                                                                                  \
                    *phModule = (HMODULE)((TEB *)NtCurrentTeb())                                   \
                                    ->ProcessEnvironmentBlock->ImageBaseAddress;                   \
                    return TRUE;                                                                   \
                }                                                                                  \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
        SetLastError(ERROR_INVALID_PARAMETER);                                                     \
        return FALSE;                                                                              \
    }
} // namespace internal
#endif

#if (YY_Thunks_Support_Version < NTDDI_WINXP)
// Windows XP [desktop apps only]
// Windows Server 2003 [desktop apps only]

EXTERN_C
BOOL WINAPI
GetModuleHandleExA(_In_ DWORD dwFlags, _In_opt_ LPCSTR lpModuleName, _Out_ HMODULE *phModule)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (const auto pGetModuleHandleExA = try_get_GetModuleHandleExA())
    {
        return pGetModuleHandleExA(dwFlags, lpModuleName, phModule);
    }

    BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule);

    if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
    {
        EnterCriticalSection(((TEB *)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
    }

    LSTATUS lStatus = ERROR_SUCCESS;

    for (;;)
    {
        HMODULE hModule;

        if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
        {
            auto pRtlPcToFileHeader = try_get_RtlPcToFileHeader();
            if (!pRtlPcToFileHeader)
            {
                lStatus = ERROR_NOT_SUPPORTED;
                break;
            }

            hModule = (HMODULE)pRtlPcToFileHeader((PVOID)lpModuleName, (PVOID *)&hModule);
        }
        else
        {
            hModule = GetModuleHandleA(lpModuleName);
        }

        if (hModule == nullptr)
        {
            lStatus = ERROR_DLL_NOT_FOUND;
            break;
        }

        if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)
        {
        }
        else
        {
            const auto pLdrAddRefDll = try_get_LdrAddRefDll();
            if (!pLdrAddRefDll)
            {
                lStatus = ERROR_NOT_SUPPORTED;
                break;
            }

            auto Status = pLdrAddRefDll(dwFlags & GET_MODULE_HANDLE_EX_FLAG_PIN, hModule);
            if (Status < 0)
            {
                lStatus = internal::BaseSetLastNTError(Status);
                break;
            }
        }

        *phModule = hModule;

        break;
    }

    if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
    {
        LeaveCriticalSection(((TEB *)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
    }

    if (lStatus)
    {
        SetLastError(lStatus);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetModuleHandleExA, 12);
#endif

#if (YY_Thunks_Support_Version < NTDDI_WINXP)
// Windows XP [desktop apps only]
// Windows Server 2003 [desktop apps only]

EXTERN_C
BOOL WINAPI
GetModuleHandleExW(_In_ DWORD dwFlags, _In_opt_ LPCWSTR lpModuleName, _Out_ HMODULE *phModule)
#ifdef YY_Thunks_Defined
    ;
#else
{
    if (const auto pGetModuleHandleExW = try_get_GetModuleHandleExW())
    {
        return pGetModuleHandleExW(dwFlags, lpModuleName, phModule);
    }

    BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule);

    if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
    {
        EnterCriticalSection(((TEB *)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
    }

    LSTATUS lStatus = ERROR_SUCCESS;

    for (;;)
    {
        HMODULE hModule;

        if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
        {
            const auto pRtlPcToFileHeader = try_get_RtlPcToFileHeader();
            if (!pRtlPcToFileHeader)
            {
                lStatus = ERROR_NOT_SUPPORTED;
                break;
            }

            hModule = (HMODULE)pRtlPcToFileHeader((PVOID)lpModuleName, (PVOID *)&hModule);
        }
        else
        {
            hModule = GetModuleHandleW(lpModuleName);
        }

        if (hModule == nullptr)
        {
            lStatus = ERROR_DLL_NOT_FOUND;
            break;
        }

        if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)
        {
        }
        else
        {
            const auto pLdrAddRefDll = try_get_LdrAddRefDll();
            if (!pLdrAddRefDll)
            {
                lStatus = ERROR_NOT_SUPPORTED;
                break;
            }

            auto Status = pLdrAddRefDll(dwFlags & GET_MODULE_HANDLE_EX_FLAG_PIN, hModule);
            if (Status < 0)
            {
                lStatus = internal::BaseSetLastNTError(Status);
                break;
            }
        }

        *phModule = hModule;

        break;
    }

    if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
    {
        LeaveCriticalSection(((TEB *)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
    }

    if (lStatus)
    {
        SetLastError(lStatus);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetModuleHandleExW, 12);
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//��Ȼ���������ˣ�����ֻ��Windows 8�Լ�����KB2533623������ϵͳ��֧�� LOAD_LIBRARY_SEARCH_SYSTEM32
//������

EXTERN_C
_Ret_maybenull_ HMODULE WINAPI
LoadLibraryExW(_In_ LPCWSTR lpLibFileName, _Reserved_ HANDLE hFile, _In_ DWORD dwFlags)
#ifdef YY_Thunks_Defined
    ;
#else
{
    const auto pLoadLibraryExW = try_get_LoadLibraryExW();

    if (!pLoadLibraryExW)
    {
        SetLastError(ERROR_FUNCTION_FAILED);
        return nullptr;
    }

    if (try_get_AddDllDirectory() != nullptr)
    {
        //����AddDllDirectory˵��֧�� LOAD_LIBRARY_SEARCH_SYSTEM32
        //�ȹ��ܣ�ֱ�ӵ���pLoadLibraryExW���ɡ�

        return pLoadLibraryExW(lpLibFileName, hFile, dwFlags);
    }

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
    // Windows Vista��ʼ��֧�� LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
    // LOAD_LIBRARY_AS_IMAGE_RESOURCE�����ڲ�֧�ֵ�ϵͳ����ֻ��Fallblack�� LOAD_LIBRARY_AS_DATAFILE
    if (dwFlags & (LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE))
    {
        auto pPeb = ((TEB *)NtCurrentTeb())->ProcessEnvironmentBlock;

        if (pPeb->OSMajorVersion < 6)
        {
            dwFlags &= ~(LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
            dwFlags |= LOAD_LIBRARY_AS_DATAFILE;
        }
    }
#endif
    wchar_t szFilePathBuffer[1024];

    do
    {
        auto dwLoadLibrarySearchFlags = dwFlags & 0xFFFFFF00;

        if (dwLoadLibrarySearchFlags == 0)
        {
            break;
        }

        if (((LOAD_WITH_ALTERED_SEARCH_PATH | 0xFFFFE000 | 0x00000004) & dwFlags) ||
            lpLibFileName == nullptr || hFile)
        {
            // LOAD_WITH_ALTERED_SEARCH_PATH ��ǲ����������������ʹ��
            // 0xFFFFE000 Ϊ ������֧�ֵ���ֵ
            // LOAD_PACKAGED_LIBRARY: 0x00000004 Windows 8����ƽ̨��֧��
            SetLastError(ERROR_INVALID_PARAMETER);
            return nullptr;
        }

        dwFlags &= 0xFF;

        // LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS |
        // LOAD_LIBRARY_SEARCH_SYSTEM32 �ȼ��� LOAD_LIBRARY_SEARCH_DEFAULT_DIRS���
        if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)
            dwLoadLibrarySearchFlags =
                (dwLoadLibrarySearchFlags & ~LOAD_LIBRARY_SEARCH_DEFAULT_DIRS) |
                (LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS |
                 LOAD_LIBRARY_SEARCH_SYSTEM32);

        if (dwLoadLibrarySearchFlags ==
            (LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS |
             LOAD_LIBRARY_SEARCH_SYSTEM32))
        {
            //���ȷ���ǵ���Ĭ����ϵ����ֱ�ӵ���ԭʼ LoadLibraryExW

            break;
        }

        const auto pRtlDetermineDosPathNameType_U = try_get_RtlDetermineDosPathNameType_U();

        const auto PathType = pRtlDetermineDosPathNameType_U
                                  ? pRtlDetermineDosPathNameType_U(lpLibFileName)
                                  : RtlPathTypeUnknown;

        if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR)
        {
            //������һ������·����
            if (PathType == RtlPathTypeUnknown || PathType == RtlPathTypeDriveRelative ||
                PathType == RtlPathTypeRelative)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return nullptr;
            }

            if (dwLoadLibrarySearchFlags ==
                (LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_APPLICATION_DIR |
                 LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32))
            {
                // LOAD_WITH_ALTERED_SEARCH_PATH������ģ�� LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR |
                // LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS |
                // LOAD_LIBRARY_SEARCH_SYSTEM32 ���Ч����
                dwFlags |= LOAD_WITH_ALTERED_SEARCH_PATH;
                break;
            }
        }

        if (LOAD_LIBRARY_SEARCH_USER_DIRS & dwLoadLibrarySearchFlags)
        {
            // LOAD_LIBRARY_SEARCH_USER_DIRS �޷�˳��ʵ�֣�������Ч��������
            SetLastError(ERROR_INVALID_PARAMETER);
            return nullptr;
        }

        if (dwFlags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE |
                       LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE))
        {
            //����Դ��ʽ����

            //�ж�·����һ������·������һ�����·��������Ǿ���·������ô����ֱ������
            //LOAD_LIBRARY_SEARCH_ ϵ�в�����
            if ((PathType == RtlPathTypeUnknown || PathType == RtlPathTypeDriveRelative ||
                 PathType == RtlPathTypeRelative) == false)
            {
                //��һ������·��������ֱ�Ӵ��ݸ� pLoadLibraryExW ����

                break;
            }

            if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_APPLICATION_DIR)
            {
                auto nSize = GetModuleFileNameW(NULL, szFilePathBuffer, _countof(szFilePathBuffer));

                if (nSize == 0 || nSize >= _countof(szFilePathBuffer))
                {
                    SetLastError(ERROR_FUNCTION_FAILED);
                    return nullptr;
                }

                for (;;)
                {
                    if (szFilePathBuffer[nSize] == L'\\' || szFilePathBuffer[nSize] == L'/')
                    {
                        ++nSize;
                        break;
                    }

                    if (nSize == 0)
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }

                    --nSize;
                }

                for (auto Str = lpLibFileName; *Str; ++Str, ++nSize)
                {
                    if (nSize >= _countof(szFilePathBuffer))
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }

                    szFilePathBuffer[nSize] = *Str;
                }

                szFilePathBuffer[nSize] = L'\0';

                if (GetFileAttributesW(szFilePathBuffer) != -1)
                {
                    lpLibFileName = szFilePathBuffer;
                    break;
                }
            }

            if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_SYSTEM32)
            {
                auto nSize = GetSystemDirectoryW(szFilePathBuffer, _countof(szFilePathBuffer));

                if (nSize == 0 || nSize >= _countof(szFilePathBuffer))
                {
                    SetLastError(ERROR_FUNCTION_FAILED);
                    return nullptr;
                }

                if (szFilePathBuffer[nSize] != L'\\')
                {
                    if (nSize >= _countof(szFilePathBuffer))
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }

                    szFilePathBuffer[++nSize] = L'\\';
                }

                for (auto Str = lpLibFileName; *Str; ++Str, ++nSize)
                {
                    if (nSize >= _countof(szFilePathBuffer))
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }

                    szFilePathBuffer[nSize] = *Str;
                }

                szFilePathBuffer[nSize] = L'\0';

                if (GetFileAttributesW(szFilePathBuffer) != -1)
                {
                    lpLibFileName = szFilePathBuffer;
                    break;
                }
            }

            SetLastError(ERROR_MOD_NOT_FOUND);
            return nullptr;
        }

        //��ģ�鷽ʽ����

        const auto pLdrLoadDll = try_get_LdrLoadDll();
        if (!pLdrLoadDll)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return nullptr;
        }

        DWORD nSize = 0;

        if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR)
        {
            for (auto Str = lpLibFileName; *Str; ++Str, ++nSize)
            {
                if (nSize >= _countof(szFilePathBuffer))
                {
                    SetLastError(ERROR_FUNCTION_FAILED);
                    return nullptr;
                }

                szFilePathBuffer[nSize] = *Str;
            }

            if (nSize == 0)
            {
                SetLastError(ERROR_FUNCTION_FAILED);
                return nullptr;
            }

            --nSize;
            //�����޳��ļ���
            for (;;)
            {
                if (szFilePathBuffer[nSize] == L'\\' || szFilePathBuffer[nSize] == L'/')
                {
                    break;
                }

                if (nSize == 0)
                {
                    SetLastError(ERROR_FUNCTION_FAILED);
                    return nullptr;
                }

                --nSize;
            }

            ++nSize;
            szFilePathBuffer[nSize] = L';';
            ++nSize;
        }

        if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_APPLICATION_DIR)
        {
            const auto nBufferMax = _countof(szFilePathBuffer) - nSize;

            auto nBuffer = GetModuleFileNameW(NULL, szFilePathBuffer + nSize, nBufferMax);

            if (nBuffer == 0 || nBuffer >= nBufferMax)
            {
                SetLastError(ERROR_FUNCTION_FAILED);
                return nullptr;
            }

            nSize += nBuffer - 1;

            for (;;)
            {
                if (szFilePathBuffer[nSize] == L'\\' || szFilePathBuffer[nSize] == L'/')
                {
                    break;
                }

                if (nSize == 0)
                {
                    SetLastError(ERROR_FUNCTION_FAILED);
                    return nullptr;
                }

                --nSize;
            }

            ++nSize;
            szFilePathBuffer[nSize] = L';';
            ++nSize;
        }

        if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_SYSTEM32)
        {
            const auto nBufferMax = _countof(szFilePathBuffer) - nSize;

            auto nBuffer = GetSystemDirectoryW(szFilePathBuffer + nSize, nBufferMax);

            if (nBuffer == 0 || nBuffer >= nBufferMax)
            {
                SetLastError(ERROR_FUNCTION_FAILED);
                return nullptr;
            }

            nSize += nBuffer;
        }

        szFilePathBuffer[nSize] = L'\0';

        UNICODE_STRING ModuleFileName;
        ModuleFileName.Buffer = (PWSTR)lpLibFileName;

        for (; *lpLibFileName; ++lpLibFileName)
            ;

        ModuleFileName.Length = (lpLibFileName - ModuleFileName.Buffer) * sizeof(lpLibFileName[0]);
        ModuleFileName.MaximumLength = ModuleFileName.Length + sizeof(lpLibFileName[0]);

        HMODULE hModule = NULL;

        ULONG dwLdrLoadDllFlags = 0;

        if (dwFlags & DONT_RESOLVE_DLL_REFERENCES)
        {
            dwLdrLoadDllFlags |= 0x2;
        }

        if (dwFlags & LOAD_IGNORE_CODE_AUTHZ_LEVEL)
        {
            dwLdrLoadDllFlags |= 0x1000;
        }

        if (dwFlags & LOAD_LIBRARY_REQUIRE_SIGNED_TARGET)
        {
            dwLdrLoadDllFlags |= 0x800000;
        }

#if defined(_X86_) || defined(_M_IX86)
        //�����ȹر��ض����ټ���DLL��Windows 7
        //SP1��ǰ��ϵͳ����ر��ض��򣬶�����ĳЩ�̹߳ر��ض����DLL�������⡣
        PVOID OldFsRedirectionLevel;

        auto pRtlWow64EnableFsRedirectionEx = try_get_RtlWow64EnableFsRedirectionEx();
        auto StatusFsRedir = pRtlWow64EnableFsRedirectionEx
                                 ? pRtlWow64EnableFsRedirectionEx(nullptr, &OldFsRedirectionLevel)
                                 : 0;
#endif

        auto Status = pLdrLoadDll(szFilePathBuffer, &dwLdrLoadDllFlags, &ModuleFileName, &hModule);

#if defined(_X86_) || defined(_M_IX86)
        if (StatusFsRedir >= 0 && pRtlWow64EnableFsRedirectionEx)
            pRtlWow64EnableFsRedirectionEx(OldFsRedirectionLevel, &OldFsRedirectionLevel);
#endif
        if (Status < 0)
        {
            YY::Thunks::internal::BaseSetLastNTError(Status);
        }

        return hModule;
    } while (false);

#if defined(_X86_) || defined(_M_IX86)
    //�����ȹر��ض����ټ���DLL��Windows 7
    //SP1��ǰ��ϵͳ����ر��ض��򣬶�����ĳЩ�̹߳ر��ض����DLL�������⡣
    PVOID OldFsRedirectionLevel;

    auto pRtlWow64EnableFsRedirectionEx = try_get_RtlWow64EnableFsRedirectionEx();
    auto StatusFsRedir = pRtlWow64EnableFsRedirectionEx
                             ? pRtlWow64EnableFsRedirectionEx(nullptr, &OldFsRedirectionLevel)
                             : 0;
#endif

    auto hModule = pLoadLibraryExW(lpLibFileName, hFile, dwFlags);

#if defined(_X86_) || defined(_M_IX86)
    if (StatusFsRedir >= 0 && pRtlWow64EnableFsRedirectionEx)
    {
        LSTATUS lStatus = GetLastError();
        pRtlWow64EnableFsRedirectionEx(OldFsRedirectionLevel, &OldFsRedirectionLevel);
        SetLastError(lStatus);
    }
#endif
    return hModule;
}
#endif

__YY_Thunks_Expand_Function(kernel32, LoadLibraryExW, 12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//��Ȼ���������ˣ�����ֻ��Windows 8�Լ�����KB2533623������ϵͳ��֧�� LOAD_LIBRARY_SEARCH_SYSTEM32
//������

EXTERN_C
_Ret_maybenull_ HMODULE WINAPI
LoadLibraryExA(_In_ LPCSTR lpLibFileName, _Reserved_ HANDLE hFile, _In_ DWORD dwFlags)
#ifdef YY_Thunks_Defined
    ;
#else
{
    const auto pLoadLibraryExA = try_get_LoadLibraryExA();

    if (!pLoadLibraryExA)
    {
        SetLastError(ERROR_FUNCTION_FAILED);
        return nullptr;
    }

    if (try_get_AddDllDirectory() != nullptr)
    {
        //����AddDllDirectory˵��֧�� LOAD_LIBRARY_SEARCH_SYSTEM32
        //�ȹ��ܣ�ֱ�ӵ���pLoadLibraryExW���ɡ�

        return pLoadLibraryExA(lpLibFileName, hFile, dwFlags);
    }

    wchar_t szLibFileNameUnicode[512];

    UNICODE_STRING usLibFileName = {0, sizeof(szLibFileNameUnicode), szLibFileNameUnicode};

    auto lStatus = internal::Basep8BitStringToStaticUnicodeString(&usLibFileName, lpLibFileName);
    if (lStatus != ERROR_SUCCESS)
    {
        SetLastError(lStatus);
        return nullptr;
    }

    return LoadLibraryExW(szLibFileNameUnicode, hFile, dwFlags);
}
#endif

__YY_Thunks_Expand_Function(kernel32, LoadLibraryExA, 12);

#endif

} // namespace Thunks
} // namespace YY