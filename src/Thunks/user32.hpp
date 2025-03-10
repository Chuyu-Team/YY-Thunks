#if (YY_Thunks_Target < __WindowsNT10_15063)
#include <shellscalingapi.h>
#endif

#if (YY_Thunks_Target < __WindowsNT10_14393) && !defined(__Comment_Lib_user32)
#define __Comment_Lib_user32
#pragma comment(lib, "User32.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT10_15063) && (YY_Thunks_Target >= __WindowsNT6_3 || __YY_Thunks_libs) && !defined(__Comment_Lib_shcore)
#define __Comment_Lib_shcore
#pragma comment(lib, "Shcore.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT10_14393) && !defined(__Comment_Lib_gdi32)
#define __Comment_Lib_gdi32
#pragma comment(lib, "Gdi32.lib")
#endif

#if defined(YY_Thunks_Implemented)
namespace YY::Thunks::internal
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT10_17134)
#if defined(__ENABLE_User32Gpsi)
        // Gpsi方案分支太多，暂时弃用。
        static const BYTE* __fastcall GetUser32Gpsi() noexcept
        {
            static const BYTE* s_gpsi;
            if (s_gpsi)
                return s_gpsi;

            auto _pfnGetCapture = (const BYTE*)::GetProcAddress(try_get_module_user32(), "GetCapture");
            if (!_pfnGetCapture)
                return nullptr;

            const BYTE* _pData = nullptr;
#if defined(_X86_)
            if (_pData[0] == 0xA1u)
            {
                // 76052B20 A1 50 CC 0C 76       mov         eax,dword ptr [_gpsi (760CCC50h)]
                _pData = _pfnGetCapture;
                _pData += 1;
                _pData = (const BYTE*)(*(DWORD*)_pData);
            }
#elif defined(_AMD64_)
            if (_pfnGetCapture[0] == 0x48u && _pfnGetCapture[1] == 0x8Bu && _pfnGetCapture[2] == 0x05u)
            {
                // 00007FFD8D60A870 48 8B 05 99 F9 08 00 mov         rax,qword ptr [gpsi (07FFD8D69A210h)] 
                _pData = _pfnGetCapture;
                _pData += 3;
                _pData += *(DWORD*)_pData;
                _pData += sizeof(DWORD);
            }
#else
#error ""
#endif

            if (!_pData)
                return nullptr;

            auto _gpsi = *(const BYTE**)_pData;
            s_gpsi = _gpsi;
            return _gpsi;
        }
#else // #if defined(__ENABLE_User32Gpsi)
        // RemoteGetSystemDpi::GetSystemDpi
        static constexpr const BYTE kRemoteGetSystemDpiShellCode[] =
#if defined(_X86_)
        {
            /*003741F0*/ 0x55,                         // push        ebp
            /*003741F1*/ 0x8B, 0xEC,                   // mov         ebp,esp
            /*003741F3*/ 0x56,                         // push        esi


            // if (!pfnLoadLibraryA(szUser32ModlueName))
            //     return 0;
            /*003741F4*/ 0x8B, 0x75, 0x08,             // mov         esi,dword ptr[this(08h)]
            /*003741F7*/ 0x8D, 0x46, 0x18,             // lea         eax,[esi +  szUser32ModlueName(18h)]
            /*003741FD*/ 0x50,                         // push        eax
            /*003741FE*/ 0x8B, 0x06,                   // mov         eax,dword ptr[esi + pfnLoadLibraryA(00h)]
            /*00374204*/ 0xFF, 0xD0,                   // call        eax
            /*00374206*/ 0x85, 0xC0,                   // test        eax,eax
            /*00374208*/ 0x75, 0x05,                   // jne         037420Fh
            /*0037420A*/ 0x5E,                         // pop         esi
            /*0037420B*/ 0x5D,                         // pop         ebp
            /*0037420C*/ 0xC2, 0x04, 0x00,             // ret         4

            // pfnSetProcessDPIAware();
            /*0037420F*/ 0x8B, 0x46, 0x04,             // mov         eax, dword ptr[esi + pfnSetProcessDPIAware(04h)]
            /*00374215*/ 0xFF, 0xD0,                   // call        eax

            // if (pfnGetDpiForSystem)
            //     return pfnGetDpiForSystem();
            /*00374217*/ 0x8B, 0x46, 0x14,             // mov         eax, dword ptr[esi + pfnGetDpiForSystem(14h)]
            /*0037421D*/ 0x85, 0xC0,                   // test        eax, eax
            /*0037421F*/ 0x74, 0x07,                   // je          0374228h
            /*00374221*/ 0xFF, 0xD0,                   // call        eax
            /*00374223*/ 0x5E,                         // pop         esi
            /*00374224*/ 0x5D,                         // pop         ebp
            /*00374225*/ 0xC2, 0x04, 0x00,             // ret         4

            // UINT _uDpi = USER_DEFAULT_SCREEN_DPI;
            // if (auto _hDc = pfnGetDC(nullptr))
            // {
            //     _uDpi = pfnGetDeviceCaps(_hDc, LOGPIXELSX);
            //     pfnReleaseDC(NULL, _hDc);
            // }
            /*00374228*/ 0x8B, 0x46, 0x08,             // mov         eax, dword ptr[esi + pfnGetDC(08h)]
            /*0037422E*/ 0x53,                         // push        ebx
            /*0037422F*/ 0x57,                         // push        edi
            /*00374230*/ 0x6A, 0x00,                   // push        0
            /*00374232*/ 0xBB, 0x60, 0x00, 0x00, 0x00, // mov         ebx, USER_DEFAULT_SCREEN_DPI(60h)
            /*00374237*/ 0xFF, 0xD0,                   // call        eax
            /*00374239*/ 0x8B, 0xF8,                   // mov         edi, eax
            /*0037423B*/ 0x85, 0xFF,                   // test        edi, edi
            /*0037423D*/ 0x74, 0x12,                   // je          0374257h
            /*0037423F*/ 0x8B, 0x46, 0x10,             // mov         eax, dword ptr[esi + pfnGetDeviceCaps(10h)]
            /*00374245*/ 0x6A, 0x58,                   // push        LOGPIXELSX(58h)
            /*00374247*/ 0x57,                         // push        edi
            /*00374248*/ 0xFF, 0xD0,                   // call        eax
            /*0037424A*/ 0x8B, 0x4E, 0x0C,             // mov         ecx, dword ptr[esi + pfnReleaseDC(0Ch)]
            /*00374250*/ 0x8B, 0xD8,                   // mov         ebx, eax
            /*00374252*/ 0x57,                         // push        edi
            /*00374253*/ 0x6A, 0x00,                   // push        0
            /*00374255*/ 0xFF, 0xD1,                   // call        ecx

            // return _uDpi;
            /*00374257*/ 0x5F,                         // pop         edi
            /*00374258*/ 0x8B, 0xC3,                   // mov         eax, ebx
            /*0037425A*/ 0x5B,                         // pop         ebx
            /*0037425B*/ 0x5E,                         // pop         esi
            /*0037425C*/ 0x5D,                         // pop         ebp
            /*0037425D*/ 0xC2, 0x04, 0x00,             // ret         4
        };
