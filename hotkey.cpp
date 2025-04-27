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

#define DEBUG_MODE 1

constexpr bool debug_mode = true; // デバッグ出力するかどうか
using WORD = unsigned short;

enum class LogLevel {
    LogInfo,
    Info,
    Warning,
    Error
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
std::ostream& operator<<(std::ostream& os, const Hotkey& h){
    os << "{";
    os << h.key << " " << h.shift << " " << h.ctrl << " " << h.alt << " " << h.win;
    os << "}";
    return os;
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

std::unordered_map<WORD, std::function<bool(bool keyDown)>> single_key_map; //単キー
std::unordered_map<Hotkey, std::function<bool(bool keyDown)>> hotkey_map; //修飾キー+通常キー
std::unordered_map<WORD, bool> suppress_keys;
std::unordered_map<Hotkey, bool> suppress_hotkeys;
std::unordered_map<std::string, HotkeyCommandAction> loaded_hotkeys;
std::atomic<bool> suppress_input = false;

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

WORD key_string_to_vk(const std::string& key_name) {
    static const std::unordered_map<std::string, WORD> vk_map = {
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
        {"VK_NONCONVERT", VK_NONCONVERT},
        {"VK_VOLUME_DOWN", VK_VOLUME_DOWN},
        {"VK_VOLUME_UP", VK_VOLUME_UP},
        {"VK_MEDIA_PLAY_PAUSE", VK_MEDIA_PLAY_PAUSE},
    };

    // "VK_"で始まる名前ならテーブル検索
    auto it = vk_map.find(key_name);
    if (it != vk_map.end()) {
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

bool parse_line(const std::string& line, std::string& key_part, std::string& action_part) {
    bool escaped = false;
    bool found_colon = false;
    key_part.clear();
    action_part.clear();

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (escaped) {
            if (!found_colon) key_part += c;
            else action_part += c;
            escaped = false;
        } else {
            if (c == '\\') {
                escaped = true;
            } else if (c == '#' && !found_colon) {
                break; // 非エスケープ#でコメント開始
            } else if (c == ':' && !found_colon) {
                found_colon = true;
            } else {
                if (!found_colon) key_part += c;
                else action_part += c;
            }
        }
    }

    // key_partが空なら無効な行
    return !key_part.empty();
}

void load_hotkeys_from_file(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(infile, line)) {
        // 前後の空白削除
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line[0] == '#') continue; // 空行・コメント行はスキップ

        std::string key_part, action_part;
        if (!parse_line(line, key_part, action_part)) continue;

        // 前後の空白をさらに削除
        key_part.erase(0, key_part.find_first_not_of(" \t"));
        key_part.erase(key_part.find_last_not_of(" \t") + 1);
        action_part.erase(0, action_part.find_first_not_of(" \t"));
        action_part.erase(action_part.find_last_not_of(" \t") + 1);

        // アクション部分を解析（最初の単語がコマンド、それ以降がパラメータ）
        std::istringstream action_stream(action_part);
        std::string command;
        action_stream >> command;

        std::string param;
        std::getline(action_stream, param);
        if (!param.empty() && param[0] == ' ') param.erase(0, 1);

        HotkeyCommandAction act = {command, param};
        loaded_hotkeys[key_part] = act;

        // デバッグ出力
        debug_log(LogLevel::Info,"Loaded: [", key_part, "] => [", command, "] [", param, "]");
    }
}

void register_single_key(WORD key, HotkeyAction hotkeyaction, bool suppress = true){
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
            result.key = key_string_to_vk(token);
        }
    }
    return result;
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
                        if (!launch_app(action.command)){
                            std::cerr << "Failed to launch app: " << action.parameter << std::endl;}
                        // system(("start \"\" \"" + action.parameter + "\"").c_str());
                    }
                    else if (action.command == "open_url") {
                        if (!open_url(action.command)) {
                            std::cerr << "Failed to open URL: " << action.parameter << std::endl;
                        }
                        // system(("start " + action.parameter).c_str());
                    }
                    else if (action.command == "volume_up") {
                        volume_up();
                        debug_log(LogLevel::Error, "volume_up");
                        // 音量上げる関数呼び出し
                    }
                    else if (action.command == "volume_down") {
                        volume_down();
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

void processHotkey(WORD vk_code, bool keyDown) {
    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    bool win = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0;

    Hotkey current = { vk_code, shift, ctrl, alt, win };
    debug_log(LogLevel::LogInfo, current);

    auto sit = single_key_map.find(vk_code);
    if (sit != single_key_map.end()) {
        sit->second(keyDown); // 登録された関数を実行
    }
    auto it = hotkey_map.find(current);
    if (it != hotkey_map.end()) {
        it->second(keyDown); // 登録された関数を実行
    }
}

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

bool shouldSuppress(WORD vkCode, bool isKeyDown) {
    auto it1 = suppress_keys.find(vkCode);
    if (it1 != suppress_keys.end()) {
        return it1->second;
    }

    return false;
}

// キーフックの中での処理
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && !suppress_input) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        int vkCode = p->vkCode; // a->65, vkcode
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isKeyUp   = (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP);
        
        // キーが押された場合
        if (isKeyDown) {

            // ストリング処理（たとえば末尾に vkCode を追加）
            // processHotstring(vkCode);

            // ホットキー処理（同時押しなどをチェック）
            processHotkey(vkCode, true);
        }
        if (isKeyUp) {
            processHotkey(vkCode, false);
        }

        if (shouldSuppress(vkCode, isKeyDown)) {
            return 1;  // このイベントはアプリに流さない
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void setupHotkeys() {
    register_hotkey('V', false, true, false, false, 
        HotkeyAction {
            []() -> bool {
            debug_log(LogLevel::Info, "Ctrl + V pressed!");
            return true;}
        },
        true
    );

    register_hotkey('D', false, true, false, false, 
        HotkeyAction {
            []() -> bool { 
            debug_log(LogLevel::Info, "Ctrl + D pressed!"); 
            return true;}
        },
        true
    );

    register_single_key(VK_NONCONVERT,
        HotkeyAction {
            []() -> bool { SendKeyboardInput(VK_BACK, true);
                return true;}, // backspace押下
            []() -> bool { SendKeyboardInput(VK_BACK, false);
                return true;}// backspace離す
        },
        true
    );

    register_single_key(VK_LWIN,
        HotkeyAction {
            []() -> bool { SendKeyboardInput(VK_CONTROL, true);
                return true;}, // Lctrl押下
            []() -> bool { SendKeyboardInput(VK_CONTROL, false);
                return true;}// Lctrl離す
        }
    );

    register_single_key(VK_LCONTROL,
        HotkeyAction {
            []() -> bool { SendKeyboardInput(VK_LWIN, true);
                return true;}, // Lwin押下
            []() -> bool { SendKeyboardInput(VK_LWIN, false);
                return true;}// Lwin離す
        }
    );

    // register_hotkey(VK_LWIN, false, false, false, false, []() {
    //     SendKeyboardInput(VK_LCONTROL, true);  // Lctrl押下
    //     SendKeyboardInput(VK_LCONTROL, false); // Lctrl離す
    //     std::cout << "Lcontrol pressed" << std::endl;
    // });

    // register_hotkey(VK_CAPITAL, false, false, false, false, []() {
    //     SendKeyboardInput(244, true);  // 全角半角押下
    //     SendKeyboardInput(244, false); // 全角半角離す
    //     std::cout << "zenkaku pressed" << std::endl;
    // });

    // register_hotkey(244, false, false, false, false, []() {
    //     SendKeyboardInput(VK_CAPITAL, true);  // caps押下
    //     SendKeyboardInput(VK_CAPITAL, false); // caps離す
    //     std::cout << "caps pressed" << std::endl;
    // });
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    HHOOK hHook = NULL;
    #define HOTKEY_ID 1

    setupHotkeys();

    // キーボードフック開始
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (hHook == NULL) {
        debug_log(LogLevel::Error, "Failed to install keyboard hook!");
        return 1;
    }
    std::string fileurl = "hotkeys.txt";
    load_hotkeys_from_file(fileurl);
    register_loaded_hotkeys();

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

    // 終了時にフック解除
    UnhookWindowsHookEx(hHook);
    return 0;
}