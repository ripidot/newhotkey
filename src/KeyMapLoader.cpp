#include "../include/KeyMapLoader.hpp"

using json = nlohmann::json;

VKMap KeyMapLoader::vkmap;

KeyMapLoader::KeyMapLoader(PATH fileurl) : fileurl(fileurl) {}
KeyMapLoader::KeyMapLoader(){}

void KeyMapLoader::load(PATH vkfilename){
    fileaccess = FileAccess(vkfilename);
    fileaccess.load_vk_from_file();
    vkmap.vk_map = *fileaccess.vk_map_getter();
    vkmap.vk_inv_map = *fileaccess.vk_inv_map_getter();
}

// char c を取り出し、asciiコード表を基にvkに変換
WORD KeyMapLoader::key_string_to_vk(const std::string& key_name) { 

    // "VK_"で始まる名前ならテーブル検索
    auto it = vkmap.vk_map.find(key_name);
    if (it != vkmap.vk_map.end()) {
        return it->second;
    }

    // アルファベット単文字（大文字小文字問わず）
    if (key_name.size() == 1) {
        char c = key_name[0];
        if ('a' <= c && c <= 'z') c -= 32; // 小文字→大文字変換, char型1byteなので32
        if ('A' <= c && c <= 'Z') return static_cast<WORD>(c); // 65--90
        if ('0' <= c && c <= '9') return static_cast<WORD>(c); // 48--57
    }
    // それ以外は失敗
    return 0;
}
std::string KeyMapLoader::vk_to_key_string(WORD vk){
    char c;
    if ((65 <= vk) && (vk <= 90)) { // アルファベット
        c = (char)vk;
        std::string str = std::string(1,c);
        return str;
    }
    if ((48 <= vk) && (vk <= 57)) { // 数字
        c = (char)vk;
        std::string str = std::string(1,c);
        return str;
    }
    // vkのテーブル検索
    auto it = vkmap.vk_inv_map.find(vk);
    if (it != vkmap.vk_inv_map.end()) {
        return it->second;
    }
    return "";
}
const VKMap KeyMapLoader::getvkmap() {
    return vkmap;
}