#elif defined(_AMD64_)
        {
            /*00007FF7F6564800*/ 0x40, 0x53,                   // push        rbx
            /*00007FF7F6564802*/ 0x48, 0x83, 0xEC, 0x20,       // sub         rsp,20h
            /*00007FF7F6564806*/ 0x48, 0x8B, 0xD9,             // mov         rbx,rcx

            // if (!pfnLoadLibraryA(szUser32ModlueName))
            //    return 0;
            /*00007FF7F6564809*/ 0x48, 0x83, 0xC1, 0x30,       // add         rcx,30h
            /*00007FF7F656480D*/ 0xFF, 0x13,                   // call        qword ptr [rbx]
            /*00007FF7F656480F*/ 0x48, 0x85, 0xC0,             // test        rax,rax
            /*00007FF7F6564812*/ 0x75, 0x06,                   // jne         MyStruct::Do+1Ah (07FF7F656481Ah)
            /*00007FF7F6564814*/ 0x48, 0x83, 0xC4, 0x20,       // add         rsp,20h
            /*00007FF7F6564818*/ 0x5B,                         // pop         rbx
            /*00007FF7F6564819*/ 0xC3,                         // ret

            // pfnSetProcessDPIAware();
            /*00007FF7F656481A*/ 0xFF, 0x53, 0x08,             // call        qword ptr [rbx+8]

            // if (pfnGetDpiForSystem)
            //   return pfnGetDpiForSystem();
            /*00007FF7F656481D*/ 0x48, 0x8B, 0x43, 0x28,       // mov         rax,qword ptr [rbx+28h]
            /*00007FF7F6564821*/ 0x48, 0x85, 0xC0,             // test        rax,rax
            /*00007FF7F6564824*/ 0x74, 0x08,                   // je          MyStruct::Do+2Eh (07FF7F656482Eh)
            /*00007FF7F6564826*/ 0x48, 0x83, 0xC4, 0x20,       // add         rsp,20h
            /*00007FF7F656482A*/ 0x5B,                         // pop         rbx
            /*00007FF7F656482B*/ 0x48, 0xFF, 0xE0,             // jmp         rax
            /*00007FF7F656482E*/ 0x48, 0x89, 0x74, 0x24, 0x30, // mov         qword ptr [rsp+30h],rsi

            // UINT _uDpi = USER_DEFAULT_SCREEN_DPI;
            // if (auto _hDc = pfnGetDC(nullptr))
            // {
            //     _uDpi = pfnGetDeviceCaps(_hDc, LOGPIXELSX);
            //     pfnReleaseDC(NULL, _hDc);
            // }
            /*00007FF7F6564833*/ 0x33, 0xC9,                   // xor         ecx,ecx
            /*00007FF7F6564835*/ 0x48, 0x89, 0x7C, 0x24, 0x38, // mov         qword ptr [rsp+38h],rdi
            /*00007FF7F656483A*/ 0xBF, 0x60, 0x00, 0x00, 0x00, // mov         edi,60h
            /*00007FF7F656483F*/ 0xFF, 0x53, 0x10,             // call        qword ptr [rbx+pfnGetDC(10h)]
            /*00007FF7F6564842*/ 0x48, 0x8B, 0xF0,             // mov         rsi,rax
            /*00007FF7F6564845*/ 0x48, 0x85, 0xC0,             // test        rax,rax
            /*00007FF7F6564848*/ 0x74, 0x15,                   // je          MyStruct::Do+5Fh (07FF7F656485Fh)
            /*00007FF7F656484A*/ 0xBA, 0x58, 0x00, 0x00, 0x00, // mov         edx,LOGPIXELSX(58h)
            /*00007FF7F656484F*/ 0x48, 0x8B, 0xC8,             // mov         rcx,rax
            /*00007FF7F6564852*/ 0xFF, 0x53, 0x20,             // call        qword ptr [rbx+pfnGetDeviceCaps(20h)]
            /*00007FF7F6564855*/ 0x48, 0x8B, 0xD6,             // mov         rdx,rsi
            /*00007FF7F6564858*/ 0x33, 0xC9,                   // xor         ecx,ecx
            /*00007FF7F656485A*/ 0x8B, 0xF8,                   // mov         edi,eax
            /*00007FF7F656485C*/ 0xFF, 0x53, 0x18,             // call        qword ptr [rbx+pfnReleaseDC(18h)]

            // return _uDpi;
            /*00007FF7F656485F*/ 0x48, 0x8B, 0x74, 0x24, 0x30, // mov         rsi,qword ptr [rsp+30h]
            /*00007FF7F6564864*/ 0x8B, 0xC7,                   // mov         eax,edi
            /*00007FF7F6564866*/ 0x48, 0x8B, 0x7C, 0x24, 0x38, // mov         rdi,qword ptr [rsp+38h]
            /*00007FF7F656486B*/ 0x48, 0x83, 0xC4, 0x20,       // add         rsp,20h
            /*00007FF7F656486F*/ 0x5B,                         // pop         rbx
            /*00007FF7F6564870*/ 0xC3,                         // ret
        };
#else // #elif defined(_AMD64_)
#error ""
#endif // #if defined(_X86_)
        struct RemoteGetSystemDpi
        {
            decltype(LoadLibraryA)* pfnLoadLibraryA = nullptr;
            decltype(SetProcessDPIAware)* pfnSetProcessDPIAware = nullptr;
            decltype(GetDC)* pfnGetDC = nullptr;
            decltype(ReleaseDC)* pfnReleaseDC = nullptr;
            decltype(GetDeviceCaps)* pfnGetDeviceCaps = nullptr;
            decltype(GetDpiForSystem)* pfnGetDpiForSystem = nullptr;
            char szUser32ModlueName[7] = "user32";

#if 0
            UINT __stdcall GetSystemDpi()
            {
                if (!pfnLoadLibraryA(szUser32ModlueName))
                    return 0;

                pfnSetProcessDPIAware();
                if (pfnGetDpiForSystem)
                {
                    return pfnGetDpiForSystem();
                }

                UINT _uDpi = USER_DEFAULT_SCREEN_DPI;
                if (auto _hDc = pfnGetDC(nullptr))
                {
                    _uDpi = pfnGetDeviceCaps(_hDc, LOGPIXELSX);
                    pfnReleaseDC(NULL, _hDc);
                }

                return _uDpi;
            }
#endif
        };

        struct RemoteGetSystemDpiBuffer
        {
            BYTE ShellCode[(_countof(kRemoteGetSystemDpiShellCode) + 127) / 128 * 128];
            RemoteGetSystemDpi ShellCodeParameter;

            bool __fastcall Init() noexcept
            {
                __if_exists(try_get_LoadLibraryA)
                {
                    ShellCodeParameter.pfnLoadLibraryA = try_get_LoadLibraryA();
                }
                __if_not_exists(try_get_LoadLibraryA)
                {
                    ShellCodeParameter.pfnLoadLibraryA = (decltype(LoadLibraryA)*)GetProcAddress(try_get_module_kernel32(), "LoadLibraryA");
                }

                if (!ShellCodeParameter.pfnLoadLibraryA)
                    return false;

                auto _hUser32Module = try_get_module_user32();
                if (!_hUser32Module)
                    return false;

                __if_exists(try_get_SetProcessDPIAware)
                {
                    ShellCodeParameter.pfnSetProcessDPIAware = try_get_SetProcessDPIAware();
                }
                __if_not_exists(try_get_SetProcessDPIAware)
                {
                    ShellCodeParameter.pfnSetProcessDPIAware = (decltype(SetProcessDPIAware)*)GetProcAddress(_hUser32Module, "SetProcessDPIAware");
                }
                if (!ShellCodeParameter.pfnSetProcessDPIAware)
                    return false;

                __if_exists(try_get_GetDpiForSystem)
                {
                    ShellCodeParameter.pfnGetDpiForSystem = try_get_GetDpiForSystem();
                }
                __if_not_exists(try_get_GetDpiForSystem)
                {
                    ShellCodeParameter.pfnGetDpiForSystem = (decltype(GetDpiForSystem)*)GetProcAddress(_hUser32Module, "GetDpiForSystem");
                }

                if (!ShellCodeParameter.pfnGetDpiForSystem)
                {
                    ShellCodeParameter.pfnGetDC = (decltype(GetDC)*)GetProcAddress(_hUser32Module, "GetDC");
                    if (!ShellCodeParameter.pfnGetDC)
                        return false;

                    ShellCodeParameter.pfnReleaseDC = (decltype(ReleaseDC)*)GetProcAddress(_hUser32Module, "ReleaseDC");
                    if (!ShellCodeParameter.pfnReleaseDC)
                        return false;

                    ShellCodeParameter.pfnGetDeviceCaps = (decltype(GetDeviceCaps)*)GetProcAddress(_hUser32Module, "GetDeviceCaps");
                    if (!ShellCodeParameter.pfnGetDeviceCaps)
                        return false;
                }

                memcpy(ShellCode, kRemoteGetSystemDpiShellCode, sizeof(kRemoteGetSystemDpiShellCode));
                return true;
            }
        };
#endif // #if defined(__ENABLE_User32Gpsi)
#endif // #if (YY_Thunks_Target < __WindowsNT10_17134)

