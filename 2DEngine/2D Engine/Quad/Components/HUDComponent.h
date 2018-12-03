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
	std::vector<Circle*> m_circles;
	unsigned int m_currentTarget = 0;
	int m_nrOfElements = 0; 

public:
	HUDComponent();
	virtual ~HUDComponent();

	void InitHUDFromFile(std::string fileName); 

	virtual void AddQuad(Quad *& quad, unsigned int keyCode = 0x0);
	virtual void AddQuad(Circle *& quad, unsigned int keyCode = 0x0);
	virtual Quad * GetQuad(const unsigned short int & i);
	virtual Circle * GetCircle(const unsigned short int & i);
	virtual Quad* GetQuad(std::string tag); 

	const int& getNrOfElements() const; 

	void removeHUD(); 

	virtual void ResetStates();
	virtual void setSelectedQuad(const unsigned short int & i);

	virtual void HUDUpdate(double deltaTime);
	virtual void HUDDraw();
};

