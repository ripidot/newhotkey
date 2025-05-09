#pragma once
#include "Types.hpp"
#include <windows.h>
#include <sstream>
#include <chrono>
#include <iostream>
#include <thread>
#include <iomanip>  // 時間のフォーマットに使う

inline std::ostream& operator<<(std::ostream& os, const Hotkey& h);
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<WORD, bool>& s);
inline std::ostream& operator<<(std::ostream& os, const ParsedLineType& p);
inline std::ostream& operator<<(std::ostream& os, const std::unordered_set<int>& s);
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<Hotkey, std::function<bool(bool keyDown)>>& s);
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<Hotkey, bool>& s);

template<typename... Args>
void debug_log(LogLevel level, Args&&... args);