#include "RipTagPCH.h"
#include "PlayState.h"
#include <DirectXCollision.h>


b3World * RipExtern::g_world = nullptr;
ContactListener * RipExtern::m_contactListener;
RayCastListener * RipExtern::m_rayListener;

bool PlayState::m_youlost = false;

PlayState::PlayState(RenderingManager * rm, void * coopData) : State(rm)
{	
	//we dont care about this yet
	if (coopData)
	{
		isCoop = true;
		pCoopData = (CoopData*)coopData;
	}
	//DO NOT USE, USE Load Function
	//DO NOT USE, USE Load Function
	//DO NOT USE, USE Load Function
	//DO NOT USE, USE Load Function
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
	//DO NOT USE, USE Load Function
	//DO NOT USE, USE Load Function
	//DO NOT USE, USE Load Function
	//DO NOT USE, USE Load Function
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

	AudioEngine::UnLoadSoundEffect(name);
}

void PlayState::Update(double deltaTime)
{
	m_step.dt = deltaTime;
	m_step.velocityIterations = 2;
	m_step.sleeping = false;
	m_firstRun = false;
			
	triggerHandler->Update(deltaTime);
	m_levelHandler->Update(deltaTime);
	m_playerManager->Update(deltaTime);


	m_playerManager->PhysicsUpdate();
	
	
	
	m_contactListener->ClearContactQueue();
	m_rayListener->ClearConsumedContacts();

	m_deltaTime = deltaTime;
	m_physicsCondition.notify_all();
	
	
	
	if (InputHandler::getShowCursor() != FALSE)
		InputHandler::setShowCursor(FALSE);	   

	
#if _DEBUG
#endif
	if (GamePadHandler::IsSelectPressed())
	{
		Input::SetActivateGamepad(Input::isUsingGamepad());
	}

	if (Input::Exit() || GamePadHandler::IsStartPressed())
	{
		m_destoryPhysicsThread = true;
		m_physicsCondition.notify_all();
		

		if (m_physicsThread.joinable())
		{
			m_physicsThread.join();
		}
		BackToMenu();
	}

	if (m_youlost)
	{
		m_destoryPhysicsThread = true;
		m_physicsCondition.notify_all();


		if (m_physicsThread.joinable())
		{
			m_physicsThread.join();
		}
		pushNewState(new LoseState(p_renderingManager));
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
	//DrawWorldCollisionboxes();
	
	p_renderingManager->Flush(*CameraHandler::getActiveCamera());
}

void PlayState::setYouLost(const bool& youLost)
{
	m_youlost = youLost;
}

void PlayState::_PhyscisThread(double deltaTime)
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
			Enemy::SoundLocation sl;
			sl.percentage = 0.0f;
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
							
							float occ = 1.0f;
							/*
							RayCastListener::Ray * ray = RipExtern::m_rayListener->ShotRay(e->getBody(), ePos, soundDirNormalized, sqrt(lengthSquared));
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
							}*/

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
								if (playerSounds > sl.percentage)
								{
									sl.soundPos.x = soundPos.x;
									sl.soundPos.y = soundPos.y;
									sl.soundPos.z = soundPos.z;
								}
								break;
							case AudioEngine::Other:
								allSounds += addThis;
								break;
							}
						}
					}
				}

				sl.percentage = playerSounds / allSounds;
				e->setSoundLocation(sl);
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

#include "EngineSource\Structs.h"
#include "EngineSource\3D Engine\Model\Meshes\StaticMesh.h"

