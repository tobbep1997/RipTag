#include "RipTagPCH.h"
#include "Trigger.h"



void Trigger::p_trigger(const bool & trigger)
{
	this->m_triggerd = trigger;
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

const bool & Trigger::Triggerd() const
{
	return m_triggerd;
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
