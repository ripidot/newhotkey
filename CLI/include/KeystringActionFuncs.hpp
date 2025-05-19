#pragma once
#include "KeyLogger.hpp"
#include <unordered_map>
#include <chrono>
#include <thread>
class KeystringActionFuncs{
    private:
        static inline std::wstring inputBuffer;

    public:
        KeystringActionFuncs(){}
        void simulateTextInput(const std::wstring& text);
        void keystring(WORD vk_code);
};