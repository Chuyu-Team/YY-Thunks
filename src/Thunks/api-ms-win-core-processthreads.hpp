#if (YY_Thunks_Target < __WindowsNT6_2)
#include <processthreadsapi.h>
#endif

#ifdef YY_Thunks_Implemented

//结构体来源：http://www.rohitab.com/discuss/topic/38601-proc-thread-attribute-list-structure-documentation/


// This structure stores the value for each attribute
typedef struct _PROC_THREAD_ATTRIBUTE_ENTRY
{
    DWORD_PTR   Attribute;  // PROC_THREAD_ATTRIBUTE_xxx
    SIZE_T      cbSize;
    PVOID       lpValue;
} PROC_THREAD_ATTRIBUTE_ENTRY, * LPPROC_THREAD_ATTRIBUTE_ENTRY;

// This structure contains a list of attributes that have been added using UpdateProcThreadAttribute
typedef struct _PROC_THREAD_ATTRIBUTE_LIST
{
    DWORD                          dwFlags;
    ULONG                          Size;
    ULONG                          Count;
    ULONG                          Reserved;
    LPPROC_THREAD_ATTRIBUTE_ENTRY  lpExtendedFlags;
    PROC_THREAD_ATTRIBUTE_ENTRY    Entries[ANYSIZE_ARRAY];
} PROC_THREAD_ATTRIBUTE_LIST, * LPPROC_THREAD_ATTRIBUTE_LIST;

#endif

namespace YY::Thunks::Fallback
{
#if defined(YY_Thunks_Implemented) && (YY_Thunks_Target < __WindowsNT6)
    namespace
    {
        static void __cdecl UninitPageVirtualProtect();

        static char* volatile* GetPageVirtualProtect()
        {
            //注册 m_pPageVirtualProtect 的反初始化工作
            __declspec(allocate(".YYThr$AAB")) static void* RunUninitPageVirtualProtect = reinterpret_cast<void*>(&Fallback::UninitPageVirtualProtect);

            __foreinclude(RunUninitPageVirtualProtect);

            static char* volatile m_pPageVirtualProtect = nullptr;

            return &m_pPageVirtualProtect;
        }

        static void __cdecl UninitPageVirtualProtect()
        {
            auto pOrgPageVirtualProtect = (char*)InterlockedExchangePointer((PVOID volatile*)GetPageVirtualProtect(), INVALID_HANDLE_VALUE);

            if (pOrgPageVirtualProtect != nullptr && pOrgPageVirtualProtect != (char*)INVALID_HANDLE_VALUE)
            {
                VirtualFree(pOrgPageVirtualProtect, 0, MEM_RELEASE);
            }
        }
    }
#endif
}

namespace YY ::Thunks
{
#if (YY_Thunks_Target < __WindowsNT5_2)

