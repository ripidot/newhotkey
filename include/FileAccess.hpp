#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include "Logger.hpp"
#include "Utils.hpp"
#include <sstream>
#include <iostream>
#include "nlohmann/json.hpp"

class FileAccess { // file access
    private:
        PATH fileurl_;
        std::unordered_map<std::string, HotkeyCommandAction> loaded_hotkeys; //"A ctrl shift", {"launch_app", "notepad.exe"}
        std::unordered_map<std::string, std::string> loaded_remaps; //"Lctrl" , "Lwin"
        std::unordered_map<std::string, std::string> loaded_keystrings;
        std::unordered_map<std::string, WORD> vk_map;
        std::unordered_map<WORD, std::string> vk_inv_map;
    public:
        FileAccess();
        FileAccess(PATH fileurl);

        std::unordered_map<std::string, HotkeyCommandAction>* lhotkeys_getter();
        std::unordered_map<std::string, std::string>* lremaps_getter();
        std::unordered_map<std::string, std::string>* lkstrings_getter();
        std::unordered_map<std::string, WORD>* vk_map_getter();
        std::unordered_map<WORD, std::string>* vk_inv_map_getter();
        void set_filename(const PATH str);
        void load_hotkeys_from_file();
        void load_vk_from_file();
        int load_launchCounter();
        void increment_launchCounter();
};
