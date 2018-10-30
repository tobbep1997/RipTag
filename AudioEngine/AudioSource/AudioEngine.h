#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <string>
#include <vector>
#include <DirectXMath.h>

class AudioEngine
{
public:
	struct Listener
	{
		FMOD_VECTOR pos;
		FMOD_VECTOR vel;		// Units per second! == (pos-lastpos / dt)
		FMOD_VECTOR forward; // Must be normalized
		FMOD_VECTOR up;		// Must be normalized
	};
private:
	static bool s_inited;

	static FMOD::System * s_system;

	static std::vector<FMOD::Sound*> s_soundEffects;
	static std::vector<FMOD::Sound*> s_ambientSounds;
	static std::vector<FMOD::Sound*> s_music;

	static std::vector<FMOD::Reverb3D*> s_reverbs;

	static std::vector<FMOD::Geometry*> s_geometry;

	static FMOD::ChannelGroup* s_soundEffectGroup;
	static FMOD::ChannelGroup* s_ambientSoundGroup;
	static FMOD::ChannelGroup* s_musicSoundGroup;

public:
	static void Init();

	static void Update();
	static void UpdateListenerAttributes(const Listener & l);

	static int LoadSoundEffect(const std::string & path);
	static int LoadAmbientSound(const std::string & path, bool loop = true);
	static int LoadMusicSound(const std::string & path, bool loop = true);

	static void PlaySoundEffect(int i);
	static void PlayAmbientSound(int i);
	static void PlayMusic(int i);

	static void StopSoundEffect(int i);
	static void StopAmbientSound(int i);
	static void StopMusic(int i);

	static void Release();

	static void SetEffectVolume(float vol);
	static void SetAmbientVolume(float vol);
	static void SetMusicVolume(float vol);

	static void ReleaseGeometry();
	
	static int TEMPGETSIZEOFGEOMETRYVECTOR();

	static void CreateReverb(FMOD_VECTOR pos, float mindist, float maxdist);

	static FMOD::Geometry** CreateGeometry();
	static FMOD::Geometry** CreateCube(float fDirectOcclusion = 1.0f, float fReverbOcclusion = 1.0f);

private:
	static void _createSystem();
	static void _createChannelGroups();
};