    //Windows Vista, Windows Server 2003
    __DEFINE_THUNK(
    kernel32,
    0,
    DWORD,
    WINAPI,
    GetCurrentProcessorNumber,
        VOID
        )
    {
        if (const auto _pfnGetCurrentProcessorNumber = try_get_GetCurrentProcessorNumber())
        {
            return _pfnGetCurrentProcessorNumber();
        }
        else
        {
            // Reference: https://www.cs.tcd.ie/Jeremy.Jones/GetCurrentProcessorNumberXP.htm
            //
            // The GetCurrentProcessorNumber() function is not available in XP. 
            // Here is a VC++ version of the function that works with Windows XP on Intel x86 single, hyperthreaded, multicore 
            // and multi-socket systems. It makes use of the APIC ID returned by the CPUID instruction. 
            // This is in the range 0 .. N-1, where N is the number of logical CPUs.

            __asm
            {
                mov eax, 1
                cpuid
                shr ebx, 24
                // eax 返回值
                mov eax, ebx
            }
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    //Windows 7, Windows Server 2008 R2
    __DEFINE_THUNK(
    kernel32,
    4,
    VOID,
    WINAPI,
    GetCurrentProcessorNumberEx,
        _Out_ PPROCESSOR_NUMBER ProcNumber
        )
    {
        if (auto pGetCurrentProcessorNumberEx = try_get_GetCurrentProcessorNumberEx())
        {
            pGetCurrentProcessorNumberEx(ProcNumber);
        }
        else
        {
            //不支持GetCurrentProcessorNumberEx时假定用户只有一组CPU
            ProcNumber->Group = 0;
            ProcNumber->Number = static_cast<BYTE>(GetCurrentProcessorNumber());
            ProcNumber->Reserved = 0;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2)

    //Windows Vista [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    //感谢 过客 提供
    __DEFINE_THUNK(
    kernel32,
    4,
    DWORD,
    WINAPI,
    GetThreadId,
        _In_ HANDLE Thread
        )
    {
        if (const auto _pfnGetThreadId = try_get_GetThreadId())
        {
            return _pfnGetThreadId(Thread);
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryInformationThread = try_get_NtQueryInformationThread();
        if(!NtQueryInformationThread)
        {
            SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
            return 0;
        }
#endif

        THREAD_BASIC_INFORMATION ThreadBasicInfo;

        LONG Status = NtQueryInformationThread(Thread, ThreadBasicInformation, &ThreadBasicInfo, sizeof(ThreadBasicInfo), nullptr);

        if (Status < 0)
        {
            internal::BaseSetLastNTError(Status);
            return 0;
        }
        else
        {
            return (DWORD)ThreadBasicInfo.ClientId.UniqueThread;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2)

    //Windows Vista [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    4,
    DWORD,
    WINAPI,
    GetProcessIdOfThread,
        _In_ HANDLE Thread
        )
    {
        if (const auto _pfnGetProcessIdOfThread = try_get_GetProcessIdOfThread())
            return _pfnGetProcessIdOfThread(Thread);
        
#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryInformationThread = try_get_NtQueryInformationThread();
        if (!NtQueryInformationThread)
        {
            SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
            return 0;
        }
#endif

        THREAD_BASIC_INFORMATION ThreadBasicInfo;

        LONG Status = NtQueryInformationThread(Thread, ThreadBasicInformation, &ThreadBasicInfo, sizeof(ThreadBasicInfo), nullptr);

        if (Status < 0)
        {
            internal::BaseSetLastNTError(Status);
            return 0;
        }
        else
        {
            return (DWORD)ThreadBasicInfo.ClientId.UniqueProcess;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1_SP1)

    //Windows Vista, Windows XP with SP1 [desktop apps | UWP apps]
    //Windows Server 2003 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    4,
    DWORD,
    WINAPI,
    GetProcessId,
        _In_ HANDLE Process
        )
    {
        if (auto pGetProcessId = try_get_GetProcessId())
        {
            return pGetProcessId(Process);
        }
#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryInformationProcess = try_get_NtQueryInformationProcess();
        if (!NtQueryInformationProcess)
        {
            SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
            return 0;
        }
#endif

        PROCESS_BASIC_INFORMATION ProcessBasicInfo;

        LONG Status = NtQueryInformationProcess(Process, ProcessBasicInformation, &ProcessBasicInfo, sizeof(ProcessBasicInfo), nullptr);

        if (Status < 0)
        {
            internal::BaseSetLastNTError(Status);
            return 0;
        }
        else
        {
            return (DWORD)ProcessBasicInfo.UniqueProcessId;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    0,
    VOID,
    WINAPI,
    FlushProcessWriteBuffers,
        VOID
        )
    {
        if (auto pFlushProcessWriteBuffers = try_get_FlushProcessWriteBuffers())
        {
            return pFlushProcessWriteBuffers();
        }


        /*
        * 参考了 14.29.30037 crt\src\concrt\ResourceManager.cpp 实现
            
        */

        for (;;)
        {
            auto pOrgPageVirtualProtect = (char*)InterlockedCompareExchangePointer((PVOID volatile*)Fallback::GetPageVirtualProtect(), nullptr, nullptr);


            //内存存在异常，我们什么也不做。
            if (pOrgPageVirtualProtect == (char*)INVALID_HANDLE_VALUE)
            {
                break;
            }
            else if (pOrgPageVirtualProtect == nullptr)
            {
                //我们需要开辟一段内存

                auto pPageVirtualProtectTmp = (char*)VirtualAlloc(NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

                if (pPageVirtualProtectTmp == nullptr)
                {
                    //内存申请失败，直接设置为 -1，并且什么也不做
                    InterlockedCompareExchangePointer((PVOID volatile*)Fallback::GetPageVirtualProtect(), INVALID_HANDLE_VALUE, nullptr);
                    break;
                }

                *pPageVirtualProtectTmp = 1;

                pOrgPageVirtualProtect = (char*)InterlockedCompareExchangePointer((PVOID volatile*)Fallback::GetPageVirtualProtect(), pPageVirtualProtectTmp, nullptr);

                if (pOrgPageVirtualProtect != nullptr)
                {
                    //说明另外一个线程已经设置，我们释放这次申请的内存
                    VirtualFree(pPageVirtualProtectTmp, 0, MEM_RELEASE);
                }
                else
                {
                    pOrgPageVirtualProtect = pPageVirtualProtectTmp;
                }
            }


            // Note that the read of *m_pPageVirtualProtect is very important, as it makes it extremely likely that this memory will
            // be in the working set when we call VirtualProtect (see comments below).
            if (*pOrgPageVirtualProtect == 1)
            {
                //
                // VirtualProtect simulates FlushProcessWriteBuffers because it happens to send an inter-processor interrupt to all CPUs,
                // and inter-processor interrupts happen to cause the CPU's store buffers to be flushed.
                //
                // Unfortunately, VirtualProtect only does this if the page whose status is being changed is in the process' working set
                // (otherwise there's no need to tell the other CPUs that anything has changed).
                //
                // One way to do this is to lock the page into the process' working set. Unfortunately, it can fail if there are already too many
                // locked pages.
                //
                // We could increase the process' working set limit, using SetProcessWorkingSet, but that would be a) intrusive (the process may
                // have its own idea of what the limit should be), and b) race-prone (another thread may be trying to adjust the limit, to a
                // different value, at the same time).
                //
                // We could stop using *m_pPageVirtualProtect as the page we fiddle with, and instead use a page we know is already locked into
                // the working set. There's no way to enumerate such pages, so it'd have to be a well-known fixed location that we know is always
                // locked, and that can have its protection fiddled with without consequence.  We know of no such location, and if we did it would
                // undoubtedly be some internal Windows data structure that would be subject to changes in the way its memory is handled at any time.
                //
                // The VirtualProtect trick has worked for many years in the CLR, without the call to VirtualLock, without apparent problems.
                // Part of the reason is because of the equivalent of the check of *m_pPageVirtualProtect above.
                //
                DWORD oldProtect;

                // We have it on good authority from the kernel team that, although VirtualProtect is repeatedly called with the
                // same protection (PAGE_READONLY), the OS will not optimize out the flush buffers as a result.
                VirtualProtect(pOrgPageVirtualProtect, sizeof(BYTE), PAGE_READONLY, &oldProtect);
            }

            break;
        }
    }

#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps only]
    //Minimum supported server	Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    16,
    _Success_(return != FALSE)
    BOOL,
    WINAPI,
    InitializeProcThreadAttributeList,
        _Out_writes_bytes_to_opt_(*lpSize,*lpSize) LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
        _In_ DWORD dwAttributeCount,
        _Reserved_ DWORD dwFlags,
        _When_(lpAttributeList == nullptr,_Out_) _When_(lpAttributeList != nullptr,_Inout_) PSIZE_T lpSize
        )
    {
        if (const auto pInitializeProcThreadAttributeList = try_get_InitializeProcThreadAttributeList())
        {
            return pInitializeProcThreadAttributeList(lpAttributeList, dwAttributeCount, dwFlags, lpSize);
        }


        LSTATUS lStatus = ERROR_SUCCESS;

        do
        {
            //参数验证
            if (dwFlags)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            //Vista只支持 3个 Type，所以只有三个
            if (dwAttributeCount > 3)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }


            const auto cbSize = *lpSize;
            const auto cbSizeNeed = sizeof(PROC_THREAD_ATTRIBUTE_LIST) + dwAttributeCount * sizeof(PROC_THREAD_ATTRIBUTE_ENTRY);
            *lpSize = cbSizeNeed;

            if (lpAttributeList == nullptr || cbSize < cbSizeNeed)
            {
                lStatus = ERROR_INSUFFICIENT_BUFFER;
                break;
            }

            lpAttributeList->dwFlags = 0;
            lpAttributeList->lpExtendedFlags = nullptr;
            lpAttributeList->Size = dwAttributeCount;
            lpAttributeList->Count = 0;

            return TRUE;

        } while (false);


        SetLastError(lStatus);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps only]
    //Minimum supported server	Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    4,
    VOID,
    WINAPI,
    DeleteProcThreadAttributeList,
        _Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList
        )
    {
        if (const auto pDeleteProcThreadAttributeList = try_get_DeleteProcThreadAttributeList())
        {
            return pDeleteProcThreadAttributeList(lpAttributeList);
        }


        //Vista原版什么也没有做……
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps only]
    //Minimum supported server	Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    28,
    BOOL,
    WINAPI,
    UpdateProcThreadAttribute,
        _Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
        _In_ DWORD dwFlags,
        _In_ DWORD_PTR Attribute,
        _In_reads_bytes_opt_(cbSize) PVOID lpValue,
        _In_ SIZE_T cbSize,
        _Out_writes_bytes_opt_(cbSize) PVOID lpPreviousValue,
        _In_opt_ PSIZE_T lpReturnSize
        )
    {
        if (const auto pUpdateProcThreadAttribute = try_get_UpdateProcThreadAttribute())
        {
            return pUpdateProcThreadAttribute(lpAttributeList, dwFlags, Attribute, lpValue, cbSize, lpPreviousValue, lpReturnSize);
        }

        
        LSTATUS lStatus = ERROR_SUCCESS;

        do
        {
            auto AttributeMark = 1ul << Attribute;

            /////////////////////////////////////////////////////
            //
            // 参数检查
            //

            if (dwFlags & (~PROC_THREAD_ATTRIBUTE_REPLACE_VALUE))
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }


            if ((Attribute & PROC_THREAD_ATTRIBUTE_ADDITIVE) == 0)
            {
                if (lpAttributeList->Count == lpAttributeList->Size)
                {
                    //internal::BaseSetLastNTError(0xC0000001);
                    lStatus = ERROR_GEN_FAILURE;
                    break;
                }
                else if (AttributeMark & lpAttributeList->dwFlags)
                {
                    //internal::BaseSetLastNTError(0x40000000);
                    lStatus = ERROR_OBJECT_NAME_EXISTS;
                    break;
                }
                else if (lpPreviousValue)
                {
                    //internal::BaseSetLastNTError(0xC00000F4);
                    lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }
                else if (dwFlags & PROC_THREAD_ATTRIBUTE_REPLACE_VALUE)
                {
                    //internal::BaseSetLastNTError(0xC00000F0);
                    lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }
            }


            if ((PROC_THREAD_ATTRIBUTE_INPUT & Attribute) && lpReturnSize)
            {
                //internal::BaseSetLastNTError(0xC00000F5);
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            //
            //
            ////////////////////////////////////////////////////


            auto pAttribute = &lpAttributeList->Entries[lpAttributeList->Count];

            constexpr auto ProcThreadAttributeExtendedFlags = 1;
            //0x60001，文档没有公开
            constexpr auto PROC_THREAD_ATTRIBUTE_EXTENDED_FLAGS = ProcThreadAttributeValue(ProcThreadAttributeExtendedFlags, FALSE, TRUE, TRUE);


            if (Attribute == PROC_THREAD_ATTRIBUTE_PARENT_PROCESS) //0x20000
            {
                //WinXP不支持 UAC，没实现似乎也没什么的。

                if (cbSize != sizeof(HANDLE))
                {
                    //internal::BaseSetLastNTError(0xC0000004);
                    lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }
            }
            else if (Attribute == PROC_THREAD_ATTRIBUTE_EXTENDED_FLAGS) //0x60001
            {
                //系统没有公开这个含义，暂时让他允许通过把……

                if (cbSize != sizeof(DWORD))
                {
                    //internal::BaseSetLastNTError(0xC0000004);
                    lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }

                DWORD dwOrgFlags;

                if (lpAttributeList->lpExtendedFlags)
                {
                    pAttribute = lpAttributeList->lpExtendedFlags;
                    dwOrgFlags = (DWORD)lpAttributeList->lpExtendedFlags->lpValue;
                    AttributeMark = 0;
                }
                else
                {
                    lpAttributeList->lpExtendedFlags = pAttribute;
                    dwOrgFlags = 0;
                }


                auto dwNewFlags = *(DWORD*)lpValue;

                if (dwNewFlags & ~0x00000003ul)
                {
                    //internal::BaseSetLastNTError(0xC000000D);
                    lStatus = ERROR_BAD_LENGTH;
                    break;
                }

                if ((dwFlags & PROC_THREAD_ATTRIBUTE_REPLACE_VALUE) == 0 && dwOrgFlags)
                {
                    dwNewFlags |= dwOrgFlags;
                }

                if (lpPreviousValue)
                    *(DWORD*)lpPreviousValue = dwOrgFlags;

                lpValue = (PVOID)dwNewFlags;
            }
            else if (Attribute == PROC_THREAD_ATTRIBUTE_HANDLE_LIST) //0x20002
            {
                //WinXP也不支持指定句柄继承，他会直接继承所有可继承的句柄，所以没实现好像也没什么大不了的。

                if (cbSize == 0 || cbSize % sizeof(HANDLE) != 0)
                {
                    //internal::BaseSetLastNTError(0xC0000004);
                    lStatus = ERROR_INVALID_PARAMETER;
                    break;
                }
            }
            else
            {
                //internal::BaseSetLastNTError(0xC00000BB);
                lStatus = ERROR_NOT_SUPPORTED;
                break;
            }

            //LABEL_17
            pAttribute->lpValue = lpValue;

            if (AttributeMark)
            {
                pAttribute->Attribute = Attribute;
                pAttribute->cbSize = cbSize;
                ++lpAttributeList->Count;
                lpAttributeList->dwFlags |= AttributeMark;
            }

            return TRUE;

        } while (false);

        SetLastError(lStatus);
            
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2)

    //Windows Vista, Windows XP Professional x64 Edition [desktop apps only]
    //Windows Server 2008, Windows Server 2003 with SP1 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    SetThreadStackGuarantee,
        _Inout_ ULONG *StackSizeInBytes
        )
    {
        if (const auto pSetThreadStackGuarantee = try_get_SetThreadStackGuarantee())
        {
            return pSetThreadStackGuarantee(StackSizeInBytes);
        }

        // 以下内容来自：ucrt\misc\resetstk.cpp


        // 因为只面向 Windows XP以及之前的系统，我们可以硬编码的确定，页大小只可能是 4K。
        // 减少 GetSystemInfo 调用。
        constexpr auto PageSize = 4096u;
        constexpr auto MinStackRequest = 2u;

        auto pStack = (LPBYTE)_alloca(1);

        auto RegionSize = *StackSizeInBytes;

        // TEB没有 GuaranteedStackBytes，没法知道上一次的结果，我们仅仅返回一个假的数值。
        *StackSizeInBytes = PageSize;

        // 长度为 0 时，不需要额外申请
        if (RegionSize == 0)
            return TRUE;

        // 我们需要额外一块 PAGE_GUARD 来进行边界防御。
        RegionSize = (RegionSize + PageSize + (PageSize - 1)) & ~(PageSize - 1);
        RegionSize = max(RegionSize, PageSize * MinStackRequest);

        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(pStack, &mbi, sizeof mbi) == 0)
            return FALSE;

        auto pStackBase = (LPBYTE)mbi.AllocationBase;

        //
        // Find the page(s) just below where the stack pointer currently points.
        // This is the highest potential guard page.
        auto pMaxGuard = (LPBYTE)(((DWORD_PTR)pStack & ~(DWORD_PTR)(PageSize - 1)) - RegionSize);

        //
        // If the potential guard page is too close to the start of the stack
        // region, abandon the reset effort for lack of space.  Win9x has a
        // larger reserved stack requirement.
        auto pMinGuard = pStackBase + PageSize;

        if (pMaxGuard < pMinGuard)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        // Set the new guard page just below the current stack page.
        DWORD flOldProtect;
        if (VirtualAlloc(pMaxGuard, RegionSize, MEM_COMMIT, PAGE_READWRITE) == nullptr
            || VirtualProtect(pMaxGuard, RegionSize, PAGE_READWRITE | PAGE_GUARD, &flOldProtect) == 0)
        {
            return FALSE;
        }

        return TRUE;
    }
#endif
        
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    GetProcessMitigationPolicy,
        _In_ HANDLE _hProcess,
        _In_ PROCESS_MITIGATION_POLICY _eMitigationPolicy,
        _Out_writes_bytes_(_cbLength) PVOID _pBuffer,
        _In_ SIZE_T _cbLength
        )
    {
        if (const auto _pfnGetProcessMitigationPolicy = try_get_GetProcessMitigationPolicy())
        {
            return _pfnGetProcessMitigationPolicy(_hProcess, _eMitigationPolicy, _pBuffer, _cbLength);
        }

        if (!_pBuffer)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if ((DWORD)_eMitigationPolicy >= (DWORD)MaxProcessMitigationPolicy)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryInformationProcess = try_get_NtQueryInformationProcess();
        if (!NtQueryInformationProcess)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
#endif

        if (_eMitigationPolicy == ProcessDEPPolicy)
        {
            if (_cbLength != sizeof(PROCESS_MITIGATION_DEP_POLICY))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }
            KEXECUTE_OPTIONS _DepOptions = {};
            NTSTATUS _Status = NtQueryInformationProcess(_hProcess, ProcessExecuteFlags, &_DepOptions, sizeof(_DepOptions), nullptr);
            if (_Status >= 0)
            {
                auto _pDepPolicy = (PROCESS_MITIGATION_DEP_POLICY*)_pBuffer;
                _pDepPolicy->Enable = _DepOptions.ExecuteEnable ? 0 : 1;
                _pDepPolicy->DisableAtlThunkEmulation = _DepOptions.DisableThunkEmulation;
                _pDepPolicy->ReservedFlags = 0;
                _pDepPolicy->Permanent = _DepOptions.Permanent;
                return TRUE;
            }
            else if (STATUS_INVALID_INFO_CLASS == _Status || STATUS_NOT_SUPPORTED == _Status)
            {
                *(DWORD*)_pBuffer = 0;
                return TRUE;
            }
            else
            {
                internal::BaseSetLastNTError(_Status);
                return FALSE;
            }
        }
        else if (_eMitigationPolicy == ProcessMitigationOptionsMask)
        {
            if (_cbLength < sizeof(UINT64))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            memset(_pBuffer, 0, _cbLength);
            return TRUE;
        }
        else
        {
            if (_cbLength != sizeof(DWORD))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            YY_ProcessPolicyInfo _Info = { static_cast<DWORD>(_eMitigationPolicy) };
            NTSTATUS _Status = NtQueryInformationProcess(_hProcess, YY_ProcessPolicy, &_Info, sizeof(_Info), nullptr);
            if (_Status >= 0)
            {
                *(DWORD*)_pBuffer = _Info.Flags;
                return TRUE;
            }
            else if (STATUS_INVALID_INFO_CLASS == _Status || STATUS_NOT_SUPPORTED == _Status)
            {
                // 如果没有这个特性，那么统一设置为0，表示内部所有环境方案都处于关闭状态
                *(DWORD*)_pBuffer = 0;
                return TRUE;
            }
            else
            {
                internal::BaseSetLastNTError(_Status);
                return FALSE;
            }
        }
            
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    SetProcessMitigationPolicy,
        _In_ PROCESS_MITIGATION_POLICY _eMitigationPolicy,
        _In_reads_bytes_(_cbLength) PVOID _pBuffer,
        _In_ SIZE_T _cbLength
        )
    {
        if (const auto _pfnSetProcessMitigationPolicy = try_get_SetProcessMitigationPolicy())
        {
            return _pfnSetProcessMitigationPolicy(_eMitigationPolicy, _pBuffer, _cbLength);
        }

        if (!_pBuffer)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        if ((DWORD)_eMitigationPolicy >= (DWORD)MaxProcessMitigationPolicy || _eMitigationPolicy == ProcessMitigationOptionsMask)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
#if !defined(__USING_NTDLL_LIB)
        const auto NtSetInformationProcess = try_get_NtSetInformationProcess();
        if (!NtSetInformationProcess)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
#endif

        NTSTATUS _Status;
        if (_eMitigationPolicy == ProcessDEPPolicy)
        {
            if (_cbLength != sizeof(PROCESS_MITIGATION_DEP_POLICY))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            auto& _DepPolicy = *(PROCESS_MITIGATION_DEP_POLICY*)_pBuffer;

            KEXECUTE_OPTIONS _DepOptions = {};
            if (_DepPolicy.Enable)
            {
                _DepOptions.ExecuteDisable = 1;
            }
            else
            {
                _DepOptions.ExecuteEnable = 1;
            }
            _DepOptions.DisableThunkEmulation = _DepPolicy.DisableAtlThunkEmulation;
            _DepOptions.Permanent = _DepPolicy.Permanent;

            _Status = NtSetInformationProcess(NtCurrentProcess(), YY_ProcessPolicy, &_DepOptions, sizeof(_DepOptions));
                
        }
        else
        {
            if (_cbLength != sizeof(DWORD))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            YY_ProcessPolicyInfo _Info = { static_cast<DWORD>(_eMitigationPolicy), *(DWORD*)_pBuffer };
            _Status = NtSetInformationProcess(NtCurrentProcess(), YY_ProcessPolicy, &_Info, sizeof(_Info));
        }

        if (_Status >= 0)
        {
            return TRUE;
        }
        else
        {
            internal::BaseSetLastNTError(_Status);
            return FALSE;
        }
    }
