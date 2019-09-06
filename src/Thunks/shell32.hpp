

#include <Shlobj.h>

namespace YY
{
	namespace Thunks
	{
#ifndef YY_Thunks_Defined
		namespace internal
		{
			struct KnownFoldersIdKey
			{
				GUID rfid;
				DWORD csidl;
			};

#define YY_CSIDL_CHECK_WOW64 0x80000000
#define YY_CSIDL_MARK        0x000000FF
			static int __fastcall KnownFoldersIdToCSIDL(const GUID& rfid)
			{

				static const KnownFoldersIdKey KnownFoldersIdsMap[] =
				{
					{ FOLDERID_Windows, CSIDL_WINDOWS },
					{ FOLDERID_ProgramFilesCommon, CSIDL_PROGRAM_FILES_COMMON },
					{ FOLDERID_PublicDesktop, CSIDL_COMMON_DESKTOPDIRECTORY },
					{ FOLDERID_CDBurning, CSIDL_CDBURN_AREA },
					{ FOLDERID_CommonStartMenu, CSIDL_COMMON_STARTMENU },
					{ FOLDERID_Videos, CSIDL_MYVIDEO },
					{ FOLDERID_ConnectionsFolder, CSIDL_CONNECTIONS },
					{ FOLDERID_PrintersFolder, CSIDL_PRINTERS },
					{ FOLDERID_Pictures, CSIDL_MYPICTURES },
					{ FOLDERID_ResourceDir, CSIDL_RESOURCES },
					{ FOLDERID_CommonStartup, CSIDL_COMMON_STARTUP },
					{ FOLDERID_PublicVideos, CSIDL_COMMON_VIDEO },
					{ FOLDERID_Desktop, CSIDL_DESKTOP },
					{ FOLDERID_History, CSIDL_HISTORY },
					{ FOLDERID_SamplePictures, CSIDL_COMMON_PICTURES }, //做个兼容处理吧，反正都是放图片的
					{ FOLDERID_RecycleBinFolder, CSIDL_BITBUCKET },
					{ FOLDERID_CommonPrograms, CSIDL_COMMON_PROGRAMS },
					{ FOLDERID_NetHood, CSIDL_NETHOOD },
					{ FOLDERID_Cookies, CSIDL_COOKIES },
					{ FOLDERID_LocalizedResourcesDir, CSIDL_RESOURCES_LOCALIZED },
					{ FOLDERID_Favorites, CSIDL_FAVORITES },
					{ FOLDERID_SampleMusic, CSIDL_COMMON_MUSIC }, //做个兼容处理吧，反正都是放音乐的
					{ FOLDERID_SendTo, CSIDL_SENDTO },
					{ FOLDERID_AdminTools, CSIDL_ADMINTOOLS },
					{ FOLDERID_Music, CSIDL_MYMUSIC },
					{ FOLDERID_InternetFolder, CSIDL_INTERNET },
					{ FOLDERID_System, CSIDL_SYSTEM },
					{ FOLDERID_Programs, CSIDL_PROGRAMS },
					{ FOLDERID_ProgramFilesX64, CSIDL_PROGRAM_FILES | YY_CSIDL_CHECK_WOW64 }, //兼容下，反正 x64路径 跟 普通的是一样的。
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
					{ FOLDERID_ProgramFilesCommonX64, CSIDL_PROGRAM_FILES_COMMON | YY_CSIDL_CHECK_WOW64 }, //兼容下，反正 x64路径 跟 普通的是一样的。
					{ FOLDERID_PublicDocuments, CSIDL_COMMON_DOCUMENTS },
					{ FOLDERID_SystemX86, CSIDL_SYSTEMX86 },
					{ FOLDERID_PublicMusic, CSIDL_COMMON_MUSIC },
					{ FOLDERID_ProgramFiles, CSIDL_PROGRAM_FILES },
					{ FOLDERID_Fonts, CSIDL_FONTS },
					{ FOLDERID_Startup, CSIDL_STARTUP },
					{ FOLDERID_StartMenu, CSIDL_STARTMENU },
					{ FOLDERID_NetworkFolder, CSIDL_NETWORK },
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
#ifdef _X86_
						if (KnownFoldersId.csidl & YY_CSIDL_CHECK_WOW64)
						{
							SYSTEM_INFO SystemInfo;
							GetNativeSystemInfo(&SystemInfo);

							if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
							{
								return -1;
							}
						}
#endif
						return KnownFoldersId.csidl & YY_CSIDL_MARK;
					}
					if (testIndex < 0)
						top = middle - 1;
					else
						bottom = middle + 1;
				}

				return -1;
			}
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
		return E_INVALIDARG;
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
		return E_INVALIDARG;
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
		return E_INVALIDARG;
	}

	return SHGetFolderLocation(nullptr, csidl, hToken, dwFlags, ppidl);
}
#endif

__YY_Thunks_Expand_Function(shell32, SHGetKnownFolderIDList, 16);

#endif

	}//namespace Thunks

} //namespace YY