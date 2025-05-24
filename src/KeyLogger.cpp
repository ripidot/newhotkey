// KeyLogger.cpp
#include "../include/KeyLogger.hpp"

void KeyLogger::setDBFilename(PATH initpath){
    dbpath = initpath;
}

std::string KeyLogger::return_Modifier_from_Hotkey(const Hotkey& current){
    std::vector<std::string> modifiers;
    if (current.shift) modifiers.push_back("shift");
    if (current.ctrl) modifiers.push_back("ctrl");
    if (current.alt) modifiers.push_back("alt");
    if (current.win) modifiers.push_back("win");

    std::ostringstream oss;
    for (size_t i = 0; i < modifiers.size(); ++i) { 
        if (0 < i) oss << ",";
        oss << modifiers[i];
    }
    return oss.str().c_str();
}
std::string to_utf8(const std::string& sjis_str) {
    // Shift_JIS → UTF-16
    int wide_len = MultiByteToWideChar(CP_ACP, 0, sjis_str.c_str(), -1, nullptr, 0);
    std::wstring wide_str(wide_len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, sjis_str.c_str(), -1, wide_str.data(), wide_len);

    // UTF-16 → UTF-8
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8_str(utf8_len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), -1, utf8_str.data(), utf8_len, nullptr, nullptr);

    return utf8_str;
}

std::string wstring_to_utf8(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

void KeyLogger::memory(const KeyLog& keylog) {
    sqlite3* db;
    char* errMsg = nullptr;

    // データベース接続
    if (sqlite3_open(dbpath.string().c_str(), &db)) {
        debug_log(LogLevel::Error, "couldn't open db file: ", sqlite3_errmsg(db));
        return;
    }

    // テーブルが存在しないとき作成
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS keylogs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            session_id TEXT,
            sequence_id INT,
            timestamp TEXT,
            key TEXT,
            modifiers TEXT,
            window_title TEXT,
            process_name TEXT
        );
    )";
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        debug_log(LogLevel::Error, "couldn't create a table ", errMsg);
        sqlite3_free(errMsg);
    }

    std::ostringstream setoss;
    std::ostringstream timeoss;

    tm time = keylog.local_time;

    setoss << StringUtils::pad(launchCounter, 3) << "_" << 1900 + time.tm_year << StringUtils::pad(1 + time.tm_mon) << StringUtils::pad(time.tm_mday)
        << "T" << StringUtils::pad(time.tm_hour) << StringUtils::pad(time.tm_min)
        << "_" << rand;
    timeoss << 1900 + time.tm_year << "-" << StringUtils::pad(1 + time.tm_mon) << "-" << StringUtils::pad(time.tm_mday)<< " " 
    << StringUtils::pad(time.tm_hour) << ":" << StringUtils::pad(time.tm_min) << ":" << StringUtils::pad(time.tm_sec);

    std::string session_id = setoss.str().c_str();
    std::string sequence_id = std::to_string(++sequence_counter);
    std::string timestamp = timeoss.str().c_str();
    std::string key = keylog.keyname;
    std::string modifiers = return_Modifier_from_Hotkey(keylog.current);
    std::string window_title = wstring_to_utf8(keylog.window_title);
    std::string process_name = wstring_to_utf8(keylog.processname);

    std::string insertSQL = "INSERT INTO keylogs (session_id, sequence_id, timestamp, key, modifiers, window_title, process_name) VALUES ('" +
        session_id + "', '" + sequence_id + "', '" + timestamp + "', '" + key + "', '" + modifiers + "', '" + window_title + "', '" + process_name + "');";

    if (sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        debug_log(LogLevel::Error, "Insertion Error ", errMsg);
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
    return;
}

void KeyLogger::setLaunchCounter(int lcounter){
    launchCounter = lcounter;
}
void KeyLogger::setRand(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(rmin, rmax);
    rand = dis(gen);
}