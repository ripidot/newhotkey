#include <windows.h>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#define DEBUG_MODE 1
#if DEBUG_MODE
    #define DEBUG_LOG(msg) (std::cout << msg << std::endl)
#else
    #define DEBUG_LOG(msg) ((void)0)
#endif

struct Hotkey {
    WORD key;
    bool shift;
    bool ctrl;
    bool alt;
    bool win;

    bool operator==(const Hotkey& o) const {
        return key == o.key && shift == o.shift && ctrl == o.ctrl && alt == o.alt && win == o.win;
    }
    bool operator==(const std::unordered_set<int>& o) const{
        return false;
    }
};

struct HotkeyAction {
    std::function<bool()> on_press;
    std::function<bool()> on_release = nullptr;
};

// ハッシュ関数を定義（unordered_map用）
namespace std {
    template <>
    struct hash<Hotkey> {
        size_t operator()(const Hotkey& k) const {
            return ((std::hash<WORD>()(k.key)) ^
                    (std::hash<bool>()(k.shift) << 1) ^
                    (std::hash<bool>()(k.ctrl) << 2) ^
                    (std::hash<bool>()(k.alt) << 3) ^
                    (std::hash<bool>()(k.win) << 4));
        }
    };
}
std::ostream& operator<<(std::ostream& os, const Hotkey& h){
    os << "{";
    os << h.key << " " << h.shift << " " << h.ctrl << " " << h.alt << " " << h.win;
    os << "}";
    return os;
}
std::ostream& operator<<(std::ostream& os, const std::unordered_set<int>& s){
    os << "{";
    for (const auto& elem : s){
        os << elem << " ";
    }
    os << "}";
    return os;
}

std::unordered_map<WORD, std::function<bool(bool keyDown)>> single_key_map; //単キー
std::unordered_map<Hotkey, std::function<bool(bool keyDown)>> hotkey_map; //修飾キー+通常キー
std::unordered_map<WORD, bool> suppress_keys;
std::atomic<bool> suppress_input = false;

void register_single_key(WORD key, HotkeyAction hotkeyaction){
    single_key_map[key] = [hotkeyaction](bool keyDown){
        auto& action = keyDown ? hotkeyaction.on_press : hotkeyaction.on_release;
        if (action) return action();
        return false;
    };
}

void register_hotkey(WORD key, bool shift, bool ctrl, bool alt, bool win, 
    HotkeyAction hotkeyaction) {
    Hotkey hk = { key, shift, ctrl, alt , win};
    hotkey_map[hk] = [hotkeyaction](bool keyDown) -> bool {
        auto& action = keyDown ? hotkeyaction.on_press : hotkeyaction.on_release;
        if (action) return action();  // trueなら抑制したい
        return false;
    };
}
    // std::cout << std::hash<Hotkey>()(hk) << std::endl;

bool processSinglekey(WORD vk_code, bool keyDown) {
    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    bool win = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0;

    Hotkey current = { vk_code, shift, ctrl, alt, win };
    DEBUG_LOG(current);

    auto sit = single_key_map.find(vk_code);
    if (sit != single_key_map.end()) {
        DEBUG_LOG("saction");
        return sit->second(keyDown); // 登録された関数を実行
    }

    // auto it = hotkey_map.find(current);
    // if (it != hotkey_map.end()) {
    //     DEBUG_LOG("action");
    //     return it->second(keyDown); // 登録された関数を実行
    // }
    return false;
}
bool processHotkey(WORD vk_code, bool keyDown) {
    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    bool win = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0;

    Hotkey current = { vk_code, shift, ctrl, alt, win };
    DEBUG_LOG(current);

    auto it = hotkey_map.find(current);
    if (it != hotkey_map.end()) {
        DEBUG_LOG("action");
        return it->second(keyDown); // 登録された関数を実行
    }
    return false;
}

