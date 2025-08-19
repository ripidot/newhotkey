
#define NOMINMAX
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "sqlite3.h"
#include <curl/curl.h>
#include <algorithm>
#include <nlohmann/json.hpp> // https://github.com/nlohmann/json

using json = nlohmann::json;

struct KeyLog {
    int id;
    std::string session_id;
    int sequence_id;
    std::string timestamp;
    std::string key;
    std::string modifiers;
    std::string window_title;
    std::string process_name;
    int user_id;
};
struct FailedLog {
    int id;
    std::string reason;
};
std::vector<KeyLog> fetch_unsent_logs(sqlite3* db) {
    std::vector<KeyLog> logs;
    const char* sql = "SELECT id, session_id, sequence_id, timestamp, key, modifiers, window_title, process_name, user_id FROM keylogs WHERE sent_flag = 0 LIMIT 3;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
        return logs;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        KeyLog log;
        log.id = sqlite3_column_int(stmt, 0);
        log.session_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        log.sequence_id = sqlite3_column_int(stmt, 2);
        log.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        log.key = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        log.modifiers = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        log.window_title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        log.process_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        log.user_id = sqlite3_column_int(stmt, 8);
        logs.push_back(log);
    }

    sqlite3_finalize(stmt);
    return logs;
}

std::string logs_to_json(const std::vector<KeyLog>& logs) {
    json j = json::array();
    for (const auto& log : logs) {
        j.push_back({
            {"id", log.id},
            {"session_id", log.session_id},
            {"sequence_id", log.sequence_id},
            {"timestamp", log.timestamp},
            {"key", log.key},
            {"modifiers", log.modifiers},
            {"window_title", log.window_title},
            {"process_name", log.process_name},
            {"user_id", log.user_id}
        });
    }
    return j.dump();
}
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}
// libcurl で POST
bool send_logs(const std::string& url, const std::string& json_data, std::string& response_body, long& http_code) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    } else {
        http_code = 0; // http_code取得失敗
    }
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK);
}
bool delete_sent_logs(sqlite3* db, const std::vector<int>& success_ids) {
    if (success_ids.empty()) return true;

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE keylogs SET sent_flag = 1 WHERE sent_flag = 0 AND id = ?;";
    // const char* sql = "DELETE FROM keylogs WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // トランザクション処理
    char* errmsg = nullptr;
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cerr << "Failed to begin transaction: " << errmsg << std::endl;
        sqlite3_free(errmsg);
        sqlite3_finalize(stmt);
        return false;
    }
    for (int id : success_ids) {
        sqlite3_reset(stmt);
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Delete failed for ID=" << id << ": " << sqlite3_errmsg(db) << std::endl;

            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            sqlite3_finalize(stmt);
            return false;
        }
    }
    if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cerr << "Failed to commit: " << errmsg << std::endl;
        sqlite3_free(errmsg);
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

void from_json(const nlohmann::json& j, FailedLog& f) {
    j.at("id").get_to(f.id);
    j.at("reason").get_to(f.reason);
}

bool escape_invalid_logs(std::vector<KeyLog>& logs, const std::vector<FailedLog>& failed) {
    std::string DB_URL = "keylog_invalid_column.db";
    sqlite3* db;
    if (sqlite3_open(DB_URL.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Cannot open database" << std::endl;
        return 0;
    }
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO keylogs"
    "(session_id, sequence_id, timestamp, key, modifiers, window_title, process_name, user_id)"
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

    // トランザクション処理
    char* errmsg = nullptr;
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cerr << "Failed to begin transaction: " << errmsg << std::endl;
        sqlite3_free(errmsg);
        sqlite3_finalize(stmt);
        return false;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    for (FailedLog flog : failed) {
        std::cout << "flog.id : " << flog.id << std::endl;
        sqlite3_reset(stmt);
        for (auto log : logs) if (log.id == flog.id){
            sqlite3_bind_text(stmt, 1, log.session_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt,  2, log.sequence_id);
            sqlite3_bind_text(stmt, 3, log.timestamp.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, log.key.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 5, log.modifiers.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 6, log.window_title.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 7, log.process_name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt,  8, log.user_id);
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Insert failed from ID=" << flog.id << ": " << sqlite3_errmsg(db) << std::endl;
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                sqlite3_finalize(stmt);
                return false;
            }
        }

    }
    sqlite3_finalize(stmt);

    if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cerr << "Failed to commit: " << errmsg << std::endl;
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}
int main() {
    std::string DB_URL = "keylog_bac_2_10column.db";
    sqlite3* db;
    if (sqlite3_open(DB_URL.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Cannot open database" << std::endl;
        return 0;
    }

    auto logs = fetch_unsent_logs(db);
    if (logs.empty()) {
        std::cout << "No unsent logs." << std::endl;
        sqlite3_close(db);
        return 0;
    }

    std::string json_data = logs_to_json(logs);
    std::string response;
    long http_code = 0;

    if (send_logs("http://localhost:8000/keylogs/batch", json_data, response, http_code)) { // sqlite to postgreSQL
        std::cout << "Response: " << response << std::endl;
        std::cout << "Logs sent successfully!" << std::endl;
 
        auto json_resp = nlohmann::json::parse(response);
        std::vector<int> success_ids =
            json_resp.value("success_ids", nlohmann::json::array()).get<std::vector<int>>();

        std::vector<FailedLog> failed =
            json_resp.value("failed", nlohmann::json::array()).get<std::vector<FailedLog>>();

        auto success_len = json_resp["success_count"];
        std::cout << "http_code : " << http_code << std::endl;
        std::cout << success_ids.size() << std::endl;

        if (http_code == 200) { // 完全成功
            delete_sent_logs(db, success_ids);
            std::cout << "Success" << std::endl;
        } else if (http_code == 400) { // 部分成功
            delete_sent_logs(db, success_ids);
            escape_invalid_logs(logs, failed);
            std::cout << "Client error (bad request, duplicate, etc.)" << std::endl;
        } else if (http_code == 500) { // サーバーエラー
            std::cout << "Server error" << std::endl;
        }

    } else {
        std::cerr << "Failed to send logs." << std::endl;
    }

    sqlite3_close(db);
    return 0;
}
