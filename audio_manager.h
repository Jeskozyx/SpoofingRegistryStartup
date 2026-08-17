#pragma once
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <string>
#include <mmsystem.h>

class AudioManager {
private:
    IAudioEndpointVolume* m_volumeControl;
    
public:
    AudioManager();
    ~AudioManager();
    
    bool SetVolume(float level); // 0.0 - 1.0
    float GetVolume();
    bool PlaySound(const std::string& soundFile);
    bool PlayEmbeddedSound(int resourceId = 201);
};