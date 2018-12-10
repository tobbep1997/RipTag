#include "RipTagPCH.h"
#include "Lever.h"

#include <AudioEngine.h>
Lever::Lever()
{
}

Lever::Lever(int uniqueId, int linkedID, bool isTrigger) : Trigger(uniqueId, linkedID, isTrigger, "activate", "deactivate")
{
	
}


Lever::~Lever()
{
	//PhysicsComponent::Release(*RipExtern::g_world);
}

void Lever::Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll)
{
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, 1.0f, 1.0f, 1.0f, true);
	BaseActor::setPositionRot(xPos, yPos, zPos, pitch, yaw, roll);
	BaseActor::setObjectTag("LEVER");
	setTexture(Manager::g_textureManager.getTexture("SPAK"));
	BaseActor::setModel(Manager::g_meshManager.getSkinnedMesh("SPAK"));//BYT TILL SPAK
	auto& machine = getAnimationPlayer()->InitStateMachine(2);
	getAnimationPlayer()->SetSkeleton(Manager::g_animationManager.getSkeleton("SPAK"));
	auto activateState = machine->AddPlayOnceState("activate", Manager::g_animationManager.getAnimation("SPAK", "SPAK_ACTIVATE_ANIMATION").get());
	auto deactivateState = machine->AddPlayOnceState("deactivate", Manager::g_animationManager.getAnimation("SPAK", "SPAK_DEACTIVATE_ANIMATION").get());
	activateState->SetDefaultBlendTime(0.0f);
	deactivateState->SetDefaultBlendTime(0.0f);
	getAnimationPlayer()->Pause();
	BaseActor::setUserDataBody(this);
}


void Lever::Update(double deltaTime)
{
	p_updatePhysics(this);

	if (m_interacted)
	{
		if (this->getTriggerState())
		{
			this->setTriggerState(false);
		}
		else
		{
			this->setTriggerState(true);
		}
		//SENDTriggerd here for network
		this->SendOverNetwork();
		m_interacted = false;
	}

	this->getAnimationPlayer()->Update(deltaTime);
}

void Lever::BeginPlay()
{
}
void Lever::handleContact(RayCastListener::RayContact& contact)
{
	if (contact.contactShape->GetBody()->GetObjectTag() == getBody()->GetObjectTag())
	{
		if (static_cast<Lever*>(contact.contactShape->GetBody()->GetUserData()) == this)
		{
			m_interacted = true;
		}
	}
}

void Lever::_playSound(AudioEngine::SoundDesc * soundDesc)
{
	FMOD_VECTOR at = { getPosition().x, getPosition().y, getPosition().z };
	if (this->getTriggerState())
		AudioEngine::PlaySoundEffect(RipSounds::g_leverActivate, &at, soundDesc);
	else
		AudioEngine::PlaySoundEffect(RipSounds::g_leverDeactivate, &at, soundDesc);
}
