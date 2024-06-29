
#if (YY_Thunks_Target < __WindowsNT10_10240) && !defined(__Comment_Lib_advapi32)
#define __Comment_Lib_advapi32
#pragma comment(lib, "Advapi32.lib")
#endif

#if defined(YY_Thunks_Implemented)
namespace YY::Thunks
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT10_10240)
        static SRWLOCK g_CompareObjectHandles;

        union ObjectStaticBuffer
        {
            wchar_t Buffer[1024];
            OBJECT_TYPE_INFORMATION TypeInfo;
            OBJECT_BASIC_INFORMATION BaseInfo;
            OBJECT_NAME_INFORMATION NameInfo;
            BY_HANDLE_FILE_INFORMATION FileInformation;
            TOKEN_STATISTICS TokenStatistics;
        };

        typedef bool(__fastcall* CompareObjectFunType)(HANDLE _hLeft, ObjectStaticBuffer& _LeftBuffer, HANDLE _hRigth, ObjectStaticBuffer& _RightBuffer);

        struct CompareObjectItem
        {
            UNICODE_STRING TypeName;
            CompareObjectFunType pfnCompareObject;

            template<size_t kLength>
            constexpr CompareObjectItem(const wchar_t(&_szTypeName)[kLength], CompareObjectFunType _pfnCompareObject)
                : TypeName(internal::MakeStaticUnicodeString(_szTypeName))
                , pfnCompareObject(_pfnCompareObject)
            {
            }
        };

        bool __fastcall CompareObjectRef(HANDLE _hFirstObjectHandle, ObjectStaticBuffer& _FirstObjectBuffer, HANDLE _hSecondObjectHandle, ObjectStaticBuffer& _SecondObjectBuffer) noexcept
        {
#if !defined(__USING_NTDLL_LIB)
            const auto NtQueryObject = try_get_NtQueryObject();
            if (!NtQueryObject)
                return false;
#endif

            const auto _pfnDuplicateHandle = try_get_DuplicateHandle();
            const auto _pfnCloseHandle = try_get_CloseHandle();
            if (_pfnDuplicateHandle == nullptr || _pfnCloseHandle == nullptr)
                return false;

            HANDLE _hFirstTmp = NULL;
            BOOL _bHandleIsSame = FALSE;

            ::AcquireSRWLockExclusive(&g_CompareObjectHandles);

            do
            {
                if (!_pfnDuplicateHandle(NtGetCurrentProcess(), _hFirstObjectHandle, NtGetCurrentProcess(), &_hFirstTmp, 0, FALSE, 0))
                {
                    break;
                }

                LONG _Status = NtQueryObject(_hFirstObjectHandle, ObjectBasicInformation, &_FirstObjectBuffer, sizeof(_FirstObjectBuffer.BaseInfo), nullptr);

                // 实际测试，ObjectBasicInformation 只出现无效句柄错误
                if (_Status < 0)
                {
                    break;
                }

                if (_FirstObjectBuffer.BaseInfo.HandleCount == 1)
                {
                    // 引用计数为 1，这肯定不可能与另外一个句柄是同一个内核对象
                    break;
                }

                _Status = NtQueryObject(_hSecondObjectHandle, ObjectBasicInformation, &_SecondObjectBuffer, sizeof(_SecondObjectBuffer.BaseInfo), nullptr);
                if (_Status < 0)
                {
                    break;
                }

                if (_SecondObjectBuffer.BaseInfo.HandleCount != _FirstObjectBuffer.BaseInfo.HandleCount)
                {
                    // 引用计数为 1，这肯定不可能与另外一个句柄是同一个内核对象
                    break;
                }

                _pfnCloseHandle(_hFirstTmp);
                _hFirstTmp = NULL;

                _Status = NtQueryObject(_hFirstObjectHandle, ObjectBasicInformation, &_FirstObjectBuffer, sizeof(_FirstObjectBuffer.BaseInfo), nullptr);

                // 实际测试，ObjectBasicInformation 只出现无效句柄错误
                if (_Status < 0)
                {
                    break;
                }

                if (_FirstObjectBuffer.BaseInfo.HandleCount == 1)
                {
                    // 引用计数为 1，这肯定不可能与另外一个句柄是同一个内核对象
                    break;
                }

                _Status = NtQueryObject(_hSecondObjectHandle, ObjectBasicInformation, &_SecondObjectBuffer, sizeof(_SecondObjectBuffer.BaseInfo), nullptr);
                if (_Status < 0)
                {
                    break;
                }

                if (_SecondObjectBuffer.BaseInfo.HandleCount != _FirstObjectBuffer.BaseInfo.HandleCount)
                {
                    // 引用计数为 1，这肯定不可能与另外一个句柄是同一个内核对象
                    break;
                }

                _bHandleIsSame = TRUE;
                break;
            } while (false);

            ::ReleaseSRWLockExclusive(&g_CompareObjectHandles);

            if (_hFirstTmp)
                _pfnCloseHandle(_hFirstTmp);

            return _bHandleIsSame;
        }

        bool __fastcall CompareObjectName(HANDLE _hLeft, ObjectStaticBuffer& _LeftBuffer, HANDLE _hRigth, ObjectStaticBuffer& _RightBuffer) noexcept
        {
#if !defined(__USING_NTDLL_LIB)
            const auto NtQueryObject = try_get_NtQueryObject();
            if (!NtQueryObject)
                return false;
#endif

            LONG _Status = NtQueryObject(_hLeft, ObjectNameInformation, &_LeftBuffer, sizeof(_LeftBuffer), nullptr);
            if (_Status < 0)
                return false;

            _Status = NtQueryObject(_hRigth, ObjectNameInformation, &_RightBuffer, sizeof(_RightBuffer), nullptr);
            if (_Status < 0)
                return false;

            if (_LeftBuffer.NameInfo.Name.Length == 0 && _RightBuffer.NameInfo.Name.Length == 0)
            {
                // 这是一个匿名对象，只能比较句柄的引用计数了。
                return CompareObjectRef(_hLeft, _LeftBuffer, _hRigth, _RightBuffer);
            }

            return internal::IsEqual(_LeftBuffer.NameInfo.Name, _RightBuffer.NameInfo.Name);
        }

        bool __fastcall CompareProcessHandle(HANDLE _hLeft, ObjectStaticBuffer& _LeftBuffer, HANDLE _hRigth, ObjectStaticBuffer& _RightBuffer) noexcept
        {
            decltype(DuplicateHandle)* _pfnDuplicateHandle = nullptr;
            decltype(CloseHandle)* _pfnCloseHandle = nullptr;

            auto _uFirstProcessId = GetProcessId(_hLeft);
            if (_uFirstProcessId == 0)
            {
                if (GetLastError() != ERROR_ACCESS_DENIED)
                    return false;

                _pfnDuplicateHandle = try_get_DuplicateHandle();
                _pfnCloseHandle = try_get_CloseHandle();
                if (_pfnDuplicateHandle == nullptr || _pfnCloseHandle == nullptr)
                    return false;

                HANDLE _hProcessTmp;
                if (!_pfnDuplicateHandle(NtGetCurrentProcess(), _hLeft, NtGetCurrentProcess(), &_hProcessTmp, PROCESS_QUERY_INFORMATION, FALSE, 0))
                    return false;

                _uFirstProcessId = GetProcessId(_hProcessTmp);
                _pfnCloseHandle(_hProcessTmp);
                if (_uFirstProcessId == 0)
                    return false;
            }

            auto _uSecondProcessId = GetProcessId(_hRigth);
            if (_uSecondProcessId == 0)
            {
                if (GetLastError() != ERROR_ACCESS_DENIED)
                    return false;

                if (!_pfnDuplicateHandle)
                {
                    _pfnDuplicateHandle = try_get_DuplicateHandle();
                    _pfnCloseHandle = try_get_CloseHandle();

                    if (_pfnDuplicateHandle == nullptr || _pfnCloseHandle == nullptr)
                        return false;
                }

                HANDLE _hProcessTmp;
                if (!_pfnDuplicateHandle(NtGetCurrentProcess(), _hRigth, NtGetCurrentProcess(), &_hProcessTmp, PROCESS_QUERY_INFORMATION, FALSE, 0))
                    return false;

                _uSecondProcessId = GetProcessId(_hProcessTmp);
                _pfnCloseHandle(_hProcessTmp);
                if (_uSecondProcessId == 0)
                    return false;
            }

            return _uFirstProcessId == _uSecondProcessId;
        }

        bool __fastcall CompareThreadHandle(HANDLE _hLeft, ObjectStaticBuffer& _LeftBuffer, HANDLE _hRigth, ObjectStaticBuffer& _RightBuffer) noexcept
        {
            decltype(DuplicateHandle)* _pfnDuplicateHandle = nullptr;
            decltype(CloseHandle)* _pfnCloseHandle = nullptr;

            auto _uFirstThreadId = GetThreadId(_hLeft);
            if (_uFirstThreadId == 0)
            {
                if (GetLastError() != ERROR_ACCESS_DENIED)
                    return false;

                _pfnDuplicateHandle = try_get_DuplicateHandle();
                _pfnCloseHandle = try_get_CloseHandle();
                if (_pfnDuplicateHandle == nullptr || _pfnCloseHandle == nullptr)
                    return false;

                HANDLE _hHandleTmp;
                if (!_pfnDuplicateHandle(NtGetCurrentProcess(), _hLeft, NtGetCurrentProcess(), &_hHandleTmp, THREAD_QUERY_INFORMATION, FALSE, 0))
                    return false;

                _uFirstThreadId = GetThreadId(_hHandleTmp);
                _pfnCloseHandle(_hHandleTmp);
                if (_uFirstThreadId == 0)
                    return false;
            }

            auto _uSecondThreadId = GetThreadId(_hRigth);
            if (_uSecondThreadId == 0)
            {
                if (GetLastError() != ERROR_ACCESS_DENIED)
                    return false;

                if (!_pfnDuplicateHandle)
                {
                    _pfnDuplicateHandle = try_get_DuplicateHandle();
                    _pfnCloseHandle = try_get_CloseHandle();

                    if (_pfnDuplicateHandle == nullptr || _pfnCloseHandle == nullptr)
                        return false;
                }

                HANDLE _hHandleTmp;
                if (!_pfnDuplicateHandle(NtGetCurrentProcess(), _hRigth, NtGetCurrentProcess(), &_hHandleTmp, THREAD_QUERY_INFORMATION, FALSE, 0))
                {
                    return false;
                }

                _uSecondThreadId = GetThreadId(_hHandleTmp);
                _pfnCloseHandle(_hHandleTmp);
                if (_uSecondThreadId == 0)
                    return false;
            }

            return _uFirstThreadId == _uSecondThreadId;
        }

        bool __fastcall CompareFileHandle(HANDLE _hFirstObjectHandle, ObjectStaticBuffer& _FirstObjectBuffer, HANDLE _hSecondObjectHandle, ObjectStaticBuffer& _SecondObjectBuffer) noexcept
        {
            // 提高判断效率，因为CompareObjectRef存在阻塞。
            // 如果是同一个对象，那么必然是同一个文件。当然是同一个文件不一定就是同一个对象。
            const auto _bRet1 = GetFileInformationByHandle(_hFirstObjectHandle, &_FirstObjectBuffer.FileInformation);
            const auto _bRet2 = GetFileInformationByHandle(_hSecondObjectHandle, &_SecondObjectBuffer.FileInformation);

            if (_bRet1 != _bRet2)
                return false;

            if (_bRet1)
            {
                if (_FirstObjectBuffer.FileInformation.dwVolumeSerialNumber != _SecondObjectBuffer.FileInformation.dwVolumeSerialNumber
                    || _FirstObjectBuffer.FileInformation.nFileIndexHigh != _SecondObjectBuffer.FileInformation.nFileIndexHigh
                    || _FirstObjectBuffer.FileInformation.nFileIndexLow != _SecondObjectBuffer.FileInformation.nFileIndexLow)
                {
                    return false;
                }
            }

            return CompareObjectRef(_hFirstObjectHandle, _FirstObjectBuffer, _hSecondObjectHandle, _SecondObjectBuffer);
        }

        bool __fastcall CompareTokenHandle(HANDLE _hFirstObjectHandle, ObjectStaticBuffer& _FirstObjectBuffer, HANDLE _hSecondObjectHandle, ObjectStaticBuffer& _SecondObjectBuffer) noexcept
        {
            decltype(DuplicateHandle)* _pfnDuplicateHandle = nullptr;
            decltype(CloseHandle)* _pfnCloseHandle = nullptr;

            DWORD _uResultLength;
            if (!GetTokenInformation(_hFirstObjectHandle, TokenStatistics, &_FirstObjectBuffer.TokenStatistics, sizeof(_FirstObjectBuffer.TokenStatistics), &_uResultLength))
            {
                if (GetLastError() != ERROR_ACCESS_DENIED)
                    return false;

                _pfnDuplicateHandle = try_get_DuplicateHandle();
                _pfnCloseHandle = try_get_CloseHandle();
                if (_pfnDuplicateHandle == nullptr || _pfnCloseHandle == nullptr)
                    return false;

                HANDLE _hTmp;
                if (!_pfnDuplicateHandle(NtCurrentProcess(), _hFirstObjectHandle, NtCurrentProcess(), &_hTmp, TOKEN_QUERY, FALSE, 0))
                    return false;

                const auto _bRet = GetTokenInformation(_hTmp, TokenStatistics, &_FirstObjectBuffer.TokenStatistics, sizeof(_FirstObjectBuffer.TokenStatistics), &_uResultLength);
                _pfnCloseHandle(_hTmp);
                if (!_bRet)
                    return false;
            }

            if (!GetTokenInformation(_hSecondObjectHandle, TokenStatistics, &_SecondObjectBuffer.TokenStatistics, sizeof(_SecondObjectBuffer.TokenStatistics), &_uResultLength))
            {
                if (GetLastError() != ERROR_ACCESS_DENIED)
                    return false;

                if (!_pfnDuplicateHandle)
                {
                    _pfnDuplicateHandle = try_get_DuplicateHandle();
                    _pfnCloseHandle = try_get_CloseHandle();
                    if (_pfnDuplicateHandle == nullptr || _pfnCloseHandle == nullptr)
                        return false;
                }
                HANDLE _hTmp;
                if (!_pfnDuplicateHandle(NtCurrentProcess(), _hSecondObjectHandle, NtCurrentProcess(), &_hTmp, TOKEN_QUERY, FALSE, 0))
                    return false;

                const auto _bRet = GetTokenInformation(_hTmp, TokenStatistics, &_SecondObjectBuffer.TokenStatistics, sizeof(_SecondObjectBuffer.TokenStatistics), &_uResultLength);
                _pfnCloseHandle(_hTmp);
                if (!_bRet)
                    return false;
            }

            return _FirstObjectBuffer.TokenStatistics.TokenId.LowPart == _SecondObjectBuffer.TokenStatistics.TokenId.LowPart
                && _FirstObjectBuffer.TokenStatistics.TokenId.HighPart == _SecondObjectBuffer.TokenStatistics.TokenId.HighPart;
        }
