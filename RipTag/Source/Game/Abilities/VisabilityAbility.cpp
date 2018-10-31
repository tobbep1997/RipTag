#include "VisabilityAbility.h"
#include "../Actors/Player.h"


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
}

void VisabilityAbility::Update(double deltaTime)
{
	if (!m_isUsing)
		return;

	Player * player = ((Player*)p_owner);
	DirectX::XMFLOAT4A po = player->getPosition();
	po.y += 1;
	DirectX::XMVECTOR ve = DirectX::XMLoadFloat4A(&po);
	DirectX::XMVECTOR cm = DirectX::XMLoadFloat4A(&player->getCamera()->getDirection());
	DirectX::XMStoreFloat4A(&po, DirectX::XMVectorAdd(ve, cm));

	m_visSphere->setPosition(po);
	m_visSphere->setColor(2.0f * player->getVisability(),
		2.0f * player->getVisability(),
		2.0f * player->getVisability(),
		1);
}

void VisabilityAbility::UpdateFromNetwork(Network::ENTITYABILITYPACKET * data)
{
}

void VisabilityAbility::Use()
{
	if (((Player*)p_owner)->CheckManaCost(getManaCost()))
	{
		m_isUsing = true;
		((Player*)p_owner)->DrainMana(getManaCost());

	}
	else
		m_isUsing = false;
}

void VisabilityAbility::Draw()
{
	if (m_isUsing)
		m_visSphere->Draw();
	m_isUsing = false;
}
