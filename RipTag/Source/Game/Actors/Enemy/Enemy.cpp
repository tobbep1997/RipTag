#include "Enemy.h"



Enemy::Enemy()
{
	this->_createCamera();
}


Enemy::~Enemy()
{
}

void Enemy::BeginPlay()
{
}

void Enemy::Update(double deltaTime)
{
}



void Enemy::_createCamera()
{
	this->m_camera = new Camera();
}

