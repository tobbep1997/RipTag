#include "PlayState.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"
#include "EngineSource/Helper/Timer.h"
#include "ImportLibrary/formatImporter.h"


PlayState::PlayState(RenderingManager * rm) : State(rm)
{	
	CameraHandler::Instance();
	auto future = std::async(std::launch::async, &PlayState::thread, this, "KOMBIN");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));

	Timer::StartTimer();

	Manager::g_meshManager.loadStaticMesh("SPHERE");
	Manager::g_textureManager.loadTextures("SPHERE");

	future.get();
	future1.get();
	Timer::StopTimer();
	std::cout << "s " << Timer::GetDurationInSeconds() << std::endl;

	
	player = new Player();
	//enemy = new Enemy();

	CameraHandler::setActiveCamera(player->getCamera());


	player->Init(m_world, e_dynamicBody,0.5f,0.5f,0.5f);
	player->setEntityType(EntityType::PlayerType);
	//player->setPosition(0, 5, 0, 0);
	player->setColor(10, 10, 0, 1);

	player->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	player->setScale(1.0f, 1.0f, 1.0f);
	player->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	player->setTextureTileMult(2, 2);

	player->InitTeleport(m_world);
	

	gTemp.setPosition(9, 0.4f, -4.5f);
	gTemp.setDir(0, 0, 1);
	gTemp.getCamera()->setFarPlane(5);
	//enemy->setPosition(0.0f, 0.4f, -5.5f);
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
	/*static double time = 0.0f;
	static DirectX::XMFLOAT2 current(0.0, 0.0);
	static DirectX::XMFLOAT2 target(1.0, 1.0);
	static double timer = 0.0f;
	timer += deltaTime;
	static float ran = 5.5f;

	if (abs(current.x - target.x) < 0.1)
	{
		timer = 0.0;

		ran = (float)(rand() % 100) / 100.0f;
		
		target.x = ran;
		
	}
	
	auto v1 = DirectX::XMLoadFloat2(&current);
	auto v2 = DirectX::XMLoadFloat2(&target);
	DirectX::XMVECTOR vec;

	vec = DirectX::XMVectorLerp(v1, v2, deltaTime * 5);


	current.x = DirectX::XMVectorGetX(vec);

	float temp = 5 + sin(current.x) * 1.5;*/


	
	//light1.setDropOff(.5f);
	//light1.setIntensity(light1.TourchEffect(deltaTime));



#if _DEBUG
	ImGui::Begin("Player Setting");                          
	ImGui::SliderFloat("PositionX", &x, -20.0f, 20.f);
	ImGui::SliderFloat("PositionY", &y, -20.0f, 20.f);
	ImGui::SliderFloat("PositionZ", &z, -20.0f, 20.f);

	ImGui::SliderFloat("DirX", &xD, -50.0f, 50.f);
	ImGui::SliderFloat("DirY", &yD, -50.0f, 50.f);
	ImGui::SliderFloat("DirZ", &zD, -50.0f, 50.f);
	ImGui::End();
#endif

	const int * e1Vis = enemy->getPlayerVisibility();
	const int * e2Vis = gTemp.getPlayerVisibility();

#if _DEBUG
	ImGui::Begin("Light");                         
	ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.f);
	ImGui::End();

	ImGui::Begin("Player Visibility");                          // Create a window called "Hello, world!" and append into it.
	/*ImGui::Text("Guard1: playerVis: %d", e1Vis[0]);
	ImGui::Text("Guard2: playerVis: %d", e2Vis[0]);*/
	ImGui::End();

#endif

	if (!unlockMouse)
	{

	
	}

	


	//light2.setIntensity(intensity);

	if (GamePadHandler::IsLeftDpadPressed())
	{
		Input::ForceDeactivateGamepad();
	}
	if (GamePadHandler::IsRightDpadPressed())
	{
		Input::ForceActivateGamepad();
	}

	if (InputHandler::isKeyPressed('J'))
	{
		player->LockPlayerInput();
		CameraHandler::setActiveCamera(gTemp.getCamera());
		gTemp.UnlockEnemyInput();
	}
	else if (InputHandler::isKeyPressed('K'))
	{
		gTemp.LockEnemyInput();
		player->UnlockPlayerInput();
		CameraHandler::setActiveCamera(player->getCamera());
	}

	if (InputHandler::isKeyPressed('H'))
	{
		CameraHandler::setActiveCamera(enemy->getCamera());
		//player->CreateBox(1.0f, 1.0f, 1.0f);
	}

	player->SetCurrentVisability(e2Vis[0] / 5000.0f);
	player->Update(deltaTime);
	//enemy->Update(deltaTime);
	//actor->Update(deltaTime);
	gTemp.Update(deltaTime);

	

	m_objectHandler.Update();
	m_levelHandler.Update(deltaTime);


	
	m_step.dt = deltaTime * 2;
	m_step.velocityIterations = 1;
	m_step.sleeping = false;
	m_firstRun = false;
	
	

	


	gTemp.CullingForVisability(*player->getTransform());
	gTemp.QueueForVisibility();

	//----------------------------------
	m_world.Step(m_step);
	player->PhysicsUpdate(deltaTime);
}

void PlayState::Draw()
{
	//light1.QueueLight();
	//light2.QueueLight();

	//enemy->Draw();

	m_objectHandler.Draw();
	m_levelHandler.Draw();
	
	//wall1->Draw();

	player->Draw();
	//player->QueueVisabilityDraw();

	model->Draw();
	
	//model->QueueVisabilityDraw();
	//m_world.Draw()

	p_renderingManager->Flush(*CameraHandler::getActiveCamera());
	
}

void PlayState::thread(std::string s)
{
	Manager::g_meshManager.loadStaticMesh(s);
}
