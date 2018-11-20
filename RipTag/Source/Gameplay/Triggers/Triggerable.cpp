#include "RipTagPCH.h"
#include "Triggerable.h"

Triggerable::Triggerable()
{
}


Triggerable::~Triggerable()
{
}

void Triggerable::setState(bool state)
{
	if (!m_isActivated && state)
	{
		//Play activated state
		m_isActivated = state;
		this->getAnimationPlayer()->GetStateMachine()->SetState(this->activatedAnimation);
		this->getAnimationPlayer()->Play();
	}
	else if (m_isActivated && !state)
	{
		//Play Inactivated state
		m_isActivated = state;
		this->getAnimationPlayer()->GetStateMachine()->SetState(this->deactivatedAnimation);
		this->getAnimationPlayer()->Play();
	}
}

void Triggerable::BeginPlay()
{
}

void Triggerable::Update(double deltaTime)
{
	
}

void Triggerable::Release()
{
	PhysicsComponent::Release(*RipExtern::g_world);
}

Triggerable::Triggerable(int uniqueId, int linkedID, bool isTrigger, std::string activeAnim, std::string deactivAnim)
{
	this->m_uniqueID = uniqueId;
	this->m_linkedID = linkedID;
	this->m_isActivated = false;
	this->activatedAnimation = activeAnim;
	this->deactivatedAnimation = deactivAnim;

}
