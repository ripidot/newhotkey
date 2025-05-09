#include "../include/Logger.hpp"

inline std::ostream& operator<<(std::ostream& os, const Hotkey& h){
    os << "{";
    os << h.key << " " << h.shift << " " << h.ctrl << " " << h.alt << " " << h.win;
    os << "}";
    return os;
}
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<WORD, bool>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem.first << " ";
    }
    os << "}";
    return os;
}
inline std::ostream& operator<<(std::ostream& os, const ParsedLineType& p){
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
inline std::ostream& operator<<(std::ostream& os, const std::unordered_set<int>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem << " ";
    }
    os << "}";
    return os;
}
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<Hotkey, std::function<bool(bool keyDown)>>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem.first << " ";
    }
    os << "}";
    return os;
}
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<Hotkey, bool>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem.first << " ";
    }
    os << "}";
    return os;
}

template<typename... Args>
void debug_log(LogLevel level, Args&&... args) {
    if (level < constants::displaylevel) return;
    if constexpr (constants::debug_mode) {
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
