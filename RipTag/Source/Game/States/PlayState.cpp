#include "PlayState.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"
#include "EngineSource/Helper/Timer.h"
#include "ImportLibrary/formatImporter.h"


PlayState::PlayState(RenderingManager * rm) : State(rm)
{	

	CameraHandler::Instance();
	auto future = std::async(std::launch::async, &PlayState::thread, this, "KOMBIN");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));

	Timer::StartTimer();

	Manager::g_meshManager.loadStaticMesh("SPHERE");
	Manager::g_animationManager.loadSkeleton("../Assets/IDLEDUDEFOLDER/IDLEDUDE_SKELETON.bin", "IDLEDUDE");
	Manager::g_animationManager.loadClipCollection("IDLEDUDE", "IDLEDUDE", "../Assets/IDLEDUDEFOLDER", Manager::g_animationManager.getSkeleton("IDLEDUDE"));
	Manager::g_meshManager.loadDynamicMesh("IDLEDUDE");
	Manager::g_textureManager.loadTextures("SPHERE");

	future.get();
	future1.get();
	player = new Player();
	Timer::StopTimer();
	std::cout << "s " << Timer::GetDurationInSeconds() << std::endl;

	SM::StateMachine<bool, float> machine(5);
	machine.AddState("idle", true);
	machine.AddState("walk", true);
	machine.AddState("jump_start", false);
	machine.AddState("jump_land", false);

	machine.AddOutStates("idle", { {"walk", 0.2f} , {"jump_start", 0.1f} });
	machine.AddOutStates("walk", { {"idle", 0.2f} , {"jump_start", 0.1f} });
	machine.AddOutStates("jump_start", { {"jump_land", 0.2f} });
	machine.AddOutStates("jump_land", { {"idle", 0.2f}, {"walk", 0.2f} });

	if (machine.TryGoTo("walk").has_value())
	{
		std::cout << "po";
	}
	if (machine.TryGoTo("jump_land").has_value())
	{
		std::cout << "popo";
	}

	CameraHandler::setActiveCamera(player->getCamera());


	player->Init(m_world, e_dynamicBody,0.5f,0.5f,0.5f);
	player->setEntityType(EntityType::PlayerType);
	//player->setPosition(0, 5, 0, 0);
	player->setColor(10, 10, 0, 1);

	player->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	player->setScale(1.0f, 1.0f, 1.0f);
	player->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	player->setTextureTileMult(2, 2);

	player->InitTeleport(m_world);
	


	//enemy->setDir(1, 0, 0);
	//enemy->getCamera()->setFarPlane(5);

	//#model creation

	model = new Drawable();
	model->setEntityType(EntityType::PlayerType);
	model->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	model->setScale(0.5, 0.5, 0.5);
	model->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	model->setTextureTileMult(50, 50);


	
	m_levelHandler.setPlayer(player);
	m_levelHandler.Init(m_world);
	
	Input::ResetMouse();
}

PlayState::~PlayState()
{
	m_levelHandler.Release();
	
	player->Release(m_world);
	player->ReleaseTeleport(m_world);
	delete player;

	//actor->Release(m_world);
	delete model;
}

void PlayState::Update(double deltaTime)
{
	if (InputHandler::getShowCursor() != FALSE)
		InputHandler::setShowCursor(FALSE);	   

#if _DEBUG
	TemporaryLobby();
#endif
	if (GamePadHandler::IsLeftDpadPressed())
	{
		Input::ForceDeactivateGamepad();
	}
	if (GamePadHandler::IsRightDpadPressed())
	{
		Input::ForceActivateGamepad();
	}

	player->Update(deltaTime);

	m_objectHandler.Update();
	m_levelHandler.Update(deltaTime);
	
	m_step.dt = deltaTime;
	m_step.velocityIterations = 1;
	m_step.sleeping = false;
	m_firstRun = false;

	m_world.Step(m_step);
	player->PhysicsUpdate(deltaTime);

	if (Input::Exit())
	{
		setKillState(true);
	}

	// Must be last in update
	if (!player->unlockMouse)
	{
		Input::ResetMouse();
		InputHandler::setShowCursor(FALSE);
	}
	else
	{
		InputHandler::setShowCursor(TRUE);
	}
}

void PlayState::Draw()
{
	m_objectHandler.Draw();
	m_levelHandler.Draw();
	
	player->Draw();
	model->Draw();

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