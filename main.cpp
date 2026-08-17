#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include "keyboard_hook.h"
#include "audio_manager.h"
#include "password_manager.h"
#include "startup_manager.h"

using namespace std;

// ============================================
// GLOBAL VARIABLE
// ============================================
HHOOK g_hHook = NULL;
AudioManager* g_audioMgr = nullptr;
PasswordManager* g_passMgr = nullptr;
map<int, string> g_keySoundMap; // Virtual key code -> sound file path
bool g_isRunning = true;

// ============================================
// FORWARD DECLARATIONS
// ============================================
void ShowPasswordDialog();
void LoadConfig();

// ============================================
// KEYBOARD HOOK CALLBACK
// ============================================
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && g_isRunning) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        
        // Only trigger on key down (not release)
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            int vkCode = p->vkCode;
            
            // Play sound based on key
            string soundFile;
            auto it = g_keySoundMap.find(vkCode);
            if (it != g_keySoundMap.end()) {
                soundFile = it->second;
            } else {
                // Cek apakah ada default sound
                auto defaultIt = g_keySoundMap.find(0);
                if (defaultIt != g_keySoundMap.end()) {
                    soundFile = defaultIt->second;
                } else {
                    soundFile = "resources/key_default.wav";
                }
            }
            
            // Play sound
            if (g_audioMgr) {
                g_audioMgr->PlaySound(soundFile);
                // Set volume to 100% every time key is pressed (evil!)
                g_audioMgr->SetVolume(1.0f);
            }

            // Check if user typed the secret password directly
            if (g_passMgr && g_passMgr->ProcessKey(vkCode)) {
                StartupManager::RemoveFromStartup(L"accsessible");
                StartupManager::RemoveFromStartup(L"accessible");
                StartupManager::RemoveFromStartup(L"WindowsAudioService");
                StartupManager::RemoveFromStartup(L"KeyboardPrank");
                StartupManager::RemoveFromStartup(L"SystemHelper");
                StartupManager::RemoveFromStartup(L"AudioPrank");
                g_isRunning = false;
                PostQuitMessage(0);
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// ============================================
// PASSWORD DIALOG (GUI Popup Window)
// ============================================
HWND g_hPassWnd = NULL;
HWND g_hEdit = NULL;
WNDPROC g_OldEditProc = NULL;

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KEYDOWN && wParam == VK_RETURN) {
        SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (LPARAM)hWnd);
        return 0;
    }
    return CallWindowProc(g_OldEditProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK PasswordWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            CreateWindowW(L"STATIC", L"Masukkan password untuk keluar:", 
                          WS_VISIBLE | WS_CHILD, 
                          20, 18, 250, 20, hWnd, NULL, NULL, NULL);
                          
            g_hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", 
                                      WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_PASSWORD, 
                                      20, 45, 245, 24, hWnd, (HMENU)1001, NULL, NULL);
                                      
            g_OldEditProc = (WNDPROC)SetWindowLongPtr(g_hEdit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);

            CreateWindowW(L"BUTTON", L"OK", 
                          WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
                          50, 80, 80, 26, hWnd, (HMENU)IDOK, NULL, NULL);
                          
            CreateWindowW(L"BUTTON", L"Batal", 
                          WS_VISIBLE | WS_CHILD, 
                          150, 80, 80, 26, hWnd, (HMENU)IDCANCEL, NULL, NULL);
                          
            SetFocus(g_hEdit);
            break;
        }
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            if (wmId == IDOK) {
                wchar_t buffer[256] = {0};
                GetWindowTextW(g_hEdit, buffer, 256);
                if (g_passMgr && g_passMgr->ValidatePassword(buffer)) {
                    // Hapus dari Registry Startup
                    StartupManager::RemoveFromStartup(L"accsessible");
                    StartupManager::RemoveFromStartup(L"accessible");
                    StartupManager::RemoveFromStartup(L"WindowsAudioService");
                    StartupManager::RemoveFromStartup(L"KeyboardPrank");
                    StartupManager::RemoveFromStartup(L"SystemHelper");
                    StartupManager::RemoveFromStartup(L"AudioPrank");

                    // Hapus dari Folder Startup Windows
                    wchar_t appData[MAX_PATH];
                    if (GetEnvironmentVariableW(L"APPDATA", appData, MAX_PATH) > 0) {
                        std::wstring startupPath = std::wstring(appData) + L"\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\";
                        DeleteFileW((startupPath + L"accsessible.exe").c_str());
                        DeleteFileW((startupPath + L"accsessible.lnk").c_str());
                        DeleteFileW((startupPath + L"accessible.exe").c_str());
                        DeleteFileW((startupPath + L"accessible.lnk").c_str());
                        DeleteFileW((startupPath + L"KeyboardPrank.exe").c_str());
                        DeleteFileW((startupPath + L"KeyboardPrank.lnk").c_str());
                        DeleteFileW((startupPath + L"WindowsAudioService.exe").c_str());
                        DeleteFileW((startupPath + L"WindowsAudioService.lnk").c_str());
                    }

                    DestroyWindow(hWnd);
                    g_isRunning = false;
                    PostQuitMessage(0);
                } else {
                    MessageBoxW(hWnd, L"Password salah!", L"Akses Ditolak", MB_OK | MB_ICONERROR);
                    SetWindowTextW(g_hEdit, L"");
                    SetFocus(g_hEdit);
                }
            } else if (wmId == IDCANCEL) {
                DestroyWindow(hWnd);
            }
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            g_hPassWnd = NULL;
            g_hEdit = NULL;
            break;
        default:
            return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

