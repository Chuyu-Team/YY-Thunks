
#ifdef YY_Thunks_Implemented
namespace YY::Thunks
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT6)
        enum _FILE_ID_TYPE_win7
        {
            FileIdType,
            ObjectIdType,
            MaximumFileIdType
        };

        struct FILE_ID_DESCRIPTOR_win7
        {
            DWORD dwSize;  // Size of the struct
            FILE_ID_TYPE Type; // Describes the type of identifier passed in.
            union {
                LARGE_INTEGER FileId;
                GUID ObjectId;
            } DUMMYUNIONNAME;
        };
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)
        struct CopyFileExToCopyFile2CallbackContext
        {
            COPYFILE2_EXTENDED_PARAMETERS* pExtendedParameters = nullptr;
            uint32_t uCurrentStreamNumber = 0;
            uint64_t uCurrentChunkNumber = 0;
            uint64_t uStreamBytesTransferred = 0;
        };

        static constexpr DWORD __fastcall CopyFile2ActionToResult(_In_ COPYFILE2_MESSAGE_ACTION _eAction)
        {
            switch (_eAction)
            {
            case COPYFILE2_PROGRESS_CONTINUE:
                return PROGRESS_CONTINUE;
            case COPYFILE2_PROGRESS_STOP:
            case COPYFILE2_PROGRESS_PAUSE:
                return PROGRESS_STOP;
            case COPYFILE2_PROGRESS_QUIET:
                return PROGRESS_QUIET;
            case COPYFILE2_PROGRESS_CANCEL:
            default:
                return PROGRESS_CANCEL;
            }
        }

        static DWORD WINAPI CopyFileExToCopyFile2Callback(
            _In_     LARGE_INTEGER TotalFileSize,
            _In_     LARGE_INTEGER TotalBytesTransferred,
            _In_     LARGE_INTEGER StreamSize,
            _In_     LARGE_INTEGER StreamBytesTransferred,
            _In_     DWORD dwStreamNumber,
            _In_     DWORD dwCallbackReason,
            _In_     HANDLE hSourceFile,
            _In_     HANDLE hDestinationFile,
            _In_opt_ LPVOID lpData
            )
        {
            auto _pContext = reinterpret_cast<CopyFileExToCopyFile2CallbackContext*>(lpData);
            COPYFILE2_MESSAGE _oMessage = {};

            COPYFILE2_MESSAGE_ACTION _eAction = COPYFILE2_PROGRESS_CONTINUE;

            do
            {
                if (dwCallbackReason == CALLBACK_CHUNK_FINISHED)
                {
                    const auto _uCurrentChunkNumber = _pContext->uCurrentChunkNumber;
                    const auto _uChunkSize = StreamBytesTransferred.QuadPart - _pContext->uStreamBytesTransferred;
                    _pContext->uCurrentChunkNumber += 1;
                    _pContext->uStreamBytesTransferred = StreamBytesTransferred.QuadPart;

                    _oMessage.Type = COPYFILE2_CALLBACK_CHUNK_STARTED;
                    _oMessage.Info.ChunkStarted.dwStreamNumber = dwStreamNumber;
                    _oMessage.Info.ChunkStarted.hSourceFile = hSourceFile;
                    _oMessage.Info.ChunkStarted.hDestinationFile = hDestinationFile;
                    _oMessage.Info.ChunkStarted.uliChunkNumber.QuadPart = _uCurrentChunkNumber;
                    _oMessage.Info.ChunkStarted.uliChunkSize.QuadPart = _uChunkSize;
                    _oMessage.Info.ChunkStarted.uliStreamSize.QuadPart = StreamSize.QuadPart;
                    _oMessage.Info.ChunkStarted.uliTotalFileSize.QuadPart = TotalFileSize.QuadPart;
                    _eAction = _pContext->pExtendedParameters->pProgressRoutine(&_oMessage, _pContext->pExtendedParameters->pvCallbackContext);
                    if (_eAction != COPYFILE2_PROGRESS_CONTINUE)
                        break;

                    _oMessage.Type = COPYFILE2_CALLBACK_CHUNK_FINISHED;
                    _oMessage.Info.ChunkFinished.dwStreamNumber = dwStreamNumber;
                    _oMessage.Info.ChunkFinished.hSourceFile = hSourceFile;
                    _oMessage.Info.ChunkFinished.hDestinationFile = hDestinationFile;
                    _oMessage.Info.ChunkFinished.uliChunkNumber.QuadPart = _uCurrentChunkNumber;
                    _oMessage.Info.ChunkFinished.uliChunkSize.QuadPart = _uChunkSize;
                    _oMessage.Info.ChunkFinished.uliStreamSize.QuadPart = StreamSize.QuadPart;
                    _oMessage.Info.ChunkFinished.uliStreamBytesTransferred.QuadPart = StreamBytesTransferred.QuadPart;
                    _oMessage.Info.ChunkFinished.uliTotalFileSize.QuadPart = TotalFileSize.QuadPart;
                    _oMessage.Info.ChunkFinished.uliTotalBytesTransferred.QuadPart = TotalBytesTransferred.QuadPart;
                    _eAction = _pContext->pExtendedParameters->pProgressRoutine(&_oMessage, _pContext->pExtendedParameters->pvCallbackContext);
                    if (_eAction != COPYFILE2_PROGRESS_CONTINUE)
                        break;

                    if (StreamSize.QuadPart != StreamBytesTransferred.QuadPart)
                    {
                        break;
                    }

                    _oMessage.Type = COPYFILE2_CALLBACK_STREAM_FINISHED;
                    _oMessage.Info.StreamFinished.dwStreamNumber = dwStreamNumber;
                    _oMessage.Info.StreamFinished.hSourceFile = hSourceFile;
                    _oMessage.Info.StreamFinished.hDestinationFile = hDestinationFile;
                    _oMessage.Info.StreamFinished.uliStreamSize.QuadPart = StreamSize.QuadPart;
                    _oMessage.Info.StreamFinished.uliStreamBytesTransferred.QuadPart = StreamBytesTransferred.QuadPart;
                    _oMessage.Info.StreamFinished.uliTotalFileSize.QuadPart = TotalFileSize.QuadPart;
                    _oMessage.Info.StreamFinished.uliTotalBytesTransferred.QuadPart = TotalBytesTransferred.QuadPart;
                    _eAction = _pContext->pExtendedParameters->pProgressRoutine(&_oMessage, _pContext->pExtendedParameters->pvCallbackContext);
                    return CopyFile2ActionToResult(_eAction);
                }
                else if (dwCallbackReason == CALLBACK_STREAM_SWITCH)
                {
                    _pContext->uCurrentStreamNumber = dwStreamNumber;
                    _pContext->uCurrentChunkNumber = 0;
                    _pContext->uStreamBytesTransferred = StreamBytesTransferred.QuadPart;

                    _oMessage.Type = COPYFILE2_CALLBACK_STREAM_STARTED;
                    _oMessage.Info.StreamStarted.dwStreamNumber = dwStreamNumber;
                    _oMessage.Info.StreamStarted.hSourceFile = hSourceFile;
                    _oMessage.Info.StreamStarted.hDestinationFile = hDestinationFile;
                    _oMessage.Info.StreamStarted.uliStreamSize.QuadPart = StreamSize.QuadPart;
                    _oMessage.Info.StreamStarted.uliTotalFileSize.QuadPart = TotalFileSize.QuadPart;
                    _eAction = _pContext->pExtendedParameters->pProgressRoutine(&_oMessage, _pContext->pExtendedParameters->pvCallbackContext);
                }
                else
                {
                    _oMessage.Type = COPYFILE2_CALLBACK_POLL_CONTINUE;
                    _oMessage.Info.PollContinue.dwReserved = 0;
                    _eAction = _pContext->pExtendedParameters->pProgressRoutine(&_oMessage, _pContext->pExtendedParameters->pvCallbackContext);
                }
            } while (false);

            const auto _uResult = CopyFile2ActionToResult(_eAction);
            if (_uResult == PROGRESS_STOP)
            {
                _oMessage.Type = COPYFILE2_CALLBACK_STREAM_FINISHED;
                _oMessage.Info.StreamFinished.dwStreamNumber = dwStreamNumber;
                _oMessage.Info.StreamFinished.hSourceFile = hSourceFile;
                _oMessage.Info.StreamFinished.hDestinationFile = hDestinationFile;
                _oMessage.Info.StreamFinished.uliStreamSize.QuadPart = StreamSize.QuadPart;
                _oMessage.Info.StreamFinished.uliStreamBytesTransferred.QuadPart = StreamBytesTransferred.QuadPart;
                _oMessage.Info.StreamFinished.uliTotalFileSize.QuadPart = TotalFileSize.QuadPart;
                _oMessage.Info.StreamFinished.uliTotalBytesTransferred.QuadPart = TotalBytesTransferred.QuadPart;
                _pContext->pExtendedParameters->pProgressRoutine(&_oMessage, _pContext->pExtendedParameters->pvCallbackContext);
            }

            return _uResult;
        }
