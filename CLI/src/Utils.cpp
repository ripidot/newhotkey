#include "../include/Types.hpp"
#include "../include/Utils.hpp"
namespace StringUtils {
    ParsedLine parse_line(const std::string& line) {
        bool escaped = false;
        bool found_colon = false;
        bool found_arrow = false;
        bool found_Darrow = false;
        ParsedLine result;
        result.type = ParsedLineType::Invalid;
        std::string left, right;
    
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (escaped) {
                ((found_colon || found_arrow || found_Darrow) ? right : left) += c;
                escaped = false;
            } else {
                if (c == '\\') {
                    escaped = true;
                } else if (c == '#' && !found_colon && !found_arrow && !found_Darrow) {
                    break; // 非エスケープ#でコメント開始
                } else if (c == ':' && !found_colon && !found_arrow && !found_Darrow) {
                    found_colon = true;
                } else if (c == '-' && i + 1 < line.size() && line[i + 1] == '>'
                            && !found_colon && !found_arrow && !found_Darrow){
                    found_arrow = true;
                    i++;
                } else if (c == '=' && i + 1 < line.size() && line[i + 1] == '>'
                            && !found_colon && !found_arrow && !found_Darrow){
                    found_Darrow = true;
                    i++;
                }else {
                    ((found_colon || found_arrow || found_Darrow) ? right : left) += c;
                }
            }
        }
        // 空白トリム
        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t"));
            s.erase(s.find_last_not_of(" \t") + 1);
        };
        trim(left);
        trim(right);
    
        if (found_colon && !left.empty()) {
            result.type = ParsedLineType::Hotkey;
            result.key_part = left;
            result.action_part = right;
        } else if (found_arrow && !left.empty() && !right.empty()) {
            result.type = ParsedLineType::Remap;
            result.from_key = left;
            result.to_key = right;
        } else if (found_Darrow && !left.empty()) {
            result.type = ParsedLineType::Keystring;
            result.from_strkey = left;
            result.to_strkey = right;
        }
    
        return result; // r.left = "A ctrl shift", r.right = "launch_app notepad.exe"
    }
    std::string pad(int value, int digits) {
        std::ostringstream oss;
        oss << std::setw(digits) << std::setfill('0') << value;
        return oss.str();
    }

}