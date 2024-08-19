// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。
std::string ReadFileData(LPCWSTR _szFilePath)
{
    std::string _FileData;
    auto _hFile = CreateFileW(_szFilePath, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (_hFile == INVALID_HANDLE_VALUE)
        return _FileData;

    LARGE_INTEGER _FileSize;
    if (GetFileSizeEx(_hFile, &_FileSize))
    {
        if (_FileSize.HighPart)
        {
            // 不支持超过4GB的文件
            return _FileData;
        }

        _FileData.resize(_FileSize.LowPart);

        if (!ReadFile(_hFile, _FileData.data(), _FileSize.LowPart, &_FileSize.LowPart, nullptr))
            _FileData.clear();
    }

    CloseHandle(_hFile);
    return _FileData;
}
