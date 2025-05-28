// KeyLogger.hpp
#pragma once
#include <boost/locale.hpp>
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
        PATH errorfilepath;
        PATH dbpath;
    public:
        KeyLogger(){}
        KeyLogger(int lCounter):launchCounter(lCounter){
        }
        static inline std::string keyBuffer;
        void setDBFilename(PATH initpath);
        void setErrorfilename(PATH path);
        void memory(const KeyLog& keylog);
        void setRand();
        void setLaunchCounter(int lcounter);
        std::string KeyLogger::return_Modifier_from_Hotkey(const Hotkey& current);
        void SaveErrorToFile(PATH errorfilepath, std::string err);

};