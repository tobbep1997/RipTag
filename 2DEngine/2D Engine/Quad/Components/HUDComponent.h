#pragma once
#include <vector>

class Quad;
class HUDComponent
{
private:
	struct QUAD_OBJECT
	{
		Quad * quad;
		unsigned int keyCode = 0x0;
	};
	struct CIRCLE_OBJECT
	{
		Circle* circle;
	};

private:
	std::vector<QUAD_OBJECT*> m_quads;
	std::vector<CIRCLE_OBJECT*> m_circles; 
	unsigned int m_currentTarget = 0;
public:
	HUDComponent();
	virtual ~HUDComponent();

	void InitHUDFromFile(std::string fileName); 

	virtual void AddQuad(Quad *& quad, unsigned int keyCode = 0x0);
	virtual Quad * GetQuad(const unsigned short int & i);

	virtual void ResetStates();
	virtual void setSelectedQuad(const unsigned short int & i);

	virtual void HUDUpdate(double deltaTime);
	virtual void HUDDraw();


};

