#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include "Actor.h"
#include "Multiplayer.h"
#include <stack>


class RemotePlayer : public Actor, public PhysicsComponent, public RakNet::NetworkIDObject
{
public:
	RemotePlayer(b3World &world, RakNet::NetworkID nID, float x, float y, float z);
	~RemotePlayer();

private:
	//PRIVATE MEMBERS
	std::stack<PlayerState> m_stateStack;

public:
	//PUBLIC MEMBER FUNCTIONS
	void BeginPlay();
	void HandlePacket(unsigned char id, unsigned char * data);
	void Update(double dt);
	void PhysicsUpdate();
	void Draw() override;

private:
	//PRIVATE FUNCTIONS

	//Network message handling
	void _onMovement(Network::ENTITY_MOVE * data);
	void _onJumpEvent(Network::ENTITY_EVENT * data);

	//Local Game logic
	void _Idle(float dt);
	void _Walking(float dt);
	void _Crouching(float dt);
	void _Sprinting(float dt);
	void _Jumping(float dt);
	void _Falling(float dt);


};

