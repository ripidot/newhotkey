#pragma once
#include <string>
#include <locale>
#include <codecvt>
#include "Types.hpp"
namespace StringUtils {
    ParsedLine parse_line(const std::string& line);
}