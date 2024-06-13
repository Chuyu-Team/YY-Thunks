#if (YY_Thunks_Support_Version < NTDDI_WIN7)
#include <shellapi.h>
#include <Shlobj.h>
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6) && !defined(__Comment_Lib_shell32)
#define __Comment_Lib_shell32
#pragma comment(lib, "Shell32.lib")
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6) && !defined(__Comment_Lib_ole32)
#define __Comment_Lib_ole32
#pragma comment(lib, "Ole32.lib")
#endif

namespace YY::Thunks
{
#ifdef YY_Thunks_Implemented
    namespace internal
    {
        namespace
        {
#if (YY_Thunks_Support_Version < NTDDI_WIN6)
            struct KnownFoldersIdKey
            {
                const GUID& rfid;
                const int csidl;
            };

            static _Success_(return != -1) _Check_return_ int __fastcall KnownFolderFlagsToCsidlFlags(_In_ DWORD _uKnownFolderFlags)
            {
                // 参数验证
                // 1. KF_FLAG_NOT_PARENT_RELATIVE必须跟KF_FLAG_DEFAULT_PATH同时使用
                if (((KF_FLAG_NOT_PARENT_RELATIVE | KF_FLAG_DEFAULT_PATH) & _uKnownFolderFlags) == KF_FLAG_NOT_PARENT_RELATIVE)
                {
                    return -1;
                }

                static_assert(KF_FLAG_CREATE == CSIDL_FLAG_CREATE, "");
                static_assert(KF_FLAG_DONT_VERIFY == CSIDL_FLAG_DONT_VERIFY, "");
                static_assert(KF_FLAG_DONT_UNEXPAND == CSIDL_FLAG_DONT_UNEXPAND, "");
                static_assert(KF_FLAG_NO_ALIAS == CSIDL_FLAG_NO_ALIAS, "");
                static_assert(KF_FLAG_INIT == CSIDL_FLAG_PER_USER_INIT, "");

                constexpr auto kWindows2KSupportCsidlFlags = CSIDL_FLAG_CREATE | CSIDL_FLAG_DONT_VERIFY | CSIDL_FLAG_DONT_UNEXPAND;
                constexpr auto kWindowsXPSupportCsidlFlags = kWindows2KSupportCsidlFlags | CSIDL_FLAG_NO_ALIAS | CSIDL_FLAG_PER_USER_INIT;

#if (YY_Thunks_Support_Version < NTDDI_WINXP)
                if (internal::GetSystemVersion() < internal::MakeVersion(5, 1))
                {
                    return static_cast<int>(_uKnownFolderFlags) & kWindows2KSupportCsidlFlags;
                }
                else
#endif
                {
                    return static_cast<int>(_uKnownFolderFlags) & kWindowsXPSupportCsidlFlags;
                }
            }

