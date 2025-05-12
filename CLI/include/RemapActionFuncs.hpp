#pragma once
#include "KeyboardHookManager.hpp"
#include <string>
#include <windows.h>
// string用のsendinput関数
void SendStringInput(const std::string& str);
namespace RemapActionFuncs { // hotkeyに対してのアクションの関数
    void SendKeyboardInput(WORD key, bool keyDown);
}