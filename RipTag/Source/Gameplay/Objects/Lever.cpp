#include "RipTagPCH.h"
#include "Lever.h"

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
	BaseActor::setModel(Manager::g_meshManager.getDynamicMesh("SPAK"));//BYT TILL SPAK
	auto& machine = getAnimatedModel()->InitStateMachine(1);
	getAnimatedModel()->SetSkeleton(Manager::g_animationManager.getSkeleton("SPAK"));
	machine->AddPlayOnceState("activate", Manager::g_animationManager.getAnimation("SPAK", "SPAK_ACTIVATE_ANIMATION").get());
	machine->AddPlayOnceState("deactivate", Manager::g_animationManager.getAnimation("SPAK", "SPAK_ACTIVATE_ANIMATION").get());
	getAnimatedModel()->Pause();
	BaseActor::setUserDataBody(this);
}


void Lever::Update(double deltaTime)
{
	p_updatePhysics(this);
	for (RayCastListener::RayContact con : RipExtern::m_rayListener->GetContacts())
	{
		if (con.originBody->GetObjectTag() == "PLAYER")
		{
			if (con.contactShape->GetBody()->GetObjectTag() == getBody()->GetObjectTag())
			{
				if (static_cast<Lever*>(con.contactShape->GetBody()->GetUserData()) == this)
				{
					if (this->getTriggerState())
						this->setTriggerState(false);
					else
						this->setTriggerState(true);
						
					*con.consumeState +=1;
					//SENDTriggerd here for network
					this->SendOverNetwork();
				}
			}
		}
	}
	this->getAnimatedModel()->Update(deltaTime);
}

bool Lever::isEqual(Lever * target)
{
	if (this->getPosition().x == target->getPosition().x && 
		this->getPosition().y == target->getPosition().y && 
		this->getPosition().z == target->getPosition().z)
		return true;

	return false;
}
