#pragma once
#include "Actor.h"
#include "Multiplayer.h"
#include <stack>

enum Ability;
class AbilityComponent;

class RemotePlayer : public Actor, public RakNet::NetworkIDObject
{
private:
	enum AnimState {
		IDLE = 0,
		FORWARD = 1,
		BACKWARD = 2,
		LEFT = 3,
		RIGHT = 4,
		BACK_LEFT = 5,
		BACK_RIGHT = 6
	};
public:
	RemotePlayer(RakNet::NetworkID nID, DirectX::XMFLOAT4A pos, DirectX::XMFLOAT4A scale, DirectX::XMFLOAT4A rot);
	~RemotePlayer();

private:
	//PRIVATE MEMBERS
	std::stack<PlayerState> m_stateStack;
	bool handleTransition = false;
	AbilityComponent ** m_abilityComponents;
	Ability m_currentAbility;// = Ability::TELEPORT;
	const unsigned short int m_nrOfAbilitys = 4;
	DirectX::XMFLOAT4A m_mostRecentPosition;
	RakNet::Time m_timeDiff;

	//ANIMATION
	float m_currentDirection = 0.0f;
	float m_currentSpeed = 0.0f;

public:
	//PUBLIC MEMBER FUNCTIONS
	void BeginPlay();
	void HandlePacket(unsigned char id, unsigned char * data);
	void Update(double dt);
	void Draw() override;
	bool hasWon = false;
private:
	//PRIVATE FUNCTIONS

	//Network message handling
	void _onNetworkUpdate(Network::ENTITYUPDATEPACKET * data);
	void _onNetworkAbility(Network::ENTITYABILITYPACKET * data);
	void _onNetworkAnimation(Network::ENTITYANIMATIONPACKET * data);

	void _lerpPosition(float dt);

	void _registerAnimationStateMachine();
};

