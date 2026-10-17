// Standalone regression fixture; include the production entry, never a copy.
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>

#define __WindowsNT6 0x06000000
#define YY_Thunks_Target __WindowsNT6
#define YY_Thunks_Implemented

extern "C" uintptr_t __security_cookie;
extern "C" uintptr_t __security_cookie_complement;

namespace
{
#ifdef _WIN64
    constexpr uintptr_t DefaultCookie = 0x00002B992DDFA232;
#else
    constexpr uintptr_t DefaultCookie = 0xBB40E64E;
#endif

    struct State
    {
        unsigned InitCalls;
        unsigned UninitCalls;
        unsigned OriginalCalls;
        unsigned CustomCalls;
        uintptr_t InitCookie;
        uintptr_t CallbackCookie;
        HINSTANCE Instance;
        DWORD Reason;
        LPVOID Reserved;
        int Terminating;
    };

    State g_State = {};
    State* g_DetachState = nullptr;
    BOOL g_Result = TRUE;

    void RecordCallback(HINSTANCE _hInstance, DWORD _uReason, LPVOID _pReserved)
    {
        g_State.CallbackCookie = __security_cookie;
        g_State.Instance = _hInstance;
        g_State.Reason = _uReason;
        g_State.Reserved = _pReserved;
    }
}

namespace YY::Thunks::internal
{
    int __YY_Thunks_Process_Terminating;
    enum class ThunksInitStatus { InitByDllMain };
    extern "C" BOOL WINAPI YY_ThunksOriginalDllMainCRTStartup(HINSTANCE, DWORD, LPVOID);

    void _YY_initialize_winapi_thunks(ThunksInitStatus _Status)
    {
        if (_Status == ThunksInitStatus::InitByDllMain)
            ++g_State.InitCalls;
        g_State.InitCookie = __security_cookie;
    }

    void __YY_uninitialize_winapi_thunks()
    {
        ++g_State.UninitCalls;
        g_State.Terminating = __YY_Thunks_Process_Terminating;
        if (g_DetachState)
            *g_DetachState = g_State;
    }

#ifndef YY_STARTUP_TEST_DLL
    extern "C" BOOL WINAPI YY_ThunksOriginalDllMainCRTStartup(
        HINSTANCE _hInstance, DWORD _uReason, LPVOID _pReserved)
    {
        ++g_State.OriginalCalls;
        RecordCallback(_hInstance, _uReason, _pReserved);
        return g_Result;
    }

    BOOL WINAPI CustomStartup(HINSTANCE _hInstance, DWORD _uReason, LPVOID _pReserved)
    {
        ++g_State.CustomCalls;
        RecordCallback(_hInstance, _uReason, _pReserved);
        return g_Result;
    }
#endif

    extern "C" decltype(YY_ThunksOriginalDllMainCRTStartup)* const __pfnDllMainCRTStartupForYY_Thunks =
#ifdef YY_STARTUP_TEST_CUSTOM
        &CustomStartup;
#else
        nullptr;
#endif
}

#include "../../Thunks/DllMainCRTStartup.hpp"

#ifdef YY_STARTUP_TEST_DLL
BOOL WINAPI DllMain(HINSTANCE _hInstance, DWORD _uReason, LPVOID _pReserved)
{
    ++g_State.OriginalCalls;
    RecordCallback(_hInstance, _uReason, _pReserved);
    return TRUE;
}

extern "C" __declspec(dllexport) const State* __cdecl ReadState()
{
    return &g_State;
}

extern "C" __declspec(dllexport) void __cdecl ObserveDetach(State* _pState)
{
    g_DetachState = _pState;
}

#ifndef _WIN64
#pragma comment(linker, "/export:ReadState=_ReadState")
#pragma comment(linker, "/export:ObserveDetach=_ObserveDetach")
#endif
#else
#include <stdio.h>

// The Windows loader seeds cookies on modern Windows. Reset only in this
// safebuffers test frame, then restore before returning to the protected caller.
__declspec(noinline) __declspec(safebuffers) uintptr_t CallWithDefaultCookie(
    HINSTANCE _hInstance, DWORD _uReason, LPVOID _pReserved, BOOL* _pResult)
{
    const auto _Saved = __security_cookie;
    const auto _SavedComplement = __security_cookie_complement;
    __security_cookie = DefaultCookie;
    __security_cookie_complement = ~DefaultCookie;
    *_pResult = YY::Thunks::internal::DllMainCRTStartupForYY_Thunks(_hInstance, _uReason, _pReserved);
    const auto _Cookie = __security_cookie;
    __security_cookie = _Saved;
    __security_cookie_complement = _SavedComplement;
    return _Cookie;
}

