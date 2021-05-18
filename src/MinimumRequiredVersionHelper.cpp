

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <winnt.h>

int __cdecl wmain(int argc, wchar_t *argv[])
{
    _tsetlocale(0, _T(".936"));

    if (argc != 3)
    {
        return ERROR_INVALID_PARAMETER;
    }

    auto szMinimumRequiredVersion = argv[2];

    if (_wcsnicmp(
            szMinimumRequiredVersion,
            L"/MinimumRequiredVersion:",
            _countof(L"/MinimumRequiredVersion:") - 1) == 0)
    {
        szMinimumRequiredVersion += _countof(L"/MinimumRequiredVersion:") - 1;
    }
    else
    {
        wprintf(L"无法识别参数：%ws。\n", szMinimumRequiredVersion);

        return ERROR_INVALID_PARAMETER;
    }

    UINT16 MinimumRequiredVersion[2] = {};

    if (swscanf(
            szMinimumRequiredVersion,
            L"%hu.%hu",
            MinimumRequiredVersion + 1,
            MinimumRequiredVersion + 0) != 2)
    {
        wprintf(L"无法识别MinimumRequiredVersion：%ws。\n", szMinimumRequiredVersion);
        return ERROR_INVALID_PARAMETER;
    }

    LSTATUS lStatus = ERROR_SUCCESS;
    auto szFilePath = argv[1];

    auto hFile = CreateFileW(
        szFilePath,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        lStatus = GetLastError();

        wprintf(L"%ws 无法打开，错误代码：%u。\n", szFilePath, lStatus);
        return ERROR_SUCCESS;
    }

    do
    {
        DWORD cbRead;
        IMAGE_DOS_HEADER DosHeader;

        IMAGE_NT_HEADERS32 Header32;

        if (ReadFile(hFile, &DosHeader, sizeof(DosHeader), &cbRead, nullptr) == FALSE)
        {
            lStatus = GetLastError();

            wprintf(L"%ws 无法访问，错误代码：%u。\n", szFilePath, lStatus);
            break;
        }

        if (cbRead != sizeof(DosHeader) || DosHeader.e_magic != IMAGE_DOS_SIGNATURE)
        {
            lStatus = ERROR_BAD_FORMAT;
            wprintf(L"%ws 不是有效的PE文件。\n", szFilePath);
            break;
        }

        SetFilePointer(hFile, DosHeader.e_lfanew, nullptr, FILE_BEGIN);

        if (ReadFile(hFile, &Header32, sizeof(Header32), &cbRead, nullptr) == FALSE)
        {
            lStatus = GetLastError();

            wprintf(L"%ws 无法访问，错误代码：%u。\n", szFilePath, lStatus);
            break;
        }

        if (cbRead != sizeof(Header32) || Header32.Signature != IMAGE_NT_SIGNATURE)
        {
            lStatus = ERROR_BAD_FORMAT;
            wprintf(L"%ws 不是有效的PE文件。\n", szFilePath);
            break;
        }

        if (Header32.FileHeader.Machine != IMAGE_FILE_MACHINE_I386 ||
            UFIELD_OFFSET(decltype(Header32.OptionalHeader), DataDirectory) >
                Header32.OptionalHeader.SizeOfHeaders)
        {
            wprintf(L"%ws 不是i386 PE文件，无需更新。\n", szFilePath);
            break;
        }

        if (Header32.OptionalHeader.Subsystem != IMAGE_SUBSYSTEM_WINDOWS_GUI &&
            Header32.OptionalHeader.Subsystem != IMAGE_SUBSYSTEM_WINDOWS_CUI)
        {
            wprintf(L"%ws 不是GUI 或者 GUI PE文件，无需更新。\n", szFilePath);
            break;
        }

#define MakeMiniVersion(v1, v2) (DWORD)(v2 | (v1 << 16))

        bool bNeedUpdate = false;

        if (MakeMiniVersion(
                Header32.OptionalHeader.MajorOperatingSystemVersion,
                Header32.OptionalHeader.MinorOperatingSystemVersion) >
            *(DWORD *)MinimumRequiredVersion)
        {
            Header32.OptionalHeader.MajorOperatingSystemVersion = MinimumRequiredVersion[1];
            Header32.OptionalHeader.MinorOperatingSystemVersion = MinimumRequiredVersion[0];
            bNeedUpdate = true;
        }

        if (MakeMiniVersion(
                Header32.OptionalHeader.MajorSubsystemVersion,
                Header32.OptionalHeader.MinorSubsystemVersion) > *(DWORD *)MinimumRequiredVersion)
        {
            Header32.OptionalHeader.MajorSubsystemVersion = MinimumRequiredVersion[1];
            Header32.OptionalHeader.MinorSubsystemVersion = MinimumRequiredVersion[0];
            bNeedUpdate = true;
        }

        if (!bNeedUpdate)
        {
            wprintf(L"%ws 最低系统版本无需更新。\n", szFilePath);
            break;
        }

        auto hFileWriter = ReOpenFile(hFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0);

        if (hFileWriter == INVALID_HANDLE_VALUE)
        {
            lStatus = GetLastError();

            wprintf(L"%ws 无法更新，错误代码：%u。\n", szFilePath, lStatus);
            break;
        }

        SetFilePointer(
            hFileWriter,
            DosHeader.e_lfanew + UFIELD_OFFSET(decltype(Header32), OptionalHeader),
            nullptr,
            FILE_BEGIN);

        auto &cbWriter = cbRead;
        if (WriteFile(
                hFileWriter,
                &Header32.OptionalHeader,
                UFIELD_OFFSET(decltype(Header32.OptionalHeader), DataDirectory),
                &cbWriter,
                nullptr))
        {
            wprintf(
                L"%ws 最低支持版本成功更新到 %hu.%hu！\n",
                szFilePath,
                MinimumRequiredVersion[1],
                MinimumRequiredVersion[0]);
        }
        else
        {
            lStatus = GetLastError();

            wprintf(L"%ws 无法更新，错误代码：%u。\n", szFilePath, lStatus);
        }

        CloseHandle(hFileWriter);

    } while (false);

    CloseHandle(hFile);

    return ERROR_SUCCESS;
}
