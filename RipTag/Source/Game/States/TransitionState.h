#pragma once
#include "State.h"

class RenderingManager;
class Quad;

enum Transition
{
	Lose = 0,
	Win = 1,
	ThankYou = 2
};

class TransitionState :
	public State 
{
private:
	Transition m_type;

	Quad * m_header			= nullptr;
	Quad * m_victory		= nullptr;
	Quad * m_eventInfo		= nullptr;
	Quad * m_backToMenu		= nullptr;
	Quad * m_ready			= nullptr;

	Quad * m_background		= nullptr;

	int m_currentRoom = 0; 

	std::string m_eventString = "";
	void * pCoopData = nullptr;

	bool isServer = false;
	bool isClient = false;

	bool isReady = false;
	bool isRemoteReady = false;

	bool backToMenu = false;

	bool m_pressed = false;
	enum ButtonOrder
	{
		Quit = 0,
		Ready = 1,
	};
	unsigned short m_maxButtons = 2;
	unsigned short m_currentButton = 0;
	double m_inputTimer = 0.0;
	const double m_timerLimit = 0.35;

	bool m_partnerLost;
public:
	TransitionState(RenderingManager * rm, Transition type, std::string eventString, void * pCoopData, int currentRoom, bool partnerLost = false);
	~TransitionState();

	void Update(double deltaTime); 

	void Draw() override;

	// Inherited via State
	virtual void Load() override;
	virtual void unLoad() override;

	//Network
	void HandlePacket(unsigned char id, unsigned char * data);

	bool ReadyToLoadNextRoom();
	bool BackToMenuBool();

private:
	void _initButtons();
	void _loadTextures();

	//Network
	void _registerThisInstanceToNetwork();

	void _onDisconnectPacket();
	void _onReadyPacket();

	void _sendDisconnectPacket();
	void _sendReadyPacket();

private:
	void _handleMouseInput();
	void _handleKeyboardInput();
	void _handleGamepadInput();
	void _buttonStateCheck();
};

