#include "RipTagPCH.h"
#include "PlayState.h"
#include <DirectXCollision.h>
#include "Helper/RandomRoomPicker.h"
#include "Source/CheetConsole/CheetParser.h"


std::vector<std::string> RipSounds::g_sneakStep;
std::vector<std::string> RipSounds::g_hardStep;
std::vector<std::string> RipSounds::g_armorStepsStone;

std::string				 RipSounds::g_leverActivate;
std::string				 RipSounds::g_leverDeactivate;
std::string				 RipSounds::g_pressurePlateActivate;
std::string				 RipSounds::g_pressurePlateDeactivate;
std::string				 RipSounds::g_torch;
std::string				 RipSounds::g_windAndDrip;
std::string				 RipSounds::g_phase;
std::string				 RipSounds::g_grunt;
std::string				 RipSounds::g_playAmbientSound;
std::string				 RipSounds::g_metalDoorOpening;
std::string				 RipSounds::g_metalDoorClosening;
std::string				 RipSounds::g_metalDoorClosed;
std::string				 RipSounds::g_smokeBomb;

b3World * RipExtern::g_world = nullptr;
ContactListener * RipExtern::g_contactListener;
RayCastListener * RipExtern::g_rayListener;
ParticleSystem  * RipExtern::g_particleSystem;

bool RipExtern::g_kill = false;
bool PlayState::m_youlost = false;

PlayState::PlayState(RenderingManager * rm, void * coopData, const unsigned short & roomIndex) : State(rm)
{	
	m_roomIndex = roomIndex;
	if (coopData)
	{
		isCoop = true;
		pCoopData = (CoopData*)coopData;
	}
}

PlayState::~PlayState()
{
	if (!m_destoryPhysicsThread)
	{
		m_destoryPhysicsThread = true;
		m_physicsCondition.notify_all();

		if (m_physicsThread.joinable())
		{
			m_physicsThread.join();
		}
	}

	m_levelHandler->Release();
	delete m_levelHandler;

	m_playerManager->getLocalPlayer()->Release(m_world);
	delete m_playerManager;

	if(m_pPauseMenu != nullptr)
	delete m_pPauseMenu; 

	//delete triggerHandler;
	
	delete RipExtern::g_contactListener;
	RipExtern::g_contactListener = nullptr;
	delete RipExtern::g_rayListener;
	RipExtern::g_rayListener = nullptr;
	RipExtern::g_world = nullptr;
	delete RipExtern::g_particleSystem;
	RipExtern::g_particleSystem = nullptr;
	//delete m_world; //FAK U BYTE // WHY U NOE FREE
}

