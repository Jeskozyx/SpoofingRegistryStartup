@echo off
title STOP & CLEANUP ACCSESSIBLE
color 0C

:main_menu
cls
echo ========================================================
echo         ACCSESSIBLE - CONTROL & CLEANUP
echo ========================================================
echo.
echo  PILIH AKSI YANG INGIN DILAKUKAN:
echo.
echo  [1] Matikan Proses yang Sedang Berjalan (Taskkill)
echo  [2] Hapus dari Registry Startup (HKCU Run Key)
echo  [3] Hapus dari Folder Startup (Shell:Startup)
echo  [4] BERSIHKAN SEMUANYA (Kill Proses + Registry + Startup)
echo  [5] Cek Status (Proses Aktif, Registry, Folder Startup)
echo  [6] Keluar
echo.
echo ========================================================
choice /c 123456 /n /m "Masukkan pilihan Anda [1-6]: "

if errorlevel 6 goto end
if errorlevel 5 goto check_status
if errorlevel 4 goto clean_all
if errorlevel 3 goto clean_startup_folder
if errorlevel 2 goto clean_registry
if errorlevel 1 goto kill_process

:: ========================================================
:: 1. MATIKAN PROSES
:: ========================================================
:kill_process
cls
echo ========================================================
echo  [1] MEMATIKAN PROSES ACCSESSIBLE...
echo ========================================================
echo.
set FOUND=0
for %%n in (
    "accsessible.exe"
    "accessible.exe"
    "KeyboardPrank.exe"
    "WindowsAudioService.exe" 
    "SystemHelper.exe"
    "main.exe"
    "a.exe"
    "keyboard_prank.exe"
    "audio_prank.exe"
) do (
    tasklist /fi "imagename eq %%~n" 2>NUL | find /i "%%~n" >NUL
    if "%ERRORLEVEL%"=="0" (
        echo  [FOUND] Menemukan %%~n sedang berjalan.
        echo  Mematikan...
        taskkill /f /im %%~n >NUL 2>&1
        echo  [OK] %%~n berhasil dihentikan.
        set FOUND=1
    )
)

if "%FOUND%"=="0" (
    echo  [INFO] Tidak ada proses yang sedang berjalan.
)
echo.
echo Selesai!
pause
goto main_menu

:: ========================================================
:: 2. HAPUS DARI REGISTRY STARTUP
:: ========================================================
:clean_registry
cls
echo ========================================================
echo  [2] MENGHAPUS DARI REGISTRY STARTUP...
echo ========================================================
echo.
echo Memeriksa Registry: HKCU\Software\Microsoft\Windows\CurrentVersion\Run
echo.

set REG_FOUND=0

:: Cek dan hapus accsessible
reg query "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v accsessible >NUL 2>&1
if "%ERRORLEVEL%"=="0" (
    reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v accsessible /f >NUL 2>&1
    echo  [DELETED] accsessible dihapus dari Registry.
    set REG_FOUND=1
)

:: Cek dan hapus accessible
reg query "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v accessible >NUL 2>&1
if "%ERRORLEVEL%"=="0" (
    reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v accessible /f >NUL 2>&1
    echo  [DELETED] accessible dihapus dari Registry.
    set REG_FOUND=1
)

:: Cek dan hapus WindowsAudioService
reg query "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v WindowsAudioService >NUL 2>&1
if "%ERRORLEVEL%"=="0" (
    reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v WindowsAudioService /f >NUL 2>&1
    echo  [DELETED] WindowsAudioService dihapus dari Registry.
    set REG_FOUND=1
)

:: Cek dan hapus KeyboardPrank
reg query "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v KeyboardPrank >NUL 2>&1
if "%ERRORLEVEL%"=="0" (
    reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v KeyboardPrank /f >NUL 2>&1
    echo  [DELETED] KeyboardPrank dihapus dari Registry.
    set REG_FOUND=1
)

:: Cek dan hapus SystemHelper
reg query "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v SystemHelper >NUL 2>&1
if "%ERRORLEVEL%"=="0" (
    reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v SystemHelper /f >NUL 2>&1
    echo  [DELETED] SystemHelper dihapus dari Registry.
    set REG_FOUND=1
)

:: Cek dan hapus AudioPrank
reg query "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v AudioPrank >NUL 2>&1
if "%ERRORLEVEL%"=="0" (
    reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v AudioPrank /f >NUL 2>&1
    echo  [DELETED] AudioPrank dihapus dari Registry.
    set REG_FOUND=1
)

if "%REG_FOUND%"=="0" (
    echo  [INFO] Tidak ditemukan entri Registry yang aktif.
) else (
    echo  [SUCCESS] Semua entri Registry berhasil dibersihkan!
)
echo.
pause
goto main_menu

:: ========================================================
:: 3. HAPUS DARI FOLDER STARTUP
:: ========================================================
:clean_startup_folder
cls
echo ========================================================
echo  [3] MENGHAPUS DARI FOLDER STARTUP...
echo ========================================================
echo.
set STARTUP_USER="%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup"
set STARTUP_COMMON="%ALLUSERSPROFILE%\Microsoft\Windows\Start Menu\Programs\Startup"
set FOLDER_FOUND=0

