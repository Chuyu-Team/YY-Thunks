#include <shellapi.h>

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN6)

	// 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 [仅限桌面应用]
	__DEFINE_THUNK(
	shell32,
	12,
	HRESULT,
	STDAPICALLTYPE,
	SHGetStockIconInfo,
        SHSTOCKICONID _eSiid,
        UINT _fFlags,
        _Inout_ SHSTOCKICONINFO* _pSii
        )
	{
		if (const auto _pfnSHGetStockIconInfo = try_get_SHGetStockIconInfo())
		{
			return _pfnSHGetStockIconInfo(_eSiid, _fFlags, _pSii);
		}
        
        if (_pSii == nullptr || _pSii->cbSize != sizeof(SHSTOCKICONINFO))
        {
            return E_INVALIDARG;
        }

        _pSii->hIcon = NULL;
        _pSii->iSysImageIndex = -1;
        _pSii->iIcon = -1;
        _pSii->szPath[0] = L'\0';

        if (_fFlags & (~(SHGSI_ICONLOCATION | SHGSI_ICON | SHGSI_SYSICONINDEX | SHGSI_LINKOVERLAY | SHGSI_SELECTED | SHGSI_LARGEICON | SHGSI_SMALLICON | SHGSI_SHELLICONSIZE)))
        {
            return E_INVALIDARG;
        }

        constexpr DWORD kMaxIcon = SIID_CLUSTEREDDRIVE + 1;

        // Vista系统的最高上限
        if ((DWORD)_eSiid >= kMaxIcon)
        {
            return E_INVALIDARG;
        }

        struct IconIndexItem
        {
            int iIcon;
            LPCWSTR szFileNmae;
            try_get_module_fun pfnGetModule;
        };

        // 适用于 Windows XP的映射表
        static const IconIndexItem kIconIndexMap[] =
        {
            { 1 }, //SIID_DOCNOASSOC = 0,              document (blank page), no associated program
            { 2 }, //SIID_DOCASSOC = 1,                document with an associated program
            { 3 }, //SIID_APPLICATION = 2,             generic application with no custom icon
            { 4 }, //SIID_FOLDER = 3,                  folder (closed)
            { 5 }, //SIID_FOLDEROPEN = 4,              folder (open)
            { 6 }, //SIID_DRIVE525 = 5,                5.25" floppy disk drive
            { 7 }, //SIID_DRIVE35 = 6,                 3.5" floppy disk drive
            { 8 }, //SIID_DRIVEREMOVE = 7,             removable drive
            { 9 }, //SIID_DRIVEFIXED = 8,              fixed (hard disk) drive
            { 10 }, //SIID_DRIVENET = 9,               network drive
            { 11 }, //SIID_DRIVENETDISABLED = 10,      disconnected network drive
            { 12 }, //SIID_DRIVECD = 11,               CD drive
            { 13 }, //SIID_DRIVERAM = 12,              RAM disk drive
            { 14 }, //SIID_WORLD = 13,                 entire network
            { 15 }, // 14
            { 16 }, //SIID_SERVER = 15,                a computer on the network
            { 17 }, //SIID_PRINTER = 16,               printer
            { 18 }, //SIID_MYNETWORK = 17,             My network places
            { 19 }, // 18
            { 20 }, // 19
            { 21 }, // 20
            { 22 }, // 21
            { 23 }, //SIID_FIND = 22,                  Find
            { 24 }, //SIID_HELP = 23,                  Help
            { 25 }, // 24
            { 26 }, // 25
            { 27 }, // 26
            { 28 }, // 27
            { 29 }, //SIID_SHARE = 28,                  overlay for shared items
            { 30 }, //SIID_LINK = 29,                   overlay for shortcuts to items
            { 31 }, //SIID_SLOWFILE = 30,               overlay for slow items
            { 32 }, //SIID_RECYCLER = 31,               empty recycle bin
            { 33 }, //SIID_RECYCLERFULL = 32,           full recycle bin
            { 34 }, // 33
            { 35 }, // 34
            { 36 }, // 35
            { 37 }, // 36
            { 38 }, // 37
            { 39 }, // 38
            { 40 }, // 39
            { 41 }, //SIID_MEDIACDAUDIO = 40,           Audio CD Media
            { 42 }, // 41
            { 43 }, // 42
            { 44 }, // 43
            { 45 }, // 44
            { 46 }, // 45
            { 47 }, // 46
            { 48 }, //SIID_LOCK = 47,                   Security lock
            { 49 }, // 48
            { 37 }, //SIID_AUTOLIST = 49,               AutoList
            { 311 }, //SIID_PRINTERNET = 50,            Network printer
            { 193 }, //SIID_SERVERSHARE = 51,           Server share
            { 197 }, //SIID_PRINTERFAX = 52,            Fax printer
            { 199 }, //SIID_PRINTERFAXNET = 53,         Networked Fax Printer
            { 252 }, //SIID_PRINTERFILE = 54,           Print to File
            { 37 }, //SIID_STACK = 55,                  Stack
            { 292 }, //SIID_MEDIASVCD = 56,             SVCD Media
            { 37 }, //SIID_STUFFEDFOLDER = 57,          Folder containing other items
            { 54 }, //SIID_DRIVEUNKNOWN = 58,           Unknown drive
            { 291 }, //SIID_DRIVEDVD = 59,              DVD Drive
            { 222 }, //SIID_MEDIADVD = 60,              DVD Media
            { 297 }, //SIID_MEDIADVDRAM = 61,           DVD-RAM Media
            { 318 }, //SIID_MEDIADVDRW = 62,            DVD-RW Media
            { 298 }, //SIID_MEDIADVDR = 63,             DVD-R Media
            { 304 }, //SIID_MEDIADVDROM = 64,           DVD-ROM Media
            { 293 }, //SIID_MEDIACDAUDIOPLUS = 65,      CD+ (Enhanced CD) Media
            { 294 }, //SIID_MEDIACDRW = 66,             CD-RW Media
            { 295 }, //SIID_MEDIACDR = 67,              CD-R Media
            { 266 }, //SIID_MEDIACDBURN = 68,           Burning CD
            { 302 }, //SIID_MEDIABLANKCD = 69,          Blank CD Media
            { 294 }, //SIID_MEDIACDROM = 70,            CD-ROM Media
            { 225 }, //SIID_AUDIOFILES = 71,            Audio files
            { 226 }, //SIID_IMAGEFILES = 72,            Image files
            { 224 }, //SIID_VIDEOFILES = 73,            Video files
            { 227 }, //SIID_MIXEDFILES = 74,            Mixed files
            { 4 }, //SIID_FOLDERBACK = 75,              Folder back
            { 4 }, //SIID_FOLDERFRONT = 76,             Folder front
            { 0 }, //SIID_SHIELD = 77,                  Security shield. Use for UAC prompts only.
            { 101, L"user32.dll", &try_get_module_user32 }, //SIID_WARNING = 78,Warning
            { 104, L"user32.dll", &try_get_module_user32 }, //SIID_INFO = 79,   Informational
            { 103, L"user32.dll", &try_get_module_user32 }, //SIID_ERROR = 80,  Error
            { 194 }, //SIID_KEY = 81,                   Key / Secure
            { 271 }, //SIID_SOFTWARE = 82,              Software
            { 242 }, //SIID_RENAME = 83,                Rename
            { 240 }, //SIID_DELETE = 84,                Delete
            { 292 }, //SIID_MEDIAAUDIODVD = 85,         Audio DVD Media
            { 292 }, //SIID_MEDIAMOVIEDVD = 86,         Movie DVD Media
            { 293 }, //SIID_MEDIAENHANCEDCD = 87,       Enhanced CD Media
            { 222 }, //SIID_MEDIAENHANCEDDVD = 88,      Enhanced DVD Media
            { 222 }, //SIID_MEDIAHDDVD = 89,            HD-DVD Media
            { 222 }, //SIID_MEDIABLURAY = 90,           BluRay Media
            { 293 }, //SIID_MEDIAVCD = 91,              VCD Media
            { 298 }, //SIID_MEDIADVDPLUSR = 92,         DVD+R Media
            { 318 }, //SIID_MEDIADVDPLUSRW = 93,        DVD+RW Media
            { 16 }, //SIID_DESKTOPPC = 94,              desktop computer
            { 16 }, //SIID_MOBILEPC = 95,               mobile computer (laptop/notebook) 找不到，弄个电脑图标
            { 269 }, //SIID_USERS = 96,                 users
            { 303 }, //SIID_MEDIASMARTMEDIA = 97,       Smart Media
            { 306 }, //SIID_MEDIACOMPACTFLASH = 98,     Compact Flash
            { 310 }, //SIID_DEVICECELLPHONE = 99,       Cell phone
            { 309 }, //SIID_DEVICECAMERA = 100,         Camera
            { 317 }, //SIID_DEVICEVIDEOCAMERA = 101,    Video camera
            { 299 }, //SIID_DEVICEAUDIOPLAYER = 102,    Audio player
            { 175 }, //SIID_NETWORKCONNECT = 103,       Connect to network
            { 273 }, //SIID_INTERNET = 104,             Internet
            { 101, L"zipfldr.dll", &try_get_module_zipfldr }, //SIID_ZIPFILE = 105, ZIP file
            { 137 }, //SIID_SETTINGS = 106,             Settings
            //// 107-131 are internal Vista RTM icons
            {}, // 107
            {}, // 108
            {}, // 109
            {}, // 110
            {}, // 111
            {}, // 112
            {}, // 113
            {}, // 114
            {}, // 115
            {}, // 116
            {}, // 117
            {}, // 118
            {}, // 119
            {}, // 120
            {}, // 121
            {}, // 122
            {}, // 123
            {}, // 124
            {}, // 125
            {}, // 126
            {}, // 127
            {}, // 128
            {}, // 129
            {}, // 130
            {}, // 131
            //// 132-159 for SP1 icons
            { 291 }, //SIID_DRIVEHDDVD = 132,         HDDVD Drive (all types)
            { 291 }, //SIID_DRIVEBD = 133,            BluRay Drive (all types)
            { 304 }, //SIID_MEDIAHDDVDROM = 134,      HDDVD-ROM Media
            { 298 }, //SIID_MEDIAHDDVDR = 135,        HDDVD-R Media
            { 297 }, //SIID_MEDIAHDDVDRAM = 136,      HDDVD-RAM Media
            { 304 }, //SIID_MEDIABDROM = 137,         BluRay ROM Media
            { 298 }, //SIID_MEDIABDR = 138,           BluRay R Media
            { 297 }, //SIID_MEDIABDRE = 139,          BluRay RE Media (Rewriable and RAM)
            { 9 }, //SIID_CLUSTEREDDRIVE = 140,       Clustered disk 图标跟普通硬盘意义，凑合用吧。
        };

        static_assert(kMaxIcon == _countof(kIconIndexMap), "");
        auto& _IconIndexInfo = kIconIndexMap[_eSiid];
        
        if (_IconIndexInfo.iIcon == 0)
        {
            return __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        }

        const auto _uResult = GetSystemDirectoryW(_pSii->szPath, _countof(_pSii->szPath));
        if (_uResult == 0 || _uResult >= _countof(_pSii->szPath))
        {
            return __HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        }

        if (::PathCchAppend(_pSii->szPath, _countof(_pSii->szPath), _IconIndexInfo.szFileNmae ? _IconIndexInfo.szFileNmae : L"shell32.dll") != S_OK)
        {
            return __HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        }

        _pSii->iIcon = _IconIndexInfo.iIcon;

        if (SHGSI_ICON & _fFlags)
        {
            auto _hModule = _IconIndexInfo.pfnGetModule ? _IconIndexInfo.pfnGetModule() : try_get_module_shell32();
            if (!_hModule)
            {
                return __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
            }

            SIZE _Size = {};
            if (_fFlags & SHGSI_SMALLICON)
            {
                _Size.cx = GetSystemMetrics(SM_CXSMICON);
                _Size.cy = GetSystemMetrics(SM_CYSMICON);
            }
            else
            {
                _Size.cx = GetSystemMetrics(SM_CXICON);
                _Size.cy = GetSystemMetrics(SM_CYICON);
            }

            if (SHGSI_SELECTED & _fFlags)
            {
                __WarningMessage__("Windows XP这里还没支持 SHGSI_SELECTED");
            }

            _pSii->hIcon = (HICON)LoadImageW(_hModule, MAKEINTRESOURCEW(_IconIndexInfo.iIcon), IMAGE_ICON, _Size.cx,_Size.cy, LR_SHARED);

            if (!_pSii->hIcon)
            {
                return __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
            }
        }

        return S_OK;
	}
#endif
}
