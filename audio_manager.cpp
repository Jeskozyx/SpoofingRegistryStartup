#include "audio_manager.h"
#include "resource.h"
#include <iostream>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ole32.lib")

AudioManager::AudioManager() {
    m_volumeControl = nullptr;
    CoInitialize(NULL);
    
    HRESULT hr;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDevice* pDevice = NULL;
    
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
                          CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                          (void**)&pEnumerator);
    
    if (FAILED(hr)) {
        std::cerr << "Failed to create audio device enumerator" << std::endl;
        return;
    }
    
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    pEnumerator->Release();
    
    if (FAILED(hr)) {
        std::cerr << "Failed to get default audio endpoint" << std::endl;
        return;
    }
    
    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                           NULL, (void**)&m_volumeControl);
    pDevice->Release();
}

AudioManager::~AudioManager() {
    if (m_volumeControl) {
        m_volumeControl->Release();
    }
    CoUninitialize();
}

bool AudioManager::SetVolume(float level) {
    if (!m_volumeControl) return false;
    
    // Clamp level between 0 and 1
    if (level < 0.0f) level = 0.0f;
    if (level > 1.0f) level = 1.0f;
    
    HRESULT hr = m_volumeControl->SetMasterVolumeLevelScalar(level, NULL);
    return SUCCEEDED(hr);
}

float AudioManager::GetVolume() {
    if (!m_volumeControl) return 0.0f;
    
    float level;
    HRESULT hr = m_volumeControl->GetMasterVolumeLevelScalar(&level);
    if (FAILED(hr)) return 0.0f;
    
    return level;
}

bool AudioManager::PlayEmbeddedSound(int resourceId) {
    // Try finding WAVE resource
    HRSRC hRes = FindResourceW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(resourceId), L"WAVE");
    if (!hRes) {
        hRes = FindResourceW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(resourceId), MAKEINTRESOURCEW(10)); // RT_RCDATA
    }
    if (hRes) {
        HGLOBAL hMem = LoadResource(GetModuleHandleW(NULL), hRes);
        if (hMem) {
            void* pData = LockResource(hMem);
            if (pData) {
                return ::PlaySoundA((LPCSTR)pData, NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT) != 0;
            }
        }
    }
    return false;
}

bool AudioManager::PlaySound(const std::string& soundFile) {
    if (!soundFile.empty()) {
        std::wstring wSoundFile(soundFile.begin(), soundFile.end());
        
        // Cek apakah file fisik ada di disk
        DWORD fileAttr = GetFileAttributesW(wSoundFile.c_str());
        if (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
            // File fisik ada di disk, putar langsung
            if (::PlaySoundW(wSoundFile.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT) != 0) {
                return true;
            }
        }
    }
    
    // Jika file tidak ditemukan di disk (misal exe dipindah sendirian tanpa folder resources),
    // otomatis putar audio yang sudah tertanam (embedded) di dalam exe!
    return PlayEmbeddedSound(IDR_WAV_DEFAULT);
}