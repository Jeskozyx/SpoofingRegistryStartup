#include "startup_manager.h"
#include <iostream>

// ============================================
// FUNGSI UTAMA: Tambah ke Startup
// ============================================
bool StartupManager::AddToStartup(const std::wstring& appName, 
                                  const std::wstring& appPath) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_SET_VALUE | KEY_QUERY_VALUE,  // Tambah KEY_QUERY_VALUE untuk cek
        &hKey
    );
    
    if (result != ERROR_SUCCESS) {
        std::wcerr << L"Gagal membuka registry: " << result << std::endl;
        return false;
    }
    
    // Cek dulu apakah sudah ada (biar nggak duplikat)
    wchar_t existingPath[MAX_PATH];
    DWORD size = sizeof(existingPath);
    DWORD type = REG_SZ;
    
    result = RegQueryValueExW(hKey, appName.c_str(), NULL, &type, 
                              (LPBYTE)existingPath, &size);
    
    if (result == ERROR_SUCCESS) {
        // Sudah ada, cek apakah path-nya sama
        if (existingPath == appPath) {
            std::wcout << L"Program sudah terdaftar di startup" << std::endl;
            RegCloseKey(hKey);
            return true; // Sudah ada, anggap sukses
        }
        
        // Path berbeda, update aja
        std::wcout << L"Update registry dengan path baru" << std::endl;
    }
    
    // Set value ke registry
    result = RegSetValueExW(
        hKey, 
        appName.c_str(), 
        0, 
        REG_SZ,
        (const BYTE*)appPath.c_str(),
        (DWORD)((appPath.length() + 1) * sizeof(wchar_t))
    );
    
    RegCloseKey(hKey);
    
    if (result == ERROR_SUCCESS) {
        std::wcout << L"✓ Berhasil tambah ke startup: " << appName << std::endl;
        return true;
    } else {
        std::wcerr << L"✗ Gagal tambah ke startup: " << result << std::endl;
        return false;
    }
}

// ============================================
// FUNGSI: Hapus dari Startup
// ============================================
bool StartupManager::RemoveFromStartup(const std::wstring& appName) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_SET_VALUE,
        &hKey
    );
    
    if (result != ERROR_SUCCESS) {
        std::wcerr << L"Gagal membuka registry" << std::endl;
        return false;
    }
    
    result = RegDeleteValueW(hKey, appName.c_str());
    RegCloseKey(hKey);
    
    if (result == ERROR_SUCCESS) {
        std::wcout << L"✓ Berhasil hapus dari startup: " << appName << std::endl;
        return true;
    } else {
        std::wcerr << L"✗ Gagal hapus dari startup: " << result << std::endl;
        return false;
    }
}

// ============================================
// FUNGSI: Cek apakah ada di Startup
// ============================================
bool StartupManager::IsInStartup(const std::wstring& appName) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_QUERY_VALUE,
        &hKey
    );
    
    if (result != ERROR_SUCCESS) return false;
    
    wchar_t value[MAX_PATH];
    DWORD size = sizeof(value);
    result = RegQueryValueExW(hKey, appName.c_str(), NULL, NULL, 
                              (LPBYTE)value, &size);
    
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

// ============================================
// FUNGSI: Dapatkan semua program di Startup
// ============================================
std::vector<std::wstring> StartupManager::GetStartupPrograms() {
    std::vector<std::wstring> programs;
    HKEY hKey;
    
    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_READ,
        &hKey
    );
    
    if (result != ERROR_SUCCESS) return programs;
    
    wchar_t valueName[256];
    wchar_t valueData[MAX_PATH];
    DWORD valueNameSize;
    DWORD valueDataSize;
    DWORD type;
    DWORD index = 0;
    
    while (true) {
        valueNameSize = sizeof(valueName) / sizeof(wchar_t);
        valueDataSize = sizeof(valueData);
        
        result = RegEnumValueW(
            hKey,
            index,
            valueName,
            &valueNameSize,
            NULL,
            &type,
            (LPBYTE)valueData,
            &valueDataSize
        );
        
        if (result == ERROR_NO_MORE_ITEMS) break;
        if (result != ERROR_SUCCESS) break;
        
        programs.push_back(std::wstring(valueName));
        index++;
    }
    
    RegCloseKey(hKey);
    return programs;
}

// ============================================
// FUNGSI: Tambah program saat ini ke Startup (OTOMATIS!)
// ============================================
bool StartupManager::AddCurrentAppToStartup(const std::wstring& appName) {
    // Dapatkan path lengkap executable saat ini
    wchar_t exePath[MAX_PATH];
    DWORD pathLength = GetModuleFileNameW(NULL, exePath, MAX_PATH);
    
    if (pathLength == 0) {
        std::wcerr << L"Gagal mendapatkan path executable" << std::endl;
        return false;
    }
    
    // Tambahkan ke startup dengan path yang sudah didapat
    std::wcout << L"Menambahkan ke startup: " << exePath << std::endl;
    return AddToStartup(appName, exePath);
}