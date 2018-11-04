#include "RipTagPCH.h"
#include "PlayState.h"

#include <AudioEngine.h>


#include "EngineSource/Helper/Timer.h"
#include "EngineSource/3D Engine/Extern.h"
#include "EngineSource/3D Engine/Model/Managers/MeshManager.h"
#include "EngineSource/3D Engine/Model/Managers/TextureManager.h"
#include "EngineSource/3D Engine/Model/Meshes/AnimatedModel.h"
#include "InputManager/XboxInput/GamePadHandler.h"

#include "ImportLibrary/formatImporter.h"
#include "2D Engine/Quad/Quad.h"



b3World * RipExtern::g_world = nullptr;
ContactListener * RipExtern::m_contactListener;
RayCastListener * RipExtern::m_rayListener;


PlayState::PlayState(RenderingManager * rm) : State(rm)
{	

	RipExtern::g_world = &m_world;
	m_contactListener = new ContactListener();
	RipExtern::m_contactListener = m_contactListener;
	RipExtern::g_world->SetContactListener(m_contactListener);
	m_rayListener = new RayCastListener();
	RipExtern::m_rayListener = m_rayListener;
	CameraHandler::Instance();
	auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	Manager::g_animationManager.loadSkeleton("../Assets/STATEFOLDER/STATE_SKELETON.bin", "STATE");
	Manager::g_animationManager.loadClipCollection("STATE", "STATE", "../Assets/STATEFOLDER", Manager::g_animationManager.getSkeleton("STATE"));
	Manager::g_meshManager.loadDynamicMesh("STATE");
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));

	//Load assets
	{

	}

	future1.get();
	
	m_playerManager = new PlayerManager(&this->m_world);
	m_playerManager->RegisterThisInstanceToNetwork();
	m_playerManager->CreateLocalPlayer();



	CameraHandler::setActiveCamera(m_playerManager->getLocalPlayer()->getCamera());


	m_playerManager->getLocalPlayer()->Init(m_world, e_dynamicBody,0.5f,0.5f,0.5f);
	m_playerManager->getLocalPlayer()->setEntityType(EntityType::PlayerType);
	m_playerManager->getLocalPlayer()->setColor(10, 10, 0, 1);

	m_playerManager->getLocalPlayer()->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	m_playerManager->getLocalPlayer()->setScale(1.0f, 1.0f, 1.0f);
	m_playerManager->getLocalPlayer()->setPosition(0.0, -3.0, 0.0);
	m_playerManager->getLocalPlayer()->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_playerManager->getLocalPlayer()->setTextureTileMult(2, 2);
	
	model->setModel(Manager::g_meshManager.getDynamicMesh("STATE"));
	model->getAnimatedModel()->SetSkeleton(Manager::g_animationManager.getSkeleton("STATE"));
	model->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	model->setScale({ 0.02, 0.02, 0.02, 1.0 });
	model->setPosition(4.0, 6.0, 0.0);

	auto& stateMachine = model->getAnimatedModel()->InitStateMachine(2);

	{
		//Blend spaces - forward&backward
		SM::BlendSpace2D * blend_fwd = stateMachine->AddBlendSpace2DState(
			"walk_forward", //state name
			&m_playerManager->getLocalPlayer()->m_currentDirection, //x-axis driver
			&m_playerManager->getLocalPlayer()->m_currentSpeed, //y-axis driver
			-90.f, 90.f, //x-axis bounds
			0.0f, 3.001f //y-axis bounds
		);
		SM::BlendSpace2D * blend_bwd = stateMachine->AddBlendSpace2DState(
			"walk_backward", //state name
			&m_playerManager->getLocalPlayer()->m_currentDirection, //x-axis driver
			&m_playerManager->getLocalPlayer()->m_currentSpeed, //y-axis driver
			-180.f, 180.f, //x-axis bounds
			0.0f, 3.001f //y-axis bounds
		);

		//Add blendspace rows 
		//forward
		blend_fwd->AddRow(
			0.0f, //y placement
			{	//uses a vector initializer list for "convinience"
				{ Manager::g_animationManager.getAnimation("STATE", "IDLE_ANIMATION").get(), -90.f }, //the clip to use and x-placement
				{ Manager::g_animationManager.getAnimation("STATE", "IDLE_ANIMATION").get(), 0.f },
				{ Manager::g_animationManager.getAnimation("STATE", "IDLE_ANIMATION").get(), 90.f }
			}
		);
		blend_fwd->AddRow(
			3.1f, //y placement
			{	//uses a vector initializer list for "convinience"
				{ Manager::g_animationManager.getAnimation("STATE", "WALK_LEFT2_ANIMATION").get(), -90.f }, //the clip to use and x-placement
				{ Manager::g_animationManager.getAnimation("STATE", "WALK_FORWARD_ANIMATION").get(), 0.f },
				{ Manager::g_animationManager.getAnimation("STATE", "WALK_RIGHT2_ANIMATION").get(), 90.f }
			}
		);
		//
		blend_bwd->AddRow(
			0.0f, //y placement
			{	//uses a vector initializer list for "convinience"
				{ Manager::g_animationManager.getAnimation("STATE", "IDLE_ANIMATION").get(), -180.f }, //the clip to use and x-placement
				{ Manager::g_animationManager.getAnimation("STATE", "IDLE_ANIMATION").get(), -90.f },
				{ Manager::g_animationManager.getAnimation("STATE", "IDLE_ANIMATION").get(), 0.f },
				{ Manager::g_animationManager.getAnimation("STATE", "IDLE_ANIMATION").get(), 90.f },
				{ Manager::g_animationManager.getAnimation("STATE", "IDLE_ANIMATION").get(), 180.f }
			}
		);
		blend_bwd->AddRow(
			3.1f, //y placement
			{	//uses a vector initializer list for "convinience"
				{ Manager::g_animationManager.getAnimation("STATE", "WALK_BACKWARD_ANIMATION").get(), -180.f }, //the clip to use and x-placement
				{ Manager::g_animationManager.getAnimation("STATE", "WALK_BLEFT_ANIMATION").get(), -90.f },
				{ Manager::g_animationManager.getAnimation("STATE", "WALK_FORWARD_ANIMATION").get(), 0.f },
				{ Manager::g_animationManager.getAnimation("STATE", "WALK_BRIGHT_ANIMATION").get(), 90.f },
				{ Manager::g_animationManager.getAnimation("STATE", "WALK_BACKWARD_ANIMATION").get(), 180.f }
			}
		);

		//Adding out state / transitions
		SM::OutState & fwd_bwd_outstate = blend_fwd->AddOutState(blend_bwd);
		//Add transition condition
		fwd_bwd_outstate.AddTransition(
			&m_playerManager->getLocalPlayer()->m_currentDirection, //referenced variable for comparision
			-89.f, 89.f, //bound range for comparision
			SM::COMPARISON_OUTSIDE_RANGE //comparision condition
		);

		SM::OutState & bwd_fwd_outstate = blend_bwd->AddOutState(blend_fwd);
		//Add transition condition
		bwd_fwd_outstate.AddTransition(
			&m_playerManager->getLocalPlayer()->m_currentDirection, //referenced variable for comparision
			-90.f, 90.f, //bound range for comparision
			SM::COMPARISON_INSIDE_RANGE //comparision condition
		);

		stateMachine->SetState("walk_forward");
		stateMachine->SetModel(model->getAnimatedModel());
	}

	m_levelHandler = new LevelHandler();
	m_levelHandler->Init(m_world, m_playerManager->getLocalPlayer());

	triggerHandler = new TriggerHandler();

	std::string name = AudioEngine::LoadSoundEffect("../Assets/Audio/AmbientSounds/Cave.ogg", true);
	FMOD_VECTOR caveSoundAt = { -2.239762f, 6.5f, -1.4f };
	FMOD_VECTOR caveSoundAt2 = { -5.00677f, 6.5f, -10.8154f };
	TEEEMPCHANNEL = AudioEngine::PlaySoundEffect(name, &caveSoundAt);
	AudioEngine::PlaySoundEffect(name, &caveSoundAt2);
	FMOD_VECTOR reverbAt = { -5.94999f, 7.0f, 3.88291f };

	AudioEngine::CreateReverb(reverbAt, 15.0f, 40.0f);

	Input::ResetMouse();

	m_step.velocityIterations = 1;
	m_step.sleeping = false;
	m_firstRun = false;
}

PlayState::~PlayState()
{
	m_levelHandler->Release();
	delete m_levelHandler;

	m_playerManager->getLocalPlayer()->Release(m_world);
	
	delete m_playerManager;

	delete triggerHandler;

	delete m_contactListener;
	delete m_rayListener;
}

void PlayState::Update(double deltaTime)
{
	m_step.dt = deltaTime;
	m_step.velocityIterations = 2;
	m_step.sleeping = false;
	m_firstRun = false;

	triggerHandler->Update(deltaTime);
	m_levelHandler->Update(deltaTime);
	model->getAnimatedModel()->Update(deltaTime);
	m_contactListener->ClearContactQueue();
	m_rayListener->ClearConsumedContacts();
	if (deltaTime <= 0.65f)
	{
		m_world.Step(m_step);
	}
	
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

	//model->getAnimatedModel()->Update(deltaTime);
	
	

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
	m_levelHandler->Draw();
	model->Draw();
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