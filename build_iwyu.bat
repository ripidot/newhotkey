cmake -S . -B build_iwyu -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

@REM cmake -B build_iwyu -S . -G "Ninja" -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
include-what-you-use -p build_iwyu src/KeyLogger.cpp
pause