
#ifndef PSAPI_VERSION
#define PSAPI_VERSION 1
#endif

#if (YY_Thunks_Target < __WindowsNT6)
#include <psapi.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_psapi)
#define __Comment_Lib_psapi
#pragma comment(lib, "Psapi.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    psapi,
    20,
    BOOL,
    WINAPI,
    EnumProcessModulesEx,
        _In_ HANDLE hProcess,
        _Out_writes_bytes_(cb) HMODULE* lphModule,
        _In_ DWORD cb,
        _Out_ LPDWORD lpcbNeeded,
        _In_ DWORD dwFilterFlag
        )
    {
        if (auto pEnumProcessModulesEx = try_get_EnumProcessModulesEx())
        {
            return pEnumProcessModulesEx(hProcess, lphModule, cb, lpcbNeeded, dwFilterFlag);
        }

        return EnumProcessModules(hProcess, lphModule, cb, lpcbNeeded);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    psapi,
    12,
    BOOL,
    WINAPI,
    GetWsChangesEx,
        _In_ HANDLE hProcess,
        _Out_writes_bytes_to_(*cb, *cb) PPSAPI_WS_WATCH_INFORMATION_EX lpWatchInfoEx,
        _Inout_ PDWORD cb
        )
    {
        if (auto pGetWsChangesEx = try_get_GetWsChangesEx())
        {
            return pGetWsChangesEx(hProcess, lpWatchInfoEx, cb);
        }

        PPSAPI_WS_WATCH_INFORMATION pWatchInfo = nullptr;
        DWORD cbWatchInfo = 1024 * sizeof(pWatchInfo[0]);
        const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
        LSTATUS lStatus = ERROR_SUCCESS;

        for (;;)
        {
            if (pWatchInfo)
            {
                cbWatchInfo *= 2;
            
                auto pNewWatchInfo = (PPSAPI_WS_WATCH_INFORMATION)HeapReAlloc(ProcessHeap, 0, pWatchInfo, cbWatchInfo);

                if (!pNewWatchInfo)
                {
                    lStatus = ERROR_OUTOFMEMORY;
                    break;
                }

                pWatchInfo = pNewWatchInfo;
            }
            else
            {
                pWatchInfo = (PPSAPI_WS_WATCH_INFORMATION)HeapAlloc(ProcessHeap, 0, cbWatchInfo);
                if (!pWatchInfo)
                {
                    lStatus = ERROR_OUTOFMEMORY;
                    break;
                }
            }

            if (!GetWsChanges(hProcess, pWatchInfo, cbWatchInfo))
            {
                lStatus = GetLastError();

                if (lStatus == ERROR_INSUFFICIENT_BUFFER)
                {
                    continue;

                }
                else
                {
                    break;
                }
            }

            //确定实际个数
            const auto pWatchInfoMax = (PPSAPI_WS_WATCH_INFORMATION)((byte*)pWatchInfo + cbWatchInfo);
            auto pWatchInfoTerminated = pWatchInfo;
            for (; pWatchInfoTerminated < pWatchInfoMax && pWatchInfoTerminated->FaultingPc != nullptr; ++pWatchInfoTerminated);

            auto ccWatchInfo = pWatchInfoTerminated - pWatchInfo;

            auto cbWatchInfoExRequest = (ccWatchInfo + 1) * sizeof(lpWatchInfoEx[0]);
            if (cbWatchInfoExRequest > MAXUINT32)
            {
                lStatus = ERROR_FUNCTION_FAILED;
                break;
            }

            auto cbBuffer = *cb;
            *cb = static_cast<DWORD>(cbWatchInfoExRequest);

            if (cbBuffer < cbWatchInfoExRequest)
            {
                lStatus = ERROR_INSUFFICIENT_BUFFER;
                break;
            }


            //复制到新缓冲区
            for (int i = 0; i != ccWatchInfo; ++i)
            {
                lpWatchInfoEx[i].BasicInfo = pWatchInfo[i];
                lpWatchInfoEx[i].FaultingThreadId = 0;
                lpWatchInfoEx[i].Flags = 0;
            }
        
            //插入终止标记
            lpWatchInfoEx[ccWatchInfo] = PSAPI_WS_WATCH_INFORMATION_EX{};

            lStatus = ERROR_SUCCESS;
            break;
        }

        if (pWatchInfo)
        {
            HeapFree(ProcessHeap, 0, pWatchInfo);
        }
    
        if (lStatus == ERROR_SUCCESS)
        {
            return TRUE;
        }
        else
        {
            SetLastError(lStatus);
            return FALSE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

    //Windows Vista, Windows XP Professional x64 Edition [desktop apps only]
    //Windows Server 2008, Windows Server 2003 with SP1 [desktop apps only]
    __DEFINE_THUNK(
    psapi,
    12,
    BOOL,
    WINAPI,
    QueryWorkingSetEx,
        _In_ HANDLE hProcess,
        _Out_writes_bytes_(cb) PVOID pv,
        _In_ DWORD cb
        )
    {
        if (const auto pQueryWorkingSetEx = try_get_QueryWorkingSetEx())
        {
            return pQueryWorkingSetEx(hProcess, pv, cb);
        }
        else
        {
            SetLastError(ERROR_INVALID_FUNCTION);
            return FALSE;
        }
    }
#endif
} //namespace YY::Thunks
