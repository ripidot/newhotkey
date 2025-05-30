#include "../include/HotkeyAndRemapMapLoader.hpp"

HotkeyAndRemapMapLoader::HotkeyAndRemapMapLoader(){}
HotkeyAndRemapMapLoader::HotkeyAndRemapMapLoader(const PathManager& pm)
    : hotkeyfilename(pm.getHotkeyFile()), vkfilename(pm.getVkFile()),
    initlogfilename(pm.getInitlogFile()), dbfilename(pm.getDbFile()),
    errorfilename(pm.getErrorFile()){}

VKMap HotkeyAndRemapMapLoader::vkmap;
HRKMap HotkeyAndRemapMapLoader::hrkmap;
SupMap HotkeyAndRemapMapLoader::supmap;

std::unordered_map<WORD, bool>* HotkeyAndRemapMapLoader::skeys_getter(){
    return &supmap.suppress_keys;
}
SupMap HotkeyAndRemapMapLoader::getSupMap(){
    return supmap;
};
void HotkeyAndRemapMapLoader::setHRKMap(const HRKMap& hrkmap){
};
void HotkeyAndRemapMapLoader::load(){
    keymaploader.load(vkfilename);
    vkmap = std::move(keymaploader.getvkmap());
    HRKMapFileSource hrkmapfile(hotkeyfilename);
    hrkmapfile.setvkmap(vkmap);
    hrkmapfile.load();
    hrkmap = std::move(hrkmapfile.getHRKMap());
    supmap = std::move(hrkmapfile.getSupMap());
    

    keylogger.setRand();
    keylogger.setDBFilename(dbfilename);

    fileaccess.set_filename(initlogfilename);
    int lcounter = fileaccess.load_launchCounter();
    keylogger.setLaunchCounter(lcounter);
    keylogger.setErrorfilename(errorfilename);

}
void HotkeyAndRemapMapLoader::simulateTextInput(const std::wstring& text) {
    for (wchar_t c : text) {
        INPUT input[2] = {};
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wScan = c;
        input[0].ki.dwFlags = KEYEVENTF_UNICODE;

        input[1] = input[0];
        input[1].ki.dwFlags |= KEYEVENTF_KEYUP;

        SendInput(2, input, sizeof(INPUT));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
void HotkeyAndRemapMapLoader::inputToBuffer(WORD vkCode){
    if ((vkCode >= 'A' && vkCode <= 'Z') || (vkCode >= '0' && vkCode <= '9')) {
        WORD ch = vkCode;
        if (GetKeyState(VK_SHIFT) >= 0) {
            ch = towlower(ch); // 小文字化
        }
        inputBuffer += ch;

    }
    if (vkCode == VK_BACK && !inputBuffer.empty()) {
        inputBuffer.pop_back();
    }
}
// アクションの実行
void HotkeyAndRemapMapLoader::execute_action(ProcessType p, WORD vk_code, const Hotkey& current, bool keyDown){
    switch(p){
        case ProcessType::Remap:{
            auto sit = hrkmap.remap_map.find(vk_code); // 見つからなければremap.end()を返す
            if (sit != hrkmap.remap_map.end()) { // 見つかったら
                vk_code = sit->second(keyDown); // 登録された関数を実行
            }

            if (keyDown) { // キーロガー機能
                auto now = std::chrono::system_clock::now();
                auto time = std::chrono::system_clock::to_time_t(now);
                auto local_time = *std::localtime(&time);

                HWND hwnd = GetForegroundWindow();
                DWORD pid;
                GetWindowThreadProcessId(hwnd, &pid);
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
                wchar_t processName[MAX_PATH];
                GetModuleBaseNameW(hProcess, nullptr, processName, MAX_PATH);
                std::wstring stringProcessName = processName;

                wchar_t title[256];
                std::wstring window_title;
                window_title = GetWindowTextW(hwnd, title, sizeof(title)) ? title : L"couldn't get title";

                std::string keyname = keymaploader.vk_to_key_string(vk_code);
                debug_log(LogLevel::LogInfo, "vk_code: ", vk_code);
                debug_log(LogLevel::LogInfo, "keyname: ", keyname);
                KeyLog keylog = {local_time, current, keyname,
                    keyDown, stringProcessName, window_title};
                keylogger.memory(keylog);
            }
            break;
        }
        case ProcessType::Hotkey:{
            auto it = hrkmap.hotkey_map.find(current);
            if (it != hrkmap.hotkey_map.end()) {
                it->second(keyDown); // 登録された関数を実行
            }
            break;
        }
        case ProcessType::HotString:{
            if (!keyDown){ // キーストリング処理
                WORD vkCode = current.key;
                inputToBuffer(vkCode);
                for (const auto& [trigger, replacement] : hrkmap.hotstring_map) { //hotstrings wo wstring ni
                    if (inputBuffer.size() >= trigger.size() &&
                        inputBuffer.substr(inputBuffer.size() - trigger.size()) == trigger) {

                        // バックスペース送信
                        for (size_t i = 0; i < trigger.size(); ++i) {
                            keybd_event(VK_BACK, 0, 0, 0);
                            keybd_event(VK_BACK, 0, KEYEVENTF_KEYUP, 0);
                        }

                        // 置換文字列送信
                        replacement();
                        inputBuffer.clear();

                        return;
                    }
                }
            }
            break;
        }
    }
}

void HotkeyAndRemapMapLoader::execute(WORD vk_code, bool keyDown) { // actionを実行
    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    bool win = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0;

    Hotkey current = { vk_code, shift, ctrl, alt, win };

    execute_action(ProcessType::Hotkey, vk_code, current, keyDown); // hotkeyのaction
    execute_action(ProcessType::Remap, vk_code, current, keyDown); // remapのaction
    execute_action(ProcessType::HotString, vk_code, current, keyDown);
}
// テストの実行
void HotkeyAndRemapMapLoader::test_invalid_key(){
    WORD unknown = keymaploader.key_string_to_vk("FAKEKEY");
    assert(unknown == 0);
}
void HotkeyAndRemapMapLoader::run_all_tests(){
    test_invalid_key();
    debug_log(LogLevel::Info, "Passed all tests");
}