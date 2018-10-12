#include "PlayState.h"
#include "../../../../InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"
#include "Source/Helper/Timer.h"

#include "../New_Library/formatImporter.h"


PlayState::PlayState(RenderingManager * rm) : State(rm)
{	
	//lua.init("playerState.lua")
	CameraHandler::Instance();
	
	player = new Player();
	enemy = new Enemy();
	CameraHandler::setActiveCamera(player->getCamera());
	
	//m_world = new b3World();
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));

	MyLibrary::Loadera l;
	
	MyLibrary::CollisionBoxes bb = l.readMeshCollisionBoxes("../Assets/BBoX.bin");



	//bodyDef = new b3BodyDef();
	//bodyDef->type = e_dynamicBody;

	//// Position the body 10 meters high from the world origin.
	//bodyDef->position.Set(0.0f, 100.0f, 0.0f);
	//
	//m_body = m_world.CreateBody(*bodyDef);
	//bodyBox = new b3Hull();

	//bodyBox->SetAsBox(b3Vec3(1, 1, 1));

	//poly = new b3Polyhedron();
	//poly->SetHull(bodyBox);
	//
	//
	//bodyBoxDef = new b3ShapeDef();
	//bodyBoxDef->shape = poly;
	//bodyBoxDef->density = 1.0f;
	//bodyBoxDef->restitution = 0;
	//
	//using namespace std::chrono_literals;
	////std::cout << "Hello waiter" << std::endl; // flush is intentional
	////auto start = std::chrono::high_resolution_clock::now();
	//


	//std::this_thread::sleep_for(2s);
	//m_body->SetTransform(b3Vec3(0, 10, 0), b3Vec3(0, 0, 0), 0);
	//m_body->SetGravityScale(-9.82f);
	//m_shape->SetTransform(b3Vec3(0, 10, 0), b3Vec3(0, 0, 0), 0);
	Timer::StartTimer();
	//pool->submit(&thread,"KOMBIN");
	auto future = std::async(std::launch::async, &PlayState::thread, this, "KOMBIN");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	auto future1 = std::async(std::launch::async, &PlayState::thread, this, "SPHERE");// Manager::g_meshManager.loadStaticMesh("KOMBIN");
	//Manager::g_meshManager.loadStaticMesh("SPHERE");
	Manager::g_meshManager.loadStaticMesh("KUB");

	//Manager::g_textureManager.loadTextures("PIRASRUM");

	Manager::g_textureManager.loadTextures("KOMBIN");
	Manager::g_textureManager.loadTextures("SPHERE");
	
	//temp = new Model();
	////temp->setEntityType();
	//temp->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	//temp->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	//temp->setPosition(0, 10, 0);
	future.get();
	future1.get();
	Timer::StopTimer();
	std::cout << "s " << Timer::GetDurationInSeconds() << std::endl;
	actor = new BaseActor();
	actor->Init(m_world, e_staticBody, 0.01f, 0.01f, 0.01f);
	actor->setModel(Manager::g_meshManager.getStaticMesh("KOMBIN"));
	actor->setTexture(Manager::g_textureManager.getTexture("KOMBIN"));


	CollisionBoxes = new BaseActor();
	std::vector<CollisionObject> boxes;
	for (int i = 0; i < bb.nrOfBoxes; i++)
	{
		
		boxes.push_back(CollisionObject(DirectX::XMFLOAT3(bb.boxes[i].translation[0], bb.boxes[i].translation[1], bb.boxes[i].translation[2]), DirectX::XMFLOAT3(bb.boxes[i].scale[0], bb.boxes[i].scale[1], bb.boxes[i].scale[2])));

	}
	CollisionBoxes->Init(m_world, boxes);


	//actor->setPosition(0, 10, 0);
	actor->setScale(1.0f,1.0f,1.0f);
	actor->setPosition(0, 0, 0);
	actor->setTextureTileMult(10, 10);
	player->Init(m_world, e_dynamicBody,0.5f,0.5f,0.5f);
	player->setEntityType(EntityType::PlayerType);
	player->setPosition(0, 5, 0,0);
	//player->setEntityType(EntityType::PlayerType);
	player->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	player->setScale(1.0f, 1.0f, 1.0f);
	player->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	player->setTextureTileMult(2, 2);

	light1.Init(DirectX::XMFLOAT4A(7, 4, 4, 1), DirectX::XMFLOAT4A(1, 1, 1, 1), 1);
	light1.CreateShadowDirection(PointLight::XYZ_ALL);
	light1.setDropOff(0);
	light1.setColor(0.8f, 0.6, 0.4f);
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

	model = new Model();
	model->setEntityType(EntityType::PlayerType);
	model->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	model->setScale(0.5, 0.5, 0.5);
	//player->setScale(0.003f, 0.003f, 0.003f);
	model->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	model->setTextureTileMult(50, 50);

	testCube = new BaseActor();
	testCube->Init(m_world, e_dynamicBody, 1.0f, 1.0f, 1.0f);
	testCube->setModel(Manager::g_meshManager.getStaticMesh("KUB"));
	testCube->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	testCube->setPosition(5, 5.0f, 0);
	

	delete [] bb.boxes;
}

