#include "../include/KeyboardHookManager.hpp"


KeyboardHookManager::KeyboardHookManager(){}
KeyboardHookManager::KeyboardHookManager(const SupMap& supmap){
    this->supmap = supmap;
}

bool KeyboardHookManager::shouldSuppress(WORD vkCode, bool isKeyDown) {
    auto it1 = supmap.suppress_keys.find(vkCode);
    if (it1 != supmap.suppress_keys.end()) {
        return it1->second;
    }
    return false;
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
