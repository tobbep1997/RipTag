#include "RipTagPCH.h"
#include "Player.h"


Player::Player() : Actor(), CameraHolder(), PhysicsComponent(), HUDComponent()
{
	Manager::g_textureManager.loadTextures("CROSS");
	Manager::g_textureManager.loadTextures("BLACK");
	float convertion = (float)Input::GetPlayerFOV() / 100;
	//p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 110.0f));
	p_initCamera(new Camera(DirectX::XM_PI * convertion, 16.0f / 9.0f, 0.1f, 110.0f));
	p_camera->setPosition(0, 0, 0);
	m_lockPlayerInput = false;
	
	VisabilityAbility * visAbl = new VisabilityAbility();
	visAbl->setOwner(this);
	visAbl->setIsLocal(true);
	visAbl->Init();
	visAbl->setManaCost(1);

	VisabilityAbility * visAbl2 = new VisabilityAbility();
	visAbl2->setOwner(this);
	visAbl2->setIsLocal(true);
	visAbl2->Init();

	TeleportAbility * m_teleport = new TeleportAbility();
	m_teleport->setOwner(this);
	m_teleport->setIsLocal(true);
	m_teleport->Init();

	DisableAbility * m_dis = new DisableAbility();
	m_dis->setOwner(this);
	m_dis->setIsLocal(true);
	m_dis->Init();

	m_abilityComponents = new AbilityComponent*[m_nrOfAbilitys];
	m_abilityComponents[0] = m_teleport;
	m_abilityComponents[1] = visAbl;
	m_abilityComponents[2] = m_dis;
	m_abilityComponents[3] = visAbl2;

	m_currentAbility = Ability::TELEPORT;
	/*m_possess.setOwner(this);
	m_possess.Init();
	
	m_blink.setOwner(this);
	m_blink.Init();*/

	Quad * quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.1f, 0.15f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quad->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	HUDComponent::AddQuad(quad, 49);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.15f, 0.1f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quad->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	HUDComponent::AddQuad(quad, 50);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.1f, 0.05f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quad->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	HUDComponent::AddQuad(quad, 50);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.05f, 0.1f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quad->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	HUDComponent::AddQuad(quad, 50);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.5f, 0.5f), DirectX::XMFLOAT2A(5.0f / 16.0f, 5.0f /9.0f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("CROSS"));
	HUDComponent::AddQuad(quad);


	m_maxMana = STANDARD_START_MANA;
	m_currentMana = m_maxMana;

	m_manaBar = new Quad();
	m_manaBar->init(DirectX::XMFLOAT2A(0.25f, 0.01f), DirectX::XMFLOAT2A(5.0f / 16.0f, 5.0f / 9.0f));
	m_manaBar->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_manaBar->setPivotPoint(Quad::PivotPoint::lowerLeft);
	

	m_manaBarBackground = new Quad();
	m_manaBarBackground->init(DirectX::XMFLOAT2A(0.248f, 0.0f), DirectX::XMFLOAT2A(5.0f / 16.0f, 5.0f / 9.0f));
	m_manaBarBackground->setUnpressedTexture(Manager::g_textureManager.getTexture("BLACK"));
	m_manaBarBackground->setPivotPoint(Quad::PivotPoint::lowerLeft);
	m_manaBarBackground->setScale(((float)m_currentMana + 1.0f) / (float)m_maxMana, 0.13f);
	
	m_manabarText = new Quad();
	m_manabarText->init(DirectX::XMFLOAT2A(0.5, 0.034f), DirectX::XMFLOAT2A(0,0));
	m_manabarText->setUnpressedTexture(Manager::g_textureManager.getTexture("BLACK"));
	m_manabarText->setPivotPoint(Quad::PivotPoint::lowerLeft);
	m_manabarText->setScale(0,0);
	m_manabarText->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
	m_manabarText->setString("MANA");
	m_manabarText->setTextColor({ 75.0f / 255.0f,0.0f,130.0f / 255.0f,1.0f });

	HUDComponent::AddQuad(m_manaBar);
	HUDComponent::AddQuad(m_manaBarBackground);
	HUDComponent::AddQuad(m_manabarText);

	m_infoText = new Quad();
	m_infoText->init(DirectX::XMFLOAT2A(0.5, 0.4f), DirectX::XMFLOAT2A(0, 0));
	m_infoText->setUnpressedTexture(Manager::g_textureManager.getTexture("BLACK"));
	m_infoText->setPivotPoint(Quad::PivotPoint::lowerLeft);
	m_infoText->setScale(0, 0);
	m_infoText->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
	m_infoText->setTextColor({ 255.0f / 255.0f , 255.0f / 255.0f, 200.0f / 255.0f,1.0f });
	HUDComponent::AddQuad(m_infoText);


	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep1.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep2.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep3.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep4.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep5.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep6.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep7.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep8.ogg"));

}