PlayState::~PlayState()
{
	delete enemy;
	player->Release(m_world);
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

	//current.x = fmin;
	
	auto v1 = DirectX::XMLoadFloat2(&current);
	auto v2 = DirectX::XMLoadFloat2(&target);
	DirectX::XMVECTOR vec;

	vec = DirectX::XMVectorLerp(v1, v2, deltaTime * 5);


	current.x = DirectX::XMVectorGetX(vec);

	float temp = 5 + sin(current.x) * 1.5;

	//temp = min(temp, 0.8);
	//temp = max(temp, 0.65f);
	
	light1.setDropOff(.5f);
	light1.setIntensity(temp);

	//std::cout << "Current: " << current.x << " Target: " << target.x << "	Result: " << temp << std::endl;

/*
	float flick = (float)(rand() % 100) / 50.0f;
	light1.setDropOff(flick);
	flick = (float)(rand() % 100) / 50.0f;
	light2.setDropOff(flick);
	flick = (float)(rand() % 100) / 30.0f;
	light1.setIntensity(flick);
	flick = (float)(rand() % 100) / 30.0f;
	light2.setIntensity(flick);*/

#if _DEBUG
	ImGui::Begin("Player Setting");                          // Create a window called "Hello, world!" and append into it.
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
	ImGui::Begin("Light");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.f);
	ImGui::End();

	ImGui::Begin("Player Visibility");                          // Create a window called "Hello, world!" and append into it.
	ImGui::Text("Guard1: playerVis: %d", e1Vis[0]);
	ImGui::Text("Guard2: playerVis: %d", e2Vis[0]);
	ImGui::End();

#endif


	light2.setIntensity(intensity);
	//light1.setPosition(x, y, z, 1);
	//model->setPosition(x, y, z);
	//testCube->setPositionRot(x, y, z,xD,yD,zD);
	//gTemp.setDir(0, 0, -1);
	//jumper->setPosition(ax, ay, az);
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

	
	
	
	// Getho Culling
	//DirectX::XMFLOAT4A plPos = player->getPosition();
	//// For each enemy
	//DirectX::XMFLOAT4A dir = enemy->getCamera()->getDirection();
	//DirectX::XMFLOAT4A ePos = enemy->getPosition();
	//DirectX::XMFLOAT4A eToP(plPos.x - ePos.x, plPos.y - ePos.y, plPos.z - ePos.z, 0.0f);
	//float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(DirectX::XMLoadFloat4A(&dir)), DirectX::XMVector3Normalize(DirectX::XMLoadFloat4A(&eToP))));
	//float l = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat4A(&eToP)));
	//
	//if (d > enemy->getCamera()->getFOV() / 2.8f && l <= (enemy->getCamera()->getFarPlane() / d) + 2)
	enemy->CullingForVisability(*player->getTransform());
	enemy->QueueForVisibility();

	/*dir = gTemp.getCamera()->getDirection();
	ePos = gTemp.getPosition();
	eToP = DirectX::XMFLOAT4A(plPos.x - ePos.x, plPos.y - ePos.y, plPos.z - ePos.z, 0.0f);
	d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(DirectX::XMLoadFloat4A(&dir)), DirectX::XMVector3Normalize(DirectX::XMLoadFloat4A(&eToP))));
	if (d > gTemp.getCamera()->getFOV() / 2.8f && l <= (gTemp.getCamera()->getFarPlane() / d) + 2)*/
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
