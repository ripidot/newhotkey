#include "../include/FileAccess.hpp"


const PATH fileurl_;
std::unordered_map<std::string, HotkeyCommandAction> loaded_hotkeys; //"A ctrl shift", {"launch_app", "notepad.exe"}
std::unordered_map<std::string, std::string> loaded_remaps; //"Lctrl" , "Lwin"
std::unordered_map<std::string, std::string> loaded_keystrings;
std::unordered_map<std::string, WORD> vk_map;
std::unordered_map<WORD, std::string> vk_inv_map;

FileAccess::FileAccess() : fileurl_(){}
FileAccess::FileAccess(PATH fileurl) : fileurl_(fileurl){}


std::unordered_map<std::string, HotkeyCommandAction>* FileAccess::lhotkeys_getter(){
    return &loaded_hotkeys;
}
std::unordered_map<std::string, std::string>* FileAccess::lremaps_getter(){
    return &loaded_remaps;
}
std::unordered_map<std::string, std::string>* FileAccess::lkstrings_getter(){
    return &loaded_keystrings;
}
std::unordered_map<std::string, WORD>* FileAccess::vk_map_getter(){
    return &vk_map;
}
std::unordered_map<WORD, std::string>* FileAccess::vk_inv_map_getter(){
    return &vk_inv_map;
}

void FileAccess::set_filename(PATH filename) {
    fileurl_ = filename;
}

void FileAccess::load_hotkeys_from_file() {
    std::ifstream infile(fileurl_);
    if (!infile) {
        throw std::runtime_error("couldn't open hotkeyfile\r\n");
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
            case ParsedLineType::Keystring:
                loaded_keystrings[parsed.from_strkey] = parsed.to_strkey;
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
            case ParsedLineType::Keystring:
                debug_log(LogLevel::Info,"Loaded Keystring: [", parsed.from_strkey, "] -> [", parsed.to_strkey,"]");
        }
    }
}

void FileAccess::load_vk_from_file(){
    using json = nlohmann::json;

    std::ifstream file(fileurl_);
    if (!file.is_open()) {
        throw std::runtime_error("couldn't open vkfile\r\n");
    }

    json j;
    file >> j;

    for (auto& [group_name, group_content] : j.items()) { // j: vkmap_grouped_numeric.json
        for (auto& [key, value] : group_content.items()) {
            vk_map[key] = value;
            vk_inv_map[value] = key;
        }
    }
}

InitLogVar FileAccess::load_launchCounter(){
    std::ifstream file(fileurl_);
    if (!file.is_open()) {
        throw std::runtime_error("couldn't open initlogfile\r\n");
    }
    InitLogVar ilv;
    std::string line;
    std::ifstream infile(fileurl_);
    std::getline(infile, line);
    int launchCounter = atoi(line.c_str());
    ilv.launchCounter = launchCounter;
    launchCounter++;
    std::getline(infile, line);
    int user_id = atoi(line.c_str());
    ilv.user_id = user_id;
    file.close();

    std::ofstream ofs(fileurl_); 
    if (!ofs) {
        throw std::runtime_error("couldn't open initlogfile\r\n");
    }
    ofs << launchCounter << "\r\n" << user_id;
    ofs.close();

    return ilv;
}