#if (YY_Thunks_Target < __WindowsNT10_14393)
        static UINT __fastcall GetDpiForSystemDownlevel() noexcept
        {
            if (!IsProcessDPIAware())
            {
                return USER_DEFAULT_SCREEN_DPI;
            }

            static int nDPICache = 0;

            if (nDPICache)
            {
                return nDPICache;
            }

            int nDpiX = USER_DEFAULT_SCREEN_DPI;

            if (HDC hdc = GetDC(NULL))
            {
                nDpiX = GetDeviceCaps(hdc, LOGPIXELSX);
                ReleaseDC(NULL, hdc);
            }

            return nDPICache = nDpiX;
        }
#endif

#if (YY_Thunks_Target < __WindowsNT10_17134)

        /// <summary>
        /// 获取真实的不受感知影响的系统DPI。如果缺少_hSystemAwareWnd参数，那么函数内部可能会创建一个临时进程去确定Dpi。
        /// </summary>
        /// <param name="_hSystemAwareWnd">系统Dpi感知的窗口。</param>
        /// <returns>系统真实的Dpi。</returns>
        static UINT __fastcall GetDpiForGlobalSystemDownlevel(_In_opt_ HWND _hSystemAwareWnd = nullptr) noexcept
        {
            static UINT s_uGlobalSystemDpi = 0;
            if (s_uGlobalSystemDpi)
                return s_uGlobalSystemDpi;

            UINT _uDpi = GetDpiForSystemDownlevel();

            do
            {
                if (IsProcessDPIAware())
                    break;

                if (internal::GetSystemVersion() >= __WindowsNT6_3)
                {
                    // [6.3, )：这类系统屏幕Dpi可以随时调整
#if !defined(__ENABLE_User32Gpsi)
                    if (_hSystemAwareWnd)
                    {
                        RECT _oWindowRect;
                        if (!GetWindowRect(_hSystemAwareWnd, &_oWindowRect))
                            return 0;

                        const SIZE _oLogicalSize = { _oWindowRect.right - _oWindowRect.left, _oWindowRect.bottom - _oWindowRect.top };
                        if (_oLogicalSize.cx >= 8 || _oLogicalSize.cy >= 8)
                        {
                            HMONITOR _hMonitor = MonitorFromWindow(_hSystemAwareWnd, MONITOR_DEFAULTTOPRIMARY);
                            if (_hMonitor)
                            {
                                MONITORINFOEXW _oMonitorInfo = { {sizeof(_oMonitorInfo)} };
                                DEVMODEW _oDevMode = {};
                                _oDevMode.dmSize = sizeof(_oDevMode);
                                if (GetMonitorInfoW(_hMonitor, &_oMonitorInfo) && EnumDisplaySettingsW(_oMonitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &_oDevMode))
                                {
                                    // MonitorDpi
                                    auto _uMonitorDpi = _oDevMode.dmPelsHeight * USER_DEFAULT_SCREEN_DPI / (_oMonitorInfo.rcMonitor.bottom - _oMonitorInfo.rcMonitor.top);

#if (YY_Thunks_Target < __WindowsNT6_3)
                                    auto const LogicalToPhysicalPointForPerMonitorDPI = try_get_LogicalToPhysicalPointForPerMonitorDPI();
#endif

                                    if (LogicalToPhysicalPointForPerMonitorDPI(_hSystemAwareWnd, (LPPOINT)&_oWindowRect.left)
                                        && LogicalToPhysicalPointForPerMonitorDPI(_hSystemAwareWnd, (LPPOINT)&_oWindowRect.right))
                                    {
                                        if (_oLogicalSize.cx >= _oLogicalSize.cy)
                                        {
                                            _uDpi = MulDiv(_oLogicalSize.cx, _uMonitorDpi, _oWindowRect.right - _oWindowRect.left);
                                        }
                                        else
                                        {
                                            _uDpi = MulDiv(_oLogicalSize.cy, _uMonitorDpi, _oWindowRect.bottom - _oWindowRect.top);
                                        }

                                        // 获取成功
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    DWORD _uRemoteProcessSystemDpi = 0;

                    UnicodeStringBuffer<MAX_PATH> _szRundll32PathBuffer;
                    _szRundll32PathBuffer.SetLength(GetSystemDirectoryW(_szRundll32PathBuffer.Buffer, _szRundll32PathBuffer.kcchStaticBuffer));
                    _szRundll32PathBuffer.AppendPath(L"rundll32.exe");

                    STARTUPINFOW _StartupInfo = { sizeof(_StartupInfo) };
                    PROCESS_INFORMATION _oProcessInformation;
                    BOOL _bRet;
                    {
                        AutoEnalbeFsRedirection _oEnalbeFsRedirectionCreateProcessW;
                        _bRet = CreateProcessW(_szRundll32PathBuffer.Buffer, nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &_StartupInfo, &_oProcessInformation);
                    }
                    if (_bRet)
                    {
                        CloseHandle(_oProcessInformation.hThread);

                        RemoteGetSystemDpiBuffer _uBuffer;
                        if (_uBuffer.Init())
                        {
                            if (auto _pRemoteData = (RemoteGetSystemDpiBuffer*)VirtualAllocEx(_oProcessInformation.hProcess, nullptr, sizeof(_uBuffer), MEM_COMMIT, PAGE_EXECUTE_READWRITE))
                            {
                                WriteProcessMemory(_oProcessInformation.hProcess, _pRemoteData, &_uBuffer, sizeof(_uBuffer), nullptr);
                                auto _hRemoteThread = CreateRemoteThread(_oProcessInformation.hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)&_pRemoteData->ShellCode, &_pRemoteData->ShellCodeParameter, 0, nullptr);
                                if (_hRemoteThread)
                                {
                                    if (WaitForSingleObject(_hRemoteThread, 1000) == WAIT_OBJECT_0)
                                    {
                                        GetExitCodeThread(_hRemoteThread, &_uRemoteProcessSystemDpi);
                                    }

                                    CloseHandle(_hRemoteThread);
                                }
                                VirtualFreeEx(_oProcessInformation.hProcess, _pRemoteData, 0, MEM_RELEASE);
                            }
                        }
                        TerminateProcess(_oProcessInformation.hProcess, -1);
                        CloseHandle(_oProcessInformation.hProcess);
                    }

                    if (_uRemoteProcessSystemDpi >= USER_DEFAULT_SCREEN_DPI && _uRemoteProcessSystemDpi <= USER_DEFAULT_SCREEN_DPI * 5)
                    {
                        _uDpi = _uRemoteProcessSystemDpi;
                        break;
                    }

                    // 暂时没办法了……先按Windows 7的方法取以下再说。对于大多数用户来说，一般不会修改屏幕Dpi。基本也是可信的。
#else
                    uint32_t _uSystemDpiOffset = 0;
                    auto _pGpsi = GetUser32Gpsi();
                    if (!_pGpsi)
                    {
                    }
                    else if (internal::GetSystemVersion() < __Version(6, 4, 0, 0))
                    {
                        // win32k.sys MonitorDpiFromProcess
                        // 6.3.9600.16384
                        switch (GetNativeSystemInfo().wProcessorArchitecture)
                        {
                        case PROCESSOR_ARCHITECTURE_AMD64:
                            _uSystemDpiOffset = 0x1AE6ul;
                            break;
                        case PROCESSOR_ARCHITECTURE_INTEL:
                            _uSystemDpiOffset = 0x18B6ul;
                            break;
                        default:
                            break;
                        }
                    }
                    else if (internal::GetSystemVersion() >= __Version(10, 0, 10240, 0) && internal::GetSystemVersion() <= __Version(10, 0, 10586, 0))
                    {
                        // win32kbase.sys MonitorDpiFromProcess
                        switch (GetNativeSystemInfo().wProcessorArchitecture)
                        {
                        case PROCESSOR_ARCHITECTURE_AMD64:
                            _uSystemDpiOffset = 0x1C76ul;
                            break;
                        case PROCESSOR_ARCHITECTURE_INTEL:
                            _uSystemDpiOffset = 0x1A3Eul;
                            break;
                        default:
                            break;
                        }
                    }
                    else if (internal::GetSystemVersion() == __Version(10, 0, 14393, 0))
                    {
                        // win32kfull.sys GetDpiForSystem
                        switch (GetNativeSystemInfo().wProcessorArchitecture)
                        {
                        case PROCESSOR_ARCHITECTURE_AMD64:
                            _uSystemDpiOffset = 0x21E6ul;
                            break;
                        case PROCESSOR_ARCHITECTURE_INTEL:
                            _uSystemDpiOffset = 0x1FB6ul;
                            break;
                        default:
                            break;
                        }
                    }
                    else if (internal::GetSystemVersion() == __Version(10, 0, 15063, 0))
                    {
                        // win32kfull.sys GetDpiForSystem
                        switch (GetNativeSystemInfo().wProcessorArchitecture)
                        {
                        case PROCESSOR_ARCHITECTURE_AMD64:
                            _uSystemDpiOffset = 0x21E6ul;
                            break;
                        case PROCESSOR_ARCHITECTURE_INTEL:
                            _uSystemDpiOffset = 0x1F9Eul;
                            break;
                        default:
                            break;
                        }
                    }

                    if (_uSystemDpiOffset)
                    {
                        _uDpi = *(uint16_t*)(_pGpsi + _uSystemDpiOffset);
                        break;
                    }
#endif
                }

                // [,6.3)：这类系统所有屏幕的Dpi == SystemDpi
                HMONITOR _hMonitor = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
                if (!_hMonitor)
                    return _uDpi;

                MONITORINFOEXW _oMonitorInfo = { {sizeof(_oMonitorInfo)} };
                DEVMODEW _oDevMode = {};
                _oDevMode.dmSize = sizeof(_oDevMode);
                if (GetMonitorInfoW(_hMonitor, &_oMonitorInfo) == FALSE || EnumDisplaySettingsW(_oMonitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &_oDevMode) == FALSE)
                    return _uDpi;

                // MonitorDpi
                _uDpi = MulDiv(_oDevMode.dmPelsHeight, USER_DEFAULT_SCREEN_DPI, _oMonitorInfo.rcMonitor.bottom - _oMonitorInfo.rcMonitor.top);
            } while (false);

            s_uGlobalSystemDpi = _uDpi;
            return _uDpi;
        }
#endif

#if (YY_Thunks_Target < __WindowsNT6_3)
        static uint32_t __fastcall GetWindowDpiForLessThanNt6_3(_In_ HWND _hWnd)
        {
            // (,6.2]：只存在一个系统Dpi，所以如果系统Dpi == USER_DEFAULT_SCREEN_DPI，那么不可能存在缩放
            const auto _uGlobalSystemDpi = GetDpiForGlobalSystemDownlevel();
            if (_uGlobalSystemDpi == USER_DEFAULT_SCREEN_DPI)
                return USER_DEFAULT_SCREEN_DPI;

            // Windows XP没有Dpi缩放，所以直接返回即可。
            if (internal::GetSystemVersion() < __WindowsNT6)
                return _uGlobalSystemDpi;

            RECT _oWindowRect;
            if (!GetWindowRect(_hWnd, &_oWindowRect))
                return 0;

            RECT _oWindowRect2 = _oWindowRect;
            if (IsProcessDPIAware())
            {
#if (YY_Thunks_Target < __WindowsNT6)
                const auto PhysicalToLogicalPoint = try_get_PhysicalToLogicalPoint();
#endif
                if (!PhysicalToLogicalPoint(_hWnd, LPPOINT(&_oWindowRect2.right)))
                    return 0;

                if (_oWindowRect.right != _oWindowRect2.right || _oWindowRect.bottom != _oWindowRect2.bottom)
                    return USER_DEFAULT_SCREEN_DPI;

                if (!PhysicalToLogicalPoint(_hWnd, LPPOINT(&_oWindowRect2.left)))
                    return 0;

                if (_oWindowRect.left != _oWindowRect2.left || _oWindowRect.top != _oWindowRect2.top)
                    return USER_DEFAULT_SCREEN_DPI;

                if ((_oWindowRect2.right - _oWindowRect2.left) >= 4 || (_oWindowRect2.bottom - _oWindowRect2.top) >= 4)
                    return _uGlobalSystemDpi;
            }
            else
            {
#if (YY_Thunks_Target < __WindowsNT6)
                const auto LogicalToPhysicalPoint = try_get_LogicalToPhysicalPoint();
#endif
                if (!LogicalToPhysicalPoint(_hWnd, LPPOINT(&_oWindowRect2.right)))
                    return 0;

                if (_oWindowRect.right != _oWindowRect2.right || _oWindowRect.bottom != _oWindowRect2.bottom)
                    return USER_DEFAULT_SCREEN_DPI;

                if (!LogicalToPhysicalPoint(_hWnd, LPPOINT(&_oWindowRect2.left)))
                    return 0;

                if (_oWindowRect.left != _oWindowRect2.left || _oWindowRect.top != _oWindowRect2.top)
                    return USER_DEFAULT_SCREEN_DPI;

                if ((_oWindowRect.right - _oWindowRect.left) >= 4 || (_oWindowRect.bottom - _oWindowRect.top) >= 4)
                    return _uGlobalSystemDpi;
            }

            return 0;
        }

        static uint32_t __fastcall GetProcessDpiForLessThanNt6_3(_In_ DWORD _uPrcessId)
        {
            const auto _uGlobalSystemDpi = GetDpiForGlobalSystemDownlevel();
            if (_uGlobalSystemDpi == USER_DEFAULT_SCREEN_DPI)
                return USER_DEFAULT_SCREEN_DPI;

            if (internal::GetSystemVersion() < __WindowsNT6)
                return _uGlobalSystemDpi;

            struct EnumData
            {
                DWORD uProcessId = 0;
                uint32_t uProcessDpi = 0;
            };

            EnumData _oData = { _uPrcessId };
            EnumWindows(
                [](HWND _hWnd, LPARAM _lParam) -> BOOL
                {
                    auto _pData = (EnumData*)_lParam;
                    if (!_hWnd)
                        return TRUE;

                    DWORD _uTargrtPocressId;
                    if (GetWindowThreadProcessId(_hWnd, &_uTargrtPocressId) == 0)
                        return TRUE;

                    if (_uTargrtPocressId != _pData->uProcessId)
                        return TRUE;

                    _pData->uProcessDpi = GetWindowDpiForLessThanNt6_3(_hWnd);
                    return _pData->uProcessDpi ? FALSE : TRUE;
                },
                LPARAM(&_oData));

            return _oData.uProcessDpi;
        }
#endif
    }
}
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows XP with SP2, Windows Server 2003 with SP1
    __DEFINE_THUNK(
    user32,
    0,
    BOOL,
    WINAPI,
    IsWow64Message,
        VOID
        )
    {
        if (auto const pIsWow64Message = try_get_IsWow64Message())
        {
            return pIsWow64Message();
        }

        return FALSE;
    }
#endif
        

#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    0,
    BOOL,
    WINAPI,
    IsProcessDPIAware,
        void)
    {
        if (const auto _pfnIsProcessDPIAware = try_get_IsProcessDPIAware())
        {
            return _pfnIsProcessDPIAware();
        }

        // 2025年3月11日实验结果 XP始终发生DPI感知
        // 我们报告API并没有什么问题。
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    0,
    BOOL,
    WINAPI,
    SetProcessDPIAware,
        VOID
        )
    {
        if (auto const pSetProcessDPIAware = try_get_SetProcessDPIAware())
        {
            return pSetProcessDPIAware();
        }

        // 2025年3月11日实验结果 XP始终发生DPI感知
        // 所以我们返回成功没有啥问题。
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_15063)

    //Windows 10, version 1703 [desktop apps only]
    //Windows Server 2016 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    SetProcessDpiAwarenessContext,
        _In_ DPI_AWARENESS_CONTEXT value
        )
    {
        if (auto const pSetProcessDpiAwarenessContext = try_get_SetProcessDpiAwarenessContext())
        {
            return pSetProcessDpiAwarenessContext(value);
        }

        LSTATUS lStatus;

        do
        {
            PROCESS_DPI_AWARENESS DpiAwareness;

            if (DPI_AWARENESS_CONTEXT_UNAWARE == value
                || DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED == value)
            {
                DpiAwareness = PROCESS_DPI_UNAWARE;
            }
            else if (DPI_AWARENESS_CONTEXT_SYSTEM_AWARE == value)
            {
                DpiAwareness = PROCESS_SYSTEM_DPI_AWARE;
            }
            else if (DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE == value
                || DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 == value)
            {
                DpiAwareness = PROCESS_PER_MONITOR_DPI_AWARE;
            }
            else
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            auto hr = SetProcessDpiAwareness(DpiAwareness);

            if (SUCCEEDED(hr))
            {
                return TRUE;
            }

            //将 HRESULT 错误代码转换到 LSTATUS
            if (hr & 0xFFFF0000)
            {
                if (HRESULT_FACILITY(hr) == FACILITY_WIN32)
                {
                    lStatus = HRESULT_CODE(hr);
                }
                else
                {
                    lStatus = ERROR_FUNCTION_FAILED;
                }
            }
            else
            {
                lStatus = hr;
            }

        } while (false);

            
        SetLastError(lStatus);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    //Windows 10, version 1607 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    0,
    UINT,
    WINAPI,
    GetDpiForSystem,
        VOID
        )
    {
        if (auto const pGetDpiForSystem = try_get_GetDpiForSystem())
        {
            return pGetDpiForSystem();
        }

        return internal::GetDpiForSystemDownlevel();
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    //Windows 10, version 1607 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    4,
    UINT,
    WINAPI,
    GetDpiForWindow,
        _In_ HWND _hWnd
        )
    {
        if (auto const _pfnGetDpiForWindow = try_get_GetDpiForWindow())
        {
            return _pfnGetDpiForWindow(_hWnd);
        }

        do
        {
            DWORD _uTargrtPocressId;
            if (GetWindowThreadProcessId(_hWnd, &_uTargrtPocressId) == 0)
                break;

            if (_uTargrtPocressId == (ULONG)NtCurrentTeb()->ClientId.UniqueProcess)
            {
#if (YY_Thunks_Target < __WindowsNT6_3)
                const auto GetDpiForMonitor = try_get_GetDpiForMonitor();
                if (GetDpiForMonitor)
#endif
                {
                    HMONITOR _hMonitor = MonitorFromWindow(_hWnd, MONITOR_DEFAULTTOPRIMARY);
                    if (_hMonitor)
                    {
                        UINT _uDpiX, _uDpiY;
                        if (SUCCEEDED(GetDpiForMonitor(_hMonitor, MDT_EFFECTIVE_DPI, &_uDpiX, &_uDpiY)))
                        {
                            return _uDpiX;
                        }
                    }
                }

                return internal::GetDpiForSystemDownlevel();
            }
#if (YY_Thunks_Target < __WindowsNT6_3)
            else if (internal::GetSystemVersion() < __WindowsNT6_3)
            {
                const auto _uWindowDpi = internal::GetWindowDpiForLessThanNt6_3(_hWnd);
                return _uWindowDpi ? _uWindowDpi : USER_DEFAULT_SCREEN_DPI;
            }
#endif
            else
            {
                // >= __WindowsNT6_3
#if (YY_Thunks_Target < __WindowsNT6_3)
                const auto GetProcessDpiAwareness = try_get_GetProcessDpiAwareness();
                const auto GetDpiForMonitor = try_get_GetDpiForMonitor();
#endif
                PROCESS_DPI_AWARENESS _eCurrentPocressDpiAwareness;
                if (FAILED(GetProcessDpiAwareness(nullptr, &_eCurrentPocressDpiAwareness)))
                    break;

                auto _hTargrtPocress = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, _uTargrtPocressId);
                if (!_hTargrtPocress)
                    break;

                PROCESS_DPI_AWARENESS _eTargrtPocressDpiAwareness;
                auto _hr = GetProcessDpiAwareness(_hTargrtPocress, &_eTargrtPocressDpiAwareness);
                CloseHandle(_hTargrtPocress);
                if (FAILED(_hr))
                    break;

                if (_eTargrtPocressDpiAwareness == _eCurrentPocressDpiAwareness)
                {
                    if (_eCurrentPocressDpiAwareness == PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE)
                    {
                        HMONITOR _hMonitor = MonitorFromWindow(_hWnd, MONITOR_DEFAULTTOPRIMARY);
                        if (_hMonitor)
                        {
                            UINT _uDpiX, _uDpiY;
                            if (SUCCEEDED(GetDpiForMonitor(_hMonitor, MDT_EFFECTIVE_DPI, &_uDpiX, &_uDpiY)))
                            {
                                return _uDpiX;
                            }
                        }
                    }

                    return internal::GetDpiForSystemDownlevel();
                }
                else if (_eTargrtPocressDpiAwareness == PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE)
                {
                    const auto _uBaseDpi = internal::GetDpiForSystemDownlevel();
                    HMONITOR _hMonitor = MonitorFromWindow(_hWnd, MONITOR_DEFAULTTOPRIMARY);
                    if (_hMonitor)
                    {
                        MONITORINFOEXW _oMonitorInfo = { {sizeof(_oMonitorInfo)} };
                        DEVMODEW _oDevMode = {};
                        _oDevMode.dmSize = sizeof(_oDevMode);
                        if (GetMonitorInfoW(_hMonitor, &_oMonitorInfo) && EnumDisplaySettingsW(_oMonitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &_oDevMode))
                        {
                            return _oDevMode.dmPelsHeight * _uBaseDpi / (_oMonitorInfo.rcMonitor.bottom - _oMonitorInfo.rcMonitor.top);
                        }
                    }

                    return _uBaseDpi;
                }
                else if (_eTargrtPocressDpiAwareness == PROCESS_DPI_AWARENESS::PROCESS_SYSTEM_DPI_AWARE)
                {
                    return internal::GetDpiForGlobalSystemDownlevel(_hWnd);
                }
                else
                {
                    // _eTargrtPocressDpiAwareness == PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE
                    return USER_DEFAULT_SCREEN_DPI;
                }
            }
        } while (false);

        SetLastError(ERROR_INVALID_PARAMETER);
        return 0ul;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    //Windows 10, version 1607 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    8,
    int,
    WINAPI,
    GetSystemMetricsForDpi,
        _In_ int _nIndex,
        _In_ UINT _uDpi
        )
    {
        if (auto const _pfnGetSystemMetricsForDpi = try_get_GetSystemMetricsForDpi())
        {
            return _pfnGetSystemMetricsForDpi(_nIndex, _uDpi);
        }

        if (_uDpi == 0ul)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }

        auto _nValue = GetSystemMetrics(_nIndex);
        if (_nValue != 0)
        {
            switch (_nIndex)
            {
            case SM_CXVSCROLL:
            case SM_CYHSCROLL:
            case SM_CYCAPTION: // 4

            case SM_CYVTHUMB: // 9
            case SM_CXHTHUMB:
            case SM_CXICON:
            case SM_CYICON:
            case SM_CXCURSOR:
            case SM_CYCURSOR:
            case SM_CYMENU: // 15

            case SM_CYVSCROLL: // 20
            case SM_CXHSCROLL:

            case SM_CXMIN: // 28
            case SM_CYMIN:
            case SM_CXSIZE:
            case SM_CYSIZE:
            case SM_CXMINTRACK:
            case SM_CYMINTRACK: //35

            case SM_CXICONSPACING: //38
            case SM_CYICONSPACING: // 39

            case SM_CXSMICON: // 49
            case SM_CYSMICON:
            case SM_CYSMCAPTION:
            case SM_CXSMSIZE:
            case SM_CYSMSIZE:
            case SM_CXMENUSIZE:
            case SM_CYMENUSIZE: // 55

            case SM_CXMENUCHECK: // 71
            case SM_CYMENUCHECK: // 72

            case SM_CXPADDEDBORDER: // 92
            {
                const auto _uBaseDpi = internal::GetDpiForSystemDownlevel();
                if (_uBaseDpi != _uDpi)
                {
                    int _nDelta = 0;
                    switch (_nIndex)
                    {
                    case SM_CYCAPTION: // 4
                    case SM_CYMENU: // 15
                    case SM_CYSMCAPTION: // 51
                        _nDelta = GetSystemMetrics(SM_CYBORDER);
                        break;
                    default:
                        __WarningMessage__("SM_CXMIN、SM_CYMIN、SM_CXMINTRACK、SM_CYMINTRACK、SM_CXMENUCHECK、SM_CYMENUCHECK 存在一定偏差。");
                        break;
                    }
                    _nValue = MulDiv(_nValue - _nDelta, _uDpi, _uBaseDpi) + _nDelta;
                }

                break;
            }
            case SM_CXFRAME:
            {
                const auto _nBorreder = GetSystemMetrics(SM_CXBORDER);
                _nValue -= 2 * _nBorreder;
                _nValue = MulDiv(_nValue, _uDpi, USER_DEFAULT_SCREEN_DPI) + _nBorreder;
                break;
            }
            case SM_CYFRAME:
            {
                const auto _nBorreder = GetSystemMetrics(SM_CYBORDER);
                _nValue -= 2 * _nBorreder;
                _nValue = MulDiv(_nValue, _uDpi, USER_DEFAULT_SCREEN_DPI) + _nBorreder;
                break;
            }
            }
        }

        return _nValue;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    //Windows 10, version 1607 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    20,
    BOOL,
    WINAPI,
    AdjustWindowRectExForDpi,
        _Inout_ LPRECT _pRect,
        _In_ DWORD _fStyle,
        _In_ BOOL _bMenu,
        _In_ DWORD _fExStyle,
        _In_ UINT _uDpi
        )
    {
        if (auto const _pfnAdjustWindowRectExForDpi = try_get_AdjustWindowRectExForDpi())
        {
            return _pfnAdjustWindowRectExForDpi(_pRect, _fStyle, _bMenu, _fExStyle, _uDpi);
        }

        if (_uDpi == 0ul)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        RECT _DeltaRect = {};
        if (!AdjustWindowRectEx(&_DeltaRect, _fStyle, _bMenu, _fExStyle))
        {
            return FALSE;
        }

        const auto _uBaseDpi = internal::GetDpiForSystemDownlevel();
        if (_uBaseDpi != _uDpi)
        {
            RECT _DlgFrameRect = {};
            if (_fStyle & (WS_THICKFRAME | WS_DLGFRAME))
            {
                AdjustWindowRectEx(&_DlgFrameRect, WS_DLGFRAME, FALSE, 0);
                if (_bMenu || (_fStyle & WS_CAPTION) == WS_CAPTION)
                {
                    const auto _nSystemBorderWidth = GetSystemMetrics(SM_CXBORDER);
                    _DlgFrameRect.top -= _nSystemBorderWidth;
                }
            }
            else if (_fStyle & WS_BORDER)
            {
                AdjustWindowRectEx(&_DlgFrameRect, WS_BORDER, FALSE, 0);
                if (_bMenu || (_fStyle & WS_CAPTION) == WS_CAPTION)
                {
                    // 特殊优化，不在调用 GetSystemMetrics(SM_CXBORDER)
                    // 毕竟理论上 _DlgFrameRect.top == -GetSystemMetrics(SM_CXBORDER)
                    _DlgFrameRect.top += _DlgFrameRect.top;
                }
            }

            _DeltaRect.left -= _DlgFrameRect.left;
            _DeltaRect.top -= _DlgFrameRect.top;
            _DeltaRect.right -= _DlgFrameRect.right;
            _DeltaRect.bottom -= _DlgFrameRect.bottom;

            _DeltaRect.left = MulDiv(_DeltaRect.left, _uDpi, _uBaseDpi) + _DlgFrameRect.left;
            _DeltaRect.top = MulDiv(_DeltaRect.top, _uDpi, _uBaseDpi) + _DlgFrameRect.top;
            _DeltaRect.right = MulDiv(_DeltaRect.right, _uDpi, _uBaseDpi) + _DlgFrameRect.right;
            _DeltaRect.bottom = MulDiv(_DeltaRect.bottom, _uDpi, _uBaseDpi) + _DlgFrameRect.bottom;
        }

        _pRect->left += _DeltaRect.left;
        _pRect->top += _DeltaRect.top;
        _pRect->right += _DeltaRect.right;
        _pRect->bottom += _DeltaRect.bottom;
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    //Windows 10, version 1607 [desktop apps only]
    __DEFINE_THUNK(
    user32,
    20,
    BOOL,
    WINAPI,
    SystemParametersInfoForDpi,
        _In_ UINT _uAction,
        _In_ UINT _uParam,
        _Pre_maybenull_ _Post_valid_ PVOID _pParam,
        _In_ UINT _fWinIni,
        _In_ UINT _uDpi
        )
    {
        if (auto const _pfnSystemParametersInfoForDpi = try_get_SystemParametersInfoForDpi())
            return _pfnSystemParametersInfoForDpi(_uAction, _uParam, _pParam, _fWinIni, _uDpi);

        // SystemParametersInfoW 函数始终拿到的是 DPI = _uBaseDpi 的情况
        // 其结果与当前系统DPI值无关。
        // 我们可以通过缩放来模拟 SystemParametersInfoForDpi
        const auto _uBaseDpi = internal::GetDpiForSystemDownlevel();

        switch (_uAction)
        {
        case SPI_GETICONTITLELOGFONT:
        {
            if (!SystemParametersInfoW(_uAction, _uParam, _pParam, _fWinIni))
                return FALSE;

            if(_pParam == nullptr || _uDpi == 0 || _uDpi == _uBaseDpi)
                return TRUE;

            auto _pInfo = (LOGFONTW*)_pParam;
            _pInfo->lfHeight = MulDiv(_pInfo->lfHeight, _uDpi, _uBaseDpi);
            return TRUE;
        }
        case SPI_GETICONMETRICS:
        {
            if (!SystemParametersInfoW(_uAction, _uParam, _pParam, _fWinIni))
                return FALSE;

            if (_pParam == nullptr || _uDpi == 0 || _uDpi == _uBaseDpi)
                return TRUE;

            auto _pInfo = (ICONMETRICSW*)_pParam;
            _pInfo->iHorzSpacing = MulDiv(_pInfo->iHorzSpacing, _uDpi, _uBaseDpi);
            _pInfo->iVertSpacing = MulDiv(_pInfo->iVertSpacing, _uDpi, _uBaseDpi);
            _pInfo->lfFont.lfHeight = MulDiv(_pInfo->lfFont.lfHeight, _uDpi, _uBaseDpi);
            return TRUE;
        }
        case SPI_GETNONCLIENTMETRICS:
        {
            // 微软强制要求Size等于 sizeof(NONCLIENTMETRICSW)
            auto _pInfo = (NONCLIENTMETRICSW*)_pParam;
            if (_pInfo == nullptr || _pInfo->cbSize != sizeof(NONCLIENTMETRICSW))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            if (!SystemParametersInfoW(_uAction, _uParam, _pParam, _fWinIni))
                return FALSE;

            if (_pParam == nullptr || _uDpi == 0)
                return TRUE;

            // GetSystemMetrics(SM_CYBORDER) 的大小始终跟 Dpi无关
            const auto _nSystemBorderWidth = MulDiv(GetSystemMetrics(SM_CYBORDER), _uDpi, USER_DEFAULT_SCREEN_DPI);
            if (_uDpi == _uBaseDpi)
            {
                // 如果DPI恰好 == _uBaseDpi，这时可以优化，避免无意义的MulDiv调用。
                // 注意，主动传入DPI时，iBorderWidth默认为 GetSystemMetrics(SM_CYBORDER)。
                const auto _nBorderWidth = _pInfo->iBorderWidth + _pInfo->iPaddedBorderWidth;
                if (_nBorderWidth > 0)
                {
                    _pInfo->iPaddedBorderWidth = _nBorderWidth - _nSystemBorderWidth;
                    _pInfo->iBorderWidth = _nSystemBorderWidth;
                }
                return TRUE;
            }

            // 跳过 iBorderWidth，通过后续的iPaddedBorderWidth调整。
            // _pInfo->iBorderWidth = MulDiv(_pInfo->iBorderWidth, _uDpi, kBaseDpi);
            _pInfo->iScrollWidth = MulDiv(_pInfo->iScrollWidth, _uDpi, _uBaseDpi);
            _pInfo->iScrollHeight = MulDiv(_pInfo->iScrollHeight, _uDpi, _uBaseDpi);
            _pInfo->iCaptionWidth = MulDiv(_pInfo->iCaptionWidth, _uDpi, _uBaseDpi);
            _pInfo->iCaptionHeight = MulDiv(_pInfo->iCaptionHeight, _uDpi, _uBaseDpi);
            _pInfo->lfCaptionFont.lfHeight = MulDiv(_pInfo->lfCaptionFont.lfHeight, _uDpi, _uBaseDpi);
            _pInfo->iSmCaptionWidth = MulDiv(_pInfo->iSmCaptionWidth, _uDpi, _uBaseDpi);
            _pInfo->iSmCaptionHeight = MulDiv(_pInfo->iSmCaptionHeight, _uDpi, _uBaseDpi);
            _pInfo->lfSmCaptionFont.lfHeight = MulDiv(_pInfo->lfSmCaptionFont.lfHeight, _uDpi, _uBaseDpi);
            _pInfo->iMenuWidth = MulDiv(_pInfo->iMenuWidth, _uDpi, _uBaseDpi);
            _pInfo->iMenuHeight = MulDiv(_pInfo->iMenuHeight, _uDpi, _uBaseDpi);
            _pInfo->lfMenuFont.lfHeight = MulDiv(_pInfo->lfMenuFont.lfHeight, _uDpi, _uBaseDpi);
            _pInfo->lfStatusFont.lfHeight = MulDiv(_pInfo->lfStatusFont.lfHeight, _uDpi, _uBaseDpi);
            _pInfo->lfMessageFont.lfHeight = MulDiv(_pInfo->lfMessageFont.lfHeight, _uDpi, _uBaseDpi);

            const auto _nBorderWidth = _pInfo->iBorderWidth + _pInfo->iPaddedBorderWidth;
            if (_nBorderWidth > 0)
            {
                _pInfo->iPaddedBorderWidth = MulDiv(_nBorderWidth, _uDpi, _uBaseDpi) - _nSystemBorderWidth;
                _pInfo->iBorderWidth = _nSystemBorderWidth;
            }
            return TRUE;
        }
        }

        // 微软原版这时不会返回错误代码，但是总感觉是微软的Bug
        // 我们这里就随便返回一个错误代码吧。
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    GetWindowDisplayAffinity,
        _In_ HWND _hWnd,
        _Out_ DWORD* _pdwAffinity)
    {
        if (auto const _pfnGetWindowDisplayAffinity = try_get_GetWindowDisplayAffinity())
        {
            return _pfnGetWindowDisplayAffinity(_hWnd, _pdwAffinity);
        }

        if (!_pdwAffinity)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        // 系统不支持，假装自己不需要任何保护
        *_pdwAffinity = WDA_NONE;
        return TRUE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    SetWindowDisplayAffinity,
        _In_ HWND _hWnd,
        _In_ DWORD _dwAffinity)
    {
        if (auto const _pfnSetWindowDisplayAffinity = try_get_SetWindowDisplayAffinity())
        {
            return _pfnSetWindowDisplayAffinity(_hWnd, _dwAffinity);
        }

        // 系统不支持，假装自己成功……
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    RegisterTouchWindow,
        _In_ HWND _hWnd,
        _In_ ULONG _ulFlags)
    {
        if (auto const _pfnRegisterTouchWindow = try_get_RegisterTouchWindow())
        {
            return _pfnRegisterTouchWindow(_hWnd, _ulFlags);
        }

        return TRUE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    UnregisterTouchWindow,
        _In_ HWND _hWnd)
    {
        if (auto const _pfnUnregisterTouchWindow = try_get_UnregisterTouchWindow())
        {
            return _pfnUnregisterTouchWindow(_hWnd);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    IsTouchWindow,
        _In_ HWND _hWnd,
        _Out_opt_ PULONG _puFlags)
    {
        if (auto const _pfnIsTouchWindow = try_get_IsTouchWindow())
        {
            return _pfnIsTouchWindow(_hWnd, _puFlags);
        }

        if (_puFlags)
            *_puFlags = 0;

        return TRUE;
    }
#endif
        

#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    16,
    BOOL,
    WINAPI,
    GetTouchInputInfo,
        _In_ HTOUCHINPUT _hTouchInput,
        _In_ UINT _uInputs,
        _Out_writes_(_uInputs) PTOUCHINPUT _pInputs,
        _In_ int _cbSize)
    {
        if (auto const _pfnGetTouchInputInfo = try_get_GetTouchInputInfo())
        {
            return _pfnGetTouchInputInfo(_hTouchInput, _uInputs, _pInputs, _cbSize);
        }

        // 老版本系统没有触摸消息，_hTouchInput 必然无效
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    CloseTouchInputHandle,
        _In_ HTOUCHINPUT _hTouchInput)
    {
        if (auto const _pfnCloseTouchInputHandle = try_get_CloseTouchInputHandle())
        {
            return _pfnCloseTouchInputHandle(_hTouchInput);
        }

        // 老版本系统没有触摸消息，_hTouchInput 必然无效
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    ChangeWindowMessageFilter,
        _In_ UINT _uMessage,
        _In_ DWORD _fFlag
        )
    {
        if (const auto _pfnChangeWindowMessageFilter = try_get_ChangeWindowMessageFilter())
        {
            return _pfnChangeWindowMessageFilter(_uMessage, _fFlag);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    UpdateLayeredWindowIndirect,
        _In_ HWND hWnd,
        _In_ const UPDATELAYEREDWINDOWINFO* pULWInfo
        )
    {
        if (const auto _pfnUpdateLayeredWindowIndirect = try_get_UpdateLayeredWindowIndirect())
        {
            return _pfnUpdateLayeredWindowIndirect(hWnd, pULWInfo);
        }

        return UpdateLayeredWindow(
            hWnd,
            pULWInfo->hdcDst,
            const_cast<POINT*>(pULWInfo->pptDst),
            const_cast<SIZE*>(pULWInfo->psize),
            pULWInfo->hdcSrc,
            const_cast<POINT*>(pULWInfo->pptSrc),
            pULWInfo->crKey,
            const_cast<BLENDFUNCTION*>(pULWInfo->pblend), pULWInfo->dwFlags);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows 2000 Professional [仅限桌面应用]
    // 最低受支持的服务器	Windows 2000 Server[仅限桌面应用]
    // 虽然 2000就支持，但是 SPI_GETNONCLIENTMETRICS 需要特殊处理。
    __DEFINE_THUNK(
    user32,
    16,
    BOOL,
    WINAPI,
    SystemParametersInfoW,
        _In_ UINT _uAction,
        _In_ UINT _uParam,
        _Pre_maybenull_ _Post_valid_ PVOID _pParam,
        _In_ UINT _fWinIni
        )
    {
        const auto _pfnSystemParametersInfoW = try_get_SystemParametersInfoW();
        if (!_pfnSystemParametersInfoW)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return FALSE;
        }

        if (_pParam && internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            if (SPI_GETNONCLIENTMETRICS == _uAction)
            {
                // https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/ns-winuser-nonclientmetricsw#remarks
                // Windows XP等系统必须减去iPaddedBorderWidth大小。
                auto _pInfo = (NONCLIENTMETRICSW*)_pParam;
                if (_pInfo->cbSize == sizeof(NONCLIENTMETRICSW))
                {
                    _pInfo->cbSize = RTL_SIZEOF_THROUGH_FIELD(NONCLIENTMETRICSW, lfMessageFont);
                    // XP不支持iPaddedBorderWidth，暂时设置为 0，如果有更佳值可以PR。
                    _pInfo->iPaddedBorderWidth = 0;
                    auto _bRet = _pfnSystemParametersInfoW(_uAction, RTL_SIZEOF_THROUGH_FIELD(NONCLIENTMETRICSW, lfMessageFont), _pParam, _fWinIni);
                    // 重新恢复其大小
                    _pInfo->cbSize = sizeof(NONCLIENTMETRICSW);
                    return _bRet;
                }
            }
        }
        
        return _pfnSystemParametersInfoW(_uAction, _uParam, _pParam, _fWinIni);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows 2000 Professional [仅限桌面应用]
    // 最低受支持的服务器	Windows 2000 Server[仅限桌面应用]
    // 虽然 2000就支持，但是 SPI_GETNONCLIENTMETRICS 需要特殊处理。
    __DEFINE_THUNK(
    user32,
    16,
    BOOL,
    WINAPI,
    SystemParametersInfoA,
        _In_ UINT _uAction,
        _In_ UINT _uParam,
        _Pre_maybenull_ _Post_valid_ PVOID _pParam,
        _In_ UINT _fWinIni
        )
    {
        const auto _pfnSystemParametersInfoA = try_get_SystemParametersInfoA();
        if (!_pfnSystemParametersInfoA)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return FALSE;
        }

#if (YY_Thunks_Target < __WindowsNT6)
        if (_pParam && internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            if (SPI_GETNONCLIENTMETRICS == _uAction)
            {
                // https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/ns-winuser-nonclientmetricsw#remarks
                // Windows XP等系统必须减去iPaddedBorderWidth大小。
                auto _pInfo = (NONCLIENTMETRICSA*)_pParam;
                if (_pInfo->cbSize == sizeof(NONCLIENTMETRICSA))
                {
                    _pInfo->cbSize = RTL_SIZEOF_THROUGH_FIELD(NONCLIENTMETRICSA, lfMessageFont);
                    _pInfo->iPaddedBorderWidth = 0;
                    auto _bRet = _pfnSystemParametersInfoA(_uAction, RTL_SIZEOF_THROUGH_FIELD(NONCLIENTMETRICSA, lfMessageFont), _pParam, _fWinIni);
                    // 重新恢复其大小
                    _pInfo->cbSize = sizeof(NONCLIENTMETRICSA);
                    return _bRet;
                }
            }
        }
#endif
        return _pfnSystemParametersInfoA(_uAction, _uParam, _pParam, _fWinIni);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    DPI_AWARENESS,
    WINAPI,
    GetAwarenessFromDpiAwarenessContext,
        _In_ DPI_AWARENESS_CONTEXT _hValue
        )
    {
        if (auto const _pfnGetAwarenessFromDpiAwarenessContext = try_get_GetAwarenessFromDpiAwarenessContext())
        {
            return _pfnGetAwarenessFromDpiAwarenessContext(_hValue);
        }

        if (_hValue == DPI_AWARENESS_CONTEXT_UNAWARE || _hValue == DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED)
        {
            return DPI_AWARENESS_UNAWARE;
        }
        else if (_hValue == DPI_AWARENESS_CONTEXT_SYSTEM_AWARE)
        {
            return DPI_AWARENESS_SYSTEM_AWARE;
        }
        else if (_hValue == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE || _hValue == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)
        {
            return DPI_AWARENESS_PER_MONITOR_AWARE;
        }
        else
        {
            return DPI_AWARENESS_INVALID;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    8,
    BOOL,
    WINAPI,
    AreDpiAwarenessContextsEqual,
        _In_ DPI_AWARENESS_CONTEXT _hDpiContextA,
        _In_ DPI_AWARENESS_CONTEXT _hDpiContextB
        )
    {
        if (auto const _pfnAreDpiAwarenessContextsEqual = try_get_AreDpiAwarenessContextsEqual())
        {
            return _pfnAreDpiAwarenessContextsEqual(_hDpiContextA, _hDpiContextB);
        }
        
        return _hDpiContextA == _hDpiContextB;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    EnableNonClientDpiScaling,
        _In_ HWND _hWnd
        )
    {
        if (auto const _pfnEnableNonClientDpiScaling = try_get_EnableNonClientDpiScaling())
        {
            return _pfnEnableNonClientDpiScaling(_hWnd);
        }
        
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    0,
    DPI_AWARENESS_CONTEXT,
    WINAPI,
    GetThreadDpiAwarenessContext
        )
    {
        if (auto const _pfnGetThreadDpiAwarenessContext = try_get_GetThreadDpiAwarenessContext())
        {
            return _pfnGetThreadDpiAwarenessContext();
        }
        
        PROCESS_DPI_AWARENESS _eValue = PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE;
        auto _hr = GetProcessDpiAwareness(NULL, &_eValue);
        if (SUCCEEDED(_hr))
        {
            switch (_eValue)
            {
            case PROCESS_DPI_UNAWARE:
                return DPI_AWARENESS_CONTEXT_UNAWARE;
            case PROCESS_SYSTEM_DPI_AWARE:
                return DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
            case PROCESS_PER_MONITOR_DPI_AWARE:
                return DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;
            default:
                break;
            }
        }

        return DPI_AWARENESS_CONTEXT_UNAWARE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    DPI_AWARENESS_CONTEXT,
    WINAPI,
    GetWindowDpiAwarenessContext,
        _In_ HWND _hWnd
        )
    {
        if (auto const _pfnGetWindowDpiAwarenessContext = try_get_GetWindowDpiAwarenessContext())
        {
            return _pfnGetWindowDpiAwarenessContext(_hWnd);
        }
        
        DWORD _uProcessId = 0;
        GetWindowThreadProcessId(_hWnd, &_uProcessId);
        if (_uProcessId == 0)
        {
            return NULL;
        }

        HANDLE _hProcess = NULL;
        if (_uProcessId != GetCurrentProcessId() && internal::GetSystemVersion() >= internal::MakeVersion(6, 3))
        {
            _hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, _uProcessId);
        }

        PROCESS_DPI_AWARENESS _eValue = PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE;
        auto _hr = GetProcessDpiAwareness(_hProcess, &_eValue);
        if (_hProcess)
        {
            CloseHandle(_hProcess);
        }

        if (SUCCEEDED(_hr))
        {
            switch (_eValue)
            {
            case PROCESS_DPI_UNAWARE:
                return DPI_AWARENESS_CONTEXT_UNAWARE;
            case PROCESS_SYSTEM_DPI_AWARE:
                return DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
            case PROCESS_PER_MONITOR_DPI_AWARE:
                return DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;
            default:
                break;
            }
        }

        return DPI_AWARENESS_CONTEXT_UNAWARE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    SetDisplayAutoRotationPreferences,
        _In_ ORIENTATION_PREFERENCE _eOrientation
        )
    {
        if (auto const _pfnSetDisplayAutoRotationPreferences = try_get_SetDisplayAutoRotationPreferences())
        {
            return _pfnSetDisplayAutoRotationPreferences(_eOrientation);
        }

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    user32,
    4,
    BOOL,
    WINAPI,
    GetDisplayAutoRotationPreferences,
        _Out_ ORIENTATION_PREFERENCE* _peOrientation
        )
    {
        if (auto const _pfnGetDisplayAutoRotationPreferences = try_get_GetDisplayAutoRotationPreferences())
        {
            return _pfnGetDisplayAutoRotationPreferences(_peOrientation);
        }

        *_peOrientation = ORIENTATION_PREFERENCE::ORIENTATION_PREFERENCE_NONE;
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_17134)

    // 最低受支持的客户端	Windows 10版本 1803 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2016[仅限桌面应用]
    __DEFINE_THUNK(
    user32,
    4,
    UINT,
    WINAPI,
    GetSystemDpiForProcess,
        _In_ HANDLE _hProcess
        )
    {
        if (auto const _pfnGetSystemDpiForProcess = try_get_GetSystemDpiForProcess())
        {
            return _pfnGetSystemDpiForProcess(_hProcess);
        }

        if (_hProcess == NULL || _hProcess == NtCurrentProcess())
            return GetDpiForSystem();

        const auto _uProcessId = GetProcessId(_hProcess);
        if (_uProcessId == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0ul;
        }

        if (_uProcessId == (ULONG)NtCurrentTeb()->ClientId.UniqueProcess)
            return GetDpiForSystem();

#if (YY_Thunks_Target < __WindowsNT6_3)
        if (internal::GetSystemVersion() < __WindowsNT6_3)
        {
            return internal::GetProcessDpiForLessThanNt6_3(_uProcessId);
        }
        else
#endif
        {
            __if_exists(try_get_GetProcessDpiAwareness)
            {
                const auto GetProcessDpiAwareness = try_get_GetProcessDpiAwareness();
            }
            PROCESS_DPI_AWARENESS _eValue;
            if (FAILED(GetProcessDpiAwareness(_hProcess, &_eValue)))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return 0;
            }

            if (_eValue == PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE)
                return USER_DEFAULT_SCREEN_DPI;

            return internal::GetDpiForGlobalSystemDownlevel();
        }
    }
#endif
} //namespace YY::Thunks
