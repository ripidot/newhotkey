#pragma once
#include "KeyboardHookManager.hpp"
#include <string>
#include <windows.h>

class RemapActionFuncs{
    private:
    public:
        static void SendKeyboardInput(WORD key, bool keyDown);

};