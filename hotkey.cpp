#include <windows.h>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <iomanip>  // 時間のフォーマットに使う
#include <fstream>
#include <sstream>
#include <cassert>

#define DEBUG_MODE 1

constexpr bool debug_mode = true; // デバッグ出力するかどうか
using WORD = unsigned short;

enum class LogLevel {
    LogInfo,
    Info,
    Warning,
    Error
};
enum class ProcessType{ // 命令のタイプ判別
    Hotkey,
    Remap
};
enum class ParsedLineType { // 読み込み行のタイプ判別
    Hotkey,
    Remap,
    Invalid
};

LogLevel displaylevel = LogLevel::Info;

struct HotkeyCommandAction {
    std::string command; // ex.openurl, volumeup...
    std::string parameter;
};
struct ParsedHotkey {
    WORD key;
    bool shift;
    bool ctrl;
    bool alt;
    bool win;
};
struct ParsedLine {
    ParsedLineType type;
    std::string key_part;
    std::string action_part;
    std::string from_key;
    std::string to_key;
};
struct Hotkey {
    WORD key;
    bool shift;
    bool ctrl;
    bool alt;
    bool win;

    bool operator==(const Hotkey& o) const {
        return key == o.key && shift == o.shift && ctrl == o.ctrl && alt == o.alt && win == o.win;
    }
    bool operator==(const std::unordered_set<int>& o) const{
        return false;
    }
};
std::ostream& operator<<(std::ostream& os, const Hotkey& h){
    os << "{";
    os << h.key << " " << h.shift << " " << h.ctrl << " " << h.alt << " " << h.win;
    os << "}";
    return os;
}

struct HotkeyAction {
    std::function<bool()> on_press;
    std::function<bool()> on_release = nullptr;
};

// ハッシュ関数を定義（unordered_map用）
namespace std {
    template <>
    struct hash<Hotkey> {
        size_t operator()(const Hotkey& k) const {
            return ((std::hash<WORD>()(k.key)) ^
                    (std::hash<bool>()(k.shift) << 1) ^
                    (std::hash<bool>()(k.ctrl) << 2) ^
                    (std::hash<bool>()(k.alt) << 3) ^
                    (std::hash<bool>()(k.win) << 4));
        }
    };
}

namespace StringUtils {
    ParsedLine parse_line(const std::string& line) {
        bool escaped = false;
        bool found_colon = false;
        bool found_arrow = false;
        ParsedLine result;
        result.type = ParsedLineType::Invalid;
        std::string left, right;
    
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (escaped) {
                ((found_colon || found_arrow) ? right : left) += c;
                escaped = false;
            } else {
                if (c == '\\') {
                    escaped = true;
                } else if (c == '#' && !found_colon && !found_arrow) {
                    break; // 非エスケープ#でコメント開始
                } else if (c == ':' && !found_colon && !found_arrow) {
                    found_colon = true;
                } else if (c == '-' && i + 1 < line.size() && line[i + 1] == '>'
                            && !found_colon && !found_arrow){
                    found_arrow = true;
                    i++;
                }else {
                    ((found_colon || found_arrow) ? right : left) += c;
                }
            }
        }
        // 空白トリム
        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t"));
            s.erase(s.find_last_not_of(" \t") + 1);
        };
        trim(left);
        trim(right);
    
        if (found_colon && !left.empty()) {
            result.type = ParsedLineType::Hotkey;
            result.key_part = left;
            result.action_part = right;
        } else if (found_arrow && !left.empty() && !right.empty()) {
            result.type = ParsedLineType::Remap;
            result.from_key = left;
            result.to_key = right;
        }
    
        return result; // r.left = "A ctrl shift", r.right = "launch_app notepad.exe"
    }
}
namespace ActionFuncs { // hotkeyに対してのアクションの関数
    void volume_up() {
        keybd_event(VK_VOLUME_UP, 0, 0, 0);   // キー押し
        keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_KEYUP, 0); // キー離し
    }
    void volume_down() {
        keybd_event(VK_VOLUME_DOWN, 0, 0, 0); 
        keybd_event(VK_VOLUME_DOWN, 0, KEYEVENTF_KEYUP, 0);
    }
    bool launch_app(const std::string& path) {
        std::string cmd = "start \"\" \"" + path + "\"";
        int result = system(cmd.c_str());
        return result == 0;
    }
    bool open_url(const std::string& url) {
        std::string cmd = "start " + url;
        int result = system(cmd.c_str());
        return result == 0;
    }
}

