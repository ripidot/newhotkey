#include "../include/KeyMapLoader.hpp"

const std::unordered_map<std::string, WORD> KeyMapLoader::vk_map_ = {
    {"VK_BACK", VK_BACK},
    {"VK_TAB", VK_TAB},
    {"VK_RETURN", VK_RETURN},
    {"VK_SHIFT", VK_SHIFT},
    {"VK_CONTROL", VK_CONTROL},
    {"VK_MENU", VK_MENU}, // Alt
    {"VK_PAUSE", VK_PAUSE},
    {"VK_CAPITAL", VK_CAPITAL}, // Caps Lock
    {"VK_ESCAPE", VK_ESCAPE},
    {"VK_SPACE", VK_SPACE},
    {"VK_PRIOR", VK_PRIOR}, // Page Up
    {"VK_NEXT", VK_NEXT},   // Page Down
    {"VK_END", VK_END},
    {"VK_HOME", VK_HOME},
    {"VK_LEFT", VK_LEFT},
    {"VK_UP", VK_UP},
    {"VK_RIGHT", VK_RIGHT},
    {"VK_DOWN", VK_DOWN},
    {"VK_INSERT", VK_INSERT},
    {"VK_DELETE", VK_DELETE},
    {"VK_LWIN", VK_LWIN},
    {"VK_RWIN", VK_RWIN},
    {"VK_APPS", VK_APPS},
    {"VK_NUMPAD0", VK_NUMPAD0},
    {"VK_NUMPAD1", VK_NUMPAD1},
    {"VK_NUMPAD2", VK_NUMPAD2},
    {"VK_NUMPAD3", VK_NUMPAD3},
    {"VK_NUMPAD4", VK_NUMPAD4},
    {"VK_NUMPAD5", VK_NUMPAD5},
    {"VK_NUMPAD6", VK_NUMPAD6},
    {"VK_NUMPAD7", VK_NUMPAD7},
    {"VK_NUMPAD8", VK_NUMPAD8},
    {"VK_NUMPAD9", VK_NUMPAD9},
    {"VK_MULTIPLY", VK_MULTIPLY},
    {"VK_ADD", VK_ADD},
    {"VK_SUBTRACT", VK_SUBTRACT},
    {"VK_DECIMAL", VK_DECIMAL},
    {"VK_DIVIDE", VK_DIVIDE},
    {"VK_F1", VK_F1},
    {"VK_F2", VK_F2},
    {"VK_F3", VK_F3},
    {"VK_F4", VK_F4},
    {"VK_F5", VK_F5},
    {"VK_F6", VK_F6},
    {"VK_F7", VK_F7},
    {"VK_F8", VK_F8},
    {"VK_F9", VK_F9},
    {"VK_F10", VK_F10},
    {"VK_F11", VK_F11},
    {"VK_F12", VK_F12},
    {"VK_LSHIFT", VK_LSHIFT},
    {"VK_RSHIFT", VK_RSHIFT},
    {"VK_LCONTROL", VK_LCONTROL},
    {"VK_RCONTROL", VK_RCONTROL},
    {"VK_NONCONVERT", VK_NONCONVERT},
    {"VK_VOLUME_DOWN", VK_VOLUME_DOWN},
    {"VK_VOLUME_UP", VK_VOLUME_UP},
    {"VK_MEDIA_PLAY_PAUSE", VK_MEDIA_PLAY_PAUSE},
    {"VK_ZENHANKAKU", 243},
    {"VK_MINUS", VK_OEM_MINUS}, // minus
    {"VK_CARET", VK_OEM_7},
    {"VK_YEN", VK_OEM_5},
    {"VK_AT", VK_OEM_3},
    {"VK_LBRAKET", VK_OEM_4},
    {"VK_SEMICOLON", VK_OEM_PLUS},
    {"VK_COLON", VK_OEM_1},
    {"VK_RBRAKET", VK_OEM_6},
    {"VK_COMMA", VK_OEM_COMMA},
    {"VK_PERIOD", VK_OEM_PERIOD},
    {"VK_SRASH", VK_OEM_2},
    {"VK_BACKSRASH", VK_OEM_102}
};

KeyMapLoader::KeyMapLoader(std::string fileurl) : fileurl_(fileurl) {}
KeyMapLoader::KeyMapLoader() : fileurl_() {}

// char c を取り出し、asciiコード表を基にvkに変換
WORD KeyMapLoader::key_string_to_vk(const std::string& key_name) { 

    // "VK_"で始まる名前ならテーブル検索
    auto it = vk_map_.find(key_name);
    if (it != vk_map_.end()) {
        return it->second;
    }

    // アルファベット単文字（大文字小文字問わず）
    if (key_name.size() == 1) {
        char c = key_name[0];
        if ('a' <= c && c <= 'z') c -= 32; // 小文字→大文字変換, char型1byteなので32
        if ('A' <= c && c <= 'Z') return static_cast<WORD>(c);
        if ('0' <= c && c <= '9') return static_cast<WORD>(c);
    }
    // それ以外は失敗
    return 0;
}