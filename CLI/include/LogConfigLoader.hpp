#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include "FileAccess.hpp"

class LogConfigLoader {
    private:
        const PATH filepath;
        static int lCounter;
    public:
        LogConfigLoader(PATH filepath);
        LogConfigLoader();
        int getlCounter();
        void load();
};