#include "RipTagPCH.h"
#include "Torch.h"


Torch::Torch(PointLight * pLight, ParticleEmitter * pParticleEmitter, int _uniqueId) : Trigger(_uniqueId, -1, true, "", "")
{
	this->pPointLight = pLight;
	this->pParticles = pParticleEmitter;
	DirectX::XMFLOAT4A lightPos = pPointLight->getPosition();
	
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, 1.0f, 1.0f, 1.0f, true);
	BaseActor::setPosition(lightPos.x, lightPos.y, lightPos.z);
	BaseActor::setObjectTag("TORCH");
	//Need a model for torch
	BaseActor::setModel(Manager::g_meshManager.getStaticMesh("TORCH"));
	BaseActor::setUserDataBody(this);
}


Torch::~Torch()
{
	/*if (pPointLight)
	{
		delete pPointLight;
		pPointLight = nullptr;
	}
	if (pParticles)
	{
		delete pParticles;
		pParticles = nullptr;
	}
	Trigger::Release();*/
}

void Torch::Update(double deltaTime)
{
	p_updatePhysics(this);
	pParticles->Update(deltaTime, pCamera);
	std::string thisObject = getBody()->GetObjectTag();
	for (RayCastListener::Ray* ray : RipExtern::g_rayListener->GetRays())
	{
		for (RayCastListener::RayContact* con : ray->GetRayContacts())
		{
			std::string originObject = con->originBody->GetObjectTag();
			std::string contactObject = con->contactShape->GetBody()->GetObjectTag();

			bool onPlayer = originObject == "PLAYER" && contactObject == thisObject;
			bool onEnemy = originObject == "ENEMY" && contactObject == thisObject;

			if (onPlayer || onEnemy)
			{
				Torch* ObjectPointer = static_cast<Torch*>(con->contactShape->GetBody()->GetUserData());
				if ( ObjectPointer == this && *con->consumeState != 2)
				{
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
	//this needs to be here because of over network state change
	if (this->getTriggerState())
	{
		this->pPointLight->setLightOn(false);
	}
	else
	{
		this->pPointLight->setLightOn(true);
	}
}

void Torch::Draw()
{
	Drawable::Draw();
	if (pPointLight->getLightOn())
	{
		this->pPointLight->QueueLight();
		pParticles->setEmmiterLife(0);
	}
	else
		pParticles->setEmmiterLife(1);
	
	pParticles->Queue();
}

void Torch::QueueLight()
{
	this->pPointLight->QueueLight();
}

void Torch::BeginPlay()
{
}

void Torch::_playSound(AudioEngine::SoundType st)
{
	//UNCOMMENT WHEN READY
	/*FMOD_VECTOR at = { getPosition().x, getPosition().y, getPosition().z };
	if (this->getTriggerState())
		AudioEngine::PlaySoundEffect(RipSounds::g_torchLit, &at, st);
	else
		AudioEngine::PlaySoundEffect(RipSounds::g_torchPutOut, &at, st);*/
}