            //Vista以后的Shell32.dll 有个 kfapi::GetFolderIdByCSIDL，我们可以根据它可以反推出出 下面的KnownFoldersIdToCSIDL
            static _Success_(return != -1) _Check_return_ int __fastcall KnownFoldersIdToCSIDL(const GUID& rfid)
            {

                constexpr static const KnownFoldersIdKey KnownFoldersIdsMap[] =
                {
                    { FOLDERID_Windows, CSIDL_WINDOWS },
                    { FOLDERID_ProgramFilesCommon, CSIDL_PROGRAM_FILES_COMMON },
                    { FOLDERID_PublicDesktop, CSIDL_COMMON_DESKTOPDIRECTORY },
                    { FOLDERID_CDBurning, CSIDL_CDBURN_AREA },
                    { FOLDERID_CommonStartMenu, CSIDL_COMMON_STARTMENU },
                    { FOLDERID_Videos, CSIDL_MYVIDEO },
                    { FOLDERID_ProgramFilesCommonX86, CSIDL_PROGRAM_FILES_COMMONX86 },
                    { FOLDERID_ConnectionsFolder, CSIDL_CONNECTIONS },
                    { FOLDERID_PrintersFolder, CSIDL_PRINTERS },
                    { FOLDERID_Pictures, CSIDL_MYPICTURES },
                    { FOLDERID_ResourceDir, CSIDL_RESOURCES },
                    { FOLDERID_CommonStartup, CSIDL_COMMON_STARTUP /*等价CSIDL_COMMON_ALTSTARTUP*/},
                    { FOLDERID_PublicVideos, CSIDL_COMMON_VIDEO },
                    { FOLDERID_Desktop, CSIDL_DESKTOP /*等价CSIDL_DESKTOPDIRECTORY*/},
                    { FOLDERID_History, CSIDL_HISTORY },
                    { FOLDERID_SamplePictures, CSIDL_COMMON_PICTURES }, //做个兼容处理吧，反正都是放图片的
                    { FOLDERID_RecycleBinFolder, CSIDL_BITBUCKET },
                    { FOLDERID_CommonPrograms, CSIDL_COMMON_PROGRAMS },
                    { FOLDERID_NetHood, CSIDL_NETHOOD },
                    { FOLDERID_Cookies, CSIDL_COOKIES },
                    { FOLDERID_LocalizedResourcesDir, CSIDL_RESOURCES_LOCALIZED },
                    { FOLDERID_Favorites, CSIDL_FAVORITES /*等价于CSIDL_COMMON_FAVORITES*/},
                    { FOLDERID_SampleMusic, CSIDL_COMMON_MUSIC }, //做个兼容处理吧，反正都是放音乐的
                    { FOLDERID_SendTo, CSIDL_SENDTO },
                    { FOLDERID_AdminTools, CSIDL_ADMINTOOLS },
                    { FOLDERID_Music, CSIDL_MYMUSIC },
                    { FOLDERID_InternetFolder, CSIDL_INTERNET },
                    { FOLDERID_System, CSIDL_SYSTEM },
                    { FOLDERID_Programs, CSIDL_PROGRAMS },
#ifdef _AMD64_
                    { FOLDERID_ProgramFilesX64, CSIDL_PROGRAM_FILES }, //兼容下，反正 x64路径 跟 普通的是一样的。
#endif
                    { FOLDERID_ComputerFolder, CSIDL_DRIVES },
                    { FOLDERID_CommonAdminTools, CSIDL_COMMON_ADMINTOOLS },
                    { FOLDERID_Recent, CSIDL_RECENT },
                    { FOLDERID_ProgramData, CSIDL_COMMON_APPDATA },
                    { FOLDERID_LocalAppData, CSIDL_LOCAL_APPDATA },
                    { FOLDERID_PublicPictures, CSIDL_COMMON_PICTURES },
                    { FOLDERID_PrintHood, CSIDL_PRINTHOOD },
                    { FOLDERID_Profile, CSIDL_PROFILE },
                    { FOLDERID_SampleVideos, CSIDL_COMMON_VIDEO }, //做个兼容处理吧，反正都是放视频的
                    { FOLDERID_LocalAppDataLow, CSIDL_LOCAL_APPDATA }, //兼容下，直接用AppData
#ifdef _AMD64_
                    { FOLDERID_ProgramFilesCommonX64, CSIDL_PROGRAM_FILES_COMMON }, //兼容下，反正 x64路径 跟 普通的是一样的。
#endif
                    { FOLDERID_PublicDocuments, CSIDL_COMMON_DOCUMENTS },
                    { FOLDERID_SystemX86, CSIDL_SYSTEMX86 },
                    { FOLDERID_PublicMusic, CSIDL_COMMON_MUSIC },
                    { FOLDERID_ProgramFiles, CSIDL_PROGRAM_FILES },
                    { FOLDERID_Fonts, CSIDL_FONTS },
                    { FOLDERID_Startup, CSIDL_STARTUP /*等效CSIDL_ALTSTARTUP*/},
                    { FOLDERID_StartMenu, CSIDL_STARTMENU },
                    { FOLDERID_NetworkFolder, CSIDL_NETWORK /*等效CSIDL_COMPUTERSNEARME*/},
                    { FOLDERID_Documents, CSIDL_MYDOCUMENTS },
                    { FOLDERID_CommonOEMLinks, CSIDL_COMMON_OEM_LINKS },
                    { FOLDERID_RoamingAppData, CSIDL_APPDATA },
                    { FOLDERID_CommonTemplates, CSIDL_COMMON_TEMPLATES },
                    { FOLDERID_InternetCache, CSIDL_INTERNET_CACHE },
                    { FOLDERID_Templates, CSIDL_TEMPLATES },
                    { FOLDERID_ControlPanelFolder, CSIDL_CONTROLS },
                    { FOLDERID_ProgramFilesX86, CSIDL_PROGRAM_FILESX86 },

                    //{ FOLDERID_SyncManagerFolder, CSIDL_PRINTERS },
                    //{ FOLDERID_SyncSetupFolder, CSIDL_PRINTERS },
                    //{ FOLDERID_ConflictFolder, CSIDL_PRINTERS },
                    //{ FOLDERID_SyncResultsFolder, CSIDL_PRINTERS },
                    //{ FOLDERID_UserProgramFiles, CSIDL_PROGRAM_FILES_COMMON },
                    //{ FOLDERID_UserProgramFilesCommon, CSIDL_PROGRAM_FILES_COMMON },
                    //{ FOLDERID_Ringtones, CSIDL_MYVIDEO },
                    //{ FOLDERID_PublicRingtones, CSIDL_COMMON_VIDEO },
                    //{ FOLDERID_UserProfiles, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_Playlists, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_SamplePlaylists, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_PhotoAlbums, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_Public, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_ChangeRemovePrograms, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_AppUpdates, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_AddNewPrograms, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_Downloads, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_PublicDownloads, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_SavedSearches, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_QuickLaunch, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_Contacts, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_SidebarParts, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_SidebarDefaultParts, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_PublicGameTasks, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_GameTasks, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_SavedGames, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_Games, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_SEARCH_MAPI, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_SEARCH_CSC, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_Links, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_UsersFiles, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_UsersLibraries, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_SearchHome, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_OriginalImages, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_DocumentsLibrary, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_MusicLibrary, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_PicturesLibrary, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_VideosLibrary, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_RecordedTVLibrary, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_HomeGroup, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_HomeGroupCurrentUser, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_DeviceMetadataStore, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_Libraries, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_PublicLibraries, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_UserPinned, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_ImplicitAppShortcuts, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_AccountPictures, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_PublicUserTiles, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_AppsFolder, CSIDL_CDBURN_AREA },
                    //{ FOLDERID_StartMenuAllPrograms, CSIDL_CDBURN_AREA },
                };


                int bottom = 0;
                int top = _countof(KnownFoldersIdsMap) - 1;

                while (bottom <= top)
                {
                    int middle = (bottom + top) / 2;

                    auto& KnownFoldersId = KnownFoldersIdsMap[middle];

                    int testIndex = memcmp(&rfid, &KnownFoldersId.rfid, sizeof(rfid));

                    if (testIndex == 0)
                    {
                        return KnownFoldersId.csidl;
                    }
                    if (testIndex < 0)
                        top = middle - 1;
                    else
                        bottom = middle + 1;
                }

                return -1;
            }

