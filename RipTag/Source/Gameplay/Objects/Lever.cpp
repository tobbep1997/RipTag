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
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, 1.0f, 1.0f, 1.0f, false);
	BaseActor::setPositionRot(xPos, yPos, zPos, pitch, yaw, roll);
	BaseActor::setObjectTag("LEVER");
	setTexture(Manager::g_textureManager.getTexture("SPAK"));
	BaseActor::setModel(Manager::g_meshManager.getSkinnedMesh("SPAK"));//BYT TILL SPAK
	auto& machine = getAnimationPlayer()->InitStateMachine(1);
	getAnimationPlayer()->SetSkeleton(Manager::g_animationManager.getSkeleton("SPAK"));
	machine->AddPlayOnceState("activate", Manager::g_animationManager.getAnimation("SPAK", "SPAK_ACTIVATE_ANIMATION").get());
	machine->AddPlayOnceState("deactivate", Manager::g_animationManager.getAnimation("SPAK", "SPAK_DEACTIVATE_ANIMATION").get());
	getAnimationPlayer()->Pause();
	BaseActor::setUserDataBody(this);
}


void Lever::Update(double deltaTime)
{
	p_updatePhysics(this);
	for (RayCastListener::Ray* ray : RipExtern::g_rayListener->GetRays())
	{
		for (RayCastListener::RayContact* con : ray->GetRayContacts())
		{
			if (con->originBody->GetObjectTag() == "PLAYER" && con->contactShape->GetBody()->GetObjectTag() == getBody()->GetObjectTag())
			{
				if (static_cast<Lever*>(con->contactShape->GetBody()->GetUserData()) == this && *con->consumeState != 2)
				{
					auto pos = getPosition();
					FMOD_VECTOR fVector = { pos.x, pos.y, pos.z };

					if (this->getTriggerState())
					{
						this->setTriggerState(false);
					}
					else
					{
						this->setTriggerState(true);
					}
					*(con->consumeState) += 1;
					//SENDTriggerd here for network
					this->SendOverNetwork();
				}
			}
		}
	}
	this->getAnimationPlayer()->Update(deltaTime);
}

void Lever::BeginPlay()
{
}

void Lever::_playSound(AudioEngine::SoundType st)
{
	FMOD_VECTOR at = { getPosition().x, getPosition().y, getPosition().z };
	if (this->getTriggerState())
		AudioEngine::PlaySoundEffect(RipSounds::g_leverActivate, &at, st);
	else
		AudioEngine::PlaySoundEffect(RipSounds::g_leverDeactivate, &at, st);
}
