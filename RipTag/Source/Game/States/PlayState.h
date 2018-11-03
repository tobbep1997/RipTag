#pragma once

#include "../Handlers/PlayerManager.h"
#include <future>
#include "State.h"

#include "../../Physics/Bounce.h"
#include "../Handlers/CameraHandler.h"
#include "../Handlers/LevelHandler.h"
#include "../Actors/BaseActor.h"
#include "../Actors/Enemy/Enemy.h"
#include "2D Engine/Quad/Quad.h"
#include "../../../Engine/EngineSource/3D Engine/Extern.h"


#include "../../Physics/Wrapper/ContactListener.h"

#include "../../Gameplay/Triggers/TriggerHandler.h"

#include "../../Gameplay/Objects/PressurePlate.h"
#include "../../Gameplay/Objects/Lever.h"
#include "../../Gameplay/Objects/Door.h"

class PlayState : public State
{
private:
	LevelHandler m_levelHandler;
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

