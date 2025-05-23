﻿#pragma once
#include "FileAccess.hpp"
#include "KeyMapLoader.hpp"
#include "HotkeyActionFuncs.hpp"
#include "RemapActionFuncs.hpp"
#include "KeystringActionFuncs.hpp"
#include "KeyLogger.hpp"
#include <psapi.h>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <cassert>
class HotkeyAndRemapMapLoader{
    private:
        PATH filename_;
        PATH vkfilename_;
        PATH initlogfilename_;
        PATH dbfilename_;
        FileAccess fileaccess = FileAccess(filename_);
        KeyMapLoader keymaploader = KeyMapLoader();
        KeyLogger keylogger = KeyLogger();
        KeystringActionFuncs keystringactionfuncs = KeystringActionFuncs();
        std::unordered_map<Hotkey, std::function<bool(bool keyDown)>> hotkey_map; //修飾キー+通常キーと関数の紐づけ
        std::unordered_map<WORD, std::function<WORD(bool keyDown)>> remap_map; //単キーと関数の紐づけ

        std::unordered_map<Hotkey, bool> suppress_hotkeys;
        std::unordered_map<WORD, bool> suppress_keys;
        static inline std::string inputBuffer;
        std::unordered_map<std::string, std::string> hotstrings;
    public:
        HotkeyAndRemapMapLoader();
        HotkeyAndRemapMapLoader(PATH filename, PATH vkfilename);
        HotkeyAndRemapMapLoader(PATH filename, PATH vkfilename, PATH initlogfilename);
        HotkeyAndRemapMapLoader(PATH filename, PATH vkfilename, PATH initlogfilename, PATH dbfilename);
        std::unordered_map<WORD, bool>* skeys_getter();
        ParsedHotkey parse_key_with_modifiers(const std::string& key_str);
        void register_remap(WORD key, HotkeyAction hotkeyaction, bool suppress);
        void register_loaded_remaps();
        void register_hotkey(WORD key, bool shift, bool ctrl, bool alt, bool win, 
            const HotkeyAction& hotkeyaction, bool suppress);
        void register_loaded_hotkeys();
        void register_loaded_keystrings();
        void load();
        void simulateTextInput(const std::wstring& text);
        void inputToBuffer(WORD vkCode);

// アクションの実行
        void execute_action(ProcessType p, WORD vk_code, const Hotkey& current, bool keyDown);
        void execute(WORD vk_code, bool keyDown);
        
// テストの実行
        void test_invalid_key();
        void run_all_tests();
};