void PlayState::Update(double deltaTime)
{
	int counter = 0;
	if (runGame)
	{
		InputMapping::Call();

		m_firstRun = false;

		//m_physRunning
		m_physThreadRun.lock();
		m_physThreadRun.unlock();
		/*while (m_physRunning)
		{
			int i = 0;
		}*/

		if (RipExtern::g_kill == true)
		{
			//m_destoryPhysicsThread = true;
			//m_physicsCondition.notify_all();


			//if (m_physicsThread.joinable())
			//{
			//	m_physicsThread.join();
			//}
			////this->pushNewState();
			//m_loadingScreen.draw();
			//RipExtern::g_kill = false;
			//m_removeHud = true;
			//this->resetState(new PlayState(this->p_renderingManager, pCoopData, m_levelHandler->getNextRoom()));
			//return;
		}
		/*
		 * Fake Spin Lock
		 */
		
		//Handle all packets
		RipExtern::g_kill = false;



	
		Network::Multiplayer::HandlePackets();
		m_levelHandler->Update(deltaTime, this->m_playerManager->getLocalPlayer()->getCamera());
	
		RipExtern::g_particleSystem->ParticleSystem::Update(deltaTime, this->m_playerManager->getLocalPlayer()->getCamera());

		m_playerManager->Update(deltaTime);

		m_playerManager->PhysicsUpdate();
		_audioAgainstGuards(deltaTime); 
	
		// Hide mouse
		if (InputHandler::getShowCursor() != FALSE && !m_gamePaused)
			InputHandler::setShowCursor(FALSE);	   
		
		// Select gamepad
		if (GamePadHandler::IsSelectPressed())
		{
			Input::SetActivateGamepad(Input::isUsingGamepad());
		}

		// On exit
		if (m_mainMenuPressed)
		{
			m_destoryPhysicsThread = true;
			m_physicsCondition.notify_all();


			if (m_physicsThread.joinable())
			{
				m_physicsThread.join();
			}
			BackToMenu();
		}


		_checkPauseState();

		if (m_gamePaused && !m_mainMenuPressed)
		{
			_runPause(deltaTime);
		}

		// On win or lost
		if (m_youlost || m_playerManager->isGameWon())
		{
			runGame = false;

			RipExtern::g_particleSystem->clearEmitters();


			if (m_youlost)
			{
				if (isCoop)
					_sendOnGameOver();

				m_destoryPhysicsThread = true;
				m_physicsCondition.notify_all();


				if (m_physicsThread.joinable())
				{
					m_physicsThread.join();
				}

				pushNewState(new TransitionState(p_renderingManager, Transition::Lose, "You got caught by a Guard!\nTry to hide in the shadows next time buddy.", (void*)pCoopData));
			}
			else
			{
				if (m_transitionState)
				{
					m_transitionState->unLoad();
					delete m_transitionState;
					m_transitionState = nullptr;
				}
				m_transitionState = new TransitionState(p_renderingManager, Transition::Win, "Round won!\nPress Ready to play next round.", (void*)pCoopData);
				m_transitionState->Load();

				runGame = false;
			}
		}
	
		// Reset mouse to middle of the window, Must be last in update
		if (!m_playerManager->getLocalPlayer()->unlockMouse && !m_gamePaused)
		{
			Input::ResetMouse();
			InputHandler::setShowCursor(false);
		}
		else
		{
			InputHandler::setShowCursor(true);
		}
		//Start Physics thread
		if (RipExtern::g_kill == false)
		{
			m_deltaTime = deltaTime * !m_physicsFirstRun;
			m_physicsCondition.notify_all();
		}
		if (InputHandler::isKeyPressed('M'))
		{
			if (RipExtern::g_kill == true)
			{
				m_destoryPhysicsThread = true;
				m_physicsCondition.notify_all();

				if (m_physicsThread.joinable())
				{
					m_physicsThread.join();
				}
				//this->pushNewState();
				m_loadingScreen.draw();
				RipExtern::g_kill = false;
				m_removeHud = true;
				this->resetState(new PlayState(this->p_renderingManager, pCoopData, m_levelHandler->getNextRoom()));
				return;
			}
		}
		
	}
	else 
	{
		Network::Multiplayer::HandlePackets();
		_updateOnCoopMode(deltaTime);

		if (m_transitionState)
		{
			m_transitionState->Update(deltaTime);

			if (m_transitionState->BackToMenuBool())
				this->BackToMenu();

			if (m_transitionState->ReadyToLoadNextRoom())
			{
				//load next room when we still have one available, otherwise push a Game Over Victory on the stack
				if (m_levelHandler->HasMoreRooms())
				{
					int player = 1;
					if (pCoopData)
					{
						player = pCoopData->localPlayerCharacter;
					}
				
					m_levelHandler->LoadNextRoom(player);

					//The critical zone
					{
						m_destoryPhysicsThread = true;
						m_physicsCondition.notify_all();

						if (m_physicsThread.joinable())
						{
							m_physicsThread.join();
						}
						//this->pushNewState();
						m_loadingScreen.draw();
						RipExtern::g_kill = false;
						m_removeHud = true;
						this->resetState(new PlayState(this->p_renderingManager, pCoopData, m_levelHandler->getNextRoom()));
						return;
					}
				}
				else
				{
					//If no more rooms are available - we thank the Player for playing our Game
					if (!m_destoryPhysicsThread)
					{
						m_destoryPhysicsThread = true;
						m_physicsCondition.notify_all();

						if (m_physicsThread.joinable())
						{
							m_physicsThread.join();
						}
					}

					this->pushNewState(new TransitionState(this->p_renderingManager, Transition::ThankYou, "Everyone here at Group 3\nwants to give you a big Thanks!\nWe hope you enjoyed our little game!", (void*)pCoopData));
				}
			}
			else if (!m_levelHandler->HasMoreRooms())
			{
				//If no more rooms are available - we thank the Player for playing our Game
				if (!m_destoryPhysicsThread)
				{
					m_destoryPhysicsThread = true;
					m_physicsCondition.notify_all();

					if (m_physicsThread.joinable())
					{
						m_physicsThread.join();
					}
				}

				this->pushNewState(new TransitionState(this->p_renderingManager, Transition::ThankYou, "Everyone here at Group 3\nwants to give you a big Thanks!\nWe hope you enjoyed our little game!", (void*)pCoopData));
			}
		

		}
	}
	m_physicsFirstRun = false;
}

