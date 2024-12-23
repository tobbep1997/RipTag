
#include <WinSock2.h>
#include <Windows.h>

#include "Source/3D Engine/RenderingManager.h"
#include "Source/Shader/ShaderManager.h"
#include "Source/3D Engine/Model/Model.h"
#include "Source/3D Engine/Model/Texture.h"
#include "Source/Light/PointLight.h"


//network
#include <Multiplayer.h>
#include "NetworkMessageIdentifiers.h"
#include "CubePrototype.h"

static std::vector<CubePrototype*> * GetPlayers();
static void FlushPlayers();

#define LUA_ADD_PLAYER "AddPlayer"
#define LUA_UPDATE_REMOTE_PLAYER "UpdateRemotePlayer"
#define LUA_UPDATE_LOCAL_PLAYER "UpdateLocalPlayer"
static int Lua_Player_Add(lua_State *L);
static int Lua_Update_Remote_Player(lua_State * L);
static int Lua_Update_Local_Player(lua_State * L);

//LUA
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "Source/3D Engine/Model/Managers/ModelManager.h"
//#pragma comment(lib, "New_Library.lib")
#include "Source/Helper/Threading.h"
#include "Source/3D Engine/Temp_Guard/TempGuard.h"
#include <chrono>
#include <thread>
#include "Source/Helper/Timer.h"
#include "../InputManager/XboxInput/GamePadHandler.h"
#include "../Engine/Source/3D Engine/Extern.h"
#include "../Lua Talker/Source/LuaTalker.h"
#if _DEBUG
#include <iostream>
//Allocates memory to the console
void _alocConsole() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
}
#endif

LuaTalker LUA::g_luaTalker;

float playerScaleX = 1.0f;
float playerScaleY = 1.0f;
float playerScaleZ = 1.0f;

float playerPosX = 0.0f;
float playerPosY = 0.0f;
float playerPosZ = 0.0f;

float lightPosX = 0, lightPosY = 5, lightPosZ = 0;
float lightColorR = 1, lightColorG = 1, lightColor, lightColorB = 1;
float nearPlane = 1.0f, farPlane = 20.0f;

float lightIntensity = 5, powVar = 2.0f, dropoff = 0.0f;

int targetLight = 0;

void MovePlayer()
{
#if _DEBUG
	ImGui::Begin("Player Setting");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("PositionX", &playerPosX, -50.0f, 50.f);
	ImGui::SliderFloat("PositionY", &playerPosY, -50.0f, 50.f);
	ImGui::SliderFloat("PositionZ", &playerPosZ, -50.0f, 50.f);
	ImGui::SliderFloat("ScaleX", &playerScaleX, 0.1f, 3.0f);
	ImGui::SliderFloat("ScaleY", &playerScaleY, -0.1f, 3.0f);
	ImGui::SliderFloat("ScaleZ", &playerScaleZ, -0.1f, 3.0f);
	ImGui::End();
#endif

}

void MoveLight() {
#if _DEBUG
	ImGui::Begin("Light pos");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("posX", &lightPosX, -30.0f, 30.f); // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("posY", &lightPosY, -50.0f, 30.f); // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("posZ", &lightPosZ, -30.0f, 30.f); // Create a window called "Hello, world!" and append into it.

	ImGui::SliderFloat("R", &lightColorR, 0.0f, 1.0f);
	ImGui::SliderFloat("G", &lightColorG, 0.0f, 1.0f);
	ImGui::SliderFloat("B", &lightColorB, 0.0f, 1.0f);

	ImGui::SliderFloat("DropOff", &dropoff, 0.0f, 1.0f);
	ImGui::SliderFloat("Intensity", &lightIntensity, 0.0f, 5.0f);
	ImGui::SliderFloat("pow", &powVar, 1.0f, 5.0f);
	ImGui::SliderFloat("NearPlane", &nearPlane, 0.1f, 3.0f);
	ImGui::SliderFloat("FarPlane", &farPlane, 3.1f, 50.0f);

	ImGui::End();
#endif
}

