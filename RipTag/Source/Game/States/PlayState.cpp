#include "PlayState.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"
#include "EngineSource/Helper/Timer.h"
#include "ImportLibrary/formatImporter.h"
#include "../Handlers/AnimationHandler.h"


PlayState::PlayState(RenderingManager * rm) : State(rm)
{	

	CameraHandler::Instance();
	auto future = std::async(std::launch::async, &PlayState::thread, this, "KOMBIN");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));

	Timer::StartTimer();

	Manager::g_meshManager.loadStaticMesh("SPHERE");
	Manager::g_animationManager.loadSkeleton("../Assets/STATEFOLDER/STATE_SKELETON.bin", "STATE");
	Manager::g_animationManager.loadClipCollection("STATE", "STATE", "../Assets/STATEFOLDER", Manager::g_animationManager.getSkeleton("STATE"));
	Manager::g_meshManager.loadDynamicMesh("STATE");
	Manager::g_textureManager.loadTextures("SPHERE");

	future.get();
	future1.get();
	player = new Player();
	Timer::StopTimer();
	std::cout << "s " << Timer::GetDurationInSeconds() << std::endl;


	//state.AddTransition<float>(&f, 1.0f, SM::COMPARISON_GREATER_THAN);
	//state.AddTransition<int>(&i, 3, SM::COMPARISON_EQUAL);
	//bool allSatisfied = state.EvaluateAllTransitions();

	CameraHandler::setActiveCamera(player->getCamera());


	player->Init(m_world, e_dynamicBody,0.5f,0.5f,0.5f);
	player->setEntityType(EntityType::PlayerType);
	//player->setPosition(0, 5, 0, 0);
	player->setColor(10, 10, 0, 1);

	//player->setModel(Manager::g_meshManager.getDynamicMesh("IDLEDUDE"));
	player->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	player->setScale(1.0f, 1.0f, 1.0f);
	player->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	player->setTextureTileMult(2, 2);

	player->InitTeleport(m_world);
	


	//enemy->setDir(1, 0, 0);
	//enemy->getCamera()->setFarPlane(5);

	//#model creation

	model = new Drawable();
	model->setEntityType(EntityType::GuarddType);
	model->setModel(Manager::g_meshManager.getDynamicMesh("STATE"));
	model->setScale(0.05, 0.05, 0.05);
	model->setPosition({ 0.0, -11.0, 0.0, 1.0 });
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

	static float hSpeed = 1.50;
	static float hDir = 78;
	auto blendFwd = stateMachine->AddBlendSpace2DState("loco_fwd", &player->m_currentDirection, &player->m_currentSpeed, -90.0f, 90.f, 0.0f, 3.001f);
	auto blendBwd = stateMachine->AddBlendSpace2DState("loco_bwd", &player->m_currentDirection, &player->m_currentSpeed, -180.0f, 180.0f, 0.0f, 3.001f);

	auto& fwdToBwdL = blendFwd->AddOutState(blendBwd);
	fwdToBwdL.AddTransition(&player->m_currentDirection, -89.9999999f, 89.9999999f, SM::COMPARISON_OUTSIDE_RANGE);

	auto& bwdToFwdL = blendBwd->AddOutState(blendFwd);
	bwdToFwdL.AddTransition(&player->m_currentDirection, -90.f, 90.f, SM::COMPARISON_INSIDE_RANGE);

	//auto blendState = stateMachine->AddBlendSpace2DState("idle_states", &hDir, &hSpeed, -180.0, 180.0, 0.0, 3.1);

	blendFwd->AddRow(0.0, { { idle_clip.get(), -90.f }, { idle_clip.get(), 0.0f }, { idle_clip.get(), 90.0f } });
	blendFwd->AddRow(3.1, { {lft_clip.get(), -90.0f }, {fwd_clip.get(), 0.0f }, {rgt_clip.get(), 90.0f } });

	blendBwd->AddRow(0.0, { { idle_clip.get(), -180.0f }, { idle_clip.get(), -90.0f }, { idle_clip.get(), 0.0f }, { idle_clip.get(), 90.0f }, { idle_clip.get(), 180.0f } });
	blendBwd->AddRow(3.1, { {bwd_clip.get(), -180.0f }, {lft_clip.get(), -90.0f }, {fwd_clip.get(), 0.0f }, {rgt_clip.get(), 90.0f }, {bwd_clip.get(), 180.0f } });
	//blendState->AddBlendNodes(v);
	auto idleState = stateMachine->AddLoopState("idle", idle_clip.get());
	auto jumpState = stateMachine->AddLoopState("jump", jmp_clip.get());

	auto& idleToJump = idleState->AddOutState(jumpState);
	auto& locoToJump = blendFwd->AddOutState(jumpState);
	idleToJump.AddTransition(&player->m_jumpedThisFrame, true, SM::COMPARISON_EQUAL);
	locoToJump.AddTransition(&player->m_jumpedThisFrame, true, SM::COMPARISON_EQUAL);

	auto& idleToLoco = idleState->AddOutState(blendFwd);
	idleToLoco.AddTransition(&player->m_currentSpeed, 0.001f, SM::COMPARISON_GREATER_THAN);
	stateMachine->SetState("loco_fwd");
	stateMachine->SetModel(model->getAnimatedModel());
	auto& locoToIdle = blendFwd->AddOutState(idleState);
	locoToIdle.AddTransition(&player->m_currentSpeed, 0.001f, SM::COMPARISON_LESS_THAN);

	auto& jumpToIdle = jumpState->AddOutState(idleState);
	auto& jumpToLoco = jumpState->AddOutState(blendFwd);
	jumpToIdle.AddTransition(&player->m_currentSpeed, 0.01f, SM::COMPARISON_LESS_THAN);
	jumpToIdle.AddTransition(&player->m_isInAir, false, SM::COMPARISON_EQUAL);
	jumpToLoco.AddTransition(&player->m_isInAir, false, SM::COMPARISON_EQUAL);

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
	model->getAnimatedModel()->Update(deltaTime);
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