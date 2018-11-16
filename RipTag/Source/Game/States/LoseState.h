#pragma once
#include "State.h"

class RenderingManager;
class Quad;
class LoseState :
	public State
{
private:
	Quad * m_gameOver;
	Quad * m_eventInfo;
	Quad * m_backToMenu;
	Quad * m_Retry;
	Quad * m_backGround;

	std::string m_eventString = "";
	void * pCoopData = nullptr;

	bool isServer = false;
	bool isClient = false;

	bool isReady = false;
	bool isRemoteReady = false;
public:
	LoseState(RenderingManager * rm, std::string eventString = "", void * pCoopData = nullptr);
	~LoseState();

	void Update(double deltaTime);

	void Draw() override;

	// Inherited via State
	virtual void Load() override;
	virtual void unLoad() override;

	//Network
	void HandlePacket(unsigned char id, unsigned char * data);

private:
	void _initButtons();

	//Network
	void _registerThisInstanceToNetwork();

	void _onDisconnectPacket();
	void _onReadyPacket();
	void _onStartGamePacket();

	void _sendDisconnectPacket();
	void _sendReadyPacket();
	void _sendStartGamePacket();
};

