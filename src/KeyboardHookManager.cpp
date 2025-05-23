﻿#include "../include/KeyboardHookManager.hpp"


KeyboardHookManager::KeyboardHookManager(){}
KeyboardHookManager::KeyboardHookManager(std::unordered_map<WORD, bool>* skeys){
    suppress_keys = *skeys;
}

bool KeyboardHookManager::shouldSuppress(WORD vkCode, bool isKeyDown) {
    auto it1 = suppress_keys.find(vkCode);
    if (it1 != suppress_keys.end()) {
        return it1->second;
    }
    return false;
}
void KeyboardHookManager::simulateTextInput(const std::wstring& text) {
    for (wchar_t c : text) {
        INPUT input[2] = {};
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wScan = c;
        input[0].ki.dwFlags = KEYEVENTF_UNICODE;

        input[1] = input[0];
        input[1].ki.dwFlags |= KEYEVENTF_KEYUP;

        SendInput(2, input, sizeof(INPUT));
    }
}
LRESULT CALLBACK KeyboardHookManager::HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && !suppress_input) {
        const KBDLLHOOKSTRUCT* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        int vkCode = p->vkCode;
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (isKeyDown && keyDownHandler) {
            keyDownHandler(vkCode); // keydownのアクションが定義されていたら



        }
        if (isKeyUp && keyUpHandler) keyUpHandler(vkCode);

        if (shouldSuppress(vkCode, isKeyDown)) return 1;
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
void KeyboardHookManager::setSuppressKeys(std::unordered_map<WORD, bool>* skeys){
    debug_log(LogLevel::Warning, *skeys);
    suppress_keys = move(*skeys);
}
void KeyboardHookManager::setKeyDownHandler(std::function<void(int)> handler) {
    keyDownHandler = std::move(handler);
}
void KeyboardHookManager::setKeyUpHandler(std::function<void(int)> handler) {
    keyUpHandler = std::move(handler);
}
void KeyboardHookManager::setHook() {
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, NULL, 0);
}
void KeyboardHookManager::removeHook() {
    if (hHook) UnhookWindowsHookEx(hHook);
    hHook = nullptr;
}
