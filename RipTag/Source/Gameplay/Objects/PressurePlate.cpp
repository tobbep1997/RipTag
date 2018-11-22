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
	setTexture(Manager::g_textureManager.getTexture("PLATE"));
	BaseActor::setModel(Manager::g_meshManager.getSkinnedMesh("PLATE"));
	auto& stateMachine = getAnimationPlayer()->InitStateMachine(2);
	getAnimationPlayer()->SetSkeleton(Manager::g_animationManager.getSkeleton("PLATE"));
	stateMachine->AddPlayOnceState("activate", Manager::g_animationManager.getAnimation("PLATE", "PLATE_ACTIVATE_ANIMATION").get());
	stateMachine->AddPlayOnceState("deactivate", Manager::g_animationManager.getAnimation("PLATE", "PLATE_DEACTIVATE_ANIMATION").get());
	getAnimationPlayer()->Pause();
	BaseActor::setUserDataBody(this);
}

void PressurePlate::Update(double deltaTime)
{
	p_updatePhysics(this);
	bool previousState = this->getTriggerState();

	for (ContactListener::S_EndContact con : RipExtern::g_contactListener->GetEndContacts())
	{
		if ((con.a->GetBody()->GetObjectTag() == "PLAYER" || con.a->GetBody()->GetObjectTag() == "ENEMY") ||
			(con.b->GetBody()->GetObjectTag() == "ENEMY" || con.b->GetBody()->GetObjectTag() == "PLAYER"))
			if ((con.a->GetBody()->GetObjectTag() == "PressurePlate") || (con.b->GetBody()->GetObjectTag() == "PressurePlate"))
			{
				if (static_cast<PressurePlate*>(con.a->GetBody()->GetUserData()) == this ||
					static_cast<PressurePlate*>(con.b->GetBody()->GetUserData()) == this)
				{
					if (this->getTriggerState())
					{
						this->setTriggerState(false);
						this->SendOverNetwork();
					}
				}
			}
	}

	for (b3Contact * con : RipExtern::g_contactListener->GetBeginContacts())
	{
		if (con)
		{
			b3Shape * shapeA = con->GetShapeA();
			b3Shape * shapeB = con->GetShapeB();
			if (shapeA && shapeB)
			{
				b3Body * bodyA = shapeA->GetBody();
				b3Body * bodyB = shapeB->GetBody();

				if (bodyA && bodyB)
				{
					std::string objectTagA = bodyA->GetObjectTag();
					std::string objectTagB = bodyB->GetObjectTag();

					if ((objectTagA == "PLAYER" || objectTagA == "ENEMY") ||
						(objectTagB == "PLAYER" || objectTagB == "ENEMY"))
					{
						if (objectTagA == "PressurePlate" || objectTagB == "PressurePlate")
						{
							if (static_cast<PressurePlate*>(bodyA->GetUserData()) == this ||
								static_cast<PressurePlate*>(bodyB->GetUserData()) == this)
							{
								if (!this->getTriggerState())
								{
									this->setTriggerState(true);
									this->SendOverNetwork();
								}
							}
						}
					}
				}
			}
		}
	}
	

	//If previous state was true, but the new state is false no one is one the plate locally
	//if (previousState && !this->getTriggerState())
		//this->SendOverNetwork();

	this->getAnimationPlayer()->Update(deltaTime);
}

void PressurePlate::_playSound(AudioEngine::SoundType st)
{
	FMOD_VECTOR at = { getPosition().x, getPosition().y, getPosition().z };
	if (!this->getTriggerState())
		AudioEngine::PlaySoundEffect(RipSounds::g_pressurePlateDeactivate, &at, st);
	else
		AudioEngine::PlaySoundEffect(RipSounds::g_pressurePlateActivate, &at, st);
}


