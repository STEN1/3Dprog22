#pragma once
#include "glm/vec3.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include "SoundSource.h"

class SoundManager
{
	friend class SoundSource;
public:
	static void SetListenerPos(const glm::vec3& pos);
	static void SetListenerVel(const glm::vec3& vel);
	static void SetListenerOri(const glm::vec3& dir, const glm::vec3& up);
	static void Init();
	static void Cleanup();
	static void UpdateSources();
	static void CheckForErrors();
private:
	static uint32_t LoadWav(const std::string& sound);
	struct WavHeader
	{
		char riff[4];
		uint32_t fileSize;
		char type[4];
		char formatMarker[4];
		uint32_t formatLenght;
		uint16_t typeOfFormat;
		uint16_t channels;
		uint32_t sampleRate;
		uint32_t dataRate; // (Sample Rate * BitsPerSample * Channels) / 8.
		uint16_t bitsPrSamplePrChannel;
		uint16_t bitsPrSample;
		char dataMarker[4];
		uint32_t dataSize;
	};
	inline static std::unordered_map<std::string, uint32_t> s_Sounds;

	static uint32_t GenSource(SoundSource* source);
	static void DeleteSource(SoundSource* source);
	inline static std::vector<SoundSource*> s_Sources;

	inline static class ALCdevice* s_Device{ nullptr };
	inline static class ALCcontext* s_Context{ nullptr };

	inline static glm::vec3 s_ListenerPos{};
	inline static glm::vec3 s_ListenerVel{};
	inline static glm::vec3 s_ListenerOrientation{};

	SoundManager() = default;
};
