#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

HHOOK hHook = NULL;
std::wstring inputBuffer;

// const std::wstring trigger = L"brb";
// const std::wstring replacement = L"おつかれ";

std::unordered_map<std::wstring, std::wstring> hotstrings = {
    {L"brb", L"be right back"},
    {L"omw", L"on my way"},
    {L"ty", L"thank you"},
    {L"idk", L"I don't know"},
};


// ホットキーID
#define HOTKEY_ID 1

void simulateTextInput(const std::wstring& text) {
    for (wchar_t c : text) {
        INPUT input[2] = {};
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wScan = c;
        input[0].ki.dwFlags = KEYEVENTF_UNICODE;

        input[1] = input[0];
        input[1].ki.dwFlags |= KEYEVENTF_KEYUP;

        SendInput(2, input, sizeof(INPUT));
    }
}
// グローバルで
bool suppressNextKey = false;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vk = p->vkCode;

        if (wParam == WM_KEYDOWN) {

            // 英数字のみ
            if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9')) {
                wchar_t ch = (wchar_t)vk;
                if (GetKeyState(VK_SHIFT) >= 0) {
                    ch = towlower(ch); // 小文字化
                }
                inputBuffer += ch;

                for (const auto& [trigger, replacement] : hotstrings){

                    if (inputBuffer.size() >= trigger.size() &&
                        inputBuffer.substr(inputBuffer.size() - trigger.size()) == trigger) {

                        // バックスペース送信
                        for (size_t i = 0; i < trigger.size()-1; ++i) {
                            keybd_event(VK_BACK, 0, 0, 0);
                            keybd_event(VK_BACK, 0, KEYEVENTF_KEYUP, 0);
                            std::cout << "erase" << std::endl;
                        }

                        // 置換文字列送信
                        simulateTextInput(replacement);
                        inputBuffer.clear();

                        // return to not input last word of hotstring 
                        return 1;
                    }
                }
            }

            if (vk == VK_BACK && !inputBuffer.empty()) {
                inputBuffer.pop_back();
            }
        }
    }
    // std::cout << "return call" << std::endl;
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    // ホットキー登録: Ctrl + Shift + K
    RegisterHotKey(NULL, HOTKEY_ID, MOD_CONTROL | MOD_SHIFT, 'K');

    // キーボードフック開始
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        switch (msg.message) {
        case WM_HOTKEY:
            if (msg.wParam == HOTKEY_ID) {
                std::cout << "hotkey is pushed" << std::endl;
            }
            break;
        }
    }

    // 終了時にフック解除
    UnhookWindowsHookEx(hHook);
    return 0;
}