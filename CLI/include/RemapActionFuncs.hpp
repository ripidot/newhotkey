#pragma once
#include "KeyboardHookManager.hpp"
#include <string>
#include <windows.h>

// string用のsendinput関数
namespace RemapActionFuncs { // hotkeyに対してのアクションの関数
    void SendKeyboardInput(WORD key, bool keyDown);
}