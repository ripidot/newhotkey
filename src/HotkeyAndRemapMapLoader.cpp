#include "../include/HotkeyAndRemapMapLoader.hpp"

HotkeyAndRemapMapLoader::HotkeyAndRemapMapLoader(){}
HotkeyAndRemapMapLoader::HotkeyAndRemapMapLoader(std::string filename, std::string vkfilename) : filename_(filename), vkfilename_(vkfilename){}
std::unordered_map<WORD, bool>* HotkeyAndRemapMapLoader::skeys_getter(){
    return &suppress_keys;
}
ParsedHotkey HotkeyAndRemapMapLoader::parse_key_with_modifiers(const std::string& key_str) {
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
            result.key = keymaploader.key_string_to_vk(token);
        }
    }
    return result;
}
void HotkeyAndRemapMapLoader::register_remap(WORD key, HotkeyAction hotkeyaction, bool suppress = true){
    suppress_keys[key] = suppress;
    remap_map[key] = [hotkeyaction](bool keyDown){
        auto& action = keyDown ? hotkeyaction.on_press : hotkeyaction.on_release;
        if (action) return action();
        return (WORD)0;
    };
}
void HotkeyAndRemapMapLoader::register_loaded_remaps(){
    std::unordered_map<std::string, std::string> lremaps = *fileaccess.lremaps_getter();
    for (auto& [from_key, to_key] : lremaps) {
        WORD vk_from_key, vk_to_key;
        vk_from_key = keymaploader.key_string_to_vk(from_key);
        vk_to_key = keymaploader.key_string_to_vk(to_key);

        if (vk_from_key == 0) {
            debug_log(LogLevel::Error, "Invalid input key: ", from_key);
            continue;
        }
        if (vk_to_key == 0) {
            debug_log(LogLevel::Error, "Invalid output key: ", to_key);
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

void HotkeyAndRemapMapLoader::register_hotkey(WORD key, bool shift, bool ctrl, bool alt, bool win, 
    HotkeyAction hotkeyaction, bool suppress = true) {
    Hotkey hk = { key, shift, ctrl, alt , win};
    suppress_hotkeys[hk] = suppress;
    hotkey_map[hk] = [hotkeyaction](bool keyDown) -> bool {
        auto& action = keyDown ? hotkeyaction.on_press : hotkeyaction.on_release;
        if (action) return action();  // trueなら抑制したい
        return false;
    };
}
void HotkeyAndRemapMapLoader::register_loaded_hotkeys(){
    std::unordered_map<std::string, HotkeyCommandAction> loaded_hotkeys = *fileaccess.lhotkeys_getter();
    for (auto& [key_str, action] : loaded_hotkeys) {
        ParsedHotkey parsed = parse_key_with_modifiers(key_str);
        
        // 無効なキー or REMAPで登録すべき内容
        if (parsed.key == 0 || !parsed.win && !parsed.alt && !parsed.ctrl && !parsed.shift
        && action.command == "input_key") {
            debug_log(LogLevel::Error, "Invalid key: ", key_str);
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
void HotkeyAndRemapMapLoader::load(){
    keymaploader.load(vkfilename_);
    fileaccess.load_hotkeys_from_file();
    register_loaded_hotkeys();
    register_loaded_remaps();
}

// アクションの実行
void HotkeyAndRemapMapLoader::execute_action(ProcessType p, WORD vk_code, const Hotkey& current, bool keyDown){
    switch(p){
        case ProcessType::KeyLogger:{
            std::string str = keymaploader.vk_to_key_string(current.key);
            keylogger.onKeyPress(str);
            break;
        }
        case ProcessType::Remap:{
            auto sit = remap_map.find(vk_code); // 見つからなければremap.end()を返す
            if (sit != remap_map.end()) { // 見つかったら
                vk_code = sit->second(keyDown); // 登録された関数を実行
            }
            if (keyDown) { // キーロガー機能
                std::string str = keymaploader.vk_to_key_string(vk_code);
                keylogger.onKeyPress(str);
            }
            break;
        }
        case ProcessType::Hotkey:{
            auto it = hotkey_map.find(current);
            if (it != hotkey_map.end()) {
                it->second(keyDown); // 登録された関数を実行
            }
            break;
        }
    }
}

void HotkeyAndRemapMapLoader::execute(WORD vk_code, bool keyDown) { // actionを実行
    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    bool win = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0;

    Hotkey current = { vk_code, shift, ctrl, alt, win };

    execute_action(ProcessType::Hotkey, vk_code, current, keyDown); // hotkeyのaction
    execute_action(ProcessType::Remap, vk_code, current, keyDown); // remapのaction
}
// テストの実行
void HotkeyAndRemapMapLoader::test_invalid_key(){
    WORD unknown = keymaploader.key_string_to_vk("FAKEKEY");
    assert(unknown == 0);
}
void HotkeyAndRemapMapLoader::run_all_tests(){
    test_invalid_key();
    debug_log(LogLevel::Info, "Passed all tests");
}