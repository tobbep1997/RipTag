#pragma once
#include "Actor.h"
#include "Multiplayer.h"
#include <stack>

enum Ability;
class AbilityComponent;

class RemotePlayer : public Actor, public RakNet::NetworkIDObject
{
public:
	enum AnimState {
		IDLE = 0,
		FORWARD = 1,
		BACKWARD = 2,
		LEFT = 3,
		RIGHT = 4,
		BACK_LEFT = 5,
		BACK_RIGHT = 6,
		RUN_FORWARD = 7,
		RUN_FORWARD_LEFT = 8,
		RUN_FORWARD_RIGHT = 9
	};
public:
	enum CHARACTER
	{
		LEJF,
		BILLY
	};
	RemotePlayer(RakNet::NetworkID nID, DirectX::XMFLOAT4A pos, DirectX::XMFLOAT4A scale, DirectX::XMFLOAT4A rot, CHARACTER character = LEJF);
	~RemotePlayer();

private:
	//PRIVATE MEMBERS
	std::stack<PlayerState> m_stateStack;
	bool handleTransition = false;
	AbilityComponent ** m_abilityComponents1;
	AbilityComponent ** m_abilityComponents2;
	AbilityComponent ** m_activeSet;
	Ability m_currentAbility;// = Ability::TELEPORT;
	const unsigned short int m_nrOfAbilitys = 2;
	DirectX::XMFLOAT4A m_mostRecentPosition;
	RakNet::Time m_timeDiff;

	//ANIMATION
	float m_currentDirection = 0.0f;
	float m_currentSpeed = 0.0f;
	float m_currentPitch = 0.0f;
	float m_currentPeek = 0.0f;

	//Visibility data
	float m_currentVisibility = 0.0f;
	float m_soundPercentage = 0.0f;
public:
	//PUBLIC MEMBER FUNCTIONS
	void BeginPlay();
	void HandlePacket(unsigned char id, unsigned char * data);
	void Update(double dt);
	void Draw() override;
	bool hasWon = false;
	void SetAbilitySet(int);
	void SetModelAndTextures(int);
	void SetVisibility(const float & value) { m_currentVisibility = value; }
	void SetSoundPercentage(const float & value) { m_soundPercentage = value; }
private:
	//PRIVATE FUNCTIONS

	//Network message handling
	void _onNetworkUpdate(Network::ENTITYUPDATEPACKET * data);
	void _onNetworkAbility(Network::ENTITYABILITYPACKET * data);
	void _onNetworkAnimation(Network::ENTITYANIMATIONPACKET * data);
	void _onNetworkRemoteThrow(unsigned char id);
	void _onNetworkRemotePosess(unsigned char id);
	void _onNetworkRemoteCrouch(unsigned char id);
	void _onNetworkSmokeDetonate(unsigned char * data);
	void _sendVisibilityPacket();
	void _onNetworkBlink(unsigned char id);
	void _onNetworkUseTeleport(unsigned char id);

	void _lerpPosition(float dt);

	void _registerAnimationStateMachine();
public:
};

