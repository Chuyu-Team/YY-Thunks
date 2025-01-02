#include <tchar.h>
#include <vector>
#include <map>
#include <set>

#include <Windows.h>
#include <winnt.h>
#include "..\Shared\km.h"

#include <atlstr.h>

#include "WinDepends.Core/apisetx.h"

#pragma comment(lib, "ntdll.lib")

CStringW g_szRoot;

DWORD g_InputRootCount;

bool g_bIgnoreReady= false;
bool g_bCheckBoxView = false;

__forceinline constexpr UINT64 __fastcall MakeVersion(_In_ UINT16 _uMajorVersion, _In_ UINT16 _uMinorVersion, UINT16 v3, UINT16 v4)
{
    return ((UINT64)_uMajorVersion << 48) | ((UINT64)_uMinorVersion << 32) | ((UINT64)v3 << 16) | (UINT64)v4;
}

CStringA ReadFileData(LPCWSTR szFile)
{
    CStringA _Result;
    auto _hFile = CreateFileW(szFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, 0, nullptr);
    if (_hFile == INVALID_HANDLE_VALUE)
        return _Result;

    DWORD cbData = GetFileSize(_hFile, nullptr);
    ReadFile(_hFile, _Result.GetBufferSetLength(cbData), cbData, &cbData, nullptr);
    CloseHandle(_hFile);
    return _Result;
}

