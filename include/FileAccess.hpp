#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include "Types.hpp"

class FileAccess { // file access
    private:
        const std::string fileurl_;
        std::unordered_map<std::string, HotkeyCommandAction> loaded_hotkeys; //"A ctrl shift", {"launch_app", "notepad.exe"}
        std::unordered_map<std::string, std::string> loaded_remaps; //"Lctrl" , "Lwin"
    public:
        FileAccess();
        FileAccess(std::string fileurl);

        auto lhotkeys_getter();
        auto lremaps_getter();
        void load_hotkeys_from_file();
};
