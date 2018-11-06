#include "RipTagPCH.h"
#include "Lever.h"

#include <AudioEngine.h>
Lever::Lever()
{
}


Lever::~Lever()
{
}

void Lever::Init()
{
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, 1.0f, 1.0f, 1.0f, true);
	p_setPosition(getPosition().x, getPosition().y, getPosition().z);
	setObjectTag("LEVER");
	setUserDataBody(this);
	lock = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/RazerClickLock.ogg");
	unlock = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/RazerClickUnlock.ogg");
}

void Lever::BeginPlay()
{

}

void Lever::Update(double deltaTime)
{
	p_updatePhysics(this);
	for (RayCastListener::Ray* ray : RipExtern::m_rayListener->GetRays())
	{
		for (RayCastListener::RayContact* con : ray->GetRayContacts())
		{
			if (con->originBody->GetObjectTag() == "PLAYER" && con->contactShape->GetBody()->GetObjectTag() == getBody()->GetObjectTag())
			{
				if (static_cast<Lever*>(con->contactShape->GetBody()->GetUserData()) == this && *con->consumeState != 2)
				{
					if (static_cast<Lever*>(con->contactShape->GetBody()->GetUserData()) == this)
					{
						auto pos = getPosition();
						FMOD_VECTOR fVector = { pos.x, pos.y, pos.z };
						p_trigger(!Triggerd());
						if (Triggerd())
						{
							AudioEngine::PlaySoundEffect(unlock, &fVector);
							//AudioEngine::PlaySoundEffect(lock, &fVector);
						}
						else
						{
							//AudioEngine::PlaySoundEffect(unlock, &fVector);
							AudioEngine::PlaySoundEffect(lock, &fVector);
						}

						*con->consumeState += 1;
					}
				}
			}
		}
	}
}
