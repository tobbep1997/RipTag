#pragma once
#include "State.h"
#include <Multiplayer.h>

class LobbyState :
	public State
{
private:
	enum ButtonOrderLobby
	{
		Host = 0,
		Join = 1,
		Return = 2,
	};
	enum CharacterSelection
	{
		CharOne = 0,
		CharTwo = 1,
		Ready = 2,
		Back = 3
	};
	std::vector<Quad*> m_lobbyButtons;
	std::vector<Quad*> m_charSelectButtons;
	std::vector<Quad*> m_hostList;
	unsigned int m_currentButton;

	bool isHosting = false;
	bool hasJoined = false;
	bool hasCharSelected = false;
	unsigned int selectedChar = 0;

	std::string hostName;
	bool isReady = false;

	RakNet::SystemAddress selectedHost;

public:
	LobbyState(RenderingManager * rm);
	~LobbyState();
	void Update(double deltaTime) override;
	void Draw() override;

private:
	void _initButtons();
	void _handleGamePadInput();
	void _handleKeyboardInput();
	void _handleMouseInput();
	void _updateSelectionStates();
};

