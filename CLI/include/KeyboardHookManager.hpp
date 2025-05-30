#pragma once
#include <windows.h>
#include <functional>
#include <atomic>
#include "Logger.hpp"

class KeyboardHookManager {
    private:
        static inline HHOOK hHook = nullptr;
        static inline std::function<void(int)> keyDownHandler = nullptr;
        static inline std::function<void(int)> keyUpHandler = nullptr;
        static inline std::wstring inputBuffer = L"";
        static inline SupMap supmap;

    public:
        KeyboardHookManager();
        KeyboardHookManager(const SupMap& supmap);

        static inline std::atomic<bool> suppress_input = false;
        static bool shouldSuppress(WORD vkCode, bool isKeyDown);

        static inline std::unordered_map<std::wstring, std::wstring> hotstrings = {
            {L"brbrb", L"be right back"},
            {L"omw", L"on my way"},
            {L"ty", L"thank you"},
            {L"idk", L"I don't know"},
        };
        static void simulateTextInput(const std::wstring& text);

        static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
        void setKeyDownHandler(std::function<void(int)> handler);
        void setKeyUpHandler(std::function<void(int)> handler);
        void setHook();
        void removeHook();
};