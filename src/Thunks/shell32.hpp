

#include <Shlobj.h>

namespace YY
{
	namespace Thunks
	{
#ifndef YY_Thunks_Defined
		namespace internal
		{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)
			struct KnownFoldersIdKey
			{
				const GUID& rfid;
				const int csidl;
			};

			//Vista以后的Shell32.dll 有个 kfapi::GetFolderIdByCSIDL，我们可以根据它可以反推出出 下面的KnownFoldersIdToCSIDL
			static int __fastcall KnownFoldersIdToCSIDL(const GUID& rfid)
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

					auto &KnownFoldersId = KnownFoldersIdsMap[middle];

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
				auto Clone =  ILCloneParent(pidl);
				*pidl_new = Clone;
				return Clone ? S_OK : E_OUTOFMEMORY;
			}

			static
			HRESULT
			STDAPICALLTYPE
			SHBindToObject(
				_In_opt_ IShellFolder *psf,
				_In_ PCUIDLIST_RELATIVE pidl,
				_In_opt_ IBindCtx *pbc,
				_In_ REFIID riid,
				_Outptr_ void **ppv
				)
			{
				*ppv = nullptr;

				IShellFolder *ppshf;

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
				_In_opt_ IShellFolder *psfRoot,
				_In_ PCUIDLIST_RELATIVE pidl,
				_In_opt_ IBindCtx *ppbc,
				_In_ REFIID riid,
				_Outptr_ void **ppv,
				_Outptr_opt_ PCUITEMID_CHILD *ppidlLast
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

			static HRESULT __fastcall CShellItem_CreateInstance(PCWSTR pszCLSID,const IID& riid, void **ppv)
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
				_Outptr_ void **ppv
				)
			{
				*ppv = nullptr;

				IPersistIDList *ppidl;

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
				/* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
				/* [string][in] */ __RPC__in_string LPWSTR pszDisplayName,
				/* [annotation][unique][out][in] */
				_Reserved_  ULONG *pchEaten,
				/* [out] */ __RPC__deref_out_opt PIDLIST_RELATIVE* ppidl,
				/* [unique][out][in] */ __RPC__inout_opt ULONG *pdwAttributes
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
				_In_opt_ IShellFolder *psfParent,
				_In_ PCUITEMID_CHILD pidl,
				_In_ REFIID riid,
				_Outptr_ void **ppvItem
				)
			{
				*ppvItem = nullptr;

				IParentAndItem *ppai;
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
				_In_ IShellItem *psiParent,
				_In_ PCWSTR pszName,
				_In_opt_ IBindCtx* pbc,
				_In_ REFIID riid,
				_Outptr_ void **ppv
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
		}
#endif



#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SHGetKnownFolderPath(
	_In_ REFKNOWNFOLDERID rfid,
	_In_ DWORD /* KNOWN_FOLDER_FLAG */ dwFlags,
	_In_opt_ HANDLE hToken,
	_Outptr_ PWSTR* ppszPath
	)
#ifdef YY_Thunks_Defined
;
#else
{
	if (const auto pSHGetKnownFolderPath = try_get_SHGetKnownFolderPath())
	{
		return pSHGetKnownFolderPath(rfid, dwFlags, hToken, ppszPath);
	}

	const auto csidl = internal::KnownFoldersIdToCSIDL(rfid);

	if (csidl == -1)
	{
		return __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}

	auto pPathBuffer = (wchar_t*)CoTaskMemAlloc(MAX_PATH * sizeof(wchar_t));
	if (!pPathBuffer)
		return E_OUTOFMEMORY;

	auto hr = SHGetFolderPathW(nullptr, csidl, hToken, dwFlags | csidl, pPathBuffer);
	if (hr != S_OK)
	{
		CoTaskMemFree(pPathBuffer);
		return hr;
	}

	*ppszPath = pPathBuffer;

	return S_OK;
}
#endif

