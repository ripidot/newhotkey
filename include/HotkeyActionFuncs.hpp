#pragma once
#include <string>
#include <windows.h>
#include "Logger.hpp"
// string用のsendinput関数
class HotkeyActionFuncs{
    private:

    public:
        static void SendStringInput(const std::string& str);
        static void volume_up();
        static void volume_down();
        static void input_key(const std::string& str);
        static bool launch_app(const std::string& path);
        static bool open_url(const std::string& url);
};