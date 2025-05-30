#pragma once
#include "KeyLogger.hpp"
#include <windows.h>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <string>
class KeystringActionFuncs{
    private:

    public:
        static void simulateTextInput(const std::string& text);
};