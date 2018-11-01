#include "PlayState.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"
#include "EngineSource/Helper/Timer.h"
#include "ImportLibrary/formatImporter.h"
#include "../RipTagExtern/RipExtern.h"
#include <AudioEngine.h>

b3World * RipExtern::g_world = nullptr;

#define JAAH TRUE
#define NEIN FALSE

PlayState::PlayState(RenderingManager * rm) : State(rm)
{	
	RipExtern::g_world = &m_world;
	CameraHandler::Instance();
	auto future = std::async(std::launch::async, &PlayState::thread, this, "KOMBIN");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));

	Timer::StartTimer();

	Manager::g_meshManager.loadStaticMesh("SPHERE");
	Manager::g_textureManager.loadTextures("SPHERE");

	future.get();
	future1.get();
	player = new Player();
	Timer::StopTimer();
	std::cout << "s " << Timer::GetDurationInSeconds() << std::endl;

	

	CameraHandler::setActiveCamera(player->getCamera());


	player->Init(m_world, e_dynamicBody,0.5f,0.5f,0.5f);
	player->setEntityType(EntityType::PlayerType);
	//player->setPosition(0, 5, 0, 0);
	player->setColor(10, 10, 0, 1);

	player->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	player->setScale(1.0f, 1.0f, 1.0f);
	player->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	player->setTextureTileMult(2, 2);	

	//enemy->setDir(1, 0, 0);
	//enemy->getCamera()->setFarPlane(5);

	model = new Drawable();
	model->setEntityType(EntityType::PlayerType);
	model->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	model->setScale(0.5, 0.5, 0.5);
	model->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	model->setTextureTileMult(50, 50);


	m_levelHandler.setPlayer(player);
	m_levelHandler.Init(m_world);

	std::string name = AudioEngine::LoadSoundEffect("../Assets/Audio/AmbientSounds/Cave.ogg", true);
	FMOD_VECTOR at = { -29.1406, -2.82f, -15.4373f };
	FMOD_VECTOR at2 = { -11.5999, -2.82f, -0.4889f };
	TEEEMPCHANNEL = AudioEngine::PlaySoundEffect(name, &at);
	AudioEngine::PlaySoundEffect(name, &at2);


	AudioEngine::CreateReverb(at, 10, 25.0f);
	//bool lol = AudioEngine::TEMP_IS_THIS_POINT_INSIDE_MESH(at);

	Input::ResetMouse();
}

PlayState::~PlayState()
{
	m_levelHandler.Release();
	player->Release(m_world);
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
	if (GetAsyncKeyState('F'))
	{
		FMOD_VECTOR at, vel, other;
		TEEEMPCHANNEL->get3DAttributes(&at, &vel, &other);
		player->setPosition(at.x, at.y, at.z);
	}

	player->Update(deltaTime);

	m_objectHandler.Update();
	m_levelHandler.Update(deltaTime);
	
	m_step.dt = deltaTime;
	m_step.velocityIterations = 1;
	m_step.sleeping = false;
	m_firstRun = false;


	static float timer = 0;
	timer += deltaTime;

	if (timer > 0.5f)
	{
		timer = 0;
		system("CLS");
		const AudioEngine::Listener & l = player->getFMODListener();
		FMOD_VECTOR point = l.pos;
		DirectX::XMFLOAT4A d = player->getCamera()->getDirection();
		point.x += d.x * 2.0f;
		point.y += d.y * 2.0f;
		point.z += d.z * 2.0f;
		int lol = -1;// AudioEngine::TEMP_IS_THIS_POINT_INSIDE_MESH(l.pos);
		int lol2 = -1;// AudioEngine::TEMP_IS_THIS_POINT_INSIDE_MESH(point);
		const DirectX::XMFLOAT4A & xmPos = player->getCamera()->getPosition();
		FMOD_VECTOR at, vel, other;
		TEEEMPCHANNEL->get3DAttributes(&at, &vel, &other);
		std::cout << "Sound Pos: " <<		at.x << ", " << at.y << ", " << at.z << std::endl;
		std::cout << "Sound Vel: " <<		vel.x << ", " << vel.y << ", " << vel.z << std::endl;
		std::cout << "Sound Other: " <<		other.x << ", " << other.y << ", " << other.z << std::endl;
		std::cout << "Listener Pos: " <<	l.pos.x << ", " << l.pos.y << ", " << l.pos.z << std::endl;
		std::cout << "Listener Vel: " <<	l.vel.x << ", " << l.vel.y << ", " << l.vel.z << std::endl;
		std::cout << "Listener Up: " <<		l.up.x << ", " << l.up.y << ", " << l.up.z << std::endl;
		std::cout << "Listener For: " <<	l.forward.x << ", " << l.forward.y << ", " << l.forward.z << std::endl;
		std::cout << "Player pos: " << xmPos.x << ", " << xmPos.y << ", " << xmPos.z << std::endl;
		std::cout << "Player inside mesh: " << lol << std::endl;
		std::cout << "Player point inside mesh: " << lol2 << std::endl;
		DirectX::XMVECTOR vAt = DirectX::XMVectorSet(at.x, at.y, at.z, 1.0f);
		DirectX::XMVECTOR vPpos = DirectX::XMLoadFloat4A(&xmPos);
		float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(vPpos, vAt)));
		std::cout << "Length: " << length << std::endl;

	}


	AudioEngine::UpdateListenerAttributes(player->getFMODListener());
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
		InputHandler::setShowCursor(NEIN);
	}
	else
	{
		InputHandler::setShowCursor(JAAH);
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
