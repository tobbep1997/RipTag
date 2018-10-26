#pragma once
#include "../Quad.h"
#include <vector>

class HUDComponent
{
private:
	struct QUAD_OBJECT
	{
		Quad * quad;
		unsigned int keyCode = 0x0;
	};

private:
	std::vector<QUAD_OBJECT*> m_quads;
	unsigned int m_currentTarget = 0;
public:
	HUDComponent();
	virtual ~HUDComponent();

	virtual void AddQuad(Quad *& quad, unsigned int keyCode = 0x0);

	virtual void ResetStates();
	virtual void setSelectedQuad(const unsigned short int & i);

	virtual void HUDUpdate(double deltaTime);
	virtual void HUDDraw();


};

