#include "AudioEngine.h"

#ifdef _DEBUG
	#include <iostream>
#endif

bool AudioEngine::s_inited = false;

FMOD::System * AudioEngine::s_system = nullptr;

std::vector<FMOD::Sound*> AudioEngine::s_soundEffects;
std::vector<FMOD::Sound*> AudioEngine::s_ambientSounds;
std::vector<FMOD::Sound*> AudioEngine::s_music;

std::vector<FMOD::Reverb3D*> AudioEngine::s_reverbs;
std::vector<FMOD::Geometry*> AudioEngine::s_geometry;


FMOD::ChannelGroup * AudioEngine::s_soundEffectGroup;
FMOD::ChannelGroup * AudioEngine::s_ambientSoundGroup;
FMOD::ChannelGroup * AudioEngine::s_musicSoundGroup;

void AudioEngine::Init()
{
	if (!s_inited)
	{
		_createSystem();
		if (s_inited)
		{
			s_system->setGeometrySettings(100.0f);
			_createChannelGroups();
		}
	}
	else
	{
		#ifdef _DEBUG
			std::cout << "AudioEngine is already initialized; U STOOPID!\n";
		#endif
	}
}

void AudioEngine::Update()
{
	s_system->update();
}

void AudioEngine::UpdateListenerAttributes(const Listener & l)
{
	s_system->set3DListenerAttributes(0, &l.pos, &l.vel, &l.forward, &l.up);
}

int AudioEngine::LoadSoundEffect(const std::string & path)
{
	int i = -1;
	FMOD::Sound * sound = nullptr;
	FMOD_RESULT result;
	result = s_system->createSound(path.c_str(), FMOD_3D_WORLDRELATIVE, NULL, &sound);
	if (result == FMOD_OK)
	{
		s_soundEffects.push_back(sound);
		i = s_soundEffects.size() - 1;
	}
	return i;
}

int AudioEngine::LoadAmbientSound(const std::string & path, bool loop)
{
	int i = -1;
	FMOD::Sound * sound = nullptr;
	FMOD_RESULT result;

	int useLoop = FMOD_LOOP_NORMAL;

	if (!loop)
		useLoop = FMOD_LOOP_OFF;

	result = s_system->createSound(path.c_str(), useLoop, NULL, &sound);
	if (result == FMOD_OK)
	{
		s_ambientSounds.push_back(sound);
		i = s_ambientSounds.size() - 1;
	}
	return i;
}

int AudioEngine::LoadMusicSound(const std::string & path, bool loop)
{
	int i = -1;
	FMOD::Sound * sound = nullptr;
	FMOD_RESULT result;

	int useLoop = FMOD_LOOP_NORMAL;

	if (!loop)
		useLoop = FMOD_LOOP_OFF;

	result = s_system->createSound(path.c_str(), useLoop, NULL, &sound);
	if (result == FMOD_OK)
	{
		s_music.push_back(sound);
		i = s_music.size() - 1;
	}
	return i;
}

void AudioEngine::PlaySoundEffect(int i)
{
	FMOD::Channel * c;
	s_system->playSound(s_soundEffects[i], nullptr, false, &c);
	FMOD_VECTOR f = { -24.324, 0.83049, -1.01608 };
	FMOD_VECTOR v = { 0,0,1 };
	c->set3DAttributes(&f, NULL);
	//c->set3DSpread(360);
}

void AudioEngine::PlayAmbientSound(int i)
{
	s_system->playSound(s_ambientSounds[i], s_ambientSoundGroup, false, NULL);
}

void AudioEngine::PlayMusic(int i)
{
	s_system->playSound(s_ambientSounds[i], s_musicSoundGroup, false, NULL);
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
		s_soundEffectGroup->release();
		s_ambientSoundGroup->release();
		s_musicSoundGroup->release();

		/*for (auto & p : s_soundEffects)
			p->release();
		for (auto & p : s_ambientSounds)
			p->release();
		for (auto & p : s_music)
			p->release();
		for (auto & p : s_reverbs)
			p->release();*/


		s_soundEffectGroup = nullptr;
		s_ambientSoundGroup = nullptr;
		s_musicSoundGroup = nullptr;
		s_inited = false;
		s_system = nullptr;
		s_geometry.clear();
		s_soundEffects.clear();
		s_ambientSounds.clear();
		s_music.clear();
		s_reverbs.clear();

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

void AudioEngine::SetEffectVolume(float vol)
{
	s_soundEffectGroup->setVolume(vol);
}

void AudioEngine::SetAmbientVolume(float vol)
{
	s_ambientSoundGroup->setVolume(vol);
}

void AudioEngine::SetMusicVolume(float vol)
{
	s_musicSoundGroup->setVolume(vol);
}

void AudioEngine::ReleaseGeometry()
{
	for (auto & g : s_geometry)
		g->release();
	s_geometry.clear();
}

int AudioEngine::TEMPGETSIZEOFGEOMETRYVECTOR()
{
	return s_geometry.size();
}

void AudioEngine::CreateReverb(FMOD_VECTOR pos, float mindist, float maxdist)
{
	FMOD::Reverb3D * r;
	FMOD_RESULT result = s_system->createReverb3D(&r);
	FMOD_REVERB_PROPERTIES properties = FMOD_PRESET_CAVE;
	r->setProperties(&properties);

	r->set3DAttributes(&pos, mindist, maxdist);
	s_reverbs.push_back(r);
}

FMOD::Geometry ** AudioEngine::CreateGeometry()
{
	FMOD::Geometry * g;
	FMOD_RESULT r = s_system->createGeometry(12, 36, &g);
	s_geometry.push_back(g);
	g->setActive(true);
	return &s_geometry.at(s_geometry.size() - 1 );
}

FMOD::Geometry ** AudioEngine::CreateCube(float fDirectOcclusion, float fReverbOcclusion)
{
	static const FMOD_VECTOR sCubeVertices[] =
	{
		// Pane #1
		{-1.0f, -1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},
		// Pane #2
		{-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f},
		// Pane #3
		{1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, 1.0f},
		// Pane #4
		{-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f},
		// Pane #5
		{-1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f},
		// Pane #6
		{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f},
		{1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f}
	};
	FMOD::Geometry* ReturnValue = nullptr;
	s_system->createGeometry(6, 24, &ReturnValue);
	for (int i = 0; i < 6; i++)
	{
		FMOD_RESULT res = ReturnValue->addPolygon(fDirectOcclusion, fReverbOcclusion, true, 4, &sCubeVertices[i * 4], nullptr);
		std::cout << "AudioEngine: " + std::to_string(res) + "\nMessage: " + FMOD_ErrorString(res) + "\n";
	}
	s_geometry.push_back(ReturnValue);

	return &s_geometry[s_geometry.size() - 1];
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
	s_system->createChannelGroup("SoundEffects", &s_soundEffectGroup);
	s_system->createChannelGroup("ambientSoundGroup", &s_soundEffectGroup);
	s_system->createChannelGroup("musicSoundGroup", &s_soundEffectGroup);
	//s_soundEffectGroup->set3DAttributes(&f, NULL);
	// TODO :: add more
}
