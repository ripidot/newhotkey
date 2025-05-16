#include "../include/HotkeyAndRemapMapLoader.hpp"

#define HOTKEY_ID 1

int main(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    const std::string hotkeyfileurl = "data/hotkeys.txt";
    const std::string vkfileurl = "data/vkmap_grouped_numeric.json";
    HotkeyAndRemapMapLoader maplod = HotkeyAndRemapMapLoader(hotkeyfileurl, vkfileurl);
    maplod.load();
    maplod.run_all_tests();

    KeyboardHookManager hook(maplod.skeys_getter());

    hook.setKeyDownHandler([&maplod](int vk) {
        maplod.execute(vk, true);
    });
    hook.setKeyUpHandler([&maplod](int vk) {
        maplod.execute(vk, false);
    });
    hook.setHook();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        switch (msg.message) {
        case WM_HOTKEY:
            if (msg.wParam == HOTKEY_ID) {
                debug_log(LogLevel::Info, "hotkey is pushed");
            }
            break;
        }
    }
    return 0;
}