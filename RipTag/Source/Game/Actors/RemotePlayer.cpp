#include "RipTagPCH.h"
#include "RemotePlayer.h"

RemotePlayer::RemotePlayer(RakNet::NetworkID nID, DirectX::XMFLOAT4A pos, DirectX::XMFLOAT4A scale, DirectX::XMFLOAT4A rot) : Actor()
{
	using namespace DirectX;
	//TODO:
	//1. Load the correct mesh and configure it
	//2. Set the transform
	//3. Assign the NID
	//4. Set the correct entity type
	//5. Push the intial state on the stack
	//6. Register abilities
	//7. Register animation state machine
	
	//1.
	this->setModel(Manager::g_meshManager.getSkinnedMesh("PLAYER1"));
	this->setTexture(Manager::g_textureManager.getTexture("PLAYER1"));
	//this->setModelTransform(XMMatrixRotationRollPitchYaw(0.0, 90.0, 0.0));
	//2.
	this->setPosition(pos);
	this->setScale(.45, .45, .45);
	this->setModelTransform(XMMatrixTranslation(0.0, -1.1, 0.0));
	this->setRotation(rot);
	this->m_mostRecentPosition = pos;
	this->m_timeDiff = 0;

	//3.
	this->networkID = nID;

	//4.
	this->setEntityType(EntityType::DefultType);

	//5.
	this->m_stateStack.push(PlayerState::Idle);

	//6.
	//Ability stuff
	{
		TeleportAbility * m_teleport = new TeleportAbility();
		m_teleport->setOwner(this);
		m_teleport->setIsLocal(false);
		m_teleport->Init();

		DisableAbility * m_dis = new DisableAbility();
		m_dis->setOwner(this);
		m_dis->setIsLocal(false);
		m_dis->Init();

		BlinkAbility * m_blink = new BlinkAbility();
		m_blink->setOwner(this);
		m_blink->setIsLocal(false);
		m_blink->Init();

		PossessGuard * m_possess = new PossessGuard();
		m_possess->setOwner(this);
		m_possess->setIsLocal(false);
		m_possess->Init();

		m_abilityComponents1 = new AbilityComponent*[m_nrOfAbilitys];
		m_abilityComponents1[0] = m_teleport;
		m_abilityComponents1[1] = m_dis;

		m_abilityComponents2 = new AbilityComponent*[m_nrOfAbilitys];
		m_abilityComponents2[0] = m_blink;
		m_abilityComponents2[1] = m_possess;

		m_currentAbility = (Ability)0;

		//By default always this set
		m_activeSet = m_abilityComponents1;
	}

	//7.
	this->_registerAnimationStateMachine();
}


RemotePlayer::~RemotePlayer()
{
	for (unsigned short int i = 0; i < m_nrOfAbilitys; i++)
		delete m_abilityComponents1[i];
	delete[] m_abilityComponents1;
	for (unsigned short int i = 0; i < m_nrOfAbilitys; i++)
		delete m_abilityComponents2[i];
	delete[] m_abilityComponents2;
}

void RemotePlayer::BeginPlay()
{
}

void RemotePlayer::HandlePacket(unsigned char id, unsigned char * data)
{
	using namespace Network;
	switch (id)
	{
	case NETWORKMESSAGES::ID_PLAYER_UPDATE:
		this->_onNetworkUpdate((Network::ENTITYUPDATEPACKET*)data);
		break;
	case NETWORKMESSAGES::ID_PLAYER_ABILITY:
		this->_onNetworkAbility((Network::ENTITYABILITYPACKET*)data);
		break;
	case NETWORKMESSAGES::ID_PLAYER_ANIMATION:
		this->_onNetworkAnimation((Network::ENTITYANIMATIONPACKET*)data);
		break;
	case NETWORKMESSAGES::ID_PLAYER_THROW_BEGIN:
		this->_onNetworkRemoteThrow(id);
		break;
	case NETWORKMESSAGES::ID_PLAYER_THROW_END:
		this->_onNetworkRemoteThrow(id);
		break;
	case NETWORKMESSAGES::ID_PLAYER_POSESS_BEGIN:
		this->_onNetworkRemotePosess(id);
		break;
	case NETWORKMESSAGES::ID_PLAYER_POSESS_END:
		this->_onNetworkRemotePosess(id);
		break;
	case NETWORKMESSAGES::ID_PLAYER_CROUCH_BEGIN:
		this->_onNetworkRemoteCrouch(id);
		break;
	case NETWORKMESSAGES::ID_PLAYER_CROUCH_END:
		this->_onNetworkRemoteCrouch(id);
		break;
	default:
		break;
	}
}

