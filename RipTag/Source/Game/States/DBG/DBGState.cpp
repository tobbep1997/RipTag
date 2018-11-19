#include "RipTagPCH.h"
#include "DBGState.h"

const float moveSpeed = 5.0f;
const float rotSpeed = 7.5f;

void CameraMovment(Camera * camera, double deltaTime)
{
	if (InputHandler::isKeyPressed('A'))
		camera->Translate(-moveSpeed * deltaTime, 0, 0);
	else if (InputHandler::isKeyPressed('D'))
		camera->Translate(moveSpeed * deltaTime, 0, 0);

	if (InputHandler::isKeyPressed('W'))
		camera->Translate(0, 0, moveSpeed * deltaTime);
	else if (InputHandler::isKeyPressed('S'))
		camera->Translate(0, 0, -moveSpeed * deltaTime);

	if (InputHandler::isKeyPressed(InputHandler::Key::Left))
		camera->Rotate(0, -rotSpeed * deltaTime, 0);
	else if (InputHandler::isKeyPressed(InputHandler::Key::Right))
		camera->Rotate(0, rotSpeed * deltaTime, 0);

	if (InputHandler::isKeyPressed(InputHandler::Key::Up))
		camera->Rotate(-rotSpeed * deltaTime, 0, 0);
	else if (InputHandler::isKeyPressed(InputHandler::Key::Down))
		camera->Rotate(rotSpeed * deltaTime, 0, 0);
}


DBGState::DBGState(RenderingManager * rm) : State(rm)
{
	camera = DBG_NEW Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, .1f, 50.0f);
	
	camera->setDirection(0, 0, -1);
	camera->setPosition(0, 2, 0);

	world = DBG_NEW Drawable*[autolol * autolol];
	pointLights = DBG_NEW PointLight*[autolol2 * autolol2];
	for (int i = 0; i < autolol * autolol; i++)
	{
		world[i] = DBG_NEW Drawable();
	}
	for (int i = 0; i < autolol2 * autolol2; i++)
	{
		pointLights[i] = DBG_NEW PointLight();
		DirectX::XMFLOAT4A pos = DirectX::XMFLOAT4A(-(autolol2 / 2.0f) - 0.5f + (i % autolol2), 3, -(autolol2 / 2.0f) - 0.5f + (i / autolol2), 1);
		pointLights[i]->Init(DirectX::XMFLOAT4A(i, 3, 0, 1), DirectX::XMFLOAT4A(1, 1, 1, 1));
		pointLights[i]->setColor(256, 256, 256, 256);
		pointLights[i]->setIntensity(10);
		pointLights[i]->setDropOff(1);
		pointLights[i]->CreateShadowDirection(PointLight::ShadowDir::XYZ_ALL);
	}
	floor = new Drawable();

}


DBGState::~DBGState()
{
	delete camera;
	for (int i = 0; i < autolol * autolol; i++)
	{
		delete world[i];
	}
	for (int i = 0; i < autolol2* autolol2; i++)
	{
		delete pointLights[i];
	}
	delete[] pointLights;
	delete[] world;
	
	delete floor;
}


void DBGState::Load()
{
	Manager::g_meshManager.loadStaticMesh("BARREL");
	Manager::g_textureManager.loadTextures("BARREL");
	Manager::g_meshManager.loadStaticMesh("FLOOR");
	Manager::g_textureManager.loadTextures("FLOOR");
	
	for (int i = 0; i < autolol * autolol; i++)
	{
		world[i]->setPosition(-(autolol / 2.0f) -0.5f + (i % autolol), 0, -(autolol / 2.0f) - 0.5f + (i / autolol));
		world[i]->setModel(Manager::g_meshManager.getStaticMesh("BARREL"));
		world[i]->setTexture(Manager::g_textureManager.getTexture("BARREL"));
	}

	floor->setPosition(0, -1, 0);
	floor->setScale(50, 1, 50);
	floor->setModel(Manager::g_meshManager.getStaticMesh("FLOOR"));
	floor->setTexture(Manager::g_textureManager.getTexture("FLOOR"));
	floor->setTextureTileMult(50, 50);
}
void DBGState::unLoad()
{
	Manager::g_textureManager.UnloadAllTexture();

}
void DBGState::Update(double deltaTime)
{
	if (InputHandler::isKeyPressed(InputHandler::Key::Esc))
		PostQuitMessage(0);
	CameraMovment(this->camera, deltaTime);
	timer += deltaTime;
	for (int i = 0; i < autolol2* autolol2; i++)
	{
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(-(autolol2 / 2.0f) - 0.5f + (i % autolol2), 3, -(autolol2 / 2.0f) - 0.5f + (i / autolol2));
		pointLights[i]->setPosition(pos.x + cos(timer), 3, pos.y + sin(timer));
	}
}
void DBGState::Draw()
{
	for (int i = 0; i < autolol * autolol; i++)
	{
		world[i]->Draw();
	}
	for (int i = 0; i < autolol2* autolol2; i++)
	{
		pointLights[i]->QueueLight();
	}
	this->floor->Draw();
	this->p_renderingManager->Flush(*camera);
}