#include "SoundSource.h"
#include "Core/Logger.h"
#include <qdir.h>
#include "AL/al.h"
#include "AL/alc.h"
#include "SoundManager.h"

SoundSource::SoundSource(const GameObject& parentObject, const std::string& filename)
	: m_ParentGameObject{ parentObject }
{
//	m_Sound = SoundManager::LoadWav(filename);
//	m_Source = SoundManager::GenSource(this);
//	alSourcei(m_Source, AL_BUFFER, m_Sound);
//	if (auto error = alGetError(); error != AL_NO_ERROR)
//		LOG_ERROR("alSourcei source attach Error: " + std::to_string(error));
}

SoundSource::~SoundSource()
{
	SoundManager::DeleteSource(this);
}

void SoundSource::Volume(float volume)
{
//	alSourcef(m_Source, AL_GAIN, volume);
//	if (auto error = alGetError(); error != AL_NO_ERROR)
//		LOG_ERROR("alSourcef Volume error: " + std::to_string(error));
}

void SoundSource::Play()
{
//	alSourcePlay(m_Source);
//	if (auto error = alGetError(); error != AL_NO_ERROR)
//		LOG_ERROR("alSourcePlay error: " + std::to_string(error));
}

void SoundSource::Stop()
{
//	alSourceStop(m_Source);
//	if (auto error = alGetError(); error != AL_NO_ERROR)
//		LOG_ERROR("alSourceStop error: " + std::to_string(error));
}

void SoundSource::Looping(bool isLooping)
{
//	if (isLooping)
//	{
//		alSourcei(m_Source, AL_LOOPING, AL_TRUE);
//		if (auto error = alGetError(); error != AL_NO_ERROR)
//			LOG_ERROR("alSourcei Looping error: " + std::to_string(error));
//	}
//	else
//	{
//		alSourcei(m_Source, AL_LOOPING, AL_FALSE);
//		if (auto error = alGetError(); error != AL_NO_ERROR)
//			LOG_ERROR("alSourcei Looping error: " + std::to_string(error));
//	}
}
