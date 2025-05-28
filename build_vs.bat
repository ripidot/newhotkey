@echo off
cd /d %~dp0

mkdir build_vs
cd build_vs

@REM vcpkg の toolchainを指定
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:\dev\vcpkg/scripts/buildsystems/vcpkg.cmake"

@REM ビルド
cmake --build .

cd Debug
.\hotkey_app.exe
pause
