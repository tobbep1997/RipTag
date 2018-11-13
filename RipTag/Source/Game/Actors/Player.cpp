#include "RipTagPCH.h"
#include "Player.h"

Player::Player() : Actor(), CameraHolder(), PhysicsComponent(), HUDComponent()
{
	Manager::g_textureManager.loadTextures("CROSS");
	Manager::g_textureManager.loadTextures("BLACK");
	Manager::g_textureManager.loadTextures("VISIBILITYICON");
	//float convertion = (float)Input::GetPlayerFOV() / 100;
	//p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 110.0f));
	p_initCamera(new Camera(DirectX::XMConvertToRadians(Input::GetPlayerFOV()), 16.0f / 9.0f, 0.1f, 110.0f));
	p_camera->setPosition(0, 0, 0);
	m_lockPlayerInput = false;

	//Ability stuff
	{
		VisabilityAbility * visAbl = new VisabilityAbility();
		visAbl->setOwner(this);
		visAbl->setIsLocal(true);
		visAbl->Init();
		

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

		BlinkAbility * m_blink = new BlinkAbility();
		m_blink->setOwner(this);
		m_blink->setIsLocal(true);
		m_blink->Init();

		PossessGuard * m_possess = new PossessGuard();
		m_possess->setOwner(this);
		m_possess->setIsLocal(true);
		m_possess->Init();

		m_abilityComponents1 = new AbilityComponent*[m_nrOfAbilitys];
		m_abilityComponents1[0] = m_teleport;
		m_abilityComponents1[1] = visAbl;
		m_abilityComponents1[2] = m_dis;
		m_abilityComponents1[3] = visAbl2;

		m_abilityComponents2 = new AbilityComponent*[m_nrOfAbilitys];
		m_abilityComponents2[0] = m_blink;
		m_abilityComponents2[1] = visAbl;
		m_abilityComponents2[2] = m_possess;
		m_abilityComponents2[3] = visAbl2;

		m_currentAbility = (Ability)0;

		//By default always this set
		m_activeSet = m_abilityComponents1;
	}
	Quad * quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.1f, 0.15f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture("SPHERE");
	quad->setPressedTexture("DAB");
	HUDComponent::AddQuad(quad, 49);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.15f, 0.1f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture("SPHERE");
	quad->setPressedTexture("DAB");
	HUDComponent::AddQuad(quad, 50);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.1f, 0.05f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture("SPHERE");
	quad->setPressedTexture("DAB");
	HUDComponent::AddQuad(quad, 50);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.05f, 0.1f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture("SPHERE");
	quad->setPressedTexture("DAB");
	HUDComponent::AddQuad(quad, 50);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.5f, 0.5f), DirectX::XMFLOAT2A(5.0f / 16.0f, 5.0f /9.0f));
	quad->setUnpressedTexture("CROSS");
	HUDComponent::AddQuad(quad);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.15f, 0.1f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture("VISIBILITYICON");
	HUDComponent::AddQuad(quad);



	   	 



	m_winBar = new Quad();
	m_winBar->init();
	m_winBar->setPosition(1.5f, 1.5f);
	m_winBar->setScale(0.5f, 0.25f);

	m_winBar->setString("YOU WIN");
	m_winBar->setUnpressedTexture("SPHERE");
	m_winBar->setPressedTexture("DAB");
	m_winBar->setHoverTexture("PIRASRUM");
	m_winBar->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_winBar->setFont(FontHandler::getFont("consolas32"));
	HUDComponent::AddQuad(m_winBar);

	m_infoText = new Quad();
	m_infoText->init(DirectX::XMFLOAT2A(0.5, 0.3f), DirectX::XMFLOAT2A(0, 0));
	m_infoText->setUnpressedTexture("BLACK");
	m_infoText->setPivotPoint(Quad::PivotPoint::lowerLeft);
	m_infoText->setScale(0, 0);
	m_infoText->setFont(FontHandler::getFont("consolas16"));
	m_infoText->setTextColor({ 255.0f / 255.0f , 255.0f / 255.0f, 200.0f / 255.0f,1.0f });
	HUDComponent::AddQuad(m_infoText);
	m_tutorialMessages.push("");
	m_tutorialMessages.push("Each player has unique abilities to assist with the escape");
	m_tutorialMessages.push("Be on the lookout for pressure plates and levers \nto reach the exit");
	m_tutorialMessages.push("Select your abilities with DPAD and use with RB");
	m_tutorialMessages.push("Peek to the sides with LT and RT");
	m_tutorialMessages.push("Rule number 1 of subterfuge:\navoid being seen!");

	m_tutorialText = new Quad();
	m_tutorialText->init(DirectX::XMFLOAT2A(0.5, 0.9f), DirectX::XMFLOAT2A(0, 0));
	m_tutorialText->setUnpressedTexture("BLACK");
	m_tutorialText->setPivotPoint(Quad::PivotPoint::lowerLeft);
	m_tutorialText->setScale(0, 0);
	m_tutorialText->setFont(FontHandler::getFont("consolas16"));
	m_tutorialText->setTextColor({ 255.0f / 255.0f , 255.0f / 255.0f, 200.0f / 255.0f,1.0f });
	HUDComponent::AddQuad(m_tutorialText);
	
	if (m_tutorialActive)
	{
		m_tutorialText->setString(m_tutorialMessages.top());
		m_tutorialMessages.pop();
	}

	m_abilityTutorialText = new Quad();
	m_abilityTutorialText->init(DirectX::XMFLOAT2A(0.15, 0.26f), DirectX::XMFLOAT2A(0, 0));
	m_abilityTutorialText->setUnpressedTexture("BLACK");
	m_abilityTutorialText->setPivotPoint(Quad::PivotPoint::lowerLeft);
	m_abilityTutorialText->setScale(0, 0);
	m_abilityTutorialText->setFont(FontHandler::getFont("consolas16"));
	m_abilityTutorialText->setTextColor({ 255.0f / 255.0f , 255.0f / 255.0f, 200.0f / 255.0f,1.0f });
	HUDComponent::AddQuad(m_abilityTutorialText);

	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep1.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep2.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep3.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep4.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep5.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep6.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep7.ogg"));
	m_sounds.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep8.ogg"));


	m_HUDcircle = new Circle();
	m_HUDcircle->init(DirectX::XMFLOAT2A(0.95f, 0.075f), DirectX::XMFLOAT2A(2.1f / 16.0f, 2.1f / 9.0f));
	m_HUDcircle->setRadie(.5f);
	m_HUDcircle->setInnerRadie(.45f);
	m_HUDcircle->setUnpressedTexture("DAB");
	m_HUDcircle->setPressedTexture("DAB");
	m_HUDcircle->setHoverTexture("PIRASRUM");

	Manager::g_textureManager.loadTextures("FML");
	m_HUDcircleFiller = new Circle();
	m_HUDcircleFiller->init(DirectX::XMFLOAT2A(0.95f, 0.075f), DirectX::XMFLOAT2A(2.f / 16.0f, 2.f / 9.0f));
	m_HUDcircleFiller->setInnerRadie(-1.0f);
	m_HUDcircleFiller->setUnpressedTexture("FML");
	
	for (int i = 0; i < MAX_ENEMY_CIRCLES; i++)
	{
		Circle * c = new Circle();
		c->init(DirectX::XMFLOAT2A(0.95f, 0.15f), DirectX::XMFLOAT2A(.25f / 16.0f, .25f / 9.0f));
		c->setRadie(.5f);
		c->setUnpressedTexture("SPHERE");
		m_enemyCircles.push_back(c);
	}
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
		delete m_abilityComponents1[i];
	delete[] m_abilityComponents1;
	delete m_abilityComponents2[0];
	delete m_abilityComponents2[2];
	delete[] m_abilityComponents2;
	for (auto & s : m_sounds)
		AudioEngine::UnLoadSoundEffect(s);
	m_HUDcircle->Release();
	delete m_HUDcircle;
	m_HUDcircleFiller->Release();
	delete m_HUDcircleFiller;

	for (int i = 0; i < MAX_ENEMY_CIRCLES; i++)
	{
		m_enemyCircles[i]->Release();
		delete m_enemyCircles[i];
	}
}