#endif
        
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    SetProcessInformation,
        _In_ HANDLE _hProcess,
        _In_ PROCESS_INFORMATION_CLASS _eProcessInformationClass,
        _In_reads_bytes_(_cbProcessInformationSize) LPVOID _pProcessInformation,
        _In_ DWORD _cbProcessInformationSize
        )
    {
        if (const auto _pfnSetProcessInformation = try_get_SetProcessInformation())
        {
            return _pfnSetProcessInformation(_hProcess, _eProcessInformationClass, _pProcessInformation, _cbProcessInformationSize);
        }

        if (_pProcessInformation == nullptr || (DWORD)_eProcessInformationClass >= (DWORD)ProcessInformationClassMax)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtSetInformationProcess = try_get_NtSetInformationProcess();
        if (!NtSetInformationProcess)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
#endif

        NTSTATUS _Status;
        if (_eProcessInformationClass == ProcessMemoryPriority)
        {
            if (_cbProcessInformationSize != sizeof(MEMORY_PRIORITY_INFORMATION))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }
            // PAGE_PRIORITY_INFORMATION
            _Status = NtSetInformationProcess(_hProcess, ProcessPagePriority, _pProcessInformation, sizeof(DWORD));
        }
        else
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }

        if (_Status >= 0)
            return TRUE;

        internal::BaseSetLastNTError(_Status);
        return FALSE;
    }
