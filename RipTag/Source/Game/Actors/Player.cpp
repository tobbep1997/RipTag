#include "Player.h"
#include "InputManager/InputHandler.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"
#include "EngineSource/3D Engine/RenderingManager.h"

Player::Player() : Actor(), CameraHolder(), PhysicsComponent()
{
	p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 50.0f));
	p_camera->setPosition(0, 0, 0);
	m_lockPlayerInput = false;
	
	visSphear = new Drawable();
	visSphear->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	visSphear->setScale(0.2f, 0.2f, 0.2f);
	visSphear->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	visSphear->setPosition(5, 5, 2);
	visSphear->setColor(1, 1, 1, 1.0f);
	visSphear->setEntityType(EntityType::ExcludeType);
}

Player::~Player()
{	
	delete visSphear;
}

void Player::BeginPlay()
{

}

void Player::Update(double deltaTime)
{
	if (m_lockPlayerInput == false)
	{
		_handleInput(deltaTime);
	}
#if _DEBUG

	m_teleport.Update(deltaTime);
	ImGui::Begin("Walk bob");
	ImGui::Text("HeadBob : %f", m_offset);
	ImGui::Text("HeadBobAmp : %f", m_currentAmp);
	ImGui::End();
#endif
	DirectX::XMFLOAT4A pos = getPosition();
	pos.y += m_offset;
	p_camera->setPosition(pos);

	if (Input::CheckVisability())
	{
		DirectX::XMFLOAT4A po = Transform::getPosition();
		DirectX::XMVECTOR ve = DirectX::XMLoadFloat4A(&po);
		DirectX::XMVECTOR cm = DirectX::XMLoadFloat4A(&p_camera->getDirection());
		DirectX::XMStoreFloat4A(&po, DirectX::XMVectorAdd(ve, cm));

		visSphear->setPosition(po);
		visSphear->setColor(1.0f * m_visability, 1.0f * m_visability, 1.0f * m_visability, 1);
	}
}

void Player::PhysicsUpdate(double deltaTime)
{
	p_updatePhysics(this);
}

void Player::setPosition(const float& x, const float& y, const float& z, const float& w)
{
	Transform::setPosition(x, y, z);
	PhysicsComponent::p_setPosition(x, y, z);
}

void Player::InitTeleport(b3World & world)
{
	m_teleport.Init(world, e_dynamicBody, 0.1f, 0.1f, 0.1f);
	m_teleport.setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	m_teleport.setScale(0.1f, 0.1f, 0.1f);
	m_teleport.setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_teleport.setGravityScale(0.001f);
	m_teleport.setPosition(-100.0f, -100.0f, -100.0f);
}

void Player::ReleaseTeleport(b3World & world)
{
	this->m_teleport.Release(world);
}

void Player::Draw()
{
	m_teleport.Draw();
	Drawable::Draw();
	if (Input::CheckVisability())
	{
		visSphear->Draw();
	}
	
}

void Player::SetCurrentVisability(const float & guard)
{
	this->m_visability = guard;
}

void Player::LockPlayerInput()
{
	m_lockPlayerInput = true;
}

void Player::UnlockPlayerInput()
{
	m_lockPlayerInput = false;
}


void Player::_handleInput(double deltaTime)
{
	using namespace DirectX;

	XMFLOAT4A forward = p_camera->getDirection();
	XMFLOAT4 UP = XMFLOAT4(0, 1, 0, 0);
	XMFLOAT4 RIGHT;
	//GeT_RiGhT;

	XMVECTOR vForward = XMLoadFloat4A(&forward);
	XMVECTOR vUP= XMLoadFloat4(&UP);
	XMVECTOR vRight;

	vRight = XMVector3Normalize(XMVector3Cross(vUP, vForward));
	vForward = XMVector3Normalize(XMVector3Cross(vRight, vUP));



	XMStoreFloat4A(&forward, vForward);
	XMStoreFloat4(&RIGHT, vRight);
	if (GamePadHandler::IsLeftStickPressed())
	{
		m_moveSpeed = 400.0f * deltaTime;
	}
	else
	{
		m_moveSpeed = 200.0f * deltaTime;
	}


	float x = Input::MoveRight() * m_moveSpeed * RIGHT.x;
	
	x += Input::MoveForward() * m_moveSpeed * forward.x;
	//walkBob += x;

	float z = Input::MoveForward() * m_moveSpeed * forward.z;
	
	z += Input::MoveRight() * m_moveSpeed * RIGHT.z;

#if _DEBUG
	ImGui::Begin("Bob Slide");
	ImGui::SliderFloat("WalkingFreq", &freq, 4.0f, 0.0f);
	ImGui::SliderFloat("WalkingBobAmp", &walkingBobAmp, 0.2f, 0.0f);
	ImGui::SliderFloat("StopBobAmp", &stopBobAmp, 0.2f, 0.0f);
	ImGui::SliderFloat("StopBobFreq", &stopBobFreq, 4.0f, 0.0f);
	ImGui::End();
#endif
	if (Input::MoveForward() != 0)
	{
		if (m_currentAmp < walkingBobAmp)
		{
			m_currentAmp += 0.0003f;
		}
		walkBob += (Input::MoveForward() * (m_moveSpeed * deltaTime));
		m_offset = walkingBobAmp * sin(freq*walkBob);
	}
	else
	{
		walkBob += 0.009f;

		if (m_currentAmp > stopBobAmp)
		{
			m_currentAmp -= 0.0001f;
		}
		/*if (walkBob < 0.0f)
		{
			walkBob += 0.01;
		}
		else if (walkBob > 0.0)
		{
			walkBob -= 0.01;
		}*/
		m_offset = m_currentAmp * sin(stopBobFreq * walkBob);
		
	}

	p_camera->Rotate((Input::TurnUp()*-1) * 5 * deltaTime, 0.0f, 0.0f);
	
	p_camera->Rotate(0.0f, Input::TurnRight() * 5 * deltaTime, 0.0f);

	if (Input::Jump())
	{
		if (isPressed == false)
		{
			addForceToCenter(0, 500, 0);
			isPressed = true;
		}
	}
	else
	{
		isPressed = false;
	}

	

	setLiniearVelocity(x, getLiniearVelocity().y, z);

	if (InputHandler::isKeyPressed('T'))
	{
		if (!m_teleport.getActiveSphere())
		{
			m_teleport.chargeSphere();
		}
	}
	else if (m_teleport.getCharging())
	{
		m_teleport.ThrowSphere(getPosition(), p_camera->getDirection());
		m_teleport.setCharging(false);
	}
	if (InputHandler::isKeyPressed('G'))
	{
		if (m_teleport.getActiveSphere())
		{
			DirectX::XMFLOAT4A newPos = m_teleport.TeleportToSphere();
			setPosition(newPos.x, newPos.y + 0.6f, newPos.z, newPos.w);
			//If we want skill... remove
			setLiniearVelocity(0, 0, 0);
			setAwakeState(true);
		}
	}
}

