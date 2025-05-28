#include "../include/RemapActionFuncs.hpp"


namespace RemapActionFuncs { // hotkeyに対してのアクションの関数
    // remapの実行関数

    void SendKeyboardInput(WORD key, bool keyDown) { // key:vk_key
        KeyboardHookManager::suppress_input = true;
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = key;
        input.ki.dwFlags = keyDown ? 0 : KEYEVENTF_KEYUP;

        SendInput(1, &input, sizeof(INPUT));

        KeyboardHookManager::suppress_input = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

}