void Player::Init(b3World& world, b3BodyType bodyType, float x, float y, float z)
{
	PhysicsComponent::Init(world, bodyType, x, y, z, false , 0);
	this->getBody()->SetObjectTag("PLAYER");
	this->getBody()->AddToFilters("TELEPORT");

	CreateShape(0, y, 0, x,y,z, "UPPERBODY");
	CreateShape(0, (y*1.5)+0.1, 0, 0.5, 0.5, 0.1, "HEAD");
	m_standHeight = (y*1.5) + 0.1;
	m_crouchHeight = y*1.1;
	setUserDataBody(this);

	setEntityType(EntityType::PlayerType);
	setColor(10, 10, 0, 1);

	setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	setScale(1.0f, 1.0f, 1.0f);
	setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	setTextureTileMult(2, 2);

	setHidden(true);
}

void Player::BeginPlay()
{

}
#include <math.h>
void Player::Update(double deltaTime)
{
	{
		using namespace DirectX;
		//calculate walk direction (-1, 1, based on camera) and movement speed
		{
			///Speed
			auto physSpeed = this->getLiniearVelocity();
			float speed = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSet(physSpeed.x, 0.0, physSpeed.z, 0)));
			m_currentSpeed = std::clamp(std::fabs(speed), 0.0f, 3.0f);

			///Walk dir
				//Get camera direction and normalize on X,Z plane
			auto cameraDir = p_camera->getDirection();
			XMVECTOR cameraDirNormalized = XMVector3Normalize(XMVectorSet(cameraDir.x, 0.0f, cameraDir.z, 0.0));
			///assert(XMVectorGetX(XMVector3Length(cameraDirNormalized)) != 0.0f);

			auto XZCameraDir = XMVectorSet(cameraDir.x, 0.0, cameraDir.z, 0.0);
			auto XZMovement = XMVectorSet(physSpeed.x, 0.0, physSpeed.z, 0.0);
			auto XZCameraDirNormalized = XMVector3Normalize(XZCameraDir);
			auto XZMovementNormalized = XMVector3Normalize(XZMovement);
			///AssertHasLength(XZCameraDir);
			//AssertHasLength(XZMovement);

				//Get dot product of cam dir and player movement
			auto dot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSet(physSpeed.x, 0, physSpeed.z, 0.0)), cameraDirNormalized));
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
	}
	const DirectX::XMFLOAT4A xmLP = p_camera->getPosition();
	FMOD_VECTOR fvLP = { xmLP.x, xmLP.y, xmLP.z, };
	//std::cout << getPosition().x << " " << getPosition().y << " " << getPosition().z << std::endl;

	using namespace DirectX;

	_hasWon();
	if (m_lockPlayerInput == false)
	{
		if (InputHandler::getWindowFocus())
		{
			_handleInput(deltaTime);
		}
	}

	m_HUDcircleFiller->setRadie((totVis)*.5f);

	

	m_activeSet[m_currentAbility]->Update(deltaTime);
	
	for (int i = 0; i < 4; i++)
	{
		if (i != m_currentAbility)
		{
			m_activeSet[i]->updateCooldown(deltaTime);
		}
	}

	_cameraPlacement(deltaTime);
	_updateFMODListener(deltaTime, xmLP);
	//HUDComponent::HUDUpdate(deltaTime);
	
	if (Input::SelectAbility1())
		m_currentAbility = (Ability)0;
	else if (Input::SelectAbility2())
		m_currentAbility = (Ability)1;
	else if (Input::SelectAbility3())
		m_currentAbility = (Ability)2;
	else if (Input::SelectAbility4())
		m_currentAbility = (Ability)3;
	
	if (GamePadHandler::IsUpDpadPressed())
		m_currentAbility = (Ability)0;
	else if (GamePadHandler::IsRightDpadPressed())
		m_currentAbility = (Ability)1;
	else if (GamePadHandler::IsDownDpadPressed())
		m_currentAbility = (Ability)2;
	else if (GamePadHandler::IsLeftDpadPressed())
		m_currentAbility = (Ability)3;


	if (m_tutorialActive)
	{
		bool isServer = Network::Multiplayer::GetInstance()->isServer();
		if (m_currentAbility == Ability::TELEPORT && isServer)
			m_abilityTutorialText->setString("Teleport Stone:\nHold button to throw further. \nPress again to teleport.");
		else if (m_currentAbility == Ability::VISIBILITY || m_currentAbility == Ability::VIS2)
			m_abilityTutorialText->setString("Visibility Sphere:\nSee how visible \nfor the guard you are.");
		else if (m_currentAbility == Ability::DISABLE && isServer)
			m_abilityTutorialText->setString("Rock:\nThrow to knock guards out.");
		else if (m_currentAbility == Ability::BLINK && !isServer)
			m_abilityTutorialText->setString("Phase:\nGo through cracks in walls.");
		else if (m_currentAbility == Ability::POSSESS && !isServer)
			m_abilityTutorialText->setString("Possess:\nControl guards.");
	}


	HUDComponent::ResetStates();
	HUDComponent::setSelectedQuad(m_currentAbility);
	for (int i = 0; i < 4; i++)
	{
		Quad * current =HUDComponent::GetQuad(i);
		if (m_activeSet[i]->getPercentage() <= 0.0f)
		{
			current->setV(1);
		}
		else
			current->setV(m_activeSet[i]->getPercentage());
	}

}