__YY_Thunks_Expand_Function(shell32, SHGetKnownFolderPath, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SHSetKnownFolderPath(
	_In_ REFKNOWNFOLDERID rfid,
	_In_ DWORD /* KNOWN_FOLDER_FLAG */ dwFlags,
	_In_opt_ HANDLE hToken,
	_In_ PCWSTR pszPath
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (const auto pSHSetKnownFolderPath = try_get_SHSetKnownFolderPath())
	{
		return pSHSetKnownFolderPath(rfid, dwFlags, hToken, pszPath);
	}

	const auto csidl = internal::KnownFoldersIdToCSIDL(rfid);

	if (csidl == -1)
	{
		return __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}

	return SHSetFolderPathW(csidl, hToken, dwFlags, pszPath);
}
#endif

__YY_Thunks_Expand_Function(shell32, SHSetKnownFolderPath, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SHGetKnownFolderIDList(
	_In_ REFKNOWNFOLDERID rfid,
	_In_ DWORD /* KNOWN_FOLDER_FLAG */ dwFlags,
	_In_opt_ HANDLE hToken,
	_Outptr_ PIDLIST_ABSOLUTE *ppidl
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (const auto pSHGetKnownFolderIDList = try_get_SHGetKnownFolderIDList())
	{
		return pSHGetKnownFolderIDList(rfid, dwFlags, hToken, ppidl);
	}

	const auto csidl = internal::KnownFoldersIdToCSIDL(rfid);

	if (csidl == -1)
	{
		return __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}

	return SHGetFolderLocation(nullptr, csidl, hToken, dwFlags, ppidl);
}
#endif

__YY_Thunks_Expand_Function(shell32, SHGetKnownFolderIDList, 16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SHBindToFolderIDListParent(
	_In_opt_ IShellFolder *psfRoot,
	_In_ PCUIDLIST_RELATIVE pidl,
	_In_ REFIID riid,
	_Outptr_ void **ppv,
	_Outptr_opt_ PCUITEMID_CHILD *ppidlLast
	)
#ifdef YY_Thunks_Defined
;
#else
{
	if (const auto pSHBindToFolderIDListParent = try_get_SHBindToFolderIDListParent())
	{
		return pSHBindToFolderIDListParent(psfRoot, pidl, riid, ppv, ppidlLast);
	}

	return internal::SHBindToFolderIDListParentEx(psfRoot, pidl, nullptr, riid, ppv, ppidlLast);
}
#endif

__YY_Thunks_Expand_Function(shell32, SHBindToFolderIDListParent, 20);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SHBindToFolderIDListParentEx(
	_In_opt_ IShellFolder *psfRoot,
	_In_ PCUIDLIST_RELATIVE pidl,
	_In_opt_ IBindCtx *ppbc,
	_In_ REFIID riid,
	_Outptr_ void **ppv,
	_Outptr_opt_ PCUITEMID_CHILD *ppidlLast
	)
#ifdef YY_Thunks_Defined
;
#else
{
	if (const auto pSHBindToFolderIDListParentEx = try_get_SHBindToFolderIDListParentEx())
	{
		return pSHBindToFolderIDListParentEx(psfRoot, pidl, ppbc, riid, ppv, ppidlLast);
	}

	return internal::SHBindToFolderIDListParentEx(psfRoot, pidl, ppbc, riid, ppv, ppidlLast);
}
#endif

__YY_Thunks_Expand_Function(shell32, SHBindToFolderIDListParentEx, 24);


#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SHBindToObject(
	_In_opt_ IShellFolder *psf,
	_In_ PCUIDLIST_RELATIVE pidl,
	_In_opt_ IBindCtx *pbc,
	_In_ REFIID riid,
	_Outptr_ void **ppv
	)
#ifdef YY_Thunks_Defined
;
#else
{
	if (const auto pSHBindToObject = try_get_SHBindToObject())
	{
		return pSHBindToObject(psf, pidl, pbc, riid, ppv);
	}

	return internal::SHBindToObject(psf, pidl, pbc, riid, ppv);
}
#endif

__YY_Thunks_Expand_Function(shell32, SHBindToObject, 20);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SHCreateItemFromIDList(
	_In_ PCIDLIST_ABSOLUTE pidl,
	_In_ REFIID riid,
	_Outptr_ void **ppv
	)
#ifdef YY_Thunks_Defined
;
#else
{
	if (const auto pSHCreateItemFromIDList = try_get_SHCreateItemFromIDList())
	{
		return pSHCreateItemFromIDList(pidl, riid, ppv);
	}

	
	return internal::SHCreateItemFromIDList(pidl, riid, ppv);
}
#endif

__YY_Thunks_Expand_Function(shell32, SHCreateItemFromIDList, 12);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SHCreateItemWithParent(
	_In_opt_ PCIDLIST_ABSOLUTE pidlParent,
	_In_opt_ IShellFolder *psfParent,
	_In_ PCUITEMID_CHILD pidl,
	_In_ REFIID riid,
	_Outptr_ void **ppvItem
	)
#ifdef YY_Thunks_Defined
;
#else
{
	if (const auto pSHCreateItemWithParent = try_get_SHCreateItemWithParent())
	{
		return pSHCreateItemWithParent(pidlParent, psfParent, pidl, riid, ppvItem);
	}

	return internal::SHCreateItemWithParent(pidlParent, psfParent, pidl, riid, ppvItem);
}

#endif

__YY_Thunks_Expand_Function(shell32, SHCreateItemWithParent, 20);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SHCreateItemFromRelativeName(
	_In_ IShellItem *psiParent,
	_In_ PCWSTR pszName,
	_In_opt_ IBindCtx* pbc,
	_In_ REFIID riid,
	_Outptr_ void **ppv
	)
#ifdef YY_Thunks_Defined
;
#else
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

__YY_Thunks_Expand_Function(shell32, SHCreateItemFromRelativeName, 20);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SHGetNameFromIDList(
	_In_ PCIDLIST_ABSOLUTE pidl,
	_In_ SIGDN sigdnName,
	_Outptr_ PWSTR *ppszName
	)
#ifdef YY_Thunks_Defined
;
#else
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

__YY_Thunks_Expand_Function(shell32, SHGetNameFromIDList, 12);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WINXPSP1)
//Windows XP with SP1 [desktop apps only] 
//Windows Server 2003 [desktop apps only]

EXTERN_C
HRESULT
STDAPICALLTYPE
SHCreateShellItem(
	_In_opt_ PCIDLIST_ABSOLUTE pidlParent,
	_In_opt_ IShellFolder *psfParent,
	_In_ PCUITEMID_CHILD pidl,
	_Outptr_ IShellItem **ppsi
	)
#ifdef YY_Thunks_Defined
;
#else
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

__YY_Thunks_Expand_Function(shell32, SHCreateShellItem, 20);

#endif

	}//namespace Thunks

} //namespace YY