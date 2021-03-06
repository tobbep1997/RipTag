#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <string>


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

	enum SoundEmitterType
	{
		NONE = 0,
		Player,
		RemotePlayer,
		Enemy,
		Other
	};
	
	struct SoundDesc
	{
		SoundEmitterType emitter = NONE;
		void * owner = nullptr;		// The owner of the sound
		float loudness = 1.0f;		// This value represents how loud the sound is
	};

private:
	static bool s_inited;

	static FMOD::System * s_system;

	static std::vector<FMOD::Sound*> s_soundEffects;
	static std::vector<FMOD::Sound*> s_ambientSounds;
	static std::vector<FMOD::Sound*> s_music;

	static std::vector<FMOD::Reverb3D*> s_reverbs;

	static FMOD::ChannelGroup* s_soundEffectGroup;
	static FMOD::ChannelGroup* s_ambientSoundGroup;
	static FMOD::ChannelGroup* s_musicSoundGroup;
	static FMOD::ChannelGroup* s_masterGroup;


public:
	static void Init();

	static void Update();
	static void UpdateListenerAttributes(const Listener & l);

	static std::string LoadSoundEffect	(const std::string & path, float minDist = 5.0f, float maxDist = 10000.0f, bool loop = false);
	static std::string LoadAmbientSound	(const std::string & path, bool loop = false);
	static std::string LoadMusicSound	(const std::string & path, bool loop = false);
	
	static void UnLoadSoundEffect	(const std::string & name);
	static void UnloadAmbiendSound	(const std::string & name);
	static void UnloadMusicSound	(const std::string & name);

	static FMOD::Channel * PlaySoundEffect(const std::string & name, FMOD_VECTOR * from = nullptr, SoundDesc * type = nullptr);
	static FMOD::Channel * PlayAmbientSound	(const std::string & name);
	static FMOD::Channel * PlayMusic		(const std::string & name);

	static void Release();

	static void SetEffectVolume(float vol);
	static void SetAmbientVolume(float vol);
	static void SetMusicVolume(float vol);
	static void SetMasterVolume(float vol);
	static float GetEffectVolume();
	static float GetAmbientVolume();
	static float GetMusicVolume();
	static float GetMasterVolume();

	static FMOD::Reverb3D* CreateReverb(FMOD_VECTOR pos, float mindist, float maxdist, FMOD_REVERB_PROPERTIES settings = FMOD_PRESET_STONEROOM);

	static FMOD::Geometry* CreateGeometry(int MAX_POLYGONS, int MAX_VERTICES);
	static FMOD::Geometry* CreateCube(float fDirectOcclusion, float fReverbOcclusion,
		DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 scl, DirectX::XMFLOAT4 q);
	static std::vector<FMOD::Channel*> getAllPlayingChannels();

private:
	static void _createSystem();
	static void _createChannelGroups();
};
