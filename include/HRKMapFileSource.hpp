#pragma once
#include "FileAccess.hpp"
#include "HotkeyActionFuncs.hpp"
#include "RemapActionFuncs.hpp"
#include "KeystringActionFuncs.hpp"
#include "PathManager.hpp"
#include <psapi.h>
class HRKMapFileSource{
    private:
        PATH hotkeyfilename;
        FileAccess fileaccess = FileAccess();
        KeystringActionFuncs keystringactionfuncs = KeystringActionFuncs();

        static HRKMap hrkmap;
        static VKMap vkmap;
        static SupMap supmap;

    public:
        HRKMapFileSource();
        HRKMapFileSource(PATH hotkeyfilename);
        WORD key_string_to_vk(const std::string& key_name);
        std::string vk_to_key_string(WORD vk);
        ParsedHotkey parse_key_with_modifiers(const std::string& key_str);
        void register_remap(WORD key, HotkeyAction hotkeyaction, bool suppress);
        void register_loaded_remaps();
        void register_hotkey(WORD key, bool shift, bool ctrl, bool alt, bool win, 
            const HotkeyAction& hotkeyaction, bool suppress);
        void register_loaded_hotkeys();
        void simulateTextInput(const std::wstring& text);
        void register_hotstring(std::string from_key, std::function<std::string()> func);
        void register_loaded_hotstrings();
        void load();
        HRKMap getHRKMap();
        SupMap getSupMap();
        void setvkmap(VKMap vkmap);
};