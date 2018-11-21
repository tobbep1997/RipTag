#include "RipTagPCH.h"
#include "Trigger.h"



void Trigger::p_trigger(const bool & trigger)
{
	this->m_triggerState = trigger;
}

Trigger::Trigger()
{
}

Trigger::Trigger(int uniqueId, int linkedID, bool isTrigger, std::string activeAnim, std::string deactiveAnim)
{
	this->m_isTrigger = isTrigger;
	this->m_linkedID = linkedID;
	this->m_uniqueID = uniqueId;
	this->activatedAnimation = activeAnim;
	this->deactivatedAnimation = deactiveAnim;

}

Trigger::~Trigger()
{
}

const bool & Trigger::Triggered() const
{
	return m_triggerState;
}

void Trigger::setTriggerState(bool state, bool isLocal)
{
	if (m_linkedID == 3)
		int lol = 0;
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
		this->_playSound();
	else
		this->_playSound(AudioEngine::RemotePlayer);
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