std::ostream& operator<<(std::ostream& os, const std::unordered_set<int>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem << " ";
    }
    os << "}";
    return os;
}
std::ostream& operator<<(std::ostream& os, const std::unordered_map<Hotkey, std::function<bool(bool keyDown)>>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem.first << " ";
    }
    os << "}";
    return os;
}
std::ostream& operator<<(std::ostream& os, const std::unordered_map<Hotkey, bool>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem.first << " ";
    }
    os << "}";
    return os;
}
std::ostream& operator<<(std::ostream& os, const ParsedLineType& p){
    os << "{";
    switch(p){
        case ParsedLineType::Hotkey:
            os << "Hotkey";
            break;
        case ParsedLineType::Remap:
            os << "Remap";
            break;
        case ParsedLineType::Invalid:
            os << "Invalid";
            break;
    }
    os << "}";
    return os;
}

template<typename... Args>
void debug_log(LogLevel level, Args&&... args) {
    if (level < displaylevel) return;
    if constexpr (debug_mode) {
        // 現在時刻
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto local_time = *std::localtime(&time);

        std::stringstream ss;
        ss << std::this_thread::get_id();
        std::string thread_id = ss.str();

        // コンソールのハンドルを取得
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        WORD saved_attributes = 0;

        if (hConsole != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {
            saved_attributes = consoleInfo.wAttributes; // 元の色を保存
        }

        // タイムスタンプ
        std::cout << "[" << std::put_time(&local_time, "%H:%M:%S") << "] ";

        // ログレベルに応じて色を変更
        switch (level) {
            case LogLevel::Info:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // 白
                std::cout << "[INFO] ";
                break;
            case LogLevel::Warning:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); // 黄色
                std::cout << "[WARN] ";
                break;
            case LogLevel::Error:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); // 赤
                std::cout << "[ERROR] ";
                break;
        }
        // スレッドID出力
        std::cout << "[Thread " << thread_id << "] ";

        // 本文メッセージ出力
        (std::cout << ... << args) << std::endl;

        // 色を元に戻す
        SetConsoleTextAttribute(hConsole, saved_attributes);
    }
}

std::unordered_map<WORD, std::function<bool(bool keyDown)>> single_key_map; //単キー
std::unordered_map<Hotkey, std::function<bool(bool keyDown)>> hotkey_map; //修飾キー+通常キー
std::unordered_map<WORD, bool> suppress_keys;
std::unordered_map<Hotkey, bool> suppress_hotkeys;
std::unordered_map<std::string, HotkeyCommandAction> loaded_hotkeys; //"A ctrl shift", {"launch_app", "notepad.exe"}
std::unordered_map<std::string, std::string> loaded_remaps; //"Lctrl" , "Lwin"
std::atomic<bool> suppress_input = false;

void SendKeyboardInput(WORD key, bool keyDown) {
    suppress_input = true;
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = keyDown ? 0 : KEYEVENTF_KEYUP;

    SendInput(1, &input, sizeof(INPUT));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    suppress_input = false;
}

class KeyMapLoader {
    private:
        const std::unordered_map<std::string, WORD> vk_map_ = {
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
            {"VK_ZENHANKAKU", 243}
        };
        const std::string fileurl_;
    public:
        KeyMapLoader(std::string fileurl) : fileurl_(fileurl){}
        KeyMapLoader() : fileurl_(){}

