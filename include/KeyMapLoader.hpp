// KeyMapLoader.h
#pragma once
#include <Windows.h>
#include <unordered_map>
#include <string>


class KeyMapLoader {
    private:
        static const std::unordered_map<std::string, WORD> vk_map_;
        const std::string fileurl_;
    public:
        KeyMapLoader(std::string fileurl);
        KeyMapLoader();

        WORD key_string_to_vk(const std::string& key_name);
};