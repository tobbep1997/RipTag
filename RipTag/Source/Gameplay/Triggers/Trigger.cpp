#include "RipTagPCH.h"
#include "Trigger.h"



void Trigger::p_trigger(const bool & trigger)
{
	this->m_triggerState = trigger;
}

Trigger::Trigger()
{
}

Trigger::Trigger(int uniqueId, int linkedID, bool isTrigger)
{
	this->m_isTrigger = isTrigger;
	this->m_linkedID = linkedID;
	this->m_uniqueID = uniqueId;

}

Trigger::~Trigger()
{
}

const bool & Trigger::Triggered() const
{
	return m_triggerState;
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