        WORD key_string_to_vk(const std::string& key_name) {

            // "VK_"で始まる名前ならテーブル検索
            auto it = vk_map_.find(key_name);
            if (it != vk_map_.end()) {
                return it->second;
            }
        
            // アルファベット単文字（大文字小文字問わず）
            if (key_name.size() == 1) {
                char c = key_name[0];
                if ('a' <= c && c <= 'z') c -= 32; // 小文字→大文字変換
                if ('A' <= c && c <= 'Z') return static_cast<WORD>(c);
                if ('0' <= c && c <= '9') return static_cast<WORD>(c);
            }
        
            // それ以外は失敗
            return 0;
        }
};
class ConfigLoader {
    private:
        const std::string fileurl_;
        KeyMapLoader keymaploader = KeyMapLoader();
    public:
        ConfigLoader(std::string fileurl) : fileurl_(fileurl){}
        ConfigLoader() : fileurl_(){}

        void load_hotkeys_from_file() {
            std::ifstream infile(fileurl_);
            if (!infile) {
                std::cerr << "Failed to open " << fileurl_ << std::endl;
                return;
            }
        
            std::string line;
            while (std::getline(infile, line)) {
                // 前後の空白削除
                line.erase(0, line.find_first_not_of(" \t"));
                line.erase(line.find_last_not_of(" \t") + 1);
        
                if (line.empty() || line[0] == '#') continue; // 空行・コメント行はスキップ
        
                ParsedLine parsed = StringUtils::parse_line(line);
                if (parsed.type == ParsedLineType::Invalid) continue;
        
                // // 前後の空白をさらに削除
                // hotkey用の変数宣言
                std::istringstream action_stream(parsed.action_part);        
                HotkeyCommandAction act;
                std::string command, param;
        
                switch (parsed.type) {
                    case ParsedLineType::Hotkey:
                        // アクション部分を解析（最初の単語がコマンド、それ以降がパラメータ）
                        action_stream >> command; // 空白までがcommandに入る
                        std::getline(action_stream, param); // actionのcommand以外がparamに入る
                        if (!param.empty() && param[0] == ' ') param.erase(0, 1);
                        act = {command, param};
                        loaded_hotkeys[parsed.key_part] = act;
                        break;
                    case ParsedLineType::Remap:
                        loaded_remaps[parsed.from_key] = parsed.to_key;
                        // loaded_remaps.push_back({parsed.from_key, parsed.to_key});
                        break;
                    case ParsedLineType::Invalid:
                        // 無視
                        break;
                }
        
                // デバッグ出力
                switch(parsed.type){
                    case ParsedLineType::Hotkey:
                        debug_log(LogLevel::Info,"Loaded Hotkey: [", parsed.key_part, "] => [", command, "] [", param, "]");
                        break;
                    case ParsedLineType::Remap:
                        debug_log(LogLevel::Info,"Loaded Remap: [", parsed.from_key, "] -> [", parsed.to_key,"]");
                        break;
                }
            }
        }
        ParsedHotkey parse_key_with_modifiers(const std::string& key_str) {
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
        void register_remap(WORD key, HotkeyAction hotkeyaction, bool suppress = true){
            suppress_keys[key] = suppress;
            single_key_map[key] = [hotkeyaction](bool keyDown){
                auto& action = keyDown ? hotkeyaction.on_press : hotkeyaction.on_release;
                if (action) return action();
                return false;
            };
        }
        void register_hotkey(WORD key, bool shift, bool ctrl, bool alt, bool win, 
            HotkeyAction hotkeyaction, bool suppress = true) {
            Hotkey hk = { key, shift, ctrl, alt , win};
            suppress_hotkeys[hk] = suppress;
            hotkey_map[hk] = [hotkeyaction](bool keyDown) -> bool {
                auto& action = keyDown ? hotkeyaction.on_press : hotkeyaction.on_release;
                if (action) return action();  // trueなら抑制したい
                return false;
            };
        }
        void register_loaded_hotkeys() {
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
                                // system(("start \"\" \"" + action.parameter + "\"").c_str());
                            }
                            else if (action.command == "open_url") {
                                if (!ActionFuncs::open_url(action.command)) {
                                    std::cerr << "Failed to open URL: " << action.parameter << std::endl;
                                }
                                // system(("start " + action.parameter).c_str());
                            }
                            else if (action.command == "volume_up") {
                                ActionFuncs::volume_up();
                                debug_log(LogLevel::Error, "volume_up");
                                // 音量上げる関数呼び出し
                            }
                            else if (action.command == "volume_down") {
                                ActionFuncs::volume_down();
                                debug_log(LogLevel::Error, "volume_down");
                                // 音量下げる関数呼び出し
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
        void register_remaps(){
            for (auto& [from_key, to_key] : loaded_remaps) {
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
                        [vk_to_key]() -> bool { SendKeyboardInput(vk_to_key, true);
                            return true;}, // backspace押下
                        [vk_to_key]() -> bool { SendKeyboardInput(vk_to_key, false);
                            return true;}// backspace離す
                    },
                true
                );
            }
        }
        void execute(){
            load_hotkeys_from_file();
            register_loaded_hotkeys();
            register_remaps();
        }
};

