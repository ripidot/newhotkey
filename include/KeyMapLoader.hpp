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
        static std::unordered_map<std::string, WORD> vk_map;
        static std::unordered_map<WORD, std::string> vk_inv_map;
        const PATH fileurl;
    public:
        KeyMapLoader(PATH fileurl);
        KeyMapLoader();

        void load(PATH vkfilename);
        WORD key_string_to_vk(const std::string& key_name);
        std::string vk_to_key_string(const WORD vk);
};