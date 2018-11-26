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
	auto activateState = stateMachine->AddPlayOnceState("activate", Manager::g_animationManager.getAnimation("PLATE", "PLATE_ACTIVATE_ANIMATION").get());
	auto deactivateState = stateMachine->AddPlayOnceState("deactivate", Manager::g_animationManager.getAnimation("PLATE", "PLATE_DEACTIVATE_ANIMATION").get());
	activateState->SetBlendTime(0.0f);
	deactivateState->SetBlendTime(0.0f);
	getAnimationPlayer()->Pause();
	BaseActor::setUserDataBody(this);
}

void PressurePlate::Update(double deltaTime)
{
	p_updatePhysics(this);
	bool previousState = this->getTriggerState();
	ContactListener::S_Contact con;
	for(int i = 0; i < RipExtern::g_contactListener->GetNrOfEndContacts(); i++)
	{
		con = RipExtern::g_contactListener->GetEndContact(i);
		b3Shape * shapeA = con.a;
		b3Shape * shapeB = con.b;
		if ((shapeA->GetBody()->GetObjectTag() == "PLAYER" || shapeA->GetBody()->GetObjectTag() == "ENEMY") ||
			(shapeB->GetBody()->GetObjectTag() == "ENEMY" || shapeB->GetBody()->GetObjectTag() == "PLAYER"))
			if ((shapeA->GetBody()->GetObjectTag() == "PressurePlate") || (shapeB->GetBody()->GetObjectTag() == "PressurePlate"))
			{
				if (static_cast<PressurePlate*>(shapeA->GetBody()->GetUserData()) == this ||
					static_cast<PressurePlate*>(shapeB->GetBody()->GetUserData()) == this)
				{
					if (this->getTriggerState())
					{
						this->setTriggerState(false);
						this->SendOverNetwork();
					}
				}
			}
	}

	for (int i = 0; i < RipExtern::g_contactListener->GetNrOfBeginContacts(); i++)
	{
		con = RipExtern::g_contactListener->GetBeginContact(i);
		b3Shape * shapeA = con.a;
		b3Shape * shapeB = con.b;
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


