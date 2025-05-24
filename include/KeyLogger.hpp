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
#include <random>

class KeyLogger {
    private:
        int launchCounter; // 起動回数
        static inline int sequence_counter = 0; // 同launchCounterでの入力回数
        int rand; // logのための乱数生成
        int rmin = 1000;
        int rmax = 9999;
        int flushBufferSize = 10;
        const char* appdata = std::getenv("APPDATA");  // AppData\Roaming
        std::filesystem::path logfilepath = std::filesystem::path(appdata) / "MyApp" / "log" / "logfile.txt";
        PATH dbpath;
    public:
        KeyLogger(){}
        KeyLogger(int lCounter):launchCounter(lCounter){
        }
        static inline std::string keyBuffer;
        void setDBFilename(PATH initpath);
        void onKeyPress(std::string str);
        void memory(KeyLog* keylog);
        void flushBufferToFile();
        void setRand();
        void setLaunchCounter(int lcounter);
        std::string KeyLogger::return_Modifier_from_Hotkey(const Hotkey& current); 
};