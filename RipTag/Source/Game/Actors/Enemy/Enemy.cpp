#include "Enemy.h"



Enemy::Enemy() : Actor(), CameraHolder()
{
	
}


Enemy::~Enemy()
{
}

void Enemy::BeginPlay()
{
	this->p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, .1f,50.f));
}

void Enemy::Update(double deltaTime)
{
}



