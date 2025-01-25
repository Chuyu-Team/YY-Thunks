
#ifdef YY_Thunks_Implemented
namespace YY::Thunks::Fallback
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT5_1_SP1)
        static SRWLOCK s_DllDirectoryLock;
        static UNICODE_STRING s_sDllDirectory;

        static void __cdecl FreeDllDirectory() noexcept
        {
            internal::UnicodeStringFree(s_sDllDirectory);
        }
#endif // (YY_Thunks_Target < __WindowsNT5_1_SP1)

#if YY_Thunks_Target < __WindowsNT6_2
        static DWORD s_DirectoryFlags/* = 0*/;
        static SRWLOCK s_DllDirectoryListLock;
        static UNICODE_STRING s_szDllDirectoryList;

        static void __cdecl FreeDllDirectoryList() noexcept
        {
            internal::UnicodeStringFree(s_szDllDirectoryList);
        }
#endif

        /*LSTATUS __fastcall BasepGetModuleHandleExParameterValidation(
            _In_ DWORD dwFlags,
            _In_opt_ LPCSTR lpModuleName,
            _Out_ HMODULE* phModule
            )*/
#define BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule) \
            for (;;)                                                           \
            {                                                                  \
                if (phModule)                                                  \
                {                                                              \
                    *phModule = nullptr;                                       \
                    if ((dwFlags & ~(GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)) == 0                                         \
                        || (dwFlags & (GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)) != (GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)   \
                        || (lpModuleName || (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS) == 0))                                                                                                     \
                    {                                                         \
                        if (lpModuleName == nullptr)                          \
                        {                                                     \
                            *phModule = (HMODULE)((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ImageBaseAddress; \
                            return TRUE;                                      \
                        }                                                     \
                        break;                                                \
                    }                                                         \
                }                                                             \
                SetLastError(ERROR_INVALID_PARAMETER);                        \
                return FALSE;                                                 \
            }

#define __FORWARD_DLL_MODULE HMODULE(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ImageBaseAddress)

        template<typename Char>
        HMODULE __fastcall ForwardDll(_In_z_ const Char* _szLibFileName)
        {
#if defined(__ENABLE_WORKAROUND_1_GetProcAddress_ProcessPrng) && (YY_Thunks_Target < __WindowsNT6_1)
            if (_szLibFileName == nullptr || *_szLibFileName == L'\0')
                return nullptr;

            auto _szFileName = _szLibFileName;
            for (; *_szLibFileName; )
            {
                if (*_szLibFileName == Char('\\') || *_szLibFileName == Char('/'))
                {
                    ++_szLibFileName;
                    _szFileName = _szLibFileName;
                }
                else
                {
                    ++_szLibFileName;
                }
            }

#if defined(__ENABLE_WORKAROUND_1_GetProcAddress_ProcessPrng) && (YY_Thunks_Target < __WindowsNT6_1)
            if (internal::GetSystemVersion() < internal::MakeVersion(6, 1)
                && StringCompareIgnoreCaseByAscii(_szFileName, L"bcryptprimitives", 16) == 0)
            {
                _szFileName += 16;
                if (*_szFileName == L'\0' || StringCompareIgnoreCaseByAscii(_szFileName, ".dll", -1) == 0)
                {
                    // Windows 7以下平台没有这个DLL，用进程模块句柄伪装一下。
                    return __FORWARD_DLL_MODULE;
                }
            }
#endif
#endif 
            return nullptr;
        }

#if defined(__ENABLE_WORKAROUND_1_GetProcAddress_ProcessPrng) && YY_Thunks_Target < __WindowsNT6_2
        void* __fastcall try_get_GetProcAddress(const ProcInfo& _ProcInfo)
        {
            auto _hModule = _ProcInfo.pfnGetModule();
            if (!_hModule)
                return nullptr;

#if defined(__USING_NTDLL_LIB)
            void* _pProc = nullptr;
            ANSI_STRING _sFunctionName = YY::Thunks::internal::MakeNtString(_ProcInfo.szProcName);
            const LONG _Status = LdrGetProcedureAddress(_hModule, &_sFunctionName, 0, &_pProc);
            return _Status >= 0 ? _pProc : nullptr;
#else // !defined(__USING_NTDLL_LIB)

            // 防止遇到畸形的DLL，抓个异常
            __try
            {
                ULONG _uSize;
                auto _pExport = (_IMAGE_EXPORT_DIRECTORY*)YY_ImageDirectoryEntryToData(_hModule, IMAGE_DIRECTORY_ENTRY_EXPORT, &_uSize);
                if (!_pExport)
                    return nullptr;

                if (_pExport->AddressOfNames == 0 || _pExport->AddressOfFunctions == 0 || _pExport->AddressOfNameOrdinals == 0)
                    return nullptr;

                auto _puFunctions = (const DWORD*)(PBYTE(_hModule) + _pExport->AddressOfFunctions);
                auto _puNames = (const DWORD*)(PBYTE(_hModule) + _pExport->AddressOfNames);
                auto _puNameOrdinals = (const WORD*)(PBYTE(_hModule) + _pExport->AddressOfNameOrdinals);

                for (DWORD i = 0; i != _pExport->NumberOfNames; ++i)
                {
                    auto _uName = _puNames[i];
                    if (_uName == 0)
                        continue;

                    auto _szName = (char*)(PBYTE(_hModule) + _uName);

                    if (StringCompare(_szName, _ProcInfo.szProcName, -1) == 0)
                    {
                        auto _pfn = PBYTE(_hModule) + _puFunctions[_puNameOrdinals[i]];
                        return _pfn;
                    }
                }
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
            }
            return nullptr;
#endif // defined(__USING_NTDLL_LIB)
        }
#endif
    }
}
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_1)

    //Windows XP [desktop apps only]
    //Windows Server 2003 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    GetModuleHandleExA,
        _In_ DWORD dwFlags,
        _In_opt_ LPCSTR lpModuleName,
        _Out_ HMODULE* phModule
        )
    {
        if (const auto pGetModuleHandleExA = try_get_GetModuleHandleExA())
        {
            return pGetModuleHandleExA(dwFlags, lpModuleName, phModule);
        }

        BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule);

        if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
        {
            EnterCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
        }

        LSTATUS lStatus = ERROR_SUCCESS;

        for (;;)
        {
            HMODULE hModule;

            if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
            {
#if !defined(__USING_NTDLL_LIB)
                const auto RtlPcToFileHeader = try_get_RtlPcToFileHeader();
                if (!RtlPcToFileHeader)
                {
                    lStatus = ERROR_NOT_SUPPORTED;
                    break;
                }
#endif

                hModule = (HMODULE)RtlPcToFileHeader((PVOID)lpModuleName, (PVOID*)&hModule);
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
#if !defined(__USING_NTDLL_LIB)
                const auto LdrAddRefDll = try_get_LdrAddRefDll();
                if (!LdrAddRefDll)
                {
                    lStatus = ERROR_NOT_SUPPORTED;
                    break;
                }
#endif

                LONG Status = LdrAddRefDll(dwFlags& GET_MODULE_HANDLE_EX_FLAG_PIN, hModule);
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
            LeaveCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
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


#if (YY_Thunks_Target < __WindowsNT5_1)

    //Windows XP [desktop apps only]
    //Windows Server 2003 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    GetModuleHandleExW,
        _In_ DWORD dwFlags,
        _In_opt_ LPCWSTR lpModuleName,
        _Out_ HMODULE* phModule
        )
    {
        if (const auto pGetModuleHandleExW = try_get_GetModuleHandleExW())
        {
            return pGetModuleHandleExW(dwFlags, lpModuleName, phModule);
        }

        BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule);

        if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
        {
            EnterCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
        }

        LSTATUS lStatus = ERROR_SUCCESS;

        for (;;)
        {
            HMODULE hModule;

            if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
            {
#if !defined(__USING_NTDLL_LIB)
                const auto RtlPcToFileHeader = try_get_RtlPcToFileHeader();
                if (!RtlPcToFileHeader)
                {
                    lStatus = ERROR_NOT_SUPPORTED;
                    break;
                }
#endif

                hModule = (HMODULE)RtlPcToFileHeader((PVOID)lpModuleName, (PVOID*)&hModule);
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
#if !defined(__USING_NTDLL_LIB)
                const auto LdrAddRefDll = try_get_LdrAddRefDll();
                if (!LdrAddRefDll)
                {
                    lStatus = ERROR_NOT_SUPPORTED;
                    break;
                }
#endif

                LONG Status = LdrAddRefDll(dwFlags & GET_MODULE_HANDLE_EX_FLAG_PIN, hModule);
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
            LeaveCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
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


#if (YY_Thunks_Target < __WindowsNT6_2)

    //虽然这个早就有了，但是只有Windows 8以及打了KB2533623补丁的系统才支持 LOAD_LIBRARY_SEARCH_SYSTEM32 等特性
    __DEFINE_THUNK(
    kernel32,
    12,
    _Ret_maybenull_
    HMODULE,
    WINAPI,
    LoadLibraryExW,
        _In_ LPCWSTR lpLibFileName,
        _Reserved_ HANDLE hFile,
        _In_ DWORD dwFlags
        )
    {
        const auto _pfnLoadLibraryExW = try_get_LoadLibraryExW();
        if (!_pfnLoadLibraryExW)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return nullptr;
        }

        if (try_get_AddDllDirectory())
        {
            //存在AddDllDirectory说明支持 LOAD_LIBRARY_SEARCH_SYSTEM32 等功能，直接调用_pfnLoadLibraryExW即可。
            auto _hModule = _pfnLoadLibraryExW(lpLibFileName, hFile, dwFlags);
            if (_hModule)
                return _hModule;

            return Fallback::ForwardDll(lpLibFileName);
        }

        if (((LOAD_WITH_ALTERED_SEARCH_PATH | 0xFFFFE000 | 0x00000004) & dwFlags) || lpLibFileName == nullptr || hFile)
        {
            //LOAD_WITH_ALTERED_SEARCH_PATH 标记不允许跟其他标记组合使用
            //0xFFFFE000 为 其他不支持的数值
            //LOAD_PACKAGED_LIBRARY: 0x00000004 Windows 8以上平台才支持
            SetLastError(ERROR_INVALID_PARAMETER);
            return nullptr;
        }

#if (YY_Thunks_Target < __WindowsNT6)
        //Windows Vista开始才支持 LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE，对于不支持的系统我们只能Fallblack到 LOAD_LIBRARY_AS_DATAFILE
        if (dwFlags & (LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE))
        {
            auto pPeb = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock;

            if (pPeb->OSMajorVersion < 6)
            {
                dwFlags &= ~(LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
                dwFlags |= LOAD_LIBRARY_AS_DATAFILE;
            }
        }
#endif

        // 检测是否包含 LOAD_LIBRARY_SEARCH_* | LOAD_WITH_ALTERED_SEARCH_PATH
        //  * 已包含 LOAD_LIBRARY_SEARCH_* 说明用户自行指定了这个参数，因此无需附加 s_DirectoryFlags
        //  * LOAD_WITH_ALTERED_SEARCH_PATH与所有 LOAD_LIBRARY_SEARCH_* 互斥，因此无需附加 s_DirectoryFlags
        if ((dwFlags & (0xFFFFFF00ul | LOAD_WITH_ALTERED_SEARCH_PATH)) == 0)
        {
            dwFlags |= Fallback::s_DirectoryFlags;
        }

#if defined(_M_IX86) && YY_Thunks_Target < __WindowsNT6_1_SP1
        //我们先关闭重定向，再加载DLL，Windows 7 SP1以前的系统不会关闭重定向，而导致某些线程关闭重定向后DLL加载问题。
        class LoadDllEnalbeFsRedirection
        {
            PVOID OldFsRedirectionLevel = nullptr;
            decltype(RtlWow64EnableFsRedirectionEx)* pfnRtlWow64EnableFsRedirectionEx = nullptr;
            LONG Status = STATUS_NOINTERFACE;

        public:
            LoadDllEnalbeFsRedirection()
            {
                pfnRtlWow64EnableFsRedirectionEx = try_get_RtlWow64EnableFsRedirectionEx();
                if (pfnRtlWow64EnableFsRedirectionEx)
                {
                    Status = pfnRtlWow64EnableFsRedirectionEx(nullptr, &OldFsRedirectionLevel);
                }
            }

            ~LoadDllEnalbeFsRedirection()
            {
                if (Status >= 0 && pfnRtlWow64EnableFsRedirectionEx)
                {
                    pfnRtlWow64EnableFsRedirectionEx(OldFsRedirectionLevel, &OldFsRedirectionLevel);
                }
            }
        };

        LoadDllEnalbeFsRedirection _AutoEnalbeFsRedirection;
#endif

        internal::UnicodeStringBuffer<1024> szFilePathBuffer;

        do
        {
            const auto dwLoadLibrarySearchFlags = dwFlags;
            dwFlags &= 0xFF;

            // 未使用任何扩展能力，这时可直接回退到系统 LoadLibraryExW
            if (dwLoadLibrarySearchFlags == 0)
            {
#if (YY_Thunks_Target < __WindowsNT5_1_SP1)
                // 不支持 GetDllDirectoryW时后续任然需要模拟
                // 这是因为GetDllDirectoryW的路径在默认搜索列表中。
                // https://learn.microsoft.com/zh-cn/windows/win32/dlls/dynamic-link-library-search-order
                if (try_get_GetDllDirectoryW() || Fallback::s_sDllDirectory.Length == 0)
#endif
                {
                    break;
                }
            }

#if defined(__USING_NTDLL_LIB)
            const auto PathType = RtlDetermineDosPathNameType_U(lpLibFileName);
#else
            const auto RtlDetermineDosPathNameType_U = try_get_RtlDetermineDosPathNameType_U();
            const auto PathType = RtlDetermineDosPathNameType_U ? RtlDetermineDosPathNameType_U(lpLibFileName) : RtlPathTypeUnknown;
#endif

            if (dwLoadLibrarySearchFlags & (LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_WITH_ALTERED_SEARCH_PATH))
            {
                //必须是一个完整路径！
                if (PathType == RtlPathTypeUnknown || PathType == RtlPathTypeDriveRelative || PathType == RtlPathTypeRelative)
                {
                    SetLastError(ERROR_INVALID_PARAMETER);
                    return nullptr;
                }
            }

            if (dwLoadLibrarySearchFlags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE))
            {
                //以资源方式加载

                //判断路径是一个绝对路径还是一个相对路径，如果是绝对路径，那么可以直接无视 LOAD_LIBRARY_SEARCH_ 系列参数。
                if ((PathType == RtlPathTypeUnknown || PathType == RtlPathTypeDriveRelative || PathType == RtlPathTypeRelative) == false)
                {
                    // 是一个绝对路径，我们直接传递给 _pfnLoadLibraryExW 即可
                    break;
                }

                if (dwLoadLibrarySearchFlags & (LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
                {
                    const auto _cchBuffer = szFilePathBuffer.MaximumLength / 2;
                    auto nSize = GetModuleFileNameW(NULL, szFilePathBuffer.Buffer, _cchBuffer);

                    if (nSize == 0 || nSize >= _cchBuffer)
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }

                    const auto _sAppDir = internal::UnicodeStringGetDir(szFilePathBuffer.Buffer, nSize);
                    if(_sAppDir.Length == 0)
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }

                    szFilePathBuffer.SetByteLength(_sAppDir.Length);
                    if (szFilePathBuffer.AppendPath(lpLibFileName))
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }

                    auto _hModule = _pfnLoadLibraryExW(szFilePathBuffer.Buffer, hFile, dwFlags);
                    if (_hModule || GetLastError() != ERROR_FILE_NOT_FOUND)
                    {
                        return _hModule;
                    }
                }

                if (dwLoadLibrarySearchFlags & (LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
                {
                    LSTATUS _lStatus = ERROR_SUCCESS;
                    HMODULE _hModule = nullptr;

                    ::AcquireSRWLockShared(&Fallback::s_DllDirectoryListLock);
                    auto _szPathList = Fallback::s_szDllDirectoryList.Buffer;
                    const auto _szPathListEnd = LPWSTR(LPBYTE(Fallback::s_szDllDirectoryList.Buffer) + Fallback::s_szDllDirectoryList.Length);
                    for (;;)
                    {
                        auto _sItem = internal::UnicodeStringGetItem(_szPathList, _szPathListEnd);
                        if (_sItem.MaximumLength == 0)
                            break;

                        (LPBYTE&)_szPathList += _sItem.MaximumLength;
                        if (_sItem.Length == 0)
                            continue;

                        szFilePathBuffer.Empty();
                        if (szFilePathBuffer.Append(_sItem))
                        {
                            _lStatus = ERROR_NOT_ENOUGH_MEMORY;
                            break;
                        }
                        if (szFilePathBuffer.AppendPath(lpLibFileName))
                        {
                            _lStatus = ERROR_NOT_ENOUGH_MEMORY;
                            break;
                        }

                        _hModule = _pfnLoadLibraryExW(szFilePathBuffer.Buffer, hFile, dwFlags);
                        _lStatus = GetLastError();
                        if (_hModule || _lStatus != ERROR_FILE_NOT_FOUND)
                        {
                            break;
                        }
                    }
                    ::ReleaseSRWLockShared(&Fallback::s_DllDirectoryListLock);

                    if (_hModule)
                    {
                        return _hModule;
                    }
                    else if(_lStatus != ERROR_FILE_NOT_FOUND)
                    {
                        SetLastError(_lStatus);
                        return nullptr;
                    }

                    const auto _cchMaxBuffer = szFilePathBuffer.MaximumLength / sizeof(wchar_t);
                    const auto _cchBuffer = GetDllDirectoryW(_cchMaxBuffer, szFilePathBuffer.Buffer);
                    if (_cchBuffer && _cchBuffer < _cchMaxBuffer)
                    {
                        szFilePathBuffer.SetLength(_cchBuffer);
                        if (szFilePathBuffer.Append(lpLibFileName))
                        {
                            SetLastError(ERROR_OUTOFMEMORY);
                            return nullptr;
                        }

                        _hModule = _pfnLoadLibraryExW(szFilePathBuffer.Buffer, hFile, dwFlags);
                        if (_hModule || GetLastError() != ERROR_FILE_NOT_FOUND)
                        {
                            return _hModule;
                        }
                    }
                }

                if (dwLoadLibrarySearchFlags & (LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
                {
                    const auto _cchBuffer = szFilePathBuffer.MaximumLength / 2;
                    auto nSize = GetSystemDirectoryW(szFilePathBuffer.Buffer, _cchBuffer);

                    if (nSize == 0 || nSize >= _cchBuffer)
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }
                    szFilePathBuffer.SetLength(nSize);
                    if (szFilePathBuffer.AppendPath(lpLibFileName))
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }

                    auto _hModule = _pfnLoadLibraryExW(szFilePathBuffer.Buffer, hFile, dwFlags);
                    if (_hModule || GetLastError() != ERROR_FILE_NOT_FOUND)
                    {
                        return _hModule;
                    }
                }

                SetLastError(ERROR_MOD_NOT_FOUND);
                return nullptr;
            }

            //以模块方式加载
#if !defined(__USING_NTDLL_LIB)
            const auto LdrLoadDll = try_get_LdrLoadDll();
            if (!LdrLoadDll)
            {
                SetLastError(ERROR_FUNCTION_FAILED);
                return nullptr;
            }
#endif
            // DLL所在目录
            if (dwLoadLibrarySearchFlags & (LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_WITH_ALTERED_SEARCH_PATH))
            {
                const auto _sDllDir = internal::UnicodeStringGetDir(lpLibFileName, internal::StringLength(lpLibFileName));
                if (_sDllDir.Length == 0)
                {
                    SetLastError(ERROR_FUNCTION_FAILED);
                    return nullptr;
                }

                if (szFilePathBuffer.Append(_sDllDir))
                {
                    SetLastError(ERROR_OUTOFMEMORY);
                    return nullptr;
                }
                if (szFilePathBuffer.Append(L';'))
                {
                    SetLastError(ERROR_OUTOFMEMORY);
                    return nullptr;
                }
            }

            // 应用程序根目录
            if ((dwLoadLibrarySearchFlags & (LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
                || (dwLoadLibrarySearchFlags & 0xFFFFFF00ul) == 0)
            {
                auto _pBuffer = szFilePathBuffer.GetAppendBuffer(MAX_PATH);
                if (!_pBuffer)
                {
                    SetLastError(ERROR_OUTOFMEMORY);
                    return nullptr;
                }
                auto nBuffer = GetModuleFileNameW(NULL, _pBuffer, MAX_PATH);

                if (nBuffer == 0 || nBuffer >= MAX_PATH)
                {
                    SetLastError(ERROR_FUNCTION_FAILED);
                    return nullptr;
                }

                const auto _sAppDir = internal::UnicodeStringGetDir(_pBuffer, nBuffer);
                if (_sAppDir.Length == 0)
                {
                    SetLastError(ERROR_FUNCTION_FAILED);
                    return nullptr;
                }

                szFilePathBuffer.SetAppendByteLength(_sAppDir.Length);
                if (szFilePathBuffer.Append(L';'))
                {
                    SetLastError(ERROR_OUTOFMEMORY);
                    return nullptr;
                }
            }

            // AddDllDirectory
            if (dwLoadLibrarySearchFlags & (LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
            {
                ::AcquireSRWLockShared(&Fallback::s_DllDirectoryListLock);
                auto _lStatus = szFilePathBuffer.Append(Fallback::s_szDllDirectoryList);
                ::ReleaseSRWLockShared(&Fallback::s_DllDirectoryListLock);
                if(_lStatus)
                {
                    SetLastError(ERROR_OUTOFMEMORY);
                    return nullptr;
                }
            }

            // SetDllDirectoryW
            bool _bHasSetDllDirectory = false;
            if ((dwLoadLibrarySearchFlags & (LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
                || (dwLoadLibrarySearchFlags & 0xFFFFFF00ul) == 0)
            {
                auto _pBuffer = szFilePathBuffer.GetAppendBuffer(MAX_PATH);
                if (!_pBuffer)
                {
                    SetLastError(ERROR_OUTOFMEMORY);
                    return nullptr;
                }

                auto _cchBuffer = GetDllDirectoryW(MAX_PATH, _pBuffer);
                if (_cchBuffer && _cchBuffer < MAX_PATH)
                {
                    _bHasSetDllDirectory = true;
                    szFilePathBuffer.SetAppendLength(_cchBuffer);
                    if (szFilePathBuffer.Append(L';'))
                    {
                        SetLastError(ERROR_OUTOFMEMORY);
                        return nullptr;
                    }
                }
            }

            // System32
            if ((dwLoadLibrarySearchFlags & (LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
                || (dwLoadLibrarySearchFlags & 0xFFFFFF00ul) == 0)
            {
                auto _pBuffer = szFilePathBuffer.GetAppendBuffer(MAX_PATH);
                if (!_pBuffer)
                {
                    SetLastError(ERROR_OUTOFMEMORY);
                    return nullptr;
                }
                auto nBuffer = GetSystemDirectoryW(_pBuffer, MAX_PATH);

                if (nBuffer == 0 || nBuffer >= MAX_PATH)
                {
                    SetLastError(ERROR_FUNCTION_FAILED);
                    return nullptr;
                }
                szFilePathBuffer.SetAppendLength(nBuffer);
                if (szFilePathBuffer.Append(L';'))
                {
                    SetLastError(ERROR_OUTOFMEMORY);
                    return nullptr;
                }
            }

#if (YY_Thunks_Target < __WindowsNT5_1_SP1)
            if ((dwLoadLibrarySearchFlags & 0xFFFFFF00ul) == 0)
            {
                // 为了偷懒同时照顾到安全性，模拟时我们始终采用安全搜索。
                // 搜索顺序参考：https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
                // 16位系统文件夹 C:\\Windows\\System
                {
                    auto _pBuffer = szFilePathBuffer.GetAppendBuffer(MAX_PATH);
                    if (!_pBuffer)
                    {
                        SetLastError(ERROR_OUTOFMEMORY);
                        return nullptr;
                    }

                    auto nBuffer = GetWindowsDirectoryW(_pBuffer, MAX_PATH);
                    if (nBuffer == 0 || nBuffer >= MAX_PATH)
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }
                    szFilePathBuffer.SetAppendLength(nBuffer);
                    if (szFilePathBuffer.AppendPath(L"System"))
                    {
                        SetLastError(ERROR_OUTOFMEMORY);
                        return nullptr;
                    }
                    if (szFilePathBuffer.Append(L';'))
                    {
                        SetLastError(ERROR_OUTOFMEMORY);
                        return nullptr;
                    }
                }

                // Windows文件夹 C:\\Windows
                {
                    auto _pBuffer = szFilePathBuffer.GetAppendBuffer(MAX_PATH);
                    if (!_pBuffer)
                    {
                        SetLastError(ERROR_OUTOFMEMORY);
                        return nullptr;
                    }

                    auto nBuffer = GetWindowsDirectoryW(_pBuffer, MAX_PATH);
                    if (nBuffer == 0 || nBuffer >= MAX_PATH)
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }
                    szFilePathBuffer.SetAppendLength(nBuffer);
                    if (szFilePathBuffer.Append(L';'))
                    {
                        SetLastError(ERROR_OUTOFMEMORY);
                        return nullptr;
                    }
                }

                // 当前目录，注意设置 SetDllDirectory后，当前目录不生效。
                if (!_bHasSetDllDirectory)
                {
                    auto _pBuffer = szFilePathBuffer.GetAppendBuffer(MAX_PATH);
                    if (!_pBuffer)
                    {
                        SetLastError(ERROR_OUTOFMEMORY);
                        return nullptr;
                    }

                    auto nBuffer = GetCurrentDirectoryW(MAX_PATH, _pBuffer);
                    if (nBuffer == 0 || nBuffer >= MAX_PATH)
                    {
                        SetLastError(ERROR_FUNCTION_FAILED);
                        return nullptr;
                    }
                    szFilePathBuffer.SetAppendLength(nBuffer);
                    if (szFilePathBuffer.Append(L';'))
                    {
                        SetLastError(ERROR_OUTOFMEMORY);
                        return nullptr;
                    }
                }

                // %PATH%
                {
                    
                    auto _cchBuffer = ExpandEnvironmentStringsW(L"%PATH%", nullptr, 0);
                    for (; _cchBuffer;)
                    {
                        auto _pBuffer = szFilePathBuffer.GetAppendBuffer(_cchBuffer);
                        if (!_pBuffer)
                        {
                            SetLastError(ERROR_OUTOFMEMORY);
                            return nullptr;
                        }

                        const auto _cchResult = ExpandEnvironmentStringsW(L"%PATH%", _pBuffer, _cchBuffer);
                        if (_cchResult == 0)
                            break;

                        if (_cchResult <= _cchBuffer)
                        {
                            szFilePathBuffer.SetAppendLength(_cchResult);
                            if (szFilePathBuffer.Append(L';'))
                            {
                                SetLastError(ERROR_OUTOFMEMORY);
                                return nullptr;
                            }
                            break;
                        }

                        _cchBuffer = _cchResult;
                    }
                }
            }
#endif

            UNICODE_STRING ModuleFileName = internal::MakeNtString(lpLibFileName);
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

            LONG Status = LdrLoadDll(szFilePathBuffer.Buffer, &dwLdrLoadDllFlags, &ModuleFileName, &hModule);
            if (Status < 0)
            {
                YY::Thunks::internal::BaseSetLastNTError(Status);
            }

            if (hModule)
                return hModule;

            return Fallback::ForwardDll(lpLibFileName);
        } while (false);

        auto hModule = _pfnLoadLibraryExW(lpLibFileName, hFile, dwFlags);
        if(hModule)
            return hModule;

        return Fallback::ForwardDll(lpLibFileName);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //虽然这个早就有了，但是只有Windows 8以及打了KB2533623补丁的系统才支持 LOAD_LIBRARY_SEARCH_SYSTEM32 等特性
    __DEFINE_THUNK(
    kernel32,
    12,
    _Ret_maybenull_
    HMODULE,
    WINAPI,
    LoadLibraryExA,
        _In_ LPCSTR lpLibFileName,
        _Reserved_ HANDLE hFile,
        _In_ DWORD dwFlags
        )
    {
        const auto pLoadLibraryExA = try_get_LoadLibraryExA();

        if (!pLoadLibraryExA)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return nullptr;
        }

        if (try_get_AddDllDirectory())
        {
            //存在AddDllDirectory说明支持 LOAD_LIBRARY_SEARCH_SYSTEM32 等功能，直接调用pLoadLibraryExW即可。

            auto _hModule = pLoadLibraryExA(lpLibFileName, hFile, dwFlags);
            if (_hModule)
                return _hModule;

            return Fallback::ForwardDll(lpLibFileName);
        }

        wchar_t szLibFileNameUnicode[512];

        UNICODE_STRING usLibFileName = { 0, sizeof(szLibFileNameUnicode), szLibFileNameUnicode };

        auto lStatus = internal::Basep8BitStringToStaticUnicodeString(&usLibFileName, lpLibFileName);
        if (lStatus != ERROR_SUCCESS)
        {
            SetLastError(lStatus);
            return nullptr;
        }

        return LoadLibraryExW(szLibFileNameUnicode, hFile, dwFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Windows Vista [仅限桌面应用], Windows Server 2008 [仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    28,
    BOOL,
    APIENTRY,
    EnumResourceLanguagesExW,
        _In_opt_ HMODULE _hModule,
        _In_ LPCWSTR _lpType,
        _In_ LPCWSTR _lpName,
        _In_ ENUMRESLANGPROCW _lpEnumFunc,
        _In_opt_ LONG_PTR _lParam,
        DWORD _dwFlags,
        LANGID _LangId
        )
    {
        const auto _pfnEnumResourceLanguagesExW = try_get_EnumResourceLanguagesExW();

        if (_pfnEnumResourceLanguagesExW)
        {
            return _pfnEnumResourceLanguagesExW(_hModule, _lpType, _lpName, _lpEnumFunc, _lParam, _dwFlags, _LangId);
        }

        // WinXP不支持MUI，故而忽略 _dwFlags、_LangId
        return EnumResourceLanguagesW(_hModule, _lpType, _lpName, _lpEnumFunc, _lParam);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Windows Vista [仅限桌面应用], Windows Server 2008 [仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    28,
    BOOL,
    APIENTRY,
    EnumResourceLanguagesExA,
        _In_opt_ HMODULE _hModule,
        _In_ LPCSTR _lpType,
        _In_ LPCSTR _lpName,
        _In_ ENUMRESLANGPROCA _lpEnumFunc,
        _In_opt_ LONG_PTR _lParam,
        DWORD _dwFlags,
        LANGID _LangId
        )
    {
        const auto _pfnEnumResourceLanguagesExA = try_get_EnumResourceLanguagesExA();

        if (_pfnEnumResourceLanguagesExA)
        {
            return _pfnEnumResourceLanguagesExA(_hModule, _lpType, _lpName, _lpEnumFunc, _lParam, _dwFlags, _LangId);
        }

        // WinXP不支持MUI，故而忽略 _dwFlags、_LangId
        return EnumResourceLanguagesA(_hModule, _lpType, _lpName, _lpEnumFunc, _lParam);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    24,
    int,
    WINAPI,
    FindStringOrdinal,
        _In_ DWORD _uFindStringOrdinalFlags,
        _In_reads_(_cchSource) LPCWSTR _pStringSource,
        _In_ int _cchSource,
        _In_reads_(_cchValue) LPCWSTR _pStringValue,
        _In_ int _cchValue,
        _In_ BOOL _bIgnoreCase
        )
    {
        if (const auto _pfnFindStringOrdinal = try_get_FindStringOrdinal())
        {
            return _pfnFindStringOrdinal(_uFindStringOrdinalFlags, _pStringSource, _cchSource, _pStringValue, _cchValue, _bIgnoreCase);
        }

        SetLastError(ERROR_SUCCESS);
        if (_pStringSource == nullptr || _cchSource < -1 || _pStringValue == nullptr || _cchValue < -1)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return -1;
        }

        if (_cchSource == -1)
        {
            _cchSource = (int)wcslen(_pStringSource);
        }

        if (_cchSource == 0)
        {
            return -1;
        }

        if (_cchValue == -1)
        {
            _cchValue = (int)wcslen(_pStringValue);
        }

        if (_cchValue == 0 || _cchValue > _cchSource)
        {
            return -1;
        }

        switch (_uFindStringOrdinalFlags)
        {
        case 0:
        case FIND_FROMSTART:
            for (auto _pStart = _pStringSource; _cchValue <= _cchSource;++_pStart, --_cchSource)
            {
                if (CompareStringOrdinal(_pStart, _cchValue, _pStringValue, _cchValue, _bIgnoreCase) == CSTR_EQUAL)
                {
                    return static_cast<int>(_pStart - _pStringSource);
                }
            }
            return -1;
            break;
        case FIND_FROMEND:
            for (auto _pStart = _pStringSource + _cchSource - _cchValue; _cchValue <= _cchSource; --_pStart, --_cchSource)
            {
                if (CompareStringOrdinal(_pStart, _cchValue, _pStringValue, _cchValue, _bIgnoreCase) == CSTR_EQUAL)
                {
                    return static_cast<int>(_pStart - _pStringSource);
                }
            }
            return -1;
            break;
        case FIND_STARTSWITH:
            if (CompareStringOrdinal(_pStringSource, _cchValue, _pStringValue, _cchValue, _bIgnoreCase) == CSTR_EQUAL)
            {
                return 0;
            }
            return -1;
            break;
        case FIND_ENDSWITH:
            _cchSource -= _cchValue;
            if (CompareStringOrdinal(_pStringSource + _cchSource, _cchValue, _pStringValue, _cchValue, _bIgnoreCase) == CSTR_EQUAL)
            {
                return _cchSource;
            }
            return -1;
            break;
        default:
            SetLastError(ERROR_INVALID_FLAGS);
            return -1;
            break;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    DisableThreadLibraryCalls,
        _In_ HMODULE _hLibModule
        )
    {
        // 虽然XP已经支持这个函数，但是Windows XP动态加载的DLL依靠的是DllMainCRTStartupForYY_Thunks通知做的。
        // 强制依赖于ThreadLibraryCalls，我们不能正真的禁用它。
        if (internal::g_TlsMode != internal::TlsMode::ByDllMainCRTStartupForYY_Thunks)
        {
            if (const auto _pfnDisableThreadLibraryCalls = try_get_DisableThreadLibraryCalls())
            {
                return _pfnDisableThreadLibraryCalls(_hLibModule);
            }
        }

        return TRUE;
    }
#endif


#if defined(__ENABLE_WORKAROUND_1_GetProcAddress_ProcessPrng) && YY_Thunks_Target < __WindowsNT6_2

    // 所有系统都支持，但是这个函数的存在是为了能顺利取到 ProcessPrng，Windows 8开始原生支持ProcessPrng
    __DEFINE_THUNK(
    kernel32,
    8,
    FARPROC,
    WINAPI,
    GetProcAddress,
        _In_ HMODULE hModule,
        _In_ LPCSTR lpProcName
        )
    {
#if defined(__ENABLE_WORKAROUND_1_GetProcAddress_ProcessPrng) && YY_Thunks_Target < __WindowsNT6_2
        if (uintptr_t(lpProcName) > UINT16_MAX)
        {
            if (StringCompare(lpProcName, "ProcessPrng", -1) == 0)
            {
                if (hModule == __FORWARD_DLL_MODULE || hModule == try_get_module_bcryptprimitives())
                {
                    const auto _pfn = try_get_ProcessPrng();
                    return reinterpret_cast<FARPROC>(_pfn ? _pfn : &ProcessPrng);
                }
            }
        }
#endif
        const auto _pfnGetProcAddress = try_get_GetProcAddress();
        if (!_pfnGetProcAddress)
        {
            SetLastError(ERROR_PROC_NOT_FOUND);
            return nullptr;
        }

        return _pfnGetProcAddress(hModule, lpProcName);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 所有系统都支持，但是现在为了支持 SetDefaultDllDirectories就Thunk下
    __DEFINE_THUNK(
    kernel32,
    4,
    HMODULE,
    WINAPI,
    LoadLibraryW,
        _In_ LPCWSTR _szLibFileName
        )
    {
        if (try_get_AddDllDirectory())
        {
            const auto _pfnLoadLibraryW = try_get_LoadLibraryW();
            if (!_pfnLoadLibraryW)
            {
                SetLastError(ERROR_PROC_NOT_FOUND);
                return nullptr;
            }

            auto _hModule = _pfnLoadLibraryW(_szLibFileName);
            if (_hModule)
                return _hModule;

            return Fallback::ForwardDll(_szLibFileName);
        }
        else
        {
            return LoadLibraryExW(_szLibFileName, NULL, 0);
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 所有系统都支持，但是现在为了支持 SetDefaultDllDirectories
    __DEFINE_THUNK(
    kernel32,
    4,
    HMODULE,
    WINAPI,
    LoadLibraryA,
        _In_ LPCSTR _szLibFileName
        )
    {
        if (try_get_AddDllDirectory())
        {
            const auto _pfnLoadLibraryA = try_get_LoadLibraryA();
            if (!_pfnLoadLibraryA)
            {
                SetLastError(ERROR_PROC_NOT_FOUND);
                return nullptr;
            }

            auto _hModule = _pfnLoadLibraryA(_szLibFileName);
            if (_hModule)
                return _hModule;

            return Fallback::ForwardDll(_szLibFileName);
        }
        else
        {
            wchar_t szLibFileNameUnicode[512];

            UNICODE_STRING usLibFileName = { 0, sizeof(szLibFileNameUnicode), szLibFileNameUnicode };

            auto lStatus = internal::Basep8BitStringToStaticUnicodeString(&usLibFileName, _szLibFileName);
            if (lStatus != ERROR_SUCCESS)
            {
                SetLastError(lStatus);
                return nullptr;
            }

            return LoadLibraryExW(szLibFileNameUnicode, NULL, 0);
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]，在 Windows 7、Windows Server 2008 R2、Windows Vista 和 Windows Server 2008 上KB2533623
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    SetDefaultDllDirectories,
        _In_ DWORD _fDirectoryFlags
        )
    {
        if (const auto _pfnSetDefaultDllDirectories = try_get_SetDefaultDllDirectories())
        {
            return _pfnSetDefaultDllDirectories(_fDirectoryFlags);
        }

        if (_fDirectoryFlags & ~(LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        Fallback::s_DirectoryFlags = _fDirectoryFlags;
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [仅限桌面应用]，在 Windows 7、Windows Server 2008 R2、Windows Vista 和 Windows Server 2008 上KB2533623
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    4,
    DLL_DIRECTORY_COOKIE,
    WINAPI,
    AddDllDirectory,
        _In_ PCWSTR _szNewDirectory
        )
    {
        if (const auto _pfnAddDllDirectory = try_get_AddDllDirectory())
        {
            return _pfnAddDllDirectory(_szNewDirectory);
        }

        __declspec(allocate(".YYThr$AAB")) static void* s_FreeDllDirectoryListData = reinterpret_cast<void*>(&Fallback::FreeDllDirectoryList);

        LSTATUS _lStatus = ERROR_SUCCESS;
        do
        {
#if !defined(__USING_NTDLL_LIB)
            const auto RtlDetermineDosPathNameType_U = try_get_RtlDetermineDosPathNameType_U();
            if (!RtlDetermineDosPathNameType_U)
            {
                _lStatus = ERROR_FUNCTION_FAILED;
                break;
            }
#endif
            const auto _ePathType = RtlDetermineDosPathNameType_U(_szNewDirectory);
            if (_ePathType == RtlPathTypeUnknown || _ePathType == RtlPathTypeDriveRelative || _ePathType == RtlPathTypeRelative)
            {
                _lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            ::AcquireSRWLockExclusive(&Fallback::s_DllDirectoryListLock);
            const auto _cbLengthBackup = Fallback::s_szDllDirectoryList.Length;
            _lStatus = internal::UnicodeStringAppend(Fallback::s_szDllDirectoryList, _szNewDirectory, internal::StringLength(_szNewDirectory));
            if (_lStatus == ERROR_SUCCESS)
            {
                _lStatus = internal::UnicodeStringAppend(Fallback::s_szDllDirectoryList, L';');
            }

            if (_lStatus)
            {
                Fallback::s_szDllDirectoryList.Length = _cbLengthBackup;
            }
            ::ReleaseSRWLockExclusive(&Fallback::s_DllDirectoryListLock);
        } while (false);

        if (_lStatus == ERROR_SUCCESS)
        {
            return (DLL_DIRECTORY_COOKIE)1;
        }
        else
        {
            SetLastError(_lStatus);
            return nullptr;
        }
    }
#endif

#if (YY_Thunks_Target < __WindowsNT5_1_SP1)

    // Minimum supported client	Windows Vista, Windows XP with SP1 [desktop apps only]
    // Minimum supported server	Windows Server 2003[desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    8,
    DWORD,
    WINAPI,
    GetDllDirectoryW,
        _In_ DWORD _cchBufferLength,
        _Out_writes_to_opt_(_cchBufferLength, return + 1) LPWSTR _szBuffer
        )
    {
        if (const auto _pfnGetDllDirectoryW = try_get_GetDllDirectoryW())
        {
            return _pfnGetDllDirectoryW(_cchBufferLength, _szBuffer);
        }

        if (_szBuffer && _cchBufferLength)
        {
            _szBuffer[0] = L'\0';
        }

        LSTATUS _lStatus = ERROR_SUCCESS;
        DWORD _uResult = 0;

        const auto _cbBufferLength = _cchBufferLength * sizeof(wchar_t);
        ::AcquireSRWLockShared(&Fallback::s_DllDirectoryLock);
        const auto _cbResult = Fallback::s_sDllDirectory.Length + sizeof(wchar_t);
        if (_szBuffer && _cbResult < _cbBufferLength)
        {
            memcpy(_szBuffer, Fallback::s_sDllDirectory.Buffer, Fallback::s_sDllDirectory.Length);
            _uResult = Fallback::s_sDllDirectory.Length / sizeof(wchar_t);
            _szBuffer[_uResult] = L'\0';
        }
        else
        {
            _uResult = _cbResult / sizeof(wchar_t);
        }
        ::ReleaseSRWLockShared(&Fallback::s_DllDirectoryLock);

        SetLastError(_lStatus);
        return _uResult;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1_SP1)

    // Minimum supported client	Windows Vista, Windows XP with SP1 [desktop apps only]
    // Minimum supported server	Windows Server 2003[desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    SetDllDirectoryW,
        _In_opt_ LPCWSTR _szPathName
        )
    {
        if (const auto _pfnSetDllDirectoryW = try_get_SetDllDirectoryW())
        {
            return _pfnSetDllDirectoryW(_szPathName);
        }

        __declspec(allocate(".YYThr$AAB")) static void* s_FreeDllDirectoryData = reinterpret_cast<void*>(&Fallback::FreeDllDirectory);

        const auto _cchPathName =  internal::StringLength(_szPathName);

        LSTATUS _lStatus = ERROR_SUCCESS;
        ::AcquireSRWLockExclusive(&Fallback::s_DllDirectoryLock);
        Fallback::s_sDllDirectory.Length = 0;
        if (_cchPathName)
        {
            _lStatus = internal::UnicodeStringAppend(Fallback::s_sDllDirectory, _cchPathName);
        }
        ::ReleaseSRWLockExclusive(&Fallback::s_DllDirectoryLock);

        if (_lStatus)
        {
            SetLastError(_lStatus);
            return FALSE;
        }

        return TRUE;
    }
#endif
}