HRESULT WriteFileData(LPCWSTR szFile, const void* _pData, DWORD _cbData)
{
    auto _hFile = CreateFileW(szFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (_hFile == INVALID_HANDLE_VALUE)
    {
        auto _hr = __HRESULT_FROM_WIN32(GetLastError());
        wprintf(L"错误：无法创建文件 %s, hr=0x%.8X\n", szFile, _hr);
        return _hr;
    }

    WriteFile(_hFile, _pData, _cbData, &_cbData, nullptr);
    CloseHandle(_hFile);
    return S_OK;
}

#define IMAGE_FIRST_DIRECTORY(ntheader) (IMAGE_DATA_DIRECTORY*)((byte*)IMAGE_FIRST_SECTION(ntheader)-sizeof(IMAGE_DATA_DIRECTORY)*IMAGE_NUMBEROF_DIRECTORY_ENTRIES)

struct DllImportItem
{
    CStringA szDllName;
    std::vector<CStringA> ImportNames;
};

struct DllExportItem
{
    CStringA szExportName;
    UINT16 uOrdinal = -1;
};

class PEImage
{
private:
    void* pBase = nullptr;
    PIMAGE_NT_HEADERS pNtHeader = nullptr;

public:
    HRESULT Init(_In_z_ LPCWSTR _szFilePath) noexcept
    {
        auto _hFile = CreateFileW(_szFilePath, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, NULL);
        if (INVALID_HANDLE_VALUE == _hFile)
        {
            return __HRESULT_FROM_WIN32(GetLastError());
        }

        HANDLE _hMap = NULL;
        HRESULT _hr = S_OK;
        do
        {
            _hMap = CreateFileMappingW(_hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
            if (!_hMap)
            {
                _hr = __HRESULT_FROM_WIN32(GetLastError());
                break;
            }

            pBase = MapViewOfFile(_hMap, FILE_MAP_READ, 0, 0, 0);
            if (!pBase)
            {
                _hr = __HRESULT_FROM_WIN32(GetLastError());
                break;
            }

            pNtHeader = YY::RtlImageNtHeader((PVOID)pBase);
            if (!pNtHeader)
            {
                UnmapViewOfFile(pBase);
                pBase = nullptr;
                _hr = __HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
            }
        } while (false);
        if(_hMap)
            CloseHandle(_hMap);

        if(INVALID_HANDLE_VALUE != _hFile)
            CloseHandle(_hFile);
        return _hr;
    }

    ~PEImage()
    {
        if (pBase)
        {
            UnmapViewOfFile(pBase);
        }
    }

    WORD GetMachine() const
    {
        return pNtHeader->FileHeader.Machine;
    }

    std::vector<DllImportItem> GetImport() const
    {
        std::vector<DllImportItem> Import;
        if (pNtHeader)
        {
            switch (pNtHeader->FileHeader.Machine)
            {
            case IMAGE_FILE_MACHINE_I386:
            case IMAGE_FILE_MACHINE_ARMNT:
                BuildImport<IMAGE_THUNK_DATA32>(pBase, pNtHeader, Import);
                break;
            case IMAGE_FILE_MACHINE_AMD64:
            case IMAGE_FILE_MACHINE_ARM64:
                BuildImport<IMAGE_THUNK_DATA64>(pBase, pNtHeader, Import);
                break;
            default:
                break;
            }
        }
        return Import;
    }

    std::vector<DllImportItem> GetDelayImport() const
    {
        std::vector<DllImportItem> DelayImport;
        if (pNtHeader)
        {
            switch (pNtHeader->FileHeader.Machine)
            {
            case IMAGE_FILE_MACHINE_I386:
            case IMAGE_FILE_MACHINE_ARMNT:
                BuildDelayImport<IMAGE_THUNK_DATA32>(pBase, pNtHeader, DelayImport);
                break;
            case IMAGE_FILE_MACHINE_AMD64:
            case IMAGE_FILE_MACHINE_ARM64:
                BuildDelayImport<IMAGE_THUNK_DATA64>(pBase, pNtHeader, DelayImport);
                break;
            default:
                break;
            }
        }

        return DelayImport;
    }

    std::map<DWORD, CStringA> GetExport() const
    {
        std::map<DWORD, CStringA> _Result;
        if (!pNtHeader)
            return _Result;

        auto pDirectorys = IMAGE_FIRST_DIRECTORY(pNtHeader);
        auto& _Export = pDirectorys[IMAGE_DIRECTORY_ENTRY_EXPORT];

        auto _pExport = (_IMAGE_EXPORT_DIRECTORY*)YY::RtlImageRvaToVa(pNtHeader, pBase, _Export.VirtualAddress, NULL);
        if (!_pExport)
            return _Result;

        const auto _uBase = _pExport->Base;
        auto _puNameOrdinals = (const WORD*)YY::RtlImageRvaToVa(pNtHeader, pBase, _pExport->AddressOfNameOrdinals, NULL);
        auto _puFunctions = (const DWORD*)YY::RtlImageRvaToVa(pNtHeader, pBase, _pExport->AddressOfFunctions, NULL);

        if (!_puFunctions)
            return _Result;

        DWORD i = 0;
        if (_puNameOrdinals)
        {
            auto _puNames = (const DWORD*)YY::RtlImageRvaToVa(pNtHeader, pBase, _pExport->AddressOfNames, NULL);
            if (_puNames)
            {
                for (; i != _pExport->NumberOfNames; ++i)
                {
                    auto _szName = (char*)YY::RtlImageRvaToVa(pNtHeader, pBase, _puNames[i], NULL);
                    if (!_szName)
                        continue;

                    _Result[_puNameOrdinals[i] + _uBase] = _szName;
                }
            }
        }

        for (i = 0; i != _pExport->NumberOfFunctions; ++i)
        {
            if (_puFunctions[i])
            {
                _Result[i + _uBase];
            }
        }

        return _Result;
    }

private:
    template<class IMAGE_THUNK_DATA_T>
    static void BuildImport(PVOID pBase, PIMAGE_NT_HEADERS pNtHeader, std::vector<DllImportItem>& Infos)
    {
        auto pDirectorys = IMAGE_FIRST_DIRECTORY(pNtHeader);

        auto& Imort = pDirectorys[IMAGE_DIRECTORY_ENTRY_IMPORT];

        auto pImport = (IMAGE_IMPORT_DESCRIPTOR*)YY::RtlImageRvaToVa(pNtHeader, pBase, Imort.VirtualAddress, NULL);

        if (!pImport)
        {
            return;
        }

        for (; pImport->Name; ++pImport)
        {
            auto DllName = (const char*)YY::RtlImageRvaToVa(pNtHeader, pBase, pImport->Name, NULL);
            if (!DllName)
            {
                continue;
            }

            DllImportItem Info;
            Info.szDllName = DllName;

            auto pThunkData = (IMAGE_THUNK_DATA_T*)YY::RtlImageRvaToVa(pNtHeader, pBase, pImport->OriginalFirstThunk, NULL);
            if (!pThunkData)
            {
                //wprintf(L"Error：程序无法读取 dllname = %S OriginalFirstThunk Rva= 0x%.8X。\n", DllName, pImport->OriginalFirstThunk);

                continue;
            }


            BuildProcNames(pBase, pNtHeader, pThunkData, Info.ImportNames);

            Infos.push_back(std::move(Info));

        }
    }

    template<class IMAGE_THUNK_DATA_T>
    static void BuildDelayImport(PVOID pBase, PIMAGE_NT_HEADERS pNtHeader, std::vector<DllImportItem>& Infos)
    {
        auto pDirectorys = IMAGE_FIRST_DIRECTORY(pNtHeader);

        auto& Imort = pDirectorys[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT];

        if (Imort.VirtualAddress == 0)
            return;

        auto pImport = (IMAGE_DELAYLOAD_DESCRIPTOR*)YY::RtlImageRvaToVa(pNtHeader, pBase, Imort.VirtualAddress, NULL);
        if (!pImport)
        {
            //wprintf(L"Warning：无法加载导入表。\n");
            return;
        }

        for (; pImport->DllNameRVA; ++pImport)
        {
            auto DllName = (const char*)YY::RtlImageRvaToVa(pNtHeader, pBase, pImport->DllNameRVA, NULL);
            if (!DllName)
            {
                //wprintf(L"Error：程序无法读取 Rva= 0x%.8X。\n", pImport->DllNameRVA);

                continue;
            }

            DllImportItem Info;
            Info.szDllName = DllName;


            auto pThunkData = (IMAGE_THUNK_DATA_T*)YY::RtlImageRvaToVa(pNtHeader, pBase, pImport->ImportNameTableRVA, NULL);
            if (!pThunkData)
            {
                //wprintf(L"Error：程序无法读取 dllname = %S OriginalFirstThunk Rva= 0x%.8X。\n", DllName, pImport->ImportNameTableRVA);

                continue;
            }

            BuildProcNames(pBase, pNtHeader, pThunkData, Info.ImportNames);

            Infos.push_back(std::move(Info));

        }
    }

    template<class IMAGE_THUNK_DATA_T>
    static void BuildProcNames(PVOID pBase, PIMAGE_NT_HEADERS  pNtHeader, IMAGE_THUNK_DATA_T* pThunkData, std::vector<CStringA>& ProcNames)
    {
        for (; pThunkData->u1.AddressOfData; ++pThunkData)
        {
            if (pThunkData->u1.AddressOfData >> ((sizeof(pThunkData->u1.AddressOfData) * 8) - 1))
            {
                // 序号导入
                CStringA szOrdinal;
                szOrdinal.Format("#%d", DWORD(pThunkData->u1.Ordinal & 0xffff));
                ProcNames.push_back(szOrdinal);
            }
            else if (auto pImportByName = (PIMAGE_IMPORT_BY_NAME)YY::RtlImageRvaToVa(pNtHeader, pBase, pThunkData->u1.AddressOfData, NULL))
            {
                ProcNames.push_back(pImportByName->Name);
            }
            else
            {
                CStringW Error;
                Error.Format(L"Error：无法加载偏移 %I64X /?\n", (long long)pThunkData->u1.AddressOfData);
            }
        }
    }
};


enum class PEImportAnalyzerFlags
{
    None,
    BuildExport,
    BuildOrdinalMap,
};

PEImportAnalyzerFlags& operator|=(PEImportAnalyzerFlags& _Left, PEImportAnalyzerFlags _Rigth)
{
    (DWORD&)_Left |= (DWORD)_Rigth;
    return _Left;
}

PEImportAnalyzerFlags operator&(PEImportAnalyzerFlags _Left, PEImportAnalyzerFlags _Rigth)
{
    return PEImportAnalyzerFlags((DWORD)_Left & (DWORD)_Rigth);
}

void EndianChange(const byte* Src, byte* Dst, size_t cData)
{
    for (size_t i = 0; i != cData; ++i)
    {
        Dst[cData - i - 1] = Src[i];
    }
}


template<class Type>
Type EndianChange(const Type Src)
{
    Type Tmp;

    EndianChange((const byte*)&Src, (byte*)&Tmp, sizeof(Type));

    return Tmp;
}

typedef struct SectionHeader : public IMAGE_ARCHIVE_MEMBER_HEADER
{
    unsigned int get_Size()
    {
        return strtoul((char*)Size, nullptr, 10);
    }

    byte* get_Data()
    {
        auto Start = EndHeader;

        for (; *Start != '`' || *Start == '\n'; ++Start);

        return (byte*)Start + 2;
    }

    SectionHeader* get_NextHeader()
    {
        return (SectionHeader*)(get_Data() + get_Size());
    }
};


struct FirstSec
{
    unsigned long SymbolNum; // 库中符号的数量

    unsigned long get_SymbolNum()
    {
        return EndianChange(SymbolNum);
    }

    unsigned long SymbolOffset[0/*SymbolNum*/]; // 符号所在目标节的偏移

    unsigned long* get_SymbolOffset()
    {
        return SymbolOffset;
    }

    //char StrTable[SectionHeader.Size-(SymbolNum + 1) * 4]; // 符号名称字符串表

    LPCSTR get_StrTable()
    {
        return (LPCSTR)(SymbolOffset + get_SymbolNum());
    }
};

struct SecondSec
{
    unsigned long ObjNum; // Obj Sec的数量

    unsigned long get_ObjNum()
    {
        return ObjNum;
    }

    unsigned long ObjOffset[0/*ObjNum*/]; // 每一个Obj Sec的偏移

    unsigned long* get_ObjOffset()
    {
        return ObjOffset;
    }

    //unsigned long SymbolNum; // 库中符号的数量
    unsigned long get_SymbolNum()
    {
        return *(unsigned long*)(ObjOffset + get_ObjNum());
    }


    //unsigned short SymbolIdx[SymbolNum]; // 符号在ObjOffset表中的索引 这个索引是从 1开始的
    unsigned short* get_SymbolIdx()
    {
        return (unsigned short*)(ObjOffset + get_ObjNum() + 1);
    }

    //char StrTable[m]; // 符号名称字符串表
    LPCSTR get_StrTable()
    {
        return (LPCSTR)(get_SymbolIdx() + get_SymbolNum());
    }

};


template<size_t kCount>
LPWSTR __fastcall GetOptionValue(LPWSTR _szArg, const wchar_t (&_szOption)[kCount])
{
    /*
    /Option:Value
    -Option:Value
    --Option:Value
    */
    if (*_szArg == L'/')
    {
        ++_szArg;
    }
    else if (*_szArg == L'-')
    {
        ++_szArg;
        if (*_szArg == L'-')
        {
            ++_szArg;
        }
    }
    else
    {
        return nullptr;
    }

    if (wcsnicmp(_szArg, _szOption, kCount - 1) != 0)
    {
        return nullptr;
    }

    _szArg += kCount - 1;

    if (*_szArg == L':')
    {
        ++_szArg;
    }
    else if (*_szArg == L'=')
    {
        ++_szArg;
    }
    else if (*_szArg == ' ' || *_szArg == L'\0')
    {
        *_szArg = L'\0';
        return _szArg;
    }
    else
    {
        return nullptr;
    }

    if (*_szArg == L'\"')
    {
        PathUnquoteSpaces(_szArg);
    }

    return _szArg;
}

BOOL IniGetValues(LPCWSTR FilePath, LPCWSTR Path, CString& Values)
{
    Values.GetBuffer(1024);

    int cchData = Values.GetAllocLength();

    while (auto chNewData = GetPrivateProfileSectionW(Path, Values.GetBuffer(cchData), cchData, FilePath))
    {
        if (cchData - chNewData == 2)
        {
            cchData *= 2;
        }
        else
        {
            Values.ReleaseBuffer(chNewData);
            return TRUE;
        }
    }

    return FALSE;
}

HRESULT BuildExport(CStringW _szInputPath, std::map<CStringA, std::map<DWORD, CStringA>>& _Out);

HRESULT BuildExportFile(CStringW _szInputPath, std::map<CStringA, std::map<DWORD, CStringA>>& _Out);

struct AnalyzerProc
{
    CStringA szProc;
    CStringA szFriendlyName;

    // 引用这个函数的模块名称
    std::vector<CStringA> Ref;
};

struct AnalyzerModule
{
    CStringA szModuleName;
    std::map<CStringA, AnalyzerProc> Procs;
};

struct AnalyzerInfo
{
    //        CPU    模块信息
    std::map<CStringA, AnalyzerModule> Modules;
};

HRESULT AnalyzerImage(CStringW _szInputPath, std::map<WORD, AnalyzerInfo>& _AnalyzerInfo);

HRESULT AnalyzerImageDir(CStringW _szInputPath, std::map<WORD, AnalyzerInfo>& _AnalyzerInfos);

HRESULT BuildAnalyzer(CStringW _szOutputPath, CStringW _szTarget, std::map<WORD, AnalyzerInfo>& _AnalyzerInfos);

struct ExportModuleInfo
{
    std::map<DWORD, CStringA> OrdinalToNameMap;
    std::map<CStringA, DWORD> NameToOrdinalMap;

    bool FindImport(CStringA _szImportName) const
    {
        if (_szImportName.IsEmpty())
            return false;

        if (NameToOrdinalMap.find(_szImportName) != NameToOrdinalMap.end())
            return true;

        if (_szImportName[0] == L'#')
        {
            // 可能是索引
            auto _iter = OrdinalToNameMap.find(strtoul(_szImportName.GetString() + 1, nullptr, 10));
            if (_iter != OrdinalToNameMap.end())
                return true;
        }

        return false;
    }
};

std::map<WORD, std::map<CStringA, ExportModuleInfo>> g_AppFiles;

struct ExportCache
{
    //       模块名称  ExportModuleInfo
    std::map<CStringA, ExportModuleInfo> ModuleToExportModuleInfoMap;
    //     API导出名称  模块的名称
    std::map<CStringA, CStringA> ProcNameToModuleNameMap;

    std::map<CStringA, CStringA> APISetMap;

    WORD _Machine = IMAGE_FILE_MACHINE_UNKNOWN;

    ExportModuleInfo* GetExportModuleInfo(CStringA _szDllName)
    {
        _szDllName.MakeLower();
        auto _pInfo = GetExportModuleInfoBase(_szDllName);
        if (_pInfo)
            return _pInfo;

        auto _iter = APISetMap.find(_szDllName);
        if (_iter == APISetMap.end())
            return nullptr;

        return GetExportModuleInfoBase(_iter->second);
    }

private:
    ExportModuleInfo* GetExportModuleInfoBase(CStringA _szDllName)
    {
        // 优先查找App目录的文件，虽然这个不是很正确，未考虑加载顺序，但是一般来说这样没有问题。
        auto _iterAppFiles = g_AppFiles.find(_Machine);
        if (_iterAppFiles != g_AppFiles.end())
        {
            auto _iterAppExport = _iterAppFiles->second.find(_szDllName);
            if (_iterAppExport != _iterAppFiles->second.end())
            {
                return &_iterAppExport->second;
            }
        }

        auto _iter = ModuleToExportModuleInfoMap.find(_szDllName);
        if (_iter != ModuleToExportModuleInfoMap.end())
        {
            return &_iter->second;
        }
        else
        {
            return nullptr;
        }
    }
};


std::map<CStringW, ExportCache> g_ExportModuleCache;

ExportCache* GetExportModule(CStringW _szDefFilePath, WORD _Machine)
{
    auto _szKey = _szDefFilePath;
    _szKey.MakeLower();

    auto& _Cache = g_ExportModuleCache[_szKey];
    if (_Cache.ModuleToExportModuleInfoMap.size())
        return &_Cache;
    _Cache._Machine = _Machine;

    FILE* file = _wfopen(_szDefFilePath, L"r");
    if (!file)
        return nullptr;

    ExportModuleInfo* _pTmp = nullptr;
    CStringA szModuleName;
    bool _bApiSet = false;

    char line[2048] = {};
    while (fgets(line, 2048, file))
    {
        if (line[0] == '\0')
            continue;
        auto Count = strlen(line);
        if (Count && line[Count - 1] == '\n')
        {
            --Count;
            line[Count] = '\0';

            if (Count && line[Count - 1] == '\r')
            {
                --Count;
                line[Count] = '\0';
            }
        }

        if (line[0] == '\0' || line[0] == ';')
            continue;

        if (line[0] == '[' && line[Count - 1] == ']')
        {
            szModuleName = CStringA(line + 1, Count - 2).MakeLower();
            if (stricmp(szModuleName, "ApiSet") == 0)
            {
                _bApiSet = true;
                continue;
            }
            else
            {
                _bApiSet = false;
                _pTmp = &_Cache.ModuleToExportModuleInfoMap[szModuleName];
            }
        }

        if (!_pTmp)
            continue;

        if (_bApiSet)
        {
            if (auto _szValue = strchr(line, '='))
            {
                _Cache.APISetMap[CStringA(line, _szValue - line).MakeLower()] = CStringA(_szValue + 1).MakeLower();
            }
            continue;
        }


        auto _szName = line;
        auto _Ordinal = strtoul(line, (char**)&_szName, 10);

        if (_szName == line)
        {
            continue;
        }

        if (*_szName == '\0')
        {
            _pTmp->OrdinalToNameMap[_Ordinal];
        }
        else if (*_szName == '=')
        {
            ++_szName;

            CStringA _szNameA(_szName);
            _szNameA.Trim();

            _pTmp->OrdinalToNameMap[_Ordinal] = _szNameA;
            _pTmp->NameToOrdinalMap[_szNameA] = _Ordinal;
            _Cache.ProcNameToModuleNameMap[_szNameA] = szModuleName;
        }
    }

    fclose(file);

    return &_Cache;
}

HRESULT BuildOrdinalMap(CStringW _szInput, CStringW _szOutput);

CStringA FindInAllTarget(CStringA _szDllName, CStringA _szImportName, CStringW _szBaseDefFileDir, LPCWSTR _szCurrentTarget, WORD _Machine)
{
    CStringA _szResult;
    if (_szBaseDefFileDir.IsEmpty() || _szCurrentTarget == nullptr)
        return _szResult;

    if (_szBaseDefFileDir[_szBaseDefFileDir.GetLength() - 1] != L'\\')
        _szBaseDefFileDir += L'\\';

    WIN32_FIND_DATAW _FindFileData;
    auto _hFindFile = FindFirstFileW(_szBaseDefFileDir + L"*.*.*.txt", &_FindFileData);
    if (_hFindFile == INVALID_HANDLE_VALUE)
    {
        return _szResult;
    }
    _szDllName.MakeLower();

    UINT16 _CurrentTargetVersion[4] = {};
    swscanf_s(_szCurrentTarget, L"%hd.%hd.%hd.%hd", &_CurrentTargetVersion[3], &_CurrentTargetVersion[2], &_CurrentTargetVersion[1], &_CurrentTargetVersion[0]);

    do
    {
        if (_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        UINT16 _TargetVersion[4] = {};
        swscanf_s(_FindFileData.cFileName, L"%hd.%hd.%hd.%hd", &_TargetVersion[3], &_TargetVersion[2], &_TargetVersion[1], &_TargetVersion[0]);

        if (*(UINT64*)_TargetVersion > *(UINT64*)_CurrentTargetVersion)
        {
            if (auto _pCahce = GetExportModule(_szBaseDefFileDir + _FindFileData.cFileName, _Machine))
            {
                if (auto _pModuleInfo = _pCahce->GetExportModuleInfo(_szDllName))
                {
                    if (_pModuleInfo->FindImport(_szImportName))
                    {
                        if (_szResult.GetLength())
                        {
                            _szResult += ',';
                            _szResult += ' ';
                        }
                        _szResult += _FindFileData.cFileName;
                        // 去掉后缀 ".txt"
                        _szResult.ReleaseBufferSetLength(_szResult.GetLength() - 4);
                    }
                }
            }

        }

    } while (FindNextFileW(_hFindFile, &_FindFileData));

    FindClose(_hFindFile);

    return _szResult;
}

int __cdecl wmain(int argc, wchar_t* argv[])
{
    _tsetlocale(0, _T(".936"));

    if (argc < 2)
    {
    __INVALIDARG:
        wprintf(LR"(无效参数！请提供可执行文件路径。或者包含可执行文件的文件夹路径。
  YY.Depends.Analyzer {可执行文件路径或者文件夹} [/IgnoreReady] [/ReportView:{Table|CheckBox}]
                  - /IgnoreReady : 自动跳过YY-Thunk已经支持的接口，减少信息干扰
                  - /ReportView:{Table|CheckBox} : 如果选择CheckBox，那么输出位CheckBox视图。默认是Table（表格）
                  - /Target:<SystemVersion> : 需要检测是目标版本，值可以是5.1.2600、5.2.3790、6.1.7600等。


比如：
  YY.Depends.Analyzer "D:\Test\Test.exe"
  YY.Depends.Analyzer "D:\Test"
  YY.Depends.Analyzer "D:\Test" /IgnoreReady /ReportView:CheckBox
  YY.Depends.Analyzer "D:\Test" /IgnoreReady /ReportView:CheckBox /Target:6.1.7600

)");
        return E_INVALIDARG;
    }

    constexpr auto kDefaultOutPathBufferLength = 512u;

    auto _uResult = GetModuleFileNameW((HMODULE)&__ImageBase, g_szRoot.GetBuffer(kDefaultOutPathBufferLength), kDefaultOutPathBufferLength);
    if (_uResult == 0 || _uResult >= kDefaultOutPathBufferLength)
    {
        auto _hr = __HRESULT_FROM_WIN32(GetLastError());
        wprintf(L"无法获取当前可执行文件路径：hr = 0x%.8X\n", _hr);
        return _hr;
    }
    g_szRoot.ReleaseBufferSetLength(_uResult);
    g_szRoot.ReleaseBufferSetLength(g_szRoot.ReverseFind(L'\\') + 1);



    auto _pArgv = argv + 1;
    const auto _pArgvEnd = argv + argc;

    PEImportAnalyzerFlags _Flags = PEImportAnalyzerFlags::None;

    CStringW _szInputPath;
    CStringW _szOutPath;
    LPCWSTR _szTarget = nullptr;

    for (; _pArgv != _pArgvEnd; ++_pArgv)
    {
        if (auto _szValue = GetOptionValue(*_pArgv, L"BuildExport"))
        {
            _szInputPath = _szValue;
            if (_szInputPath.IsEmpty())
                goto __INVALIDARG;

            _Flags |= PEImportAnalyzerFlags::BuildExport;
        }
        else if (auto _szValue = GetOptionValue(*_pArgv, L"BuildOrdinalMap"))
        {
            _szInputPath = _szValue;
            if (_szInputPath.IsEmpty())
                goto __INVALIDARG;

            _Flags |= PEImportAnalyzerFlags::BuildOrdinalMap;
            
        }
        else if (auto _szValue = GetOptionValue(*_pArgv, L"OutDir"))
        {
            _szOutPath = _szValue;
            if (_szOutPath.IsEmpty())
                goto __INVALIDARG;
        }
        else if (auto _szValue = GetOptionValue(*_pArgv, L"Target"))
        {
            if(*_szValue == L'\0' || _szTarget)
                goto __INVALIDARG;

            _szTarget = _szValue;
        }
        else if (auto _szValue = GetOptionValue(*_pArgv, L"IgnoreReady"))
        {
            g_bIgnoreReady = true;
        }
        else if (auto _szValue = GetOptionValue(*_pArgv, L"ReportView"))
        {
            g_bCheckBoxView = wcsicmp(_szValue, L"CheckBox") == 0;
        }
        else if (wcsicmp(*_pArgv, L"/?") == 0 || wcsicmp(*_pArgv, L"--?") == 0 || wcsicmp(*_pArgv, L"/help") == 0)
        {
            goto __INVALIDARG;
        }
        else
        {
            if(_szInputPath.GetLength())
                goto __INVALIDARG;
            _szInputPath = *_pArgv;
        }
    }

    if (_szInputPath.IsEmpty())
    {
        goto __INVALIDARG;
    }

    if (_szOutPath.IsEmpty())
    {
        _szOutPath = g_szRoot;
    }

    if ((_Flags & PEImportAnalyzerFlags::BuildExport) == PEImportAnalyzerFlags::BuildExport)
    {
        if (_szOutPath[_szOutPath.GetLength() - 1] != L'\\')
            _szOutPath += L'\\';

        PVOID OldValue = nullptr;
        Wow64DisableWow64FsRedirection(&OldValue);

        std::map<CStringA, std::map<DWORD, CStringA>> _Out;
        std::vector<ApisetNameSet> _oApiSetInfo;

        if (PathIsDirectoryW(_szInputPath))
        {
            if (_szInputPath[_szInputPath.GetLength() - 1] != L'\\')
                _szInputPath += L'\\';
            _szOutPath += L"DllMap.txt";

            auto _szSystem32Path = _szInputPath + L"System32\\";
            if (PathIsDirectoryW(_szSystem32Path))
            {
                // 这是一个系统目录
                g_InputRootCount = _szSystem32Path.GetLength();
                BuildExport(_szSystem32Path, _Out);

                PEImage _Image;
                auto _hr = _Image.Init(_szInputPath + L"System32\\ntdll.dll");
                if (SUCCEEDED(_hr))
                {
                    CStringW _szProcessorArchitecture;
                    // 简单处理一下需要旁载的DLL。
                    WIN32_FIND_DATAW _FindFileData;
                    if (_Image.GetMachine() == IMAGE_FILE_MACHINE_I386)
                    {
                        _szProcessorArchitecture = L"x86";
                    }
                    else if (_Image.GetMachine() == IMAGE_FILE_MACHINE_AMD64)
                    {
                        _szProcessorArchitecture = L"amd64";
                    }

                    if (_szProcessorArchitecture.GetLength())
                    {
                        auto _szWinSxSPath = _szInputPath + L"winsxs\\";

                        static const LPCWSTR s_szNames[] =
                        {
                            L"Microsoft.Windows.Common-Controls",
                            L"Microsoft.Windows.GdiPlus",
                            L"Microsoft.Windows.WinHTTP",
                        };

                        for (auto _szName : s_szNames)
                        {
                            auto _hFind = FindFirstFileW(_szWinSxSPath + _szProcessorArchitecture + L"_" + _szName + L"_*", &_FindFileData);
                            if (_hFind == INVALID_HANDLE_VALUE)
                                continue;

                            do
                            {
                                if ((_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
                                    continue;

                                auto _szTmp = _szWinSxSPath + _FindFileData.cFileName + L'\\';
                                g_InputRootCount = _szTmp.GetLength();
                                BuildExport(_szTmp, _Out);

                            } while (FindNextFileW(_hFind, &_FindFileData));

                            FindClose(_hFind);
                        }
                    }

                    if (auto _pApisetNamespace = LoadApisetNamespace(_szInputPath + LR"(System32\apisetschema.dll)"))
                    {
                        _oApiSetInfo = GetAllApisetNames(_pApisetNamespace);
                    }
                }
            }
            else
            {
                g_InputRootCount = _szInputPath.GetLength();
                BuildExport(_szInputPath, _Out);
            }
        }
        else
        {            
            _szOutPath += PathFindFileNameW(_szInputPath);
            _szOutPath += L".txt";
            BuildExportFile(_szInputPath, _Out);
        }

        Wow64RevertWow64FsRedirection(&OldValue);

        CStringA _szData;
        for (auto& _Dll : _Out)
        {
            _szData += '[';
            _szData += _Dll.first;
            _szData += ']';
            _szData += '\n';

            for (auto& _Proc : _Dll.second)
            {
                if (_Proc.second.GetLength())
                {
                    _szData.AppendFormat("%d=%s", _Proc.first, _Proc.second.GetString());
                }
                else
                {
                    _szData.AppendFormat("%d", _Proc.first);
                }
                _szData += '\n';
            }
            _szData += '\n';
        }

        if (_oApiSetInfo.size())
        {
            _szData += "[ApiSet]\n";
            for (auto& Item : _oApiSetInfo)
            {
                CStringA _szApisetName = Item.ApisetName;
                CStringA _szResolveName(Item.ResolveName.Buffer, Item.ResolveName.Length / 2);

                const auto _szApisetExtension = PathFindExtensionA(_szApisetName);
                auto _szResolveExtension = PathFindExtensionA(_szResolveName);

                if (_szApisetExtension == nullptr || *_szApisetExtension == '\0')
                {
                    _szApisetName += _szResolveExtension ? _szResolveExtension : ".dll";
                }

                if (_szResolveExtension == nullptr || *_szResolveExtension == '\0')
                {
                    _szResolveName += ".dll";
                }
                _szData.Append(_szApisetName);
                _szData += '=';
                _szData.Append(_szResolveName);
                _szData += '\n';
            }
        }

        WriteFileData(_szOutPath, _szData.GetString(), _szData.GetLength());

    }
    else if ((_Flags & PEImportAnalyzerFlags::BuildOrdinalMap) == PEImportAnalyzerFlags::BuildOrdinalMap)
    {
        _szOutPath += L"OrdinalMap.txt";
        return BuildOrdinalMap(_szInputPath, _szOutPath);
    }
    else
    {
        std::map<WORD, AnalyzerInfo> _AnalyzerInfos;
        if (PathIsDirectoryW(_szInputPath))
        {

            while (_szInputPath.GetLength() && _szInputPath[_szInputPath.GetLength() - 1] == L'\\')
                _szInputPath.GetBufferSetLength(_szInputPath.GetLength() - 1);

            _szOutPath += PathFindFileNameW(_szInputPath);
            _szOutPath += L".md";

            _szInputPath += L'\\';
            g_InputRootCount = _szInputPath.GetLength();

            auto _hr = AnalyzerImageDir(_szInputPath, _AnalyzerInfos);
        }
        else
        {
            if (_szOutPath[_szOutPath.GetLength() - 1] != L'\\')
                _szOutPath += L'\\';
            _szOutPath += PathFindFileNameW(_szInputPath);
            _szOutPath += L".md";
            auto _hr = AnalyzerImage(_szInputPath, _AnalyzerInfos);
        }

        BuildAnalyzer(_szOutPath, _szTarget, _AnalyzerInfos);
    }

    return 0;
}

HRESULT BuildExportFile(CStringW _szInputPath, std::map<CStringA, std::map<DWORD, CStringA>>& _Out)
{
    PEImage _Image;
    auto _hr = _Image.Init(_szInputPath);
    if (FAILED(_hr))
    {
        wprintf(L"错误：PEImage无法打开文件 %s, hr=0x%.8X\n", _szInputPath.GetString(), _hr);
        return _hr;
    }

    auto _Export = _Image.GetExport();
    if (_Export.empty())
        return S_OK;

    CStringA FileName(PathFindFileNameW(_szInputPath));
    FileName.MakeLower();
    _Out[FileName] = std::move(_Export);
    wprintf(L"信息：为 %s 成功生成导出定义。\n", _szInputPath.GetString());
    return S_OK;
}

HRESULT BuildExport(CStringW _szInputPath, std::map<CStringA, std::map<DWORD, CStringA>>& _Out)
{
    if (_szInputPath.IsEmpty())
        return E_INVALIDARG;

    if (_szInputPath[_szInputPath.GetLength() - 1] != L'\\')
        _szInputPath += L'\\';

    WIN32_FIND_DATAW _FindFileData;
    auto _hFileFind = FindFirstFileW(_szInputPath + L"*", &_FindFileData);
    if (_hFileFind == INVALID_HANDLE_VALUE)
    {
        return __HRESULT_FROM_WIN32(GetLastError());
    }

    do
    {
        if (_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        auto _szExtension = PathFindExtensionW(_FindFileData.cFileName);
        if (!_szExtension)
            continue;

        if (wcsicmp(_szExtension, L".dll") == 0 || wcsicmp(_szExtension, L".DRV") == 0 || wcsicmp(_szExtension, L".cpl") == 0)
        {
            BuildExportFile(_szInputPath + _FindFileData.cFileName, _Out);
        }

    } while (FindNextFileW(_hFileFind, &_FindFileData));

    FindClose(_hFileFind);

    return S_OK;
}

HRESULT AnalyzerImage(CStringW _szInputPath, std::map<WORD, AnalyzerInfo>& _AnalyzerInfos)
{
    PEImage _Image;
    auto _hr = _Image.Init(_szInputPath);
    if (FAILED(_hr))
    {
        wprintf(L"错误：PEImage无法打开文件 %s, hr=0x%.8X\n", _szInputPath.GetString(), _hr);
        return _hr;
    }
    const auto _Machine = _Image.GetMachine();
    auto&& _Export = _Image.GetExport();
    if (_Export.size())
    {
        auto _szFileName = PathFindFileNameW(_szInputPath);
        if (_szFileName)
        {
            auto& _ExportModule = g_AppFiles[_Machine][CStringA(_szFileName).MakeLower()];
            if (_ExportModule.OrdinalToNameMap.empty())
            {
                for (auto& _Item : _Export)
                {
                    _ExportModule.NameToOrdinalMap[_Item.second] = _Item.first;
                }

                _ExportModule.OrdinalToNameMap = std::move(_Export);
            }
            else
            {
                wprintf(L"警告：存在同名文件 %s，依赖分析可能出现错误。\n", _szFileName);
            }
        }
    }

    std::vector<DllImportItem> _Imports[2] = {_Image.GetImport(), _Image.GetDelayImport()};
    if (_Imports[0].empty() && _Imports[1].empty())
        return S_OK;

    auto& _AnalyzerInfo = _AnalyzerInfos[_Machine];

    for (auto i = 0; i != _countof(_Imports); ++i)
    {
        for (auto& _Item : _Imports[i])
        {
            AnalyzerModule* _pModule = nullptr;
            for (auto& _Import : _Item.ImportNames)
            {
                if (_Import.IsEmpty())
                    continue;

                if (!_pModule)
                {
                    auto _Key = _Item.szDllName;
                    _Key.MakeLower();
                    auto& _Module = _AnalyzerInfo.Modules[_Key];
                    if (_Module.szModuleName.IsEmpty())
                    {
                        _Module.szModuleName = _Item.szDllName;
                    }
                    _pModule = &_Module;
                }

                auto& _Proc = _pModule->Procs[_Import];
                if (_Proc.szProc.IsEmpty())
                    _Proc.szProc = _Import;

                auto _szInputSubPath = _szInputPath.GetString() + g_InputRootCount;
                if (i)
                {
                    _Proc.Ref.push_back(CStringA("[Delay] ") + _szInputSubPath);
                }
                else
                {
                    _Proc.Ref.push_back(_szInputSubPath);
                }
            }
        }
    }

    return S_OK;
}

HRESULT AnalyzerImageDir(CStringW _szInputPath, std::map<WORD, AnalyzerInfo>& _AnalyzerInfos)
{
    if (_szInputPath.IsEmpty())
        return E_INVALIDARG;

    if (_szInputPath[_szInputPath.GetLength() - 1] != L'\\')
        _szInputPath += L'\\';

    WIN32_FIND_DATAW _FindFileData;
    auto _hFileFind = FindFirstFileW(_szInputPath + L"*", &_FindFileData);
    if (_hFileFind == INVALID_HANDLE_VALUE)
    {
        return __HRESULT_FROM_WIN32(GetLastError());
    }

    do
    {
        if (_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (_FindFileData.cFileName[0] == L'.' && _FindFileData.cFileName[1] == L'\0')
                continue;

            if (_FindFileData.cFileName[0] == L'.' && _FindFileData.cFileName[1] == L'.' && _FindFileData.cFileName[2] == L'\0')
                continue;

            AnalyzerImageDir(_szInputPath + _FindFileData.cFileName, _AnalyzerInfos);
        }

        auto _szExtension = PathFindExtensionW(_FindFileData.cFileName);
        if (!_szExtension)
            continue;

        if (wcsicmp(_szExtension, L".exe") == 0 || wcsicmp(_szExtension, L".dll") == 0 || wcsicmp(_szExtension, L".DRV") == 0 || wcsicmp(_szExtension, L".cpl") == 0)
        {
            AnalyzerImage(_szInputPath + _FindFileData.cFileName, _AnalyzerInfos);
        }

    } while (FindNextFileW(_hFileFind, &_FindFileData));

    FindClose(_hFileFind);

    return S_OK;
}

bool IsInYY_Thunks(WORD _Machine, const AnalyzerProc& _ProcInfo)
{
    static std::set<CStringA> s_SymbolCache[2];
    int SymbolCacheIndex = 0;

    switch (_Machine)
    {
    case IMAGE_FILE_MACHINE_I386:
        SymbolCacheIndex = 0;
        break;
    case IMAGE_FILE_MACHINE_AMD64:
        SymbolCacheIndex = 1;
        break;
    default:
        return false;
    }

    auto& _Cache = s_SymbolCache[SymbolCacheIndex];

    if (_Cache.size() == 0)
    {
        static const LPCWSTR s_szSubPath[2] = { L"objs\\x86\\YY_Thunks_for_Win2K.obj" , L"objs\\x64\\YY_Thunks_for_WinXP.obj" };
        static const LPCWSTR s_szSubPathFallback[2] = { L"Lib\\5.0.2195.0\\x86\\YY_Thunks_for_5.0.2195.0.obj" , L"Lib\\5.2.3790.1180\\x64\\YY_Thunks_for_5.2.3790.1180.obj" };

        bool _bFallback = false;

        CStringA _FileData = ReadFileData(g_szRoot + s_szSubPath[SymbolCacheIndex]);
        if (_FileData.IsEmpty())
        {
            _FileData = ReadFileData(g_szRoot + s_szSubPathFallback[SymbolCacheIndex]);
            if (_FileData.IsEmpty())
                return false;

            _bFallback = true;
        }

        auto pData = _FileData.GetString();
        auto tttt = (IMAGE_FILE_HEADER*)pData;

        auto TTT2 = (IMAGE_SECTION_HEADER*)(pData + sizeof(IMAGE_FILE_HEADER));

        auto TTTT = (IMAGE_SYMBOL*)((byte*)pData + tttt->PointerToSymbolTable);


        auto pStringTable = (char*)TTTT + tttt->NumberOfSymbols * sizeof(IMAGE_SYMBOL);
        auto cbStringTable = *(DWORD*)pStringTable;

        for (int i = 0; i != tttt->NumberOfSymbols; ++i)
        {
            auto& Symbol = TTTT[i];

            //是一个声明
            if (Symbol.StorageClass == IMAGE_SYM_CLASS_EXTERNAL || Symbol.StorageClass == IMAGE_SYM_CLASS_WEAK_EXTERNAL)
            {
                CStringA _szName;
                //长度明显大于8，肯定是个长命名
                if (Symbol.N.Name.Short == 0)
                {
                    _szName = pStringTable + Symbol.N.Name.Long;
                }
                else
                {
                    _szName = CStringA((char*)Symbol.N.ShortName, strnlen((char*)Symbol.N.ShortName, _countof(Symbol.N.ShortName)));
                }

                // 过滤掉纯声明
                if (Symbol.SectionNumber == 0 && Symbol.StorageClass == IMAGE_SYM_CLASS_EXTERNAL)
                    continue;

                if (_szName.IsEmpty())
                    continue;
                auto _szBuffer = _szName.GetString();
                if (strncmp(_szBuffer, "__imp_", 6) != 0)
                {
                    continue;
                }

                if (_bFallback)
                {
                    _szBuffer += 6;
                    if (_Machine == IMAGE_FILE_MACHINE_I386 && _szBuffer[0] == '_')
                    {
                        ++_szBuffer;
                        if (strncmp(_szBuffer, "YY_Thunks_", 10) != 0)
                        {
                            continue;
                        }

                        // 故意留下一个 '_'
                        _szBuffer += 9;
                        _Cache.insert(CStringA("__imp_") + _szBuffer);
                    }
                    else
                    {
                        if (strncmp(_szBuffer, "YY_Thunks_", 10) != 0)
                        {
                            continue;
                        }

                        _szBuffer += 10;
                        _Cache.insert(CStringA("__imp_") + _szBuffer);
                    }
                }
                else
                {
                    _Cache.insert(_szBuffer);
                    _szBuffer += 6;
                }


                _Cache.insert(_szBuffer);

                if(_Machine == IMAGE_FILE_MACHINE_I386 && *_szBuffer == '_')
                {
                    ++_szBuffer;
                    _Cache.insert(_szBuffer);
                    auto _pEnd = strrchr(_szBuffer, '@');
                    if (_pEnd)
                    {
                        _Cache.insert(CStringA(_szBuffer, _pEnd - _szBuffer));
                    }
                }
            }
        }
    }

    auto _iter = _Cache.find(_ProcInfo.szFriendlyName.GetLength() ? _ProcInfo.szFriendlyName : _ProcInfo.szProc);
    return _iter != _Cache.end();
}

void TryGetFirendlyName(WORD _Machine, CStringA szModuleName, AnalyzerProc& _ProcInfo)
{
    if (_ProcInfo.szFriendlyName.GetLength())
        return;

    if (_ProcInfo.szProc.GetLength() == 0)
        return;

    if(_ProcInfo.szProc[0] != '#')
        return;

    static std::map<CStringA, std::map<DWORD, CStringA>> _FirendlyNameCaches[2];
    int CacheIndex = 0;
    switch (_Machine)
    {
    case IMAGE_FILE_MACHINE_I386:
        CacheIndex = 0;
        break;
    case IMAGE_FILE_MACHINE_AMD64:
        CacheIndex = 1;
        break;
    default:
        return;
    }

    auto& _FirendlyNameCache = _FirendlyNameCaches[CacheIndex];

    if (_FirendlyNameCache.size() == 0)
    {
        static const LPCWSTR s_szSubPath[2] = { L"Config\\x86\\OrdinalMap.txt" , L"Config\\x64\\OrdinalMap.txt" };
        FILE* file = _wfopen(g_szRoot + s_szSubPath[CacheIndex], L"r");
        if (!file)
            return;

        std::map<DWORD, CStringA>* _pOrdinalMap = nullptr;

        char line[2048] = {};
        while (fgets(line, 2048, file))
        {
            if (line[0] == '\0')
                continue;
            auto Count = strlen(line);
            if (Count && line[Count - 1] == '\n')
            {
                --Count;
                line[Count] = '\0';

                if (Count && line[Count - 1] == '\r')
                {
                    --Count;
                    line[Count] = '\0';
                }
            }

            if (line[0] == '\0' || line[0] == ';')
                continue;

            if (line[0] == '[')
            {
                if (line[Count - 1] != ']')
                {
                    continue;
                }

                auto _Key = CStringA(line + 1, Count - 2);
                _Key.MakeLower();
                _pOrdinalMap = &_FirendlyNameCache[_Key];
            }

            if (!_pOrdinalMap)
                continue;

            auto _szName = line;
            auto _Ordinal = strtoul(line, (char**)&_szName, 10);

            if (_szName == line)
            {
                continue;
            }

            if (*_szName == '=')
            {
                ++_szName;

                CStringA _szNameA(_szName);
                _szNameA.Trim();

                (*_pOrdinalMap)[_Ordinal] = _szNameA;
            }
        }

        fclose(file);
    }

    szModuleName.MakeLower();
    auto _iter = _FirendlyNameCache.find(szModuleName);
    if (_iter == _FirendlyNameCache.end())
        return;

    auto _iter2 = _iter->second.find(strtoul(_ProcInfo.szProc.GetString() + 1, nullptr, 10));
    if (_iter2 == _iter->second.end())
        return;

    _ProcInfo.szFriendlyName = _iter2->second;
}

HRESULT BuildAnalyzer(CStringW _szOutputPath, CStringW _szTarget, std::map<WORD, AnalyzerInfo>& _AnalyzerInfos)
{
    UINT16 _TargetVersion[4] = {};
    if (_szTarget.GetLength())
    {
        swscanf_s(_szTarget, L"%hd.%hd.%hd", &_TargetVersion[3], &_TargetVersion[2], &_TargetVersion[1]);
    }

    CStringW szDefBase = g_szRoot;
    szDefBase += L"Config\\";

    CStringA _szData;

    for (auto& _AnalyzerInfo : _AnalyzerInfos)
    {
        auto _szDefRootPath = szDefBase;
        _szData += "# Report By YY.Depends.Analyzer (Target:";
        switch (_AnalyzerInfo.first)
        {
        case IMAGE_FILE_MACHINE_I386:
            _szDefRootPath += L"x86\\";
            if (*(UINT64*)_TargetVersion < MakeVersion(5, 1, 2600, 0))
            {
                _szTarget = L"5.1.2600";
            }
            _szData += _szTarget;
            _szData += '-';
            _szData += "x86";
            break;
        case IMAGE_FILE_MACHINE_AMD64:
            _szDefRootPath += L"x64\\";
            if (*(UINT64*)_TargetVersion < MakeVersion(5, 2, 3790, 0))
            {
                _szTarget = L"5.2.3790";
            }
            _szData += _szTarget;
            _szData += '-';
            _szData += "x64";
            break;
        case IMAGE_FILE_MACHINE_ARM:
            _szDefRootPath += L"arm\\";
            if (*(UINT64*)_TargetVersion < MakeVersion(6, 2, 9200, 0))
            {
                _szTarget = L"6.2.9200";
            }
            _szData += _szTarget;
            _szData += '-';
            _szData += "arm";
            break;
        case IMAGE_FILE_MACHINE_ARM64:
            _szDefRootPath += L"arm64\\";
            if (*(UINT64*)_TargetVersion < MakeVersion(10, 0, 19041, 0))
            {
                _szTarget = L"10.0.19041";
            }
            _szData += _szTarget;
            _szData += '-';
            _szData += "arm64";
            break;
        default:
            _szData.AppendFormat("%d", _AnalyzerInfo.first);
            _szDefRootPath += L"unknow\\";
            break;
        }
        _szData += ')';
        _szData += '\r';
        _szData += '\n';

        auto _szDefPath = _szDefRootPath + _szTarget;
        _szDefPath += ".txt";

        auto _pCache = GetExportModule(_szDefPath, _AnalyzerInfo.first);
        if (!_pCache)
        {
            _szData += "not spuuorted ";
            _szData += _szDefPath;
            _szData += '\r';
            _szData += '\n';

            // wprintf(L"错误：数据库 %s 为空，请确认 Target 是否输入正确。\n", _szDefPath.GetString());
            continue;
        }


        if (g_bCheckBoxView)
        {
            for (auto& _Module : _AnalyzerInfo.second.Modules)
            {
                _Module.second.szModuleName.MakeLower();
                auto _pExportModule = _pCache->GetExportModuleInfo(_Module.second.szModuleName);

                const auto _LengthBackup = _szData.GetLength();
                bool _bHasValue = false;

                _szData += '#';
                _szData += '#';
                _szData += ' ';
                _szData += _Module.second.szModuleName;
                _szData += '\r';
                _szData += '\n';

                for (auto& _Proc : _Module.second.Procs)
                {
                    if (_pExportModule)
                    {
                        if (_pExportModule->FindImport(_Proc.second.szProc))
                            continue;
                    }

                    auto _szIncludeTargets = FindInAllTarget(_Module.second.szModuleName, _Proc.second.szProc, _szDefRootPath, _szTarget, _AnalyzerInfo.first);
                    TryGetFirendlyName(_AnalyzerInfo.first, _Module.second.szModuleName, _Proc.second);

                    const auto _bYYReady = IsInYY_Thunks(_AnalyzerInfo.first, _Proc.second);
                    if (_bYYReady && g_bIgnoreReady)
                        continue;
                    _bHasValue = true;

                    _szData += '*';
                    _szData += ' ';
                    _szData += '[';
                    _szData += _bYYReady ? 'x' : ' ';
                    _szData += ']';
                    _szData += ' ';
                    _szData += _Proc.second.szFriendlyName.GetLength() ? _Proc.second.szFriendlyName : _Proc.second.szProc;
                    _szData += '\r';
                    _szData += '\n';

                    if (!_bYYReady)
                    {
                        auto _iter = _pCache->ProcNameToModuleNameMap.find(_Proc.second.szProc);
                        if (_iter != _pCache->ProcNameToModuleNameMap.end())
                        {
                            _szData += "  - Tips: YY-Thunks not ready, but found in ";
                            _szData += '\"';
                            _szData += _iter->second;
                            _szData += '\"';
                            _szData += '.';
                            _szData += '\r';
                            _szData += '\n';
                        }
                    }

                    if (_szIncludeTargets.GetLength())
                    {
                        _szData += "  - Supported OS: ";
                        _szData += _szIncludeTargets;
                        _szData += '\r';
                        _szData += '\n';
                    }

                    if (_Proc.second.Ref.size() == 1)
                    {
                        _szData += "  - Ref Module: " + _Proc.second.Ref[0];
                        _szData += '\r';
                        _szData += '\n';
                    }
                    else
                    {
                        _szData += "  - Ref Module: \r\n";
                        for (auto& _szRefPath : _Proc.second.Ref)
                        {
                            _szData += ' ';
                            _szData += ' ';
                            _szData += ' ';
                            _szData += ' ';
                            _szData += '-';
                            _szData += ' ';
                            _szData += _szRefPath;
                            _szData += '\r';
                            _szData += '\n';
                        }
                    }
                }

                if (!_bHasValue)
                {
                    _szData.ReleaseBufferSetLength(_LengthBackup);
                    continue;
                }

                _szData += '\r';
                _szData += '\n';
            }
        }
        else
        {
            for (auto& _Module : _AnalyzerInfo.second.Modules)
            {
                auto _pExportModule = _pCache->GetExportModuleInfo(_Module.second.szModuleName);

                const auto _LengthBackup = _szData.GetLength();
                bool _bHasValue = false;

                _szData += '#';
                _szData += '#';
                _szData += ' ';
                _szData += _Module.second.szModuleName;
                _szData += '\r';
                _szData += '\n';

                _szData += "| API                                        | YY-Thunks Ready | Supported OS | Ref Module\r\n";
                _szData += "| ----                                       | --------------  | ------------ | --------\r\n";
                for (auto& _Proc : _Module.second.Procs)
                {
                    if (_pExportModule)
                    {
                        if (_pExportModule->FindImport(_Proc.second.szProc))
                            continue;
                    }

                    CStringA _szIncludeTargets = FindInAllTarget(_Module.second.szModuleName, _Proc.second.szProc, _szDefRootPath, _szTarget, _AnalyzerInfo.first);
                    TryGetFirendlyName(_AnalyzerInfo.first, _Module.second.szModuleName, _Proc.second);

                    const auto _bYYReady = IsInYY_Thunks(_AnalyzerInfo.first, _Proc.second);
                    if (_bYYReady && g_bIgnoreReady)
                        continue;

                    _bHasValue = true;
                    _szData += '|';
                    _szData += ' ';
                    _szData += _Proc.second.szFriendlyName.GetLength() ? _Proc.second.szFriendlyName : _Proc.second.szProc;
                    _szData += ' ';
                    _szData += '|';
                    _szData += ' ';

                    _szData += _bYYReady ? "Yes" : "No";
                    if (!_bYYReady)
                    {
                        auto _iter = _pCache->ProcNameToModuleNameMap.find(_Proc.second.szProc);
                        if (_iter != _pCache->ProcNameToModuleNameMap.end())
                        {
                            _szData += ", but found in \"";
                            _szData += _iter->second;
                            _szData += '\"';
                            _szData += '.';
                        }
                    }
                    _szData += ' ';
                    _szData += '|';
                    _szData += ' ';
                    _szIncludeTargets.Replace(", ", "<br>");
                    _szData += _szIncludeTargets;
                    _szData += ' ';
                    _szData += '|';

                    bool _bFirst = true;
                    for (auto& _szRefPath : _Proc.second.Ref)
                    {
                        if (!_bFirst)
                        {
                            _szData += "<br>";
                        }
                        _bFirst = false;
                        _szData += _szRefPath;
                    }
                    _szData += '\r';
                    _szData += '\n';
                }
                _szData += '\r';
                _szData += '\n';

                if (!_bHasValue)
                {
                    // 没有实际内容输出，所以删除该内容。
                    _szData.ReleaseBufferSetLength(_LengthBackup);
                }
            }
        }
        _szData += '\r';
        _szData += '\n';
    }

    auto _hr = WriteFileData(_szOutputPath, _szData.GetString(), _szData.GetLength());
    if (SUCCEEDED(_hr))
    {
        wprintf(L"信息：成功分析依赖，请查看文件 %s\n", _szOutputPath.GetString());
    }

    return _hr;
}



HRESULT BuildOrdinalMap(CStringW _szInputPath, std::map<CStringA, std::map<DWORD, CStringA>>& _Output)
{
    auto _FileData = ReadFileData(_szInputPath);
    auto pData = _FileData.GetString();
    auto pHeader = (SectionHeader*)(pData + IMAGE_ARCHIVE_START_SIZE);
    auto pHeader2 = pHeader->get_NextHeader();

    auto pSecondSec = (SecondSec*)pHeader2->get_Data();

    auto ObjNum = pSecondSec->get_ObjNum();
    auto ObjOffsets = pSecondSec->get_ObjOffset();
    auto SymbolNum = pSecondSec->get_SymbolNum();
    auto StrTable = pSecondSec->get_StrTable();
    auto SymbolIdx = pSecondSec->get_SymbolIdx();



    auto SymbolName = StrTable;
    for (int i = 0; i != SymbolNum; ++i, SymbolName += strlen(SymbolName) + 1)
    {
        auto ObjHeader = (SectionHeader*)(pData + ObjOffsets[SymbolIdx[i] - 1]);

        auto pObjData = (IMPORT_OBJECT_HEADER*)ObjHeader->get_Data();
        auto cbObjData = ObjHeader->get_Size();

        if (pObjData->Sig1 != IMAGE_FILE_MACHINE_UNKNOWN || pObjData->Sig2 != IMPORT_OBJECT_HDR_SIG2)
        {
            continue;
        }

        if (pObjData->NameType != IMPORT_OBJECT_ORDINAL)
        {
            continue;
        }

        if (strnicmp(SymbolName, "__imp_", 6) != 0)
            continue;

        // 故意跳过 __imp_，因为这是必然携带的信息。
        SymbolName += 6;

        CStringA szDLlName;

        if (ObjHeader->Name[0] == '/')
        {
            auto szProcName = (const char*)pObjData + sizeof(IMPORT_OBJECT_HEADER);
            szDLlName = szProcName + strlen(szProcName) + 1;
        }
        else
        {
            szDLlName.SetString((char*)ObjHeader->Name, _countof(ObjHeader->Name));
            szDLlName.TrimRight();

            szDLlName.ReleaseBuffer();

            if (szDLlName[szDLlName.GetLength() - 1] == '/')
                szDLlName.ReleaseBufferSetLength(szDLlName.GetLength() - 1);
        }

        _Output[szDLlName][pObjData->Ordinal] = SymbolName;
    }

    return S_OK;
}

HRESULT BuildOrdinalMap(CStringW _szInputPath, CStringW _szOutput)
{
    if (_szInputPath.IsEmpty())
        return E_INVALIDARG;

    std::map<CStringA, std::map<DWORD, CStringA>> _Output;

    if (PathIsDirectoryW(_szInputPath))
    {
        if (_szInputPath[_szInputPath.GetLength() - 1] != L'\\')
            _szInputPath += L'\\';

        WIN32_FIND_DATAW _FindFileData;
        auto _hFileFind = FindFirstFileW(_szInputPath + L"*.lib", &_FindFileData);
        if (_hFileFind == INVALID_HANDLE_VALUE)
        {
            return __HRESULT_FROM_WIN32(GetLastError());
        }

        do
        {
            if (_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                continue;
            }

            BuildOrdinalMap(_szInputPath + _FindFileData.cFileName, _Output);

        } while (FindNextFileW(_hFileFind, &_FindFileData));


        FindClose(_hFileFind);
    }
    else
    {
        BuildOrdinalMap(_szInputPath, _Output);
    }
    if (_Output.size() == 0)
        return S_OK;

    CStringA _FileData;
    for (auto& Item : _Output)
    {
        _FileData += '[';
        _FileData += Item.first;
        _FileData += ']';
        _FileData += '\n';

        for (auto& _Proc : Item.second)
        {
            _FileData.AppendFormat("%d=%s\n", _Proc.first, _Proc.second.GetString());
        }
        _FileData += '\n';
    }

    return WriteFileData(_szOutput, _FileData.GetString(), _FileData.GetLength());
}
