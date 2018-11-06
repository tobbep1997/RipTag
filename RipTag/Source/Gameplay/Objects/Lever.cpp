#include "RipTagPCH.h"
#include "Lever.h"

Lever::Lever()
{
}

Lever::Lever(int uniqueId, int linkedID, bool isTrigger) : Trigger(uniqueId, linkedID, isTrigger) 
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
	BaseActor::setModel(Manager::g_meshManager.getDynamicMesh("SPAK"));//BYT TILL SPAK
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
					//set lever animation here
					//SENDTriggerd here for network
					this->SendOverNetwork();
				}
			}
		}
	}
	//std::cout << Triggerd() << std::endl;
}

bool Lever::isEqual(Lever * target)
{
	if (this->getPosition().x == target->getPosition().x && 
		this->getPosition().y == target->getPosition().y && 
		this->getPosition().z == target->getPosition().z)
		return true;

	return false;
}
