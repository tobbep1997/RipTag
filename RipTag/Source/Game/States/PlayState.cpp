#include "PlayState.h"
#include "../../../../InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"

PlayState::PlayState(RenderingManager * rm) : State(rm)
{	

	CameraHandler::Instance();
	
	player = new Player();
	enemy = new Enemy();
	CameraHandler::setActiveCamera(player->getCamera());
	
	//m_world = new b3World();
	m_world.SetGravityDirection(b3Vec3(0, -1, 0));

	

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
	//m_shape = m_body->CreateShape(*bodyBoxDef);

	bodyDef2 = new b3BodyDef();
	bodyDef2->type = e_staticBody;

	// Position the body 10 meters high from the world origin.
	bodyDef2->position.Set(0.0f, 0.0f, 0.0f);

	m_floor = m_world.CreateBody(*bodyDef2);
	bodyBox2 = new b3Hull();

	bodyBox2->SetAsBox(b3Vec3(20, 1, 20));

	poly2 = new b3Polyhedron();
	poly2->SetHull(bodyBox2);


	bodyBoxDef2 = new b3ShapeDef();
	bodyBoxDef2->shape = poly2;
	bodyBoxDef2->density = 1.0f;
	bodyBoxDef2->restitution = 0.1;

	m_shape2 = m_floor->CreateShape(*bodyBoxDef2);

	//std::this_thread::sleep_for(2s);
	//m_body->SetTransform(b3Vec3(0, 10, 0), b3Vec3(0, 0, 0), 0);
	//m_body->SetGravityScale(-9.82f);
	//m_shape->SetTransform(b3Vec3(0, 10, 0), b3Vec3(0, 0, 0), 0);
	Manager::g_meshManager.loadStaticMesh("KOMBIN");
	Manager::g_meshManager.loadStaticMesh("KUB");
	Manager::g_meshManager.loadStaticMesh("SPHERE");
	Manager::g_textureManager.loadTextures("KOMBIN");
	Manager::g_textureManager.loadTextures("SPHERE");
	
	//temp = new Model();
	////temp->setEntityType();
	//temp->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	//temp->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	//temp->setPosition(0, 10, 0);

	actor = new BaseActor();
	actor->Init(m_world, e_staticBody, 0.01f, 0.01f, 0.01f);
	actor->setModel(Manager::g_meshManager.getStaticMesh("KOMBIN"));
	actor->setTexture(Manager::g_textureManager.getTexture("KOMBIN"));
	//actor->setPosition(0, 10, 0);
	actor->setScale(1.0f,1.0f,1.0f);
	player->Init(m_world, e_dynamicBody,1,1,1);
	player->setPosition(0, 5, 0,0);
	//player->setEntityType(EntityType::PlayerType);
	player->setModel(Manager::g_meshManager.getStaticMesh("KUB"));
	player->setScale(1.0f, 1.0f, 1.0f);
	player->setTexture(Manager::g_textureManager.getTexture("SPHERE"));

	wall1 = new BaseActor();
	wall1->Init(m_world, e_staticBody, 8.0f, 2.0f, 0.1f);
	wall1->setModel(Manager::g_meshManager.getStaticMesh("KUB"));
	wall1->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	wall1->setPosition(-1.5, 2.1, -2.1);

	light1.Init(DirectX::XMFLOAT4A(7, 4, 4, 1), DirectX::XMFLOAT4A(1, 1, 1, 1), 1);
	light1.CreateShadowDirection(PointLight::XYZ_ALL);
	light1.setDropOff(0);

	light2.Init(DirectX::XMFLOAT4A(7, 3, -6, 1), DirectX::XMFLOAT4A(1, 1, 1, 1), 1);
	light2.CreateShadowDirection(PointLight::XYZ_ALL);
	light2.setDropOff(0);
	
	gTemp.setPos(9, 0.4f, -4.5f);
	gTemp.setDir(0, 0, 1);
	gTemp.getCamera().setFarPlane(5);

	model = new Model();
	model->setEntityType(EntityType::PlayerType);
	model->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	model->setScale(0.5, 0.5, 0.5);
	//player->setScale(0.003f, 0.003f, 0.003f);
	model->setTexture(Manager::g_textureManager.getTexture("SPHERE"));

	
	Manager::g_meshManager.loadDynamicMesh("JUMP");
	skeleton = Animation::LoadAndCreateSkeleton("../Assets/JUMPFOLDER/JUMP_SKELETON.bin");
	animation = Animation::LoadAndCreateAnimation("../Assets/JUMPFOLDER/JUMP_ANIMATION.bin", skeleton);
	Manager::g_meshManager.getDynamicMesh("JUMP")->m_anim = new Animation::AnimatedModel();
	Manager::g_meshManager.getDynamicMesh("JUMP")->getAnimatedModel()->SetSkeleton(skeleton);
	Manager::g_meshManager.getDynamicMesh("JUMP")->getAnimatedModel()->SetPlayingClip(animation);
	Manager::g_meshManager.getDynamicMesh("JUMP")->getAnimatedModel()->Play();

	jumper = new Model();
	jumper->setModel(Manager::g_meshManager.getDynamicMesh("JUMP"));
	jumper->setScale(0.05f, 0.05f, 0.05f);
	jumper->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	adirX = 0.015f;
	adirY = 0.015f;
	adirZ = 0.015f;
	jumper->setPosition(4.5f, 0.2f, 5);
}

