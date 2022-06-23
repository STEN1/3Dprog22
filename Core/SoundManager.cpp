#include "SoundManager.h"

#include <fstream>
#include <vector>

#include "AL/al.h"
#include "AL/alc.h"

#include "Core/Logger.h"
#include "Core/Globals.h"
#include "GameObject/GameObject.h"

void SoundManager::SetListenerPos(const glm::vec3& pos)
{
    //s_ListenerPos = pos;
    //alListenerfv(AL_POSITION, reinterpret_cast<ALfloat*>(&s_ListenerPos));
    
    //if (auto error = alGetError(); error != AL_NO_ERROR)
    //    LOG_ERROR("alListenerfv Pos error: " + std::to_string(error));
}

void SoundManager::SetListenerVel(const glm::vec3& vel)
{
    //s_ListenerVel = vel;
    //alListenerfv(AL_VELOCITY, reinterpret_cast<ALfloat*>(&s_ListenerVel));
    
    //if (auto error = alGetError(); error != AL_NO_ERROR)
    //    LOG_ERROR("alListenerfv Vel Error: " + std::to_string(error));
}

void SoundManager::SetListenerOri(const glm::vec3& dir, const glm::vec3& up)
{
    //float data[] = { dir.x, dir.y, dir.z, up.x, up.y, up.z };
    //alListenerfv(AL_ORIENTATION, data);

    //if (auto error = alGetError(); error != AL_NO_ERROR)
    //    LOG_ERROR("alListenerfv Ori Error: " + std::to_string(error));
}

void SoundManager::Init()
{
    // Initialization
    //ALCdevice* s_Device = alcOpenDevice(nullptr); // select the "preferred device"

    //if (s_Device)
    //{
    //    ALCcontext* s_Context = alcCreateContext(s_Device, nullptr);
    //    alcMakeContextCurrent(s_Context);
    //}
}

void SoundManager::Cleanup()
{
    //s_Context = alcGetCurrentContext();
    //s_Device = alcGetContextsDevice(s_Context);
    //alcMakeContextCurrent(nullptr);
    //alcDestroyContext(s_Context);
    //alcCloseDevice(s_Device);
}

void SoundManager::UpdateSources()
{
    //for (auto source : s_Sources)
    //{
    //    auto vPos = source->m_ParentGameObject.GetPosition();
    //    alSourcefv(source->m_Source, AL_POSITION, (float*)&vPos);
    //    if (auto error = alGetError(); error != AL_NO_ERROR)
    //        LOG_ERROR("alSourcefv error: " + std::to_string(error));
    //    // TODO Velocity.
    //}
}

void SoundManager::CheckForErrors()
{
//    if (auto error = alGetError(); error != AL_NO_ERROR)
//        LOG_ERROR("OpenAL error: " + std::to_string(error));
//    else
//        LOG_ERROR("No OpenAL error");
}


uint32_t SoundManager::LoadWav(const std::string& sound)
{
//    if (auto it = s_Sounds.find(sound); it != s_Sounds.end())
//        return it->second;

//    std::string path = Globals::AssetPath + sound;
//    std::ifstream soundFile(path, std::ios::binary | std::ios::in);
//    WavHeader wavHeader;
//    soundFile.read(reinterpret_cast<char*>(&wavHeader), sizeof(wavHeader));
//    std::vector<char> data(wavHeader.dataSize);
//    soundFile.read(data.data(), wavHeader.dataSize);

    uint32_t ABO{};
//    alGenBuffers(1, &ABO);
//    if (auto error = alGetError(); error != AL_NO_ERROR)
//        LOG_ERROR("alGenBuffers error: " + std::to_string(error));
//    int format{};
//    if (wavHeader.channels == 1)
//    {
//        if (wavHeader.bitsPrSample == 8)
//            format = AL_FORMAT_MONO8;
//        else if (wavHeader.bitsPrSample == 16)
//            format = AL_FORMAT_MONO16;
//    }
//    else if (wavHeader.channels == 2)
//    {
//        if (wavHeader.bitsPrSample == 8)
//            format = AL_FORMAT_STEREO8;
//        else if (wavHeader.bitsPrSample == 16)
//            format = AL_FORMAT_STEREO16;
//    }

//    alBufferData(ABO, format, data.data(), wavHeader.dataSize, wavHeader.sampleRate);
//    if (auto error = alGetError(); error != AL_NO_ERROR)
//        LOG_ERROR("alBufferData error: " + std::to_string(error));

//    s_Sounds[sound] = ABO;
    return ABO;
}

uint32_t SoundManager::GenSource(SoundSource* source)
{
    uint32_t SBO{};
//    alGenSources(1, &SBO);
//    if (auto error = alGetError(); error != AL_NO_ERROR)
//        LOG_ERROR("alGenSources error: " + std::to_string(error));
//    s_Sources.push_back(source);
    return SBO;
}

void SoundManager::DeleteSource(SoundSource* source)
{
//    if (auto it = std::find(s_Sources.begin(), s_Sources.end(), source); it != s_Sources.end())
//    {
//        alDeleteSources(1, &(*it)->m_Source);
//        s_Sources.erase(it);
//    }
}
