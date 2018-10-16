#include "PlayState.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"
#include "EngineSource/Helper/Timer.h"
#include "ImportLibrary/formatImporter.h"


PlayState::PlayState(RenderingManager * rm) : State(rm)
{	
	CameraHandler::Instance();
	
	player = new Player();
	enemy = new Enemy();
	CameraHandler::setActiveCamera(player->getCamera());	

	m_world.SetGravityDirection(b3Vec3(0, -1, 0));

	Timer::StartTimer();

	auto future = std::async(std::launch::async, &PlayState::thread, this, "KOMBIN");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");

	Manager::g_meshManager.loadStaticMesh("KUB");

	Manager::g_textureManager.loadTextures("KOMBIN");
	Manager::g_textureManager.loadTextures("SPHERE");

	future.get();
	future1.get();
	Timer::StopTimer();
	std::cout << "s " << Timer::GetDurationInSeconds() << std::endl;
	actor = new BaseActor();
	actor->Init(m_world, e_staticBody, 0.01f, 0.01f, 0.01f);
	actor->setModel(Manager::g_meshManager.getStaticMesh("KOMBIN"));
	actor->setTexture(Manager::g_textureManager.getTexture("KOMBIN"));

	CollisionBoxes = new BaseActor();
	CollisionBoxes->Init(m_world, Manager::g_meshManager.getCollisionBoxes("KOMBIN"));



	actor->setScale(1.0f,1.0f,1.0f);
	actor->setPosition(0, 0, 0);
	actor->setTextureTileMult(10, 10);
	player->Init(m_world, e_dynamicBody,0.5f,0.5f,0.5f);
	player->setEntityType(EntityType::PlayerType);
	player->setPosition(0, 5, 0,0);

	player->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	player->setScale(1.0f, 1.0f, 1.0f);
	player->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	player->setTextureTileMult(2, 2);

	player->InitTeleport(m_world);
	light1.Init(DirectX::XMFLOAT4A(7, 4, 4, 1), DirectX::XMFLOAT4A(1, 1, 1, 1), 1);
	light1.CreateShadowDirection(PointLight::XYZ_ALL);
	light1.setDropOff(0);
	light1.setColor(0.8f, 0.6f, 0.4f);
	light1.setDropOff(1);

	light2.Init(DirectX::XMFLOAT4A(7, 3, -6, 1), DirectX::XMFLOAT4A(1, 1, 1, 1), 1);
	light2.CreateShadowDirection(PointLight::XYZ_ALL);
	light2.setDropOff(0);
	
	gTemp.setPosition(9, 0.4f, -4.5f);
	gTemp.setDir(0, 0, 1);
	gTemp.getCamera()->setFarPlane(5);
	enemy->setPosition(0.0f, 0.4f, -5.5f);
	enemy->setDir(1, 0, 0);
	enemy->getCamera()->setFarPlane(5);

	model = new Drawable();
	model->setEntityType(EntityType::PlayerType);
	model->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	model->setScale(0.5, 0.5, 0.5);
	model->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	model->setTextureTileMult(50, 50);

	testCube = new BaseActor();
	testCube->Init(m_world, e_dynamicBody, 1.0f, 1.0f, 1.0f);
	testCube->setModel(Manager::g_meshManager.getStaticMesh("KUB"));
	testCube->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	testCube->setPosition(5, 5.0f, 0);
	

}

PlayState::~PlayState()
{
	delete enemy;
	player->Release(m_world);
	player->ReleaseTeleport(m_world);
	delete player;

	actor->Release(m_world);
	delete actor;



	delete model;

	testCube->Release(m_world);
	delete testCube;

	CollisionBoxes->Release(m_world);
	delete CollisionBoxes;

}

void PlayState::Update(double deltaTime)
{
	static double time = 0.0f;
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

	float temp = 5 + sin(current.x) * 1.5;


	
	light1.setDropOff(.5f);
	light1.setIntensity(temp);



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
	ImGui::Text("Guard1: playerVis: %d", e1Vis[0]);
	ImGui::Text("Guard2: playerVis: %d", e2Vis[0]);
	ImGui::End();

#endif


	light2.setIntensity(intensity);

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
		CameraHandler::setActiveCamera(gTemp.getCamera());
	}
	else if (InputHandler::isKeyPressed('K'))
	{
		CameraHandler::setActiveCamera(player->getCamera());
	}

	if (InputHandler::isKeyPressed('H'))
	{
		CameraHandler::setActiveCamera(enemy->getCamera());
		//player->CreateBox(1.0f, 1.0f, 1.0f);
	}


	player->Update(deltaTime);
	enemy->Update(deltaTime);
	actor->Update(deltaTime);
	gTemp.Update(deltaTime);

	testCube->Update(deltaTime);

	m_objectHandler.Update();
	m_levelHandler.Update();


	
	m_step.dt = deltaTime * 2;
	m_step.velocityIterations = 1;
	m_step.sleeping = false;
	m_firstRun = false;
	
	

	enemy->CullingForVisability(*player->getTransform());
	enemy->QueueForVisibility();


	gTemp.CullingForVisability(*player->getTransform());
	gTemp.QueueForVisibility();

	//----------------------------------
	m_world.Step(m_step);
	player->PhysicsUpdate(deltaTime);
}

void PlayState::Draw()
{
	light1.QueueLight();
	//light2.QueueLight();

	gTemp.Draw();
	enemy->Draw();

	m_objectHandler.Draw();
	m_levelHandler.Draw();
	actor->Draw();
	//wall1->Draw();

	player->Draw();
	//player->QueueVisabilityDraw();

	model->Draw();
	testCube->Draw();
	//model->QueueVisabilityDraw();
	//m_world.Draw()

	p_renderingManager->Flush(*CameraHandler::getActiveCamera());
	
}

void PlayState::thread(std::string s)
{
	Manager::g_meshManager.loadStaticMesh(s);
}
