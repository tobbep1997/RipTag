#include "RipTagPCH.h"
#include "PlayState.h"
#include <DirectXCollision.h>

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


	Manager::g_meshManager.loadStaticMesh("PRESSUREPLATE");
	Manager::g_meshManager.loadStaticMesh("JOCKDOOR");
	Manager::g_textureManager.loadTextures("SPHERE");

	//Load assets
	{

	}

	future1.get();
	
	m_playerManager = new PlayerManager(&this->m_world);
	m_playerManager->RegisterThisInstanceToNetwork();
	m_playerManager->CreateLocalPlayer();



	CameraHandler::setActiveCamera(m_playerManager->getLocalPlayer()->getCamera());


	m_playerManager->getLocalPlayer()->Init(m_world, e_dynamicBody,0.5f,0.9f,0.5f);
	m_playerManager->getLocalPlayer()->setEntityType(EntityType::PlayerType);
	m_playerManager->getLocalPlayer()->setColor(10, 10, 0, 1);

	m_playerManager->getLocalPlayer()->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	m_playerManager->getLocalPlayer()->setScale(1.0f, 1.0f, 1.0f);
	m_playerManager->getLocalPlayer()->setPosition(0.0, -3.0, 0.0);
	m_playerManager->getLocalPlayer()->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_playerManager->getLocalPlayer()->setTextureTileMult(2, 2);
	
	//Do not remove pls <3
	{
	//	model->setModel(Manager::g_meshManager.getDynamicMesh("STATE"));
	//	model->getAnimatedModel()->SetSkeleton(Manager::g_animationManager.getSkeleton("STATE"));
	//	model->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	//	model->setScale({ 0.02, 0.02, 0.02, 1.0 });
	//	model->setPosition(4.0, 4.3, 0.0);

	//	//Number of states as argument, ok if bigger than actual states added
	//	auto& stateMachine = model->getAnimatedModel()->InitStateMachine(2);

	//	//Blend spaces - forward&backward
	//	SM::BlendSpace2D * blend_fwd = stateMachine->AddBlendSpace2DState(
	//		"walk_forward", //state name
	//		&m_playerManager->getLocalPlayer()->m_currentDirection, //x-axis driver
	//		&m_playerManager->getLocalPlayer()->m_currentSpeed, //y-axis driver
	//		-90.f, 90.f, //x-axis bounds
	//		0.0f, 3.001f //y-axis bounds
	//	);
	//	SM::BlendSpace2D * blend_bwd = stateMachine->AddBlendSpace2DState(
	//		"walk_backward", //state name
	//		&m_playerManager->getLocalPlayer()->m_currentDirection, //x-axis driver
	//		&m_playerManager->getLocalPlayer()->m_currentSpeed, //y-axis driver
	//		-180.f, 180.f, //x-axis bounds
	//		0.0f, 3.001f //y-axis bounds
	//	);

	//	//Add blendspace rows 
	//	//forward
	//	blend_fwd->AddRow(
	//		0.0f, //y placement
	//		{	//uses a vector initializer list for "convinience"
	//			{ Manager::g_animationManager.getAnimation("STATE", "IDLE_LONG_ANIMATION").get(), -90.f }, //the clip to use and x-placement
	//			{ Manager::g_animationManager.getAnimation("STATE", "IDLE_LONG_ANIMATION").get(), 0.f },
	//			{ Manager::g_animationManager.getAnimation("STATE", "IDLE_LONG_ANIMATION").get(), 90.f }
	//		}
	//	);
	//	blend_fwd->AddRow(
	//		3.1f, //y placement
	//		{	//uses a vector initializer list for "convinience"
	//			{ Manager::g_animationManager.getAnimation("STATE", "WALK_LEFT2_ANIMATION").get(), -90.f }, //the clip to use and x-placement
	//			{ Manager::g_animationManager.getAnimation("STATE", "WALK_FORWARD_ANIMATION").get(), 0.f },
	//			{ Manager::g_animationManager.getAnimation("STATE", "WALK_RIGHT2_ANIMATION").get(), 90.f }
	//		}
	//	);
	//	//
	//	blend_bwd->AddRow(
	//		0.0f, //y placement
	//		{	//uses a vector initializer list for "convinience"
	//			{ Manager::g_animationManager.getAnimation("STATE", "IDLE_LONG_ANIMATION").get(), -180.f }, //the clip to use and x-placement
	//			{ Manager::g_animationManager.getAnimation("STATE", "IDLE_LONG_ANIMATION").get(), -90.f },
	//			{ Manager::g_animationManager.getAnimation("STATE", "IDLE_LONG_ANIMATION").get(), 0.f },
	//			{ Manager::g_animationManager.getAnimation("STATE", "IDLE_LONG_ANIMATION").get(), 90.f },
	//			{ Manager::g_animationManager.getAnimation("STATE", "IDLE_LONG_ANIMATION").get(), 180.f }
	//		}
	//	);
	//	blend_bwd->AddRow(
	//		3.1f, //y placement
	//		{	//uses a vector initializer list for "convinience"
	//			{ Manager::g_animationManager.getAnimation("STATE", "WALK_BACKWARD_ANIMATION").get(), -180.f }, //the clip to use and x-placement
	//			{ Manager::g_animationManager.getAnimation("STATE", "WALK_BLEFT_ANIMATION").get(), -90.f },
	//			{ Manager::g_animationManager.getAnimation("STATE", "WALK_FORWARD_ANIMATION").get(), 0.f },
	//			{ Manager::g_animationManager.getAnimation("STATE", "WALK_BRIGHT_ANIMATION").get(), 90.f },
	//			{ Manager::g_animationManager.getAnimation("STATE", "WALK_BACKWARD_ANIMATION").get(), 180.f }
	//		}
	//	);

	//	//Adding out state / transitions
	//	SM::OutState & fwd_bwd_outstate = blend_fwd->AddOutState(blend_bwd);
	//	//Add transition condition
	//	fwd_bwd_outstate.AddTransition(
	//		&m_playerManager->getLocalPlayer()->m_currentDirection, //referenced variable for comparision
	//		-89.f, 89.f, //bound range for comparision
	//		SM::COMPARISON_OUTSIDE_RANGE //comparision condition
	//	);

	//	SM::OutState & bwd_fwd_outstate = blend_bwd->AddOutState(blend_fwd);
	//	//Add transition condition
	//	bwd_fwd_outstate.AddTransition(
	//		&m_playerManager->getLocalPlayer()->m_currentDirection, //referenced variable for comparision
	//		-90.f, 90.f, //bound range for comparision
	//		SM::COMPARISON_INSIDE_RANGE //comparision condition
	//	);

	//	//stateMachine->SetState("walk_forward");
	//	stateMachine->SetModel(model->getAnimatedModel());
	//	stateMachine->SetState("walk_forward");

	}

	m_levelHandler = new LevelHandler();
	m_levelHandler->Init(m_world, m_playerManager->getLocalPlayer());

	triggerHandler = new TriggerHandler();

	name = AudioEngine::LoadSoundEffect("../Assets/Audio/AmbientSounds/Cave.ogg", true);
	FMOD_VECTOR caveSoundAt = { -2.239762f, 6.5f, -1.4f };
	FMOD_VECTOR caveSoundAt2 = { -5.00677f, 6.5f, -10.8154f };
	
	AudioEngine::PlaySoundEffect(name, &caveSoundAt, AudioEngine::Other);
	AudioEngine::PlaySoundEffect(name, &caveSoundAt2, AudioEngine::Other);
	
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
	delete m_particleEmitter;
	delete m_contactListener;
	delete m_rayListener;

	AudioEngine::UnLoadSoundEffect(name);
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

	//5.5,5,-4.5

	/*ImGui::Begin("Player Setting");
	ImGui::SliderFloat("PositionX", &posX, -20.0f, 20.f);
	ImGui::SliderFloat("PositionY", &posY, -20.0f, 20.f);
	ImGui::SliderFloat("PositionZ", &posZ, -20.0f, 20.f);

	ImGui::SliderFloat("DirX", &xD, -180.0f, 180.f);
	ImGui::SliderFloat("DirY", &yD, -180.0f, 180.f);
	ImGui::SliderFloat("DirZ", &zD, -180.0f, 180.f);
	if (ImGui::Button("test"))
	{
		
	}
	
	ImGui::End();*/

	
	triggerHandler->Update(deltaTime);
	m_levelHandler->Update(deltaTime);
	m_playerManager->Update(deltaTime);

	m_particleEmitter->setPosition(6, 10, 0);
	m_particleEmitter->Update(deltaTime, m_playerManager->getLocalPlayer()->getCamera());

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

	
	TemporaryLobby();