PlayState::~PlayState()
{
	delete enemy;
	player->Release(m_world);
	delete player;
	//delete m_world;
	/*m_body->DestroyShape(m_shape);
	delete poly;
	delete bodyBox;
	delete bodyDef;
	delete bodyBoxDef;
	m_world.DestroyBody(m_body);*/

	m_floor->DestroyShape(m_shape2);
	delete poly2;
	delete bodyBox2;
	delete bodyDef2;
	delete bodyBoxDef2;
	m_world.DestroyBody(m_floor);

	actor->Release(m_world);
	delete actor;

	wall1->Release(m_world);
	delete wall1;

	delete model;

	delete skeleton;
	delete animation;
	delete jumper;
}

void PlayState::Update(double deltaTime)
{
	std::cout << "\a";

#if _DEBUG
	ImGui::Begin("Player Setting");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("PositionX", &x, -20.0f, 20.f);
	ImGui::SliderFloat("PositionY", &y, -20.0f, 20.f);
	ImGui::SliderFloat("PositionZ", &z, -20.0f, 20.f);

	/*ImGui::SliderFloat("DirX", &dirX, -50.0f, 50.f);
	ImGui::SliderFloat("DirY", &dirY, -50.0f, 50.f);
	ImGui::SliderFloat("DirZ", &dirZ, -50.0f, 50.f);*/
	ImGui::End();
#endif

#if _DEBUG
	ImGui::Begin("Light");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.f);
	
	ImGui::End();
#endif

#if _DEBUG
	ImGui::Begin("Animation Setting");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("PositionX", &ax, -20.0f, 20.f);
	ImGui::SliderFloat("PositionY", &ay, -20.0f, 20.f);
	ImGui::SliderFloat("PositionZ", &az, -20.0f, 20.f);

	ImGui::SliderFloat("DirX", &adirX, 0.0f, 0.05f);
	ImGui::SliderFloat("DirY", &adirY, 0.0f, 0.05f);
	ImGui::SliderFloat("DirZ", &adirZ, 0.0f, 0.05f);
	ImGui::End();
#endif
	light2.setIntensity(intensity);
	//light1.setPosition(x, y, z, 1);
	model->setPosition(x, y, z);
	//gTemp.setDir(0, 0, -1);
	//jumper->setPosition(ax, ay, az);
	jumper->setScale(adirX, adirY, adirZ);
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
		CameraHandler::setActiveCamera(&gTemp.getCamera());
	}
	else if (InputHandler::isKeyPressed('K'))
	{
		CameraHandler::setActiveCamera(player->getCamera());
	}
	Manager::g_meshManager.getDynamicMesh("JUMP")->getAnimatedModel()->Update(deltaTime);
	player->Update(deltaTime);
	enemy->Update(deltaTime);
	actor->Update(deltaTime);
	
	m_objectHandler.Update();
	m_levelHandler.Update();


	b3TimeStep m_step;
	m_step.dt = 1.0 / 60.0f;
	m_step.velocityIterations = 10;
	m_step.sleeping = true;
	m_world.Step(m_step);

	player->PhysicsUpdate(deltaTime);

}

void PlayState::Draw()
{
	//light1.QueueLight();
	light2.QueueLight();

	gTemp.Draw();
	m_objectHandler.Draw();
	m_levelHandler.Draw();
	actor->Draw();
	wall1->Draw();

	player->Draw();
	//player->QueueVisabilityDraw();

	model->Draw();
	//model->QueueVisabilityDraw();
	jumper->Draw();

	p_renderingManager->Flush(*CameraHandler::getActiveCamera());
	
}
