#include <windows.h>
#include <functional>
#include <atomic>

class KeyboardHookManager {
    private:
        static inline HHOOK hHook;
        static inline std::function<void(int)> keyDownHandler;
        static inline std::function<void(int)> keyUpHandler;

    public:
        KeyboardHookManager();
        KeyboardHookManager(std::unordered_map<WORD, bool>* skeys);
        static inline std::unordered_map<WORD, bool> suppress_keys;
        static inline std::atomic<bool> suppress_input;
        static bool shouldSuppress(WORD vkCode, bool isKeyDown);
        static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
        void setSuppressKeys(std::unordered_map<WORD, bool>* skeys);
        void setKeyDownHandler(std::function<void(int)> handler);
        void setKeyUpHandler(std::function<void(int)> handler);
        void setHook();
        void removeHook();
};