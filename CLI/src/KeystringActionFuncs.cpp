#include "../include/KeystringActionFuncs.hpp"

void KeystringActionFuncs::simulateTextInput(const std::wstring& text) {
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