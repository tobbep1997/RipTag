#include "Enemy.h"
#include "../RipTag/Source/Input/Input.h"


Enemy::Enemy() : Actor(), CameraHolder()
{
	this->p_initCamera(new Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f, 0.1f, 50.0f));
	m_vc.Init(this->p_camera);

}

Enemy::Enemy(float startPosX, float startPosY, float startPosZ)
{
	this->p_initCamera(new Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f, 0.1f, 50.0f));
	m_vc.Init(this->p_camera);
	this->setPosition(startPosX, startPosY, startPosZ);
	this->setDir(1, 0, 0);
	this->getCamera()->setFarPlane(5);
	this->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	this->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
}

Enemy::Enemy(b3World* world, float startPosX, float startPosY, float startPosZ)
{
	this->p_initCamera(new Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f, 0.1f, 50.0f));
	m_vc.Init(this->p_camera);
	this->setDir(1, 0, 0);
	this->getCamera()->setFarPlane(5);
	this->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	this->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	this->Init(*world, e_staticBody);
	this->getBody()->SetUserData((void*)2);
	this->setEntityType(EntityType::GuarddType);
	this->setPosition(startPosX, startPosY, startPosZ);
}


Enemy::~Enemy()
{
	this->Release(*this->getBody()->GetScene());
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

	if (d > p_camera->getFOV() / 3.14f && lenght <= (p_camera->getFarPlane() / d) + 3)
	{
		m_allowVisability = true;
	}
	else
	{
		m_allowVisability = false;
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
	PhysicsComponent::p_setPosition(x, y, z);
}

void Enemy::BeginPlay()
{
	
}

void Enemy::Update(double deltaTime)
{
	if (!m_inputLocked)
	{
		_handleInput(deltaTime);
	}
	else
	{
		_TempGuardPath(true,deltaTime);
	}
	
}

void Enemy::PhysicsUpdate(double deltaTime)
{
	p_updatePhysics(this);
}

void Enemy::QueueForVisibility()
{
	if (true == m_allowVisability)
	{
		m_vc.QueueVisibility();
	}
	
}

void Enemy::LockEnemyInput()
{
	m_inputLocked = true;
}

void Enemy::UnlockEnemyInput()
{
	m_inputLocked = false;
}

void Enemy::_handleInput(double deltaTime)
{
	_handleMovement(deltaTime);
	_handleRotation(deltaTime);
	
}

void Enemy::_handleMovement(double deltaTime)
{
	using namespace DirectX;

	XMFLOAT4A forward = p_camera->getDirection();
	XMFLOAT4 UP = XMFLOAT4(0, 1, 0, 0);
	XMFLOAT4 RIGHT;
	//GeT_RiGhT;

	XMVECTOR vForward = XMLoadFloat4A(&forward);
	XMVECTOR vUP = XMLoadFloat4(&UP);
	XMVECTOR vRight;

	vRight = XMVector3Normalize(XMVector3Cross(vUP, vForward));
	vForward = XMVector3Normalize(XMVector3Cross(vRight, vUP));



	XMStoreFloat4A(&forward, vForward);
	XMStoreFloat4(&RIGHT, vRight);

	float x = Input::MoveRight() * (m_movementSpeed * deltaTime) * RIGHT.x;

	x += Input::MoveForward() * (m_movementSpeed * deltaTime) * forward.x;
	//walkBob += x;

	float z = Input::MoveForward() * (m_movementSpeed * deltaTime) * forward.z;

	z += Input::MoveRight() * (m_movementSpeed * deltaTime) * RIGHT.z;

	x = Transform::getPosition().x + x;
	z = Transform::getPosition().z + z;

	Transform::setPosition(x, getPosition().y, z);
}

void Enemy::_handleRotation(double deltaTime)
{
	p_camera->Rotate((Input::TurnUp()*-1) * 5 * deltaTime, 0.0f, 0.0f);

	p_camera->Rotate(0.0f, Input::TurnRight() * 5 * deltaTime, 0.0f);
}

void Enemy::_TempGuardPath(bool x, double deltaTime)
{
	p_camera->Rotate(0.0f, .1f * 5 * deltaTime, 0.0f);
}