Player::Player(RakNet::NetworkID nID, float x, float y, float z) : Actor(), CameraHolder(), PhysicsComponent()
{
	p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 50.0f));
	p_camera->setPosition(x, y, z);
	m_lockPlayerInput = false;

}

Player::~Player()
{
	for (unsigned short int i = 0; i < m_nrOfAbilitys; i++)
		delete m_abilityComponents[i];
	delete[] m_abilityComponents;
	delete m_blink;
	delete m_possess;
}

void Player::Init(b3World& world, b3BodyType bodyType, float x, float y, float z)
{
	PhysicsComponent::Init(world, bodyType, x, y, z);
	this->getBody()->SetObjectTag("PLAYER");
	this->getBody()->AddToFilters("TELEPORT");
	setUserDataBody(this);

	setEntityType(EntityType::PlayerType);
	setColor(10, 10, 0, 1);

	setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	setScale(1.0f, 1.0f, 1.0f);
	setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	setTextureTileMult(2, 2);

	m_blink = new BlinkAbility();
	m_blink->Init();
	m_blink->setOwner(this);
	m_possess = new PossessGuard();
	m_possess->Init();
	m_possess->setOwner(this);

	setHidden(true);

}

void Player::BeginPlay()
{

}
#include <math.h>
void Player::Update(double deltaTime)
{
	const DirectX::XMFLOAT4A xmLP = p_camera->getPosition();
	FMOD_VECTOR fvLP = { xmLP.x, xmLP.y, xmLP.z, };
	//std::cout << getPosition().x << " " << getPosition().y << " " << getPosition().z << std::endl;

	using namespace DirectX;

	if (m_lockPlayerInput == false)
	{
		if (InputHandler::getWindowFocus())
		{
			_handleInput(deltaTime);
		}
	}

	m_manaBar->setScale((float)m_currentMana / (float)m_maxMana, 0.1f);
	if (InputHandler::isKeyPressed('I'))
	{
		RefillMana(10);
	}
	if (InputHandler::isKeyPressed('J'))
	{
		m_maxMana += 10;
	}

	m_abilityComponents[m_currentAbility]->Update(deltaTime);
	m_possess->Update(deltaTime);
	m_blink->Update(deltaTime);
	_cameraPlacement(deltaTime);
	_updateFMODListener(deltaTime, xmLP);
	//HUDComponent::HUDUpdate(deltaTime);
	
	if (Input::SelectAbility1())	
		m_currentAbility = Ability::TELEPORT;		
	else if (Input::SelectAbility2())	
		m_currentAbility = Ability::VISIBILITY;	
	else if (Input::SelectAbility3())	
		m_currentAbility = Ability::DISABLE;	
	else if (Input::SelectAbility4())
		m_currentAbility = Ability::VIS2;
	
	if (GamePadHandler::IsUpDpadPressed())
		m_currentAbility = Ability::TELEPORT;
	else if (GamePadHandler::IsRightDpadPressed())
		m_currentAbility = Ability::VISIBILITY;
	else if (GamePadHandler::IsDownDpadPressed())
		m_currentAbility = Ability::DISABLE;
	else if (GamePadHandler::IsLeftDpadPressed())
		m_currentAbility = Ability::VIS2;

	HUDComponent::ResetStates();
	HUDComponent::setSelectedQuad(m_currentAbility);
}

void Player::PhysicsUpdate()
{
	p_updatePhysics(this);
}

void Player::setPosition(const float& x, const float& y, const float& z, const float& w)
{
	Transform::setPosition(x, y, z, w);
	PhysicsComponent::p_setPosition(x, y, z);
}
	
