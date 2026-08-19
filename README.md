# SpoofingRegistryStartup

A lightweight C++ Windows application for keyboard audio prank effects with a safety exit password and auto-start registry capability.

## 📁 Struktur Proyek

```
SpoofingRegistryStartup/
├── CMakeLists.txt           # File konfigurasi build CMake
├── main.cpp                 # Entry point & Windows message loop
├── keyboard_hook.h/.cpp     # Global low-level keyboard hook (WH_KEYBOARD_LL)
├── audio_manager.h/.cpp     # Pengatur volume sistem & pemutar suara (WinMM / Core Audio)
├── password_manager.h/.cpp  # Sistem password untuk menghentikan aplikasi (default: "stop")
├── startup_manager.h/.cpp   # Registry manager untuk auto-start saat Windows booting
├── resource.h               # Definisi resource ID
└── resources/               # Folder untuk file suara (.wav)
    ├── key_default.wav      # Suara default tombol
    └── key_a.wav            # Contoh suara khusus tombol 'A'
```

## 🛠️ Cara Kompilasi

### 1. Menggunakan Visual Studio
1. Buka folder `SpoofingRegistryStartup` di Visual Studio (`Open Folder`).
2. Visual Studio akan otomatis mendeteksi `CMakeLists.txt`.
3. Klik **Build** > **Build All** atau tekan `Ctrl + Shift + B`.

### 2. Menggunakan CMake CLI (MinGW / MSVC)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 3. Menggunakan G++ (MinGW Manual)
```bash
g++ -std=c++17 -O2 main.cpp keyboard_hook.cpp audio_manager.cpp password_manager.cpp startup_manager.cpp -o [name_your_exe].exe -lwinmm -lole32 -mwindows
```

## 🔑 Penggunaan & Penghentian
- Jalankan `[name_your_exe]`. Aplikasi akan berjalan di latar belakang (tanpa jendela konsol).
- Setiap tombol yang ditekan akan membunyikan suara prank.
- Untuk **menghentikan aplikasi**, ketik kata sandi rahasia: **`stop`** kapan saja di keyboard.
