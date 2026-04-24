#include "pch.h"
#include "Thunks/api-ms-win-core-file.hpp"

namespace api_ms_win_core_file {

TEST_CLASS(GetFileInformationByHandleEx)
{
    AwaysNullGuard Guard;

public:
    GetFileInformationByHandleEx()
    {
        Guard |= YY::Thunks::aways_null_try_get_GetFileInformationByHandleEx;  
    }

    TEST_METHOD(FileIdInfo)
    {
        g_uSystemVersion = MakeVersion(6, 1, 7601, 0);

        auto _hFile = CreateFileW(LR"(C:\Windows\System32\ntdll.dll)", GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        Assert::AreNotEqual(_hFile, INVALID_HANDLE_VALUE);

        FILE_ID_INFO _oFileIdInfo;
        memset(&_oFileIdInfo, 0xCC, sizeof(_oFileIdInfo));
        Assert::IsTrue(::GetFileInformationByHandleEx(_hFile, ::FileIdInfo, &_oFileIdInfo, sizeof(_oFileIdInfo)));

        BY_HANDLE_FILE_INFORMATION _oFileInformation;
        Assert::IsTrue(GetFileInformationByHandle(_hFile, &_oFileInformation));
        CloseHandle(_hFile);
        g_uSystemVersion = 0;

        Assert::AreEqual(_oFileIdInfo.VolumeSerialNumber, ULONGLONG(_oFileInformation.dwVolumeSerialNumber));
        Assert::AreEqual(((ULONGLONG*)&_oFileIdInfo.FileId)[0], ULONGLONG(_oFileInformation.nFileIndexHigh) << 32 | _oFileInformation.nFileIndexLow);
        Assert::AreEqual(((ULONGLONG*)&_oFileIdInfo.FileId)[1], ULONGLONG(0));

    }
};

} // namespace api_ms_win_core_file