void PlayState::DrawWorldCollisionboxes(const std::string & type)
{
	static const DirectX::XMFLOAT4A _SXMcube[] =
	{
		{ 1.0,	-1.0,  1.0, 1.0},	{-1.0,	-1.0,	-1.0, 1.0},	{ 1.0,	-1.0,	-1.0, 1.0},
		{-1.0,	 1.0, -1.0, 1.0},	{ 1.0,	 1.0,	 1.0, 1.0},	{ 1.0,	 1.0,	-1.0, 1.0},
		{ 1.0,	 1.0, -1.0, 1.0},	{ 1.0,	-1.0,	 1.0, 1.0},	{ 1.0,	-1.0,	-1.0, 1.0},
		{ 1.0,	 1.0,  1.0, 1.0},	{-1.0,	-1.0,	 1.0, 1.0},	{ 1.0,	-1.0,	 1.0, 1.0},
		{-1.0,	-1.0,  1.0, 1.0},	{-1.0,	 1.0,	-1.0, 1.0},	{-1.0,	-1.0,	-1.0, 1.0},
		{ 1.0,	-1.0, -1.0, 1.0},	{-1.0,	 1.0,	-1.0, 1.0},	{ 1.0,	 1.0,	-1.0, 1.0},
		{ 1.0,	-1.0,  1.0, 1.0},	{-1.0,	-1.0,	 1.0, 1.0},	{-1.0,	-1.0,	-1.0, 1.0},
		{-1.0,	 1.0, -1.0, 1.0},	{-1.0,	 1.0,	 1.0, 1.0},	{ 1.0,	 1.0,	 1.0, 1.0},
		{ 1.0,	 1.0, -1.0, 1.0},	{ 1.0,	 1.0,	 1.0, 1.0},	{ 1.0,	-1.0,	 1.0, 1.0},
		{ 1.0,	 1.0,  1.0, 1.0},	{-1.0,	 1.0,	 1.0, 1.0},	{-1.0,	-1.0,	 1.0, 1.0},
		{-1.0,	-1.0,  1.0, 1.0},	{-1.0,	 1.0,	 1.0, 1.0},	{-1.0,	 1.0,	-1.0, 1.0},
		{ 1.0,	-1.0, -1.0, 1.0},	{-1.0,	-1.0,	-1.0, 1.0},	{-1.0,	 1.0,	-1.0, 1.0}
	};
	static std::vector<Drawable*> _drawables;
	static std::vector<StaticVertex> _vertices;
	static StaticMesh _sm;


	static bool _loaded = false;

	if (!_loaded)
	{
		for (int i = 0; i < 36; i++)
		{
			StaticVertex v;
			v.pos = _SXMcube[i];
			_vertices.push_back(v);
		}
		_sm.setVertices(_vertices);
		
		
		_loaded = true;
		const b3Body * b = m_world.getBodyList();

		while (b != nullptr)
		{
			if (b->GetObjectTag() != "TELEPORT")
			{
				if (type == "" || b->GetObjectTag() == type)
				{
					b3Shape * s = b->GetShapeList();
					auto b3BodyRot = b->GetTransform().rotation;
					while (s != nullptr)
					{
						Drawable * d = new Drawable;
						d->setModel(&_sm);
						DirectX::XMFLOAT4A shapePos = {
							s->GetTransform().translation.x + b->GetTransform().translation.x,
							s->GetTransform().translation.y + b->GetTransform().translation.y,
							s->GetTransform().translation.z + b->GetTransform().translation.z,
						1.0f
						};
						auto b3ShapeRot = s->GetTransform().rotation;
						DirectX::XMFLOAT3X3 shapeRot;
						shapeRot._11 = b3BodyRot.x.x;
						shapeRot._12 = b3BodyRot.x.y;
						shapeRot._13 = b3BodyRot.x.z;
						shapeRot._21 = b3BodyRot.y.x;
						shapeRot._22 = b3BodyRot.y.y;
						shapeRot._23 = b3BodyRot.y.z;
						shapeRot._31 = b3BodyRot.z.x;
						shapeRot._32 = b3BodyRot.z.y;
						shapeRot._33 = b3BodyRot.z.z;

						DirectX::XMMATRIX rot = DirectX::XMLoadFloat3x3(&shapeRot);
						const b3Hull * h = dynamic_cast<b3Polyhedron*>(s)->GetHull();
						DirectX::XMMATRIX scl = DirectX::XMMatrixScaling(h->rawScale.x, h->rawScale.y, h->rawScale.z);
						DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(shapePos.x, shapePos.y, shapePos.z);

						DirectX::XMMATRIX world = scl * rot * trans;
						d->ForceWorld(DirectX::XMMatrixTranspose(world));
						_drawables.push_back(d);

						s = s->GetNext();
					}
				}
			}
			b = b->GetNext();
		}
	}
	else
	{
		int counter = 0;
		const b3Body * b = m_world.getBodyList();
		while (b != nullptr)
		{
			if (b->GetObjectTag() != "TELEPORT")
			{
				if (type == "" || b->GetObjectTag() == type)
				{
					b3Shape * s = b->GetShapeList();
					auto b3BodyRot = b->GetTransform().rotation;
					while (s != nullptr)
					{
						DirectX::XMFLOAT4A shapePos = {
							s->GetTransform().translation.x + b->GetTransform().translation.x,
							s->GetTransform().translation.y + b->GetTransform().translation.y,
							s->GetTransform().translation.z + b->GetTransform().translation.z,
						1.0f
						};
						auto b3ShapeRot = s->GetTransform().rotation;
						DirectX::XMFLOAT3X3 shapeRot;
						shapeRot._11 = b3BodyRot.x.x;
						shapeRot._12 = b3BodyRot.x.y;
						shapeRot._13 = b3BodyRot.x.z;
						shapeRot._21 = b3BodyRot.y.x;
						shapeRot._22 = b3BodyRot.y.y;
						shapeRot._23 = b3BodyRot.y.z;
						shapeRot._31 = b3BodyRot.z.x;
						shapeRot._32 = b3BodyRot.z.y;
						shapeRot._33 = b3BodyRot.z.z;

						DirectX::XMMATRIX rot = DirectX::XMLoadFloat3x3(&shapeRot);
						const b3Hull * h = dynamic_cast<b3Polyhedron*>(s)->GetHull();
						DirectX::XMMATRIX scl = DirectX::XMMatrixScaling(h->rawScale.x, h->rawScale.y, h->rawScale.z);
						DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(shapePos.x, shapePos.y, shapePos.z);

						DirectX::XMMATRIX world = scl * rot * trans;
						_drawables[counter++]->ForceWorld(DirectX::XMMatrixTranspose(world));
						
						s = s->GetNext();
					}
				}
			
			}
			b = b->GetNext();
		}
	}


	for (auto & d : _drawables)
		d->DrawWireFrame();
}

