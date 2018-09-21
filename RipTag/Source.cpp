#include <Windows.h>
#include "Source/3D Engine/RenderingManager.h"
#include "Source/Shader/ShaderManager.h"
#include "Source/3D Engine/Model/Model.h"
#include "Source/3D Engine/Model/Texture.h"
#include "Source/Light/PointLight.h"
#include "Source/3D Engine/Model/ModelManager.h"
//#pragma comment(lib, "New_Library.lib")
#include "Source/Helper/Threading.h"
 
#include <chrono>

#include "Source/Helper/Timer.h"
#if _DEBUG
#include <iostream>
//Allocates memory to the console
void _alocConsole() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
}
#endif

float scaleX = 0;
float scaleY = 0;
float scaleZ = 0;

float lightPosX = 0, lightPosY = 5, lightPosZ = 0;
float lightColorR = 1, lightColorG = 1, lightColor, lightColorB = 1;
float nearPlane = 1.0f, farPlane = 20.0f;

float lightIntensity = 1, powVar = 2.0f, dropoff = 1.0f;

int targetLight = 0;

void ImGuiTest()
{
#if _DEBUG
	ImGui::Begin("Sphere Setting");                          // Create a window called "Hello, world!" and append into it.
	//ImGui::SliderFloat("Rotation", &rotSpeed, 0.0f, 0.1f);
	ImGui::SliderFloat("PosX", &scaleX, -10.0f, 10.f);
	ImGui::SliderFloat("PosY", &scaleY, -10.0f, 10.f);
	ImGui::SliderFloat("PosZ", &scaleZ, -10.0f, 10.f);
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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
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




	RenderingManager renderingManager;


	renderingManager.Init(hInstance);
	
	//std::chrono::

	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f/9.0f);
	camera.setPosition(0, 0, -6);

	
	ModelManager modelManager;

	modelManager.addStaticMesh("../Assets/KUB.bin");
	modelManager.m_staticMesh[0]->setScale(20, 1, 20);
	modelManager.addStaticMesh("../Assets/KUB.bin");
	modelManager.m_staticMesh[1]->setScale(1, 1, 1);
	modelManager.addDynamicMesh("../Assets/Animationmeshtorus.bin");
	modelManager.m_dynamicMesh[0]->setPosition(0, 2, 0);
	modelManager.m_staticMesh[1]->setPosition(0, 1, 0);


	//PointLight pl;
	//pl.Init(DirectX::XMFLOAT4A(0,5,0,1), DirectX::XMFLOAT4A(1,1,1,1), 0.0f);
	//pl.CreateShadowDirection(PointLight::XYZ_ALL);
	//pl.CreateShadowDirection(std::vector<PointLight::ShadowDir>({ PointLight::ShadowDir::Y_NEGATIVE, PointLight::ShadowDir::X_NEGATIVE}));
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

	while (renderingManager.getWindow().isOpen())
	{
		
		renderingManager.Update();
		renderingManager.ImGuiStartFrame();
		ImGuiTest();
		MoveLight();
		point[targetLight].setColor(lightColorR, lightColorG, lightColorB);
		point[targetLight].setDropOff(dropoff);
		point[targetLight].setIntensity(lightIntensity);
		point[targetLight].setPower(powVar);
		point[targetLight].setPosition(lightPosX, lightPosY, lightPosZ); 

		auto currentTime = steady_clock::now();
		auto dt = duration_cast<nanoseconds>(currentTime - time).count();
		time = steady_clock::now();
		unprocessed += (dt / freq);

		while (unprocessed > 1)
		{
			updates++;
			unprocessed -= 1;
			

			/*
				Test Camera movement
			*/
			if (InputHandler::isKeyPressed('W'))
				camera.Translate(0.0f, 0.0f, 0.1f);
			else if (InputHandler::isKeyPressed('S'))
				camera.Translate(0.0f, 0.0f, -0.1f);
			if (InputHandler::isKeyPressed('A'))
				camera.Translate(-0.1f, 0.0f, 0.0f);
			else if (InputHandler::isKeyPressed('D'))
				camera.Translate(0.1f, 0.0f, 0.0f);
			if (InputHandler::isKeyPressed(InputHandler::SPACEBAR))
				camera.Translate(0.0f, 0.1f, 0.0f);
			else if (InputHandler::isKeyPressed(InputHandler::Shift))
				camera.Translate(0.0f, -0.1f, 0.0f);

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
		
		

		


		for (int i = 0; i < 8; i++)
		{
			//point[i].setPosition((rand() % 20) - 10, 5, (rand() % 20) - 10);
			point[i].QueueLight();
		}
		
		modelManager.DrawMeshes();
		
		renderingManager.Flush(camera);

		if (duration_cast<milliseconds>(steady_clock::now() - timer).count() > 1000)
		{
			updates = 0;
			fpsCounter = 0;
			timer += milliseconds(1000);
		}

		renderingManager.Clear();
		
	}
	DX::g_shaderManager.Release();
	renderingManager.Release();

	
	return 0;
}