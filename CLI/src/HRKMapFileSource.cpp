#include "../include/HRKMapFileSource.hpp"

VKMap HRKMapFileSource::vkmap;
HRKMap HRKMapFileSource::hrkmap;
SupMap HRKMapFileSource::supmap;

HRKMapFileSource::HRKMapFileSource(){}
HRKMapFileSource::HRKMapFileSource(PATH hotkeyfilename)
 : hotkeyfilename(hotkeyfilename){}

HRKMap HRKMapFileSource::getHRKMap(){
    return hrkmap;
};
SupMap HRKMapFileSource::getSupMap(){
    return supmap;
};
void HRKMapFileSource::setvkmap(VKMap vkmap){
    this->vkmap = std::move(vkmap);
};
// char c を取り出し、asciiコード表を基にvkに変換
WORD HRKMapFileSource::key_string_to_vk(const std::string& key_name) { 

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
std::string HRKMapFileSource::vk_to_key_string(WORD vk){
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

ParsedHotkey HRKMapFileSource::parse_key_with_modifiers(const std::string& key_str) {
    std::istringstream iss(key_str);
    std::string token;

    ParsedHotkey result = {0, false, false, false, false};

    while (iss >> token) {
        if (token == "shift") result.shift = true;
        else if (token == "ctrl") result.ctrl = true;
        else if (token == "alt") result.alt = true;
        else if (token == "win") result.win = true;
        else {
            // 最初に出てくるキーが本体キー
            result.key = key_string_to_vk(token);
        }
    }
    return result;
}
void HRKMapFileSource::register_remap(WORD key, HotkeyAction hotkeyaction, bool suppress = true){
    supmap.suppress_keys[key] = suppress;
    hrkmap.remap_map[key] = [hotkeyaction](bool keyDown){
        auto& action = keyDown ? hotkeyaction.on_press : hotkeyaction.on_release;
        if (action) return action();
        return (WORD)0;
    };
}
void HRKMapFileSource::register_loaded_remaps(){
    std::unordered_map<std::string, std::string> lremaps = *fileaccess.lremaps_getter();
    for (auto& [from_key, to_key] : lremaps) {
        WORD vk_from_key, vk_to_key;
        vk_from_key = key_string_to_vk(from_key);
        vk_to_key = key_string_to_vk(to_key);

        if (vk_from_key == 0) {
            debug_log(LogLevel::Error, "Invalid remap input key: ", from_key);
            continue;
        }
        if (vk_to_key == 0) {
            debug_log(LogLevel::Error, "Invalid remap output key: ", to_key);
            continue;
        }
        // remap機能の登録
        register_remap(vk_from_key,
            HotkeyAction {
                [&, vk_to_key]() -> WORD { RemapActionFuncs::SendKeyboardInput(vk_to_key, true);
                    return vk_to_key;},
                [&, vk_to_key]() -> WORD { RemapActionFuncs::SendKeyboardInput(vk_to_key, false);
                    return vk_to_key;}
            },
        true
        );
    }
}

void HRKMapFileSource::register_hotkey(WORD key, bool shift, bool ctrl, bool alt, bool win, 
    const HotkeyAction& hotkeyaction, bool suppress = true) {
    Hotkey hk = { key, shift, ctrl, alt , win};
    supmap.suppress_hotkeys[hk] = suppress;
    hrkmap.hotkey_map[hk] = [hotkeyaction](bool keyDown) -> bool {
        auto& action = keyDown ? hotkeyaction.on_press : hotkeyaction.on_release;
        if (action) return action();  // trueなら抑制したい
        return false;
    };
}
void HRKMapFileSource::register_loaded_hotkeys(){
    std::unordered_map<std::string, HotkeyCommandAction> loaded_hotkeys = *fileaccess.lhotkeys_getter();
    for (auto& [key_str, action] : loaded_hotkeys) {
        ParsedHotkey parsed = parse_key_with_modifiers(key_str);
        
        // 無効なキー or REMAPで登録すべき内容
        if (parsed.key == 0 || !parsed.win && !parsed.alt && !parsed.ctrl && !parsed.shift
        && action.command == "input_key") {
            debug_log(LogLevel::Error, "Invalid hotkey: ", key_str);
            continue;
        }

        // hotkey機能の登録
        register_hotkey(parsed.key, parsed.shift, parsed.ctrl, parsed.alt, parsed.win,
            HotkeyAction { // defaultでkeyupはfalseが入る
                [action]() -> bool{
                    if (action.command == "launch_app") {
                        if (!HotkeyActionFuncs::launch_app(action.parameter)){
                            debug_log(LogLevel::Error, "Failed to launch app: ", action.parameter);
                        }
                    }
                    else if (action.command == "open_url") {
                        if (!HotkeyActionFuncs::open_url(action.parameter)) {
                            debug_log(LogLevel::Error, "Failed to open url: ", action.parameter);
                        }
                    }
                    else if (action.command == "volume_up") {
                        HotkeyActionFuncs::volume_up();
                        debug_log(LogLevel::Info, "volume_up");
                        // 音量上げる関数呼び出し
                    }
                    else if (action.command == "volume_down") {
                        HotkeyActionFuncs::volume_down();
                        debug_log(LogLevel::Info, "volume_down");
                        // 音量下げる関数呼び出し
                    }
                    else if (action.command == "input_key") {
                        HotkeyActionFuncs::input_key(action.parameter); // キー入力
                        debug_log(LogLevel::Info, "input_key", action.parameter);
                    }
                    else {
                        debug_log(LogLevel::Error, "Unknown command: ", action.command);
                    }
                    return true;
                }
            },
            true
        );

    }
}

void HRKMapFileSource::register_hotstring(std::string from_key,
    std::function<std::string()> func){
    hrkmap.hotstring_map[from_key] = func;
}
void HRKMapFileSource::register_loaded_hotstrings(){ // lkstrings = hotkstrings
    std::unordered_map<std::string, std::string> lkeystrings = *fileaccess.lkstrings_getter();
    for (auto& [from_key, to_key] : lkeystrings) {
        if (to_key.size() == 0) {
            debug_log(LogLevel::Error, "Invalid keystring:", from_key);
            continue;
        }
        register_hotstring(from_key, [from_key, to_key]() -> std::string { 
            KeystringActionFuncs::simulateTextInput(to_key);
            return from_key;
        });
    }
    // hrkmap.hotstrings = move(*fileaccess.lkstrings_getter());
}
void HRKMapFileSource::load(){
    fileaccess.set_filename(hotkeyfilename);
    fileaccess.load_hotkeys_from_file();

    register_loaded_hotkeys();
    register_loaded_remaps();
    register_loaded_hotstrings();
}