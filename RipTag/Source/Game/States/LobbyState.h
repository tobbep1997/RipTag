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
	std::vector<Quad*> m_hostListButtons;
	unsigned int m_currentButton;

	bool isHosting = false;
	bool hasJoined = false;
	bool hasCharSelected = false;
	unsigned int selectedChar = 0;

	//Network
	Network::Multiplayer * pNetwork;

	std::string m_MyHostName;
	//This packet is created when we create a Server and host it
	Network::LOBBYEVENTPACKET m_adPacket;

	bool isReady = false;
	bool isRemoteReady = false;

	RakNet::SystemAddress selectedHost;

	std::map<uint64_t, std::string> m_hostNameMap;
	std::map<std::string, RakNet::SystemAddress> m_hostAdressMap;


public:
	LobbyState(RenderingManager * rm);
	~LobbyState();
	void Update(double deltaTime) override;
	void Draw() override;

	//Network
	void HandlePacket(unsigned char id, RakNet::Packet * packet);

private:
	void _initButtons();
	void _handleGamePadInput();
	void _handleKeyboardInput();
	void _handleMouseInput();
	void _updateSelectionStates();
	void _resetLobbyButtonStates();
	void _resetCharSelectButtonStates();


	//Network
	void _registerThisInstanceToNetwork();
	void _onAdvertisePacket(RakNet::Packet * data);
	void _onClientJoinPacket(RakNet::Packet * data);
	void _newHostEntry(std::string& hostName);


	// Inherited via State
	virtual void Load() override;

	virtual void unLoad() override;

};

