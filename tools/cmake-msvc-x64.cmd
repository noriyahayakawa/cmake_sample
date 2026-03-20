@echo off
setlocal

rem VS2022 Community の標準パス（違う場合はここだけ直す）
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul

rem 以降は x64 環境（cl/link/lib/SDK）が整った状態で cmake を実行
"C:\Program Files\CMake\bin\cmake.exe" %*