#include "../include/HotkeyAndRemapMapLoader.hpp"

#define HOTKEY_ID 1

int main(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    const PATH hotkeyfileurl = "data/hotkeys.txt";
    const PATH vkfileurl = "data/vkmap_grouped_numeric.json";
    const char* appdata = std::getenv("APPDATA");  // AppData\Roaming
    const PATH initlogfileurl = std::filesystem::path(appdata) / "MyApp" / "log" / "initlog.txt";
    const PATH dbfileurl = std::filesystem::path(appdata) / "MyApp" / "log" / "keylog.db";
    HotkeyAndRemapMapLoader maplod = HotkeyAndRemapMapLoader(hotkeyfileurl, vkfileurl, initlogfileurl, dbfileurl);
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
    //フレームのようなもの, メッセージループが終わるとアプリ側での処理が実行される
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