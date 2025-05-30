#include "../include/HotkeyActionFuncs.hpp"

// string用のsendinput関数
void HotkeyActionFuncs::SendStringInput(const std::string& str){
    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
    }
}
void HotkeyActionFuncs::volume_up() {
    keybd_event(VK_VOLUME_UP, 0, 0, 0);   // キー押し
    keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_KEYUP, 0); // キー離し
}
void HotkeyActionFuncs::volume_down() {
    keybd_event(VK_VOLUME_DOWN, 0, 0, 0); 
    keybd_event(VK_VOLUME_DOWN, 0, KEYEVENTF_KEYUP, 0);
}
void HotkeyActionFuncs::input_key(const std::string& vk) {
    SendStringInput(vk);
}
bool HotkeyActionFuncs::launch_app(const std::string& path) {
    std::string cmd = "start \"\" \"" + path + "\"";
    int result = system(cmd.c_str());
    return result == 0;
}
bool HotkeyActionFuncs::open_url(const std::string& url) {
    std::string cmd = "start " + url;
    int result = system(cmd.c_str());
    return result == 0;
}