void PlayState::Draw()
{
	if (!m_removeHud && runGame)
	{

		m_levelHandler->Draw();

		RipExtern::g_particleSystem->ParticleSystem::Queue();
		
		_lightCulling();

		m_playerManager->Draw();

		if (m_gamePaused && !m_mainMenuPressed)
		{
			m_pPauseMenu->Draw();
		}

	}
	if (!runGame)
	{
		if (m_transitionState)
			m_transitionState->Draw();
	}
	if (CheetParser::GetDrawCollisionBoxes())
	{
		DrawWorldCollisionboxes();
	}

#ifdef _DEBUG
	//DrawWorldCollisionboxes();
#endif

	//Camera * camera = new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 1, 100);
	//camera->setUP(1, 0, 0);
	//camera->setDirection(0, -1, 0);
	//DirectX::XMFLOAT4A lol = m_playerManager->getLocalPlayer()->getPosition();
	//lol.y += 20;
	//camera->setPosition(lol);
	p_renderingManager->Flush(*CameraHandler::getActiveCamera());
	//p_renderingManager->Flush(*camera);
	//delete camera;
}

void PlayState::setYouLost(const bool& youLost)
{
	m_youlost = youLost;
}

void PlayState::HandlePacket(unsigned char id, unsigned char * data)
{
	switch (id)
	{
	case Network::ID_PLAYER_WON:
		_onGameWonPacket();
		break;
	case Network::ID_PLAYER_LOST:
		_onGameOverPacket();
		break;
	case Network::ID_PLAYER_DISCONNECT:
		_onDisconnectPacket();
		break;
	case DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION:
		_onDisconnectPacket();
		break;
	}
}

