#pragma once
#include "State.h"
#include "../Handlers/CameraHandler.h"
#include "../Handlers/LevelHandler.h"
#include "../Handlers/ObjectHandler.h"
#include "../Actors/Player.h"

class PlayState : public State
{
private:
	LevelHandler m_levelHandler;
	ObjectHandler m_objectHandler;

	Camera * m_tempCam;
	Player player;
public:
	PlayState(RenderingManager * rm);
	~PlayState();

	void Update() override;

	void Draw() override;

private:

	void _tempCameraControls();
};
