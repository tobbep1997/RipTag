#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <string>
#include <vector>
class AudioEngine
{
private:
	static bool s_inited;
	static FMOD::System * s_system;
	static std::vector<FMOD::Sound*> s_sounds;
	static FMOD::ChannelGroup* s_soundEffects;
public:
	static void Init();
	static void LoadSound(const std::string & path);
	static void PlaySounds();
	static void Release();

private:
	static void _createSystem();
	static void _createChannelGroups();
};