void PlayState::_PhyscisThread(double deltaTime)
{
	//static DeltaTime dt;
	if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
	{
		std::cout << "FAILED TO SET PRIORITY LEVEL OF THREAD" << std::endl;
	}

	static int counter = 0;
	//dt.Init();
	while (m_destoryPhysicsThread == false)
	{
		
		std::unique_lock<std::mutex> lock(m_physicsMutex);
		m_physicsCondition.wait(lock);

		//m_physRunning = true;
		m_physThreadRun.lock();
		if (RipExtern::g_kill == true)
		{
			return;
		}
		
		m_timer += m_deltaTime;
		
		RipExtern::g_contactListener->ClearContactQueue();
		
		while (m_timer >= UPDATE_TIME)
		{
			m_world.Step(m_step);
			m_timer -= UPDATE_TIME;
		}
		RipExtern::g_rayListener->ShotRays();
		m_physThreadRun.unlock();
		//m_physRunning = false;
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
			AI::SoundLocation sl;
			sl.percentage = 0.0f;
			if (length < 20.0f)
			{
				for (auto & c : channels)
				{
					AudioEngine::SoundDesc * sd = nullptr;
					FMOD_RESULT res = c->getUserData((void**)&sd);
					if (res == FMOD_OK && sd != nullptr)
					{
						FMOD_VECTOR soundPos;
						if (c->get3DAttributes(&soundPos, nullptr) == FMOD_OK)
						{
							DirectX::XMVECTOR vSPos = DirectX::XMVectorSet(soundPos.x, soundPos.y, soundPos.z, 1.0f);
							DirectX::XMVECTOR soundDir = DirectX::XMVectorSubtract(vSPos, vEPos);
							float lengthSquared = DirectX::XMVectorGetX(DirectX::XMVector3Dot(soundDir, soundDir));
							float occ = 1.0f;

							if (RipExtern::g_rayListener->hasRayHit(m_rayId))
							{
								RayCastListener::Ray* ray = RipExtern::g_rayListener->ConsumeProcessedRay(m_rayId);
								RayCastListener::RayContact* c;
								for (int i = 0; i < ray->getNrOfContacts(); i++)
								{
									c = ray->GetRayContact(i);
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

							if (!DirectX::XMVectorGetX(DirectX::XMVectorEqual(soundDir, DirectX::XMVectorZero())))
							{
								DirectX::XMFLOAT4A soundDirNormalized;
								DirectX::XMStoreFloat4A(&soundDirNormalized, DirectX::XMVector3Normalize(soundDir));

								if(m_rayId == -100)
									m_rayId = RipExtern::g_rayListener->PrepareRay(e->getBody(), ePos, soundDirNormalized, sqrt(lengthSquared));
							}
							

							float volume = sd->loudness;;
							
							volume *= 100.0f;
							
							volume *= occ;
							float addThis = (volume / (lengthSquared * 3));
							

							//Pro Tip: Not putting break in a case will not stop execution, 
							//it will continue execute until a break is found. Break acts like a GOTO command in switch cases
							switch (sd->emitter)
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
							case AudioEngine::Enemy:
								if (e != sd->owner)
								{
									allSounds += addThis;
								}
								break;
							case AudioEngine::RemotePlayer:
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
			else
			{
				sl.soundPos = { 0,0,0 };
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

void PlayState::_runPause(double deltaTime)
{
	Camera* camera = m_playerManager->getLocalPlayer()->getCamera(); 
	m_pPauseMenu->Update(deltaTime, camera); 
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
	std::cout << "PlayState unLoad" << std::endl;

	Manager::g_textureManager.UnloadTexture("KOMBIN");
	Manager::g_textureManager.UnloadTexture("SPHERE");
	Manager::g_textureManager.UnloadTexture("PIRASRUM");
	Manager::g_textureManager.UnloadTexture("DAB");
	Manager::g_textureManager.UnloadAllTexture();
	Manager::g_meshManager.UnloadAllMeshes();

	for (auto & s : RipSounds::g_sneakStep)
		AudioEngine::UnLoadSoundEffect(s);
	for (auto & s : RipSounds::g_hardStep)
		AudioEngine::UnLoadSoundEffect(s);
	for (auto & s : RipSounds::g_armorStepsStone)
		AudioEngine::UnLoadSoundEffect(s);

	AudioEngine::UnLoadSoundEffect(RipSounds::g_windAndDrip);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_leverActivate);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_leverDeactivate);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_pressurePlateActivate);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_pressurePlateDeactivate);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_torch);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_grunt);
	AudioEngine::UnLoadSoundEffect(RipSounds::g_smokeBomb); 
	AudioEngine::UnloadAmbiendSound(RipSounds::g_playAmbientSound);

	if (m_eventOverlay)
	{
		m_eventOverlay->Release();
		delete m_eventOverlay;
		m_eventOverlay = nullptr;
	}
	Network::Multiplayer::LocalPlayerOnSendMap.clear();
	Network::Multiplayer::RemotePlayerOnReceiveMap.clear();
	Network::Multiplayer::inPlayState = false;


	RipExtern::g_particleSystem->clearEmitters();


	if (m_transitionState)
	{
		m_transitionState->unLoad();
		delete m_transitionState;
		m_transitionState = nullptr;
	}

	m_pPauseMenu->unLoad(); 
}

void PlayState::Load()
{
	m_loadingScreen.Init();
	std::cout << "PlayState Load" << std::endl;
	std::vector<RandomRoomPicker::RoomPicker> rooms;
	//Initially Clear network maps

	//phy.open("physData.txt");
	
	if (isCoop)
	{
		//Reset the all relevant networking maps - this is crucial since Multiplayer is a Singleton
		Network::Multiplayer::LocalPlayerOnSendMap.clear();
		Network::Multiplayer::RemotePlayerOnReceiveMap.clear();

		rooms = RandomRoomPicker::RoomPick(pCoopData->seed);
	}
	else
	{
		rooms = RandomRoomPicker::RoomPick(1);
	}
	
	m_youlost = false;
	Input::ResetMouse();
	CameraHandler::Instance();
	
	_loadSound();
	_loadTextures();
	_loadPhysics();
	_loadMeshes();
	_loadAnimations();
	_loadPlayers(rooms);
	_loadNetwork();
	RipExtern::g_particleSystem = new ParticleSystem();
	m_pPauseMenu->Load(); 

	m_physicsThread = std::thread(&PlayState::_PhyscisThread, this, 0);
}

void PlayState::_checkPauseState()
{
	//Check if escape was pressed
	if (!m_pausePressed)
	{
		if (Input::isUsingGamepad())
			m_pausePressed = GamePadHandler::IsStartReleased();
		if (InputHandler::wasKeyPressed(InputHandler::Esc))
			m_pausePressed = true;
	}

	if (m_pausePressed && !m_pauseWasPressed && m_currentState == 0)
	{
		m_gamePaused = true; 
		m_currentState = 1; 
		m_playerManager->getLocalPlayer()->LockPlayerInput();
		m_playerManager->getLocalPlayer()->setLiniearVelocity(0, m_playerManager->getLocalPlayer()->getLiniearVelocity().y, 0);
		m_playerManager->getLocalPlayer()->getBody()->SetAngularVelocity(b3Vec3(0, 0, 0)); 
		m_pPauseMenu = new PauseMenu(); 
	}

	if (m_pPauseMenu != nullptr)
	{
		if (m_pPauseMenu->getExitPause())
		{
			m_pausePressed = false;
			m_gamePaused = false;
			m_currentState = 0;
			m_playerManager->getLocalPlayer()->UnlockPlayerInput();
			delete m_pPauseMenu;
			m_pPauseMenu = nullptr;

		}
		else if (m_pPauseMenu->getMainMenuPressed())
		{
			delete m_pPauseMenu;
			m_pPauseMenu = nullptr;
			m_mainMenuPressed = true; 
		}
	}

	m_pauseWasPressed = m_pausePressed; 
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
	Manager::g_textureManager.loadTextures("FIRE");
	Manager::g_textureManager.loadTextures("GUARD");
	Manager::g_textureManager.loadTextures("ARMS");
	Manager::g_textureManager.loadTextures("PLAYER1");
	Manager::g_textureManager.loadTextures("PLAYER2");



}

void PlayState::_loadPhysics()
{
	RipExtern::g_world = &m_world;
	RipExtern::g_contactListener = new ContactListener();
	RipExtern::g_world->SetContactListener(RipExtern::g_contactListener);
	RipExtern::g_rayListener = new RayCastListener();
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));
	// triggerHandler = new TriggerHandler();
	m_step.dt = UPDATE_TIME;
	m_step.velocityIterations = 8;
	m_step.sleeping = true;
	m_firstRun = false;
}

void PlayState::_loadMeshes()
{
	//auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	Manager::g_meshManager.loadStaticMesh("SPHERE");
	Manager::g_animationManager.loadSkeleton("../Assets/STATEFOLDER/STATE_SKELETON.bin", "STATE");
	Manager::g_animationManager.loadSkeleton("../Assets/GUARDFOLDER/GUARD_SKELETON.bin", "GUARD");
	Manager::g_animationManager.loadClipCollection("STATE", "STATE", "../Assets/STATEFOLDER", Manager::g_animationManager.getSkeleton("STATE"));
	Manager::g_animationManager.loadClipCollection("GUARD", "GUARD", "../Assets/GUARDFOLDER", Manager::g_animationManager.getSkeleton("GUARD"));

	Manager::g_meshManager.loadSkinnedMesh("STATE");
	Manager::g_meshManager.loadSkinnedMesh("PLAYER1");
	Manager::g_meshManager.loadSkinnedMesh("PLAYER2");
	Manager::g_meshManager.loadSkinnedMesh("GUARD");
	Manager::g_meshManager.loadSkinnedMesh("ARMS");


	Manager::g_meshManager.loadStaticMesh("PRESSUREPLATE");
	Manager::g_meshManager.loadStaticMesh("JOCKDOOR");
	//future1.get();
}

void PlayState::_loadAnimations()
{
	//First-person arms
	Manager::g_animationManager.loadSkeleton("../Assets/ARMSFOLDER/ARMS_SKELETON.bin", "ARMS");
	Manager::g_animationManager.loadClipCollection("ARMS", "ARMS", "../Assets/ARMSFOLDER", Manager::g_animationManager.getSkeleton("ARMS"));

	//State (old placeholder character)
	Manager::g_animationManager.loadSkeleton("../Assets/STATEFOLDER/STATE_SKELETON.bin", "STATE");
	Manager::g_animationManager.loadClipCollection("STATE", "STATE", "../Assets/STATEFOLDER", Manager::g_animationManager.getSkeleton("STATE"));

	//Guard
	Manager::g_animationManager.loadSkeleton("../Assets/GUARDFOLDER/GUARD_SKELETON.bin", "GUARD");
	Manager::g_animationManager.loadClipCollection("GUARD", "GUARD", "../Assets/GUARDFOLDER", Manager::g_animationManager.getSkeleton("GUARD"));

	//Player1
	Manager::g_animationManager.loadSkeleton("../Assets/PLAYER1FOLDER/PLAYER1_SKELETON.bin", "PLAYER1");
	Manager::g_animationManager.loadClipCollection("PLAYER1", "PLAYER1", "../Assets/PLAYER1FOLDER", Manager::g_animationManager.getSkeleton("PLAYER1"));
}

void PlayState::_loadPlayers(std::vector<RandomRoomPicker::RoomPicker> rooms)
{
	m_playerManager = new PlayerManager(&this->m_world);
	m_playerManager->CreateLocalPlayer();


	m_levelHandler = new LevelHandler(m_roomIndex);
	m_levelHandler->Init(*RipExtern::g_world, m_playerManager->getLocalPlayer(), rooms.at(m_roomIndex).seedNumber, rooms.at(m_roomIndex).roomNumber);
	CameraHandler::setActiveCamera(m_playerManager->getLocalPlayer()->getCamera());
}

void PlayState::_loadNetwork()
{
	if (isCoop)
	{
		Network::Multiplayer::inPlayState = true;
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
		this->_registerThisInstanceToNetwork();
		//free up memory when we are done with this data
		m_levelHandler->getEnemyHandler()->setRemotePlayer(m_playerManager->getRemotePlayer());
	}
	else
	{
		m_playerManager->isCoop(false);
	}

	//triggerHandler = m_levelHandler->getTriggerHandler();
	
	//Create event overlay quad
	{
		m_eventOverlay = new Quad();
		m_eventOverlay->setPivotPoint(Quad::PivotPoint::center);
		m_eventOverlay->setPosition(0.5f, 0.5f);
		m_eventOverlay->setScale(0.0001f, 0.0001f);
		m_eventOverlay->setUnpressedTexture("gui_pressed_pixel");
		m_eventOverlay->setPressedTexture("gui_pressed_pixel");
		m_eventOverlay->setHoverTexture("gui_pressed_pixel");
		m_eventOverlay->setFont(FontHandler::getFont("consolas32"));
	}


}

void PlayState::_loadSound()
{
	RipSounds::g_windAndDrip = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/Ambient/Cave.ogg", 5.0f, 10000.0f, true);

	for (int i = 1; i <= 6; i++)
	{
		RipSounds::g_sneakStep.push_back(AudioEngine::LoadSoundEffect(
			"../Assets/Audio/SoundEffects/Player/Sneaking/soft_"
			+ std::to_string(i) +
			".ogg"
		));
	}
	for (int i = 1; i <= 6; i++)
	{
		RipSounds::g_hardStep.push_back(AudioEngine::LoadSoundEffect(
			"../Assets/Audio/SoundEffects/Player/Running/hard_"
			+ std::to_string(i) +
			".ogg"
		));
	}
	for (int i = 1; i <= 12; i++)
	{
		RipSounds::g_armorStepsStone.push_back(AudioEngine::LoadSoundEffect(
			"../Assets/Audio/SoundEffects/Armored_Guard/Footsteps/step_"
			+ std::to_string(i) +
			".ogg"
		));
	}


	RipSounds::g_leverActivate = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/Interactables/Lever/RazerClickUnlock.ogg");
	RipSounds::g_leverDeactivate = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/Interactables/Lever/RazerClickLock.ogg");
	RipSounds::g_pressurePlateActivate = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/Interactables/Pressureplate/PressureplatePush.ogg");
	RipSounds::g_pressurePlateDeactivate = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/Interactables/Pressureplate/PressureplateRelease.ogg");
	RipSounds::g_torch = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/Light/Torch.ogg", 1.0f, 5000.0f, true);
	RipSounds::g_grunt = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/Armored_Guard/Alert/TimAllenGrunt.ogg");
	RipSounds::g_smokeBomb = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/smokeBomb.ogg"); 
	RipSounds::g_playAmbientSound = AudioEngine::LoadAmbientSound("../Assets/Audio/AmbientSounds/play_ambient.ogg", true);
	RipSounds::g_metalDoorOpening = AudioEngine::LoadSoundEffect("../Assets/Audio/SoundEffects/Interactables/Small_Door/open.ogg");


	AudioEngine::PlayAmbientSound(RipSounds::g_playAmbientSound)->setVolume(0.2f);
}

void PlayState::_registerThisInstanceToNetwork()
{

	using namespace Network;
	using namespace std::placeholders;

	Multiplayer::addToOnReceiveFuncMap(ID_PLAYER_WON, std::bind(&PlayState::HandlePacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(ID_PLAYER_LOST, std::bind(&PlayState::HandlePacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(ID_PLAYER_DISCONNECT, std::bind(&PlayState::HandlePacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION, std::bind(&PlayState::HandlePacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(ID_SMOKE_DETONATE, std::bind(&PlayState::HandlePacket, this, _1, _2));
}

void PlayState::_sendOnGameOver()
{
	Network::COMMONEVENTPACKET packet(Network::ID_PLAYER_LOST, 0);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::COMMONEVENTPACKET), PacketPriority::LOW_PRIORITY);
}

void PlayState::_sendOnGameWon()
{
	Network::COMMONEVENTPACKET packet(Network::ID_PLAYER_WON, 0);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::COMMONEVENTPACKET), PacketPriority::LOW_PRIORITY);
}

void PlayState::_sendOnDisconnect()
{
	Network::COMMONEVENTPACKET packet(Network::ID_PLAYER_DISCONNECT, 0);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::COMMONEVENTPACKET), PacketPriority::IMMEDIATE_PRIORITY);
}

void PlayState::_onGameOverPacket()
{
	m_destoryPhysicsThread = true;
	m_physicsCondition.notify_all();


	if (m_physicsThread.joinable())
	{
		m_physicsThread.join();
	}
	pushNewState(new TransitionState(p_renderingManager, Transition::Lose, "Your partner got caught by a Guard!\nTime to get a better friend?", (void*)pCoopData, true));
}

void PlayState::_onGameWonPacket()
{
	m_coopState.gameWon = true;
	runGame = false;
}

void PlayState::_onDisconnectPacket()
{
	m_destoryPhysicsThread = true;
	m_physicsCondition.notify_all();


	if (m_physicsThread.joinable())
	{
		m_physicsThread.join();
	}
	pushNewState(new TransitionState(p_renderingManager, Transition::Lose, "Your partner has abandoned you!\nIs he really your friend?", (void*)pCoopData));
}

void PlayState::_updateOnCoopMode(double deltaTime)
{
	static const double MAX_DURATION = 1.0; //unit is in seconds
	static double accumulatedTime = 0.0;

	if (m_coopState.gameOver || m_coopState.gameWon || m_coopState.remoteDisconnected)
	{
		
		accumulatedTime += deltaTime;
		double percentage = accumulatedTime / MAX_DURATION; //range[0,1[
		m_eventOverlay->setScale({ (float)(percentage * 2.0f), (float)(percentage * 2.0f) });

		if (true)
		{
			m_eventOverlay->setScale({2.0f, 2.0f});
			accumulatedTime = 0;

			m_destoryPhysicsThread = true;
			m_physicsCondition.notify_all();


			if (m_physicsThread.joinable())
			{
				m_physicsThread.join();
			}
			if (m_coopState.gameWon)
			{
				if (m_transitionState)
				{
					m_transitionState->unLoad();
					delete m_transitionState;
					m_transitionState = nullptr;
				}
				m_transitionState = new TransitionState(p_renderingManager, Transition::Win, "Round won!\nPress Ready to play next round.", (void*)pCoopData);
				m_transitionState->Load();

				runGame = false;
			}
		
		}
	}
}