            static __forceinline PIDLIST_RELATIVE __fastcall ILCloneParent(LPCITEMIDLIST pidl)
            {
                auto pClone = ILClone(pidl);
                if (pClone)
                    ILRemoveLastID(pClone);

                return pClone;
            }

            static __forceinline HRESULT __fastcall SHILCloneParent(PCUIDLIST_RELATIVE pidl, PUIDLIST_RELATIVE* pidl_new)
            {
                auto Clone = ILCloneParent(pidl);
                *pidl_new = Clone;
                return Clone ? S_OK : E_OUTOFMEMORY;
            }

            static
                HRESULT
                STDAPICALLTYPE
                SHBindToObject(
                    _In_opt_ IShellFolder* psf,
                    _In_ PCUIDLIST_RELATIVE pidl,
                    _In_opt_ IBindCtx* pbc,
                    _In_ REFIID riid,
                    _Outptr_ void** ppv
                )
            {
                *ppv = nullptr;

                IShellFolder* ppshf;

                auto hr = psf ? psf->QueryInterface(__uuidof(IShellFolder), (void**)&ppshf) : SHGetDesktopFolder(&ppshf);

                if (SUCCEEDED(hr))
                {
                    if (ILIsEmpty(pidl))
                    {
                        hr = ppshf->QueryInterface(riid, ppv);
                    }
                    else
                    {
                        hr = ppshf->BindToObject(pidl, pbc, riid, ppv);
                    }

                    if (SUCCEEDED(hr) && *ppv == nullptr)
                        hr = E_FAIL;

                    ppshf->Release();
                }


                return hr;
            }

            static
                HRESULT
                STDAPICALLTYPE
                SHBindToFolderIDListParentEx(
                    _In_opt_ IShellFolder* psfRoot,
                    _In_ PCUIDLIST_RELATIVE pidl,
                    _In_opt_ IBindCtx* ppbc,
                    _In_ REFIID riid,
                    _Outptr_ void** ppv,
                    _Outptr_opt_ PCUITEMID_CHILD* ppidlLast
                )
            {
                *ppv = nullptr;
                if (ppidlLast)
                    *ppidlLast = nullptr;

                HRESULT hr;

                if (pidl != nullptr)
                {
                    if (psfRoot && ILIsChild(pidl))
                    {
                        hr = psfRoot->QueryInterface(riid, ppv);
                    }
                    else
                    {
                        PUIDLIST_RELATIVE pidl_new;

                        hr = SHILCloneParent(pidl, &pidl_new);

                        if (FAILED(hr))
                            return hr;

                        hr = internal::SHBindToObject(psfRoot, pidl_new, ppbc, riid, ppv);

                        ILFree(pidl_new);
                    }

                    if (SUCCEEDED(hr) && ppidlLast)
                        *ppidlLast = ILFindLastID(pidl);
                }
                else
                {
                    hr = E_INVALIDARG;
                }

                return hr;
            }

            EXTERN_C __declspec(selectany) const CLSID CLSID_ShellItem = __uuidof(ShellItem);

            static HRESULT __fastcall CShellItem_CreateInstance(PCWSTR pszCLSID, const IID& riid, void** ppv)
            {
                *ppv = nullptr;

                IShellItem* psi;

                auto hr = SHCoCreateInstance(nullptr, &CLSID_ShellItem, nullptr, __uuidof(IShellItem), (void**)&psi);

                if (SUCCEEDED(hr))
                {
                    hr = psi->QueryInterface(riid, ppv);

                    psi->Release();
                }

                return hr;

            }

