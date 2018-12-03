#include "RipTagPCH.h"
#include "Trigger.h"



void Trigger::p_trigger(const bool & trigger)
{
	this->m_triggerState = trigger;
	m_soundDesc.emitter = AudioEngine::Other;
	m_soundDesc.loudness = 1.0f;
	m_soundDesc.owner = this;
}

Trigger::Trigger()
{
	m_soundDesc.emitter = AudioEngine::Other;
	m_soundDesc.loudness = 1.0f;
	m_soundDesc.owner = this;
}

Trigger::Trigger(int uniqueId, int linkedID, bool isTrigger, std::string activeAnim, std::string deactiveAnim)
{
	this->m_isTrigger = isTrigger;
	this->m_linkedID = linkedID;
	this->m_uniqueID = uniqueId;
	this->activatedAnimation = activeAnim;
	this->deactivatedAnimation = deactiveAnim;
	m_soundDesc.emitter = AudioEngine::Other;
	m_soundDesc.loudness = 1.0f;
	m_soundDesc.owner = this;
}

Trigger::~Trigger()
{
}

const bool & Trigger::Triggered() const
{
	return m_triggerState;
}

void Trigger::setTriggerState(bool state, bool isLocal, const std::string & triggerer)
{
	
	if (!m_triggerState && state)
	{
		//Play activated state
		m_triggerState = state;
		if (this->getAnimationPlayer())
		{
			this->getAnimationPlayer()->GetStateMachine()->SetState(this->activatedAnimation);
			this->getAnimationPlayer()->Play();
		}
	}
	else if (m_triggerState && !state)
	{
		//Play Inactivated state
		m_triggerState = state;
		if (this->getAnimationPlayer())
		{
			this->getAnimationPlayer()->GetStateMachine()->SetState(this->deactivatedAnimation);
			this->getAnimationPlayer()->Play();
		}
	}
	if (isLocal)
	{
		if (triggerer == "PLAYER")
		{
			m_soundDesc.emitter = AudioEngine::Player;
			this->_playSound(&m_soundDesc);
		}
		else
		{
			m_soundDesc.emitter = AudioEngine::Other;
			this->_playSound(&m_soundDesc);
		}

	}
	else
	{
		m_soundDesc.emitter = AudioEngine::RemotePlayer;
		this->_playSound(&m_soundDesc);
	}
}

void Trigger::BeginPlay()
{
}

void Trigger::Update(double deltaTime)
{
	
}

void Trigger::Release()
{
	PhysicsComponent::Release(*RipExtern::g_world);
}

void Trigger::SendOverNetwork()
{
	if (Network::Multiplayer::GetInstance()->isConnected())
	{
		Network::TRIGGEREVENTPACKET packet(Network::ID_TRIGGER_USED, this->m_uniqueID, this->m_triggerState);
		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::TRIGGEREVENTPACKET), PacketPriority::LOW_PRIORITY);
	}
}