#pragma region AnimationDebugStuff
Animation::AnimatedModel* g_animatedModel = nullptr;
Animation::AnimationClip* g_currentTargetClip = nullptr;
Animation::AnimationClip* g_currentClip = nullptr;
float g_blendTime = 1.0f;
float g_currentTime = 0.0f;
float g_weight = 0.0f;
int  g_currentFrame = 0.0f;
void AnimationGUI()
{
#if _DEBUG
	ImGui::Begin("Animation");
	ImGui::SliderFloat("Blend time", &g_blendTime, 0.1, 10.0);
	if (ImGui::Button("Blend.."))
	{
		g_animatedModel->SetTargetClip(g_currentClip, BLEND_FROM_START, g_blendTime);
		std::swap(g_currentClip, g_currentTargetClip);
	}
	ImGui::Text("%f", g_currentTime);
	ImGui::SliderInt("Frame", &g_currentFrame, 0.0, g_currentClip->m_frameCount);

	ImGui::Separator();

	if (ImGui::SliderFloat("Weight", &g_weight, 0.0, 1.0))
		g_animatedModel->SetLayeredClipWeight(g_weight);


	ImGui::End();
#endif
}
#pragma endregion Animation ImGui stuff and globals for testing

/*v*/

#if _DEBUG
	_alocConsole();
