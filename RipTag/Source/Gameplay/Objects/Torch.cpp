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

	m_tourchSound.emitter = AudioEngine::Other;
	m_tourchSound.loudness = 1.5f;
	m_tourchSound.owner = this;

	
	/*FMOD_VECTOR at = { lightPos.x, lightPos.y,lightPos.z };
	m_channel = AudioEngine::PlaySoundEffect(RipSounds::g_torch, &at, &m_tourchSound);
	m_channel->setVolume(0.6f);*/
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
	if (m_preState != getTriggerState())
		_playSound(&m_tourchSound);

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
			pParticles->setEmmiterLife(0);
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
	if (this->HasPacketReceived())
	{
		if (!this->getTriggerState())
		{
			this->setTriggerState(false);
			pPointLight->setLightOn(true);
			pParticles = new ParticleEmitter();
			pParticles->setPosition(this->getPosition().x, this->getPosition().y, this->getPosition().z);
			pParticles->setEmmiterLife(0);
		}
		else
		{
			this->pPointLight->setLightOn(false);
			this->setTriggerState(true);
			delete pParticles;
			pParticles = nullptr;
		}
		this->SetPacketReceived(false);
	}
	p_updatePhysics(this);
	this->m_preState = this->getTriggerState();
}

void Torch::Draw()
{
	Drawable::Draw();
	if (!this->getTriggerState())
	{
		this->pPointLight->QueueLight();
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
	_playSound(&m_tourchSound);
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

void Torch::_playSound(AudioEngine::SoundDesc * soundDesc)
{
	FMOD_VECTOR at = { getPosition().x, getPosition().y, getPosition().z };
	if (!this->getTriggerState())
		m_channel = AudioEngine::PlaySoundEffect(RipSounds::g_torch, &at, soundDesc);
	else
		m_channel->stop();
	m_channel->setVolume(0.6f);
}
