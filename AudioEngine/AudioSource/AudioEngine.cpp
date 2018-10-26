#include "AudioEngine.h"

#ifdef _DEBUG
	#include <iostream>
#endif

bool AudioEngine::s_inited = false;
FMOD::System * AudioEngine::s_system = nullptr;
std::vector<FMOD::Sound*> AudioEngine::s_sounds;
FMOD::ChannelGroup * AudioEngine::s_soundEffects;

void AudioEngine::Init()
{
	if (!s_inited)
	{
		_createSystem();
		if (s_inited)
			_createChannelGroups();
	}
	else
	{
		#ifdef _DEBUG
			std::cout << "AudioEngine is already initialized; U STOOPID!\n";
		#endif
	}
}

void AudioEngine::LoadSound(const std::string & path)
{
	FMOD::Sound * sound = nullptr;
	FMOD_RESULT result;
	result = s_system->createSound(path.c_str(), FMOD_LOOP_OFF, NULL, &sound);
	if (result == FMOD_OK)
	{
		s_sounds.push_back(sound);
	}

}

void AudioEngine::PlaySounds()
{
	for (auto & s : s_sounds)
	{
		s_system->playSound(s, s_soundEffects, false, NULL);
	}
}

void AudioEngine::Release()
{
	if (s_inited)
	{
		FMOD_RESULT result;
		result = s_system->release();
		if (result != FMOD_OK)
		{
			#ifdef _DEBUG
				std::cout << "AudioEngine error!\nError:" + std::to_string(result) + "\nMessage: " + FMOD_ErrorString(result) + "\n";
			#endif
			return;
		}
		s_soundEffects->release();
		s_inited = false;
		s_system = nullptr;
		s_sounds.clear();
		#ifdef _DEBUG
		std::cout << "AudioEngine released!\n";
		#endif
	}
	else
	{

	#ifdef _DEBUG
		std::cout << "AudioEngine is already released; U STOOPID!\n";
	#endif
	}

}

void AudioEngine::_createSystem()
{
	FMOD_RESULT result;
	result = FMOD::System_Create(&s_system);	// Create the Studio System object
	if (result != FMOD_OK)
	{
#ifdef _DEBUG
		std::cout << "AudioEngine error!\nError:" + std::to_string(result) + "\nMessage: " + FMOD_ErrorString(result) + "\n";
#endif
		return;
	}

	// Initialize FMOD Studio, which will also initialize FMOD Low Level
	result = s_system->init(512, FMOD_INIT_NORMAL, 0);	// Initialize FMOD
	if (result != FMOD_OK)
	{
#ifdef _DEBUG
		std::cout << "AudioEngine error!\nError:" + std::to_string(result) + "\nMessage: " + FMOD_ErrorString(result) + "\n";
#endif
		return;
	}

	s_inited = true;
#ifdef _DEBUG
	std::cout << "AudioEngine initialized!\n";
#endif
}

void AudioEngine::_createChannelGroups()
{
	s_system->createChannelGroup("SoundEffects", &s_soundEffects);
	s_soundEffects->setVolume(1.0f);
	// TODO :: add more

}
