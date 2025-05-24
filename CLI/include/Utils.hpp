#pragma once
#include <string>
#include <locale>
#include <codecvt>
#include <sstream>
#include <iomanip>
#include "Types.hpp"
namespace StringUtils {
    ParsedLine parse_line(const std::string& line);
    std::string pad(int value, int digits = 2);
}