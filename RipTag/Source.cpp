
#include <WinSock2.h>
#include <Windows.h>

#include "Source/3D Engine/RenderingManager.h"
#include "Source/Shader/ShaderManager.h"
#include "Source/3D Engine/Model/Model.h"
#include "Source/Light/PointLight.h"
//#pragma comment(lib, "New_Library.lib")

//network
#include <Multiplayer.h>
#include "NetworkMessageIdentifiers.h"
#include "CubePrototype.h"

static std::vector<CubePrototype*> * GetPlayers();
static void FlushPlayers();

#define LUA_ADD_PLAYER "AddPlayer"
static int Lua_Player_Add(lua_State *L);

//LUA
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "Source/Helper/Timer.h"
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


float lightPosX = 0, lightPosY = 5, lightPosZ = 0;
float lightColorR = 1, lightColorG = 1, lightColor, lightColorB = 1;
float lightIntensity = 1;

void MoveLight() {
#if _DEBUG
	ImGui::Begin("Light pos");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("posX", &lightPosX, -500.0f, 500.f);
	ImGui::SliderFloat("posY", &lightPosY, -50.0f, 50.f);
	ImGui::SliderFloat("posZ", &lightPosZ, -50.0f, 50.f);

	ImGui::SliderFloat("R", &lightColorR, 0.0f, 1.0f);
	ImGui::SliderFloat("G", &lightColorG, 0.0f, 1.0f);
	ImGui::SliderFloat("B", &lightColorB, 0.0f, 1.0f);

	ImGui::SliderFloat("Intensity", &lightIntensity, 0.0f, 1.0f);

	ImGui::End();
#endif
}


bool gameIsRunning = false;

void NetworkSettings(Network::Multiplayer * pMP, lua_State * L)
{
	bool startServer = false;
	bool startClient = false;
	
	ImGui::Begin("Network Settings");
	if (!pMP->isRunning())
	{
		startServer = ImGui::Button("Start Server");
		startClient = ImGui::Button("Start Client");
	}
	if (startServer)
	{
		pMP->StartUpServer();
	}
	else if (startClient)
	{
		pMP->StartUpClient();
	}
	if (pMP->isRunning() && !pMP->isConnected())
	{
		if (pMP->isServer())
			ImGui::Text("Server running... Awaiting Connections...");
		else
			ImGui::Text("Client running... Looking for Server...");
	}
	if (pMP->isRunning() && pMP->isConnected())
	{
		ImGui::Text(pMP->GetNetworkInfo().c_str());
		if (!gameIsRunning && pMP->isServer())
		{
			if (ImGui::Button("Start Game"))
			{
				lua_getglobal(L, "GameStart");
				lua_pushboolean(L, pMP->isServer());
				if (lua_pcall(L, 1, 0, NULL) > 0)
				{
					printf(lua_tostring(L, -1));
					lua_pop(L, 1);
				}
				gameIsRunning = true;
			}
		}
		if (ImGui::Button("Disconnect"))
			pMP->Disconnect();
	}
	ImGui::End();
}

