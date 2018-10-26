#include "RemotePlayer.h"



RemotePlayer::RemotePlayer(b3World &world, RakNet::NetworkID nID, float x, float y, float z) : Actor(), PhysicsComponent()
{
	//TODO:
	//1. Load the correct mesh and configure it
	//2. Set the position
	//3. Assign the NID
	//4. Set the correct entity type
	//5. Push the intial state on the stack
	
	//1.
	this->Init(world, e_dynamicBody);
	this->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	this->setScale(1.0f, 1.0f, 1.0f);
	this->setTexture(Manager::g_textureManager.getTexture("SPHERE"));

	//2.
	this->setPosition(DirectX::XMFLOAT4A(x, y, z, 1.0f));
	this->p_setPosition(x, y, z);

	//3.
	this->networkID = nID;

	//4.
	this->setEntityType(EntityType::DefultType);

	//5.
	this->m_stateStack.push(PlayerState::Idle);
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
	case GAME_MESSAGES::ID_PLAYER_MOVE:
		this->_onMovement((ENTITY_MOVE*)data);
		break;
	case GAME_MESSAGES::ID_PLAYER_JUMP:
		this->_onJumpEvent((ENTITY_EVENT*)data);
		break;
	}
}

void RemotePlayer::Update(double dt)
{
	//TODO:
	//1. Handle the state machine (transitions are handled in the called functions)

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

}

void RemotePlayer::PhysicsUpdate()
{
	this->p_updatePhysics(this);
	this->setLiniearVelocity(0, this->getLiniearVelocity().y, 0);
}

void RemotePlayer::Draw()
{
	Drawable::Draw();
}

void RemotePlayer::_onMovement(Network::ENTITY_MOVE * data)
{
	//Check the NID first then update the current velocity
	if (this->networkID == data->nID)
	{
		//Determine if we need to update the state stack
		//data->state can be walking, sprinting and crouching
		switch (this->m_stateStack.top())
		{
		//under these circumstances the stack is locked 
		case PlayerState::Jumping:
		case PlayerState::Falling:
		case PlayerState::CheckingVisibility:
		case PlayerState::InBlink:
		case PlayerState::InTeleport:
			break;
		case PlayerState::Idle:
			this->m_stateStack.push((PlayerState)data->state);
			break;
		case PlayerState::Walking:
			if (data->state != PlayerState::Walking)
			{
				//if we have a different movement state update the stack
				this->m_stateStack.pop();
				this->m_stateStack.push((PlayerState)data->state);
			}
			break;
		case PlayerState::Sprinting:
			if (data->state != PlayerState::Sprinting)
			{
				this->m_stateStack.pop();
				this->m_stateStack.push((PlayerState)data->state);
			}
			break;
		case PlayerState::Crouching:
			if (data->state != PlayerState::Crouching)
			{
				this->m_stateStack.pop();
				this->m_stateStack.push((PlayerState)data->state);
			}
			break;

		default:
			break;
		}

		//In any case we always apply the given velocity
		//this->setPosition(DirectX::XMFLOAT4A(data->x, data->y, data->z, 0.0f));
		this->setLiniearVelocity(data->x, getLiniearVelocity().y, data->z);
	}
}

void RemotePlayer::_onJumpEvent(Network::ENTITY_EVENT * data)
{
	//Check NID first then check the current state machine
	if (this->networkID == data->nID)
	{
		//We can jump under these circumstances
		switch (this->m_stateStack.top())
		{
		case PlayerState::Idle:
		case PlayerState::Walking:
		case PlayerState::Sprinting:
			//We can apply jump from this state
			this->addForceToCenter(0.0f, 200.0f, 0.0f);
			this->m_stateStack.push(PlayerState::Jumping);
			break;
		default:
			break;
		}
	}
}

void RemotePlayer::_Idle(float dt)
{
	//Play the idle animation
}

void RemotePlayer::_Walking(float dt)
{
	//play the walking animation
}

void RemotePlayer::_Crouching(float dt)
{
	//play the crouching animation
}

void RemotePlayer::_Sprinting(float dt)
{
	//play sprinting animation
}

void RemotePlayer::_Jumping(float dt)
{
	if (this->getLiniearVelocity().y < 0)
	{
		this->m_stateStack.pop();
		this->m_stateStack.push(PlayerState::Falling);
	}
	else
	{
		//Play Jumping animation

	}
}

void RemotePlayer::_Falling(float dt)
{
	float epsilon = 0.0002;

	if (this->getLiniearVelocity().y < epsilon && this->getLiniearVelocity().y > -epsilon)
	{
		this->m_stateStack.pop();
	}
	else
	{
		//play falling animation

	}
}