#endif
	assert(CoInitializeEx(NULL, NULL) == S_OK);
	
	Threading::Init();
	Timer::StartTimer();
	
	using namespace std::chrono;
	const float REFRESH_RATE = 60.0f;
	auto time = steady_clock::now();
	auto timer = steady_clock::now();
	int updates = 0;
	int fpsCounter = 0;
	float freq = 1000000000.0f / REFRESH_RATE;
	float unprocessed = 0;

	LUA::g_luaTalker.runScript("print('lol')");

	RenderingManager renderingManager;

	

	renderingManager.Init(hInstance);
	
	//std::chrono::

	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f/9.0f, 0.1f, 50.0f);
	camera.setPosition(0, 0, -6);

	Guard gTemp;
	gTemp.setPos(0, 5, 0);
	
	GamePadHandler::Instance();
	
	Manager::g_textureManager.loadTextures("SPHERE");

	
	Manager::g_meshManager.loadStaticMesh("SCENE");
	Manager::g_meshManager.loadStaticMesh("PIRASRUM");
	Manager::g_meshManager.loadStaticMesh("SPHERE");
	//Manager::g_meshManager.loadDynamicMesh("TORUS");
	//Manager::g_meshManager.loadDynamicMesh("KON");
	
	Animation::Skeleton* skeleton = nullptr;
	Animation::AnimationClip* animation = nullptr;
	Animation::AnimationClip* animation2 = nullptr;
	Animation::AnimationClip* diffAnimation = nullptr;
	Manager::g_meshManager.loadDynamicMesh("CYLS");
	skeleton = Animation::LoadAndCreateSkeleton("../Assets/CYLSFOLDER/CYLS_SKELETON.bin");
	animation = Animation::LoadAndCreateAnimation("../Assets/CYLSFOLDER/CYLS_ANIMATION.bin", skeleton);
	animation2 = Animation::LoadAndCreateAnimation("../Assets/CYLSFOLDER/CYLR_ANIMATION.bin", skeleton);
	Manager::g_meshManager.getDynamicMesh("CYLS")->m_anim = new Animation::AnimatedModel();
	g_animatedModel = Manager::g_meshManager.getDynamicMesh("CYLS")->getAnimatedModel();

	diffAnimation = Animation::computeDifferenceClip(animation, animation2);
	Animation::bakeDifferenceClipOntoClip(animation2, diffAnimation);
	g_animatedModel->SetSkeleton(skeleton);
	g_animatedModel->SetPlayingClip(animation2);
	g_animatedModel->SetLayeredClip(diffAnimation, .0, BLEND_MATCH_TIME);
	g_animatedModel->Play();

	g_currentTargetClip = animation2;
	g_currentClip = diffAnimation;
	
	ModelManager modelmanager;
	modelmanager.addNewModel(Manager::g_meshManager.getStaticMesh("SCENE"), Manager::g_textureManager.getTexture("SPHERE"));


	Model * player = new Model();
	player->setEntityType(EntityType::PlayerType);
	player->setModel(Manager::g_meshManager.getDynamicMesh("CYLS"));
	//player->setScale(0.003f, 0.003f, 0.003f);
	player->setTexture(Manager::g_textureManager.getTexture("SPHERE"));

	std::vector<PointLight> point;

	for (int i = 0; i < 8; i++)
	{
		point.push_back(PointLight());
	}
	//srand(time(0));
	for (int i = 0; i < 8; i++)
	{

		point[i].Init(DirectX::XMFLOAT4A(0, 5, 0, 1), DirectX::XMFLOAT4A(1, 1, 1, 1), 0.5f);
		point[i].CreateShadowDirection(PointLight::XYZ_ALL);
		point[i].setPosition((rand() % 20) - 10, 5, (rand() % 20) - 10);
		point[i].setColor((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f);
		point[i].setFarPlane(farPlane);
		point[i].setNearPlane(nearPlane);
		point[i].setIntensity((rand() % 2));
		point[i].setDropOff(.2f);
		point[i].setPower(2.0f);
	}

	Timer::StopTimer();
	std::cout << Timer::GetDurationInSeconds() << ":s" << std::endl;

	camera.setLookTo(0, 0, 0, 1);

	double pos = 0;
	//modelManager.bindTextures();

	CubePrototype * antiCrashObject = 0;
	antiCrashObject = new CubePrototype(-1.f, -1.f, -1.f);

	bool hasMoved = false;

	while (renderingManager.getWindow().isOpen())
	{
		renderingManager.Update();
		renderingManager.ImGuiStartFrame();
		GamePadHandler::UpdateState();
		MovePlayer();
		MoveLight();
		AnimationGUI();

		point[targetLight].setColor(lightColorR, lightColorG, lightColorB);
		point[targetLight].setDropOff(dropoff);
		point[targetLight].setIntensity(lightIntensity);
		point[targetLight].setPower(powVar);
		point[targetLight].setPosition(lightPosX, lightPosY, lightPosZ); 


		//player->setScale(playerScaleX, playerScaleY, playerScaleZ);
		player->setPosition(playerPosX, playerPosY, playerPosZ);
		//modelManager.m_dynamicModel[0]->setScale(playerScaleX, playerScaleY, playerScaleZ);

		auto currentTime = steady_clock::now();
		auto dt = duration_cast<nanoseconds>(currentTime - time).count();
		float floatDt = static_cast<float>(dt) / 1000000000;
		time = steady_clock::now();
		unprocessed += (dt / freq);

		while (unprocessed > 1)
		{
			updates++;
			unprocessed -= 1;
			if (GamePadHandler::IsAPressed())
			{
				std::cout << "wop" << std::endl;
			}

			/*
				Test Camera movement
			*/
			if (InputHandler::isKeyPressed('W'))
				camera.Translate(0.0f, 0.0f, 0.5f);
			else if (InputHandler::isKeyPressed('S'))
				camera.Translate(0.0f, 0.0f, -0.5f);
			if (InputHandler::isKeyPressed('A'))
				camera.Translate(-0.5f, 0.0f, 0.0f);
			else if (InputHandler::isKeyPressed('D'))
				camera.Translate(0.5f, 0.0f, 0.0f);
			if (InputHandler::isKeyPressed(InputHandler::SPACEBAR))
				camera.Translate(0.0f, 0.5f, 0.0f);
			else if (InputHandler::isKeyPressed(InputHandler::Shift))
				camera.Translate(0.0f, -0.5f, 0.0f);


			// VERRY TEMP
			static float t1 = 0.0f, t2 = 0.0f;
			static float t3 = gTemp.getPos().y;

			if (InputHandler::isKeyPressed('T'))
				gTemp.setPos(t2, t3, t1 += 0.5f);
			else if (InputHandler::isKeyPressed('G'))
				gTemp.setPos(t2, t3, t1 -= 0.5f);
			if (InputHandler::isKeyPressed('F'))
				gTemp.setPos(t2 -= 0.5f, t3, t1);
			else if (InputHandler::isKeyPressed('H'))
				gTemp.setPos(t2 += 0.5f, t3, t1);

			if (InputHandler::isKeyPressed('I'))
				gTemp.Rotate(0.05f, 0.0f, 0.0f);
			else if (InputHandler::isKeyPressed('K'))
				gTemp.Rotate(-0.05f, 0.0f, 0.0f);
			if (InputHandler::isKeyPressed('J'))
				gTemp.Rotate(0.0f, -0.05f, 0.0f);
			else if (InputHandler::isKeyPressed('L'))
				gTemp.Rotate(0.0f, 0.05f, 0.0f);
			// VERRY TEMP END


			/*
				Test Camera rotation
			*/
			if (InputHandler::isKeyPressed(InputHandler::UpArrow))
				camera.Rotate(0.05f, 0.0f, 0.0f);
			else if (InputHandler::isKeyPressed(InputHandler::DownArrow))
				camera.Rotate(-0.05f, 0.0f, 0.0f);
			if (InputHandler::isKeyPressed(InputHandler::LeftArrow))
				camera.Rotate(0.0f, -0.05f, 0.0f);
			else if (InputHandler::isKeyPressed(InputHandler::RightArrow))
				camera.Rotate(0.0f, 0.05f, 0.0f);

			if (InputHandler::isKeyPressed(InputHandler::Esc))
				renderingManager.getWindow().Close();
			if (InputHandler::isKeyPressed(InputHandler::BackSpace))
				camera.setLookTo(0, 0, 0, 1);

			if (InputHandler::isKeyPressed('1'))
				targetLight = 0;
			else if (InputHandler::isKeyPressed('2'))
				targetLight = 1;
			else if (InputHandler::isKeyPressed('3'))
				targetLight = 2;
			else if (InputHandler::isKeyPressed('4'))
				targetLight = 3;
			else if (InputHandler::isKeyPressed('5'))
				targetLight = 4;
			else if (InputHandler::isKeyPressed('6'))
				targetLight = 5;
			else if (InputHandler::isKeyPressed('7'))
				targetLight = 6;
			else if (InputHandler::isKeyPressed('8'))
				targetLight = 7;

			DirectX::XMFLOAT4A pointPos = point[targetLight].getPosition();
			DirectX::XMFLOAT4A pointColor = point[targetLight].getColor();
			dropoff = point[targetLight].getDropOff();
			lightIntensity = point[targetLight].getIntensity();
			powVar = point[targetLight].getPow();
			lightPosX = pointPos.x;
			lightPosY = pointPos.y;
			lightPosZ = pointPos.z;
			lightColorR = pointColor.x;
			lightColorG = pointColor.y;
			lightColorB = pointColor.z;

		}
		
		g_animatedModel->Update(floatDt);
		g_currentTime = g_animatedModel->GetCurrentTimeInClip();
		g_currentFrame = g_animatedModel->GetCurrentFrameIndex();
		modelmanager.DrawMeshes();


		for (int i = 0; i < 8; i++)
		{
			point[i].QueueLight();
		}		
		gTemp.Draw();
		player->Draw();
		//player->DrawWireFrame();
		player->QueueVisabilityDraw();


		renderingManager.Flush(camera);
		
		if (duration_cast<milliseconds>(steady_clock::now() - timer).count() > 1000)
		{
			updates = 0;
			fpsCounter = 0;
			timer += milliseconds(1000);
		}

		renderingManager.Clear();
		
	}

	if (skeleton)
		delete skeleton;
	if (animation)
		delete animation;
	if (animation2)
		delete animation2;

	DX::g_shaderManager.Release();
	renderingManager.Release();
	delete player;
	return 0;
}