void NetworkDebug(Network::Multiplayer * pMP)
{
	ImGui::Begin("Network Debug Statistics");
	ImGui::Text(pMP->GetNetworkStatistics().c_str());
	ImGui::End();
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//-------NETWORKING-----------
	Network::Multiplayer * pNetwork = Network::Multiplayer::GetInstance();

	

#if _DEBUG
	_alocConsole();
#endif
	//LUA
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	//register classes in Lua
	Network::LUA_Register_Network(L);
	Network::LUA_Register_Network_Structs(L);
	Network::LUA_Register_Network_MessageTypes(L);
	Network::LUA_Register_Packet_Priorities(L);
	LUA_Register_CubePrototype(L);
	lua_register(L, LUA_ADD_PLAYER, Lua_Player_Add);

	//load our Lua libraries into the global enviroment (maybe run a LoadLibs.lua script for this?)
	luaL_dofile(L, "..//Scripts//LuaInit.lua");
	luaL_dofile(L, "..//Scripts//GameObjects//EntityLib.lua");
	luaL_dofile(L, "..//Scripts//GameObjects//Game.lua");

	pNetwork->pLuaState = L;

	Timer::StartTimer();

	RenderingManager renderingManager;

	
	

	renderingManager.Init(hInstance);
	
	//std::chrono::

	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f/9.0f);
	camera.setPosition(0, 0, -6);


	LUA_Register_Camera(L, &camera);
	

	PointLight pl;
	pl.Init(DirectX::XMFLOAT4A(0,5,0,1), DirectX::XMFLOAT4A(1,1,1,1), 0.0f);

	Timer::StopTimer();
	std::cout << Timer::GetDurationInSeconds() << ":s" << std::endl;

	camera.setLookTo(0, 0, 0, 1);

	std::vector<CubePrototype*> * players = GetPlayers();

	CubePrototype * antiCrashObject = 0;
	antiCrashObject = new CubePrototype(-1.f, -1.f, -1.f);

	while (renderingManager.getWindow().isOpen())
	{
		renderingManager.Update();
		renderingManager.ImGuiStartFrame();
		pl.SetPosition(lightPosX, lightPosY, lightPosZ);
		pl.SetColor(lightColorR, lightColorG, lightColorB);
		pl.SetIntensity(lightIntensity);

		/*
			Test Network		
		*/
		/*
			Test Camera movement
		*/
		{
			if (InputHandler::isKeyPressed('W'))
				camera.Move(0.0f, 0.0f, 0.01f);
			else if (InputHandler::isKeyPressed('S'))
				camera.Move(0.0f, 0.0f, -0.01f);
			if (InputHandler::isKeyPressed('A'))
				camera.Move(-0.01f, 0.0f, 0.0f);
			else if (InputHandler::isKeyPressed('D'))
				camera.Move(0.01f, 0.0f, 0.0f);
			if (InputHandler::isKeyPressed(InputHandler::SPACEBAR))
				camera.Move(0.0f, 0.01f, 0.0f);
			else if (InputHandler::isKeyPressed(InputHandler::Shift))
				camera.Move(0.0f, -0.01f, 0.0f);
			/*
				Test Camera rotation
			*/
			if (InputHandler::isKeyPressed(InputHandler::UpArrow))
				camera.Rotate(0.005f, 0.0f, 0.0f);
			else if (InputHandler::isKeyPressed(InputHandler::DownArrow))
				camera.Rotate(-0.005f, 0.0f, 0.0f);
			if (InputHandler::isKeyPressed(InputHandler::LeftArrow))
				camera.Rotate(0.0f, -0.005f, 0.0f);
			else if (InputHandler::isKeyPressed(InputHandler::RightArrow))
				camera.Rotate(0.0f, 0.005f, 0.0f);
			if (InputHandler::isKeyPressed(InputHandler::Esc))
				renderingManager.getWindow().Close();

			if (InputHandler::isKeyPressed(InputHandler::BackSpace))
				camera.setLookTo(0, 0, 0, 1);
		}


		//ImGuiTest();
		//CameraTest();
		//MoveLight();
		NetworkSettings(pNetwork, L);
		NetworkDebug(pNetwork);

		pNetwork->Update();
		
		pl.QueueLight();
		
		antiCrashObject->Draw();

		for(size_t i = 0; i < players->size(); i++)
			players->at(i)->Draw();

		if (players->size() == 2)
			int _break = 0;

		renderingManager.Flush(camera);
	}
	DX::g_shaderManager.Release();
	renderingManager.Release();
	lua_close(L);
	FlushPlayers();
	delete antiCrashObject;

	return 0;
}

static std::vector<CubePrototype*> * GetPlayers()
{
	static std::vector<CubePrototype*> Players;

	return &Players;
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

static void FlushPlayers()
{
	std::vector<CubePrototype*> * vec = GetPlayers();
	for (size_t i = 0; i < vec->size(); i++)
	{
		delete vec->at(i);
	}
	vec->clear();
}