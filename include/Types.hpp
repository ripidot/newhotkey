#pragma once
#include <string>
#include <functional>
#include <unordered_set>


using WORD = unsigned short;
enum class LogLevel {
    LogInfo,
    Info,
    Warning,
    Error
};
enum class ProcessType{ // 命令のタイプ判別
    Hotkey,
    Remap,
    KeyLogger
};
enum class ParsedLineType { // 読み込み行のタイプ判別
    Hotkey,
    Remap,
    Invalid
};

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
struct HotkeyAction {
    std::function<WORD()> on_press;
    std::function<WORD()> on_release = nullptr;
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

namespace constants {
    inline constexpr LogLevel displaylevel = LogLevel::Info;
    constexpr bool debug_mode = true; // デバッグ出力するかどうか
}
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
