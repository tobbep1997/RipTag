#include "RipTagPCH.h"
#include "PressurePlate.h"


PressurePlate::PressurePlate() : Trigger()
{
	
}

PressurePlate::PressurePlate(int uniqueId, int linkedID, bool isTrigger) : Trigger(uniqueId, linkedID, isTrigger, "activate", "deactivate")
{
	
}


PressurePlate::~PressurePlate()
{
	//PhysicsComponent::Release(*RipExtern::g_world);
}

void PressurePlate::Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float bboxScaleX, float bboxScaleY, float bboxScaleZ, float scaleX, float scaleY, float scaleZ)
{
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, bboxScaleX, bboxScaleY, bboxScaleZ, true);
	BaseActor::setPositionRot(xPos, yPos, zPos, pitch, yaw, roll);
	BaseActor::setScale(scaleX, scaleY, scaleZ);
	BaseActor::setObjectTag("PressurePlate");
	BaseActor::setModel(Manager::g_meshManager.getDynamicMesh("PLATE"));
	auto& stateMachine = getAnimatedModel()->InitStateMachine(2);
	getAnimatedModel()->SetSkeleton(Manager::g_animationManager.getSkeleton("PLATE"));
	stateMachine->AddPlayOnceState("activate", Manager::g_animationManager.getAnimation("PLATE", "PLATE_ACTIVATE_ANIMATION").get());
	stateMachine->AddPlayOnceState("deactivate", Manager::g_animationManager.getAnimation("PLATE", "PLATE_DEACTIVATE_ANIMATION").get());
	getAnimatedModel()->Pause();
	BaseActor::setUserDataBody(this);
}

void PressurePlate::Update(double deltaTime)
{
	p_updatePhysics(this);
	bool previousState = this->getTriggerState();

	for (ContactListener::S_EndContact con : RipExtern::m_contactListener->GetEndContacts())
	{
		if ((con.a->GetBody()->GetObjectTag() == "PLAYER" || con.a->GetBody()->GetObjectTag() == "ENEMY") ||
			(con.b->GetBody()->GetObjectTag() == "ENEMY" || con.b->GetBody()->GetObjectTag() == "PLAYER"))
			if ((con.a->GetBody()->GetObjectTag() == "PressurePlate") || (con.b->GetBody()->GetObjectTag() == "PressurePlate"))
			{
				this->setTriggerState(false);
			}
	}
	for (b3Contact * con : RipExtern::m_contactListener->GetBeginContacts())
	{
		if ((con->GetShapeA()->GetBody()->GetObjectTag() == "PLAYER" || con->GetShapeA()->GetBody()->GetObjectTag() == "ENEMY") || 
			(con->GetShapeB()->GetBody()->GetObjectTag() == "ENEMY" || con->GetShapeB()->GetBody()->GetObjectTag() == "PLAYER"))
			if ((con->GetShapeB()->GetBody()->GetObjectTag() == "PressurePlate") || (con->GetShapeA()->GetBody()->GetObjectTag() == "PressurePlate"))
			{
				this->setTriggerState(true);
				this->SendOverNetwork();
			}
	}
	//If previous state was true, but the new state is false no one is one the plate locally
	if (previousState && !this->getTriggerState())
		this->SendOverNetwork();

	this->getAnimatedModel()->Update(deltaTime);
}


