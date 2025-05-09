#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include "Types.hpp"
#include "Logger.hpp"
#include "../include/Utils.hpp"
#include <windows.h>
#include <sstream>
#include <iostream>

class FileAccess { // file access
    private:
        const std::string fileurl_;
        std::unordered_map<std::string, HotkeyCommandAction> loaded_hotkeys; //"A ctrl shift", {"launch_app", "notepad.exe"}
        std::unordered_map<std::string, std::string> loaded_remaps; //"Lctrl" , "Lwin"
    public:
        FileAccess();
        FileAccess(std::string fileurl);

        std::unordered_map<std::string, HotkeyCommandAction>* lhotkeys_getter();
        std::unordered_map<std::string, std::string>* lremaps_getter();
        void load_hotkeys_from_file();
};
