#include "../include/HotkeyAndRemapMapLoader.hpp"

std::string filename_;
FileAccess amap = FileAccess(filename_);
KeyMapLoader keymaploader = KeyMapLoader();
std::unordered_map<Hotkey, std::function<bool(bool keyDown)>> hotkey_map; //修飾キー+通常キーと関数の紐づけ
std::unordered_map<WORD, std::function<bool(bool keyDown)>> remap_map; //単キーと関数の紐づけ

std::unordered_map<Hotkey, bool> suppress_hotkeys;
std::unordered_map<WORD, bool> suppress_keys;

HotkeyAndRemapMapLoader::HotkeyAndRemapMapLoader(){}
HotkeyAndRemapMapLoader::HotkeyAndRemapMapLoader(std::string filename) : filename_(filename){}
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
// remap用sendinput関数
void HotkeyAndRemapMapLoader::SendKeyboardInput(WORD key, bool keyDown) { //key:vk_key
    KeyboardHookManager::suppress_input = true;
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = keyDown ? 0 : KEYEVENTF_KEYUP;

    SendInput(1, &input, sizeof(INPUT));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    KeyboardHookManager::suppress_input = false;
}
void HotkeyAndRemapMapLoader::register_remap(WORD key, HotkeyAction hotkeyaction, bool suppress = true){
    suppress_keys[key] = suppress;
    remap_map[key] = [hotkeyaction](bool keyDown){
        auto& action = keyDown ? hotkeyaction.on_press : hotkeyaction.on_release;
        if (action) return action();
        return false;
    };
}
void HotkeyAndRemapMapLoader::register_loaded_remaps(){
    std::unordered_map<std::string, std::string> lremaps = *amap.lremaps_getter();
    for (auto& [from_key, to_key] : lremaps) {
        WORD vk_from_key, vk_to_key;
        vk_from_key = keymaploader.key_string_to_vk(from_key);
        vk_to_key = keymaploader.key_string_to_vk(to_key);

        if (vk_from_key == 0) {
            std::cerr << "Invalid input key: " << from_key << std::endl;
            continue;
        }
        if (vk_to_key == 0) {
            std::cerr << "Invalid output key: " << to_key << std::endl;
            continue;
        }
        // ここでホットキー登録
        register_remap(vk_from_key,
            HotkeyAction {
                [&, vk_to_key]() -> bool { SendKeyboardInput(vk_to_key, true);
                    return true;}, // backspace押下
                [&, vk_to_key]() -> bool { SendKeyboardInput(vk_to_key, false);
                    return true;}// backspace離す
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
    std::unordered_map<std::string, HotkeyCommandAction> loaded_hotkeys = *amap.lhotkeys_getter();
    for (auto& [key_str, action] : loaded_hotkeys) {
        ParsedHotkey parsed = parse_key_with_modifiers(key_str);

        if (parsed.key == 0) {
            std::cerr << "Invalid key: " << key_str << std::endl;
            continue;
        }

        // ここでホットキー登録
        register_hotkey(parsed.key, parsed.shift, parsed.ctrl, parsed.alt, parsed.win,
            HotkeyAction {
                [action]() -> bool{
                    if (action.command == "launch_app") {
                        if (!ActionFuncs::launch_app(action.command)){
                            std::cerr << "Failed to launch app: " << action.parameter << std::endl;}
                    }
                    else if (action.command == "open_url") {
                        if (!ActionFuncs::open_url(action.command)) {
                            std::cerr << "Failed to open URL: " << action.parameter << std::endl;
                        }
                    }
                    else if (action.command == "volume_up") {
                        ActionFuncs::volume_up();
                        debug_log(LogLevel::Info, "volume_up");
                        // 音量上げる関数呼び出し
                    }
                    else if (action.command == "volume_down") {
                        ActionFuncs::volume_down();
                        debug_log(LogLevel::Info, "volume_down");
                        // 音量下げる関数呼び出し
                    }
                    else if (action.command == "input_key") {
                        ActionFuncs::input_key(action.command); // キー入力
                        debug_log(LogLevel::Info, "input_key");
                    }
                    else {
                        std::cerr << "Unknown action: " << action.command << std::endl;
                    }
                    return true;
                }
            },
            true
        );

    }
}
void HotkeyAndRemapMapLoader::load(){
    amap.load_hotkeys_from_file();
    register_loaded_hotkeys();
    register_loaded_remaps();
}

// アクションの実行
void HotkeyAndRemapMapLoader::execute_action(ProcessType p, WORD vk_code, const Hotkey& current, bool keyDown){
    switch(p){
        case ProcessType::Remap:{
            auto sit = remap_map.find(vk_code);
            if (sit != remap_map.end()) {
                sit->second(keyDown); // 登録された関数を実行
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
    debug_log(LogLevel::LogInfo, current);

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

