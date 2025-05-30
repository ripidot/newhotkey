#include "../include/HotkeyAndRemapMapLoader.hpp"

#define HOTKEY_ID 1

int main(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    try{
        PathManager pm(std::filesystem::current_path());
        pm.ensureDirectoriesExist();

        HotkeyAndRemapMapLoader maplod = HotkeyAndRemapMapLoader(pm);
        maplod.load();
        maplod.run_all_tests();

        KeyboardHookManager hook(maplod.getSupMap());

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
    }catch(const std::exception& e) {
        debug_log(LogLevel::Error, e.what());
        return 1;
    }
    return 0;
}