#endif
        
#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    SetThreadInformation,
        _In_ HANDLE _hThread,
        _In_ THREAD_INFORMATION_CLASS _eThreadInformationClass,
        _In_reads_bytes_(_cbThreadInformationSize) LPVOID _pThreadInformation,
        _In_ DWORD _cbThreadInformationSize
        )
    {
        if (const auto _pfnSetThreadInformation = try_get_SetThreadInformation())
        {
            return _pfnSetThreadInformation(_hThread, _eThreadInformationClass, _pThreadInformation, _cbThreadInformationSize);
        }

        if (_pThreadInformation == nullptr || (DWORD)_eThreadInformationClass >= (DWORD)ThreadInformationClassMax)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
#if !defined(__USING_NTDLL_LIB)
        const auto NtSetInformationThread = try_get_NtSetInformationThread();
        if (!NtSetInformationThread)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
#endif

        NTSTATUS _Status;
        if (_eThreadInformationClass == ThreadMemoryPriority)
        {
            if (_cbThreadInformationSize != sizeof(MEMORY_PRIORITY_INFORMATION))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }
            _Status = NtSetInformationThread(_hThread, ThreadPagePriority, _pThreadInformation, sizeof(DWORD));
        }
        else if (_eThreadInformationClass == ThreadAbsoluteCpuPriority)
        {
            if (_cbThreadInformationSize != sizeof(DWORD))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }
            _Status = NtSetInformationThread(_hThread, ThreadActualBasePriority, _pThreadInformation, sizeof(DWORD));
        }
        else
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }

        if (_Status >= 0)
            return TRUE;

        internal::BaseSetLastNTError(_Status);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    GetThreadInformation,
        _In_ HANDLE _hThread,
        _In_ THREAD_INFORMATION_CLASS _eThreadInformationClass,
        _Out_writes_bytes_(_cbThreadInformationSize) LPVOID _pThreadInformation,
        _In_ DWORD _cbThreadInformationSize
        )
    {
        if (const auto _pfnGetThreadInformation = try_get_GetThreadInformation())
        {
            return _pfnGetThreadInformation(_hThread, _eThreadInformationClass, _pThreadInformation, _cbThreadInformationSize);
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryInformationThread = try_get_NtQueryInformationThread();
        if (!NtQueryInformationThread)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
#endif

        long _Status;
        if (_eThreadInformationClass == ThreadMemoryPriority)
        {
            _Status = NtQueryInformationThread(_hThread, ThreadPagePriority, _pThreadInformation, _cbThreadInformationSize, nullptr);
        }
        else if (_eThreadInformationClass == ThreadAbsoluteCpuPriority)
        {
            _Status = NtQueryInformationThread(_hThread, ThreadActualBasePriority, _pThreadInformation, _cbThreadInformationSize, nullptr);
        }
        else
        {
            _Status = STATUS_INVALID_PARAMETER;
        }

        if (_Status < 0)
        {
            internal::BaseSetLastNTError(_Status);
            return FALSE;
        }

        return TRUE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    SetThreadIdealProcessorEx,
        _In_ HANDLE _hThread,
        _In_ PPROCESSOR_NUMBER _pIdealProcessor,
        _Out_opt_ PPROCESSOR_NUMBER _pPreviousIdealProcessor
        )
    {
        if (const auto _pfnSetThreadIdealProcessorEx = try_get_SetThreadIdealProcessorEx())
        {
            return _pfnSetThreadIdealProcessorEx(_hThread, _pIdealProcessor, _pPreviousIdealProcessor);
        }

        // 不支持的平台统一认为只有一组处理器
        // 微软这里就没有检查 _pIdealProcessor
        if (_pIdealProcessor->Group != 0 || _pIdealProcessor->Number >= MAXIMUM_PROCESSORS)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        const auto _uPreviousIdealProcessor = SetThreadIdealProcessor(_hThread, _pIdealProcessor->Number);
        if (_uPreviousIdealProcessor == static_cast<DWORD>(-1))
        {
            return FALSE;
        }

        if (_pPreviousIdealProcessor)
        {
            _pPreviousIdealProcessor->Group = 0;
            _pPreviousIdealProcessor->Number = static_cast<BYTE>(_uPreviousIdealProcessor);
            _pPreviousIdealProcessor->Reserved = 0;
        }
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    8,
    BOOL,
    WINAPI,
    GetThreadIdealProcessorEx,
        _In_ HANDLE _hThread,
        _Out_ PPROCESSOR_NUMBER _pIdealProcessor
        )
    {
        if (const auto _pfnGetThreadIdealProcessorEx = try_get_GetThreadIdealProcessorEx())
        {
            return _pfnGetThreadIdealProcessorEx(_hThread, _pIdealProcessor);
        }
            
        // 不支持的平台统一认为只有一组处理器
        const auto _uPreviousIdealProcessor = SetThreadIdealProcessor(_hThread, MAXIMUM_PROCESSORS);
        if (_uPreviousIdealProcessor == static_cast<DWORD>(-1))
        {
            return FALSE;
        }

        _pIdealProcessor->Group = 0;
        _pIdealProcessor->Number = static_cast<BYTE>(_uPreviousIdealProcessor);
        _pIdealProcessor->Reserved = 0;
        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows 7 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2008 R2[仅限桌面应用]
    __DEFINE_THUNK(
    kernel32,
    32,
    HANDLE,
    WINAPI,
    CreateRemoteThreadEx,
        _In_  HANDLE                       _hProcess,
        _In_  LPSECURITY_ATTRIBUTES        _pThreadAttributes,
        _In_  SIZE_T                       _uStackSize,
        _In_  LPTHREAD_START_ROUTINE       _pStartAddress,
        _In_  LPVOID                       _pParameter,
        _In_  DWORD                        _fCreationFlags,
        _In_  LPPROC_THREAD_ATTRIBUTE_LIST _pAttributeList,
        _Out_ LPDWORD                      _pThreadId
        )
    {
        if (const auto _pfnCreateRemoteThreadEx = try_get_CreateRemoteThreadEx())
        {
            return _pfnCreateRemoteThreadEx(_hProcess, _pThreadAttributes, _uStackSize, _pStartAddress, _pParameter, _fCreationFlags, _pAttributeList, _pThreadId);
        }

        return CreateRemoteThread(_hProcess, _pThreadAttributes, _uStackSize, _pStartAddress, _pParameter, _fCreationFlags, _pThreadId);
    }
#endif
} //namespace YY::Thunks
