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
	if (pParticles)
	{
		pParticles->Update(deltaTime, pCamera);
	}

	if (m_interacted)
	{
		if (this->getTriggerState())
		{
			this->setTriggerState(false);
			pPointLight->setLightOn(true);
			pParticles = new ParticleEmitter();
			pParticles->setPosition(this->getPosition().x, this->getPosition().y, this->getPosition().z);
		}
		else
		{
			this->pPointLight->setLightOn(false);
			this->setTriggerState(true);
			delete pParticles;
			pParticles = nullptr;
		}
		//SENDTriggerd here for network
		this->SendOverNetwork();
		m_interacted = false;
	}
	p_updatePhysics(this);
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
	_playSound();
}

void Torch::handleContact(RayCastListener::RayContact * contact)
{
	if (contact->contactShape->GetBody()->GetObjectTag() == getBody()->GetObjectTag())
	{
		Torch* ObjectPointer = static_cast<Torch*>(contact->contactShape->GetBody()->GetUserData());
		if (ObjectPointer == this)
		{
			m_interacted = true;
		}
	}
}

void Torch::_playSound(AudioEngine::SoundType st)
{
	//UNCOMMENT WHEN READY
	FMOD_VECTOR at = { getPosition().x, getPosition().y, getPosition().z };
	if (!this->getTriggerState())
		m_channel = AudioEngine::PlaySoundEffect(RipSounds::g_torch, &at, st);
	else
		m_channel->stop();
}
