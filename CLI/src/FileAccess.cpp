#include "../include/FileAccess.hpp"


const std::string fileurl_;
std::unordered_map<std::string, HotkeyCommandAction> loaded_hotkeys; //"A ctrl shift", {"launch_app", "notepad.exe"}
std::unordered_map<std::string, std::string> loaded_remaps; //"Lctrl" , "Lwin"

FileAccess::FileAccess() : fileurl_(){}
FileAccess::FileAccess(std::string fileurl) : fileurl_(fileurl){}

std::unordered_map<std::string, HotkeyCommandAction>* FileAccess::lhotkeys_getter(){
    return &loaded_hotkeys;
}
std::unordered_map<std::string, std::string>* FileAccess::lremaps_getter(){
    return &loaded_remaps;
}
void FileAccess::load_hotkeys_from_file() {
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