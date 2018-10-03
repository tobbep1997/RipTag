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

	bodyDef = new b3BodyDef();
	bodyDef->type = e_dynamicBody;

	// Position the body 10 meters high from the world origin.
	bodyDef->position.Set(0.0f, 100.0f, 0.0f);
	
	m_body = m_world.CreateBody(*bodyDef);
	bodyBox = new b3Hull();

	bodyBox->SetAsBox(b3Vec3(1, 1, 1));

	poly = new b3Polyhedron();
	poly->SetHull(bodyBox);
	
	
	bodyBoxDef = new b3ShapeDef();
	bodyBoxDef->shape = poly;
	bodyBoxDef->density = 1.0f;
	bodyBoxDef->restitution = 0.2;
	
	using namespace std::chrono_literals;
	//std::cout << "Hello waiter" << std::endl; // flush is intentional
	//auto start = std::chrono::high_resolution_clock::now();
	
	m_shape = m_body->CreateShape(*bodyBoxDef);

	bodyDef2 = new b3BodyDef();
	bodyDef2->type = e_staticBody;

	// Position the body 10 meters high from the world origin.
	bodyDef2->position.Set(0.0f, 0.0f, 0.0f);

	m_floor = m_world.CreateBody(*bodyDef2);
	bodyBox2 = new b3Hull();

	bodyBox2->SetAsBox(b3Vec3(100, 1, 100));

	poly2 = new b3Polyhedron();
	poly2->SetHull(bodyBox2);


	bodyBoxDef2 = new b3ShapeDef();
	bodyBoxDef2->shape = poly2;
	bodyBoxDef2->density = 1.0f;
	bodyBoxDef2->restitution = 1;

	m_shape2 = m_floor->CreateShape(*bodyBoxDef2);

	//std::this_thread::sleep_for(2s);
	//m_body->SetTransform(b3Vec3(0, 10, 0), b3Vec3(0, 0, 0), 0);
	//m_body->SetGravityScale(-9.82f);
	//m_shape->SetTransform(b3Vec3(0, 10, 0), b3Vec3(0, 0, 0), 0);
	Manager::g_meshManager.loadStaticMesh("SPHERE");
	temp = new Model();
	//temp->setEntityType();
	temp->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	temp->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	temp->setPosition(0, 10, 0);
}

PlayState::~PlayState()
{
	delete enemy;
	delete player;
	//delete m_world;
	//m_body->DestroyShape(shape);
}

void PlayState::Update(double deltaTime)
{
	if (GamePadHandler::IsAPressed())
	{
		Input::ForceDeactivateGamepad();
	}
	
	player->Update(deltaTime);
	enemy->Update(deltaTime);
	
	m_objectHandler.Update();
	m_levelHandler.Update();


	b3TimeStep m_step;
	m_step.dt = 1.0 / 60.0f;
	m_step.velocityIterations = 10;
	m_world.Step(m_step);

	std::cout << "Y: " << m_body->GetTransform().translation.y << std::endl;
	temp->setPosition(0, m_body->GetTransform().translation.y, 0);
}

void PlayState::Draw()
{
	m_objectHandler.Draw();
	m_levelHandler.Draw();
	temp->Draw();
	p_renderingManager->Flush(*CameraHandler::getActiveCamera());
}
