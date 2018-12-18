#include "RipTagPCH.h"
#include "Bars.h"

Bars::Bars() : Triggerable()
{

}
Bars::Bars(int uniqueID, int linkedID, bool isTrigger) : Triggerable(uniqueID, linkedID, isTrigger, "activate", "activate")
{
}

void Bars::Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float bboxScaleX, float bboxScaleY, float bboxScaleZ, float scaleX, float scaleY, float scaleZ)
{
	m_sd.emitter = AudioEngine::Other;
	m_sd.owner = this;
	m_sd.loudness = 1.5f;

	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, bboxScaleX * scaleX, bboxScaleY * scaleY, bboxScaleZ * scaleZ, false);
	BaseActor::setPosition(xPos, yPos, zPos);
	BaseActor::setRotation(pitch, yaw, roll, false);
	BaseActor::setPhysicsRotation(pitch, yaw, roll);


	m_closePos = { xPos, yPos, zPos , 1.0f };
	m_openPos = { xPos, yPos + 8.0f, zPos , 1.0f };

	BaseActor::setScale(scaleX, scaleY, scaleZ);
	BaseActor::setObjectTag("BLINK_WALL");
	BaseActor::getBody()->SetUserData(this);
	BaseActor::setTexture(Manager::g_textureManager.getTexture("BARS"));
	BaseActor::getBody()->SetUserData(this);
	BaseActor::setModel(Manager::g_meshManager.getStaticMesh("BARS"));
	BaseActor::setUserDataBody(this);
	m_closePos = { xPos, yPos, zPos, 1 };
}


void Bars::Update(double deltaTime)
{
	BaseActor::Update(deltaTime);
	FMOD_VECTOR at = { getPosition().x, getPosition().y, getPosition().z };
	float t = deltaTime * 0.5f;// *1.5f;

	if (m_wasClosed)
		m_timer += t;
	else
		m_timer -= t;

	m_timer = max(m_timer, 0.0f);
	m_timer = min(m_timer, 1.0f);

	DirectX::XMVECTOR v1, v2;
	v1 = DirectX::XMLoadFloat4A(&m_openPos);
	v2 = DirectX::XMLoadFloat4A(&m_closePos);
	if (Triggerable::getState() == true)
	{
		if (m_wasClosed)
		{
			m_wasClosed = false;

			if (m_channel != nullptr)
			{
				bool isPlaying = false;
				auto res = m_channel->isPlaying(&isPlaying);
				if (res == FMOD_OK && isPlaying)
				{
					m_channel->stop();
				}
			}
			m_channel = AudioEngine::PlaySoundEffect(RipSounds::g_gateClosening, &at, &m_sd);
			m_channel->setVolume(0.7f);
		}
		if (m_timer < 0.0001f && !m_playedOpenSound)
		{
			bool isPlaying = false;
			auto res = m_channel->isPlaying(&isPlaying);
			if (res == FMOD_OK && isPlaying)
			{
				m_channel->stop();
			}
			AudioEngine::PlaySoundEffect(RipSounds::g_gateOpend, &at, &m_sd)->setVolume(0.7f);
			m_playedOpenSound = true;
		}
		m_playedClosedSound = false;

		DirectX::XMVECTOR lerp = DirectX::XMVectorLerp(v1, v2, m_timer);
		DirectX::XMFLOAT3 openPos;
		DirectX::XMStoreFloat3(&openPos, lerp);
		setPosition(openPos.x, openPos.y, openPos.z);
		p_setPosition(openPos.x, openPos.y + m_YOffset, openPos.z);
	}
	else
	{
		if (!m_wasClosed)
		{
			m_wasClosed = true;
			if (m_channel != nullptr)
			{
				bool isPlaying = false;
				auto res = m_channel->isPlaying(&isPlaying);
				if (res == FMOD_OK && isPlaying)
				{
					m_channel->stop();
				}
			}

			m_channel = AudioEngine::PlaySoundEffect(RipSounds::g_gateOpening, &at, &m_sd);
			m_channel->setVolume(0.7f);
		}

		if (m_timer > 0.9999f && !m_playedClosedSound)
		{
			bool isPlaying = false;
			auto res = m_channel->isPlaying(&isPlaying);
			if (res == FMOD_OK && isPlaying)
			{
				m_channel->stop();
			}
			m_playedClosedSound = true;
			AudioEngine::PlaySoundEffect(RipSounds::g_gateClosed, &at, &m_sd)->setVolume(0.7f);
		}
		m_playedOpenSound = false;
		DirectX::XMVECTOR lerp = DirectX::XMVectorLerp(v1, v2, m_timer);

		DirectX::XMFLOAT3 closePos;
		DirectX::XMStoreFloat3(&closePos, lerp);

		setPosition(closePos.x, closePos.y, closePos.z);
		p_setPosition(closePos.x, closePos.y + m_YOffset, closePos.z);

	}
}

Bars::~Bars()
{
}