static std::vector<CubePrototype*> * GetPlayers()
{
	static std::vector<CubePrototype*> Players;

	return &Players;
}

static void FlushPlayers()
{
	std::vector<CubePrototype*> * vec = GetPlayers();
	for (size_t i = 0; i < vec->size(); i++)
	{
		delete vec->at(i);
	}
	vec->clear();

	lua_pushnil(L);
	lua_setglobal(L, "PLAYER_NID");
}

static int Lua_Player_Add(lua_State *L)
{
	CubePrototype * ptr = (CubePrototype*)lua_touserdata(L, lua_gettop(L));
	if (ptr)
	{
		GetPlayers()->push_back(ptr);
	}
	return 0;
}

static int Lua_Update_Remote_Player(lua_State * L)
{
	Network::ENTITY_MOVE_MESSAGE * data = (Network::ENTITY_MOVE_MESSAGE *)lua_touserdata(L, -1);
	if (data)
	{
		RakNet::NetworkID nid = data->networkId;
		std::vector<CubePrototype*> * players = GetPlayers();
		for (size_t i = 0; i < players->size(); i++)
		{
			if (players->at(i)->GetNetworkID() == nid)
				players->at(i)->lerpPosition(DirectX::XMFLOAT4A(data->x, data->y, data->z, 1.0f), data->timeStamp);
		}
	}
	return 0;
}

static int Lua_Update_Local_Player(lua_State * L)
{
	RakNet::NetworkID nid = std::stoull(lua_tostring(L, -4));
	float x = lua_tonumber(L, -3);
	float y = lua_tonumber(L, -2);
	float z = lua_tonumber(L, -1);

	lua_pop(L, 4);

	DirectX::XMFLOAT4A pos(x, y, z, 1.0f);

	std::vector<CubePrototype*> * players = GetPlayers();
	for (size_t i = 0; i < players->size(); i++)
	{
		if (players->at(i)->GetNetworkID() == nid)
			players->at(i)->setPosition(pos);
	}
	return 0;
}

