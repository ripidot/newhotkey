#pragma once
#include <iostream>
#include "../include/DebugPrint.hpp"

inline std::ostream& operator<<(std::ostream& os, const Hotkey& h){
    os << "{";
    os << h.key << " " << h.shift << " " << h.ctrl << " " << h.alt << " " << h.win;
    os << "}";
    return os;
}
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<WORD, bool>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem.first << " ";
    }
    os << "}";
    return os;
}
inline std::ostream& operator<<(std::ostream& os, const ParsedLineType& p){
    os << "{";
    switch(p){
        case ParsedLineType::Hotkey:
            os << "Hotkey";
            break;
        case ParsedLineType::Remap:
            os << "Remap";
            break;
        case ParsedLineType::Invalid:
            os << "Invalid";
            break;
    }
    os << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const std::unordered_set<int>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem << " ";
    }
    os << "}";
    return os;
}
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<Hotkey, std::function<bool(bool keyDown)>>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem.first << " ";
    }
    os << "}";
    return os;
}
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<Hotkey, bool>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem.first << " ";
    }
    os << "}";
    return os;
}
