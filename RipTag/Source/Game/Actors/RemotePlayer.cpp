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
	this->setModel(Manager::g_meshManager.getDynamicMesh("STATE"));
	this->setTexture(Manager::g_textureManager.getTexture("STATE"));
	//this->setModelTransform(XMMatrixRotationRollPitchYaw(0.0, 90.0, 0.0));
	//2.
	this->setPosition(pos);
	this->setScale(scale);
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

	//7.
	this->_registerAnimationStateMachine();
}


RemotePlayer::~RemotePlayer()
{
	for (int i = 0; i < m_nrOfAbilitys; i++)
		delete m_abilityComponents1[i];
	delete[] m_abilityComponents1;
	delete m_abilityComponents2[0];
	delete m_abilityComponents2[2];
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
	}
}

void RemotePlayer::Update(double dt)
{
	//TODO:
	//1. Update position
	//2. Update the ability compononent
	//3. Update animation

	//1.
	this->_lerpPosition(dt);

	//2.
	m_activeSet[m_currentAbility]->Update(dt);

	//3.
	this->getAnimatedModel()->Update(dt);
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
	if ((Ability)data->ability != Ability::NONE)
	{
		m_currentAbility = (Ability)data->ability;
		m_abilityComponents1[m_currentAbility]->UpdateFromNetwork(data);
	}
}

void RemotePlayer::_onNetworkAnimation(Network::ENTITYANIMATIONPACKET * data)
{
	if (this->networkID == data->nid)
	{
		this->m_currentDirection = data->direction;
		this->m_currentSpeed = data->speed;
		this->setRotation(data->rot);
	}
}


void RemotePlayer::_lerpPosition(float dt)
{
	DirectX::XMVECTOR curr, next, newPos;

	curr = DirectX::XMLoadFloat4A(&this->getPosition());
	next = DirectX::XMLoadFloat4A(&this->m_mostRecentPosition);

	newPos = DirectX::XMVectorLerp(curr, next, dt*this->m_timeDiff);

	DirectX::XMFLOAT4A _newPos;
	DirectX::XMStoreFloat4A(&_newPos, newPos);

	this->setPosition(_newPos);
}

void RemotePlayer::_registerAnimationStateMachine()
{
	std::vector<SharedAnimation> sharedAnimations;
	const char * collection = "STATE";
	int nrOfStates = 2;

	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "IDLE_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "WALK_FORWARD_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "WALK_BACKWARD_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "WALK_LEFT2_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "WALK_RIGHT2_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "WALK_BLEFT_ANIMATION"));
	sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "WALK_BRIGHT_ANIMATION"));

	this->getAnimatedModel()->SetPlayingClip(sharedAnimations[IDLE].get());
	this->getAnimatedModel()->Play();
	this->getAnimatedModel()->SetSkeleton(Manager::g_animationManager.getSkeleton(collection));

	std::unique_ptr<SM::AnimationStateMachine>& stateMachine = this->getAnimatedModel()->InitStateMachine(nrOfStates);

	{
		//Blend spaces - forward&backward
		SM::BlendSpace2D * blend_fwd = stateMachine->AddBlendSpace2DState(
			"walk_forward", //state name
			&this->m_currentDirection, //x-axis driver
			&this->m_currentSpeed, //y-axis driver
			-90.f, 90.f, //x-axis bounds
			0.0f, 3.001f //y-axis bounds
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
				{ sharedAnimations[IDLE].get(), -90.f }, //the clip to use and x-placement
				{ sharedAnimations[IDLE].get(), 0.f },
				{ sharedAnimations[IDLE].get(), 90.f }
			}
		);
		blend_fwd->AddRow(
			3.1f, //y placement
			{	//uses a vector initializer list for "convinience"
				{ sharedAnimations[RIGHT].get(), -90.f }, //the clip to use and x-placement
				{ sharedAnimations[FORWARD].get(), 0.f },
				{ sharedAnimations[LEFT].get(), 90.f }
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

		//Adding out state / transitions
		SM::OutState & fwd_bwd_outstate = blend_fwd->AddOutState(blend_bwd);
		//Add transition condition
		fwd_bwd_outstate.AddTransition(
			&this->m_currentDirection, //referenced variable for comparision
			-90.f, 90.f, //bound range for comparision
			SM::COMPARISON_OUTSIDE_RANGE //comparision condition
		);

		SM::OutState & bwd_fwd_outstate = blend_bwd->AddOutState(blend_fwd);
		//Add transition condition
		bwd_fwd_outstate.AddTransition(
			&this->m_currentDirection, //referenced variable for comparision
			-90.f, 90.f, //bound range for comparision
			SM::COMPARISON_INSIDE_RANGE //comparision condition
		);

		//set initial state
		stateMachine->SetState("walk_forward");
	}

}