#endif
    }
}
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    16,
    _Success_(return)
    BOOL,
    WINAPI,
    GetFileInformationByHandleEx,
        _In_  HANDLE hFile,
        _In_  FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
        _Out_writes_bytes_(dwBufferSize) LPVOID lpFileInformation,
        _In_  DWORD dwBufferSize
        )
    {
        if (auto const pGetFileInformationByHandleEx = try_get_GetFileInformationByHandleEx())
        {
            return pGetFileInformationByHandleEx(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
        }


        FILE_INFORMATION_CLASS NtFileInformationClass;
        DWORD cbMinBufferSize;
        bool bNtQueryDirectoryFile = false;
        BOOLEAN RestartScan = false;

        switch (FileInformationClass)
        {
        case FileBasicInfo:
            NtFileInformationClass = FileBasicInformation;
            cbMinBufferSize = sizeof(FILE_BASIC_INFO);
            break;
        case FileStandardInfo:
            NtFileInformationClass = FileStandardInformation;
            cbMinBufferSize = sizeof(FILE_STANDARD_INFO);
            break;
        case FileNameInfo:
            NtFileInformationClass = FileNameInformation;
            cbMinBufferSize = sizeof(FILE_NAME_INFO);
            break;
        case FileStreamInfo:
            NtFileInformationClass = FileStreamInformation;
            cbMinBufferSize = sizeof(FILE_STREAM_INFO);
            break;
        case FileCompressionInfo:
            NtFileInformationClass = FileCompressionInformation;
            cbMinBufferSize = sizeof(FILE_COMPRESSION_INFO);
            break;
        case FileAttributeTagInfo:
            NtFileInformationClass = FileAttributeTagInformation;
            cbMinBufferSize = sizeof(FILE_ATTRIBUTE_TAG_INFO);
            break;
        case FileIdBothDirectoryRestartInfo:
            RestartScan = true;
        case FileIdBothDirectoryInfo:
            NtFileInformationClass = FileIdBothDirectoryInformation;
            cbMinBufferSize = sizeof(FILE_ID_BOTH_DIR_INFO);
            bNtQueryDirectoryFile = true;
            break;
        case FileRemoteProtocolInfo:
            NtFileInformationClass = FileRemoteProtocolInformation;
            cbMinBufferSize = sizeof(FILE_REMOTE_PROTOCOL_INFO);
            break;
        default:
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
            break;
        }


        if (cbMinBufferSize > dwBufferSize)
        {
            SetLastError(ERROR_BAD_LENGTH);
            return FALSE;
        }

        IO_STATUS_BLOCK IoStatusBlock;
        NTSTATUS Status;

        if (bNtQueryDirectoryFile)
        {
#if !defined(__USING_NTDLL_LIB)
            const auto NtQueryDirectoryFile = try_get_NtQueryDirectoryFile();
            if (!NtQueryDirectoryFile)
            {
                SetLastError(ERROR_INVALID_FUNCTION);
                return FALSE;
            }
#endif

            Status = NtQueryDirectoryFile(
                hFile,
                nullptr,
                nullptr,
                nullptr,
                &IoStatusBlock,
                lpFileInformation,
                dwBufferSize,
                NtFileInformationClass,
                false,
                nullptr,
                RestartScan
                );

            if (STATUS_PENDING == Status)
            {
                if (WaitForSingleObjectEx(hFile, 0, FALSE) == WAIT_FAILED)
                {
                    //WaitForSingleObjectEx会设置LastError
                    return FALSE;
                }

                Status = IoStatusBlock.Status;
            }
        }
        else
        {
#if !defined(__USING_NTDLL_LIB)
            const auto NtQueryInformationFile = try_get_NtQueryInformationFile();
            if (!NtQueryInformationFile)
            {
                SetLastError(ERROR_INVALID_FUNCTION);
                return FALSE;
            }
#endif

            Status = NtQueryInformationFile(hFile, &IoStatusBlock, lpFileInformation, dwBufferSize, NtFileInformationClass);
        }

        if (Status >= STATUS_SUCCESS)
        {
            if (FileStreamInfo == FileInformationClass && IoStatusBlock.Information == 0)
            {
                SetLastError(ERROR_HANDLE_EOF);
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
        else
        {
            internal::BaseSetLastNTError(Status);
            return FALSE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    SetFileInformationByHandle,
        _In_ HANDLE hFile,
        _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
        _In_reads_bytes_(dwBufferSize) LPVOID lpFileInformation,
        _In_ DWORD dwBufferSize
        )
    {
        if (auto pSetFileInformationByHandle = try_get_SetFileInformationByHandle())
        {
            return pSetFileInformationByHandle(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtSetInformationFile = try_get_NtSetInformationFile();
        if (!NtSetInformationFile)
        {
            SetLastError(ERROR_INVALID_FUNCTION);
            return FALSE;
        }
#endif

        const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
        FILE_INFORMATION_CLASS NtFileInformationClass;
        DWORD cbMinBufferSize;
        bool bFreeFileInformation = false;

        switch (FileInformationClass)
        {
        case FileBasicInfo:
            NtFileInformationClass = FileBasicInformation;
            cbMinBufferSize = sizeof(FILE_BASIC_INFO);
            break;
        case FileRenameInfo:
            NtFileInformationClass = FileRenameInformation;
            cbMinBufferSize = sizeof(FILE_RENAME_INFO);

            if (cbMinBufferSize > dwBufferSize)
            {
                SetLastError(ERROR_BAD_LENGTH);
                return FALSE;
            }

            if (lpFileInformation == nullptr)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }
            else
            {
                auto pRenameInfo = (FILE_RENAME_INFO*)lpFileInformation;

                if (pRenameInfo->FileNameLength < sizeof(wchar_t) || pRenameInfo->FileName[0] != L':')
                {
#if !defined(__USING_NTDLL_LIB)
                    const auto RtlDosPathNameToNtPathName_U = try_get_RtlDosPathNameToNtPathName_U();
                    const auto RtlFreeUnicodeString = try_get_RtlFreeUnicodeString();

                    if (RtlDosPathNameToNtPathName_U == nullptr || RtlFreeUnicodeString == nullptr)
                    {
                        SetLastError(ERROR_INVALID_FUNCTION);
                        return FALSE;
                    }
#endif

                    UNICODE_STRING NtName = {};


                    if (!RtlDosPathNameToNtPathName_U(pRenameInfo->FileName, &NtName, nullptr, nullptr))
                    {
                        SetLastError(ERROR_INVALID_PARAMETER);

                        return FALSE;
                    }

                    DWORD dwNewBufferSize = sizeof(FILE_RENAME_INFO) + NtName.Length;

                    auto NewRenameInfo = (FILE_RENAME_INFO*)HeapAlloc(ProcessHeap, 0, dwNewBufferSize);
                    if (!NewRenameInfo)
                    {
                        auto lStatus = GetLastError();

                        RtlFreeUnicodeString(&NtName);

                        SetLastError(lStatus);
                        return FALSE;
                    }

                    bFreeFileInformation = true;

                    NewRenameInfo->ReplaceIfExists = pRenameInfo->ReplaceIfExists;
                    NewRenameInfo->RootDirectory = pRenameInfo->RootDirectory;
                    NewRenameInfo->FileNameLength = NtName.Length;

                    memcpy(NewRenameInfo->FileName, NtName.Buffer, NtName.Length);

                    *(wchar_t*)((byte*)NewRenameInfo->FileName + NtName.Length) = L'\0';


                    lpFileInformation = NewRenameInfo;
                    dwBufferSize = dwNewBufferSize;

                    RtlFreeUnicodeString(&NtName);
                }
            }
            break;
        case FileDispositionInfo:
            NtFileInformationClass = FileDispositionInformation;
            cbMinBufferSize = sizeof(FILE_DISPOSITION_INFO);
            break;
        case FileAllocationInfo:
            NtFileInformationClass = FileAllocationInformation;
            cbMinBufferSize = sizeof(FILE_ALLOCATION_INFO);
            break;
        case FileEndOfFileInfo:
            NtFileInformationClass = FileEndOfFileInformation;
            cbMinBufferSize = sizeof(FILE_END_OF_FILE_INFO);
            break;
        case FileIoPriorityHintInfo:
            NtFileInformationClass = FileIoPriorityHintInformation;
            cbMinBufferSize = sizeof(FILE_IO_PRIORITY_HINT_INFO);

            //长度检查，微软原版似乎没有该安全检查
            if (cbMinBufferSize > dwBufferSize)
            {
                SetLastError(ERROR_BAD_LENGTH);
                return FALSE;
            }

            if (lpFileInformation == nullptr || ((FILE_IO_PRIORITY_HINT_INFO*)lpFileInformation)->PriorityHint >= MaximumIoPriorityHintType)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return FALSE;
            }

            break;
        default:
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
            break;
        }

        if (cbMinBufferSize > dwBufferSize)
        {
            if (bFreeFileInformation)
            {
                HeapFree(ProcessHeap, 0, lpFileInformation);
            }

            SetLastError(ERROR_BAD_LENGTH);
            return FALSE;
        }

        IO_STATUS_BLOCK IoStatusBlock;

        LONG Status = NtSetInformationFile(hFile, &IoStatusBlock, lpFileInformation, dwBufferSize, NtFileInformationClass);

        if (bFreeFileInformation)
        {
            HeapFree(ProcessHeap, 0, lpFileInformation);
        }

        if (Status >= STATUS_SUCCESS)
            return TRUE;
    
        internal::BaseSetLastNTError(Status);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    16,
    DWORD,
    WINAPI,
    GetFinalPathNameByHandleW,
        _In_ HANDLE hFile,
        _Out_writes_(cchFilePath) LPWSTR lpszFilePath,
        _In_ DWORD cchFilePath,
        _In_ DWORD dwFlags
        )
    {
        if (auto pGetFinalPathNameByHandleW = try_get_GetFinalPathNameByHandleW())
        {
            return pGetFinalPathNameByHandleW(hFile, lpszFilePath, cchFilePath, dwFlags);
        }

        //参数检查
        if (INVALID_HANDLE_VALUE == hFile)
        {
            SetLastError(ERROR_INVALID_HANDLE);
            return 0;
        }


        switch (dwFlags & (VOLUME_NAME_DOS | VOLUME_NAME_GUID | VOLUME_NAME_NONE | VOLUME_NAME_NT))
        {
        case VOLUME_NAME_DOS:
            break;
        case VOLUME_NAME_GUID:
            break;
        case VOLUME_NAME_NT:
            break;
        case VOLUME_NAME_NONE:
            break;
        default:
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
            break;
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtQueryObject = try_get_NtQueryObject();
        const auto NtQueryInformationFile = try_get_NtQueryInformationFile();

        if (nullptr == NtQueryObject
            || nullptr == NtQueryInformationFile)
        {
            SetLastError(ERROR_INVALID_FUNCTION);
            return 0;
        }
#endif

        UNICODE_STRING VolumeNtName = {};

        wchar_t szVolumeRoot[MAX_PATH];
        szVolumeRoot[0] = L'\0';

        wchar_t* szLongPathNameBuffer = nullptr;
    
        //目标所需的分区名称，不包含最后的 '\\'
        UNICODE_STRING TargetVolumeName = {};
        //目标所需的文件名，开始包含 '\\'
        UNICODE_STRING TargetFileName = {};

        const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
        LSTATUS lStatus = ERROR_SUCCESS;
        DWORD   cchReturn = 0;

        OBJECT_NAME_INFORMATION* pObjectName = nullptr;
        ULONG cbObjectName = 528;

        FILE_NAME_INFORMATION* pFileNameInfo = nullptr;
        ULONG cbFileNameInfo = 528;

        for (;;)
        {
            if (pObjectName)
            {
                auto pNewBuffer = (OBJECT_NAME_INFORMATION*)HeapReAlloc(ProcessHeap, 0, pObjectName, cbObjectName);

                if (!pNewBuffer)
                {
                    lStatus = ERROR_NOT_ENOUGH_MEMORY;
                    goto __Exit;
                }

                pObjectName = pNewBuffer;
            }
            else
            {
                pObjectName = (OBJECT_NAME_INFORMATION*)HeapAlloc(ProcessHeap, 0, cbObjectName);

                if (!pObjectName)
                {
                    //内存不足？
                    lStatus = ERROR_NOT_ENOUGH_MEMORY;
                    goto __Exit;
                }
            }

            LONG Status = NtQueryObject(hFile, ObjectNameInformation, pObjectName, cbObjectName, &cbObjectName);

            if (STATUS_BUFFER_OVERFLOW == Status)
            {
                continue;
            }
            else if (Status < 0)
            {
                lStatus = internal::NtStatusToDosError(Status);

                goto __Exit;
            }
            else
            {
                break;
            }
        }

        for (;;)
        {
            if (pFileNameInfo)
            {
                auto pNewBuffer = (FILE_NAME_INFORMATION*)HeapReAlloc(ProcessHeap, 0, pFileNameInfo, cbFileNameInfo);
                if (!pNewBuffer)
                {
                    lStatus = ERROR_NOT_ENOUGH_MEMORY;
                    goto __Exit;
                }

                pFileNameInfo = pNewBuffer;
            }
            else
            {
                pFileNameInfo = (FILE_NAME_INFORMATION*)HeapAlloc(ProcessHeap, 0, cbFileNameInfo);

                if (!pFileNameInfo)
                {
                    //内存不足？
                    lStatus = ERROR_NOT_ENOUGH_MEMORY;
                    goto __Exit;
                }
            }

            IO_STATUS_BLOCK IoStatusBlock;

            LONG Status = NtQueryInformationFile(hFile, &IoStatusBlock, pFileNameInfo, cbFileNameInfo, FileNameInformation);

            if (STATUS_BUFFER_OVERFLOW == Status)
            {
                cbFileNameInfo = pFileNameInfo->FileNameLength + sizeof(FILE_NAME_INFORMATION);
                continue;
            }
            else if (Status < 0)
            {
                lStatus = internal::NtStatusToDosError(Status);

                goto __Exit;
            }
            else
            {
                break;
            }
        }

        if (pFileNameInfo->FileName[0] != '\\')
        {
            lStatus = ERROR_ACCESS_DENIED;
            goto __Exit;
        }



        if (pFileNameInfo->FileNameLength >= pObjectName->Name.Length)
        {
            lStatus = ERROR_BAD_PATHNAME;
            goto __Exit;
        }

        {
            const auto _uNewLength = pObjectName->Name.Length - pFileNameInfo->FileNameLength + sizeof(wchar_t);
            if (_uNewLength > MAXUINT16)
            {
                lStatus = ERROR_BAD_PATHNAME;
                goto __Exit;
            }
            VolumeNtName.Buffer = pObjectName->Name.Buffer;
            VolumeNtName.Length = VolumeNtName.MaximumLength = static_cast<USHORT>(_uNewLength);
        }


        if (VOLUME_NAME_NT & dwFlags)
        {
            //返回NT路径
            TargetVolumeName.Buffer = VolumeNtName.Buffer;
            TargetVolumeName.Length = TargetVolumeName.MaximumLength = VolumeNtName.Length - sizeof(wchar_t);
        }
        else if (VOLUME_NAME_NONE & dwFlags)
        {
            //仅返回文件名
        }
        else
        {
            if (VOLUME_NAME_GUID & dwFlags)
            {
                //返回分区GUID名称
                if (!internal::BasepGetVolumeGUIDFromNTName(&VolumeNtName, szVolumeRoot))
                {
                    lStatus = GetLastError();
                    goto __Exit;
                }
            }
            else
            {
                //返回Dos路径
                if (!internal::BasepGetVolumeDosLetterNameFromNTName(&VolumeNtName, szVolumeRoot))
                {
                    lStatus = GetLastError();
                    goto __Exit;
                }
            }

            const auto _uNewLength = (wcslen(szVolumeRoot) - 1) * sizeof(szVolumeRoot[0]);
            if (_uNewLength > MAXUINT16)
            {
                lStatus = ERROR_BAD_PATHNAME;
                goto __Exit;
            }
            TargetVolumeName.Buffer = szVolumeRoot;
            TargetVolumeName.Length = TargetVolumeName.MaximumLength = static_cast<USHORT>(_uNewLength);
        }

        //将路径进行规范化
        if ((FILE_NAME_OPENED & dwFlags) == 0)
        {
            //由于 Windows XP不支持 FileNormalizedNameInformation，所以我们直接调用 GetLongPathNameW 获取完整路径。

            DWORD cbszVolumeRoot = TargetVolumeName.Length;

            if (szVolumeRoot[0] == L'\0')
            {
                //转换分区信息

                if (!internal::BasepGetVolumeDosLetterNameFromNTName(&VolumeNtName, szVolumeRoot))
                {
                    lStatus = GetLastError();

                    if(lStatus == ERROR_NOT_ENOUGH_MEMORY)
                        goto __Exit;

                    if (!internal::BasepGetVolumeGUIDFromNTName(&VolumeNtName, szVolumeRoot))
                    {
                        lStatus = GetLastError();
                        goto __Exit;
                    }
                }

                const auto _cbData = (wcslen(szVolumeRoot) - 1) * sizeof(szVolumeRoot[0]);
                if (_cbData > MAXUINT16)
                {
                    lStatus = ERROR_BAD_PATHNAME;
                    goto __Exit;
                }

                cbszVolumeRoot = static_cast<DWORD>(_cbData);
            }



            auto cbLongPathNameBufferSize = cbszVolumeRoot + pFileNameInfo->FileNameLength + 1024;

            szLongPathNameBuffer = (wchar_t*)HeapAlloc(ProcessHeap, 0, cbLongPathNameBufferSize);
            if (!szLongPathNameBuffer)
            {
                lStatus = ERROR_NOT_ENOUGH_MEMORY;
                goto __Exit;
            }

            DWORD cchLongPathNameBufferSize = cbLongPathNameBufferSize / sizeof(szLongPathNameBuffer[0]);

            memcpy(szLongPathNameBuffer, szVolumeRoot, cbszVolumeRoot);
            memcpy((char*)szLongPathNameBuffer + cbszVolumeRoot, pFileNameInfo->FileName, pFileNameInfo->FileNameLength);
            szLongPathNameBuffer[(cbszVolumeRoot + pFileNameInfo->FileNameLength) / sizeof(wchar_t)] = L'\0';

            for (;;)
            {
                auto result = GetLongPathNameW(szLongPathNameBuffer, szLongPathNameBuffer, cchLongPathNameBufferSize);

                if (result == 0)
                {
                    //失败
                    lStatus = GetLastError();
                    goto __Exit;
                }
                else if (result >= cchLongPathNameBufferSize)
                {
                    cchLongPathNameBufferSize = result + 1;

                    auto pNewLongPathName = (wchar_t*)HeapReAlloc(ProcessHeap, 0, szLongPathNameBuffer, cchLongPathNameBufferSize * sizeof(wchar_t));
                    if (!pNewLongPathName)
                    {
                        lStatus = ERROR_NOT_ENOUGH_MEMORY;
                        goto __Exit;
                    }

                    szLongPathNameBuffer = pNewLongPathName;
                
                }
                else
                {
                    const auto _uNewLength = result * sizeof(wchar_t) - cbszVolumeRoot;
                    if (_uNewLength > MAXUINT16)
                    {
                        lStatus = ERROR_BAD_PATHNAME;
                        goto __Exit;
                    }

                    //转换成功
                    TargetFileName.Buffer = (wchar_t*)((char*)szLongPathNameBuffer + cbszVolumeRoot);
                    TargetFileName.Length = TargetFileName.MaximumLength = static_cast<USHORT>(_uNewLength);
                    break;
                }
            }
        }
        else
        {
            if (pFileNameInfo->FileNameLength > MAXUINT16)
            {
                lStatus = ERROR_BAD_PATHNAME;
                goto __Exit;
            }

            //直接返回原始路径
            TargetFileName.Buffer = pFileNameInfo->FileName;
            TargetFileName.Length = TargetFileName.MaximumLength = static_cast<USHORT>(pFileNameInfo->FileNameLength);
        }


        //返回结果，根目录 + 文件名 的长度
        cchReturn = (TargetVolumeName.Length + TargetFileName.Length) / sizeof(wchar_t);

        if (cchFilePath <= cchReturn)
        {
            //长度不足……

            cchReturn += 1;
        }
        else
        {
            //复制根目录
            memcpy(lpszFilePath, TargetVolumeName.Buffer, TargetVolumeName.Length);
            //复制文件名
            memcpy((char*)lpszFilePath + TargetVolumeName.Length, TargetFileName.Buffer, TargetFileName.Length);
            //保证字符串 '\0' 截断
            lpszFilePath[cchReturn] = L'\0';
        }

    __Exit:
        if (pFileNameInfo)
            HeapFree(ProcessHeap, 0, pFileNameInfo);
        if (pObjectName)
            HeapFree(ProcessHeap, 0, pObjectName);
        if (szLongPathNameBuffer)
            HeapFree(ProcessHeap, 0, szLongPathNameBuffer);

        if (lStatus != ERROR_SUCCESS)
        {
            SetLastError(lStatus);
            return 0;
        }
        else
        {
            return cchReturn;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    16,
    DWORD,
    WINAPI,
    GetFinalPathNameByHandleA,
        _In_ HANDLE hFile,
        _Out_writes_(cchFilePath) LPSTR lpszFilePath,
        _In_ DWORD cchFilePath,
        _In_ DWORD dwFlags
        )
    {
        if (auto pGetFinalPathNameByHandleA = try_get_GetFinalPathNameByHandleA())
        {
            return pGetFinalPathNameByHandleA(hFile, lpszFilePath, cchFilePath, dwFlags);
        }

        const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
        wchar_t* szFilePathUnicode = nullptr;
        for (DWORD cchszFilePathUnicode = 1040;;)
        {
            if (szFilePathUnicode)
            {
                auto pNewBuffer = (wchar_t*)HeapReAlloc(ProcessHeap, 0, szFilePathUnicode, cchszFilePathUnicode * sizeof(wchar_t));
                if (!pNewBuffer)
                {
                    HeapFree(ProcessHeap, 0, szFilePathUnicode);
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    return 0;
                }

                szFilePathUnicode = pNewBuffer;
            }
            else
            {
                szFilePathUnicode = (wchar_t*)HeapAlloc(ProcessHeap, 0, cchszFilePathUnicode * sizeof(wchar_t));
                if (!szFilePathUnicode)
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    return 0;
                }
            }

            auto cchReturn = GetFinalPathNameByHandleW(hFile, szFilePathUnicode, cchszFilePathUnicode, dwFlags);

            if (cchReturn == 0)
            {
            __Error:

                auto lStatus = GetLastError();
                HeapFree(ProcessHeap, 0, szFilePathUnicode);
                SetLastError(lStatus);

                return 0;
            }
            else if (cchReturn > cchszFilePathUnicode)
            {
                //缓冲区不足
                cchszFilePathUnicode = cchReturn;
                continue;
            }
            else
            {
                //操作成功！
                const UINT CodePage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

                auto cchReturnANSI = WideCharToMultiByte(CodePage, WC_NO_BEST_FIT_CHARS, szFilePathUnicode, cchReturn, nullptr, 0, nullptr, nullptr);

                if (cchReturnANSI <= 0)
                {
                    goto __Error;
                }
                else if (static_cast<DWORD>(cchReturnANSI) >= cchFilePath)
                {
                    //长度不足
                    ++cchReturnANSI;
                }
                else
                {
                    WideCharToMultiByte(CodePage, WC_NO_BEST_FIT_CHARS, szFilePathUnicode, cchReturn, lpszFilePath, cchFilePath, nullptr, nullptr);
                    lpszFilePath[cchReturnANSI] = '\0';
                }

                HeapFree(ProcessHeap, 0, szFilePathUnicode);
                return cchReturnANSI;
            }
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    20,
    HANDLE,
    WINAPI,
    CreateFile2,
        _In_ LPCWSTR lpFileName,
        _In_ DWORD dwDesiredAccess,
        _In_ DWORD dwShareMode,
        _In_ DWORD dwCreationDisposition,
        _In_opt_ LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams
        )
    {
        if (auto pCreateFile2 = try_get_CreateFile2())
        {
            return pCreateFile2(lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams);
        }

        DWORD dwFlagsAndAttributes = 0;
        LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr;
        HANDLE hTemplateFile = nullptr;

        if (pCreateExParams)
        {
            if (pCreateExParams->dwSize < sizeof(*pCreateExParams))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return INVALID_HANDLE_VALUE;
            }
        
            dwFlagsAndAttributes = pCreateExParams->dwFileAttributes | pCreateExParams->dwFileFlags;

            if (pCreateExParams->dwSecurityQosFlags != 0)
                dwFlagsAndAttributes |= SECURITY_SQOS_PRESENT | pCreateExParams->dwSecurityQosFlags;

            lpSecurityAttributes = pCreateExParams->lpSecurityAttributes;
            hTemplateFile        = pCreateExParams->hTemplateFile;
        }
     
        return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_26100)

    // 最低支持的客户端	Windows 11 24H2 [桌面应用 |UWP 应用]
    // 支持的最低服务器	Windows Server 2025[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    20,
    HANDLE,
    WINAPI,
    CreateFile3,
        _In_z_ LPCWSTR _szFileName,
        _In_ DWORD _uDesiredAccess,
        _In_ DWORD _uShareMode,
        _In_ DWORD _uCreationDisposition,
        _In_opt_ LPCREATEFILE3_EXTENDED_PARAMETERS _pCreateExParams
        )
    {
        if (const auto _pfnCreateFile3 = try_get_CreateFile3())
        {
            return _pfnCreateFile3(_szFileName, _uDesiredAccess, _uShareMode, _uCreationDisposition, _pCreateExParams);
        }

        static_assert(sizeof(CREATEFILE3_EXTENDED_PARAMETERS) == sizeof(CREATEFILE2_EXTENDED_PARAMETERS), "");
        static_assert(UFIELD_OFFSET (CREATEFILE3_EXTENDED_PARAMETERS, dwSize) == UFIELD_OFFSET(CREATEFILE2_EXTENDED_PARAMETERS, dwSize), "");
        static_assert(UFIELD_OFFSET (CREATEFILE3_EXTENDED_PARAMETERS, dwFileAttributes) == UFIELD_OFFSET(CREATEFILE2_EXTENDED_PARAMETERS, dwFileAttributes), "");
        static_assert(UFIELD_OFFSET(CREATEFILE3_EXTENDED_PARAMETERS, dwFileFlags) == UFIELD_OFFSET(CREATEFILE2_EXTENDED_PARAMETERS, dwFileFlags), "");
        static_assert(UFIELD_OFFSET(CREATEFILE3_EXTENDED_PARAMETERS, dwSecurityQosFlags) == UFIELD_OFFSET(CREATEFILE2_EXTENDED_PARAMETERS, dwSecurityQosFlags), "");
        static_assert(UFIELD_OFFSET(CREATEFILE3_EXTENDED_PARAMETERS, lpSecurityAttributes) == UFIELD_OFFSET(CREATEFILE2_EXTENDED_PARAMETERS, lpSecurityAttributes), "");
        static_assert(UFIELD_OFFSET(CREATEFILE3_EXTENDED_PARAMETERS, hTemplateFile) == UFIELD_OFFSET(CREATEFILE2_EXTENDED_PARAMETERS, hTemplateFile), "");

        return CreateFile2(_szFileName, _uDesiredAccess, _uShareMode, _uCreationDisposition, reinterpret_cast<LPCREATEFILE2_EXTENDED_PARAMETERS>(_pCreateExParams));
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    24,
    HANDLE,
    WINAPI,
    OpenFileById,
        _In_     HANDLE hVolumeHint,
        _In_     LPFILE_ID_DESCRIPTOR lpFileId,
        _In_     DWORD dwDesiredAccess,
        _In_     DWORD dwShareMode,
        _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        _In_     DWORD dwFlagsAndAttributes
        )
    {
        if (const auto pOpenFileById = try_get_OpenFileById())
        {
            return pOpenFileById(hVolumeHint, lpFileId, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwFlagsAndAttributes);
        }

        if (lpFileId == nullptr || lpFileId->dwSize < sizeof(FILE_ID_DESCRIPTOR_win7) || lpFileId->Type >= _FILE_ID_TYPE_win7::MaximumFileIdType)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return INVALID_HANDLE_VALUE;
        }

#if !defined(__USING_NTDLL_LIB)
        const auto NtCreateFile = try_get_NtCreateFile();
        if (!NtCreateFile)
        {
            SetLastError(ERROR_FUNCTION_FAILED);
            return INVALID_HANDLE_VALUE;
        }
#endif

        dwDesiredAccess |= SYNCHRONIZE | FILE_READ_ATTRIBUTES;

        UNICODE_STRING ObjectName;

        if (FileIdType == lpFileId->Type)
        {
            ObjectName.Buffer = (PWSTR)(&lpFileId->FileId);
            ObjectName.Length = ObjectName.MaximumLength = sizeof(lpFileId->FileId);
        }
        else
        {
            ObjectName.Buffer = (PWSTR)(&lpFileId->ObjectId);
            ObjectName.Length = ObjectName.MaximumLength = sizeof(lpFileId->ObjectId);
        }

        OBJECT_ATTRIBUTES ObjectAttributes = {sizeof(ObjectAttributes), hVolumeHint, &ObjectName,  OBJ_CASE_INSENSITIVE };
    
        ULONG CreateOptions = FILE_OPEN_BY_FILE_ID;

        if (FILE_FLAG_WRITE_THROUGH & dwFlagsAndAttributes)
        {
            CreateOptions |= FILE_WRITE_THROUGH;
        }

        if (FILE_FLAG_NO_BUFFERING & dwFlagsAndAttributes)
        {
            CreateOptions |= FILE_NO_INTERMEDIATE_BUFFERING;
        }

        if (FILE_FLAG_SEQUENTIAL_SCAN & dwFlagsAndAttributes)
        {
            CreateOptions |= FILE_SEQUENTIAL_ONLY;
        }
    
        if (FILE_FLAG_RANDOM_ACCESS & dwFlagsAndAttributes)
        {
            CreateOptions |= FILE_RANDOM_ACCESS;
        }

        if (FILE_FLAG_BACKUP_SEMANTICS & dwFlagsAndAttributes)
        {
            CreateOptions |= FILE_OPEN_FOR_BACKUP_INTENT;
        }

        if ((FILE_FLAG_OVERLAPPED & dwFlagsAndAttributes) == 0)
        {
            CreateOptions |= FILE_SYNCHRONOUS_IO_NONALERT;
        }

        if (FILE_FLAG_DELETE_ON_CLOSE & dwFlagsAndAttributes)
        {
            CreateOptions |= FILE_DELETE_ON_CLOSE;
            dwDesiredAccess |= DELETE;
        }

        if (FILE_FLAG_OPEN_REPARSE_POINT & dwFlagsAndAttributes)
        {
            CreateOptions |= FILE_OPEN_REPARSE_POINT;
        }

        if (FILE_FLAG_OPEN_NO_RECALL & dwFlagsAndAttributes)
        {
            CreateOptions |= FILE_OPEN_NO_RECALL;
        }

        HANDLE hFile;

        IO_STATUS_BLOCK IoStatusBlock;

        auto Status = NtCreateFile(&hFile, dwDesiredAccess, &ObjectAttributes, &IoStatusBlock, nullptr, 0, dwShareMode, FILE_OPEN, CreateOptions, nullptr, 0);

        if (Status < 0)
        {
            hFile = INVALID_HANDLE_VALUE;
            internal::BaseSetLastNTError(Status);
        }

        return hFile;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008[desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOLEAN,
    APIENTRY,
    CreateSymbolicLinkW,
        _In_ LPCWSTR lpSymlinkFileName,
        _In_ LPCWSTR lpTargetFileName,
        _In_ DWORD dwFlags
        )
    {
        if (const auto pCreateSymbolicLinkW = try_get_CreateSymbolicLinkW())
        {
            return pCreateSymbolicLinkW(lpSymlinkFileName, lpTargetFileName, dwFlags);
        }

        SetLastError(ERROR_INVALID_FUNCTION);

        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista [desktop apps only]
    //Windows Server 2008[desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOLEAN,
    APIENTRY,
    CreateSymbolicLinkA,
        _In_ LPCSTR lpSymlinkFileName,
        _In_ LPCSTR lpTargetFileName,
        _In_ DWORD dwFlags
        )
    {
        if (const auto pCreateSymbolicLinkA = try_get_CreateSymbolicLinkA())
        {
            return pCreateSymbolicLinkA(lpSymlinkFileName, lpTargetFileName, dwFlags);
        }

        SetLastError(ERROR_INVALID_FUNCTION);

        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_2)

    //Windows Vista [desktop apps only]
    //Windows Server 2003 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    16,
    HANDLE,
    WINAPI,
    ReOpenFile,
        _In_ HANDLE  hOriginalFile,
        _In_ DWORD   dwDesiredAccess,
        _In_ DWORD   dwShareMode,
        _In_ DWORD   dwFlagsAndAttributes
        )
    {
        if (const auto pReOpenFile = try_get_ReOpenFile())
        {
            return pReOpenFile(hOriginalFile, dwDesiredAccess, dwShareMode, dwFlagsAndAttributes);
        }

        NTSTATUS Status;

        do
        {
#if !defined(__USING_NTDLL_LIB)
            const auto NtCreateFile = try_get_NtCreateFile();
            if (!NtCreateFile)
            {
                Status = STATUS_INVALID_INFO_CLASS;
                break;
            }
#endif

            if ((size_t(hOriginalFile) & 0x10000003) == 3)
            {
                Status = STATUS_INVALID_HANDLE;
                break;
            }

            if (dwFlagsAndAttributes & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_SPARSE_FILE | FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED | FILE_ATTRIBUTE_ENCRYPTED))
            {
                Status = STATUS_INVALID_PARAMETER;
                break;
            }

            ULONG CreateOptions = FILE_NON_DIRECTORY_FILE;

            if (FILE_FLAG_WRITE_THROUGH & dwFlagsAndAttributes)
            {
                CreateOptions |= FILE_WRITE_THROUGH;
            }

            if (FILE_FLAG_NO_BUFFERING & dwFlagsAndAttributes)
            {
                CreateOptions |= FILE_NO_INTERMEDIATE_BUFFERING;
            }

            if (FILE_FLAG_SEQUENTIAL_SCAN & dwFlagsAndAttributes)
            {
                CreateOptions |= FILE_SEQUENTIAL_ONLY;
            }

            if (FILE_FLAG_RANDOM_ACCESS & dwFlagsAndAttributes)
            {
                CreateOptions |= FILE_RANDOM_ACCESS;
            }
        
            if (FILE_FLAG_BACKUP_SEMANTICS & dwFlagsAndAttributes)
            {
                CreateOptions |= FILE_OPEN_FOR_BACKUP_INTENT;
            }
        
            if ((FILE_FLAG_OVERLAPPED & dwFlagsAndAttributes) == 0)
            {
                CreateOptions |= FILE_SYNCHRONOUS_IO_NONALERT;
            }

            if (FILE_FLAG_OPEN_NO_RECALL & dwFlagsAndAttributes)
            {
                CreateOptions |= FILE_OPEN_NO_RECALL;
            }

            if (FILE_FLAG_OPEN_REPARSE_POINT & dwFlagsAndAttributes)
            {
                CreateOptions |= FILE_OPEN_REPARSE_POINT;
            }

            if (FILE_FLAG_DELETE_ON_CLOSE & dwFlagsAndAttributes)
            {
                CreateOptions |= FILE_DELETE_ON_CLOSE;
                dwDesiredAccess |= DELETE;
            }

            dwDesiredAccess |= SYNCHRONIZE | FILE_READ_ATTRIBUTES;

            UNICODE_STRING ObjectName = { 0, 2, (PWSTR)L"" };
            SECURITY_QUALITY_OF_SERVICE SecurityQualityOfService;

            SecurityQualityOfService.Length = sizeof(SecurityQualityOfService);

            if (SECURITY_SQOS_PRESENT & dwFlagsAndAttributes)
            {
                SecurityQualityOfService.ImpersonationLevel = SECURITY_IMPERSONATION_LEVEL((dwFlagsAndAttributes >> 16) & 0x3);

                if (SECURITY_CONTEXT_TRACKING & dwFlagsAndAttributes)
                {
                    SecurityQualityOfService.ContextTrackingMode = TRUE;
                }
                else
                {
                    SecurityQualityOfService.ContextTrackingMode = FALSE;
                }


                if (SECURITY_EFFECTIVE_ONLY & dwFlagsAndAttributes)
                {
                    SecurityQualityOfService.EffectiveOnly = TRUE;
                }
                else
                {
                    SecurityQualityOfService.EffectiveOnly = FALSE;
                }
            }
            else
            {
                SecurityQualityOfService.ImpersonationLevel = SecurityImpersonation;
                SecurityQualityOfService.ContextTrackingMode = TRUE;
                SecurityQualityOfService.EffectiveOnly = TRUE;
            }

            OBJECT_ATTRIBUTES ObjectAttributes =
            {
                sizeof(ObjectAttributes),
                hOriginalFile,
                &ObjectName,
                FILE_FLAG_POSIX_SEMANTICS & dwFlagsAndAttributes ? 0ul : OBJ_CASE_INSENSITIVE,
                nullptr,
                &SecurityQualityOfService
            };


            IO_STATUS_BLOCK IoStatusBlock;

            HANDLE hFile;

            Status = NtCreateFile(
                &hFile,
                dwDesiredAccess,
                &ObjectAttributes,
                &IoStatusBlock,
                nullptr,
                0,
                dwShareMode,
                FILE_OPEN,
                CreateOptions,
                nullptr,
                0);

            if (Status >= 0)
            {
                return hFile;
            }


        } while (false);


        internal::BaseSetLastNTError(Status);

        return INVALID_HANDLE_VALUE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5)

    // 虽然MSDN文档说此API XP才又，但是实际上Windows 2000 RTM就有了。
    __DEFINE_THUNK(
    kernel32,
    20,
    _Success_(return)
    BOOL,
    WINAPI,
    SetFilePointerEx,
        _In_ HANDLE hFile,
        _In_ LARGE_INTEGER liDistanceToMove,
        _Out_opt_ PLARGE_INTEGER lpNewFilePointer,
        _In_ DWORD dwMoveMethod
        )
    {
        if (const auto pSetFilePointerEx = try_get_SetFilePointerEx())
        {
            return pSetFilePointerEx(hFile, liDistanceToMove, lpNewFilePointer, dwMoveMethod);
        }

        liDistanceToMove.LowPart = SetFilePointer(hFile, liDistanceToMove.LowPart, &liDistanceToMove.HighPart, dwMoveMethod);

        if (liDistanceToMove.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
        {
            return FALSE;
        }

        if (lpNewFilePointer)
            *lpNewFilePointer = liDistanceToMove;

        return TRUE;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows XP [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    24,
    HANDLE,
    WINAPI,
    FindFirstFileExW,
        _In_ LPCWSTR lpFileName,
        _In_ FINDEX_INFO_LEVELS fInfoLevelId,
        _Out_writes_bytes_(sizeof(WIN32_FIND_DATAW)) LPVOID lpFindFileData,
        _In_ FINDEX_SEARCH_OPS fSearchOp,
        _Reserved_ LPVOID lpSearchFilter,
        _In_ DWORD dwAdditionalFlags
        )
    {
        if (const auto _pfnFindFirstFileExW = try_get_FindFirstFileExW())
        {
            if (internal::GetSystemVersion() < internal::MakeVersion(6, 1))
            {
                // dwAdditionalFlags : Windows 7开始才支持 FIND_FIRST_EX_LARGE_FETCH
                dwAdditionalFlags &= (FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_ON_DISK_ENTRIES_ONLY);
                    
                // Windows 7开始才支持 FindExInfoBasic
                fInfoLevelId = FindExInfoStandard;
            }


            return _pfnFindFirstFileExW(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
        }

        return FindFirstFileW(lpFileName, (WIN32_FIND_DATAW*)lpFindFileData);
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_1)

    // 最低受支持的客户端	Windows XP [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    24,
    HANDLE,
    WINAPI,
    FindFirstFileExA,
        _In_ LPCSTR lpFileName,
        _In_ FINDEX_INFO_LEVELS fInfoLevelId,
        _Out_writes_bytes_(sizeof(WIN32_FIND_DATAA)) LPVOID lpFindFileData,
        _In_ FINDEX_SEARCH_OPS fSearchOp,
        _Reserved_ LPVOID lpSearchFilter,
        _In_ DWORD dwAdditionalFlags
        )
    {
        if (const auto _pfnFindFirstFileExA = try_get_FindFirstFileExA())
        {
            if (internal::GetSystemVersion() < internal::MakeVersion(6, 1))
            {
                // dwAdditionalFlags : Windows 7开始才支持 FIND_FIRST_EX_LARGE_FETCH
                dwAdditionalFlags &= (FIND_FIRST_EX_CASE_SENSITIVE | FIND_FIRST_EX_ON_DISK_ENTRIES_ONLY);
                    
                // Windows 7开始才支持 FindExInfoBasic
                fInfoLevelId = FindExInfoStandard;
            }

            return _pfnFindFirstFileExA(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
        }

        return FindFirstFileA(lpFileName, (WIN32_FIND_DATAA*)lpFindFileData);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // * 6.2开始，_puNumberOfBytesWritten与_pOverlapped可以同时为NULL
    __DEFINE_THUNK(
    kernel32,
    20,
    BOOL,
    WINAPI,
    WriteFile,
        _In_ HANDLE _hFile,
        _In_reads_bytes_opt_(_uNumberOfBytesToWrite) LPCVOID _pBuffer,
        _In_ DWORD _uNumberOfBytesToWrite,
        _Out_opt_ LPDWORD _puNumberOfBytesWritten,
        _Inout_opt_ LPOVERLAPPED _pOverlapped
        )
    {
        if (const auto _pfnWriteFile = try_get_WriteFile())
        {
            DWORD _uNumberOfBytesWritten = 0;
            if (_puNumberOfBytesWritten == nullptr && _pOverlapped == nullptr)
            {
                _puNumberOfBytesWritten = &_uNumberOfBytesWritten;
            }

            return _pfnWriteFile(_hFile, _pBuffer, _uNumberOfBytesToWrite, _puNumberOfBytesWritten, _pOverlapped);
        }

        SetLastError(ERROR_INVALID_FUNCTION);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // * 6.2开始，_puNumberOfBytesRead与_pOverlapped可以同时为NULL
    __DEFINE_THUNK(
    kernel32,
    20,
    BOOL,
    WINAPI,
    ReadFile,
        _In_ HANDLE _hFile,
        _Out_writes_bytes_to_opt_(_uNumberOfBytesToRead, *_puNumberOfBytesRead) __out_data_source(FILE) LPVOID _pBuffer,
        _In_ DWORD _uNumberOfBytesToRead,
        _Out_opt_ LPDWORD _puNumberOfBytesRead,
        _Inout_opt_ LPOVERLAPPED _pOverlapped
        )
    {
        if (const auto _pfnReadFile = try_get_ReadFile())
        {
            DWORD _uNumberOfBytesRead = 0;
            if (_puNumberOfBytesRead == nullptr && _pOverlapped == nullptr)
            {
                _puNumberOfBytesRead = &_uNumberOfBytesRead;
            }

            return _pfnReadFile(_hFile, _pBuffer, _uNumberOfBytesToRead, _puNumberOfBytesRead, _pOverlapped);
        }

        SetLastError(ERROR_INVALID_FUNCTION);
        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    12,
    HRESULT,
    WINAPI,
    CopyFile2,
        _In_      PCWSTR                         _szExistingFileName,
        _In_      PCWSTR                         _szNewFileName,
        _In_opt_  COPYFILE2_EXTENDED_PARAMETERS* _pExtendedParameters
        )
    {      
        if (const auto _pfnCopyFile2 = try_get_CopyFile2())
        {
            return _pfnCopyFile2(_szExistingFileName, _szNewFileName, _pExtendedParameters);
        }

        if (_pExtendedParameters)
        {
            if (_pExtendedParameters->dwSize < sizeof(COPYFILE2_EXTENDED_PARAMETERS))
            {
                return E_INVALIDARG;
            }

            CopyFileExToCopyFile2CallbackContext _oContext = { _pExtendedParameters };
            if (CopyFileExW(
                _szExistingFileName,
                _szNewFileName,
                _pExtendedParameters->pProgressRoutine ? CopyFileExToCopyFile2Callback : nullptr,
                &_oContext,
                _pExtendedParameters->pfCancel,
                _pExtendedParameters->dwCopyFlags))
            {
                return S_OK;
            }
        }
        else
        {
            if (CopyFileExW(
                _szExistingFileName,
                _szNewFileName,
                nullptr,
                nullptr,
                nullptr,
                0))
            {
                return S_OK;
            }
        }

        return __HRESULT_FROM_WIN32(GetLastError());
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_20348)

    // 最低支持的客户端	Windows 11 内部版本 22000
    // 支持的最低服务器	Windows Server 2022 内部版本 20348
    __DEFINE_THUNK(
    kernel32,
    8,
    DWORD,
    WINAPI,
    GetTempPath2W,
        _In_ DWORD BufferLength,
        _Out_writes_to_opt_(BufferLength,return + 1) LPWSTR Buffer
        )
    {
        if (const auto _pfnGetTempPath2W = try_get_GetTempPath2W())
        {
            return _pfnGetTempPath2W(BufferLength, Buffer);
        }

        return GetTempPathW(BufferLength, Buffer);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT10_20348)

    // 最低支持的客户端	Windows 11 内部版本 22000
    // 支持的最低服务器	Windows Server 2022 内部版本 20348
    __DEFINE_THUNK(
    kernel32,
    8,
    DWORD,
    WINAPI,
    GetTempPath2A,
        _In_ DWORD BufferLength,
        _Out_writes_to_opt_(BufferLength,return + 1) LPSTR Buffer
        )
    {
        if (const auto _pfnGetTempPath2A = try_get_GetTempPath2A())
        {
            return _pfnGetTempPath2A(BufferLength, Buffer);
        }

        return GetTempPathA(BufferLength, Buffer);
    }
#endif
} //namespace YY::Thunks
