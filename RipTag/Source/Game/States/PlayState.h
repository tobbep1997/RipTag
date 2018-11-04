#pragma once
#include <future>
#include <string>
#include "State.h"

#include "Source/Physics/Bounce.h"

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


class PlayState : public State
{
private:
	LevelHandler * m_levelHandler;
	ContactListener * m_contactListener;
	RayCastListener * m_rayListener;

	PlayerManager * m_playerManager;

	b3World m_world;

	TriggerHandler *	triggerHandler;
	b3TimeStep m_step;
	bool m_firstRun = true;
	bool unlockMouse = true;
	FMOD::Channel * TEEEMPCHANNEL;


public:

	PlayState(RenderingManager * rm);
	~PlayState();

	void Update(double deltaTime) override;

	void Draw() override;

private:
	void thread(std::string s);
	void TemporaryLobby();
};

