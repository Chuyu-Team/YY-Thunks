@echo off
setlocal

::需要设置LibMaker.exe路径到环境变量Path。
::如果你需要LibMaker.exe，请自行到 初雨开源项目交流QQ群（633710173）群文件中下载。

pushd "%~dp0.."

set "include=%~dp0Thunks;%~dp0Shared;%~dp0;%include%"

if "%Platform%"=="" set Platform=x86

md "objs\\%Platform%"

::先生成 YY_Thunks_List.hpp
msbuild "%~dp0YY-Thunks.UnitTest\YY-Thunks.UnitTest.vcxproj" -t:Build_YY_Thunks_List_hpp

call:Build%Platform%


popd

goto:eof

:: BuildObj YY_Thunks_for_Vista.obj NTDDI_WIN6 1.def+2.def
:BuildObj
echo BuildObj %1 %2 %3

cl /O1 /Os /Oi /GS- /std:c++17 /execution-charset:utf-8 /Zc:sizedDealloc- /Zc:tlsGuards- /Zc:alignedNew- /arch:IA32 /Z7 /MT /Fo"objs\\%Platform%\\%1" /Zl /c /D "NDEBUG" /D "YY_Thunks_Target=%2" "%~dp0Thunks\YY_Thunks.cpp"
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

::生成weak符号，一些非必须符号安排为weak可以避免链接失败
call:FixObj "%~dp0..\\objs\\%Platform%\\%1" %3

goto:eof

:: BuildObj 6.0.6000.0 NTDDI_WIN6 1.def+2.def
:BuildLib
echo BuildLib %1 %2 %3

md "Lib\\%1\\%Platform%"

cl /O1 /Os /Oi /GS- /std:c++17 /execution-charset:utf-8 /Zc:sizedDealloc- /Zc:tlsGuards- /Zc:alignedNew- /arch:IA32 /Z7 /MT /Fo"Lib\\%1\\%Platform%\\YY_Thunks_for_%1.obj" /Zl /c /D "__APPLY_CHROMIUM_WORKAROUNDS" /D "__USING_NTDLL_LIB" /D "NDEBUG" /D "YY_Thunks_Target=%2" /D "__FALLBACK_PREFIX=YY_Thunks_" "%~dp0Thunks\YY_Thunks.cpp"

if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

::生成weak符号，一些非必须符号安排为weak可以避免链接失败
call:FixObj "%~dp0..\\Lib\\%1\\%Platform%\\YY_Thunks_for_%1.obj" %3

set "SupportApiSet=/SupportApiSet"
set "WinVersion=%1"

:: Win8以前不支持API Set
if "%WinVersion:~0,2%"=="5." (set "SupportApiSet=")
if "%WinVersion:~0,3%"=="6.0" (set "SupportApiSet=")
if "%WinVersion:~0,3%"=="6.1" (set "SupportApiSet=")

LibMaker.exe %SupportApiSet% /PREFIX:YY_Thunks_ BuildYY_ThunksLibraries "%~dp0..\\Lib\\%1\\%Platform%\\YY_Thunks_for_%1.obj" "%WindowsSdkDir%\\Lib\\%WindowsSDKLibVersion%\\um\\%Platform%" "%~dp0..\\Lib\\%1\\%Platform%"
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%
goto:eof

:: 6.0.6000.0 YY_Thunks_for_Vista.obj NTDDI_WIN6 1.def+2.def
:BuildX
call:BuildObj %2 %3 %4
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%
call:BuildLib %1 %3 %4
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%
goto:eof

:Buildx86
set PointType=4
call:BuildX 5.0.2195.0 YY_Thunks_for_Win2K.obj __WindowsNT5 PSAPI2Kernel32.def+esent.def
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 5.1.2600.0 YY_Thunks_for_WinXP.obj __WindowsNT5_1 PSAPI2Kernel32.def+esent.def
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 6.0.6000.0 YY_Thunks_for_Vista.obj __WindowsNT6 PSAPI2Kernel32.def
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 6.1.7600.0 YY_Thunks_for_Win7.obj __WindowsNT6_1
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 6.2.9200.0 YY_Thunks_for_Win8.obj __WindowsNT6_2
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 10.0.10240.0 YY_Thunks_for_Win10.0.10240.obj __WindowsNT10_10240
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 10.0.19041.0 YY_Thunks_for_Win10.0.19041.obj __WindowsNT10_19041
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

goto:eof


:Buildx64
set PointType=8
call:BuildX 5.2.3790.1830 YY_Thunks_for_WinXP.obj __WindowsNT5_2_SP1 PSAPI2Kernel32.def+esent.def
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 6.0.6000.0 YY_Thunks_for_Vista.obj __WindowsNT6 PSAPI2Kernel32.def
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 6.1.7600.0 YY_Thunks_for_Win7.obj __WindowsNT6_1
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 6.2.9200.0 YY_Thunks_for_Win8.obj __WindowsNT6_2
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 10.0.10240.0 YY_Thunks_for_Win10.0.10240.obj __WindowsNT10_10240
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 10.0.19041.0 YY_Thunks_for_Win10.0.19041.obj __WindowsNT10_19041
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

goto:eof

:Buildarm
set PointType=4
call:BuildX 6.2.9200.0 YY_Thunks_for_Win8.obj __WindowsNT6_2
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 10.0.10240.0 YY_Thunks_for_Win10.0.10240.obj __WindowsNT10_10240
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 10.0.19041.0 YY_Thunks_for_Win10.0.19041.obj __WindowsNT10_19041
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

goto:eof

:Buildarm64
set PointType=8
call:BuildX 10.0.10240.0 YY_Thunks_for_Win10.0.10240.obj __WindowsNT10_16299
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

call:BuildX 10.0.19041.0 YY_Thunks_for_Win10.0.19041.obj __WindowsNT10_19041
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%

goto:eof

:: FixObj "XXX\YY_Thunks_for_Vista.obj" 1.def+2.def
:FixObj
LibMaker.exe FixObj %1 /WeakExternFix:__security_cookie=%PointType% /WeakExternFix:__acrt_atexit_table=%PointType% /WeakExternFix:__pfnDllMainCRTStartupForYY_Thunks=%PointType% /WeakExternFix:__YY_Thunks_Disable_Rreload_Dlls=4 /WeakExternFix:__pfnYY_Thunks_CustomLoadLibrary=%PointType%
if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%
if "%2"=="" goto:eof
set DEF_FILES=%2
:AppendWeak
for /f "tokens=1* delims=+" %%a in ("%DEF_FILES%") do (
    echo "AppendWeak %~dp0def\\%Platform%\\%%a" %1
    LibMaker.exe AppendWeak /MACHINE:%Platform% /DEF:"%~dp0def\\%Platform%\\%%a" /OUT:%1
    if %ErrorLevel% NEQ 0 exit /b %ErrorLevel%
    set DEF_FILES=%%b
)

if defined DEF_FILES goto :AppendWeak
goto:eof
