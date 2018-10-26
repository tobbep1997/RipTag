#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include "Actor.h"
#include "Multiplayer.h"
#include <stack>

#include "../Abilities/TeleportAbility.h"
#include "../Abilities/VisabilityAbility.h"
#include "../Abilities/Disable/DisableAbility.h"


class RemotePlayer : public Actor, public RakNet::NetworkIDObject
{
public:
	RemotePlayer(RakNet::NetworkID nID, DirectX::XMFLOAT4A pos, DirectX::XMFLOAT4A scale, DirectX::XMFLOAT4A rot);
	~RemotePlayer();

private:
	//PRIVATE MEMBERS
	std::stack<PlayerState> m_stateStack;
	bool handleTransition = false;
	AbilityComponent ** m_abilityComponents;
	const unsigned short int m_nrOfAbilitys = 4;
public:
	//PUBLIC MEMBER FUNCTIONS
	void BeginPlay();
	void HandlePacket(unsigned char id, unsigned char * data);
	void Update(double dt);
	void Draw() override;

private:
	//PRIVATE FUNCTIONS

	//Network message handling
	void _onNetworkUpdate(Network::ENTITYUPDATEPACKET * data);

	//Local Game logic
	void _Idle(float dt);
	void _Walking(float dt);
	void _Crouching(float dt);
	void _Sprinting(float dt);
	void _Jumping(float dt);
	void _Falling(float dt);


};