const float & Player::getVisability() const
{
	return m_visability;
}

const int & Player::getFullVisability() const
{
	return g_fullVisability;
}

bool Player::CheckManaCost(const int& manaCost)
{
	if (manaCost <= m_currentMana)
	{
		return true;
	}
	else
	{
		return false;
	}
}

const AudioEngine::Listener & Player::getFMODListener() const
{
	return m_FMODlistener;
}

bool Player::DrainMana(const float& manaCost)
{
	if (manaCost <= m_currentMana)
	{
		m_currentMana -= manaCost;
		return true;
	}
	else
	{
		return false;
	}
}

void Player::RefillMana(const float& manaFill)
{
	m_currentMana += manaFill;

	float rest = m_maxMana - m_currentMana;
	if (rest < 0)
	{
		m_currentMana += rest;
	}
}

void Player::Draw()
{
	for (int i = 0; i < m_nrOfAbilitys; i++)
		m_abilityComponents[i]->Draw();
	Drawable::Draw();
	HUDComponent::HUDDraw();

}

void Player::LockPlayerInput()
{
	m_lockPlayerInput = true;
}

bool Player::IsInputLocked()
{
	return m_lockPlayerInput;
}

void Player::UnlockPlayerInput()
{
	m_lockPlayerInput = false;
}

void Player::SetCurrentVisability(const float & guard)
{
	this->m_visability = guard;
}

void Player::SendOnUpdateMessage()
{
	Network::ENTITYUPDATEPACKET packet = Network::ENTITYUPDATEPACKET(
		Network::ID_PLAYER_UPDATE,
		Network::Multiplayer::GetInstance()->GetNetworkID(),
		PlayerState::Idle,
		this->getPosition(),
		this->getCamera()->getYRotationEuler()
		);

	
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
}

void Player::SendOnAbilityUsed()
{
	using namespace Network;
	ENTITYABILITYPACKET packet;

	//Same for every ability packet
	packet.id = ID_TIMESTAMP;
	packet.timeStamp = RakNet::GetTime();
	packet.m_id = ID_PLAYER_ABILITY;

	TeleportAbility * tp_ptr = dynamic_cast<TeleportAbility*>(m_abilityComponents[m_currentAbility]);
	DisableAbility * dis_ptr = dynamic_cast<DisableAbility*>(m_abilityComponents[m_currentAbility]);
	VisabilityAbility * vis_ptr = dynamic_cast<VisabilityAbility*>(m_abilityComponents[m_currentAbility]);
	//unique based on active ability
	switch (this->m_currentAbility)
	{
	case Ability::TELEPORT:
		packet.ability = (unsigned int)TELEPORT;
		packet.start = tp_ptr->getStart();
		packet.velocity = tp_ptr->getVelocity();
		packet.state = tp_ptr->getState();
		break;
	case Ability::DISABLE:
		packet.ability = (unsigned int)DISABLE;
		packet.start = dis_ptr->getStart();
		packet.velocity = dis_ptr->getVelocity();
		packet.state = dis_ptr->getState();
		break;
	case Ability::VIS2:
	case Ability::VISIBILITY:
		packet.ability = (unsigned int)VISIBILITY;
		packet.start = vis_ptr->getStart();
		packet.velocity = vis_ptr->getLastColor();
		packet.state = vis_ptr->getState();
		break;
	}

	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(ENTITYABILITYPACKET), PacketPriority::LOW_PRIORITY);
}

