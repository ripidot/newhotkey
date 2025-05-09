#include "../include/ActionFuncs.hpp"
// string用のsendinput関数
void SendStringInput(const std::string& str){
    // str.find()
}
namespace ActionFuncs { // hotkeyに対してのアクションの関数
    void volume_up() {
        keybd_event(VK_VOLUME_UP, 0, 0, 0);   // キー押し
        keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_KEYUP, 0); // キー離し
    }
    void volume_down() {
        keybd_event(VK_VOLUME_DOWN, 0, 0, 0); 
        keybd_event(VK_VOLUME_DOWN, 0, KEYEVENTF_KEYUP, 0);
    }
    void input_key(const std::string& str) {
        // std::string cmd = "start \"\" \"" + str + "\"";
        SendStringInput(str);
        // int result = system(cmd.c_str());
    }
    bool launch_app(const std::string& path) {
        std::string cmd = "start \"\" \"" + path + "\"";
        int result = system(cmd.c_str());
        return result == 0;
    }
    bool open_url(const std::string& url) {
        std::string cmd = "start " + url;
        int result = system(cmd.c_str());
        return result == 0;
    }
}