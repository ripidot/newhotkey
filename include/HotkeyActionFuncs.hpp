#pragma once
#include <string>
#include <windows.h>
#include "Logger.hpp"
// string用のsendinput関数
void SendStringInput(const std::string& str);
namespace HotkeyActionFuncs { // hotkeyに対してのアクションの関数
    void volume_up();
    void volume_down();
    void input_key(const std::string& str);
    bool launch_app(const std::string& path);
    bool open_url(const std::string& url);
}