bool Check(bool _bCondition, const char* _szMessage)
{
    if (!_bCondition)
        fprintf(stderr, "%s\n", _szMessage);
    return _bCondition;
}

int wmain(int _Argc, wchar_t** _Argv)
{
    bool _bSuccess = true;
    const auto _hInstance = reinterpret_cast<HINSTANCE>(static_cast<uintptr_t>(0x1234));
    const DWORD _Reasons[] = {DLL_PROCESS_ATTACH, DLL_THREAD_ATTACH, DLL_THREAD_DETACH, DLL_PROCESS_DETACH, 99};
    const BOOL _Results[] = {FALSE, TRUE};
    LPVOID const _Reserved[] = {nullptr, reinterpret_cast<LPVOID>(static_cast<uintptr_t>(0x5678))};
    for (DWORD _uReason : _Reasons)
    {
        for (BOOL _bResult : _Results)
        {
            for (LPVOID _pReserved : _Reserved)
            {
                g_State = {};
                g_Result = _bResult;
                BOOL _Result;
                const auto _Cookie = CallWithDefaultCookie(_hInstance, _uReason, _pReserved, &_Result);
                _bSuccess &= Check(_Result == _bResult, "startup return value changed");
                _bSuccess &= Check(g_State.Instance == _hInstance && g_State.Reason == _uReason &&
                    g_State.Reserved == _pReserved, "startup arguments changed");
                _bSuccess &= Check(g_State.InitCalls == (_uReason == DLL_PROCESS_ATTACH), "incorrect thunk initialization count");
                _bSuccess &= Check(g_State.UninitCalls == (_uReason == DLL_PROCESS_DETACH), "incorrect thunk cleanup count");
#ifdef YY_STARTUP_TEST_CUSTOM
                _bSuccess &= Check(g_State.CustomCalls == 1 && g_State.OriginalCalls == 0, "custom entry not selected");
#else
                _bSuccess &= Check(g_State.CustomCalls == 0 && g_State.OriginalCalls == 1, "original entry not selected");
#endif
#if defined(YY_THUNKS_HARDENED) && YY_THUNKS_HARDENED == 1
                if (_uReason == DLL_PROCESS_ATTACH)
                {
                    _bSuccess &= Check(_Cookie != DefaultCookie && g_State.InitCookie == _Cookie &&
                        g_State.CallbackCookie == _Cookie, "CRT cookie was not initialized before thunk work");
                }
                else
#endif
                {
                    _bSuccess &= Check(_Cookie == DefaultCookie && g_State.CallbackCookie == DefaultCookie,
                        "cookie changed outside hardened process attach");
                }
                if (_uReason == DLL_PROCESS_DETACH)
                {
                    _bSuccess &= Check(g_State.Terminating == (_pReserved ? -1 : 1), "termination state changed");
                }
            }
        }
    }

    if (_Argc == 2)
    {
        const auto _hModule = LoadLibraryW(_Argv[1]);
        if (!Check(_hModule != nullptr, "LoadLibrary failed"))
            return 1;
        const auto _ReadState = reinterpret_cast<const State* (__cdecl*)()>(GetProcAddress(_hModule, "ReadState"));
        const auto _ObserveDetach = reinterpret_cast<void (__cdecl*)(State*)>(GetProcAddress(_hModule, "ObserveDetach"));
        if (!Check(_ReadState && _ObserveDetach, "fixture exports missing"))
        {
            FreeLibrary(_hModule);
            return 1;
        }
        const auto _State = *_ReadState();
        _bSuccess &= Check(_State.InitCalls == 1 && _State.OriginalCalls == 1 &&
            _State.InitCookie != DefaultCookie && _State.InitCookie != 0 &&
            _State.CallbackCookie == _State.InitCookie && _State.Instance == _hModule &&
            _State.Reason == DLL_PROCESS_ATTACH && _State.Reserved == nullptr, "DLL attach contract failed");
        State _Detached = {};
        _ObserveDetach(&_Detached);
        _bSuccess &= Check(FreeLibrary(_hModule) != FALSE, "FreeLibrary failed");
        _bSuccess &= Check(_Detached.UninitCalls == 1 && _Detached.OriginalCalls == 2 &&
            _Detached.Instance == _hModule && _Detached.Reason == DLL_PROCESS_DETACH &&
            _Detached.Reserved == nullptr && _Detached.Terminating == 1, "DLL detach contract failed");
    }
    return _bSuccess ? 0 : 1;
}
#endif