#endif
    }
}
#endif

namespace YY::Thunks
{

#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端	Windows 2000 专业版 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows 2000 Server[桌面应用 | UWP 应用]
    // 虽然2000就支持，但是为了实现CompareObjectHandles，目前需要处理一下关闭。
    __DEFINE_THUNK(
    kernel32,
    4,
    BOOL,
    WINAPI,
    CloseHandle,
        _In_ _Post_ptr_invalid_ HANDLE _hObject
        )
    {
        const auto _pfnCloseHandle = try_get_CloseHandle();
        const auto _pfnCompareObjectHandles = try_get_CompareObjectHandles();

        if (_pfnCompareObjectHandles == nullptr && _hObject && _hObject != INVALID_HANDLE_VALUE)
        {
            ::AcquireSRWLockShared(&g_CompareObjectHandles);
        }
        // 空指针故意崩溃
        auto _bRet = _pfnCloseHandle(_hObject);

        if (_pfnCompareObjectHandles == nullptr && _hObject && _hObject != INVALID_HANDLE_VALUE)
        {
            auto _lStatus = GetLastError();
            ::ReleaseSRWLockShared(&g_CompareObjectHandles);
            SetLastError(_lStatus);
        }

        return _bRet;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端	Windows 2000 专业版 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows 2000 Server[桌面应用 | UWP 应用]
    // 虽然2000就支持，但是为了实现CompareObjectHandles，目前需要处理一下关闭。
    __DEFINE_THUNK(
    kernel32,
    28,
    BOOL,
    WINAPI,
    DuplicateHandle,
        _In_ HANDLE _hSourceProcessHandle,
        _In_ HANDLE _hSourceHandle,
        _In_ HANDLE _hTargetProcessHandle,
        _Outptr_ LPHANDLE _phTargetHandle,
        _In_ DWORD _fDesiredAccess,
        _In_ BOOL _bInheritHandle,
        _In_ DWORD _uOptions
        )
    {
        const auto _pfnDuplicateHandle = try_get_DuplicateHandle();
        const auto _pfnCompareObjectHandles = try_get_CompareObjectHandles();

        bool _bNeedLock = false;
        if (_pfnCompareObjectHandles)
        {
            // 无须加锁
        }
        if (_hTargetProcessHandle == NtGetCurrentProcess() || GetProcessId(_hTargetProcessHandle) == GetCurrentProcessId())
        {
            _bNeedLock = true;
        }
        else if ((DUPLICATE_CLOSE_SOURCE & _uOptions)
            && (_hSourceProcessHandle == NtGetCurrentProcess() || GetProcessId(_hSourceProcessHandle) == GetCurrentProcessId()))
        {
            _bNeedLock = true;
        }

        if (_bNeedLock)
        {
            ::AcquireSRWLockShared(&g_CompareObjectHandles);
        }
        
        // 空指针故意崩溃
        auto _bRet = _pfnDuplicateHandle(_hSourceProcessHandle, _hSourceHandle, _hTargetProcessHandle, _phTargetHandle, _fDesiredAccess, _bInheritHandle, _uOptions);

        if (_bNeedLock)
        {
            auto _lStatus = GetLastError();
            ::ReleaseSRWLockShared(&g_CompareObjectHandles);
            SetLastError(_lStatus);
        }
        return _bRet;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端	Windows 10 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2016[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    api_ms_win_core_handle_l1_1_0,
    8,
    BOOL,
    WINAPI,
    CompareObjectHandles,
        _In_ HANDLE _hFirstObjectHandle,
        _In_ HANDLE _hSecondObjectHandle
        )
    {
        if (const auto _pfnCompareObjectHandles = try_get_CompareObjectHandles())
        {
            return _pfnCompareObjectHandles(_hFirstObjectHandle, _hSecondObjectHandle);
        }

        ObjectStaticBuffer _FirstObjectBuffer;
        ObjectStaticBuffer _SecondObjectBuffer;

        if (_hFirstObjectHandle == _hSecondObjectHandle)
        {
            if (NtGetCurrentProcess() == _hFirstObjectHandle
                || NtGetCurrentThread() == _hFirstObjectHandle)
            {
                return TRUE;
            }

#if !defined(__USING_NTDLL_LIB)
            const auto NtQueryObject = try_get_NtQueryObject();
            if (!NtQueryObject)
                return FALSE;
#endif

            // 用来检测句柄是否合法
            LONG _Status = NtQueryObject(_hFirstObjectHandle, ObjectBasicInformation, &_FirstObjectBuffer, sizeof(_FirstObjectBuffer.BaseInfo), nullptr);
            if (_Status < 0)
            {
                return FALSE;
            }
            return TRUE;
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryObject = try_get_NtQueryObject();
        if (!NtQueryObject)
            return FALSE;
#endif

        LONG _Status = NtQueryObject(_hFirstObjectHandle, ObjectBasicInformation, &_FirstObjectBuffer, sizeof(_FirstObjectBuffer.BaseInfo), nullptr);

        // 实际测试，ObjectBasicInformation 只出现无效句柄错误
        if (_Status < 0)
        {
            return FALSE;
        }

        if (_FirstObjectBuffer.BaseInfo.HandleCount == 1)
        {
            // 引用计数为 1，这肯定不可能与另外一个句柄是同一个内核对象
            return FALSE;
        }

        _Status = NtQueryObject(_hSecondObjectHandle, ObjectBasicInformation, &_SecondObjectBuffer, sizeof(_SecondObjectBuffer.BaseInfo), nullptr);
        if (_Status < 0)
        {
            return FALSE;
        }

        if (_SecondObjectBuffer.BaseInfo.HandleCount == 1)
        {
            // 引用计数为 1，这肯定不可能与另外一个句柄是同一个内核对象
            return FALSE;
        }

        if (_FirstObjectBuffer.BaseInfo.NameInfoSize != _SecondObjectBuffer.BaseInfo.NameInfoSize
            || _FirstObjectBuffer.BaseInfo.TypeInfoSize != _SecondObjectBuffer.BaseInfo.TypeInfoSize)
        {
            return FALSE;
        }

        _Status = NtQueryObject(_hFirstObjectHandle, ObjectTypeInformation, &_FirstObjectBuffer, sizeof(_FirstObjectBuffer), nullptr);
        if (_Status < 0)
        {
            return FALSE;
        }
        _Status = NtQueryObject(_hSecondObjectHandle, ObjectTypeInformation, &_SecondObjectBuffer, sizeof(_SecondObjectBuffer), nullptr);
        if (_Status < 0)
        {
            return FALSE;
        }

        // 同一个内核句柄，必然拥有相同的TypeName。
        if (!internal::IsEqual(_FirstObjectBuffer.TypeInfo.TypeName, _SecondObjectBuffer.TypeInfo.TypeName))
        {
            return FALSE;
        }

        // 我们不直接比较句柄的Name，因为某些句柄获取Name会导致阻塞，而比如文件，文件相同也并非意味着是同一对象。
        // 我们也不能无脑判断ObjectRef，因为它也带有锁，而且无法完全保证其他代码不复制句柄。
        // 下面这个表格存储了所有已知的内核对象快速判断是否同一对象的方法。
        static const CompareObjectItem s_CompareObjectTable[] =
        {
            { L"Event", &CompareObjectName },
            { L"Process", &CompareProcessHandle },
            { L"Thread", &CompareThreadHandle },
            { L"Section", &CompareObjectName },
            { L"Mutant", &CompareObjectName },
            { L"Semaphore", &CompareObjectName },
            { L"File", &CompareFileHandle },
            { L"WindowStation", &CompareObjectName },
            { L"Desktop", &CompareObjectName },
            { L"Directory", &CompareObjectName },
            { L"Timer", &CompareObjectName },
            { L"KeyedEvent", &CompareObjectName },
            { L"Token", &CompareTokenHandle },
            { L"ALPC Port", &CompareObjectName },
        };

        for (auto& _CompareObject : s_CompareObjectTable)
        {
            if (internal::IsEqualI(_FirstObjectBuffer.TypeInfo.TypeName, _CompareObject.TypeName))
            {
                return _CompareObject.pfnCompareObject(_hFirstObjectHandle, _FirstObjectBuffer, _hSecondObjectHandle, _SecondObjectBuffer);
            }
        }

        // 保底逻辑，判断二个句柄的引用计数是否同步。但是需要注意的是该方法比较缓慢，因为需要加锁！如果这里有更好方案，欢迎PR。
        // Key : 重复打开时不是同一个内核对象
        // CreateIoCompletionPort : 无法获取名称
        // TmTx : 无法获取名称
        return CompareObjectRef(_hFirstObjectHandle, _FirstObjectBuffer, _hSecondObjectHandle, _SecondObjectBuffer);
    }
#endif
}
