#pragma once
#include "KeyLogger.hpp"
#include <unordered_map>
#include <chrono>
#include <thread>
class KeystringActionFuncs{
    private:

    public:
        KeystringActionFuncs(){}
        void simulateTextInput(const std::string& text);
        void keystring(WORD vk_code);
};