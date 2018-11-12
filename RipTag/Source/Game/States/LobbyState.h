#pragma once
#include "State.h"
#include <Multiplayer.h>

class LobbyState :
	public State
{
private:
	const double FLUSH_FREQUENCY = 1.0; //unit is in seconds

	enum ButtonOrderLobby
	{
		Host = 0,
		Join = 1,
		Refresh = 2,
		Return = 3,
	};
	enum CharacterSelection
	{
		CharOne = 0,
		CharTwo = 1,
		Ready = 2,
		Back = 3
	};

	Quad* m_infoWindow = nullptr;
	std::vector<Quad*> m_lobbyButtons;
	std::vector<Quad*> m_charSelectButtons;
	std::vector<Quad*> m_hostListButtons;
	unsigned int m_currentButton;

	bool inServerList = false;

	bool isHosting = false;
	bool hasClient = false;
	bool hasJoined = false;
	bool hasCharSelected = false;
	unsigned int selectedChar = 0;

	//Network
	Network::Multiplayer * pNetwork;
	RakNet::SystemAddress m_clientIP = RakNet::SystemAddress("0.0.0.0");
	RakNet::SystemAddress m_MySysAdress = RakNet::SystemAddress("0.0.0.0");

	std::string m_MyHostName;
	std::string m_ServerName = "";
	//This packet is created when we create a Server and host it
	Network::LOBBYEVENTPACKET m_adPacket;

	bool isReady = false;
	bool isRemoteReady = false;

	RakNet::SystemAddress selectedHost = RakNet::SystemAddress("0.0.0.0");
	std::string selectedHostInfo = "";

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
	void _flushServerList();
	void _updateInfoString();

	//Seperate input handling for cleaner code
	void _gamePadMainLobby();
	void _gamePadCharSelection();
	void _gamePadServerList();
	void _keyboardMainLobby();
	void _keyboardCharSelection();
	void _keyboardServerList();
	void _mouseMainLobby();
	void _mouseCharSelection();
	void _mouseServerList();

	//Network
	void _registerThisInstanceToNetwork();
	void _onAdvertisePacket(RakNet::Packet * data);
	void _onClientJoinPacket(RakNet::Packet * data);
	void _onFailedPacket(RakNet::Packet * data);
	void _onSucceedPacket(RakNet::Packet * data);
	void _onDisconnectPacket(RakNet::Packet * data);
	void _onServerDenied(RakNet::Packet * data);


	void _newHostEntry(std::string& hostName);

};

