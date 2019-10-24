@echo off
setlocal

::需要设置LibMaker.exe路径到环境变量Path。
::如果你需要LibMaker.exe，请自行到 初雨开源项目交流QQ群（633710173）群文件中下载。

pushd "%~dp0.."

if "%Platform%"=="" set Platform=x86

md "objs\\%Platform%"

call:Build%Platform%


popd

goto:eof

:: BuildObj YY_Thunks_for_Vista.obj NTDDI_WIN6
:BuildObj
cl /O1 /Os /Oi /GS- /arch:IA32 /Z7 /MT /Fo"objs\\%Platform%\\%1" /Zl /c /D "NDEBUG" /D "YY_Thunks_Support_Version=%2" "%~dp0YY_Thunks.cpp"

::进行函数名称进行修正 __imp__%s_%u -> __imp__%s@%u
LibMaker.exe FixObj "%~dp0..\\objs\\%Platform%\\%1"

LibMaker.exe AppendWeak /MACHINE:%Platform% /DEF:"%~dp0def\\%Platform%\\PSAPI2Kernel32.def" /OUT:"%~dp0..\\objs\\%Platform%\\%1"

goto:eof

:Buildx86
call:BuildObj YY_Thunks_for_WinXP.obj NTDDI_WINXP
call:BuildObj YY_Thunks_for_Vista.obj NTDDI_WIN6


goto:eof


:Buildx64
call:BuildObj YY_Thunks_for_WinXP.obj NTDDI_WS03SP1
call:BuildObj YY_Thunks_for_Vista.obj NTDDI_WIN6
goto:eof

:Buildarm
call:BuildObj YY_Thunks_for_Win8.obj NTDDI_WIN8
goto:eof

:Buildarm64
call:BuildObj YY_Thunks_for_Win10_RS3.obj NTDDI_WIN10_RS3
goto:eof