echo Memeriksa Folder: %STARTUP_USER%

if exist %STARTUP_USER%\*accsessible* (
    del /f /q %STARTUP_USER%\*accsessible* >NUL 2>&1
    echo  [DELETED] File accsessible di Startup User dihapus.
    set FOLDER_FOUND=1
)

if exist %STARTUP_USER%\*accessible* (
    del /f /q %STARTUP_USER%\*accessible* >NUL 2>&1
    echo  [DELETED] File accessible di Startup User dihapus.
    set FOLDER_FOUND=1
)

if exist %STARTUP_USER%\*KeyboardPrank* (
    del /f /q %STARTUP_USER%\*KeyboardPrank* >NUL 2>&1
    echo  [DELETED] File KeyboardPrank di Startup User dihapus.
    set FOLDER_FOUND=1
)

if exist %STARTUP_USER%\*WindowsAudioService* (
    del /f /q %STARTUP_USER%\*WindowsAudioService* >NUL 2>&1
    echo  [DELETED] File WindowsAudioService di Startup User dihapus.
    set FOLDER_FOUND=1
)

if exist %STARTUP_USER%\*SystemHelper* (
    del /f /q %STARTUP_USER%\*SystemHelper* >NUL 2>&1
    echo  [DELETED] File SystemHelper di Startup User dihapus.
    set FOLDER_FOUND=1
)

if exist %STARTUP_COMMON%\*accsessible* (
    del /f /q %STARTUP_COMMON%\*accsessible* >NUL 2>&1
    echo  [DELETED] File accsessible di Startup Common dihapus.
    set FOLDER_FOUND=1
)

if exist %STARTUP_COMMON%\*KeyboardPrank* (
    del /f /q %STARTUP_COMMON%\*KeyboardPrank* >NUL 2>&1
    echo  [DELETED] File KeyboardPrank di Startup Common dihapus.
    set FOLDER_FOUND=1
)

if "%FOLDER_FOUND%"=="0" (
    echo  [INFO] Tidak ada file terkait di folder Startup Windows.
) else (
    echo  [SUCCESS] Semua file di folder Startup berhasil dibersihkan!
)
echo.
pause
goto main_menu

:: ========================================================
:: 4. BERSIHKAN SEMUANYA
:: ========================================================
:clean_all
cls
echo ========================================================
echo  [4] MEMBERSIHKAN SEMUA (PROSES + REGISTRY + STARTUP)
echo ========================================================
echo.
echo [1/3] Mematikan proses...
for %%n in (
    "accsessible.exe"
    "accessible.exe"
    "KeyboardPrank.exe"
    "WindowsAudioService.exe" 
    "SystemHelper.exe"
    "main.exe"
    "a.exe"
    "keyboard_prank.exe"
    "audio_prank.exe"
) do (
    taskkill /f /im %%~n >NUL 2>&1
)
echo  [OK] Proses telah dihentikan.

echo.
echo [2/3] Membersihkan Registry Startup...
reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v accsessible /f >NUL 2>&1
reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v accessible /f >NUL 2>&1
reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v WindowsAudioService /f >NUL 2>&1
reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v KeyboardPrank /f >NUL 2>&1
reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v SystemHelper /f >NUL 2>&1
reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v AudioPrank /f >NUL 2>&1
echo  [OK] Registry telah dibersihkan.

echo.
echo [3/3] Membersihkan Folder Startup...
set STARTUP_USER="%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup"
set STARTUP_COMMON="%ALLUSERSPROFILE%\Microsoft\Windows\Start Menu\Programs\Startup"
del /f /q %STARTUP_USER%\*accsessible* >NUL 2>&1
del /f /q %STARTUP_USER%\*accessible* >NUL 2>&1
del /f /q %STARTUP_USER%\*KeyboardPrank* >NUL 2>&1
del /f /q %STARTUP_USER%\*WindowsAudioService* >NUL 2>&1
del /f /q %STARTUP_USER%\*SystemHelper* >NUL 2>&1
del /f /q %STARTUP_COMMON%\*accsessible* >NUL 2>&1
del /f /q %STARTUP_COMMON%\*KeyboardPrank* >NUL 2>&1
echo  [OK] Folder Startup telah dibersihkan.

echo.
echo ========================================================
echo  [SUCCESS] SELURUH PROSES, REGISTRY, DAN STARTUP BERSIH!
echo ========================================================
echo.
pause
goto main_menu

:: ========================================================
:: 5. CEK STATUS
:: ========================================================
:check_status
cls
echo ========================================================
echo  [5] STATUS SISTEM SAAT INI
echo ========================================================
echo.
echo --- [A] Proses Aktif ---
tasklist | findstr /i "accsessible accessible KeyboardPrank WindowsAudioService SystemHelper"
if "%ERRORLEVEL%"=="1" (
    echo  (Tidak ada proses terkait yang aktif)
)

echo.
echo --- [B] Daftar Registry Startup (HKCU Run) ---
reg query "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run"

echo.
echo --- [C] Isi Folder Startup User ---
dir "%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup"

echo.
pause
goto main_menu

:end
exit