            static
                HRESULT
                STDAPICALLTYPE
                SHCreateItemFromIDList(
                    _In_ PCIDLIST_ABSOLUTE pidl,
                    _In_ REFIID riid,
                    _Outptr_ void** ppv
                )
            {
                *ppv = nullptr;

                IPersistIDList* ppidl;

                auto hr = CShellItem_CreateInstance(nullptr, __uuidof(IPersistIDList), (void**)&ppidl);

                if (SUCCEEDED(hr))
                {
                    hr = ppidl->SetIDList(pidl);

                    if (SUCCEEDED(hr))
                    {
                        hr = ppidl->QueryInterface(riid, ppv);
                    }

                    ppidl->Release();
                }

                return hr;
            }

            __forceinline
                static
                HRESULT
                ParseDisplayNameChild(
                    IShellFolder* pShellFolder,
                    /* [unique][in] */ __RPC__in_opt HWND hwnd,
                    /* [unique][in] */ __RPC__in_opt IBindCtx* pbc,
                    /* [string][in] */ __RPC__in_string LPWSTR pszDisplayName,
                    /* [annotation][unique][out][in] */
                    _Reserved_  ULONG* pchEaten,
                    /* [out] */ __RPC__deref_out_opt PIDLIST_RELATIVE* ppidl,
                    /* [unique][out][in] */ __RPC__inout_opt ULONG* pdwAttributes
                )
            {
                *ppidl = nullptr;

                PIDLIST_RELATIVE pidl;

                auto hr = pShellFolder->ParseDisplayName(hwnd, pbc, pszDisplayName, pchEaten, &pidl, pdwAttributes);

                if (SUCCEEDED(hr))
                {
                    *ppidl = pidl;
                }

                return hr;
            }


            static
                HRESULT
                STDAPICALLTYPE
                SHCreateItemWithParent(
                    _In_opt_ PCIDLIST_ABSOLUTE pidlParent,
                    _In_opt_ IShellFolder* psfParent,
                    _In_ PCUITEMID_CHILD pidl,
                    _In_ REFIID riid,
                    _Outptr_ void** ppvItem
                )
            {
                *ppvItem = nullptr;

                IParentAndItem* ppai;
                auto hr = CShellItem_CreateInstance(nullptr, __uuidof(IParentAndItem), (void**)&ppai);

                if (SUCCEEDED(hr))
                {
                    hr = ppai->SetParentAndItem(pidlParent, psfParent, pidl);

                    if (SUCCEEDED(hr))
                    {
                        hr = ppai->QueryInterface(riid, ppvItem);
                    }

                    ppai->Release();
                }

                return hr;
            }

            static
                HRESULT
                STDAPICALLTYPE
                SHCreateItemFromRelativeName(
                    _In_ IShellItem* psiParent,
                    _In_ PCWSTR pszName,
                    _In_opt_ IBindCtx* pbc,
                    _In_ REFIID riid,
                    _Outptr_ void** ppv
                )
            {
                *ppv = nullptr;

                IShellFolder* pShellFolder;

                auto hr = psiParent->BindToHandler(nullptr, BHID_SFObject, __uuidof(pShellFolder), (void**)&pShellFolder);

                if (SUCCEEDED(hr))
                {
                    PIDLIST_RELATIVE pidl;

                    hr = internal::ParseDisplayNameChild(pShellFolder, nullptr, pbc, (LPWSTR)pszName, nullptr, &pidl, nullptr);


                    if (SUCCEEDED(hr))
                    {
                        hr = internal::SHCreateItemWithParent(nullptr, pShellFolder, pidl, riid, ppv);

                        ILFree(pidl);
                    }


                    pShellFolder->Release();

                }

                return hr;
            }

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN7)
            static BOOL WINAPI ReadProcessMemory64(
                _In_ HANDLE hProcess,
                _In_ PVOID64 lpBaseAddress,
                _Out_writes_bytes_to_(nSize, *lpNumberOfBytesRead) LPVOID lpBuffer,
                _In_ ULONGLONG nSize,
                _Out_opt_ PULONGLONG lpNumberOfBytesRead
                )
            {
#if defined(_WIN64)
                static_assert(sizeof(SIZE_T) == sizeof(*lpNumberOfBytesRead), "");
                return ::ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, reinterpret_cast<SIZE_T*>(lpNumberOfBytesRead));
#else

