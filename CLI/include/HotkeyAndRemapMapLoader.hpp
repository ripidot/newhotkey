#pragma once
#include "KeyMapLoader.hpp"
#include "LogConfigLoader.hpp"
#include "KeyLogger.hpp"
#include "PathManager.hpp"
#include "HRKMapFileSource.hpp"
#include <psapi.h>
#include <chrono>
#include <thread>
#include <cassert>
class HotkeyAndRemapMapLoader{
    private:
        PATH hotkeyfilename;
        PATH vkfilename;
        PATH initlogfilename;
        PATH dbfilename;
        PATH errorfilename;
        FileAccess fileaccess = FileAccess(hotkeyfilename);
        KeyMapLoader keymaploader = KeyMapLoader();
        KeyLogger keylogger = KeyLogger();
        KeystringActionFuncs keystringactionfuncs = KeystringActionFuncs();

        static HRKMap hrkmap;
        static SupMap supmap;
        static VKMap vkmap;
        static inline std::string inputBuffer;

    public:
        HotkeyAndRemapMapLoader();
        HotkeyAndRemapMapLoader(const PathManager& pm);
        void setHRKMap(const HRKMap& hrkmap);
        void setSupMap(const SupMap& supmap);
        std::unordered_map<WORD, bool>* skeys_getter();
        SupMap getSupMap();
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
        KeyLog returnLogMsg(WORD vk_code, const Hotkey& current, bool keyDown);
        void deleteTrigger(int triggerSize);
// アクションの実行
        void execute_action(ProcessType p, WORD vk_code, const Hotkey& current, bool keyDown);
        void execute(WORD vk_code, bool keyDown);
        
// テストの実行
        void test_invalid_key();
        void run_all_tests();
};
