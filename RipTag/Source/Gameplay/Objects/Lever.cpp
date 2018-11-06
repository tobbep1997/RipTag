#include "Lever.h"
#include "../../../RipTagExtern/RipExtern.h"
#include <iostream>
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
	for (RayCastListener::RayContact con : RipExtern::m_rayListener->GetContacts())
	{
		if (con.originBody->GetObjectTag() == "PLAYER")
		{
			if (con.contactShape->GetBody()->GetObjectTag() == getBody()->GetObjectTag())
			{
				if (static_cast<Lever*>(con.contactShape->GetBody()->GetUserData()) == this)
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

					*con.consumeState +=1;
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
