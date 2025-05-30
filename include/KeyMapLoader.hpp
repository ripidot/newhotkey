// KeyMapLoader.h
#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include "FileAccess.hpp"

class KeyMapLoader {
    private:
        FileAccess fileaccess;
        static VKMap vkmap;
        const PATH fileurl;
    public:
        KeyMapLoader(PATH fileurl);
        KeyMapLoader();

        void load(PATH vkfilename);
        WORD key_string_to_vk(const std::string& key_name);
        std::string vk_to_key_string(const WORD vk);
        const VKMap KeyMapLoader::getvkmap();
};