void Player::SendOnAnimationUpdate(double dt)
{
	using namespace DirectX;

	static double accumulatedTime = 0.0;
	static const double FREQUENCY = 1.0 / 50.0; //20 ms
	accumulatedTime += dt;

	if (accumulatedTime >= FREQUENCY)
	{
		accumulatedTime -= FREQUENCY;
		
		{
			///Speed
			b3Vec3 physSpeed = this->getLiniearVelocity();

			//if y speed is zero, set isInAir to false,
			if (std::abs(physSpeed.y) < 0.001f)
				m_isInAir = false;

			float speed = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSet(physSpeed.x, 0.0, physSpeed.z, 0)));
			m_currentSpeed = std::clamp(std::fabs(speed), 0.0f, 3.0f);

			///Walk dir
				//Get camera direction and normalize on X,Z plane
			XMFLOAT4A cameraDir = p_camera->getDirection();
			XMVECTOR cameraDirNormalized = XMVector3Normalize(XMVectorSet(cameraDir.x, 0.0f, cameraDir.z, 0.0));
			///assert(XMVectorGetX(XMVector3Length(cameraDirNormalized)) != 0.0f);

			XMVECTOR XZCameraDir = XMVectorSet(cameraDir.x, 0.0, cameraDir.z, 0.0);
			XMVECTOR XZMovement = XMVectorSet(physSpeed.x, 0.0, physSpeed.z, 0.0);
			XMVECTOR XZCameraDirNormalized = XMVector3Normalize(XZCameraDir);
			XMVECTOR XZMovementNormalized = XMVector3Normalize(XZMovement);
			///AssertHasLength(XZCameraDir);
			//AssertHasLength(XZMovement);

				//Get dot product of cam dir and player movement
			float dot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSet(physSpeed.x, 0, physSpeed.z, 0.0)), cameraDirNormalized));
			dot = std::clamp(dot, -0.999999f, 0.999999f);
			//Convert to degrees
			m_currentDirection = XMConvertToDegrees(std::acos(dot));
			//Negate if necessary
			float inverter = (XMVectorGetY(XMVector3Cross(XZMovement, XZCameraDir)));

			m_currentDirection *= (inverter > 0.0)
				? -1.0
				: 1.0;
			m_currentDirection = std::clamp(m_currentDirection, -180.0f, 180.0f);
			///AssertNotNAN(m_currentDirection);
		}
		Network::ENTITYANIMATIONPACKET packet(
			Network::ID_PLAYER_ANIMATION, 
			Network::Multiplayer::GetInstance()->GetNetworkID(),
			this->m_currentDirection, 
			this->m_currentSpeed,
			this->getCamera()->getYRotationEuler());
		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENTITYANIMATIONPACKET), PacketPriority::LOW_PRIORITY);
	}
}

