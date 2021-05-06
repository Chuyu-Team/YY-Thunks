

namespace YY
{
	namespace Thunks
	{


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
BOOL
WINAPI
GetFileInformationByHandleEx(
	_In_  HANDLE hFile,
	_In_  FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
	_Out_writes_bytes_(dwBufferSize) LPVOID lpFileInformation,
	_In_  DWORD dwBufferSize
	)
#ifdef YY_Thunks_Defined
	;
#else
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
		auto pNtQueryDirectoryFile = try_get_NtQueryDirectoryFile();
		if (!pNtQueryDirectoryFile)
		{
			SetLastError(ERROR_INVALID_FUNCTION);
			return FALSE;
		}

		Status = pNtQueryDirectoryFile(
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
		auto pNtQueryInformationFile = try_get_NtQueryInformationFile();

		if (!pNtQueryInformationFile)
		{
			SetLastError(ERROR_INVALID_FUNCTION);
			return FALSE;
		}

		Status = pNtQueryInformationFile(hFile, &IoStatusBlock, lpFileInformation, dwBufferSize, NtFileInformationClass);
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

__YY_Thunks_Expand_Function(kernel32, GetFileInformationByHandleEx, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
BOOL
WINAPI
SetFileInformationByHandle(
	_In_ HANDLE hFile,
	_In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
	_In_reads_bytes_(dwBufferSize) LPVOID lpFileInformation,
	_In_ DWORD dwBufferSize
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pSetFileInformationByHandle = try_get_SetFileInformationByHandle())
	{
		return pSetFileInformationByHandle(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
	}


	auto pNtSetInformationFile = try_get_NtSetInformationFile();
	if (!pNtSetInformationFile)
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		return FALSE;
	}

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
				auto pRtlDosPathNameToNtPathName_U_WithStatus = try_get_RtlDosPathNameToNtPathName_U_WithStatus();
				auto pRtlFreeUnicodeString = try_get_RtlFreeUnicodeString();

				if (pRtlDosPathNameToNtPathName_U_WithStatus == nullptr || pRtlFreeUnicodeString ==nullptr)
				{
					SetLastError(ERROR_INVALID_FUNCTION);
					return FALSE;
				}

				UNICODE_STRING NtName;

				auto Status = pRtlDosPathNameToNtPathName_U_WithStatus(pRenameInfo->FileName, &NtName, nullptr, nullptr);

				if (Status < STATUS_SUCCESS)
				{
					internal::BaseSetLastNTError(Status);

					return FALSE;
				}

				auto dwNewBufferSize = sizeof(FILE_RENAME_INFO) + NtName.Length;

				auto NewRenameInfo = (FILE_RENAME_INFO*)HeapAlloc(ProcessHeap, 0, dwNewBufferSize);
				if (!NewRenameInfo)
				{
					auto lStatus = GetLastError();

					pRtlFreeUnicodeString(&NtName);

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

				pRtlFreeUnicodeString(&NtName);
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

	auto Status = pNtSetInformationFile(hFile, &IoStatusBlock, lpFileInformation, dwBufferSize, NtFileInformationClass);

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

__YY_Thunks_Expand_Function(kernel32, SetFileInformationByHandle, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
DWORD
WINAPI
GetFinalPathNameByHandleW(
	_In_ HANDLE hFile,
	_Out_writes_(cchFilePath) LPWSTR lpszFilePath,
	_In_ DWORD cchFilePath,
	_In_ DWORD dwFlags
	)
#ifdef YY_Thunks_Defined
	;
#else
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


	auto pNtQueryObject = try_get_NtQueryObject();
	auto pNtQueryInformationFile = try_get_NtQueryInformationFile();

	if (nullptr == pNtQueryObject
		|| nullptr == pNtQueryInformationFile)
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		return 0;
	}

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

		auto Status = pNtQueryObject(hFile, ObjectNameInformation, pObjectName, cbObjectName, &cbObjectName);

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

		auto Status = pNtQueryInformationFile(hFile, &IoStatusBlock, pFileNameInfo, cbFileNameInfo, FileNameInformation);

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

	//低于Vista平台无法支持 FILE_NAME_OPENED，因此忽略

	if (VOLUME_NAME_NT & dwFlags)
	{
		//返回NT路径
		cchReturn = pObjectName->Name.Length / sizeof(lpszFilePath[0]);

		if (cchFilePath <= cchReturn)
		{
			//长度不足……

			cchReturn += 1;
		}
		else
		{
			memcpy(lpszFilePath, pObjectName->Name.Buffer, cchReturn * sizeof(lpszFilePath[0]));

			lpszFilePath[cchReturn] = L'\0';
		}
	}
	else if (VOLUME_NAME_NONE & dwFlags)
	{
		//仅返回文件名
		cchReturn = pFileNameInfo->FileNameLength / sizeof(lpszFilePath[0]);

		if (cchFilePath <= cchReturn)
		{
			//长度不足……

			cchReturn += 1;
		}
		else
		{
			memcpy(lpszFilePath, pFileNameInfo->FileName, cchReturn * sizeof(lpszFilePath[0]));
			lpszFilePath[cchReturn] = L'\0';
		}
	}
	else
	{
		const wchar_t szGobal[] = L"\\\\?\\GLOBALROOT";

		const DWORD cbVolumeName = pObjectName->Name.Length - pFileNameInfo->FileNameLength + sizeof(lpszFilePath[0]);

		auto szVolumeMountPoint = (wchar_t*)HeapAlloc(ProcessHeap, 0, cbVolumeName + sizeof(szGobal));
		if (!szVolumeMountPoint)
		{
			lStatus = ERROR_NOT_ENOUGH_MEMORY;
			goto __Exit;
		}

		DWORD cbVolumeMountPoint = sizeof(szGobal) - sizeof(szGobal[0]);

		memcpy(szVolumeMountPoint, szGobal, cbVolumeMountPoint);

		memcpy((byte*)szVolumeMountPoint + cbVolumeMountPoint, pObjectName->Name.Buffer, cbVolumeName);
		cbVolumeMountPoint += cbVolumeName;

		szVolumeMountPoint[cbVolumeMountPoint / sizeof(szVolumeMountPoint[0])] = L'\0';

		wchar_t szVolumeName[MAX_PATH];

		if (!GetVolumeNameForVolumeMountPointW(szVolumeMountPoint, szVolumeName, _countof(szVolumeName)))
		{
			lStatus = GetLastError();
			HeapFree(ProcessHeap, 0, szVolumeMountPoint);

			goto __Exit;
		}

		HeapFree(ProcessHeap, 0, szVolumeMountPoint);


		if (VOLUME_NAME_GUID & dwFlags)
		{
			//返回分区GUID名称

			auto cchVolumeName = wcslen(szVolumeName);

			if (cchVolumeName == 0)
			{
				//逗我？
				lStatus = ERROR_INVALID_FUNCTION;
				goto __Exit;
			}

			--cchVolumeName;

			cchReturn = cchVolumeName + pFileNameInfo->FileNameLength / sizeof(pFileNameInfo->FileName[0]);

			if (cchFilePath <= cchReturn)
			{
				cchReturn += 1;
			}
			else
			{
				//复制VolumeName
				memcpy(lpszFilePath, szVolumeName, cchVolumeName * sizeof(szVolumeName[0]));

				//复制文件名
				memcpy(lpszFilePath + cchVolumeName, pFileNameInfo->FileName, pFileNameInfo->FileNameLength);

				//NULL边界
				lpszFilePath[cchReturn] = L'\0';
			}
		}
		else
		{
			//返回Dos路径
			DWORD cchVolumePathName = 0;

			wchar_t VolumePathName[MAX_PATH + 4] = L"\\\\?\\";

			if (!GetVolumePathNamesForVolumeNameW(szVolumeName, VolumePathName + 4, MAX_PATH, &cchVolumePathName))
			{
				lStatus = GetLastError();
				goto __Exit;
			}

			cchVolumePathName = wcslen(VolumePathName);

			if (cchVolumePathName == 0)
			{
				//逗我？
				lStatus = ERROR_INVALID_FUNCTION;
				goto __Exit;
			}

			--cchVolumePathName;

			cchReturn = cchVolumePathName + pFileNameInfo->FileNameLength / sizeof(pFileNameInfo->FileName[0]);

			if (cchFilePath <= cchReturn)
			{
				cchReturn += 1;
			}
			else
			{
				//复制VolumePathName 
				memcpy(lpszFilePath, VolumePathName, cchVolumePathName * sizeof(VolumePathName[0]));

				//复制文件名
				memcpy(lpszFilePath + cchVolumePathName, pFileNameInfo->FileName, pFileNameInfo->FileNameLength);

				//NULL边界
				lpszFilePath[cchReturn] = L'\0';
			}
		}
	}

__Exit:
	if (pFileNameInfo)
		HeapFree(ProcessHeap, 0, pFileNameInfo);
	if (pObjectName)
		HeapFree(ProcessHeap, 0, pObjectName);

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

__YY_Thunks_Expand_Function(kernel32, GetFinalPathNameByHandleW, 16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista,  Windows Server 2008

EXTERN_C
DWORD
WINAPI
GetFinalPathNameByHandleA(
	_In_ HANDLE hFile,
	_Out_writes_(cchFilePath) LPSTR lpszFilePath,
	_In_ DWORD cchFilePath,
	_In_ DWORD dwFlags
	)
#ifdef YY_Thunks_Defined
	;
#else
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

			if (0 == cchReturnANSI)
			{
				goto __Error;
			}
			else if (cchReturnANSI >= cchFilePath)
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

__YY_Thunks_Expand_Function(kernel32, GetFinalPathNameByHandleA, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8 [desktop apps | UWP apps]
//Windows Server 2012 [desktop apps | UWP apps]

EXTERN_C
HANDLE
WINAPI
CreateFile2(
    _In_ LPCWSTR lpFileName,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwShareMode,
    _In_ DWORD dwCreationDisposition,
    _In_opt_ LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams
    )
#ifdef YY_Thunks_Defined
	;
#else
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

__YY_Thunks_Expand_Function(kernel32, CreateFile2, 20);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008 [desktop apps only]

#ifdef YY_Thunks_Defined

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

EXTERN_C
HANDLE
WINAPI
OpenFileById(
    _In_     HANDLE hVolumeHint,
    _In_     LPFILE_ID_DESCRIPTOR lpFileId,
    _In_     DWORD dwDesiredAccess,
    _In_     DWORD dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_     DWORD dwFlagsAndAttributes
    )
#ifdef YY_Thunks_Defined
;
#else
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

	const auto pNtCreateFile = try_get_NtCreateFile();
	if (!pNtCreateFile)
	{
		SetLastError(ERROR_FUNCTION_FAILED);
		return INVALID_HANDLE_VALUE;
	}

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

	auto Status = pNtCreateFile(&hFile, dwDesiredAccess, &ObjectAttributes, &IoStatusBlock, nullptr, 0, dwShareMode, FILE_OPEN, CreateOptions, nullptr, 0);

	if (Status < 0)
	{
		hFile = INVALID_HANDLE_VALUE;
		internal::BaseSetLastNTError(Status);
	}

	return hFile;
}
#endif

__YY_Thunks_Expand_Function(kernel32, OpenFileById, 24);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008[desktop apps only]

#ifndef SYMLINK_FLAG_RELATIVE
#define SYMLINK_FLAG_RELATIVE  1
#endif

#ifndef YY_Thunks_Defined

namespace internal
{
	typedef struct _REPARSE_DATA_BUFFER {
		ULONG  ReparseTag;
		USHORT ReparseDataLength;
		USHORT Reserved;
		union {
			struct {
				USHORT SubstituteNameOffset;
				USHORT SubstituteNameLength;
				USHORT PrintNameOffset;
				USHORT PrintNameLength;
				ULONG Flags;
				WCHAR PathBuffer[1];
			} SymbolicLinkReparseBuffer;
			struct {
				USHORT SubstituteNameOffset;
				USHORT SubstituteNameLength;
				USHORT PrintNameOffset;
				USHORT PrintNameLength;
				WCHAR PathBuffer[1];
			} MountPointReparseBuffer;
			struct {
				UCHAR  DataBuffer[1];
			} GenericReparseBuffer;
		} DUMMYUNIONNAME;
	} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

	static
	BOOLEAN
	APIENTRY
	CreateSymbolicLinkW_Downlevel(
		_In_ LPCWSTR lpSymlinkFileName,
		_In_ LPCWSTR lpTargetFileName,
		_In_ DWORD dwFlags
		)
	{
		if (lpSymlinkFileName == nullptr || lpTargetFileName == nullptr || (dwFlags&SYMBOLIC_LINK_FLAG_DIRECTORY) != dwFlags)
		{
			//参数错误
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		const auto pRtlAdjustPrivilege = try_get_RtlAdjustPrivilege();
		const auto pNtCreateFile = try_get_NtCreateFile();
		const auto pNtClose = try_get_NtClose();
		const auto pRtlDetermineDosPathNameType_U = try_get_RtlDetermineDosPathNameType_U();
		const auto pRtlDosPathNameToNtPathName_U = try_get_RtlDosPathNameToNtPathName_U();
		const auto pRtlFreeUnicodeString = try_get_RtlFreeUnicodeString();
		const auto pNtSetInformationFile = try_get_NtSetInformationFile();

		if (pRtlAdjustPrivilege == nullptr || pNtCreateFile == nullptr || pNtClose == nullptr
			|| pRtlDetermineDosPathNameType_U == nullptr || pRtlDosPathNameToNtPathName_U == nullptr
			|| pRtlFreeUnicodeString == nullptr || pNtSetInformationFile == nullptr)
		{
			SetLastError(ERROR_FUNCTION_FAILED);
			return FALSE;
		}

		if (!ImpersonateSelf(SecurityImpersonation))
			return FALSE;


		LSTATUS lStatus = ERROR_SUCCESS;

		const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
		wchar_t* lpFullTargetFileName = nullptr;
		REPARSE_DATA_BUFFER* pReparseData = nullptr;

		do
		{
			BOOLEAN Enabled = FALSE;

			auto Status = pRtlAdjustPrivilege(SE_CREATE_SYMBOLIC_LINK_PRIVILEGE, TRUE, TRUE, &Enabled);
			if (Status)
			{
				lStatus = internal::NtStatusToDosError(Status);
				break;
			}

			UNICODE_STRING NtName = {};
			bool bRelative = false;

			auto PathType = pRtlDetermineDosPathNameType_U(lpTargetFileName);

			if (PathType == RtlPathTypeUnknown || PathType == RtlPathTypeRooted || PathType == RtlPathTypeRelative)
			{
				bRelative = true;
				NtName.Buffer = (wchar_t*)lpTargetFileName;
				NtName.Length = NtName.MaximumLength = wcslen(lpTargetFileName) * sizeof(wchar_t);
			}
			else if (PathType == RtlPathTypeDriveRelative)
			{
				LPWSTR FilePart = nullptr;
				auto cchFull = GetFullPathNameW(lpTargetFileName, 0, 0, &FilePart);
				if (cchFull == 0)
				{
					lStatus = GetLastError();
					break;
				}

				lpFullTargetFileName = (wchar_t*)HeapAlloc(ProcessHeap, 0, cchFull * sizeof(wchar_t));
				if (!lpFullTargetFileName)
				{
					lStatus = ERROR_NOT_ENOUGH_MEMORY;
					break;
				}

				if (GetFullPathNameW(lpTargetFileName, cchFull, lpFullTargetFileName, &FilePart) == 0)
				{
					lStatus = GetLastError();
					break;
				}

				lpTargetFileName = lpFullTargetFileName;
			}
			else
			{
				if (!pRtlDosPathNameToNtPathName_U(lpTargetFileName, &NtName, nullptr, nullptr))
				{
					lStatus = ERROR_INVALID_PARAMETER;
					break;
				}
			}

			do
			{
				auto cbTargetFileName = wcslen(lpTargetFileName) * sizeof(wchar_t);

				auto cBuffer = NtName.Length + cbTargetFileName + UFIELD_OFFSET(REPARSE_DATA_BUFFER, SymbolicLinkReparseBuffer.PathBuffer);


				pReparseData = (REPARSE_DATA_BUFFER*)HeapAlloc(ProcessHeap, HEAP_ZERO_MEMORY, cBuffer);

				if (!pReparseData)
				{
					lStatus = ERROR_NOT_ENOUGH_MEMORY;
					break;
				}


				if (bRelative)
					pReparseData->SymbolicLinkReparseBuffer.Flags |= SYMLINK_FLAG_RELATIVE;


				pReparseData->ReparseTag = IO_REPARSE_TAG_SYMLINK;

				pReparseData->ReparseDataLength = cBuffer - UFIELD_OFFSET(REPARSE_DATA_BUFFER, GenericReparseBuffer);

				auto& SymbolicLinkReparseBuffer = pReparseData->SymbolicLinkReparseBuffer;


				SymbolicLinkReparseBuffer.SubstituteNameOffset = SymbolicLinkReparseBuffer.PrintNameLength = wcslen(lpTargetFileName) * sizeof(wchar_t);
				SymbolicLinkReparseBuffer.SubstituteNameLength = NtName.Length;

				memcpy(SymbolicLinkReparseBuffer.PathBuffer, lpTargetFileName, SymbolicLinkReparseBuffer.PrintNameLength);
				memcpy((byte*)(SymbolicLinkReparseBuffer.PathBuffer) + SymbolicLinkReparseBuffer.PrintNameLength, NtName.Buffer, NtName.Length);


				UNICODE_STRING NtSymlinkFileName = {};

				if (!pRtlDosPathNameToNtPathName_U(lpSymlinkFileName, &NtSymlinkFileName, nullptr, nullptr))
				{
					lStatus = ERROR_INVALID_PARAMETER;
					break;
				}


				HANDLE hFile;

				OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), nullptr, &NtSymlinkFileName, OBJ_CASE_INSENSITIVE };
				IO_STATUS_BLOCK IoStatusBlock;

				Status = pNtCreateFile(
					&hFile,
					FILE_WRITE_ATTRIBUTES | SYNCHRONIZE | DELETE,
					&ObjectAttributes,
					&IoStatusBlock,
					nullptr,
					FILE_ATTRIBUTE_NORMAL,
					0,
					FILE_CREATE,
					(dwFlags&SYMBOLIC_LINK_FLAG_DIRECTORY) ? FILE_OPEN_REPARSE_POINT | FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE : FILE_OPEN_REPARSE_POINT | FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
					nullptr,
					0);
			
			
				if (Status < 0)
				{
					lStatus = internal::NtStatusToDosError(Status);
				}
				else
				{
					DWORD cRet;

					if (!DeviceIoControl(hFile, FSCTL_SET_REPARSE_POINT, pReparseData, cBuffer, NULL, NULL, &cRet, NULL))
					{
						lStatus = GetLastError();

						if (lStatus == ERROR_SUCCESS)
							lStatus = ERROR_INVALID_FUNCTION;

						//设置失败则删除创建的文件
						FILE_DISPOSITION_INFORMATION DispositionInformation;
						DispositionInformation.NeedDeleteFile = TRUE;
						pNtSetInformationFile(hFile, &IoStatusBlock, &DispositionInformation, sizeof(DispositionInformation), FileDispositionInformation);
					}


					pNtClose(hFile);
				}

				pRtlFreeUnicodeString(&NtSymlinkFileName);
			} while (false);


			if (!bRelative)
				pRtlFreeUnicodeString(&NtName);


		} while (false);

		if (pReparseData)
			HeapFree(ProcessHeap, 0, pReparseData);

		if(lpFullTargetFileName)
			HeapFree(ProcessHeap, 0, lpFullTargetFileName);

		RevertToSelf();

		if (lStatus)
		{
			SetLastError(lStatus);
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
}

#endif

EXTERN_C
BOOLEAN
APIENTRY
CreateSymbolicLinkW(
    _In_ LPCWSTR lpSymlinkFileName,
    _In_ LPCWSTR lpTargetFileName,
    _In_ DWORD dwFlags
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (const auto pCreateSymbolicLinkW = try_get_CreateSymbolicLinkW())
	{
		return pCreateSymbolicLinkW(lpSymlinkFileName, lpTargetFileName, dwFlags);
	}

	return internal::CreateSymbolicLinkW_Downlevel(lpSymlinkFileName, lpTargetFileName, dwFlags);
}
#endif

__YY_Thunks_Expand_Function(kernel32, CreateSymbolicLinkW, 12);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista [desktop apps only]
//Windows Server 2008[desktop apps only]

EXTERN_C
BOOLEAN
APIENTRY
CreateSymbolicLinkA(
    _In_ LPCSTR lpSymlinkFileName,
    _In_ LPCSTR lpTargetFileName,
    _In_ DWORD dwFlags
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (const auto pCreateSymbolicLinkA = try_get_CreateSymbolicLinkA())
	{
		return pCreateSymbolicLinkA(lpSymlinkFileName, lpTargetFileName, dwFlags);
	}

	LSTATUS lStatus;
	
	do
	{
		if (lpSymlinkFileName == nullptr || lpTargetFileName == nullptr)
		{
			//参数错误
			lStatus = ERROR_INVALID_PARAMETER;
			break;
		}

		wchar_t SymlinkFileNameBuffer[512];

		UNICODE_STRING SymlinkFileName = { 0, sizeof(SymlinkFileNameBuffer), SymlinkFileNameBuffer };

		lStatus = internal::Basep8BitStringToStaticUnicodeString(&SymlinkFileName, lpSymlinkFileName);

		if (lStatus != ERROR_SUCCESS)
		{
			break;
		}

		wchar_t TargetFileNameBuffer[512];
		UNICODE_STRING TargetFileName = { 0, sizeof(TargetFileNameBuffer), TargetFileNameBuffer };

		lStatus = internal::Basep8BitStringToStaticUnicodeString(&TargetFileName, lpTargetFileName);

		if (lStatus != ERROR_SUCCESS)
		{
			break;
		}

		return internal::CreateSymbolicLinkW_Downlevel(SymlinkFileName.Buffer, TargetFileName.Buffer, dwFlags);

	} while (false);


	SetLastError(lStatus);

	return FALSE;
}
#endif

__YY_Thunks_Expand_Function(kernel32, CreateSymbolicLinkA, 12);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)
//Windows Vista [desktop apps only]
//Windows Server 2003 [desktop apps only]

EXTERN_C
HANDLE
WINAPI
ReOpenFile(
    _In_ HANDLE  hOriginalFile,
    _In_ DWORD   dwDesiredAccess,
    _In_ DWORD   dwShareMode,
    _In_ DWORD   dwFlagsAndAttributes
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (const auto pReOpenFile = try_get_ReOpenFile())
	{
		return pReOpenFile(hOriginalFile, dwDesiredAccess, dwShareMode, dwFlagsAndAttributes);
	}

	NTSTATUS Status;

	do
	{
		const auto pNtCreateFile = try_get_NtCreateFile();
		if (!pNtCreateFile)
		{
			Status = STATUS_INVALID_INFO_CLASS;
			break;
		}


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

		if (FILE_FLAG_OPEN_NO_RECALL & dwFlagsAndAttributes)
		{
			SecurityQualityOfService.ImpersonationLevel = SECURITY_IMPERSONATION_LEVEL((dwFlagsAndAttributes >> 16) & 0x3);

			if (FILE_FLAG_OPEN_REQUIRING_OPLOCK & dwFlagsAndAttributes)
			{
				SecurityQualityOfService.ContextTrackingMode = TRUE;
			}
			else
			{
				SecurityQualityOfService.ContextTrackingMode = FALSE;
			}


			if (FILE_FLAG_FIRST_PIPE_INSTANCE & dwFlagsAndAttributes)
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

		Status = pNtCreateFile(
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

__YY_Thunks_Expand_Function(kernel32, ReOpenFile, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WINXP)
//Windows XP [desktop apps | UWP apps]
//Windows Server 2003 [desktop apps | UWP apps]

EXTERN_C
BOOL
WINAPI
SetFilePointerEx(
    _In_ HANDLE hFile,
    _In_ LARGE_INTEGER liDistanceToMove,
    _Out_opt_ PLARGE_INTEGER lpNewFilePointer,
    _In_ DWORD dwMoveMethod
    )
#ifdef YY_Thunks_Defined
	;
#else
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

__YY_Thunks_Expand_Function(kernel32, SetFilePointerEx, 20);

#endif

	}//namespace Thunks

} //namespace YY