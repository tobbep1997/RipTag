#include "PlayState.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"
#include "EngineSource/Helper/Timer.h"
#include "ImportLibrary/formatImporter.h"
#include "../RipTagExtern/RipExtern.h"
#include "../Handlers/AnimationHandler.h"

b3World * RipExtern::g_world = nullptr;
ContactListener * RipExtern::m_contactListener;
RayCastListener * RipExtern::m_rayListener;
#define JAAH TRUE
#define NEIN FALSE



PlayState::PlayState(RenderingManager * rm) : State(rm)
{	

	RipExtern::g_world = &m_world;
	m_contactListener = new ContactListener();
	RipExtern::m_contactListener = m_contactListener;
	RipExtern::g_world->SetContactListener(m_contactListener);
	m_rayListener = new RayCastListener();
	RipExtern::m_rayListener = m_rayListener;
	CameraHandler::Instance();
	//auto future = std::async(std::launch::async, &PlayState::thread, this, "KOMBIN");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));

	Manager::g_meshManager.loadStaticMesh("SPHERE");
	Manager::g_animationManager.loadSkeleton("../Assets/STATEFOLDER/STATE_SKELETON.bin", "STATE");
	Manager::g_animationManager.loadClipCollection("STATE", "STATE", "../Assets/STATEFOLDER", Manager::g_animationManager.getSkeleton("STATE"));
	Manager::g_meshManager.loadDynamicMesh("STATE");
	Manager::g_textureManager.loadTextures("SPHERE");

//	future.get();
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
	model->setEntityType(EntityType::GuarddType);
	model->setModel(Manager::g_meshManager.getDynamicMesh("STATE"));
	model->setScale(0.03, 0.03, 0.03);
	model->setPosition({ 0.0, -4.9, 0.0, 1.0 });
	model->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	model->setTextureTileMult(50, 50);
	auto idle_clip = Manager::g_animationManager.getAnimation("STATE", "IDLE_ANIMATION");
	auto fwd_clip = Manager::g_animationManager.getAnimation("STATE", "WALK_FORWARD_ANIMATION");
	auto bwd_clip = Manager::g_animationManager.getAnimation("STATE", "WALK_BACKWARD_ANIMATION");
	auto lft_clip = Manager::g_animationManager.getAnimation("STATE", "WALK_LEFT2_ANIMATION");
	auto rgt_clip = Manager::g_animationManager.getAnimation("STATE", "WALK_RIGHT2_ANIMATION");
	auto jmp_clip = Manager::g_animationManager.getAnimation("STATE", "JUMP_ANIMATION");
	model->getAnimatedModel()->SetPlayingClip(idle_clip.get());
	model->getAnimatedModel()->Play();
	model->getAnimatedModel()->SetSkeleton(Manager::g_animationManager.getSkeleton("STATE"));
	auto& stateMachine = model->getAnimatedModel()->InitStateMachine(2);
	{
		auto blendFwd = stateMachine->AddBlendSpace2DState("loco_fwd", &m_playerManager->getLocalPlayer()->m_currentDirection, &m_playerManager->getLocalPlayer()->m_currentSpeed, -90.0f, 90.f, 0.0f, 3.001f);
		auto blendBwd = stateMachine->AddBlendSpace2DState("loco_bwd", &m_playerManager->getLocalPlayer()->m_currentDirection, &m_playerManager->getLocalPlayer()->m_currentSpeed, -180.0f, 180.0f, 0.0f, 3.001f);

		auto& fwdToBwdL = blendFwd->AddOutState(blendBwd);
		fwdToBwdL.AddTransition(&m_playerManager->getLocalPlayer()->m_currentDirection, -89.9999999f, 89.9999999f, SM::COMPARISON_OUTSIDE_RANGE);

		auto& bwdToFwdL = blendBwd->AddOutState(blendFwd);
		bwdToFwdL.AddTransition(&m_playerManager->getLocalPlayer()->m_currentDirection, -90.f, 90.f, SM::COMPARISON_INSIDE_RANGE);

		//auto blendState = stateMachine->AddBlendSpace2DState("idle_states", &hDir, &hSpeed, -180.0, 180.0, 0.0, 3.1);

		blendFwd->AddRow(0.0, { { idle_clip.get(), -90.f }, { idle_clip.get(), 0.0f }, { idle_clip.get(), 90.0f } });
		blendFwd->AddRow(3.1, { {lft_clip.get(), -90.0f }, {fwd_clip.get(), 0.0f }, {rgt_clip.get(), 90.0f } });

		blendBwd->AddRow(0.0, { { idle_clip.get(), -180.0f }, { idle_clip.get(), -90.0f }, { idle_clip.get(), 0.0f }, { idle_clip.get(), 90.0f }, { idle_clip.get(), 180.0f } });
		blendBwd->AddRow(3.1, { {bwd_clip.get(), -180.0f }, {lft_clip.get(), -90.0f }, {fwd_clip.get(), 0.0f }, {rgt_clip.get(), 90.0f }, {bwd_clip.get(), 180.0f } });
		stateMachine->SetState("loco_fwd");
	}
	
	m_levelHandler.setPlayer(m_playerManager->getLocalPlayer());
	m_levelHandler.Init(m_world);

	triggerHandler = new TriggerHandler();

	pressureplate = new PressurePlate();
	pressureplate->Init();
	pressureplate->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	pressureplate->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	pressureplate->setPosition(0, -3, 0);

	lever = new Lever();
	lever->Init();
	lever->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	lever->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	lever->setPosition(-3, -3, 0);

	door = new Door();
	door->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	door->setTexture(Manager::g_textureManager.getTexture("SPHERE"));

	std::vector<Trigger*> t1;
	std::vector<Triggerble*> t2;

	t1.push_back(pressureplate);
	t1.push_back(lever);
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
	
	m_playerManager->getLocalPlayer()->Release(m_world);
	
	delete m_playerManager;

	//actor->Release(m_world);
	delete m_contactListener;
	delete model;
	delete triggerHandler;
	pressureplate->Release(*RipExtern::g_world);

	delete pressureplate;
	lever->Release(*RipExtern::g_world);
	delete lever;
	delete door;
	delete m_rayListener;
}

void PlayState::Update(double deltaTime)
{
	pressureplate->Update(deltaTime);
	lever->Update(deltaTime);
	triggerHandler->Update(deltaTime);
	m_levelHandler.Update(deltaTime);
	m_contactListener->ClearContactQueue();
	m_rayListener->ClearQueue();
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

	//player->SetCurrentVisability((e2Vis[0] / 5000.0f) + (e1Visp[0] / 5000));
	m_playerManager->Update(deltaTime);
	m_levelHandler.Update(deltaTime);
	model->getAnimatedModel()->Update(deltaTime);
	
	m_step.dt = deltaTime;
	m_step.velocityIterations = 1;
	m_step.sleeping = false;
	m_firstRun = false;
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
		
	door->Draw();

	model->Draw();
	pressureplate->DrawWireFrame();
	lever->DrawWireFrame();
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