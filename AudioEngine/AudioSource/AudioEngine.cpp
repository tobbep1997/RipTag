#include "AudioEngine.h"

	#include <iostream>
#ifdef _DEBUG
#endif

bool AudioEngine::s_inited = false;

FMOD::System * AudioEngine::s_system = nullptr;

std::vector<FMOD::Sound*> AudioEngine::s_soundEffects;
std::vector<FMOD::Sound*> AudioEngine::s_ambientSounds;
std::vector<FMOD::Sound*> AudioEngine::s_music;

std::vector<FMOD::Reverb3D*> AudioEngine::s_reverbs;

FMOD::ChannelGroup * AudioEngine::s_soundEffectGroup;
FMOD::ChannelGroup * AudioEngine::s_ambientSoundGroup;
FMOD::ChannelGroup * AudioEngine::s_musicSoundGroup;
FMOD::ChannelGroup * AudioEngine::s_masterGroup;

void AudioEngine::Init()
{
	if (!s_inited)
	{
		_createSystem();
		if (s_inited)
		{
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

std::string AudioEngine::LoadSoundEffect(const std::string & path, bool loop)
{
	FMOD::Sound * sound = nullptr;
	FMOD_RESULT result;
	result = s_system->createSound(path.c_str(), FMOD_3D_WORLDRELATIVE, NULL, &sound);
	
	std::string sName= "";

	if (result == FMOD_OK)
	{
		char name[256];
		sound->getName(name, 256);
		sName = std::string(name);

		sound->set3DMinMaxDistance(5.0f, 10000.0f);
		if (loop)
			sound->setMode(FMOD_LOOP_NORMAL);
		s_soundEffects.push_back(sound);
	}
	return sName;
}

std::string AudioEngine::LoadAmbientSound(const std::string & path, bool loop)
{
	FMOD::Sound * sound = nullptr;
	FMOD_RESULT result;

	int useLoop = FMOD_LOOP_NORMAL;

	if (!loop)
		useLoop = FMOD_LOOP_OFF;

	result = s_system->createSound(path.c_str(), useLoop, NULL, &sound);

	std::string sName = "";
	if (result == FMOD_OK)
	{
		char name[256];
		sound->getName(name, 256);
		sName = std::string(name);
		s_ambientSounds.push_back(sound);
	}
	return sName;
}

std::string AudioEngine::LoadMusicSound(const std::string & path, bool loop)
{
	FMOD::Sound * sound = nullptr;
	FMOD_RESULT result;

	int useLoop = FMOD_LOOP_NORMAL;

	if (!loop)
		useLoop = FMOD_LOOP_OFF;

	result = s_system->createSound(path.c_str(), useLoop, NULL, &sound);
	std::string sName = "";
	if (result == FMOD_OK)
	{
		char name[256];
		sound->getName(name, 256);
		sName = std::string(name);
		s_music.push_back(sound);
	}
	return sName;
}

void AudioEngine::UnLoadSoundEffect(const std::string & name)
{
	int i = -1;
	int size = (int)s_soundEffects.size();
	for (int k = 0; k < size && i == -1; k++)
	{
		char c[256];
		s_soundEffects[k]->getName(c, (int)name.size() + 1);
		if (!std::strcmp(c, name.c_str()))
		{
			i = k;
		}
	}
	if (i != -1)
	{
		s_soundEffects[i]->release();
		s_soundEffects.erase(s_soundEffects.begin() + i);
	}
}

void AudioEngine::UnloadAmbiendSound(const std::string & name)
{
	int i = -1;
	int size = (int)s_ambientSounds.size();
	for (int k = 0; k < size && i == -1; k++)
	{
		char c[256];
		s_ambientSounds[k]->getName(c, (int)name.size() + 1);
		if (!std::strcmp(c, name.c_str()))
		{
			i = k;
		}
	}
	if (i != -1)
	{
		s_ambientSounds[i]->release();
		s_ambientSounds.erase(s_ambientSounds.begin() + i);
	}
}

void AudioEngine::UnloadMusicSound(const std::string & name)
{
	int i = -1;
	int size = (int)s_music.size();
	for (int k = 0; k < size && i == -1; k++)
	{
		char c[256];
		s_music[k]->getName(c, (int)name.size() + 1);
		if (!std::strcmp(c, name.c_str()))
		{
			i = k;
		}
	}
	if (i != -1)
	{
		s_music[i]->release();
		s_music.erase(s_music.begin() + i);
	}
}

FMOD::Channel * AudioEngine::PlaySoundEffect(const std::string &name, FMOD_VECTOR * from, SoundType type)
{
	int i = -1;
	int size = (int)s_soundEffects.size();
	for (int k = 0; k < size && i == -1; k++)
	{
		char c[256];
		s_soundEffects[k]->getName(c, (int)name.size() + 1);
		if (!std::strcmp(c, name.c_str()))
		{
			i = k;
		}
	}

	FMOD::Channel * c = nullptr;

	if (i != -1)
	{
		FMOD_RESULT res = s_system->playSound(s_soundEffects[i], s_soundEffectGroup, true, &c);
		if (from)
		{
			res = c->set3DAttributes(from, NULL);
			res = c->set3DDopplerLevel(5);
		}

		c->setPaused(false);

		switch (type)
		{
		case AudioEngine::NONE:
			c->setUserData((void*)&NONE_SOUND);
			break;
		case AudioEngine::Player:
			c->setUserData((void*)&PLAYER_SOUND);
			break;
		case AudioEngine::RemotePlayer:
			c->setUserData((void*)&REMOTE_SOUND);
			break;
		case AudioEngine::Other:
			c->setUserData((void*)&OTHER_SOUND);
			break;
		}
	}
	return c;
}

FMOD::Channel * AudioEngine::PlayAmbientSound(const std::string &name)
{
	int i = -1;
	int size = (int)s_ambientSounds.size();
	for (int k = 0; k < size && i == -1; k++)
	{
		char c[256];
		s_ambientSounds[k]->getName(c, (int)name.size() + 1);
		if (!std::strcmp(c, name.c_str()))
		{
			i = k;
		}
	}
	FMOD::Channel * c = nullptr;
	if (i != -1)
	{
		s_system->playSound(s_ambientSounds[i], s_ambientSoundGroup, false, &c);
	}
	return c;
}

FMOD::Channel * AudioEngine::PlayMusic(const std::string &name)
{
	int i = -1;
	int size = (int)s_music.size();
	for (int k = 0; k < size && i == -1; k++)
	{
		char c[256];
		s_music[k]->getName(c, (int)name.size() + 1);
		if (!std::strcmp(c, name.c_str()))
		{
			i = k;
		}
	}
	FMOD::Channel * c = nullptr;
	if (i != -1)
	{
		s_system->playSound(s_music[i], s_musicSoundGroup, false, &c);
	}
	return c;
}

void AudioEngine::Release()
{
	if (s_inited)
	{
		FMOD_RESULT result;
		s_soundEffectGroup->release();
		s_ambientSoundGroup->release();
		s_musicSoundGroup->release();
		s_masterGroup->release();
		s_soundEffectGroup = nullptr;
		s_ambientSoundGroup = nullptr;
		s_musicSoundGroup = nullptr;
		s_masterGroup = nullptr;
		s_inited = false;
		s_system = nullptr;

		for (auto & a : s_soundEffects)
			a->release();
		for (auto & a : s_ambientSounds)
			a->release();
		for (auto & a : s_music)
			a->release();
		for (auto & a : s_reverbs)
			a->release();
		s_soundEffects.clear();
		s_ambientSounds.clear();
		s_music.clear();
		s_reverbs.clear();

		result = s_system->release();
		s_system = nullptr;
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

void AudioEngine::SetMasterVolume(float vol)
{
	s_masterGroup->setVolume(vol);
}

void AudioEngine::CreateReverb(FMOD_VECTOR pos, float mindist, float maxdist, FMOD_REVERB_PROPERTIES settings)
{
	FMOD::Reverb3D * r;
	FMOD_RESULT result = s_system->createReverb3D(&r);
	FMOD_REVERB_PROPERTIES properties = settings;
	r->setProperties(&properties);

	r->set3DAttributes(&pos, mindist, maxdist);
	s_reverbs.push_back(r);
}

FMOD::Geometry * AudioEngine::CreateGeometry(int MAX_POLYGONS, int MAX_VERTICES)
{
	FMOD::Geometry * g;
	FMOD_RESULT r = s_system->createGeometry(MAX_POLYGONS, MAX_VERTICES, &g);
	g->setActive(true);
	return g;
}

FMOD::Geometry * AudioEngine::CreateCube(float fDirectOcclusion, float fReverbOcclusion,
	DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 scl, DirectX::XMFLOAT4 q)
{
	static const DirectX::XMFLOAT4 _SXMcube[] =
	{
		{ 1.0,	-1.0,  1.0, 1.0},	{-1.0,	-1.0,	-1.0, 1.0},	{ 1.0,	-1.0,	-1.0, 1.0},
		{-1.0,	 1.0, -1.0, 1.0},	{ 1.0,	 1.0,	 1.0, 1.0},	{ 1.0,	 1.0,	-1.0, 1.0},
		{ 1.0,	 1.0, -1.0, 1.0},	{ 1.0,	-1.0,	 1.0, 1.0},	{ 1.0,	-1.0,	-1.0, 1.0},
		{ 1.0,	 1.0,  1.0, 1.0},	{-1.0,	-1.0,	 1.0, 1.0},	{ 1.0,	-1.0,	 1.0, 1.0},
		{-1.0,	-1.0,  1.0, 1.0},	{-1.0,	 1.0,	-1.0, 1.0},	{-1.0,	-1.0,	-1.0, 1.0},
		{ 1.0,	-1.0, -1.0, 1.0},	{-1.0,	 1.0,	-1.0, 1.0},	{ 1.0,	 1.0,	-1.0, 1.0},
		{ 1.0,	-1.0,  1.0, 1.0},	{-1.0,	-1.0,	 1.0, 1.0},	{-1.0,	-1.0,	-1.0, 1.0},
		{-1.0,	 1.0, -1.0, 1.0},	{-1.0,	 1.0,	 1.0, 1.0},	{ 1.0,	 1.0,	 1.0, 1.0},
		{ 1.0,	 1.0, -1.0, 1.0},	{ 1.0,	 1.0,	 1.0, 1.0},	{ 1.0,	-1.0,	 1.0, 1.0},
		{ 1.0,	 1.0,  1.0, 1.0},	{-1.0,	 1.0,	 1.0, 1.0},	{-1.0,	-1.0,	 1.0, 1.0},
		{-1.0,	-1.0,  1.0, 1.0},	{-1.0,	 1.0,	 1.0, 1.0},	{-1.0,	 1.0,	-1.0, 1.0},
		{ 1.0,	-1.0, -1.0, 1.0},	{-1.0,	-1.0,	-1.0, 1.0},	{-1.0,	 1.0,	-1.0, 1.0}
	};


	DirectX::XMMATRIX mWorld, mTranslation, mRotation, mScale;
	mTranslation = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	mRotation = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&q));
	mScale = DirectX::XMMatrixScaling(scl.x, scl.y, scl.z);

	mWorld = mScale * mRotation * mTranslation;

	DirectX::XMMatrixTranspose(mWorld);

	FMOD_VECTOR worldPosCube[36];
	for (int i = 0; i < 36; i++)
	{
		DirectX::XMVECTOR worldPos;
		worldPos = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&_SXMcube[i]), mWorld);
		DirectX::XMFLOAT4 xmWorldPos;
		DirectX::XMStoreFloat4(&xmWorldPos, worldPos);
		worldPosCube[i] = { xmWorldPos.x, xmWorldPos.y , xmWorldPos.z };
	}


	FMOD::Geometry* ReturnValue = nullptr;
	s_system->createGeometry(12, 36, &ReturnValue);
	for (int i = 0; i < 12; i++)
	{
		FMOD_RESULT res = ReturnValue->addPolygon(fDirectOcclusion, fReverbOcclusion, false, 3, &worldPosCube[i * 3], nullptr);
#ifdef _DEBUG
		/*std::cout << "AudioEngine: " + std::to_string(res) + "\nMessage: " + FMOD_ErrorString(res) + "\n";*/
#endif
	}
	
	return ReturnValue;
}

std::vector<FMOD::Channel*> AudioEngine::getAllPlayingChannels()
{
	int index = 0;
	FMOD::Channel * c = nullptr;
	std::vector<FMOD::Channel*> Channel;
	while (FMOD_OK == s_soundEffectGroup->getChannel(index++, &c))
	{
		bool isPlaying = false;
		c->isPlaying(&isPlaying);
		if (isPlaying)
			Channel.push_back(c);
	}
	index = 0;
	while (FMOD_OK == s_ambientSoundGroup->getChannel(index++, &c))
	{
		bool isPlaying = false;
		c->isPlaying(&isPlaying);
		if (isPlaying)
			Channel.push_back(c);
	}

	return Channel;
}

void AudioEngine::_createSystem()
{
	unsigned int version;
	FMOD_RESULT result;
	result = FMOD::System_Create(&s_system);	// Create the Studio System object
	if (result != FMOD_OK)
	{
#ifdef _DEBUG
		std::cout << "AudioEngine error!\nError:" + std::to_string(result) + "\nMessage: " + FMOD_ErrorString(result) + "\n";
#endif
		return;
	}

	result = s_system->getVersion(&version);

	if (version < FMOD_VERSION)
	{
		exit(-1);
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
	s_system->set3DSettings(1.0f, 1.0f, 1.0f);
	s_system->setGeometrySettings(100.0f);
#ifdef _DEBUG
	std::cout << "AudioEngine initialized!\n";
#endif
}

void AudioEngine::_createChannelGroups()
{
	s_system->createChannelGroup("Master", &s_masterGroup);
	s_masterGroup->setVolume(1.0f);
	s_system->createChannelGroup("SoundEffects", &s_soundEffectGroup);
	s_soundEffectGroup->setVolume(1.0f);
	s_system->createChannelGroup("AmbientSounds", &s_ambientSoundGroup);
	s_ambientSoundGroup->setVolume(1.0f);
	s_system->createChannelGroup("Music", &s_musicSoundGroup);
	s_musicSoundGroup->setVolume(1.0f);


	

	FMOD_RESULT res = s_masterGroup->addGroup(s_soundEffectGroup);
	res = s_masterGroup->addGroup(s_ambientSoundGroup);
	res = s_masterGroup->addGroup(s_musicSoundGroup);
}