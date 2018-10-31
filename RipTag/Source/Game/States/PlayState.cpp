#include "PlayState.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"
#include "EngineSource/Helper/Timer.h"
#include "ImportLibrary/formatImporter.h"
#include "../RipTagExtern/RipExtern.h"

b3World * RipExtern::g_world = nullptr;
ContactListener * RipExtern::m_contactListener;

#define JAAH TRUE
#define NEIN FALSE

PlayState::PlayState(RenderingManager * rm) : State(rm)
{	

	RipExtern::g_world = &m_world;
	m_contactListener = new ContactListener();
	RipExtern::m_contactListener = m_contactListener;

	RipExtern::g_world->SetContactListener(m_contactListener);

	CameraHandler::Instance();
	auto future = std::async(std::launch::async, &PlayState::thread, this, "KOMBIN");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));

	Manager::g_meshManager.loadStaticMesh("SPHERE");
	Manager::g_textureManager.loadTextures("SPHERE");

	future.get();
	future1.get();
	
	m_playerManager = new PlayerManager(&this->m_world);
	m_playerManager->RegisterThisInstanceToNetwork();
	m_playerManager->CreateLocalPlayer();

	Timer::StopTimer();
	std::cout << "s " << Timer::GetDurationInSeconds() << std::endl;


	CameraHandler::setActiveCamera(m_playerManager->getLocalPlayer()->getCamera());


	m_playerManager->getLocalPlayer()->Init(m_world, e_dynamicBody,0.5f,0.5f,0.5f);
	m_playerManager->getLocalPlayer()->setEntityType(EntityType::PlayerType);
	//player->setPosition(0, 5, 0, 0);
	m_playerManager->getLocalPlayer()->setColor(10, 10, 0, 1);

	m_playerManager->getLocalPlayer()->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	m_playerManager->getLocalPlayer()->setScale(1.0f, 1.0f, 1.0f);
	m_playerManager->getLocalPlayer()->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_playerManager->getLocalPlayer()->setTextureTileMult(2, 2);

	


	//enemy->setDir(1, 0, 0);
	//enemy->getCamera()->setFarPlane(5);

	model = new Drawable();
	model->setEntityType(EntityType::PlayerType);
	model->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	model->setScale(0.5, 0.5, 0.5);
	model->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	model->setTextureTileMult(50, 50);

	
	m_levelHandler.setPlayer(m_playerManager->getLocalPlayer());
	m_levelHandler.Init(m_world);
	
	Input::ResetMouse();

	m_step.velocityIterations = 1;
	m_step.sleeping = false;
	m_firstRun = false;
}

PlayState::~PlayState()
{
	m_levelHandler.Release();
	
	m_playerManager->getLocalPlayer()->Release(m_world);
	
	delete m_playerManager;

	//actor->Release(m_world);
	delete m_contactListener;
	delete model;
}

void PlayState::Update(double deltaTime)
{
	if (InputHandler::getShowCursor() != FALSE)
		InputHandler::setShowCursor(FALSE);	   

#if _DEBUG
	TemporaryLobby();
#endif
	if (GamePadHandler::IsSelectPressed())
	{
		Input::SetActivateGamepad(Input::isUsingGamepad());
	}

	//player->SetCurrentVisability((e2Vis[0] / 5000.0f) + (e1Visp[0] / 5000));
	m_playerManager->Update(deltaTime);
	m_levelHandler.Update(deltaTime);
	
	
	m_step.dt = deltaTime;
	m_contactListener->ClearContactQueue();
	m_world.Step(m_step);
	m_playerManager->PhysicsUpdate();

	if (Input::Exit() || GamePadHandler::IsStartPressed())
	{
		setKillState(true);
	}


	// Must be last in update
	if (!m_playerManager->getLocalPlayer()->unlockMouse)
	{
		Input::ResetMouse();
		InputHandler::setShowCursor(false);
	}
	else
	{
		InputHandler::setShowCursor(true);
	}
}

void PlayState::Draw()
{
	m_levelHandler.Draw();
	
	m_playerManager->Draw();
		
	p_renderingManager->Flush(*CameraHandler::getActiveCamera());
}

void PlayState::thread(std::string s)
{
	Manager::g_meshManager.loadStaticMesh(s);
}

void PlayState::TemporaryLobby()
{
	Network::Multiplayer * ptr = Network::Multiplayer::GetInstance();

	ImGui::Begin("Network Lobby");
	if (!ptr->isConnected() && !ptr->isRunning())
	{
		if (ImGui::Button("Start Server"))
		{
			ptr->StartUpServer();
		}
		else if (ImGui::Button("Start Client"))
		{
			ptr->StartUpClient();
		}
	}

	if (ptr->isRunning() && ptr->isServer() && !ptr->isConnected())
	{
		ImGui::Text("Server running... Awaiting Connections...");
		if (ImGui::Button("Cancel"))
			ptr->EndConnectionAttempt();
	}

	if (ptr->isRunning() && ptr->isClient() && !ptr->isConnected())
	{
		ImGui::Text("Client running... Searching for Host...");
		if (ImGui::Button("Cancel"))
			ptr->EndConnectionAttempt();
	}

	if (ptr->isRunning() && ptr->isConnected() && !ptr->isGameRunning())
	{
		if (ptr->isServer() && !ptr->isGameRunning())
			if (ImGui::Button("Start Game"))
			{
				ptr->setIsGameRunning(true);
				Network::EVENTPACKET packet(Network::NETWORKMESSAGES::ID_GAME_START);
				Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::IMMEDIATE_PRIORITY);
			}
		ImGui::Text(ptr->GetNetworkInfo().c_str());
		if (ImGui::Button("Disconnect"))
			ptr->Disconnect();
	}

	if (ptr->isRunning() && ptr->isConnected() && ptr->isGameRunning())
	{
		if (ImGui::Button("Spawn on Remote"))
			m_playerManager->SendOnPlayerCreate();
	}

	ImGui::End();
}