void Player::PhysicsUpdate()
{
	p_updatePhysics(this);
	_collision();
	//PhysicsComponent::p_setRotation(p_camera->getYRotationEuler().x, p_camera->getYRotationEuler().y, p_camera->getYRotationEuler().z);
	PhysicsComponent::p_setRotation(0, p_camera->getEulerRotation().y , 0);
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

const AudioEngine::Listener & Player::getFMODListener() const
{
	return m_FMODlistener;
}

void Player::SetAbilitySet(int set)
{
	if (set == 1)
		m_activeSet = m_abilityComponents1;
	else if (set == 2)
		m_activeSet = m_abilityComponents2;
}

void Player::setEnemyPositions(std::vector<Enemy*> enemys)
{
	using namespace DirectX;
	std::vector<DirectX::XMFLOAT2> relativEnemyPostions;
	totVis = 0;
	maxVis = 0;
	for (int i = 0; i < enemys.size(); i++)
	{
	
		DirectX::XMFLOAT2 pos = enemys[i]->GetDirectionToPlayer(getPosition(), *getCamera());
		if (pos.x > 0 || pos.y > 0)
		{
			relativEnemyPostions.push_back(enemys[i]->GetDirectionToPlayer(getPosition(), *getCamera()));
			if (enemys[i]->getTotalVisablilty() > totVis)
			{
				totVis = enemys[i]->getTotalVisablilty();
				maxVis = enemys[i]->getMaxVisability();
			}
		}
	}
	XMFLOAT2A finalPos = m_HUDcircle->getPosition();

	m_currentEnemysVisable = 0;
	for (int i = 0; i < relativEnemyPostions.size() && i < MAX_ENEMY_CIRCLES; i++)
	{
		m_currentEnemysVisable++;

		m_enemyCircles[i]->setPosition(XMFLOAT2A(finalPos.x + (relativEnemyPostions[i].x * (m_HUDcircle->getScale().x /4.0f) ),
			finalPos.y + (relativEnemyPostions[i].y * (m_HUDcircle->getScale().y / 4.0f))));
	}
}

TeleportAbility * Player::getTeleportAbility()
{
	TeleportAbility* tp = (TeleportAbility *)m_abilityComponents1[0];
	return tp;
}

void Player::Draw()
{
	for (int i = 0; i < m_nrOfAbilitys; i++)
	{
		
		m_activeSet[i]->Draw();
	}
	Drawable::Draw();

	HUDComponent::HUDDraw();
	m_HUDcircleFiller->Draw();
	m_HUDcircle->Draw();
	for (unsigned short i = 0; i < m_currentEnemysVisable; i++)
	{
		m_enemyCircles[i]->Draw();
	}
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

	bool isServer = Multiplayer::GetInstance()->isServer();
	//Same for every ability packet
	packet.id = ID_TIMESTAMP;
	packet.timeStamp = RakNet::GetTime();
	packet.m_id = ID_PLAYER_ABILITY;
	packet.ability = (unsigned int)NONE;

	TeleportAbility * tp_ptr = dynamic_cast<TeleportAbility*>(m_abilityComponents1[m_currentAbility]);
	DisableAbility * dis_ptr = dynamic_cast<DisableAbility*>(m_abilityComponents1[m_currentAbility]);
	VisabilityAbility * vis_ptr = dynamic_cast<VisabilityAbility*>(m_abilityComponents1[m_currentAbility]);
	//unique based on active ability
	switch (this->m_currentAbility)
	{
	case Ability::TELEPORT:
		if (isServer)
		{
			packet.ability = (unsigned int)TELEPORT;
			packet.start = tp_ptr->getStart();
			packet.velocity = tp_ptr->getVelocity();
			packet.state = tp_ptr->getState();
		}
		break;
	case Ability::DISABLE:
		if (isServer)
		{
			packet.ability = (unsigned int)DISABLE;
			packet.start = dis_ptr->getStart();
			packet.velocity = dis_ptr->getVelocity();
			packet.state = dis_ptr->getState();
		}
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

void Player::SendOnWin()
{
	Network::EVENTPACKET packet(Network::ID_PLAYER_WON);

	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
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

void Player::_collision()
{
	for (ContactListener::S_EndContact con : RipExtern::m_contactListener->GetEndContacts())
	{
		if (con.a->GetBody()->GetObjectTag() == "PLAYER" || con.b->GetBody()->GetObjectTag() == "PLAYER")
				if(con.a->GetObjectTag() == "HEAD" || con.b->GetObjectTag() == "HEAD")
				{
					m_allowPeek = true;
					m_recentHeadCollision = true;
				}
	}
	for (b3Contact * con : RipExtern::m_contactListener->GetBeginContacts())
	{
		if (con)
		{
			if (con->GetShapeA()->GetBody()->GetObjectTag() == "PLAYER" || con->GetShapeB()->GetBody()->GetObjectTag() == "PLAYER")
					if (con->GetShapeA()->GetObjectTag() == "HEAD" || con->GetShapeB()->GetObjectTag() == "HEAD")
					{
						m_allowPeek = false;
						peekDir = -LastPeekDir;
						m_peekRangeA = m_peektimer;
						m_peekRangeB = 0;
					}
		}
	}
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
	//_onJump();
	_onAbility(deltaTime);
	_onInteract();
	_onPeak(deltaTime);
	_onRotate(deltaTime);
	_objectInfo(deltaTime);
	_updateTutorial(deltaTime);
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

	DirectX::XMFLOAT2 dir = { Input::MoveRight(), Input::MoveForward() };
	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat2(&dir);
	float length = DirectX::XMVectorGetX(DirectX::XMVector2Length(vDir));

	if (length > 1.0)
		vDir = DirectX::XMVector2Normalize(vDir);

	DirectX::XMStoreFloat2(&dir, vDir);

	x = dir.x * m_moveSpeed  * RIGHT.x;
	x += dir.y * m_moveSpeed * forward.x;
	z = dir.y * m_moveSpeed * forward.z;
	z += dir.x * m_moveSpeed * RIGHT.z;

	//p_setPosition(getPosition().x + x, getPosition().y, getPosition().z + z);
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
				this->getBody()->GetShapeList()->GetNext()->SetSensor(true);
				crouchDir = 1;
				
				m_kp.crouching = true;
			}
		}
		else
		{
			if (m_kp.crouching)
			{
				crouchDir = -1;
				this->getBody()->GetShapeList()->GetNext()->SetSensor(false);
				
				m_kp.crouching = false;
			}
		}
	}

	if (m_kp.crouching)
	{
		m_moveSpeed = MOVE_SPEED * 0.5f;
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

		if (deltaX && (m_peekRotate + deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime) <= 0.5 && (m_peekRotate + deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime) >=-0.5)
		{
			p_camera->Rotate(0.0f, deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime, 0.0f);
			if (Input::PeekRight() > 0.1 || Input::PeekRight() < -0.1)
			{
				m_peekRotate += deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime;
			}
		}
		if (deltaY) 
		{
			if ((p_camera->getDirection().y - deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime) < 0.90f)
			{
				p_camera->Rotate(deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime, 0.0f, 0.0f);
			}
			else if (p_camera->getDirection().y >= 0.90f)
			{
				p_camera->setDirection(p_camera->getDirection().x, 0.89f, p_camera->getDirection().z);
			}
			if ((p_camera->getDirection().y - deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime) > -0.90f)
			{
				p_camera->Rotate(deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime, 0.0f, 0.0f);
			}
			else if (p_camera->getDirection().y <= -0.90f)
			{
				p_camera->setDirection(p_camera->getDirection().x, -0.89f, p_camera->getDirection().z);
			}
		
			
		}
		
	}
}


void Player::_onPeak(double deltaTime)
{
	if (Input::PeekRight() != 0) //Cap max peak range based on key input
	{
		if (m_allowPeek)
		{
			if (m_recentHeadCollision)
			{
				peekDir = LastPeekDir;
				m_peekRangeA = m_peektimer;
				m_peekRangeB = 0;
			}
			else
			{
				m_peekRangeA = Input::PeekRight();
				m_peekRangeB = -Input::PeekRight();

				if (Input::PeekRight() > 0) //Left Side
					peekDir = 1;
				if (Input::PeekRight() < 0) //Right Side
					peekDir = -1;

				LastPeekDir = peekDir;
			}
		}
		
	}
	else //Return to default pos
	{
		m_recentHeadCollision = false;
		peekDir = -LastPeekDir;
		m_peekRangeA = m_peektimer;
		m_peekRangeB = 0;
	}

}

void Player::_onInteract()
{
	if (Input::Interact())
	{
		if (m_kp.interact == false)
		{
			RayCastListener::Ray* ray = RipExtern::m_rayListener->ShotRay(this->getBody(), this->getCamera()->getPosition(), this->getCamera()->getDirection(), Player::INTERACT_RANGE, false);
			if (ray)
			{
				for (RayCastListener::RayContact* con : ray->GetRayContacts())
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
	this->m_activeSet[m_currentAbility]->Update(dt);
}

//Sends a ray every second and check if there is relevant data for the object to show on the screen
void Player::_objectInfo(double deltaTime)
{
	if (m_tutorialActive)
	{
		if (m_objectInfoTime >= 0.5f)
		{
			m_infoText->setString("");
			RayCastListener::Ray* ray = RipExtern::m_rayListener->ShotRay(getBody(), getCamera()->getPosition(), getCamera()->getDirection(), 10);
			if (ray != nullptr)
			{
				RayCastListener::RayContact* cContact = ray->getClosestContact();
				if (cContact->contactShape->GetBody()->GetObjectTag() == "LEVER" && cContact->fraction <= 0.3)
				{
					m_infoText->setString("Press X to pull");
				}
				else if (cContact->contactShape->GetBody()->GetObjectTag() == "TORCH")
				{
					//Snuff out torches (example)
				}
				else if (cContact->contactShape->GetBody()->GetObjectTag() == "ENEMY" && m_currentAbility == Ability::POSSESS)
				{
					m_infoText->setString("Press RB to possess");
					//Snuff out torches (example)
				}
				else if (cContact->contactShape->GetBody()->GetObjectTag() == "BLINK_WALL" && cContact->fraction <= 0.3  && m_currentAbility == Ability::BLINK)
				{
					m_infoText->setString("Press RB to pass");
					//m_infoText->setString("Illusory wall ahead");
					//Snuff out torches (example)
				}
			}
			m_objectInfoTime = 0;
		}

		m_objectInfoTime += deltaTime;
	}
}

void Player::_updateTutorial(double deltaTime)
{
	if (m_tutorialActive)
	{
		if (!m_tutorialMessages.empty())
		{
			if (m_tutorialDuration >= 5)
			{
				m_tutorialDuration = 0;
				m_tutorialText->setString(m_tutorialMessages.top());
				m_tutorialMessages.pop();
			}
			m_tutorialDuration += deltaTime;
		}
	}
}

void Player::_cameraPlacement(double deltaTime)
{
	//Head Movement
	b3Vec3 upperBodyLocal = this->getBody()->GetShapeList()->GetNext()->GetTransform().translation;
	b3Vec3 headPosLocal = this->getBody()->GetShapeList()->GetTransform().translation;


	//-------------------------------------------Peeking--------------------------------------------// 

	m_peektimer += peekDir * (float)deltaTime *m_peekSpeed;

	if (m_peekRangeB > m_peekRangeA)
		m_peektimer = std::clamp(m_peektimer, m_peekRangeA, m_peekRangeB);
	else
		m_peektimer = std::clamp(m_peektimer, m_peekRangeB, m_peekRangeA);
	
	//Offsets to the sides to slerp between
	b3Vec3 peekOffsetLeft;
	b3Vec3 peekOffsetRight;

	peekOffsetLeft.x = (upperBodyLocal.x - 1) * p_camera->getDirection().z;
	peekOffsetLeft.y = upperBodyLocal.y;
	peekOffsetLeft.z = (upperBodyLocal.z + 1)* p_camera->getDirection().x;

	peekOffsetRight.x = (upperBodyLocal.x + 1) * p_camera->getDirection().z;
	peekOffsetRight.y = upperBodyLocal.y;
	peekOffsetRight.z = (upperBodyLocal.z - 1) * p_camera->getDirection().x;

	headPosLocal += _slerp(peekOffsetRight, peekOffsetLeft, (m_peektimer+1)*0.5) - headPosLocal;

	//-------------------------------------------Crouch-------------------------------------------// 

	m_crouchAnimSteps += crouchDir * (float)deltaTime*m_crouchSpeed;
	m_crouchAnimSteps = std::clamp(m_crouchAnimSteps, 0.0f, 1.0f);
	headPosLocal.y += lerp(m_standHeight, m_crouchHeight, m_crouchAnimSteps) - m_standHeight;

	if (m_crouchAnimSteps == 1 || m_crouchAnimSteps == 0) //Animation Finished
	{
		crouchDir = 0;
	}

	//--------------------------------------Camera movement---------------------------------------// 
	b3Vec3 headPosWorld = this->getBody()->GetTransform().translation + headPosLocal;
	DirectX::XMFLOAT4A pos = DirectX::XMFLOAT4A(headPosWorld.x, headPosWorld.y, headPosWorld.z, 1.0f);
	p_camera->setPosition(pos);
	//Camera Tilt
	p_CameraTilting(deltaTime, m_peektimer);

	static float lastOffset = 0.0f;
	static bool hasPlayed = true;
	static int last = 0;

	//Head Bobbing
	float offsetY = p_viewBobbing(deltaTime, Input::MoveForward(), m_moveSpeed, p_moveState);

	pos.y += offsetY;

	//Footsteps
	if (p_moveState == Walking || p_moveState == Sprinting)
	{
		if (!hasPlayed)
		{
			if (lastOffset < offsetY)
			{
				hasPlayed = true;
				auto xmPos = getPosition();
				FMOD_VECTOR at = { xmPos.x, xmPos.y, xmPos.z };
				int index = -1;
				while (index == -1 || index == last)
				{
					index = rand() % (int)m_sounds.size();
				}
				FMOD::Channel * c = nullptr;
				c = AudioEngine::PlaySoundEffect(m_sounds[index], &at, AudioEngine::Player);
				b3Vec3 vel = getLiniearVelocity();
				DirectX::XMVECTOR vVel = DirectX::XMVectorSet(vel.x, vel.y, vel.z, 0.0f);
				float speed = DirectX::XMVectorGetX(DirectX::XMVector3Length(vVel));

				speed *= 0.1;
				speed -= 0.2f;
				c->setVolume(speed);
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

	this->getBody()->GetShapeList()->SetTransform(headPosLocal, getBody()->GetQuaternion());
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
	this->getBody()->GetShapeList()->GetNext()->SetSensor(true);
	crouchDir = 1;
	m_kp.crouching = true;
}

void Player::_deActivateCrouch()
{
	this->getBody()->GetShapeList()->GetNext()->SetSensor(false);
	crouchDir = -1;
	m_kp.crouching = false;
}

void Player::_hasWon()
{
	for (int i = 0; i < RipExtern::m_contactListener->GetBeginContacts().size(); i++)
	{
		if (RipExtern::m_contactListener->GetBeginContacts()[i]->GetShapeA()->GetBody()->GetObjectTag() == "PLAYER")
		{
			if (RipExtern::m_contactListener->GetBeginContacts()[i]->GetShapeB()->GetBody()->GetObjectTag() == "WIN_BOX")
			{
				hasWon = true;
				SendOnWin();
				
				break;
			}
		}
		else if(RipExtern::m_contactListener->GetBeginContacts()[i]->GetShapeA()->GetBody()->GetObjectTag() == "WIN_BOX")
		{
			if (RipExtern::m_contactListener->GetBeginContacts()[i]->GetShapeB()->GetBody()->GetObjectTag() == "PLAYER")
			{
				hasWon = true;
				SendOnWin();
				break;
			}
		}
	}
	if (gameIsWon == true)
		drawWinBar();
}

b3Vec3 Player::_slerp(b3Vec3 start, b3Vec3 end, float percent)
{
	// Dot product - the cosine of the angle between 2 vectors.
	float dot = b3Dot(start, end);
	// Clamp it to be in the range of Acos()
	// This may be unnecessary, but floating point
	// precision can be a fickle mistress.
	dot = std::clamp(dot, -1.0f, 1.0f);
	// Acos(dot) returns the angle between start and end,
	// And multiplying that by percent returns the angle between
	// start and the final result.
	float theta = std::acosf(dot)*percent;
	//float theta = mathf.Acos(dot)*percent;
	b3Vec3 tempStart = start;
	tempStart.x *= dot;
	tempStart.y *= dot;
	tempStart.z *= dot;
	b3Vec3 relativeVec = end - tempStart;
	b3Normalize(relativeVec);   // Orthonormal basis
	// The final result.
	tempStart = start;
	tempStart.x *= std::cos(theta);
	tempStart.y *= std::cos(theta);
	tempStart.z *= std::cos(theta);

	b3Vec3 tempRelativeVec = relativeVec;
	tempRelativeVec.x *= std::sin(theta);
	tempRelativeVec.y *= std::sin(theta);
	tempRelativeVec.z *= std::sin(theta);


	return (tempStart + tempRelativeVec);
}

void Player::drawWinBar()
{
	m_winBar->setPosition(0.5f, 0.5f);
}

