@echo off

set LIB_PATH="..\resources\raylib\lib"
set INCLUDE_PATH="..\resources\raylib\include"
set LIBS=""

pushd ..\build
cl -Zi "..\src\main.c" /I%INCLUDE_PATH% /link /libpath:%LIB_PATH% ^
	gdi32.lib msvcrt.lib raylib.lib winmm.lib user32.lib shell32.lib ^
	/DEBUG /NODEFAULTLIB:libcmt /NODEFAULTLIB:msvcrtd
popd
