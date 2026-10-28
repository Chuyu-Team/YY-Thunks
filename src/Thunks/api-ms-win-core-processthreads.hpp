#if (YY_Thunks_Target < __WindowsNT10_15063)
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


#if (YY_Thunks_Target < __WindowsNT5_2_SP1)

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


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端    Windows 8 [桌面应用|UWP 应用]
    // 最低受支持的服务器    Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    8,
    VOID,
    WINAPI,
    GetCurrentThreadStackLimits,
        _Out_ PULONG_PTR _pLowLimit,
        _Out_ PULONG_PTR _pHighLimit
        )
    {
        if (const auto _pfnGetCurrentThreadStackLimits = try_get_GetCurrentThreadStackLimits())
        {
            return _pfnGetCurrentThreadStackLimits(_pLowLimit, _pHighLimit);
        }

        const auto _pTeb = NtCurrentTeb();
        *_pLowLimit = reinterpret_cast<ULONG_PTR>(_pTeb->DeallocationStack);
        *_pHighLimit = reinterpret_cast<ULONG_PTR>(_pTeb->NtTib.StackBase);
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


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    // Windows XP无法支持 PROCESS_QUERY_LIMITED_INFORMATION等权限。
    __DEFINE_THUNK(
    kernel32,
    12,
    HANDLE,
    WINAPI,
    OpenProcess,
        _In_ DWORD _fDesiredAccess,
        _In_ BOOL _bInheritHandle,
        _In_ DWORD _uProcessId
        )
    {
        const auto _pfnOpenProcess = try_get_OpenProcess();
        if (!_pfnOpenProcess)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return nullptr;
        }

        if (internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            if (PROCESS_QUERY_LIMITED_INFORMATION & _fDesiredAccess)
            {
                _fDesiredAccess |= PROCESS_QUERY_INFORMATION;
            }

            if (PROCESS_SET_LIMITED_INFORMATION & _fDesiredAccess)
            {
                _fDesiredAccess |= PROCESS_SET_INFORMATION;
            }

            // Vista以上最高为    0xFFFF
            // Windows XP最高只有 0x0FFF
            _fDesiredAccess &= ~(0xFFFFu ^ 0x0FFF);
        }

        return _pfnOpenProcess(_fDesiredAccess, _bInheritHandle, _uProcessId);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows XP [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    // Windows XP无法支持 PROCESS_QUERY_LIMITED_INFORMATION等权限。
    __DEFINE_THUNK(
    kernel32,
    12,
    HANDLE,
    WINAPI,
    OpenThread,
        _In_ DWORD _fDesiredAccess,
        _In_ BOOL _bInheritHandle,
        _In_ DWORD _uThreadId
        )
    {
        const auto _pfnOpenThread = try_get_OpenThread();
        if (!_pfnOpenThread)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return nullptr;
        }

        if (internal::GetSystemVersion() < internal::MakeVersion(6, 0))
        {
            if (THREAD_QUERY_LIMITED_INFORMATION & _fDesiredAccess)
            {
                _fDesiredAccess |= THREAD_QUERY_INFORMATION;
            }

            if (THREAD_SET_LIMITED_INFORMATION & _fDesiredAccess)
            {
                _fDesiredAccess |= THREAD_SET_INFORMATION;
            }

            // Vista以上最高为    0xFFFF
            // Windows XP最高只有 0x03FF
            _fDesiredAccess &= ~(0xFFFFu ^ 0x03FFu);
        }

        return _pfnOpenThread(_fDesiredAccess, _bInheritHandle, _uThreadId);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_15063)

    // 最低受支持的客户端	Windows 10版本 1607 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2016[桌面应用 | UWP 应用]
    // 虽然14393就有这个接口，但是14393必须从 kernelbase加载，因此也需要修正。
    __DEFINE_THUNK(
    kernelbase,
    8,
    HRESULT,
    WINAPI,
    GetThreadDescription,
        _In_ HANDLE _hThread,
        _Outptr_result_z_ PWSTR* _pszThreadDescription
        )
    {
        if (const auto _pfnGetThreadDescription = try_get_GetThreadDescription())
        {
            return _pfnGetThreadDescription(_hThread, _pszThreadDescription);
        }

        // 假装自己是一个空的 ThreadDescription

        *_pszThreadDescription = nullptr;
        auto _szThreadDescription = (wchar_t*)LocalAlloc(LMEM_FIXED, sizeof(wchar_t));
        if (!_szThreadDescription)
            return E_OUTOFMEMORY;

        *_szThreadDescription = L'\0';
        *_pszThreadDescription = _szThreadDescription;
        return 0x10000000;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_15063)

    // 最低受支持的客户端	Windows 10版本 1607 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2016[桌面应用 | UWP 应用]
    // 虽然14393就有这个接口，但是14393必须从 kernelbase加载，因此也需要修正。
    __DEFINE_THUNK(
    kernelbase,
    8,
    HRESULT,
    WINAPI,
    SetThreadDescription,
        _In_ HANDLE _hThread,
        _In_ PCWSTR _szThreadDescription
        )
    {
        if (const auto _pfnSetThreadDescription = try_get_SetThreadDescription())
        {
            return _pfnSetThreadDescription(_hThread, _szThreadDescription);
        }

        return E_NOTIMPL;
    }
#endif


#if defined(YY_Thunks_Implemented) && (YY_Thunks_Target < __WindowsNT10_16299)
namespace Fallback
{
    namespace
    {
        // 旧系统没有 CPU Set 概念，这里把“每个逻辑处理器”当作一个 CPU Set 来模拟：
        // Id 固定取 组号 * 掩码位宽 + 组内编号，查询接口与设置接口共用同一套映射，
        // 调用方拿到的 Id 因此仍能相互匹配（真实的 CPU Set Id 本身也只是不透明标识）。
        //
        // 亲和性掩码的位宽就是旧系统的处理器上限：NT 内核早期的亲和性 API 用 KAFFINITY 表达，
        // x86 最多 32 个处理器、x64 最多 64 个；Windows 7 才引入处理器组把 64 个/组扩展成多组。
        // 所以模拟实现只覆盖“组 0 + 掩码位宽内”的处理器，跨组集合如实报告不支持。
        static const ULONG __uCpuSetIdBitCount = sizeof(KAFFINITY) * 8;

        typedef BOOL(WINAPI* __PFN_GetThreadGroupAffinity)(_In_ HANDLE _hThread, _Out_ PGROUP_AFFINITY _pGroupAffinity);
        typedef BOOL(WINAPI* __PFN_SetThreadGroupAffinity)(_In_ HANDLE _hThread, _In_ const GROUP_AFFINITY* _pGroupAffinity, _Out_opt_ PGROUP_AFFINITY _pPreviousGroupAffinity);
        typedef BOOL(WINAPI* __PFN_GetLogicalProcessorInformationEx)(_In_ LOGICAL_PROCESSOR_RELATIONSHIP _eRelationshipType, _Out_writes_bytes_to_opt_(*_pcbBuffer, *_pcbBuffer) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX _pBuffer, _Inout_ PDWORD _pcbBuffer);

        // 这几个接口在 XP/2003 上并不存在，静态导入会让程序直接加载失败，因此按需动态解析。
        static FARPROC __fastcall __TryGetKernel32Proc(const char* _szProcName)
        {
            const auto _hKernel32 = try_get_module_kernel32();
            if (_hKernel32 == nullptr)
                return nullptr;

            return ::GetProcAddress(_hKernel32, _szProcName);
        }

        static ULONG __fastcall __CountAffinityMask(KAFFINITY _uMask)
        {
            ULONG _uCount = 0;

            for (; _uMask != 0; _uMask &= _uMask - 1)
                ++_uCount;

            return _uCount;
        }

        // 把 CPU Set Id 列表折算成“单组”的亲和性掩码；跨组的 Id 旧接口无法表达。
        static bool __fastcall __TryAffinityMaskFromCpuSetIds(
            _In_reads_(_uCpuSetIdCount) const ULONG* _puCpuSetIds,
            _In_ ULONG _uCpuSetIdCount,
            _Out_ WORD& _uGroup,
            _Out_ KAFFINITY& _uMask)
        {
            _uGroup = (WORD)(_puCpuSetIds[0] / __uCpuSetIdBitCount);
            _uMask = 0;

            for (ULONG _uIdIndex = 0; _uIdIndex != _uCpuSetIdCount; ++_uIdIndex)
            {
                const ULONG _uCpuSetId = _puCpuSetIds[_uIdIndex];
                if (_uCpuSetId / __uCpuSetIdBitCount != _uGroup)
                    return false;

                _uMask |= (KAFFINITY)1 << (_uCpuSetId % __uCpuSetIdBitCount);
            }

            return true;
        }

        // 按掩码位序展开 CPU Set Id。
        static void __fastcall __CpuSetIdsFromAffinityMask(
            _In_ KAFFINITY _uMask,
            _In_ WORD _uGroup,
            _Out_writes_bytes_(_uCpuSetIdCount) PULONG _puCpuSetIds,
            _In_ ULONG _uCpuSetIdCount)
        {
            for (ULONG _uBit = 0; _uBit != __uCpuSetIdBitCount && _uCpuSetIdCount != 0; ++_uBit)
            {
                if ((_uMask & ((KAFFINITY)1 << _uBit)) == 0)
                    continue;

                *_puCpuSetIds++ = _uGroup * __uCpuSetIdBitCount + _uBit;
                --_uCpuSetIdCount;
            }
        }

        struct __CPU_SET_PROCESSOR
        {
            WORD Group;
            BYTE LogicalProcessorIndex;
            BYTE CoreIndex;
            BYTE EfficiencyClass;
        };

        // 用 GetLogicalProcessorInformationEx 枚举“每个逻辑处理器一条记录”。
        // 该接口 Vista 才有，XP/2003 上返回 0，由调用方退化到 GetSystemInfo。
        static ULONG __fastcall __EnumCpuSetProcessorsEx(
            _Out_writes_to_(_cProcessors, return) __CPU_SET_PROCESSOR* _pProcessors,
            _In_ ULONG _cProcessors)
        {
            const auto _pfnGetLogicalProcessorInformationEx = (__PFN_GetLogicalProcessorInformationEx)__TryGetKernel32Proc("GetLogicalProcessorInformationEx");
            if (_pfnGetLogicalProcessorInformationEx == nullptr)
                return 0;

            // 单组 64 个处理器时约 5KB，取 8KB 留余量；更大的机器走退化路径。
            BYTE _Buffer[8192];
            DWORD _cbBuffer = sizeof(_Buffer);
            if (_pfnGetLogicalProcessorInformationEx(RelationProcessorCore, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)_Buffer, &_cbBuffer) == FALSE)
                return 0;

            ULONG _uCount = 0;
            BYTE _uCoreIndex = 0; // 组 0 内已枚举的物理核数量，用于给出稳定的 CoreIndex

            for (BYTE* _pCurrent = _Buffer, *_pBufferEnd = _Buffer + _cbBuffer; _pCurrent + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) <= _pBufferEnd;)
            {
                const auto _pInfo = (const SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)_pCurrent;
                if (_pInfo->Size < sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX))
                    break;

                if (_pInfo->Relationship == RelationProcessorCore)
                {
                    const PROCESSOR_RELATIONSHIP& _Relation = _pInfo->Processor;
                    const GROUP_AFFINITY* _pGroupMask = _Relation.GroupMask;

                    for (WORD _uGroupIndex = 0; _uGroupIndex != _Relation.GroupCount; ++_uGroupIndex)
                    {
                        const GROUP_AFFINITY& _GroupAffinity = _pGroupMask[_uGroupIndex];

                        // 只模拟组 0：NT 早期的亲和性接口本身只能表达一个处理器组
                        if (_GroupAffinity.Group == 0)
                        {
                            for (ULONG _uBit = 0; _uBit != __uCpuSetIdBitCount; ++_uBit)
                            {
                                if ((_GroupAffinity.Mask & ((KAFFINITY)1 << _uBit)) == 0)
                                    continue;

                                if (_uCount == _cProcessors)
                                    return _uCount;

                                _pProcessors[_uCount].Group = _GroupAffinity.Group;
                                _pProcessors[_uCount].LogicalProcessorIndex = (BYTE)_uBit;
                                _pProcessors[_uCount].CoreIndex = _uCoreIndex;
                                _pProcessors[_uCount].EfficiencyClass = _Relation.EfficiencyClass;
                                ++_uCount;
                            }

                            ++_uCoreIndex;
                        }
                    }
                }

                _pCurrent += _pInfo->Size;
            }

            return _uCount;
        }

        // XP/2003 没有处理器组信息，退化为“一个逻辑处理器一个核、只有组 0”。
        static ULONG __fastcall __EnumCpuSetProcessors(
            _Out_writes_to_(_cProcessors, return) __CPU_SET_PROCESSOR* _pProcessors,
            _In_ ULONG _cProcessors)
        {
            ULONG _uCount = __EnumCpuSetProcessorsEx(_pProcessors, _cProcessors);
            if (_uCount != 0)
                return _uCount;

            SYSTEM_INFO _SystemInfo = {};
            GetSystemInfo(&_SystemInfo);

            _uCount = _SystemInfo.dwNumberOfProcessors;
            if (_uCount > _cProcessors)
                _uCount = _cProcessors;
            if (_uCount > __uCpuSetIdBitCount)
                _uCount = __uCpuSetIdBitCount;

            for (ULONG _uIndex = 0; _uIndex != _uCount; ++_uIndex)
            {
                _pProcessors[_uIndex].Group = 0;
                _pProcessors[_uIndex].LogicalProcessorIndex = (BYTE)_uIndex;
                _pProcessors[_uIndex].CoreIndex = (BYTE)_uIndex;
                _pProcessors[_uIndex].EfficiencyClass = 0;
            }

            return _uCount;
        }
    }
}
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2016 [桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    GetProcessDefaultCpuSets,
        _In_ HANDLE _hProcess,
        _Out_writes_to_opt_(_uCpuSetIdCount, *_puRequiredIdCount) PULONG _puCpuSetIds,
        _In_ ULONG _uCpuSetIdCount,
        _Out_ PULONG _puRequiredIdCount
        )
    {
        if (const auto _pfnGetProcessDefaultCpuSets = try_get_GetProcessDefaultCpuSets())
        {
            return _pfnGetProcessDefaultCpuSets(_hProcess, _puCpuSetIds, _uCpuSetIdCount, _puRequiredIdCount);
        }

        if (_puRequiredIdCount == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        // 旧系统没有“进程默认 CPU Set”，用进程亲和性掩码（组 0）等价表达。
        const HANDLE _hQueryProcess = _hProcess != nullptr ? _hProcess : GetCurrentProcess();

        DWORD_PTR _uProcessAffinityMask = 0;
        DWORD_PTR _uSystemAffinityMask = 0;
        if (GetProcessAffinityMask(_hQueryProcess, &_uProcessAffinityMask, &_uSystemAffinityMask) == FALSE)
            return FALSE;

        __WarningMessage__("旧系统没有 CPU Set，GetProcessDefaultCpuSets 按进程亲和性（组 0、掩码位宽内：x86 32 / x64 64 个处理器）模拟。");

        const KAFFINITY _uMask = (KAFFINITY)_uProcessAffinityMask;
        const ULONG _cCpuSetIds = Fallback::__CountAffinityMask(_uMask);

        *_puRequiredIdCount = _cCpuSetIds;

        // 与系统语义一致：缓冲区为空时只回填所需数量
        if (_puCpuSetIds == nullptr)
            return TRUE;

        if (_uCpuSetIdCount < _cCpuSetIds)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        Fallback::__CpuSetIdsFromAffinityMask(_uMask, 0, _puCpuSetIds, _cCpuSetIds);

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2016 [桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    SetProcessDefaultCpuSets,
        _In_ HANDLE _hProcess,
        _In_reads_(_uCpuSetIdCount) const ULONG* _puCpuSetIds,
        _In_ ULONG _uCpuSetIdCount
        )
    {
        if (const auto _pfnSetProcessDefaultCpuSets = try_get_SetProcessDefaultCpuSets())
        {
            return _pfnSetProcessDefaultCpuSets(_hProcess, _puCpuSetIds, _uCpuSetIdCount);
        }

        if (_puCpuSetIds == nullptr || _uCpuSetIdCount == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        WORD _uGroup = 0;
        KAFFINITY _uMask = 0;
        if (Fallback::__TryAffinityMaskFromCpuSetIds(_puCpuSetIds, _uCpuSetIdCount, _uGroup, _uMask) == false || _uGroup != 0)
        {
            // 旧接口只能表达一个处理器组，跨组集合无法等价落到亲和性上
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }

        __WarningMessage__("旧系统没有 CPU Set，SetProcessDefaultCpuSets 通过 SetProcessAffinityMask 模拟，语义为进程亲和性而非默认 CPU Set。");

        const HANDLE _hTargetProcess = _hProcess != nullptr ? _hProcess : GetCurrentProcess();
        if (SetProcessAffinityMask(_hTargetProcess, _uMask) == FALSE)
            return FALSE;

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2016 [桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    GetThreadSelectedCpuSets,
        _In_ HANDLE _hThread,
        _Out_writes_to_opt_(_uCpuSetIdCount, *_puRequiredIdCount) PULONG _puCpuSetIds,
        _In_ ULONG _uCpuSetIdCount,
        _Out_ PULONG _puRequiredIdCount
        )
    {
        if (const auto _pfnGetThreadSelectedCpuSets = try_get_GetThreadSelectedCpuSets())
        {
            return _pfnGetThreadSelectedCpuSets(_hThread, _puCpuSetIds, _uCpuSetIdCount, _puRequiredIdCount);
        }

        if (_puRequiredIdCount == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        const auto _pfnGetThreadGroupAffinity = (Fallback::__PFN_GetThreadGroupAffinity)Fallback::__TryGetKernel32Proc("GetThreadGroupAffinity");
        if (_pfnGetThreadGroupAffinity == nullptr)
        {
            // XP/2003 连线程组亲和性查询都没有，且没有不修改线程亲和性就能读到掩码的旧接口
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }

        GROUP_AFFINITY _ThreadGroupAffinity = {};
        if (_pfnGetThreadGroupAffinity(_hThread, &_ThreadGroupAffinity) == FALSE)
            return FALSE;

        if (_ThreadGroupAffinity.Group != 0)
        {
            // 线程落在组 0 之外时，Id 无法用掩码位序表达
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }

        __WarningMessage__("旧系统没有 CPU Set，GetThreadSelectedCpuSets 按线程组亲和性（组 0、掩码位宽内）模拟。");

        const ULONG _cCpuSetIds = Fallback::__CountAffinityMask(_ThreadGroupAffinity.Mask);

        *_puRequiredIdCount = _cCpuSetIds;

        if (_puCpuSetIds == nullptr)
            return TRUE;

        if (_uCpuSetIdCount < _cCpuSetIds)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        Fallback::__CpuSetIdsFromAffinityMask(_ThreadGroupAffinity.Mask, _ThreadGroupAffinity.Group, _puCpuSetIds, _cCpuSetIds);

        return TRUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_14393)

    // 最低受支持的客户端	Windows 10版本 1607 [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2016 [桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    SetThreadSelectedCpuSets,
        _In_ HANDLE _hThread,
        _In_reads_(_uCpuSetIdCount) const ULONG* _puCpuSetIds,
        _In_ ULONG _uCpuSetIdCount
        )
    {
        if (const auto _pfnSetThreadSelectedCpuSets = try_get_SetThreadSelectedCpuSets())
        {
            return _pfnSetThreadSelectedCpuSets(_hThread, _puCpuSetIds, _uCpuSetIdCount);
        }

        if (_puCpuSetIds == nullptr || _uCpuSetIdCount == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        WORD _uGroup = 0;
        KAFFINITY _uMask = 0;
        if (Fallback::__TryAffinityMaskFromCpuSetIds(_puCpuSetIds, _uCpuSetIdCount, _uGroup, _uMask) == false)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }

        const auto _pfnSetThreadGroupAffinity = (Fallback::__PFN_SetThreadGroupAffinity)Fallback::__TryGetKernel32Proc("SetThreadGroupAffinity");
        if (_pfnSetThreadGroupAffinity != nullptr)
        {
            __WarningMessage__("旧系统没有 CPU Set，SetThreadSelectedCpuSets 通过 SetThreadGroupAffinity 模拟。");

            GROUP_AFFINITY _NewThreadGroupAffinity = {};
            _NewThreadGroupAffinity.Mask = _uMask;
            _NewThreadGroupAffinity.Group = _uGroup;

            return _pfnSetThreadGroupAffinity(_hThread, &_NewThreadGroupAffinity, nullptr);
        }

        if (_uGroup != 0)
        {
            // SetThreadAffinityMask 只能作用于组 0
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }

        __WarningMessage__("旧系统没有 CPU Set，SetThreadSelectedCpuSets 通过 SetThreadAffinityMask 模拟，处理器上限为掩码位宽（x86 32 / x64 64）。");

        return SetThreadAffinityMask(_hThread, _uMask) != 0;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_16299)

    // 最低受支持的客户端	Windows 10版本 1709 [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 版本 1709 [桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    20,
    BOOL,
    WINAPI,
    GetSystemCpuSetInformation,
        _Out_writes_bytes_to_opt_(_cbInformation, *_puReturnedLength) PSYSTEM_CPU_SET_INFORMATION _pInformation,
        _In_ ULONG _cbInformation,
        _Out_ PULONG _puReturnedLength,
        _In_opt_ HANDLE _hProcess,
        _In_ ULONG _uFlags
        )
    {
        if (const auto _pfnGetSystemCpuSetInformation = try_get_GetSystemCpuSetInformation())
        {
            return _pfnGetSystemCpuSetInformation(_pInformation, _cbInformation, _puReturnedLength, _hProcess, _uFlags);
        }

        if (_puReturnedLength == nullptr)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        *_puReturnedLength = 0;

        // 旧系统没有 CPU Set 调度，这里按“一个逻辑处理器 = 一个 CPU Set”模拟，
        // 让按 CPU Set 规划亲和性的调用方仍能拿到真实的组号与组内处理器编号，
        // 后续用 SetThreadSelectedCpuSets（本文件同样已模拟）即可真正落到亲和性上。
        Fallback::__CPU_SET_PROCESSOR _Processors[sizeof(KAFFINITY) * 8] = {};
        const ULONG _cProcessors = Fallback::__EnumCpuSetProcessors(_Processors, _countof(_Processors));
        if (_cProcessors == 0)
        {
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }

        // _hProcess 非空时只报告该进程亲和性允许的处理器
        KAFFINITY _uAllowedMask = (KAFFINITY)-1;
        if (_hProcess != nullptr)
        {
            DWORD_PTR _uProcessAffinityMask = 0;
            DWORD_PTR _uSystemAffinityMask = 0;
            if (GetProcessAffinityMask(_hProcess, &_uProcessAffinityMask, &_uSystemAffinityMask) == FALSE)
                return FALSE;

            _uAllowedMask = (KAFFINITY)_uProcessAffinityMask;
        }

        // 模拟出来的集合全部是“已分配、非驻留、非实时”，_uFlags 里的过滤器位不会改变结果集
        UNREFERENCED_PARAMETER(_uFlags);

        ULONG _cMatchedProcessors = 0;
        for (ULONG _uIndex = 0; _uIndex != _cProcessors; ++_uIndex)
        {
            if ((_uAllowedMask & ((KAFFINITY)1 << _Processors[_uIndex].LogicalProcessorIndex)) == 0)
                continue;

            ++_cMatchedProcessors;
        }

        const ULONG _cbRequired = _cMatchedProcessors * sizeof(SYSTEM_CPU_SET_INFORMATION);

        *_puReturnedLength = _cbRequired;

        // 与系统语义一致：缓冲区为空或空间不足时只回填所需长度
        if (_pInformation == nullptr || _cbInformation < _cbRequired)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        __WarningMessage__("旧系统没有 CPU Set，GetSystemCpuSetInformation 按“一个逻辑处理器 = 一个 CPU Set”模拟，仅覆盖组 0 与掩码位宽内的处理器（x86 32 / x64 64）。");

        BYTE* _pCurrent = (BYTE*)_pInformation;
        for (ULONG _uIndex = 0; _uIndex != _cProcessors; ++_uIndex)
        {
            const auto& _Processor = _Processors[_uIndex];
            if ((_uAllowedMask & ((KAFFINITY)1 << _Processor.LogicalProcessorIndex)) == 0)
                continue;

            const auto _pCpuSetInformation = (SYSTEM_CPU_SET_INFORMATION*)_pCurrent;

            _pCpuSetInformation->Size = sizeof(SYSTEM_CPU_SET_INFORMATION);
            _pCpuSetInformation->Type = CpuSetInformation;
            _pCpuSetInformation->CpuSet.Id = _Processor.Group * Fallback::__uCpuSetIdBitCount + _Processor.LogicalProcessorIndex;
            _pCpuSetInformation->CpuSet.Group = _Processor.Group;
            _pCpuSetInformation->CpuSet.LogicalProcessorIndex = _Processor.LogicalProcessorIndex;
            _pCpuSetInformation->CpuSet.CoreIndex = _Processor.CoreIndex;
            _pCpuSetInformation->CpuSet.LastLevelCacheIndex = 0;
            _pCpuSetInformation->CpuSet.NumaNodeIndex = 0;
            _pCpuSetInformation->CpuSet.EfficiencyClass = _Processor.EfficiencyClass;
            _pCpuSetInformation->CpuSet.AllFlags = SYSTEM_CPU_SET_INFORMATION_ALLOCATED;
            _pCpuSetInformation->CpuSet.SchedulingClass = 0;
            _pCpuSetInformation->CpuSet.AllocationTag = 0;

            _pCurrent += sizeof(SYSTEM_CPU_SET_INFORMATION);
        }

        return TRUE;
    }
#endif

} //namespace YY::Thunks
