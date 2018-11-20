#pragma once
#include "../State.h"

class Camera;
class PointLight;
class Drawable;

class DBGState :
	public State
{
private:
	const int autolol = 1;
	const int autolol2 = 1;
private:
	Camera * camera;
	Drawable ** world;
	Drawable * floor;


	PointLight ** pointLights;
	float timer;
public:
	DBGState(RenderingManager * rm);
	~DBGState();

	// Inherited via State
	virtual void Load() override;
	virtual void unLoad() override;
	virtual void Update(double deltaTime) override;
	virtual void Draw() override;
};