void PlayState::unLoad()
{
	Manager::g_textureManager.UnloadTexture("KOMBIN");
	Manager::g_textureManager.UnloadTexture("SPHERE");
	Manager::g_textureManager.UnloadTexture("PIRASRUM");
	Manager::g_textureManager.UnloadTexture("DAB");
	Manager::g_textureManager.UnloadAllTexture();
	Manager::g_meshManager.UnloadAllMeshes();

	std::cout << "PlayState unLoad" << std::endl;

}

void PlayState::Load()
{
	Manager::g_textureManager.loadTextures("KOMBIN");
	Manager::g_textureManager.loadTextures("SPHERE");
	Manager::g_textureManager.loadTextures("PIRASRUM");
	Manager::g_textureManager.loadTextures("DAB");
	Manager::g_textureManager.loadTextures("CROSS");
	Manager::g_textureManager.loadTextures("FML");
	Manager::g_textureManager.loadTextures("VISIBILITYICON");
	Manager::g_textureManager.loadTextures("BLACK");
	Manager::g_textureManager.loadTextures("BAR");

	//Reset the all relevant networking maps - this is crucial since Multiplayer is a Singleton
	Network::Multiplayer::LocalPlayerOnSendMap.clear();
	Network::Multiplayer::RemotePlayerOnReceiveMap.clear();

	m_youlost = false;
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

	//Load assets
	{
		//:c *queue sad music*
	}

	future1.get();

	m_playerManager = new PlayerManager(&this->m_world);
	m_playerManager->CreateLocalPlayer();
	
	m_levelHandler = new LevelHandler();
	m_levelHandler->Init(m_world, m_playerManager->getLocalPlayer());

	if (isCoop)
	{
		this->m_seed = pCoopData->seed;
		auto startingPositions = m_levelHandler->getStartingPositions();
		DirectX::XMFLOAT4 posOne = std::get<0>(startingPositions);
		DirectX::XMFLOAT4 posTwo = std::get<1>(startingPositions);
		switch (pCoopData->localPlayerCharacter)
		{
		case 1:
			m_playerManager->getLocalPlayer()->setPosition(posOne.x, posOne.y, posOne.z, posOne.w);
			m_playerManager->getLocalPlayer()->SetAbilitySet(1);
			m_playerManager->CreateRemotePlayer({ posTwo.x, posTwo.y, posTwo.z, posTwo.w }, pCoopData->remoteID);
			m_playerManager->getRemotePlayer()->SetAbilitySet(2);
			break;
		case 2:
			m_playerManager->getLocalPlayer()->setPosition(posTwo.x, posTwo.y, posTwo.z, posTwo.w);
			m_playerManager->getLocalPlayer()->SetAbilitySet(2);
			m_playerManager->CreateRemotePlayer({ posOne.x, posOne.y, posOne.z, posOne.w }, pCoopData->remoteID);
			m_playerManager->getRemotePlayer()->SetAbilitySet(1);
			break;
		}
		m_playerManager->isCoop(true);
		//free up memory when we are done with this data
		delete pCoopData;
		pCoopData = nullptr;
	}
	else
	{
		m_playerManager->isCoop(false);
	}

	triggerHandler = new TriggerHandler();
	if (isCoop)
		triggerHandler->RegisterThisInstanceToNetwork();

	name = AudioEngine::LoadSoundEffect("../Assets/Audio/AmbientSounds/Cave.ogg", true);
	FMOD_VECTOR caveSoundAt = { -2.239762f, 6.5f, -1.4f };
	FMOD_VECTOR caveSoundAt2 = { -5.00677f, 6.5f, -10.8154f };

	AudioEngine::PlaySoundEffect(name, &caveSoundAt, AudioEngine::Other);
	AudioEngine::PlaySoundEffect(name, &caveSoundAt2, AudioEngine::Other);

	FMOD_VECTOR reverbAt = { -5.94999f, 7.0f, 3.88291f };

	AudioEngine::CreateReverb(reverbAt, 15.0f, 40.0f);

	



	CameraHandler::setActiveCamera(m_playerManager->getLocalPlayer()->getCamera());

	Input::ResetMouse();

	m_step.velocityIterations = 1;
	m_step.sleeping = false;
	m_firstRun = false;

	m_physicsThread = std::thread(&PlayState::_PhyscisThread, this, 0);

	std::cout << "PlayState Load" << std::endl;
}
