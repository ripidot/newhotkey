// KeyLogger.cpp
#include "../include/KeyLogger.hpp"

void KeyLogger::setDBFilename(PATH initpath){
    dbpath = initpath;
}

void KeyLogger::onKeyPress(std::string str) {
    keyBuffer += str + ",";
    if (keyBuffer.length() >= 2 * flushBufferSize){
        keyBuffer += "\r\n";
        flushBufferToFile();
        keyBuffer = "";
    }
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

void KeyLogger::memory(KeyLog* keylog) {
    sqlite3* db;
    char* errMsg = nullptr;

    // データベース接続（なければ作成される）
    if (sqlite3_open(dbpath.string().c_str(), &db)) {
        debug_log(LogLevel::Error, "couldn't open db file: ", sqlite3_errmsg(db));
        return;
    }

    // テーブル作成（なければ作成）
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS keylogs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            session_id TEXT,
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

    tm time = keylog->local_time;

    setoss << StringUtils::pad(launchCounter, 3) << "_" << 1900 + time.tm_year << StringUtils::pad(1 + time.tm_mon) << StringUtils::pad(time.tm_mday);
    timeoss << 1900 + time.tm_year << "-" << StringUtils::pad(1 + time.tm_mon) << "-" << StringUtils::pad(time.tm_mday)<< " " 
    << StringUtils::pad(time.tm_hour) << ":" << StringUtils::pad(time.tm_min) << ":" << StringUtils::pad(time.tm_sec);

    std::string session_id = setoss.str().c_str();
    std::string timestamp = timeoss.str().c_str();
    std::string key = keylog->keyname;
    std::string modifiers = return_Modifier_from_Hotkey(keylog->current);
    std::string window_title = keylog->window_title;
    std::string process_name = keylog->processname;

    std::string insertSQL = "INSERT INTO keylogs (session_id, timestamp, key, modifiers, window_title, process_name) VALUES ('" +
        session_id + "', '" + timestamp + "', '" + key + "', '" + modifiers + "', '" + window_title + "', '" + process_name + "');";

    if (sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        debug_log(LogLevel::Error, "Insertion Error ", errMsg);
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
    return;
}

void KeyLogger::flushBufferToFile(){
    debug_log(LogLevel::Info, "flush: ", keyBuffer);
    std::ofstream file;
    file.open(logfilepath, std::ios::app);  // 追記モード
    if (!file.is_open()) {
        debug_log(LogLevel::Info, "couldn't open file");
    }
    file << keyBuffer;
    file.close();
}
void KeyLogger::setLaunchCounter(int lcounter){
    launchCounter = lcounter;
}