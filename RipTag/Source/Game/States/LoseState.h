#pragma once
#include "State.h"

class RenderingManager;
class Quad;
class LoseState :
	public State
{
private:
	Quad * m_youLost;
	Quad * m_backToMenu;

public:
	LoseState(RenderingManager * rm);
	~LoseState();

	void Update(double deltaTime);

	void Draw() override;
};