void Player::RegisterThisInstanceToNetwork()
{
	Network::Multiplayer::addToOnSendFuncMap("Jump", std::bind(&Player::SendOnUpdateMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveRight", std::bind(&Player::SendOnUpdateMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveLeft", std::bind(&Player::SendOnUpdateMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveForward", std::bind(&Player::SendOnUpdateMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveBackward", std::bind(&Player::SendOnUpdateMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("AbilityPressed", std::bind(&Player::SendOnAbilityUsed, this));
	Network::Multiplayer::addToOnSendFuncMap("AbilityReleased", std::bind(&Player::SendOnAbilityUsed, this));
}

void Player::_handleInput(double deltaTime)
{
	if (Input::MouseLock() && !m_kp.unlockMouse)
	{
		m_kp.unlockMouse = true;
		unlockMouse = !unlockMouse;
	}
	else if (!Input::MouseLock())
		m_kp.unlockMouse = false;
	

	_onSprint();
	_onCrouch();
	_onMovement();
	_onJump();
	_onAbility(deltaTime);
	_onBlink();
	_onPossess();
	_onInteract();
	_onRotate(deltaTime);
	_objectInfo(deltaTime);
}

void Player::_onMovement()
{
	using namespace DirectX;
	XMFLOAT4A forward = p_camera->getDirection();

	float yDir = forward.y;
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
	float x = 0;
	float z = 0;

	x = Input::MoveRight() * m_moveSpeed  * RIGHT.x;
	x += Input::MoveForward() * m_moveSpeed * forward.x;
	z = Input::MoveForward() * m_moveSpeed * forward.z;
	z += Input::MoveRight() * m_moveSpeed * RIGHT.z;

	setLiniearVelocity(x, getLiniearVelocity().y, z);
}

void Player::_onSprint()
{
	if (Input::MoveForward() != 0 || Input::MoveRight() != 0)
	{
		if (Input::isUsingGamepad())
		{
			m_currClickSprint = Input::Sprinting();
			if (m_currClickSprint && !m_prevClickSprint && m_toggleSprint == 0 && Input::MoveForward() > 0.9)
			{
				m_toggleSprint = 1;
			}

			if (m_toggleSprint == 1 && Input::MoveForward() > 0.9)
			{
				m_moveSpeed = MOVE_SPEED * SPRINT_MULT;
				p_moveState = Sprinting;
			}
			else
			{
				m_toggleSprint = 0;
			}

			if (m_toggleSprint == 0)
			{
				m_moveSpeed = MOVE_SPEED;
				p_moveState = Walking;
			}

			if (Input::MoveForward() == 0)
			{
				p_moveState = Idle;
				m_toggleSprint = 0; 
			}

			m_prevClickSprint = m_currClickSprint;
		}
		else
		{
			if (Input::Sprinting())
			{
				m_moveSpeed = MOVE_SPEED * SPRINT_MULT;
				p_moveState = Sprinting;
			}
			else
			{
				m_moveSpeed = MOVE_SPEED;
				p_moveState = Walking;
			}
		}
	}	
	else
	{
		m_moveSpeed = 0; 
		p_moveState = Idle; 
	}
	
}

void Player::_onCrouch()
{
	if(Input::isUsingGamepad())
	{
		m_currClickCrouch = Input::Crouch();
		if (m_currClickCrouch && !m_prevClickCrouch && m_toggleCrouch == 0)
		{
			_activateCrouch();
			m_toggleCrouch = 1;
		}
		else if (m_currClickCrouch && !m_prevClickCrouch && m_toggleCrouch == 1)
		{
			_deActivateCrouch();
			m_toggleCrouch = 0; 

			//Just so we don't end up in an old sprint-mode when deactivating crouch.
			m_toggleSprint = 0;
		}
		m_prevClickCrouch = m_currClickCrouch;
	}
	else
	{
		if (Input::Crouch())
		{
			if (m_kp.crouching == false)
			{
				m_standHeight = this->p_camera->getPosition().y;
				this->CreateBox(0.5f, 0.10f, 0.5f);
				this->setPosition(this->getPosition().x, this->getPosition().y - 0.4, this->getPosition().z, 1);
				m_kp.crouching = true;
			}
		}
		else
		{
			if (m_kp.crouching)
			{
				m_standHeight = this->p_camera->getPosition().y;
				this->CreateBox(0.5, 0.5, 0.5);
				this->setPosition(this->getPosition().x, this->getPosition().y + 0.4, this->getPosition().z, 1);
				m_kp.crouching = false;
			}
		}
	}

	if (m_kp.crouching)
	{
		m_moveSpeed = MOVE_SPEED * 0.5f;
	}
}

void Player::_onBlink()
{
	if (Input::Blink()) //Phase acts like short range teleport through objects
	{
		if (m_kp.blink == false)
		{
			m_blink->Use();
			m_kp.blink = true;
		}
	}
	else
	{
		m_kp.blink = false;
	}
}

void Player::_onPossess()
{
	if (Input::Possess()) //Phase acts like short range teleport through objects
	{
		
		if (m_kp.possess == false)
		{
			setHidden(false);
			m_possess->Use();
			m_kp.possess = true;
		}
	}
	else
	{
		setHidden(true);
		m_kp.possess = false;
	}
}

void Player::_onRotate(double deltaTime)
{
	if (!unlockMouse)
	{	
		float deltaY = Input::TurnUp();
		float deltaX = Input::TurnRight();
		if (Input::PeekRight() > 0.1 || Input::PeekRight() < -0.1)
		{
			
		}
		else
		{
			if (m_peekRotate > 0.05f || m_peekRotate < -0.05f)
			{
				if (m_peekRotate > 0)
				{
					p_camera->Rotate(0.0f, -0.05f, 0.0f);
					m_peekRotate -= 0.05;
				}
				else
				{
					p_camera->Rotate(0.0f, +0.05f, 0.0f);
					m_peekRotate += 0.05;
				}

			}
			else
			{
				m_peekRotate = 0;
			}
			//m_peekRotate = 0;
		}

		if (deltaX && (m_peekRotate + deltaX * Input::GetPlayerMouseSensitivity() * deltaTime) <= 0.5 && (m_peekRotate + deltaX * Input::GetPlayerMouseSensitivity() * deltaTime) >=-0.5)
		{
			p_camera->Rotate(0.0f, deltaX * Input::GetPlayerMouseSensitivity() * deltaTime, 0.0f);
			if (Input::PeekRight() > 0.1 || Input::PeekRight() < -0.1)
			{
				m_peekRotate += deltaX * Input::GetPlayerMouseSensitivity() * deltaTime;
			}
		}
		if (deltaY) 
		{
			if ((p_camera->getDirection().y - deltaY * Input::GetPlayerMouseSensitivity() * deltaTime) < 0.90f)
			{
				p_camera->Rotate(deltaY * Input::GetPlayerMouseSensitivity() * deltaTime, 0.0f, 0.0f);
			}
			else if (p_camera->getDirection().y >= 0.90f)
			{
				p_camera->setDirection(p_camera->getDirection().x, 0.89f, p_camera->getDirection().z);
			}
			if ((p_camera->getDirection().y - deltaY * Input::GetPlayerMouseSensitivity() * deltaTime) > -0.90f)
			{
				p_camera->Rotate(deltaY * Input::GetPlayerMouseSensitivity() * deltaTime, 0.0f, 0.0f);
			}
			else if (p_camera->getDirection().y <= -0.90f)
			{
				p_camera->setDirection(p_camera->getDirection().x, -0.89f, p_camera->getDirection().z);
			}
		
			
		}

	}
}

void Player::_onJump()
{
	if (Input::Jump())
	{
		if (m_kp.jump == false)
		{
			addForceToCenter(0, JUMP_POWER, 0);
			m_kp.jump = true;
			m_jumpedThisFrame = true;
			m_isInAir = true;
		}
	}


	float epsilon = 0.002f;
	if (this->getLiniearVelocity().y < epsilon && this->getLiniearVelocity().y > -epsilon)
		m_kp.jump = false;
}

void Player::_onInteract()
{
	if (Input::Interact())
	{
		if (m_kp.interact == false)
		{
			RipExtern::m_rayListener->ShotRay(this->getBody(), this->getCamera()->getPosition(), this->getCamera()->getDirection(), Player::INTERACT_RANGE, false);
			for (RayCastListener::RayContact* con : RipExtern::m_rayListener->GetContacts())
			{
				if (*con->consumeState != 2)
				{
					if (con->originBody->GetObjectTag() == getBody()->GetObjectTag())
					{
						if (con->contactShape->GetBody()->GetObjectTag() == "ITEM")
						{
							*con->consumeState += 1;
							//do the pickups
						}
						else if (con->contactShape->GetBody()->GetObjectTag() == "LEVER")
						{
							*con->consumeState += 1;
							m_infoText->setString("");
							m_objectInfoTime = 0;
						}
						else if (con->contactShape->GetBody()->GetObjectTag() == "TORCH")
						{
							*con->consumeState += 1;
							//Snuff out torches (example)
						}
						else if (con->contactShape->GetBody()->GetObjectTag() == "ENEMY")
						{
							//*con->consumeState += 1;
							//std::cout << "Enemy Found!" << std::endl;
							//Snuff out torches (example)
						}
						else if (con->contactShape->GetBody()->GetObjectTag() == "BLINK_WALL")
						{
							//*con->consumeState += 1;
							//std::cout << "illusory wall ahead" << std::endl;
							//Snuff out torches (example)
						}
					}
				}
			}
			m_kp.interact = true;
		}
	}
	else
	{
		m_kp.interact = false;
	}
}

void Player::_onAbility(double dt)
{
	this->m_abilityComponents[m_currentAbility]->Update(dt);
}

//Sends a ray every second and check if there is relevant data for the object to show on the screen
void Player::_objectInfo(double deltaTime)
{
	if (m_objectInfoTime >= 1)
	{
		RayCastListener::RayContact* contact = RipExtern::m_rayListener->ShotRay(getBody(), getCamera()->getPosition(), getCamera()->getDirection(), 10);
		if (contact != nullptr)
		{
			if (contact->contactShape->GetBody()->GetObjectTag() == "NULL")
			{
				m_infoText->setString("");
				//do the pickups
			}
			else if (contact->contactShape->GetBody()->GetObjectTag() == "LEVER" && contact->fraction <= 0.3)
			{
				//m_infoText->setString("Press TAB to pull");				
			}
			else if (contact->contactShape->GetBody()->GetObjectTag() == "TORCH")
			{
				//Snuff out torches (example)
			}
			else if (contact->contactShape->GetBody()->GetObjectTag() == "ENEMY")
			{
				//m_infoText->setString("Press R to possess");
				//Snuff out torches (example)
			}
			else if (contact->contactShape->GetBody()->GetObjectTag() == "BLINK_WALL" && contact->fraction <= 0.3)
			{
				//m_infoText->setString("Illusory wall ahead");
				//Snuff out torches (example)
			}
		}
		else
		{
			m_infoText->setString("");
		}
		m_objectInfoTime = 0;
	}

	m_objectInfoTime += deltaTime;
}

void Player::_cameraPlacement(double deltaTime)
{
	static float lastOffset = 0.0f;
	static bool hasPlayed = true;
	static int last = 0;

	float cameraOffset = 1.87f;
	DirectX::XMFLOAT4A pos = getPosition();
	pos.y += cameraOffset;
	p_camera->setPosition(pos);
	pos = p_CameraTilting(deltaTime, Input::PeekRight(), getPosition());
	float offsetY = p_viewBobbing(deltaTime, Input::MoveForward(), m_moveSpeed, p_moveState);

	pos.y += offsetY;
	
	if (p_moveState == Walking || p_moveState == Sprinting)
	{
		if (!hasPlayed)
		{
			if (lastOffset < offsetY)
			{
				hasPlayed = true;
				auto xmPos = getPosition();
				FMOD_VECTOR at = {xmPos.x, xmPos.y, xmPos.z};
				int index = -1;
				while (index == -1 || index == last)
				{
					index = rand() % (int)m_sounds.size();
				}
				AudioEngine::PlaySoundEffect(m_sounds[index], &at)->setVolume(0.3 + (p_moveState * 0.4));
				last = index;
			}
		}
		else
		{
			if (lastOffset > offsetY)
			{
				hasPlayed = false;
			}
		}

		lastOffset = offsetY;
	}


	pos.y += p_Crouching(deltaTime, this->m_standHeight, p_camera->getPosition());
	p_camera->setPosition(pos);
}

void Player::_updateFMODListener(double deltaTime, const DirectX::XMFLOAT4A & xmLastPos)
{
	const DirectX::XMFLOAT4A & xmDir = p_camera->getDirection();
	const DirectX::XMFLOAT4A & xmPos = p_camera->getPosition();
	const DirectX::XMFLOAT4A & xmRight = p_camera->getRight();
	DirectX::XMFLOAT3 xmUp;
	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat4A(&xmDir);
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat4A(&xmRight);
	DirectX::XMVECTOR vUp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vDir, vRight));
	DirectX::XMStoreFloat3(&xmUp, vUp);

	//std::cout << xmPos.x << ", " << xmPos.y << ", " << xmPos.z << std::endl;

	FMOD_VECTOR vel;
	vel.x = ( xmPos.x - xmLastPos.x ) / deltaTime;
	vel.y = ( xmPos.y - xmLastPos.y ) / deltaTime;
	vel.z = ( xmPos.z - xmLastPos.z ) / deltaTime;

	m_FMODlistener.pos = { xmPos.x, xmPos.y, xmPos.z };
	m_FMODlistener.up = { xmUp.x, xmUp.y, xmUp.z };
	m_FMODlistener.forward = { xmDir.x, xmDir.y, xmDir.z };
	m_FMODlistener.vel = vel;
}
void Player::_activateCrouch()
{
	this->setPosition(this->getPosition().x, this->getPosition().y - m_offPutY, this->getPosition().z);
	m_standHeight = this->p_camera->getPosition().y;
	this->CreateBox(0.5f, 0.10f, 0.5f);
	m_kp.crouching = true; 
}

void Player::_deActivateCrouch()
{
	this->setPosition(this->getPosition().x, this->getPosition().y + m_offPutY, this->getPosition().z);
	m_standHeight = this->p_camera->getPosition().y;
	this->CreateBox(0.5, 0.5, 0.5);
	m_kp.crouching = false;
}

