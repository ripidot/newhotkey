// KeyMapLoader.h
#pragma once
#include <Windows.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include "nlohmann/json.hpp"
#include "Logger.hpp"
#include <fstream>
// #include "FileAccess.hpp"

class KeyMapLoader {
    private:
        // FileAccess fileaccess = FileAccess(fileurl_);
        static std::unordered_map<std::string, WORD> vk_map_;
        static std::unordered_map<WORD, std::string> vk_inv_map_;
        static const std::unordered_map<WORD, std::string> string_map_;
        const std::string fileurl_;
    public:
        KeyMapLoader(std::string fileurl);
        KeyMapLoader();

        void test();
        void load();
        WORD key_string_to_vk(const std::string& key_name);
        std::string vk_to_key_string(const WORD vk);
        std::string returnA();
};