void RemotePlayer::Update(double dt)
{
	static Network::Multiplayer * pNetwork = Network::Multiplayer::GetInstance();
	//TODO:
	//1. Update position
	//2. Update the ability compononent
	//3. Update animation
	//4. send visibility data is we are server

	//1.
	this->_lerpPosition(dt);
	std::cout << this->getPosition().x << std::endl;
	//2.
	for (size_t i = 0; i < m_nrOfAbilitys; i++)
		m_activeSet[i]->Update(dt);

	//3.
	this->getAnimationPlayer()->Update(dt);

	//4.
	if (pNetwork->isServer())
		_sendVisibilityPacket();
}

void RemotePlayer::Draw()
{
	for (int i = 0; i < m_nrOfAbilitys; i++)
		m_activeSet[i]->Draw();
	Drawable::Draw();
}

void RemotePlayer::SetAbilitySet(int set)
{
	if (set == 1)
		m_activeSet = m_abilityComponents1;
	else if (set == 2)
		m_activeSet = m_abilityComponents2;
}

void RemotePlayer::_onNetworkUpdate(Network::ENTITYUPDATEPACKET * data)
{
	//Check the NID first then update the current position
	if (this->networkID == data->nid)
	{
		//Determine if we need to update the state stack
		//if the new state is different from the current state
		//we have to handle the transition animation
		if (!m_stateStack.top() == data->state)
		{
			handleTransition = true;
			m_stateStack.push((PlayerState)data->state);
		}

		//In any case we always apply position and rotation
		//this->setPosition(data->pos);
		this->m_mostRecentPosition = data->pos;
		this->setRotation(data->rot);
		this->m_timeDiff = RakNet::GetTime() - data->timeStamp;
	}
}

void RemotePlayer::_onNetworkAbility(Network::ENTITYABILITYPACKET * data)
{
	if ((Ability)data->ability != Ability::NONE && !data->isCommonUpadate)
	{
		m_currentAbility = (Ability)data->ability;
		m_abilityComponents1[m_currentAbility]->UpdateFromNetwork(data);
	}
	else if (data->isCommonUpadate)
	{
		m_abilityComponents1[data->ability]->UpdateFromNetwork(data);
	}
}

void RemotePlayer::_onNetworkAnimation(Network::ENTITYANIMATIONPACKET * data)
{
	if (this->networkID == data->nid)
	{
		this->m_currentDirection = data->direction;
		this->m_currentSpeed = data->speed;
		this->m_currentPitch = data->pitch;
		this->setRotation(data->rot);

		std::cout << m_currentSpeed << std::endl;
	}
}

void RemotePlayer::_sendVisibilityPacket()
{
	//Network::VISIBILITYPACKET packet;
	//packet.value = m_currentVisibility;
	/*Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), LOW_PRIORITY);*/
}


void RemotePlayer::_lerpPosition(float dt)
{
	DirectX::XMVECTOR curr, next, newPos;

	curr = DirectX::XMLoadFloat4A(&this->getPosition());
	next = DirectX::XMLoadFloat4A(&this->m_mostRecentPosition);

	if (DirectX::XMVector3IsNaN(curr) || DirectX::XMVector3IsNaN(next))
		std::cout << "isNAN\n";

	if (!DirectX::XMVector3IsNaN(curr) && !DirectX::XMVector3IsNaN(next))
	{

		newPos = DirectX::XMVectorLerp(curr, next, 0.5f);

		DirectX::XMFLOAT4A _newPos;
		DirectX::XMStoreFloat4A(&_newPos, newPos);

		assert(!DirectX::XMVector3IsNaN(newPos));

		if (!DirectX::XMVector3IsNaN(newPos))
			this->setPosition(_newPos);
		else
			this->setPosition(this->m_mostRecentPosition);
	}
}

