#include "RipTagPCH.h"
#include "PlayState.h"


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
	
	m_physicsThread = std::thread(&PlayState::testtThread, this, 0);
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
	
	/*if (m_physicsThread.joinable())
	{
		m_physicsThread.join();
	}*/
	
	triggerHandler->Update(deltaTime);
	m_levelHandler->Update(deltaTime);
	m_playerManager->Update(deltaTime);

	//model->getAnimatedModel()->Update(deltaTime);

	m_playerManager->PhysicsUpdate();
	
	
	
	
	m_contactListener->ClearContactQueue();
	m_rayListener->ClearConsumedContacts();

	/*m_deltaTime = deltaTime;
	std::lock_guard<std::mutex> lg(m_physicsMutex);*/
	m_deltaTime = deltaTime;
	m_physicsCondition.notify_all();
	
	
	
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
	

	if (Input::Exit() || GamePadHandler::IsStartPressed())
	{
		m_destoryPhysicsThread = true;
		m_physicsCondition.notify_all();
		

		if (m_physicsThread.joinable())
		{
			m_physicsThread.join();
		}
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

	/*for (auto & lights : DX::g_lights)
	{
		RayCastListener::RayContact * rc = RipExtern::m_rayListener->ShotRay(m_playerManager->getLocalPlayer()->getBody(),
			lights->getPosition(),
			lights->getDir(*m_playerManager->getLocalPlayer()->getBody()),
			lights->getFarPlane() / cos(lights->getFOV() / 2.0f));
		if (rc)
		{		
			lights->setUpdate(rc->contactShape->GetBody()->GetObjectTag() == "PLAYER");
		}
	}*/
	_lightCulling();

	m_playerManager->Draw();
		
	p_renderingManager->Flush(*CameraHandler::getActiveCamera());	
}

void PlayState::testtThread(double deltaTime)
{
	while (m_destoryPhysicsThread == false)
	{
		std::unique_lock<std::mutex> lock(m_physicsMutex);
		m_physicsCondition.wait(lock);

		if (m_deltaTime <= 0.65f)
		{
			m_world.Step(m_step);
		}
	}
}
#include <DirectXCollision.h>
void PlayState::_lightCulling()
{
	Player * p = m_playerManager->getLocalPlayer();
	DirectX::BoundingFrustum PlayerWorldBox;
	DirectX::XMMATRIX viewInv, proj;

	proj = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&p->getCamera()->getProjection()));
	viewInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&p->getCamera()->getView())));
	DirectX::BoundingFrustum::CreateFromMatrix(PlayerWorldBox, proj);
	PlayerWorldBox.Transform(PlayerWorldBox, viewInv);

	for (auto & light : DX::g_lights)
	{
		light->DisableSides(PointLight::ShadowDir::XYZ_ALL);
		const std::vector<Camera*> & sidesVec = light->getSides();
		int counter = 0;
		for (auto & sides : sidesVec)
		{
			DirectX::BoundingFrustum WorldBox;
			DirectX::XMMATRIX sViewInv, sProj;
			sProj = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&sides->getProjection()));
			sViewInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&sides->getView())));
			DirectX::BoundingFrustum::CreateFromMatrix(WorldBox, sProj);
			WorldBox.Transform(WorldBox, sViewInv);
			if (PlayerWorldBox.Intersects(WorldBox))
			{

				light->EnableSides((PointLight::ShadowDir)counter);
			}
			counter++;
		}
	}
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