                if (const auto _pfnNtWow64ReadVirtualMemory64 = try_get_NtWow64ReadVirtualMemory64())
                {
                    LONG _lStatus = _pfnNtWow64ReadVirtualMemory64(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
                    return _lStatus >= 0;
                }

                *lpNumberOfBytesRead = 0;
                return ::ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, (SIZE_T)nSize, reinterpret_cast<SIZE_T*>(lpNumberOfBytesRead));
#endif
            }
#endif
        }
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Windows Vista [desktop apps only]
	//Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	16,
	HRESULT,
	STDAPICALLTYPE,
	SHGetKnownFolderPath,
		_In_ REFKNOWNFOLDERID rfid,
		_In_ DWORD /* KNOWN_FOLDER_FLAG */ dwFlags,
		_In_opt_ HANDLE hToken,
		_Outptr_ PWSTR* ppszPath
		)
	{
		if (const auto pSHGetKnownFolderPath = try_get_SHGetKnownFolderPath())
		{
			return pSHGetKnownFolderPath(rfid, dwFlags, hToken, ppszPath);
		}
        if (!ppszPath)
        {
            return E_POINTER;
        }
		*ppszPath = nullptr;

        const auto _iCsidl = internal::KnownFoldersIdToCSIDL(rfid);
		if (_iCsidl == -1)
		{
			return __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}

        const auto _iCsidlFlags = internal::KnownFolderFlagsToCsidlFlags(dwFlags);
        if (_iCsidlFlags == -1)
        {
            return E_INVALIDARG;
        }

		auto pPathBuffer = (wchar_t*)CoTaskMemAlloc(MAX_PATH * sizeof(wchar_t));
		if (!pPathBuffer)
			return E_OUTOFMEMORY;

		auto hr = SHGetFolderPathW(
            nullptr,
            _iCsidl | _iCsidlFlags,
            hToken,
            (KF_FLAG_DEFAULT_PATH & dwFlags) ? SHGFP_TYPE_DEFAULT : SHGFP_TYPE_CURRENT,
            pPathBuffer);
		if (hr != S_OK)
		{
			CoTaskMemFree(pPathBuffer);
			return hr;
		}

		*ppszPath = pPathBuffer;
		return S_OK;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Windows Vista [desktop apps only]
	//Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	16,
	HRESULT,
	STDAPICALLTYPE,
	SHSetKnownFolderPath,
		_In_ REFKNOWNFOLDERID rfid,
		_In_ DWORD /* KNOWN_FOLDER_FLAG */ dwFlags,
		_In_opt_ HANDLE hToken,
		_In_ PCWSTR pszPath
		)
	{
		if (const auto pSHSetKnownFolderPath = try_get_SHSetKnownFolderPath())
		{
			return pSHSetKnownFolderPath(rfid, dwFlags, hToken, pszPath);
		}

		const auto _iCsidl = internal::KnownFoldersIdToCSIDL(rfid);
		if (_iCsidl == -1)
		{
			return __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}
        const auto _iCsidlFlags = internal::KnownFolderFlagsToCsidlFlags(dwFlags);
        if (_iCsidlFlags == -1)
        {
            return E_INVALIDARG;
        }

		return SHSetFolderPathW(_iCsidl | _iCsidlFlags, hToken, 0u, pszPath);
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Windows Vista [desktop apps only]
	//Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	16,
	HRESULT,
	STDAPICALLTYPE,
	SHGetKnownFolderIDList,
		_In_ REFKNOWNFOLDERID rfid,
		_In_ DWORD /* KNOWN_FOLDER_FLAG */ dwFlags,
		_In_opt_ HANDLE hToken,
		_Outptr_ PIDLIST_ABSOLUTE *ppidl
		)
	{
		if (const auto pSHGetKnownFolderIDList = try_get_SHGetKnownFolderIDList())
		{
			return pSHGetKnownFolderIDList(rfid, dwFlags, hToken, ppidl);
		}
        if (!ppidl)
        {
            return E_POINTER;
        }
        *ppidl = nullptr;

		const auto _iCsidl = internal::KnownFoldersIdToCSIDL(rfid);
		if (_iCsidl == -1)
		{
			return __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}
        const auto _iCsidlFlags = internal::KnownFolderFlagsToCsidlFlags(dwFlags);
        if (_iCsidlFlags == -1)
        {
            return E_INVALIDARG;
        }
		return SHGetFolderLocation(nullptr, _iCsidl | _iCsidlFlags, hToken, 0u, ppidl);
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Windows Vista [desktop apps only]
	//Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	20,
	HRESULT,
	STDAPICALLTYPE,
	SHBindToFolderIDListParent,
		_In_opt_ IShellFolder *psfRoot,
		_In_ PCUIDLIST_RELATIVE pidl,
		_In_ REFIID riid,
		_Outptr_ void **ppv,
		_Outptr_opt_ PCUITEMID_CHILD *ppidlLast
		)
	{
		if (const auto pSHBindToFolderIDListParent = try_get_SHBindToFolderIDListParent())
		{
			return pSHBindToFolderIDListParent(psfRoot, pidl, riid, ppv, ppidlLast);
		}

		return internal::SHBindToFolderIDListParentEx(psfRoot, pidl, nullptr, riid, ppv, ppidlLast);
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Windows Vista [desktop apps only]
	//Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	24,
	HRESULT,
	STDAPICALLTYPE,
	SHBindToFolderIDListParentEx,
		_In_opt_ IShellFolder *psfRoot,
		_In_ PCUIDLIST_RELATIVE pidl,
		_In_opt_ IBindCtx *ppbc,
		_In_ REFIID riid,
		_Outptr_ void **ppv,
		_Outptr_opt_ PCUITEMID_CHILD *ppidlLast
		)
	{
		if (const auto pSHBindToFolderIDListParentEx = try_get_SHBindToFolderIDListParentEx())
		{
			return pSHBindToFolderIDListParentEx(psfRoot, pidl, ppbc, riid, ppv, ppidlLast);
		}

		return internal::SHBindToFolderIDListParentEx(psfRoot, pidl, ppbc, riid, ppv, ppidlLast);
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Windows Vista [desktop apps only]
	//Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	20,
	HRESULT,
	STDAPICALLTYPE,
	SHBindToObject,
		_In_opt_ IShellFolder *psf,
		_In_ PCUIDLIST_RELATIVE pidl,
		_In_opt_ IBindCtx *pbc,
		_In_ REFIID riid,
		_Outptr_ void **ppv
		)
	{
		if (const auto pSHBindToObject = try_get_SHBindToObject())
		{
			return pSHBindToObject(psf, pidl, pbc, riid, ppv);
		}

		return internal::SHBindToObject(psf, pidl, pbc, riid, ppv);
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Windows Vista [desktop apps only]
	//Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	12,
	HRESULT,
	STDAPICALLTYPE,
	SHCreateItemFromIDList,
		_In_ PCIDLIST_ABSOLUTE pidl,
		_In_ REFIID riid,
		_Outptr_ void **ppv
		)
	{
		if (const auto pSHCreateItemFromIDList = try_get_SHCreateItemFromIDList())
		{
			return pSHCreateItemFromIDList(pidl, riid, ppv);
		}

	
		return internal::SHCreateItemFromIDList(pidl, riid, ppv);
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Windows Vista [desktop apps only]
	//Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	20,
	HRESULT,
	STDAPICALLTYPE,
	SHCreateItemWithParent,
		_In_opt_ PCIDLIST_ABSOLUTE pidlParent,
		_In_opt_ IShellFolder *psfParent,
		_In_ PCUITEMID_CHILD pidl,
		_In_ REFIID riid,
		_Outptr_ void **ppvItem
		)
	{
		if (const auto pSHCreateItemWithParent = try_get_SHCreateItemWithParent())
		{
			return pSHCreateItemWithParent(pidlParent, psfParent, pidl, riid, ppvItem);
		}

		return internal::SHCreateItemWithParent(pidlParent, psfParent, pidl, riid, ppvItem);
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Windows Vista [desktop apps only]
	//Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	20,
	HRESULT,
	STDAPICALLTYPE,
	SHCreateItemFromRelativeName,
		_In_ IShellItem *psiParent,
		_In_ PCWSTR pszName,
		_In_opt_ IBindCtx* pbc,
		_In_ REFIID riid,
		_Outptr_ void **ppv
		)
	{
		if (const auto pSHCreateItemFromRelativeName = try_get_SHCreateItemFromRelativeName())
		{
			return pSHCreateItemFromRelativeName(psiParent, pszName, pbc, riid, ppv);
		}

		*ppv = nullptr;

		IShellFolder* pShellFolder;

		auto hr = psiParent->BindToHandler(nullptr, BHID_SFObject, __uuidof(pShellFolder), (void**)&pShellFolder);

		if (SUCCEEDED(hr))
		{
			PIDLIST_RELATIVE pidl;

			hr = internal::ParseDisplayNameChild(pShellFolder, nullptr, pbc, (LPWSTR)pszName, nullptr, &pidl, nullptr);


			if (SUCCEEDED(hr))
			{
				hr = internal::SHCreateItemWithParent(nullptr, pShellFolder, pidl, riid, ppv);
				ILFree(pidl);
			}

			pShellFolder->Release();
		}

		return hr;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Windows Vista [desktop apps only]
	//Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	12,
	HRESULT,
	STDAPICALLTYPE,
	SHGetNameFromIDList,
		_In_ PCIDLIST_ABSOLUTE pidl,
		_In_ SIGDN sigdnName,
		_Outptr_ PWSTR *ppszName
		)
	{
		if (const auto pSHGetNameFromIDList = try_get_SHGetNameFromIDList())
		{
			return pSHGetNameFromIDList(pidl, sigdnName, ppszName);
		}

		*ppszName = NULL;

		IShellItem* ppsi;

	
		auto hr = internal::SHCreateItemFromIDList(pidl, __uuidof(ppsi), (void**)&ppsi);

		if (SUCCEEDED(hr))
		{
			hr = ppsi->GetDisplayName(sigdnName, ppszName);

			ppsi->Release();
		}

		return hr;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WINXPSP1)

	//Windows XP with SP1 [desktop apps only] 
	//Windows Server 2003 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	16,
	HRESULT,
	STDAPICALLTYPE,
	SHCreateShellItem,
		_In_opt_ PCIDLIST_ABSOLUTE pidlParent,
		_In_opt_ IShellFolder *psfParent,
		_In_ PCUITEMID_CHILD pidl,
		_Outptr_ IShellItem **ppsi
		)
	{
		if (const auto pSHCreateShellItem = try_get_SHCreateShellItem())
		{
			return pSHCreateShellItem(pidlParent, psfParent, pidl, ppsi);
		}
	
		*ppsi = nullptr;

		if (pidlParent || psfParent)
			return internal::SHCreateItemWithParent(pidlParent, psfParent, pidl, __uuidof(IShellItem), (void**)ppsi);
		else
			return internal::SHCreateItemFromIDList(pidl, __uuidof(IShellItem), (void**)ppsi);
	}
