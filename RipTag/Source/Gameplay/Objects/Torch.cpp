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
	if (pPointLight)
	{
		delete pPointLight;
		pPointLight = nullptr;
	}
	if (pParticles)
	{
		delete pParticles;
		pParticles = nullptr;
	}
	
	Release();
}

void Torch::Update(double deltaTime)
{
	//Check wether to crate new fire.
	if (pParticles == nullptr)
	{
		std::string thisObject = getBody()->GetObjectTag();
		RayCastListener::Ray* ray;
		for (int i = 0; i < RipExtern::g_rayListener->getNrOfProcessedRays(); i++)
		{
			ray = RipExtern::g_rayListener->GetProcessedRay(i);
			for (int k = 0; k < ray->getNrOfContacts(); k++)
			{
				RayCastListener::RayContact* con = ray->GetRayContact(k);
				std::string originObject = ray->getOriginBody()->GetObjectTag();
				std::string contactObject = con->contactShape->GetBody()->GetObjectTag();

				bool onPlayer = originObject == "PLAYER" && contactObject == thisObject;
				bool onEnemy = originObject == "ENEMY" && contactObject == thisObject;

				if (onPlayer || onEnemy)
				{
					Torch* ObjectPointer = static_cast<Torch*>(con->contactShape->GetBody()->GetUserData());
					if (ObjectPointer == this)
					{
						if (this->getTriggerState())
						{
							this->setTriggerState(false);
							pPointLight->setLightOn(true);
							pParticles = new ParticleEmitter();
							pParticles->setPosition(this->getPosition().x, this->getPosition().y, this->getPosition().z);
							m_hasChecked = true;
						}
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
	
	p_updatePhysics(this);

	if (pParticles != nullptr && !m_hasChecked)
	{
		pParticles->Update(deltaTime, pCamera);

		std::string thisObject = getBody()->GetObjectTag();
		RayCastListener::Ray* ray;
		for (int i = 0; i < RipExtern::g_rayListener->getNrOfProcessedRays(); i++)
		{
			ray = RipExtern::g_rayListener->GetProcessedRay(i);
			for (int k = 0; k < ray->getNrOfContacts(); k++)
			{
				RayCastListener::RayContact* con = ray->GetRayContact(k);
				std::string originObject = ray->getOriginBody()->GetObjectTag();
				std::string contactObject = con->contactShape->GetBody()->GetObjectTag();

				bool onPlayer = originObject == "PLAYER" && contactObject == thisObject;
				bool onEnemy = originObject == "ENEMY" && contactObject == thisObject;

				if (onPlayer || onEnemy)
				{
					Torch* ObjectPointer = static_cast<Torch*>(con->contactShape->GetBody()->GetUserData());
					if (ObjectPointer == this)
					{
						if (this->getTriggerState())
						{
							this->setTriggerState(false);
							pPointLight->setLightOn(true);
						}
						else if(!this->getTriggerState() && !m_hasChecked)
						{
							this->setTriggerState(true);
							pPointLight->setLightOn(false);
							delete pParticles;
							pParticles = nullptr;
						}
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
	m_hasChecked = false; 
}

void Torch::Draw()
{
	Drawable::Draw();
	if (!this->getTriggerState())
	{
		this->pPointLight->QueueLight();
		pParticles->setEmmiterLife(0);
	}
		
	if(pParticles != nullptr)
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
