#pragma once
#include <windows.h>
#include <string>
#include <vector>

class StartupManager {
public:
    // Tambah ke startup registry
    static bool AddToStartup(const std::wstring& appName, 
                             const std::wstring& appPath);
    
    // Hapus dari startup registry
    static bool RemoveFromStartup(const std::wstring& appName);
    
    // Cek apakah sudah terdaftar di startup
    static bool IsInStartup(const std::wstring& appName);
    
    // Dapatkan daftar semua program di startup
    static std::vector<std::wstring> GetStartupPrograms();
    
    // Tambah ke startup dengan current executable path (otomatis)
    static bool AddCurrentAppToStartup(const std::wstring& appName);
};