#endif
		

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	//Minimum supported client	Windows Vista [desktop apps only]
	//Minimum supported server	Windows Server 2008 [desktop apps only]
	__DEFINE_THUNK(
	shell32,
	16,
	HRESULT,
	STDAPICALLTYPE,
	SHCreateItemFromParsingName,
		_In_     PCWSTR    pszPath,
		_In_opt_ IBindCtx* pbc,
		_In_     REFIID    riid,
		_Outptr_ void**    ppv
		)
	{
		if (const auto pSHCreateItemFromParsingName = try_get_SHCreateItemFromParsingName())
		{
			return pSHCreateItemFromParsingName(pszPath, pbc, riid, ppv);
		}

		*ppv = nullptr;

		PIDLIST_ABSOLUTE pidl;
		auto hr = ::SHParseDisplayName(pszPath, pbc, &pidl, 0, nullptr);
		if (SUCCEEDED(hr))
		{
			hr = internal::SHCreateItemFromIDList(pidl, riid, ppv);

			ILFree(pidl);
		}

		return hr;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

	// 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
	__DEFINE_THUNK(
	shell32,
	8,
	HRESULT,
	STDAPICALLTYPE,
	Shell_NotifyIconGetRect,
        _In_ const NOTIFYICONIDENTIFIER* _pIdentifier,
        _Out_ RECT* _pIconLocation
        )
	{
		if (const auto _pfnShell_NotifyIconGetRect = try_get_Shell_NotifyIconGetRect())
		{
			return _pfnShell_NotifyIconGetRect(_pIdentifier, _pIconLocation);
		}

        if (!_pIconLocation)
        {
            return E_POINTER;
        }
        memset(_pIconLocation, 0, sizeof(*_pIconLocation));
        if (!_pIdentifier)
        {
            return E_POINTER;
        }

        if (_pIdentifier->cbSize < sizeof(*_pIdentifier))
        {
            return E_INVALIDARG;
        }

        if (GUID_NULL != _pIdentifier->guidItem)
        {
            return E_FAIL;
        }

        __WarningMessage__("没适配 Windows Vista。");

        // Vista系统？？？不好意思，我没有Vista系统……
        // 这是一份适用于XP的实现，代码参考：https://www.cnblogs.com/swarmbees/p/5812031.html
        HWND _hTrayHandle = FindWindowW(L"Shell_TrayWnd", NULL);

        //find the toolbar used in the notification area
        if (_hTrayHandle)
        {
            _hTrayHandle = FindWindowExW(_hTrayHandle, NULL, L"TrayNotifyWnd", NULL);
            if (_hTrayHandle)
            {
                if (HWND _hWnd = FindWindowExW(_hTrayHandle, NULL, L"SysPager", NULL))
                {
                    _hWnd = FindWindowExW(_hWnd, NULL, L"ToolbarWindow32", NULL);
                    if (_hWnd)
                    {
                        _hTrayHandle = _hWnd;
                    }
                }
            }
        }

        if (!_hTrayHandle)
        {
            return E_FAIL;
        }

        DWORD _uProcessID = 0;
        if (GetWindowThreadProcessId(_hTrayHandle, &_uProcessID) == 0)
        {
            return E_FAIL;
        }

        HRESULT _hr = E_FAIL;

        HANDLE _hTrayProcess = NULL;
        LPVOID _pBufferData = nullptr;

        do
        {
            _hTrayProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ, 0, _uProcessID);
            if (!_hTrayProcess)
            {
                break;
            }
            const int _nButtonCount = SendMessageW(_hTrayHandle, TB_BUTTONCOUNT, 0, 0);
            if (_nButtonCount < 1)
            {
                break;
            }

            union BufferInfo
            {
                TBBUTTON Button;
#if defined(_WIN64)
                TBBUTTON Button64;
#else
                struct _TBBUTTON64 {
                    int iBitmap;
                    int idCommand;
                    BYTE fsState;
                    BYTE fsStyle;
                    BYTE bReserved[6];          // padding for alignment
                    ULONGLONG dwData;
                    LONGLONG iString;
                };

                _TBBUTTON64 Button64;
#endif

                RECT Rect;
            };

            _pBufferData = VirtualAllocEx(_hTrayProcess, NULL, sizeof(BufferInfo), MEM_COMMIT, PAGE_READWRITE);
            if (!_pBufferData)
            {
                _hr = E_OUTOFMEMORY;
                break;
            }

#if defined(_X86_)
            SYSTEM_INFO _SystemInfo;
            ::GetNativeSystemInfo(&_SystemInfo);
            // 虽然这个逻辑不是完全的判断正确（不适用于ARM等情况，但是Shell_NotifyIconGetRect函数Windows 7开始就有了，所以完全够用。）
            const bool bWow64 = _SystemInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL;
#else
            constexpr bool bWow64 = false;
#endif

            //search for our icon among all toolbar buttons
            for (int _nToolbarButton = 0; _nToolbarButton < _nButtonCount; ++_nToolbarButton)
            {
                ULONGLONG _uReadBytes = 0;
                if (!(BOOL)SendMessageW(_hTrayHandle, TB_GETBUTTON, _nToolbarButton, (LPARAM)_pBufferData))
                    continue;

                decltype(BufferInfo::Button64) _Button64 = {};
#if defined(_X86_)
                if (sizeof(TBBUTTON) != sizeof(_Button64) && bWow64 == false)
                {
                    TBBUTTON Button = {};
                    if (!internal::ReadProcessMemory64(_hTrayProcess, _pBufferData, &Button, sizeof(Button), &_uReadBytes))
                        continue;
                    _Button64.fsState = Button.fsState;
                    _Button64.dwData = Button.dwData;
                }
                else
#endif
                {
                    if (!internal::ReadProcessMemory64(_hTrayProcess, _pBufferData, &_Button64, sizeof(_Button64), &_uReadBytes))
                        continue;
                }

                struct AppData
                {
                    HWND hWnd;
                    UINT uID;
                };

                AppData _AppData = {};
#if defined(_X86_)
                if (bWow64)
                {
                    struct AppData64
                    {
                        PVOID64 hWnd;
                        UINT uID;
                    };
                    AppData64 _AppData64 = {};
                    if (!internal::ReadProcessMemory64(_hTrayProcess, (PVOID64)_Button64.dwData, &_AppData64, sizeof(_AppData64), &_uReadBytes))
                        continue;

                    _AppData.hWnd = (HWND)_AppData64.hWnd;
                    _AppData.uID = _AppData64.uID;
                }
                else
#endif
                {
                    if (!internal::ReadProcessMemory64(_hTrayProcess, (PVOID64)_Button64.dwData, &_AppData, sizeof(_AppData), &_uReadBytes))
                        continue;
                }

                if (_pIdentifier->hWnd == _AppData.hWnd && _AppData.uID == _pIdentifier->uID)
                {
                    if (_Button64.fsState & TBSTATE_HIDDEN)
                        break;

                    if (!(BOOL)SendMessageW(_hTrayHandle, TB_GETITEMRECT, _nToolbarButton, (LPARAM)_pBufferData))
                    {
                        break;
                    }
                    RECT _IconRect = {};
                    if (internal::ReadProcessMemory64(_hTrayProcess, _pBufferData, &_IconRect, sizeof(RECT), &_uReadBytes))
                    {
                        if (MapWindowPoints(_hTrayHandle, NULL, (LPPOINT)&_IconRect, 2) == 0)
                        {
                            break;
                        }

                        _pIconLocation->left = _IconRect.left + 1;
                        _pIconLocation->top = _IconRect.top + 1;
                        _pIconLocation->right = _IconRect.right - 1;
                        _pIconLocation->bottom = _IconRect.bottom - 1;
                        _hr = S_OK;
                    }

                    break;
                }
            }

        } while (false);

        if(_pBufferData)
            VirtualFreeEx(_hTrayProcess, _pBufferData, 0, MEM_RELEASE);
        if(_hTrayProcess)
            CloseHandle(_hTrayProcess);

		return _hr;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    shell32,
    12,
    HRESULT,
    STDAPICALLTYPE,
    SHGetPropertyStoreForWindow,
        _In_  HWND   hwnd,
        _In_  REFIID riid,
        _Inout_ void** ppv
        )
    {
        if (auto const _pfnSHGetPropertyStoreForWindow = try_get_SHGetPropertyStoreForWindow())
        {
            return _pfnSHGetPropertyStoreForWindow(hwnd, riid, ppv);
        }

        if (ppv)
            *ppv = nullptr;

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    shell32,
    8,
    HRESULT,
    STDAPICALLTYPE,
    SHOpenWithDialog,
        _In_  HWND   hwndParent,
        _In_  const OPENASINFO* poainfo
        )
    {
        if (auto const _pfnSHOpenWithDialog = try_get_SHOpenWithDialog())
        {
            return _pfnSHOpenWithDialog(hwndParent, poainfo);
        }

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    shell32,
    4,
    HRESULT,
    STDAPICALLTYPE,
    SHQueryUserNotificationState,
        _Out_  QUERY_USER_NOTIFICATION_STATE* pquns
        )
    {
        if (auto const _pfnSHQueryUserNotificationState = try_get_SHQueryUserNotificationState())
        {
            return _pfnSHQueryUserNotificationState(pquns);
        }

        if (!pquns)
            return E_INVALIDARG;

        *pquns = QUNS_QUIET_TIME;
        return S_OK;
    }
#endif
} //namespace YY::Thunks