void SendKeyboardInput(WORD key, bool keyDown) {
    suppress_input = true;
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = keyDown ? 0 : KEYEVENTF_KEYUP;

    SendInput(1, &input, sizeof(INPUT));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    suppress_input = false;
}

bool fshouldSuppress(WORD vkCode, bool keyDown) {
    auto it = suppress_keys.find(vkCode);
    if (it != suppress_keys.end()) {
        return it->second;  // suppressする設定ならtrue
    }
    return false;  // デフォルトでは抑制しない
}

// キーフックの中での処理
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && !suppress_input) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        int vkCode = p->vkCode; // a->65, vkcode
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isKeyUp   = (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP);

        bool shouldSuppress;
        // キーが押された場合
        if (isKeyDown) {

            // ストリング処理（たとえば末尾に vkCode を追加）
            // processHotstring(vkCode);

            // ホットキー処理（同時押しなどをチェック）
            shouldSuppress = processSinglekey(vkCode, true);
        }
        if (isKeyUp) {
            shouldSuppress = processSinglekey(vkCode, false);
        }
        if (shouldSuppress) {
            DEBUG_LOG("suppress");
            return 1;
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);  // 通常処理
        
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void setupHotkeys() {
    register_hotkey('d', false, true, false, false, 
        HotkeyAction {
            []() -> bool {
            DEBUG_LOG("Ctrl + D pressed!");
            return true;}
        }
    );

    register_hotkey('e', true, false, false, false, 
        HotkeyAction {
            []() -> bool { 
            DEBUG_LOG("Ctrl + E pressed!"); 
            return true;}
        }
    );

    register_single_key(VK_NONCONVERT,
        HotkeyAction {
            []() -> bool { SendKeyboardInput(VK_BACK, true);
                return true;}, // backspace押下
            []() -> bool { SendKeyboardInput(VK_BACK, false);
                return true;}// backspace離す
        }
    );

    register_single_key(VK_LWIN,
        HotkeyAction {
            []() -> bool { SendKeyboardInput(VK_CONTROL, true);
                return true;}, // Lctrl押下
            []() -> bool { SendKeyboardInput(VK_CONTROL, false);
                return true;}// Lctrl離す
        }
    );

    register_single_key(VK_LCONTROL,
        HotkeyAction {
            []() -> bool { SendKeyboardInput(VK_LWIN, true);
                return true;}, // Lwin押下
            []() -> bool { SendKeyboardInput(VK_LWIN, false);
                return true;}// Lwin離す
        }
    );

    // register_hotkey(VK_LWIN, false, false, false, false, []() {
    //     SendKeyboardInput(VK_LCONTROL, true);  // Lctrl押下
    //     SendKeyboardInput(VK_LCONTROL, false); // Lctrl離す
    //     std::cout << "Lcontrol pressed" << std::endl;
    // });

    // register_hotkey(VK_CAPITAL, false, false, false, false, []() {
    //     SendKeyboardInput(244, true);  // 全角半角押下
    //     SendKeyboardInput(244, false); // 全角半角離す
    //     std::cout << "zenkaku pressed" << std::endl;
    // });

    // register_hotkey(244, false, false, false, false, []() {
    //     SendKeyboardInput(VK_CAPITAL, true);  // caps押下
    //     SendKeyboardInput(VK_CAPITAL, false); // caps離す
    //     std::cout << "caps pressed" << std::endl;
    // });
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    HHOOK hHook = NULL;
    #define HOTKEY_ID 1

    setupHotkeys();

    // キーボードフック開始
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (hHook == NULL) {
        DEBUG_LOG("Failed to install keyboard hook!");
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        switch (msg.message) {
        case WM_HOTKEY:
            if (msg.wParam == HOTKEY_ID) {
                DEBUG_LOG("hotkey is pushed");
            }
            break;
        }
    }

    // 終了時にフック解除
    UnhookWindowsHookEx(hHook);
    return 0;
}