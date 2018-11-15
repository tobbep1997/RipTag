#include "RipTagPCH.h"
#include "PlayState.h"
#include <DirectXCollision.h>


std::vector<std::string> RipSounds::g_stepsStone;
std::string				 RipSounds::g_leverActivate;
std::string				 RipSounds::g_leverDeactivate;
std::string				 RipSounds::g_pressurePlateActivate;
std::string				 RipSounds::g_pressurePlateDeactivate;
std::string				 RipSounds::g_torch;
std::string				 RipSounds::g_windAndDrip;
std::string				 RipSounds::g_phase;

b3World * RipExtern::g_world = nullptr;
ContactListener * RipExtern::g_contactListener;
RayCastListener * RipExtern::g_rayListener;

bool PlayState::m_youlost = false;

PlayState::PlayState(RenderingManager * rm, void * coopData) : State(rm)
{	
	//USE LOAD NOT THIS
	//we dont care about this yet
	if (coopData)
	{
		isCoop = true;
		pCoopData = (CoopData*)coopData;
	}
	
}

PlayState::~PlayState()
{

	m_levelHandler->Release();
	delete m_levelHandler;

	m_playerManager->getLocalPlayer()->Release(m_world);
	delete m_playerManager;


	//delete triggerHandler;

	delete m_contactListener;
	delete m_rayListener;

}

void PlayState::Update(double deltaTime)
{
	m_step.dt = deltaTime;
	m_step.velocityIterations = 2;
	m_step.sleeping = false;
	m_firstRun = false;
		//int i = 0;
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

#ifdef _DEBUG
	//DrawWorldCollisionboxes();
#endif
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
							float occ = 1.0f;
							if (!DirectX::XMVectorGetX(DirectX::XMVectorEqual(soundDir, DirectX::XMVectorZero())))
							{
								DirectX::XMFLOAT4A soundDirNormalized;
								DirectX::XMStoreFloat4A(&soundDirNormalized, DirectX::XMVector3Normalize(soundDir));

								RayCastListener::Ray * ray = RipExtern::g_rayListener->ShotRay(e->getBody(), ePos, soundDirNormalized, sqrt(lengthSquared));
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
							}
							

							float volume = 0;
							c->getVolume(&volume);
							volume *= 100.0f;
							volume *= occ;
							float addThis = (volume / (lengthSquared * 3));

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

#ifdef _DEBUG
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
#endif

void PlayState::unLoad()
{
	std::cout << "PlayState unLoad" << std::endl;

	Manager::g_textureManager.UnloadTexture("KOMBIN");
	Manager::g_textureManager.UnloadTexture("SPHERE");
	Manager::g_textureManager.UnloadTexture("PIRASRUM");
	Manager::g_textureManager.UnloadTexture("DAB");
	Manager::g_textureManager.UnloadAllTexture();
	Manager::g_meshManager.UnloadAllMeshes();

	for (auto & s : RipSounds::g_stepsStone)
		AudioEngine::UnLoadSoundEffect(s);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_windAndDrip);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_leverActivate);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_leverDeactivate);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_pressurePlateActivate);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_pressurePlateDeactivate);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_torch);
}

void PlayState::Load()
{
	std::cout << "PlayState Load" << std::endl;

	//Initially Clear network maps
	if (isCoop)
	{
		//Reset the all relevant networking maps - this is crucial since Multiplayer is a Singleton
		Network::Multiplayer::LocalPlayerOnSendMap.clear();
		Network::Multiplayer::RemotePlayerOnReceiveMap.clear();
	}

	m_youlost = false;
	Input::ResetMouse();
	CameraHandler::Instance();
	
	_loadSound();
	_loadTextures();
	_loadPhysics();
	_loadMeshes();
	_loadPlayers();
	_loadNetwork();

	m_physicsThread = std::thread(&PlayState::_PhyscisThread, this, 0);
}

void PlayState::_loadTextures()
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
	Manager::g_textureManager.loadTextures("STATE");
}

void PlayState::_loadPhysics()
{
	RipExtern::g_world = &m_world;
	m_contactListener = new ContactListener();
	RipExtern::g_contactListener = m_contactListener;
	RipExtern::g_world->SetContactListener(m_contactListener);
	m_rayListener = new RayCastListener();
	RipExtern::g_rayListener = m_rayListener;
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));
	// triggerHandler = new TriggerHandler();
	m_step.velocityIterations = 1;
	m_step.sleeping = false;
	m_firstRun = false;
}

void PlayState::_loadMeshes()
{
	auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	Manager::g_animationManager.loadSkeleton("../Assets/STATEFOLDER/STATE_SKELETON.bin", "STATE");
	Manager::g_animationManager.loadClipCollection("STATE", "STATE", "../Assets/STATEFOLDER", Manager::g_animationManager.getSkeleton("STATE"));
	Manager::g_meshManager.loadDynamicMesh("STATE");


	Manager::g_meshManager.loadStaticMesh("PRESSUREPLATE");
	Manager::g_meshManager.loadStaticMesh("JOCKDOOR");
	future1.get();
}

void PlayState::_loadPlayers()
{
	m_playerManager = new PlayerManager(&this->m_world);
	m_playerManager->CreateLocalPlayer();

	m_levelHandler = new LevelHandler();
	m_levelHandler->Init(m_world, m_playerManager->getLocalPlayer());
	CameraHandler::setActiveCamera(m_playerManager->getLocalPlayer()->getCamera());
}

void PlayState::_loadNetwork()
{

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

	triggerHandler = m_levelHandler->getTriggerHandler();

	/*triggerHandler->LoadTriggerPairMap();

	if (isCoop)
		triggerHandler->RegisterThisInstanceToNetwork();*/

}

void PlayState::_loadSound()
{
	RipSounds::g_windAndDrip = AudioEngine::LoadSoundEffect("../Assets/Audio/AmbientSounds/Cave.ogg", 5.0f, 10000.0f, true);
	RipSounds::g_stepsStone.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep1.ogg"));
	RipSounds::g_stepsStone.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep2.ogg"));
	RipSounds::g_stepsStone.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep3.ogg"));
	RipSounds::g_stepsStone.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep4.ogg"));
	RipSounds::g_stepsStone.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep5.ogg"));
	RipSounds::g_stepsStone.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep6.ogg"));
	RipSounds::g_stepsStone.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep7.ogg"));
	RipSounds::g_stepsStone.push_back(AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/footstep8.ogg"));
	RipSounds::g_leverActivate = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/RazerClickUnlock.ogg");
	RipSounds::g_leverDeactivate = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/RazerClickLock.ogg");
	RipSounds::g_pressurePlateActivate = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/PressureplatePush.ogg");
	RipSounds::g_pressurePlateDeactivate = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/PressureplateRelease.ogg");
	RipSounds::g_torch = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/Torch.ogg", 1.0f, 5000.0f, true);


	FMOD_VECTOR caveSoundAt = { -2.239762f, 6.5f, -1.4f };
	FMOD_VECTOR caveSoundAt2 = { -5.00677f, 6.5f, -10.8154f };

	AudioEngine::PlaySoundEffect(RipSounds::g_windAndDrip, &caveSoundAt, AudioEngine::Other);
	AudioEngine::PlaySoundEffect(RipSounds::g_windAndDrip, &caveSoundAt2, AudioEngine::Other);

	FMOD_VECTOR reverbAt = { -5.94999f, 7.0f, 3.88291f };

	AudioEngine::CreateReverb(reverbAt, 30.0f, 50.0f);
}
