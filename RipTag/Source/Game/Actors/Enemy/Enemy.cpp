#include "Enemy.h"



Enemy::Enemy() : Actor(), CameraHolder()
{
	this->p_initCamera(new Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f, 0.1f, 50.0f));
	m_vc.Init(this->p_camera);
}


Enemy::~Enemy()
{
}

void Enemy::setDir(const float & x, const float & y, const float & z)
{
	p_camera->setDirection(x, y, z);
}

Camera * Enemy::getCamera()
{
	return p_camera;
}

const int * Enemy::getPlayerVisibility() const
{
	return m_vc.getVisibilityForPlayers();
}

void Enemy::CullingForVisability(const Transform& player)
{
	
	DirectX::XMVECTOR enemyToPlayer = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&player.getPosition()), DirectX::XMLoadFloat4A(&getPosition()));

	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(DirectX::XMLoadFloat4A(&p_camera->getDirection())), DirectX::XMVector3Normalize(enemyToPlayer)));
	float lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(enemyToPlayer));

	if (d > p_camera->getFOV() / 5.3f && lenght <= (p_camera->getFarPlane() / d) + 3)
	{
		m_allowVisability = true;
	}
	else
	{
		m_allowVisability = true;
	}
		
}

void Enemy::setPosition(const DirectX::XMFLOAT4A & pos)
{
	Transform::setPosition(pos);
	DirectX::XMFLOAT4A cPos = pos;
	cPos.y += 1;
	p_camera->setPosition(cPos);
}

void Enemy::setPosition(const float & x, const float & y, const float & z, const float & w)
{
	this->Enemy::setPosition(DirectX::XMFLOAT4A(x, y, z, w));
}

void Enemy::BeginPlay()
{
	
}

void Enemy::Update(double deltaTime)
{
	
}

void Enemy::QueueForVisibility()
{
	if (true == m_allowVisability)
	{
		m_vc.QueueVisibility();
	}
	
}