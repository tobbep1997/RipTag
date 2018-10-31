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
	//auto future = std::async(std::launch::async, &PlayState::thread, this, "KOMBIN");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));

	Manager::g_meshManager.loadStaticMesh("SPHERE");
	Manager::g_textureManager.loadTextures("SPHERE");

//	future.get();
	future1.get();


	player = new Player();

	CameraHandler::setActiveCamera(player->getCamera());

	player->Init(m_world, e_dynamicBody, 0.5f,0.5f,0.5f);
	

	model = new Drawable();
	model->setEntityType(EntityType::PlayerType);
	model->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	model->setScale(0.5, 0.5, 0.5);
	model->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	model->setTextureTileMult(50, 50);

	
	m_levelHandler.setPlayer(player);
	m_levelHandler.Init(m_world);

	triggerHandler = new TriggerHandler();

	pressureplate = new PressurePlate();
	pressureplate->Init();
	pressureplate->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	pressureplate->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	pressureplate->setPosition(0, -3, 0);

	door = new Door();
	door->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	door->setTexture(Manager::g_textureManager.getTexture("SPHERE"));

	std::vector<Trigger*> t1;
	std::vector<Triggerble*> t2;

	t1.push_back(pressureplate);
	t2.push_back(door);

	triggerHandler->AddPair(t1, t2);

	Input::ResetMouse();

	m_step.velocityIterations = 1;
	m_step.sleeping = false;
	m_firstRun = false;
}

PlayState::~PlayState()
{
	m_levelHandler.Release();
	
	
	player->Release(m_world);
	
	delete player;
	delete model;
	delete triggerHandler;
	pressureplate->Release(*RipExtern::g_world);

	delete pressureplate;
	delete door;
	delete m_contactListener;
}

void PlayState::Update(double deltaTime)
{
	pressureplate->Update(deltaTime);

	triggerHandler->Update(deltaTime);
	m_levelHandler.Update(deltaTime);
	m_contactListener->ClearContactQueue();
	m_world.Step(m_step);

	if (InputHandler::getShowCursor() != FALSE)
		InputHandler::setShowCursor(FALSE);	   

#if _DEBUG
	TemporaryLobby();
#endif
	if (GamePadHandler::IsSelectPressed())
	{
		Input::SetActivateGamepad(Input::isUsingGamepad());
	}

	player->Update(deltaTime);
	m_levelHandler.Update(deltaTime);
	
	
	m_step.dt = deltaTime;
	m_step.velocityIterations = 1;
	m_step.sleeping = false;
	m_firstRun = false;

	player->Update(deltaTime);
	//m_objectHandler.Update();

	
	player->PhysicsUpdate(deltaTime);

	if (Input::Exit() || GamePadHandler::IsStartPressed())
	{
		setKillState(true);
	}


	if (!player->unlockMouse)
	{
		Input::ResetMouse();
		InputHandler::setShowCursor(NEIN);
	}
	else
	{
		InputHandler::setShowCursor(JAAH);
	}


}

void PlayState::Draw()
{
	m_levelHandler.Draw();
	
	door->Draw();

	player->Draw();
	model->Draw();
	pressureplate->DrawWireFrame();
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

	if (ptr->isRunning() && ptr->isConnected())
	{
		if (ptr->isServer() && !ptr->isGameRunning())
			if (ImGui::Button("Start Game"))
			{				//set game running, send a start game message
			}
		ImGui::Text(ptr->GetNetworkInfo().c_str());
		if (ImGui::Button("Disconnect"))
			ptr->Disconnect();
	}
	ImGui::End();
}