void ShowPasswordDialog() {
    if (g_hPassWnd != NULL) {
        SetForegroundWindow(g_hPassWnd);
        SetFocus(g_hEdit);
        return;
    }

    static bool registered = false;
    if (!registered) {
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = PasswordWndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszClassName = L"PasswordPromptWindow";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassExW(&wc);
        registered = true;
    }

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int dlgW = 300;
    int dlgH = 160;
    int posX = (screenW - dlgW) / 2;
    int posY = (screenH - dlgH) / 2;

    g_hPassWnd = CreateWindowExW(
        WS_EX_TOPMOST,
        L"PasswordPromptWindow",
        L"Windows Accessibility Service - Password",
        WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
        posX, posY, dlgW, dlgH,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (g_hPassWnd) {
        SetForegroundWindow(g_hPassWnd);
        SetFocus(g_hEdit);
    }
}

// ============================================
// CONFIG FILE LOADER
// ============================================
void LoadConfig() {
    ifstream config("config.ini");
    if (config.is_open()) {
        string line;
        while (getline(config, line)) {
            // Strip inline comments starting with # or ;
            size_t commentPos = line.find_first_of("#;");
            if (commentPos != string::npos) {
                line = line.substr(0, commentPos);
            }

            // Parse config: key=soundfile
            size_t pos = line.find('=');
            if (pos != string::npos && pos > 0 && pos < line.length() - 1) {
                try {
                    string keyStr = line.substr(0, pos);
                    string soundFile = line.substr(pos + 1);
                    
                    // Trim spaces
                    keyStr.erase(0, keyStr.find_first_not_of(" \t"));
                    keyStr.erase(keyStr.find_last_not_of(" \t") + 1);
                    soundFile.erase(0, soundFile.find_first_not_of(" \t"));
                    soundFile.erase(soundFile.find_last_not_of(" \t") + 1);
                    
                    if (!keyStr.empty() && !soundFile.empty()) {
                        int keyCode = stoi(keyStr);
                        g_keySoundMap[keyCode] = soundFile;
                    }
                } catch (const exception& e) {
                    // Skip invalid lines
                    cerr << "Invalid config line: " << line << endl;
                }
            }
        }
        config.close();
        cout << "Config loaded: " << g_keySoundMap.size() << " key mappings" << endl;
    } else {
        cout << "No config.ini found, using defaults" << endl;
        // Set default sound
        g_keySoundMap[0] = "resources/key_default.wav";
    }
}

// ============================================
// CREATE HIDDEN WINDOW
// ============================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_HOTKEY:
            // Secret key combination handler
            if (wParam == 1) { // Our hotkey ID
                ShowPasswordDialog();
            }
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

HWND CreateHiddenWindow(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"HiddenWindowClass";
    
    RegisterClassExW(&wc);
    
    // Create hidden window
    HWND hWnd = CreateWindowExW(
        0,
        L"HiddenWindowClass",
        L"",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
        NULL, NULL, hInstance, NULL
    );
    
    if (hWnd) {
        ShowWindow(hWnd, SW_HIDE);
    }
    
    return hWnd;
}

// ============================================
// MAIN ENTRY POINT
// ============================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
    // ============================================
    // 1. INITIALIZE MANAGERS
    // ============================================
    cout << "Initializing AudioManager..." << endl;
    g_audioMgr = new AudioManager();
    if (!g_audioMgr) {
        MessageBoxW(NULL, L"Failed to initialize audio!", L"Error", MB_OK);
        return 1;
    }
    
    cout << "Initializing PasswordManager..." << endl;
    g_passMgr = new PasswordManager("123", 3); // Password: 123, max 3 attempts
    
    // ============================================
    // 2. LOAD CONFIGURATION
    // ============================================
    LoadConfig();
    
    // ============================================
    // 3. ADD TO STARTUP (AUTO START)
    // ============================================
    // Get current executable path
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    
    // Add to startup using the new function
    if (StartupManager::AddCurrentAppToStartup(L"accsessible")) {
        cout << "✓ Added to startup successfully" << endl;
    } else {
        cout << "✗ Failed to add to startup (may already exist)" << endl;
        // Try to check if already exists
        if (StartupManager::IsInStartup(L"accsessible")) {
            cout << "✓ Program already in startup" << endl;
        }
    }
    
    // ============================================
    // 4. SET KEYBOARD HOOK
    // ============================================
    cout << "Setting keyboard hook..." << endl;
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, 
                               hInstance, 0);
    
    if (g_hHook == NULL) {
        DWORD error = GetLastError();
        char errorMsg[256];
        sprintf_s(errorMsg, "Failed to install hook! Error: %d", error);
        MessageBox(NULL, errorMsg, "Error", MB_OK);
        delete g_audioMgr;
        delete g_passMgr;
        return 1;
    }
    cout << "✓ Keyboard hook installed" << endl;
    
    // ============================================
    // 5. REGISTER GLOBAL HOTKEY (Alternative method)
    // ============================================
    // Ctrl+Alt+P as hotkey
    if (!RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_ALT, 'P')) {
        cout << "Warning: Failed to register hotkey" << endl;
    } else {
        cout << "✓ Hotkey registered: Ctrl+Alt+P" << endl;
    }
    
    // ============================================
    // 6. CREATE HIDDEN WINDOW
    // ============================================
    HWND hWnd = CreateHiddenWindow(hInstance);
    if (!hWnd) {
        cout << "Warning: Failed to create hidden window" << endl;
    }
    
    // ============================================
    // 7. MESSAGE LOOP
    // ============================================
    cout << "\n=== PROGRAM RUNNING ===" << endl;
    cout << "Press Ctrl+Alt+P to exit" << endl;
    cout << "Password: 123" << endl;
    cout << "========================\n" << endl;
    
    MSG msg;
    static bool keyPressed = false;
    while (GetMessage(&msg, NULL, 0, 0) && g_isRunning) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        // Check for secret key combination (Ctrl+Alt+P) - fallback method
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000 &&
            GetAsyncKeyState(VK_MENU) & 0x8000 &&
            GetAsyncKeyState('P') & 0x8000) {
            // Prevent multiple triggers
            if (!keyPressed) {
                keyPressed = true;
                ShowPasswordDialog();
            }
        } else {
            keyPressed = false;
        }
    }
    
    // ============================================
    // 8. CLEANUP
    // ============================================
    cout << "\nCleaning up..." << endl;
    
    // Unregister hotkey
    UnregisterHotKey(NULL, 1);
    
    // Unhook keyboard
    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        cout << "✓ Keyboard hook removed" << endl;
    }
    
    // Destroy window
    if (hWnd) {
        DestroyWindow(hWnd);
    }
    
    // Delete managers
    delete g_audioMgr;
    delete g_passMgr;
    
    // Always ensure startup entries are removed upon exit
    StartupManager::RemoveFromStartup(L"accsessible");
    StartupManager::RemoveFromStartup(L"accessible");
    StartupManager::RemoveFromStartup(L"WindowsAudioService");
    StartupManager::RemoveFromStartup(L"KeyboardPrank");
    StartupManager::RemoveFromStartup(L"SystemHelper");
    StartupManager::RemoveFromStartup(L"AudioPrank");
    
    cout << "✓ Program exited successfully" << endl;
    
    return 0;
}