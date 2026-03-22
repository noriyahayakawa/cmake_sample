@echo off
setlocal EnableDelayedExpansion

rem VS のインストール先を自動検出して vcvars64.bat を呼ぶ
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "!VSWHERE!" (
	echo [ERROR] vswhere.exe が見つかりません: !VSWHERE!
	exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"

if not defined VSINSTALL (
	echo [ERROR] C++ Build Tools を含む Visual Studio が見つかりません。
	exit /b 1
)

call "!VSINSTALL!\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
	echo [ERROR] vcvars64.bat の実行に失敗しました: !VSINSTALL!\VC\Auxiliary\Build\vcvars64.bat
	exit /b 1
)
 
rem x64 環境（cl/link/lib/SDK）が整った状態で cmake を実行
cmake %*