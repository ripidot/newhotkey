#pragma once
#include "KeyLogger.hpp"
#include <unordered_map>
#include <chrono>
#include <thread>
class HotkeyAndRemapMapLoader{
    private:
        static inline std::wstring inputBuffer;
        
        std::unordered_map<std::wstring, std::wstring> hotstrings = {
            {L"brb", L"be right back"},
            {L"omw", L"on my way"},
            {L"ty", L"thank you"},
            {L"idk", L"I don't know"},
        };
    public:
        void HotkeyAndRemapMapLoader::simulateTextInput(const std::wstring& text) {
            for (wchar_t c : text) {
                INPUT input[2] = {};
                input[0].type = INPUT_KEYBOARD;
                input[0].ki.wScan = c;
                input[0].ki.dwFlags = KEYEVENTF_UNICODE;

                input[1] = input[0];
                input[1].ki.dwFlags |= KEYEVENTF_KEYUP;

                SendInput(2, input, sizeof(INPUT));
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        void keystring(int vk_code){
        // 英数字のみ
            if (('A' <= vk_code && vk_code <= 'Z')) vk_code += 32;
            if (('a' <= vk_code && vk_code <= 'z') || 
            (vk_code >= '0' && vk_code <= '9')) {
                char c = (char)vk_code;
                HotkeyAndRemapMapLoader::inputBuffer += c;
                debug_log(LogLevel::Info, inputBuffer.size());
                std::wcout << inputBuffer << std::endl;
                for (const auto& [trigger, replacement] : hotstrings){

                    if (inputBuffer.size() >= trigger.size() &&
                        inputBuffer.substr(inputBuffer.size() - trigger.size()) == trigger) {

                        // バックスペース送信
                        for (size_t i = 0; i < trigger.size()-1; ++i) {
                            keybd_event(VK_BACK, 0, 0, 0);
                            keybd_event(VK_BACK, 0, KEYEVENTF_KEYUP, 0);
                            std::cout << "erase" << std::endl;
                        }
                        debug_log(LogLevel::Info, "sent keystring");

                        // 置換文字列送信
                        simulateTextInput(replacement);
                        inputBuffer.clear();

                        // return to not input last word of hotstring 
                        return;
                    }
                }
            }
        }
};