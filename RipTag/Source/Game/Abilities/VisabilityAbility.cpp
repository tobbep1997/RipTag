#include "VisabilityAbility.h"
#include "../Actors/Player.h"
#include "EngineSource/3D Engine/RenderingManager.h"


VisabilityAbility::VisabilityAbility()
{
}


VisabilityAbility::~VisabilityAbility()
{
	delete m_visSphere;
}

void VisabilityAbility::Init()
{
	m_visSphere = new Drawable();
	m_visSphere->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	m_visSphere->setScale(0.2f, 0.2f, 0.2f);
	m_visSphere->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_visSphere->setPosition(5, 5, 2);
	m_visSphere->setColor(1, 1, 1, 1.0f);
	m_visSphere->setEntityType(EntityType::ExcludeType);
	m_vState = Inactive;
}

void VisabilityAbility::Update(double deltaTime)
{
	if (isLocal)
		_localUpdate(deltaTime);
	else
		_remoteUpdate(deltaTime);
}

void VisabilityAbility::UpdateFromNetwork(Network::ENTITYABILITYPACKET * data)
{
	switch ((VisibilityState)data->state)
	{
	case Active:
		m_visSphere->setPosition(data->start.x, data->start.y, data->start.z);
		m_visSphere->setColor(data->velocity.x, data->velocity.y, data->velocity.z, data->velocity.w);
		m_vState = RemoteActive;
		recentPacket = true;
		break;
	case Inactive:
		m_vState = Inactive;
	}
}

void VisabilityAbility::Use()
{
}

void VisabilityAbility::Draw()
{
	if (m_vState == Active || m_vState == RemoteActive)
		m_visSphere->Draw();
}

unsigned int VisabilityAbility::getState()
{
	return (unsigned int)m_vState;
}

DirectX::XMFLOAT4A VisabilityAbility::getStart()
{
	return m_lastStart;
}

DirectX::XMFLOAT4A VisabilityAbility::getLastColor()
{
	return m_lastColor;
}

void VisabilityAbility::_localUpdate(double dt)
{
	switch (m_vState)
	{
	case Inactive:
		_inStateInactive();
		break;
	case Active:
		_inStateActive();
		break;
	}
}

void VisabilityAbility::_remoteUpdate(double dt)
{
	if (m_vState == RemoteActive)
		_inStateRemoteActive(dt);
}

void VisabilityAbility::_inStateInactive()
{
	if (Input::OnAbilityPressed())
	{
		if (((Player*)p_owner)->CheckManaCost(getManaCost()))
		{
			((Player*)p_owner)->DrainMana(getManaCost());
			m_vState = Active;
		}
	}
	else
		m_vState = Inactive;
}

void VisabilityAbility::_inStateActive()
{
	if (Input::OnAbilityReleased())
		m_vState = Inactive;
	else
	{
		Player * player = ((Player*)p_owner);
		DirectX::XMFLOAT4A po = player->getPosition();
		po.y += 1;
		DirectX::XMVECTOR ve = DirectX::XMLoadFloat4A(&po);
		DirectX::XMVECTOR cm = DirectX::XMLoadFloat4A(&player->getCamera()->getDirection());
		DirectX::XMStoreFloat4A(&po, DirectX::XMVectorAdd(ve, cm));

		m_lastStart = po;

		m_visSphere->setPosition(po);

		DirectX::XMFLOAT4A color;

		float x = /*color.x **/ 3.0f * player->getVisability();
		x = std::clamp(x, (float)1, (float)6);
		float y = /*color.y **/ 3.0f * player->getVisability();
		y = std::clamp(y, (float)1, (float)6);
		float z = /*color.z **/ 3.0f * player->getVisability();
		z = std::clamp(z, (float)1, (float)6);

		//color.x = color.y = color.z = 2.0f * player->getVisability();
		color.x = x;
		color.y = y;
		color.z = z;

		m_lastColor = color;

		#if _DEBUG
			ImGui::Begin("vis");
			ImGui::Text("visa, %f", player->getVisability());
			ImGui::End();
		#endif

		m_visSphere->setColor(color.x, color.y, color.z, 1.0f);
	}
}

void VisabilityAbility::_inStateRemoteActive(double dt)
{
	static double const TimeOut = 1.0 / 2.0; //500 ms timeout
	static double accumulatedTime = 0.0;
	if (recentPacket)
	{
		accumulatedTime = delay;
		recentPacket = false;
	}
	accumulatedTime += dt;
	if (accumulatedTime >= TimeOut)
	{
		accumulatedTime = 0.0;
		m_vState = Inactive;
	}
}