void RemotePlayer::_registerAnimationStateMachine()
{
	std::vector<SharedAnimation> sharedAnimations;
	const char * collection = "PLAYER1";
	int nrOfStates = 5;

	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "IDLE_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_F_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_B_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_FL_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_FR_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_BL_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_BR_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "R_F_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "R_FL_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "R_FR_ANIMATION"));

	this->getAnimationPlayer()->Play();
	this->getAnimationPlayer()->SetSkeleton(Manager::g_animationManager.getSkeleton(collection));

	std::unique_ptr<SM::AnimationStateMachine>& stateMachine = this->getAnimationPlayer()->InitStateMachine(nrOfStates);

	{
		//Blend spaces - forward&backward
		SM::BlendSpace2D * blend_fwd = stateMachine->AddBlendSpace2DState(
			"walk_forward", //state name
			&this->m_currentDirection, //x-axis driver
			&this->m_currentSpeed, //y-axis driver
			-115.f, 115.f, //x-axis bounds
			0.0f, 6.0f //y-axis bounds
		);
		SM::BlendSpace2D * blend_bwd = stateMachine->AddBlendSpace2DState(
			"walk_backward", //state name
			&this->m_currentDirection, //x-axis driver
			&this->m_currentSpeed, //y-axis driver
			-180.f, 180.f, //x-axis bounds
			0.0f, 3.001f //y-axis bounds
		);

		//Add blendspace rows 
		//forward
		blend_fwd->AddRow(
			0.0f, //y placement
			{	//uses a vector initializer list for "convinience"
				{ sharedAnimations[IDLE].get(), -115.f }, //the clip to use and x-placement
				{ sharedAnimations[IDLE].get(), 0.f },
				{ sharedAnimations[IDLE].get(), 115.f }
			}
		);
		blend_fwd->AddRow(
			3.1f, //y placement
			{	//uses a vector initializer list for "convinience"
				{ sharedAnimations[RIGHT].get(), -115.f }, //the clip to use and x-placement
				{ sharedAnimations[FORWARD].get(), 0.f },
				{ sharedAnimations[LEFT].get(), 115.f }
			}
		);
		blend_fwd->AddRow(
			6.0f, //y placement
			{	//uses a vector initializer list for "convinience"
				{ sharedAnimations[RUN_FORWARD_RIGHT].get(), -115.f }, //the clip to use and x-placement
				{ sharedAnimations[RUN_FORWARD].get(), 0.f },
				{ sharedAnimations[RUN_FORWARD_LEFT].get(), 115.f }
			}
		);
		//
		blend_bwd->AddRow(
			0.0f, //y placement
			{	//uses a vector initializer list for "convinience"
				{ sharedAnimations[IDLE].get(), -180.f }, //the clip to use and x-placement
				{ sharedAnimations[IDLE].get(), -90.f },
				{ sharedAnimations[IDLE].get(), 0.f },
				{ sharedAnimations[IDLE].get(), 90.f },
				{ sharedAnimations[IDLE].get(), 180.f }
			}
		);
		blend_bwd->AddRow(
			3.1f, //y placement
			{	//uses a vector initializer list for "convinience"
				{ sharedAnimations[BACKWARD].get(), -180.f }, //the clip to use and x-placement
				{ sharedAnimations[BACK_RIGHT].get(), -90.f },
				{ sharedAnimations[FORWARD].get(), 0.f },
				{ sharedAnimations[BACK_LEFT].get(), 90.f },
				{ sharedAnimations[BACKWARD].get(), 180.f }
			}
		);
		blend_bwd->AddRow(
			6.0f, //y placement
			{	//uses a vector initializer list for "convinience"
				{ sharedAnimations[BACKWARD].get(), -180.f }, //the clip to use and x-placement
				{ sharedAnimations[BACK_RIGHT].get(), -90.f },
				{ sharedAnimations[FORWARD].get(), 0.f },
				{ sharedAnimations[BACK_LEFT].get(), 90.f },
				{ sharedAnimations[BACKWARD].get(), 180.f }
			}
		);

		//Adding out state / transitions
		SM::OutState & fwd_bwd_outstate = blend_fwd->AddOutState(blend_bwd);
		//Add transition condition
		fwd_bwd_outstate.AddTransition(
			&this->m_currentDirection, //referenced variable for comparision
			-115.f, 115.f, //bound range for comparision
			SM::COMPARISON_OUTSIDE_RANGE //comparision condition
		);

		SM::OutState & bwd_fwd_outstate = blend_bwd->AddOutState(blend_fwd);
		//Add transition condition
		bwd_fwd_outstate.AddTransition(
			&this->m_currentDirection, //referenced variable for comparision
			-90.f, 90.f, //bound range for comparision
			SM::COMPARISON_INSIDE_RANGE //comparision condition
		);

		auto throwBeginClip = Manager::g_animationManager.getAnimation(collection, "THROW_BEGIN_ANIMATION").get();
		auto throwHoldClip = Manager::g_animationManager.getAnimation(collection, "THROW_HOLD_ANIMATION").get();
		auto throwEndClip = Manager::g_animationManager.getAnimation(collection, "THROW_END_ANIMATION").get();
		auto posessClip = Manager::g_animationManager.getAnimation(collection, "POSESSING_ANIMATION").get();
		auto crouchClip = Manager::g_animationManager.getAnimation(collection, "CROUCH_POSE_ANIMATION").get();
		auto lookUpPose = &Manager::g_animationManager.getAnimation(collection, "LOOK_UP_ANIMATION").get()->m_SkeletonPoses[0];
		auto lookDownPose = &Manager::g_animationManager.getAnimation(collection, "LOOK_DOWN_ANIMATION").get()->m_SkeletonPoses[0];

		auto holdState = stateMachine->AddLoopState("throw_hold", throwHoldClip);
		stateMachine->AddAutoTransitionState("throw_begin", throwBeginClip, holdState);
		auto throwEndState = stateMachine->AddAutoTransitionState("throw_end", throwEndClip, blend_fwd);
		throwEndState->SetBlendTime(0.05f);
		auto posessState = stateMachine->AddLoopState("posessing", posessClip);
		posessState->SetBlendTime(0.3f);
		//set initial state
		stateMachine->SetState("walk_forward");

		auto& layerMachine = this->getAnimationPlayer()->InitLayerMachine(Manager::g_animationManager.getSkeleton(collection).get());
		auto crouchState = layerMachine->AddBasicLayer("crouch", crouchClip, 0.0, 0.0);
		crouchState->UseFirstPoseOnly(true);
		auto pitchState = layerMachine->Add1DPoseLayer("pitch", &m_currentPitch, -1.0f, 1.0f, { {lookUpPose, -1.0f}, {lookDownPose, 1.0f} });
		pitchState->UseSmoothDriver(false);
		layerMachine->ActivateLayer("pitch");
	}
}

