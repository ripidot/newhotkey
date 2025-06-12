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
        static int lid;
    public:
        LogConfigLoader(PATH filepath);
        LogConfigLoader();
        int getlCounter();
        int getlId();
        void load();
};