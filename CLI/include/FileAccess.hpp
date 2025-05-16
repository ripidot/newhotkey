#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include "Logger.hpp"
#include "Utils.hpp"
#include <windows.h>
#include <sstream>
#include <iostream>
#include "nlohmann/json.hpp"

class FileAccess { // file access
    private:
        std::string fileurl_;
        std::unordered_map<std::string, HotkeyCommandAction> loaded_hotkeys; //"A ctrl shift", {"launch_app", "notepad.exe"}
        std::unordered_map<std::string, std::string> loaded_remaps; //"Lctrl" , "Lwin"
        std::unordered_map<std::string, WORD> vk_map_;
        std::unordered_map<WORD, std::string> vk_inv_map_;
    public:
        FileAccess();
        FileAccess(std::string fileurl);

        std::unordered_map<std::string, HotkeyCommandAction>* lhotkeys_getter();
        std::unordered_map<std::string, std::string>* lremaps_getter();
        std::unordered_map<std::string, WORD>* vk_map_getter();
        std::unordered_map<WORD, std::string>* vk_inv_map_getter();
        void load_hotkeys_from_file();
        void load_vk_from_file();
};
