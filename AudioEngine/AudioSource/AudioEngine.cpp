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

int AudioEngine::LoadSoundEffect(const std::string & path, bool loop)
{
	int i = -1;
	FMOD::Sound * sound = nullptr;
	FMOD_RESULT result;
	result = s_system->createSound(path.c_str(), FMOD_3D_WORLDRELATIVE, NULL, &sound);
	sound->set3DMinMaxDistance(5.0f, 10000.0f);
	if (loop)
		sound->setMode(FMOD_LOOP_NORMAL);
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

FMOD::Channel * AudioEngine::PlaySoundEffect(int i, FMOD_VECTOR * from)
{
	FMOD::Channel * c;
	s_system->playSound(s_soundEffects[i], nullptr, true, &c);
	FMOD_VECTOR vel = { 0,0,0 };
	if (from)
	{
		c->set3DAttributes(from, &vel);
		c->set3DDopplerLevel(5);
		//c->set3DOcclusion(1, 1);
	}
	c->setPaused(false);
	return c;
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

int AudioEngine::TEMP_IS_THIS_POINT_INSIDE_MESH(FMOD_VECTOR POINTLOL)
{
	int lololol = 0;
	for (auto & ge : s_geometry)
	{
		FMOD_VECTOR pos;
		FMOD_VECTOR scl;
		FMOD_VECTOR forw;
		FMOD_VECTOR up;
		ge->getPosition(&pos);
		ge->getScale(&scl);
		ge->getRotation(&forw, &up);
		FMOD_VECTOR vertices[24];
		int counter = 0;
		for (int pol = 0; pol < 6; pol++)
		{
			for (int ver = 0; ver < 4; ver++)
			{
				FMOD_RESULT res = ge->getPolygonVertex(pol, ver, &vertices[counter++]);
				if (res != FMOD_OK)
				{
#ifdef _DEBUG
					std::cout << "AudioEngine error!\nError:" + std::to_string(res) + "\nMessage: " + FMOD_ErrorString(res) + "\n";
#endif
				}
			}
		}

		float ZPaneNeg = vertices[0].z * scl.z + pos.z;
		float ZPanePos = vertices[4].z * scl.z + pos.z;
		float XPaneNeg = vertices[8].x * scl.x + pos.x;
		float XPanePos = vertices[12].x * scl.x + pos.x;
		float YPaneNeg = vertices[16].y * scl.y + pos.y;
		float YPanePos = vertices[20].y * scl.y + pos.y;

		if (POINTLOL.x > XPaneNeg && POINTLOL.x < XPanePos &&
			POINTLOL.y > YPaneNeg && POINTLOL.y < YPanePos &&
			POINTLOL.z > ZPaneNeg && POINTLOL.z < ZPanePos)
		{
			return lololol;
		}

		lololol++;
	}
	return -1;
}

std::vector<FMOD::Geometry*>* AudioEngine::tmp_getAllGeometry()
{
	return &s_geometry;
}

FMOD::Geometry ** AudioEngine::CreateGeometry(int MAX_POLYGONS, int MAX_VERTICES)
{
	FMOD::Geometry * g;
	FMOD_RESULT r = s_system->createGeometry(MAX_POLYGONS, MAX_VERTICES, &g);
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
	static const FMOD_VECTOR _sCube[] =
	{
		// Side -Z
		{-1.0f, -1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},
		// Side Z
		{1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f},
		// Side -X
		{-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f},
		// Side X
		{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, 1.0f},
		// Side -Y
		{-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, -1.0f},
		// Side Y
		{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, 1.0f}
	};

	static const FMOD_VECTOR _sCube2[] =
	{
		{1, 1, -1},
		{-1, 1, -1},
		{-1, -1, -1},
		{1, -1, -1},
		{1, 1, 1},
		{-1, 1, 1},
		{-1, -1, 1},
		{1, -1, 1},
	};	

	static const FMOD_VECTOR sides[] =
	{
		// Left
		_sCube2[1], _sCube2[2], _sCube2[6], _sCube2[6],
		// Right
		_sCube2[4], _sCube2[7], _sCube2[3], _sCube2[0],
		// Down
		_sCube2[3], _sCube2[7], _sCube2[6], _sCube2[2],
		// Up
		_sCube2[0], _sCube2[1], _sCube2[5], _sCube2[4],
		// Back
		_sCube2[4], _sCube2[5], _sCube2[6], _sCube2[7],
		// Front
		_sCube2[0], _sCube2[3], _sCube2[2], _sCube2[1]
	};

	static const FMOD_VECTOR sides2[] = 
	{
		{1.000000, -1.000000, 1.000000}, {-1.000000, -1.000000, -1.000000},{1.000000, -1.000000, -1.000000},
		{-1.000000, 1.000000, -1.000000},{1.0, 1.000000, 1.000000},{1.000000, 1.000000, -1.0},
		{1.000000, 1.000000, -1.0},{1.000000, -1.000000, 1.000000},{1.000000, -1.000000, -1.000000},
		{1.0, 1.000000, 1.000000},{-1.000000, -1.000000, 1.000000},{1.000000, -1.000000, 1.000000},
		{-1.000000, -1.000000, 1.000000},{-1.000000, 1.000000, -1.000000},{-1.000000, -1.000000, -1.000000},
		{1.000000, -1.000000, -1.000000},{-1.000000, 1.000000, -1.000000},{1.000000, 1.000000, -1.0},
		{1.000000, -1.000000, 1.000000},{-1.000000, -1.000000, 1.000000},{-1.000000, -1.000000, -1.000000},
		{-1.000000, 1.000000, -1.000000},{-1.000000, 1.000000, 1.000000},{1.0, 1.000000, 1.000000},
		{1.000000, 1.000000, -1.0},{1.0, 1.000000, 1.000000},{1.000000, -1.000000, 1.000000},
		{1.0, 1.000000, 1.000000},{-1.000000, 1.000000, 1.000000},{-1.000000, -1.000000, 1.000000},
		{-1.000000, -1.000000, 1.000000},{-1.000000, 1.000000, 1.000000},{-1.000000, 1.000000, -1.000000},
		{1.000000, -1.000000, -1.000000},{-1.000000, -1.000000, -1.000000},{-1.000000, 1.000000, -1.000000}
	};


	FMOD::Geometry* ReturnValue = nullptr;
	s_system->createGeometry(12, 36, &ReturnValue);
	for (int i = 0; i < 12; i++)
	{
		FMOD_RESULT res = ReturnValue->addPolygon(fDirectOcclusion, fReverbOcclusion, true, 3, &sides2[i * 3], nullptr);
		//FMOD_RESULT res = ReturnValue->addPolygon(0.0, 0.0, true, 4, &sCubeVertices[i * 4], nullptr);
		std::cout << "AudioEngine: " + std::to_string(res) + "\nMessage: " + FMOD_ErrorString(res) + "\n";
	}
	s_geometry.push_back(ReturnValue);

	return &s_geometry[s_geometry.size() - 1];
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
	s_system->createChannelGroup("SoundEffects", &s_soundEffectGroup);
	s_system->createChannelGroup("ambientSoundGroup", &s_soundEffectGroup);
	s_system->createChannelGroup("musicSoundGroup", &s_soundEffectGroup);
	//s_soundEffectGroup->set3DAttributes(&f, NULL);
	// TODO :: add more
}
