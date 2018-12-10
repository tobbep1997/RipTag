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
	for(int i = 0; i < (int)RipExtern::g_contactListener->GetNrOfEndContacts(); i++)
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
					bool useTrigger = true;
					//Do a check with the remote player if he is inside this objects bounding box, we have a pointer to RemotePlayer in extern
					//I am assuming the DirectX BoundingBox is equal to the Bounce BB. IF this is not the case, find a solution with the BOUNCE BB.  
					if (DX::g_remotePlayer)
					{
						DirectX::XMFLOAT4A remotePlayerPos = DX::g_remotePlayer->getPosition();
						b3AABB bb;
						if (static_cast<PressurePlate*>(shapeA->GetBody()->GetUserData()) == this)
							shapeA->ComputeAabb(bb, shapeA->GetTransform());
						else
							shapeB->ComputeAabb(bb, shapeB->GetTransform());
						
						//Put the position into the same x-z plane
						remotePlayerPos.y = bb.GetCenter().y;
						if (bb.ContainsPoint(b3Vec3((r32)remotePlayerPos.x, (r32)remotePlayerPos.y, (r32)remotePlayerPos.z)))
							useTrigger = false;
					}
					if (this->getTriggerState() && useTrigger)
					{
						if(shapeA->GetBody()->GetObjectTag() == "ENEMY" || shapeB->GetBody()->GetObjectTag() == "ENEMY")
							this->setTriggerState(false, true, "ENEMY");
						else
							this->setTriggerState(false);
						this->SendOverNetwork();
					}
				}
			}
	}

	for (int i = 0; i < (int)RipExtern::g_contactListener->GetNrOfBeginContacts(); i++)
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
								if (shapeA->GetBody()->GetObjectTag() == "ENEMY" || shapeB->GetBody()->GetObjectTag() == "ENEMY")
									this->setTriggerState(true, true, "ENEMY");
								else
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

void PressurePlate::_playSound(AudioEngine::SoundDesc * soundDesc)
{
	FMOD_VECTOR at = { getPosition().x, getPosition().y, getPosition().z };
	if (!this->getTriggerState())
		AudioEngine::PlaySoundEffect(RipSounds::g_pressurePlateDeactivate, &at, soundDesc);
	else
		AudioEngine::PlaySoundEffect(RipSounds::g_pressurePlateActivate, &at, soundDesc);
}


