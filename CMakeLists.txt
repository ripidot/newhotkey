# CMakeLists.txt (ルート)
cmake_minimum_required(VERSION 3.10)
project(newhotkey)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ヘッダーファイルの場所をインクルードパスに追加
include_directories(include)

#出力ディレクトリを指定
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)


# src ディレクトリ内のすべての .cpp ファイルを対象に
file(GLOB SOURCES "src/*.cpp")
# list(FILTER SOURCES EXCLUDE REGEX ".*RemapActionFunc.cpp$")

# 実行ファイルの作成 win32指定
add_executable(hotkey_app ${SOURCES})

add_custom_command(TARGET hotkey_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:hotkey_app>/data
)