void test_invalid_key(){
    WORD unknown = KeyMapLoader().key_string_to_vk("FAKEKEY");
    assert(unknown == 0);
}
void run_all_tests(){
    test_invalid_key();
    debug_log(LogLevel::Info, "Passed all tests");
}

class ActionManager {
    public:
        void processHotkey(WORD vk_code, bool keyDown) {
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
            bool win = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0;
        
            Hotkey current = { vk_code, shift, ctrl, alt, win };
            debug_log(LogLevel::LogInfo, current);
        
            execute_action(ProcessType::Hotkey, vk_code, current, keyDown); // hotkeyのaction
            execute_action(ProcessType::Remap, vk_code, current, keyDown); // remapのaction
        }

        void execute_action(ProcessType p, WORD vk_code, const Hotkey& current, bool keyDown){
            switch(p){
                case ProcessType::Remap:{
                    auto sit = single_key_map.find(vk_code);
                    if (sit != single_key_map.end()) {
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
};

class KeyboardHookManager {
    private:
        static inline HHOOK hHook = nullptr;
        static inline std::function<void(int)> keyDownHandler = nullptr;
        static inline std::function<void(int)> keyUpHandler = nullptr;
        static bool shouldSuppress(WORD vkCode, bool isKeyDown) {
            auto it1 = suppress_keys.find(vkCode);
            if (it1 != suppress_keys.end()) {
                return it1->second;
            }
        
            return false;
        }
        static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
            if (nCode == HC_ACTION && !suppress_input) {
                const KBDLLHOOKSTRUCT* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
                int vkCode = p->vkCode;
                bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
                bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
    
                if (isKeyDown && keyDownHandler) keyDownHandler(vkCode);
                if (isKeyUp && keyUpHandler) keyUpHandler(vkCode);
    
                if (shouldSuppress(vkCode, isKeyDown)) return 1;
            }
            return CallNextHookEx(NULL, nCode, wParam, lParam);
        }
    public:
        void setKeyDownHandler(std::function<void(int)> handler) {
            keyDownHandler = std::move(handler);
        }
        void setKeyUpHandler(std::function<void(int)> handler) {
            keyUpHandler = std::move(handler);
        }
        void setHook() {
            hHook = SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, NULL, 0);
        }
        void removeHook() {
            if (hHook) UnhookWindowsHookEx(hHook);
            hHook = nullptr;
        }
};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    #define HOTKEY_ID 1

    KeyboardHookManager hook;
    ActionManager act;

    hook.setKeyDownHandler([&act](int vk) {
        act.processHotkey(vk, true);
    });
    hook.setKeyUpHandler([&act](int vk) {
        act.processHotkey(vk, false);
    });
    hook.setHook();

    const std::string fileurl = "data/hotkeys.txt";
    ConfigLoader(fileurl).execute();
    run_all_tests();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        switch (msg.message) {
        case WM_HOTKEY:
            if (msg.wParam == HOTKEY_ID) {
                debug_log(LogLevel::Info, "hotkey is pushed");
            }
            break;
        }
    }
    return 0;
}