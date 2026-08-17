#include "keyboard_hook.h"
#include <iostream>

namespace KeyboardHook {
    static HHOOK g_hook = NULL;
    static KeyCallback g_callback = nullptr;

    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0 && g_callback) {
            KBDLLHOOKSTRUCT* pKey = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
            
            if (isKeyDown) {
                g_callback(pKey->vkCode, true);
            }
        }
        return CallNextHookEx(g_hook, nCode, wParam, lParam);
    }

    bool InstallHook(KeyCallback callback) {
        if (g_hook != NULL) return true;
        g_callback = callback;
        g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
        return g_hook != NULL;
    }

    void UninstallHook() {
        if (g_hook != NULL) {
            UnhookWindowsHookEx(g_hook);
            g_hook = NULL;
            g_callback = nullptr;
        }
    }

    bool IsHooked() {
        return g_hook != NULL;
    }
}
