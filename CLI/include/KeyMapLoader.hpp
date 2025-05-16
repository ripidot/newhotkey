// KeyMapLoader.h
#pragma once
#include <Windows.h>
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
        const std::string fileurl;
    public:
        KeyMapLoader(std::string fileurl);
        KeyMapLoader();

        void load(std::string vkfilename);
        WORD key_string_to_vk(const std::string& key_name);
        std::string vk_to_key_string(const WORD vk);
};