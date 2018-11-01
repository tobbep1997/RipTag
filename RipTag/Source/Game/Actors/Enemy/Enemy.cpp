#include "Enemy.h"
#include "../RipTag/Source/Input/Input.h"
#include "../Player.h"
#include "EngineSource/3D Engine/RenderingManager.h"


Enemy::Enemy() : Actor(), CameraHolder(), PhysicsComponent()
{
	this->p_initCamera(new Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f, 0.1f, 50.0f));
	m_vc.Init(this->p_camera);

}

Enemy::Enemy(float startPosX, float startPosY, float startPosZ) : Actor(), CameraHolder()
{
	this->p_initCamera(new Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f, 0.1f, 50.0f));
	m_vc.Init(this->p_camera);
	this->setPosition(startPosX, startPosY, startPosZ);
	this->setDir(1, 0, 0);
	this->getCamera()->setFarPlane(20);
	this->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	this->setTexture(Manager::g_textureManager.getTexture("SPHERE"));

	srand(time(NULL));
}

Enemy::Enemy(b3World* world, float startPosX, float startPosY, float startPosZ) : Actor(), CameraHolder(), PhysicsComponent()
{
	this->p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 50.0f));
	m_vc.Init(this->p_camera);
	this->setDir(1, 0, 0);
	this->getCamera()->setFarPlane(20);
	this->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	this->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	this->Init(*world, e_staticBody);

	this->getBody()->SetUserData(Enemy::validate());
	this->getBody()->SetObjectTag("Enemy");

	this->setEntityType(EntityType::GuarddType);
	this->setPosition(startPosX, startPosY, startPosZ);
}


Enemy::~Enemy()
{
	this->Release(*this->getBody()->GetScene());
	for (auto path : m_path)
	{
		delete path;
	}
	m_path.clear();
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
	if (!m_disabled)
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
	if (!m_disabled)
	{
		if (!m_inputLocked)
		{
			_handleInput(deltaTime);
		}
		else
		{
			_TempGuardPath(true, 0.001f);
			if (m_path.size() > 0)
			{
				_MoveTo(m_path.at(0), deltaTime);
			}
		}

		_CheckPlayer(deltaTime);

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

void Enemy::DisableEnemy()
{
	m_disabled = true;
}

void Enemy::EnableEnemy()
{
	m_disabled = false;
}

bool Enemy::GetDisabledState()
{
	return m_disabled;
}

void Enemy::_handleInput(double deltaTime)
{
	_handleMovement(0.001f);
	_handleRotation(0.001f);
	_possessed(deltaTime);
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

void Enemy::_IsInSight()
{
	float temp = (float)m_vc.getVisibilityForPlayers()[0] / (float)Player::g_fullVisability;
	temp *= 100;

	//std::cout << m_vc.getVisibilityForPlayers()[0] << std::endl;

	//int ran = rand() % 100 + 1;
#if _DEBUG
	/*ImGui::Begin("Sight");
	ImGui::Text("vis: %f", temp);
	ImGui::Text("FullVis: %f", Player::g_fullVisability);
	ImGui::End();*/
#endif
	//if (ran < temp) 
	//{
	//	std::cout << "Saw you" << std::endl;
	//}
}

Enemy* Enemy::validate()
{
	return this;
}
void Enemy::setPossessor(Actor* possessor, float maxDuration, float delay)
{
	m_possessor = possessor;
	m_possessReturnDelay = delay;
	m_maxPossessDuration = maxDuration;
}

void Enemy::removePossessor()
{
	if (m_possessor != nullptr)
	{
		static_cast<Player*>(m_possessor)->UnlockPlayerInput();
		m_possessor = nullptr;
		m_possessReturnDelay = 0;
		m_maxPossessDuration = 0;
	}
}

void Enemy::SetPathVector(std::vector<Node*> path)
{
	m_path = path;
}

std::vector<Node*> Enemy::GetPathVector()
{
	return m_path;
}


void Enemy::_possessed(double deltaTime)
{
	if (m_possessor != nullptr)
	{
		if (m_possessReturnDelay <= 0)
		{
			if (Input::Possess())
			{
				static_cast<Player*>(m_possessor)->UnlockPlayerInput();
				m_possessor = nullptr;
			}
			else if(m_maxPossessDuration <= 0)
			{
				static_cast<Player*>(m_possessor)->UnlockPlayerInput();
				m_possessor = nullptr;
			}
		}
		else
			m_possessReturnDelay -= deltaTime;

		m_maxPossessDuration -= deltaTime;
	}
}

bool Enemy::_MoveTo(Node* nextNode, double deltaTime)
{
	if (abs(nextNode->worldPos.x - getPosition().x) <= 1 && abs(nextNode->worldPos.y - getPosition().z) <= 1)
	{
		delete m_path.at(0);
		m_path.erase(m_path.begin());
		return true;
	}
	else
	{
		float x = nextNode->worldPos.x - getPosition().x;
		float y = nextNode->worldPos.y - getPosition().z;

		float angle = atan2(y, x);

		float dx = cos(angle) * m_guardSpeed * deltaTime;
		float dy = sin(angle) * m_guardSpeed * deltaTime;

		setPosition(getPosition().x + dx, getPosition().y, getPosition().z + dy);
		return false;
	}
}

void Enemy::_CheckPlayer(double deltaTime)
{
	if (m_allowVisability)
	{
		float visPres = (float)m_vc.getVisibilityForPlayers()[0] / (float)Player::g_fullVisability;


		if (visPres > 0)
		{
			m_visCounter += visPres * deltaTime;
			if (m_visabilityTimer <= m_visCounter)
			{
				//std::cout << "FOUND YOU BITCH" << std::endl;
			}
		}
		else
		{
			
			if (m_visCounter - deltaTime > 0)
			{
				m_visCounter -= deltaTime;
			}
			else
			{
				m_visCounter = 0;
			}
		}

	}
	else
	{
		if (m_visCounter - deltaTime > 0)
		{
			m_visCounter -= deltaTime;
		}
		else
		{
			m_visCounter = 0;
		}
	}

}
