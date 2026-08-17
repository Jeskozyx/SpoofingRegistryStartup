@echo off
title Compiling accsessible.exe
color 0A

echo ========================================
echo     COMPILING ACCSESSIBLE (STANDALONE)
echo ========================================
echo.

echo [1/4] Compiling Windows Resources (Embedding Audio)...
windres resources.rc -O coff -o resources.o
if errorlevel 1 (
    echo [WARN] windres gagal, kompilasi tanpa embedded resources...
    set RES_OBJ=
) else (
    echo [OK] Audio berhasil ditanam ke dalam file EXE!
    set RES_OBJ=resources.o
)

echo.
echo [2/4] Compiling Standalone EXE (-mwindows -static)...
g++ -mwindows main.cpp audio_manager.cpp startup_manager.cpp %RES_OBJ% -o accsessible.exe -lwinmm -lole32 -luser32 -lgdi32 -std=c++17 -s -Os -static -fno-rtti -fomit-frame-pointer

if errorlevel 1 (
    echo.
    echo [ERROR] Compile dengan static gagal!
    echo Mencoba compile tanpa static...
    g++ -mwindows main.cpp audio_manager.cpp startup_manager.cpp %RES_OBJ% -o accsessible.exe -lwinmm -lole32 -luser32 -lgdi32 -std=c++17 -s -Os -fno-rtti -fomit-frame-pointer
)

if errorlevel 1 (
    echo.
    echo [ERROR] Compile gagal lagi!
    echo Mencoba compile basic...
    g++ -mwindows main.cpp audio_manager.cpp startup_manager.cpp %RES_OBJ% -o accsessible.exe -lwinmm -lole32 -luser32 -lgdi32 -std=c++17
)

if errorlevel 1 (
    echo.
    echo [ERROR] Compile gagal total!
    color 0C
    pause
    exit /b 1
)

echo.
echo [3/4] Optimasi UPX...
where upx >nul 2>nul
if %errorlevel% equ 0 (
    echo     UPX ditemukan, mengompres file...
    upx --best accsessible.exe >nul 2>nul
    if %errorlevel% equ 0 (
        echo     [OK] Kompresi berhasil!
    ) else (
        echo     [WARN] Kompresi gagal, file tetap original
    )
) else (
    echo     UPX tidak ditemukan, skip kompresi
)

echo.
echo [4/4] Verifikasi file...
if not exist accsessible.exe goto fail_file

echo.
echo ========================================
echo  [SUCCESS] COMPILE BERHASIL!
echo ========================================
echo.
echo  File: accsessible.exe
echo  Ukuran: 
dir accsessible.exe | find "accsessible.exe"
echo.
echo  [INFO] Keunggulan Portable Standalone:
echo  - Suara audio SUDAH TERTANAM di dalam .exe
echo  - Bisa dikirim 1 file .exe saja via WA / Telegram
echo  - Jalan di laptop mana saja tanpa folder resources
echo  - Tanpa jendela CMD (background stealth mode)
echo.
echo ========================================
goto end_script

:fail_file
echo     [ERROR] File tidak ditemukan!
color 0C

:end_script
echo.
pause