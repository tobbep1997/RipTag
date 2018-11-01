#include "RemotePlayer.h"



RemotePlayer::RemotePlayer(RakNet::NetworkID nID, DirectX::XMFLOAT4A pos, DirectX::XMFLOAT4A scale, DirectX::XMFLOAT4A rot) : Actor()
{
	//TODO:
	//1. Load the correct mesh and configure it
	//2. Set the transform
	//3. Assign the NID
	//4. Set the correct entity type
	//5. Push the intial state on the stack
	//6. Register abilities
	
	//1.
	this->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	this->setTexture(Manager::g_textureManager.getTexture("SPHERE"));

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
	VisabilityAbility * visAbl = new VisabilityAbility();
	visAbl->setOwner(this);
	visAbl->setIsLocal(false);
	visAbl->Init();

	VisabilityAbility * visAbl2 = new VisabilityAbility();
	visAbl2->setOwner(this);
	visAbl2->setIsLocal(false);
	visAbl2->Init();

	TeleportAbility * m_teleport = new TeleportAbility();
	m_teleport->setOwner(this);
	m_teleport->setIsLocal(false);
	m_teleport->Init();

	DisableAbility * m_dis = new DisableAbility();
	m_dis->setOwner(this);
	m_dis->setIsLocal(false);
	m_dis->Init();

	m_abilityComponents = new AbilityComponent*[m_nrOfAbilitys];
	m_abilityComponents[0] = m_teleport;
	m_abilityComponents[1] = visAbl;
	m_abilityComponents[2] = m_dis;
	m_abilityComponents[3] = visAbl2;
}


RemotePlayer::~RemotePlayer()
{
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
	}
}

void RemotePlayer::Update(double dt)
{
	//TODO:
	//1. Handle the state machine (transitions are handled in the called functions)
	//2. Update the ability compononent

	//1.
	switch (this->m_stateStack.top())
	{
	case PlayerState::Idle:
		this->_Idle(dt);
		break;
	case PlayerState::Walking:
		this->_Walking(dt);
		break;
	case PlayerState::Crouching:
		this->_Crouching(dt);
		break;
	case PlayerState::Sprinting:
		this->_Sprinting(dt);
		break;
	case PlayerState::Jumping:
		this->_Jumping(dt);
		break;
	case PlayerState::Falling:
		this->_Falling(dt);
		break;
	}

	//2.
	m_abilityComponents[m_currentAbility]->Update(dt);
}

void RemotePlayer::Draw()
{
	for (int i = 0; i < m_nrOfAbilitys; i++)
		m_abilityComponents[i]->Draw();
	Drawable::Draw();
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
	m_currentAbility = (Ability)data->ability;
	m_abilityComponents[m_currentAbility]->UpdateFromNetwork(data);
}

void RemotePlayer::_Idle(float dt)
{
	//Play the idle animation
	this->_lerpPosition(dt);
}

void RemotePlayer::_Walking(float dt)
{
	//play the walking animation
	this->_lerpPosition(dt);
}

void RemotePlayer::_Crouching(float dt)
{
	//play the crouching animation
	this->_lerpPosition(dt);
}

void RemotePlayer::_Sprinting(float dt)
{
	//play sprinting animation
	this->_lerpPosition(dt);
}

void RemotePlayer::_Jumping(float dt)
{
	//Play Jumping animation
	this->_lerpPosition(dt);
}

void RemotePlayer::_Falling(float dt)
{
	//play falling animation
	this->_lerpPosition(dt);
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
