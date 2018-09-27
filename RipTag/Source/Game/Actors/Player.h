#pragma once
#include "Source/3D Engine/Model/Model.h"
#include "Actor.h"

class Player : public Actor
{
private:
	Model * m_model;
	Camera * m_camera;
public:
	Player();
	~Player();

	void BeginPlay() override;
	void Update() override;
	void Draw() override;

	Camera * getCamera();
private:

	void _handleInput();

};
