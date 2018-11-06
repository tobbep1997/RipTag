#pragma once
#include "AbilityComponent.h"

class Drawable;
class VisabilityAbility : public AbilityComponent
{
private:
	enum VisibilityState
	{
		Active,
		RemoteActive,
		Inactive
	};
	Drawable * m_visSphere;
	VisibilityState m_vState;

	//Network
	RakNet::Time delay;
	bool recentPacket = false;
	DirectX::XMFLOAT4A m_lastStart;
	DirectX::XMFLOAT4A m_lastColor;
	DirectX::XMFLOAT4A color;
public:
	VisabilityAbility();
	~VisabilityAbility();

	// Inherited via AbilityComponent
	virtual void Init() override;
	virtual void Update(double deltaTime) override;
	void UpdateFromNetwork(Network::ENTITYABILITYPACKET * data) override;
	virtual void Use() override;
	virtual void Draw() override;

	unsigned int getState();
	DirectX::XMFLOAT4A getStart();
	DirectX::XMFLOAT4A getLastColor();
private:
	void _localUpdate(double dt);
	void _remoteUpdate(double dt);

	void _inStateInactive();
	void _inStateActive();
	void _inStateRemoteActive(double dt);

};

