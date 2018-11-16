#pragma once
#include <future>
#include <string>
#include "State.h"

#include "Source/Physics/Bounce.h"
#include "Source/Game/Actors/BaseActor.h"
#include "Source/Game/Item/Map.h"
#include "Source/Game/Item/Rock.h"

namespace FMOD
{
	class Channel;
};

class RayCastListener;
class PlayerManager;
class TriggerHandler;
class LevelHandler;
class ContactListener;
class RayCastListener;
class RenderingManager;
class Drawable; //#todoREMOVE
class ParticleEmitter;


class PlayState : public State
{
private:
	LevelHandler * m_levelHandler;		//Released
	ContactListener * m_contactListener;//Released
	RayCastListener * m_rayListener;	//Released
	PlayerManager * m_playerManager;	//Released

	b3World m_world;

	ParticleEmitter * m_particleEmitter = new ParticleEmitter();
	TriggerHandler *	triggerHandler; //Released
	b3TimeStep m_step;
	bool m_firstRun = true;
	bool unlockMouse = true;

	std::thread m_physicsThread;
	std::mutex m_physicsMutex;
	std::condition_variable m_physicsCondition;
	double m_deltaTime = 0;
	bool m_destoryPhysicsThread = false;

	static bool m_youlost;
	//BaseActor * tempp;
	//DirectX::XMFLOAT4A rot;
	bool isCoop = false;
	CoopData * pCoopData = nullptr;
	int m_seed = 0;
	float timer = 0;
public:
	PlayState(RenderingManager * rm, void * coopData = nullptr);
	~PlayState();

	void Update(double deltaTime) override;

	void Draw() override;

	static void setYouLost(const bool & youLost);

private:
	void _PhyscisThread(double deltaTime);
	void _audioAgainstGuards(double deltaTime);
	void _lightCulling();
	void thread(std::string s);
	void DrawWorldCollisionboxes(const std::string & type = "");
	// Inherited via State
	virtual void unLoad();
	virtual void Load();

	// LOAD functions
	void _loadTextures();
	void _loadPhysics();
	void _loadMeshes();
	void _loadPlayers();
	void _loadNetwork();
	void _loadSound();
	// Unload functions

};