#if _DEBUG
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

	
	
	_audioAgainstGuards(deltaTime);


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

	_lightCulling();

	m_playerManager->Draw();
	m_particleEmitter->Queue();


	p_renderingManager->Flush(*CameraHandler::getActiveCamera(), m_particleEmitter);

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

void PlayState::_audioAgainstGuards(double deltaTime)
{
	static double timer = 0.0f;
	timer += deltaTime;

	if (timer > 0.5)
	{
		timer = 0.0f;
		std::vector<FMOD::Channel*> channels = AudioEngine::getAllPlayingChannels();
		const std::vector<Enemy*>* enemies = m_levelHandler->getEnemies();
		int counter = 0;
		for (auto & e : *enemies)
		{
			float allSounds = 0.0f;
			float playerSounds = 0.0f;
			const DirectX::XMFLOAT4A & ePos = e->getPosition();
			const DirectX::XMFLOAT4A & pPos = m_playerManager->getLocalPlayer()->getPosition();
			DirectX::XMVECTOR vEPos = DirectX::XMLoadFloat4A(&ePos);
			DirectX::XMVECTOR vPPos = DirectX::XMLoadFloat4A(&pPos);
			DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(vPPos, vEPos);
			float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(dir));
			if (length < 20.0f)
			{
				for (auto & c : channels)
				{
					AudioEngine::SoundType * soundType = nullptr;
					FMOD_RESULT res = c->getUserData((void**)&soundType);
					if (res == FMOD_OK)
					{
						FMOD_VECTOR soundPos;
						if (c->get3DAttributes(&soundPos, nullptr) == FMOD_OK)
						{
							DirectX::XMVECTOR vSPos = DirectX::XMVectorSet(soundPos.x, soundPos.y, soundPos.z, 1.0f);
							DirectX::XMVECTOR soundDir = DirectX::XMVectorSubtract(vSPos, vEPos);
							float lengthSquared = DirectX::XMVectorGetX(DirectX::XMVector3Dot(soundDir, soundDir));

							DirectX::XMFLOAT4A soundDirNormalized;
							DirectX::XMStoreFloat4A(&soundDirNormalized, DirectX::XMVector3Normalize(soundDir));
							
							RayCastListener::Ray * ray = RipExtern::m_rayListener->ShotRay(e->getBody(), ePos, soundDirNormalized, sqrt(lengthSquared));
							float occ = 1.0f;
							if (ray)
							{
								for (auto & c : ray->GetRayContacts())
								{
									std::string tag = c->contactShape->GetBody()->GetObjectTag();
									if (tag == "WORLD" || tag == "NULL")
									{
										occ *= 0.15f;
									}
									else if (tag == "BLINK_WALL")
									{
										occ *= 0.50f;
									}
								}
							}

							float volume = 0;
							c->getVolume(&volume);
							volume *= 100.0f;
							volume *= occ;
							float addThis = (volume / lengthSquared) * DirectX::XM_PI;

							switch (*soundType)
							{
							case AudioEngine::Player:
								allSounds += addThis;
								playerSounds += addThis;
								break;
							case AudioEngine::Other:
								allSounds += addThis;
								break;
							}
						}
					}
				}

				if (playerSounds > 0.1)
				{
					if (playerSounds / allSounds > 0.3f)
					{
						// TODO :: Update the enemy, it has heard the player.
					}
				}
			}
			counter++;
		}
	}
}

void PlayState::_lightCulling()
{
	Player * p = m_playerManager->getLocalPlayer();
	DirectX::BoundingFrustum PlayerWorldBox;
	DirectX::XMMATRIX viewInv, proj;

	proj = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&p->getCamera()->getProjection()));
	viewInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&p->getCamera()->getView())));
	DirectX::BoundingFrustum::CreateFromMatrix(PlayerWorldBox, proj);
	PlayerWorldBox.Transform(PlayerWorldBox, viewInv);
	const DirectX::XMFLOAT4A & pPos = p->getPosition();
	DirectX::XMVECTOR vpPos = DirectX::XMLoadFloat4A(&pPos);

	for (auto & light : DX::g_lights)
	{
		light->DisableSides(PointLight::ShadowDir::XYZ_ALL);

		const DirectX::XMFLOAT4A & lPos = light->getPosition();
		DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&lPos), vpPos);
		float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(dir));
		if (length < p->getCamera()->getFarPlane())
		{
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