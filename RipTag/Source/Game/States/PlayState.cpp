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

	/*Manager::g_meshManager.loadStaticMesh("SPHERE");*/
	Manager::g_animationManager.loadSkeleton("../Assets/STATEFOLDER/STATE_SKELETON.bin", "STATE");
	Manager::g_animationManager.loadClipCollection("STATE", "STATE", "../Assets/STATEFOLDER", Manager::g_animationManager.getSkeleton("STATE"));
	Manager::g_meshManager.loadDynamicMesh("STATE");
	/*Manager::g_textureManager.loadTextures("SPHERE");*/

//	future.get();
	future1.get();


	player = new Player();

	CameraHandler::setActiveCamera(player->getCamera());

	
	player->Init(m_world, e_dynamicBody, 0.5f,0.5f,0.5f);
	//
	//player->setPosition(0, 100, 0);
	model = new Drawable();
	model->setEntityType(EntityType::GuarddType);
	model->setModel(Manager::g_meshManager.getDynamicMesh("STATE"));
	model->setScale(0.05, 0.05, 0.05);
	model->setPosition({ 0.0, 200, 0.0, 1.0 });
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

	
	m_levelHandler.setPlayer(player);
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
	
	
	player->Release(m_world);
	
	delete player;
	delete model;
	delete triggerHandler;
	pressureplate->Release(*RipExtern::g_world);

	delete pressureplate;
	lever->Release(*RipExtern::g_world);
	delete lever;
	delete door;
	delete m_contactListener;
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

	player->Update(deltaTime);
	m_levelHandler.Update(deltaTime);
	
	
	m_step.dt = deltaTime;
	m_step.velocityIterations = 20;
	
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
		InputHandler::setShowCursor(FALSE);
	}
	else
	{
		InputHandler::setShowCursor(TRUE);
	}


}

void PlayState::Draw()
{
	m_levelHandler.Draw();
	
	door->Draw();

	player->Draw();
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