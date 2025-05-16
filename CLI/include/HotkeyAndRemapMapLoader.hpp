#pragma once
#include "FileAccess.hpp"
#include "KeyMapLoader.hpp"
#include "HotkeyActionFuncs.hpp"
#include "RemapActionFuncs.hpp"
#include "KeyLogger.hpp"
#include <unordered_map>
#include <chrono>
#include <thread>
#include <cassert>
class HotkeyAndRemapMapLoader{
    private:
        std::string filename_;
        std::string vkfilename_;
        FileAccess fileaccess = FileAccess(filename_);
        KeyMapLoader keymaploader = KeyMapLoader();
        KeyLogger keylogger = KeyLogger();
        std::unordered_map<Hotkey, std::function<bool(bool keyDown)>> hotkey_map; //修飾キー+通常キーと関数の紐づけ
        std::unordered_map<WORD, std::function<WORD(bool keyDown)>> remap_map; //単キーと関数の紐づけ

        std::unordered_map<Hotkey, bool> suppress_hotkeys;
        std::unordered_map<WORD, bool> suppress_keys;
    public:
        HotkeyAndRemapMapLoader();
        HotkeyAndRemapMapLoader(std::string filename, std::string vkfilename);
        std::unordered_map<WORD, bool>* skeys_getter();
        ParsedHotkey parse_key_with_modifiers(const std::string& key_str);
        void register_remap(WORD key, HotkeyAction hotkeyaction, bool suppress);
        void register_loaded_remaps();
        void register_hotkey(WORD key, bool shift, bool ctrl, bool alt, bool win, 
            HotkeyAction hotkeyaction, bool suppress);
        void register_loaded_hotkeys();
        void load();

// アクションの実行
        void execute_action(ProcessType p, WORD vk_code, const Hotkey& current, bool keyDown);
        void execute(WORD vk_code, bool keyDown);
// テストの実行
        void test_invalid_key();
        void run_all_tests();
};