void RemotePlayer::_onNetworkRemoteThrow(unsigned char id)
{
	using namespace Network;

	switch (id)
	{
	case NETWORKMESSAGES::ID_PLAYER_THROW_BEGIN:
		this->getAnimationPlayer()->GetStateMachine()->SetState("throw_begin");
		break;
	case NETWORKMESSAGES::ID_PLAYER_THROW_END:
		this->getAnimationPlayer()->GetStateMachine()->SetState("throw_end");
		break;
	}
}

void RemotePlayer::_onNetworkRemotePosess(unsigned char id)
{
	using namespace Network;

	switch (id)
	{
	case NETWORKMESSAGES::ID_PLAYER_POSESS_BEGIN:
		this->getAnimationPlayer()->GetStateMachine()->SetState("posessing");
		break;
	case NETWORKMESSAGES::ID_PLAYER_POSESS_END:
		this->getAnimationPlayer()->GetStateMachine()->SetState("walk_forward");
		break;
	}
}

void RemotePlayer::_onNetworkRemoteCrouch(unsigned char id)
{
	using namespace Network;

	switch (id)
	{
	case NETWORKMESSAGES::ID_PLAYER_CROUCH_BEGIN:
		this->getAnimationPlayer()->GetLayerMachine()->ActivateLayer("crouch");
		break;
	case NETWORKMESSAGES::ID_PLAYER_CROUCH_END:
		this->getAnimationPlayer()->GetLayerMachine()->PopLayer("crouch");
		break;
	}
}
