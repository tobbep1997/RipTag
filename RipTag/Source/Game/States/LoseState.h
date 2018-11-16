#pragma once
#include "State.h"

class RenderingManager;
class Quad;
class LoseState :
	public State
{
private:
	Quad * m_gameOver;
	Quad * m_eventInfo;
	Quad * m_backToMenu;
	Quad * m_backGround;

	std::string m_eventString = "";

public:
	LoseState(RenderingManager * rm, std::string eventString = "");
	~LoseState();

	void Update(double deltaTime);

	void Draw() override;

	// Inherited via State
	virtual void Load() override;
	virtual void unLoad() override;

private:
	void _initButtons();
};

