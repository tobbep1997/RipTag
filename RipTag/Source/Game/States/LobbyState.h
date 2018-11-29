#pragma once
#include "State.h"
#include <Multiplayer.h>

class LobbyState :
	public State
{
private:
	const double FLUSH_FREQUENCY = 1.0; //unit is in seconds
	const DirectX::XMFLOAT4A DefaultColor = { 1.0f, 1.0f, 1.0f, 1.0f }; //Sharp/Full White
	const DirectX::XMFLOAT4A ActivatedColor = { 0.0f, 0.6f, 0.1f, 1.0f }; //Warmer/softer green
	const DirectX::XMFLOAT4A InactivatedColor = { 0.7f, 0.2f, 0.0f, 1.0f }; //Warmer/softer red
	
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
		Back = 3, 
		SkipTutorial = 4,
	};
	enum Role
	{
		Server = 0,
		Client = 1
	};

	Quad* m_infoWindow			 = nullptr;
	Quad* m_background			 = nullptr;
	Quad* m_charSelectionBG		 = nullptr;
	Quad* m_charSelectInfo		 = nullptr;
	Quad* m_charOneInfo			 = nullptr;
	Quad* m_charTwoInfo			 = nullptr;
	Quad* m_skipTutorialBox		 = nullptr;

	LoadingScreen m_loadingScreen;

	std::vector<Quad*> m_lobbyButtons;
	std::vector<Quad*> m_charSelectButtons;
	std::vector<Quad*> m_hostListButtons;

	unsigned int m_currentButton;
	unsigned int m_currentButtonServerList;

	bool inServerList					= false;

	bool skipTutorial					= false;
	bool isHosting						= false;
	bool hasClient						= false;
	bool hasJoined						= false;
	bool hasCharSelected				= false;
	unsigned int selectedChar			= 0;
	bool hasRemoteCharSelected			= false;
	unsigned int remoteSelectedChar		= 0;

	float m_stickTimerY = 0;
	float m_stickTimerX = 0; 

	//Network
	Network::Multiplayer * pNetwork;
	//Setting a SystemAdress to "0.0.0.0" will yield "UNASSIGNED_SYSTEM_ADRESSS" when calling toString() on the object
	RakNet::SystemAddress m_clientIP		= RakNet::SystemAddress("0.0.0.0");
	RakNet::SystemAddress m_MySysAdress		= RakNet::SystemAddress("0.0.0.0");
	RakNet::NetworkID m_remoteNID = 0;

	std::string m_MyHostName;
	std::string m_ServerName				= "None";
	//This packet is created when we create a Server and host it
	Network::LOBBYEVENTPACKET m_adPacket;

	bool isReady			= false;
	bool isRemoteReady		= false;

	RakNet::SystemAddress selectedHost = RakNet::SystemAddress("0.0.0.0");
	std::string selectedHostInfo = "Selected Host:\nNone\n";

	std::map<uint64_t, std::string> m_hostNameMap;
	std::map<std::string, RakNet::SystemAddress> m_hostAdressMap;

	CoopData * pCoopData	= nullptr;
public:
	LobbyState(RenderingManager * rm);
	~LobbyState();
	void Update(double deltaTime) override;
	void Draw() override;

	//Network
	void HandlePacket(unsigned char id, RakNet::Packet * packet);

private:
	void _initButtons();
	void _handleGamePadInput(float deltaTime);
	void _handleKeyboardInput();
	void _handleMouseInput();
	void _updateSelectionStates();
	void _resetLobbyButtonStates();
	void _resetCharSelectButtonStates();
	void _flushServerList();
	void _updateInfoString();

	//Seperate input handling for cleaner code
	void _gamePadMainLobby(float deltaTime);
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
	void _onCharacterSelectionPacket(RakNet::Packet * data);
	void _onReadyPacket(RakNet::Packet * data);
	void _onGameStartedPacket(RakNet::Packet * data);
	void _onRequestPacket(unsigned char id, RakNet::Packet * data);
	void _onReplyPacket(RakNet::Packet * data);
	void _onHostNamePacket(RakNet::Packet * data);

	void _sendCharacterSelectionPacket();
	void _sendReadyPacket();
	void _sendGameStartedPacket();
	void _sendMyHostNamePacket();

	void _newHostEntry(std::string& hostName);


	// Inherited via State
	virtual void Load() override;
	virtual void unLoad() override;

};

