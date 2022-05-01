#pragma once
#include <string>
#include <memory>

class GameObject;
class SoundSource
{
	friend class SoundManager;
public:
	SoundSource() = delete;
	SoundSource(const GameObject& parentObject, const std::string& filename);
	~SoundSource();
	void Volume(float volume);
	void Play();
	void Stop();
	void Looping(bool isLooping);
private:
	const GameObject& m_ParentGameObject;
	uint32_t m_Source;
	uint32_t m_Sound;
};

