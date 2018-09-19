#include <Windows.h>
#include "Source/3D Engine/RenderingManager.h"
#include "Source/Shader/ShaderManager.h"
#include "Source/3D Engine/Model/Model.h"
#include "Source/3D Engine/Model/Texture.h"
#include "Source/Light/PointLight.h"
#include "Source/3D Engine/Model/ModelManager.h"
//#pragma comment(lib, "New_Library.lib")
#include "Source/Helper/Threading.h"
 
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
float lightPosX1 = 0, lightPosY1 = 5, lightPosZ1 = 0;
float lightColorR = 1, lightColorG = 1, lightColor, lightColorB = 1;
float nearPlane = 1.0f, farPlane = 20.0f;

float lightIntensity = 1, powVar = 2.0f, dropoff = 1.0f;
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

	ImGui::SliderFloat("posX1", &lightPosX1, -30.0f, 30.f); // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("posY1", &lightPosY1, -50.0f, 30.f); // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("posZ1", &lightPosZ1, -30.0f, 30.f); // Create a window called "Hello, world!" and append into it.


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

	RenderingManager renderingManager;


	renderingManager.Init(hInstance);
	
	//std::chrono::

	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f/9.0f);
	camera.setPosition(0, 0, -6);

	
	ModelManager modelManager;

	modelManager.addStaticMesh("../Assets/KUB.bin");
	modelManager.staticMesh[0]->setScale(10, 1, 10);
	modelManager.addStaticMesh("../Assets/KUB.bin");
	modelManager.staticMesh[1]->setScale(1, 1, 1);
	modelManager.addDynamicMesh("../Assets/Animationmeshtorus.bin");
	modelManager.dynamicMesh[0]->setPosition(0, 2, 0);
	modelManager.staticMesh[1]->setPosition(0, 1, 0);


	PointLight pl;
	pl.Init(DirectX::XMFLOAT4A(0,5,0,1), DirectX::XMFLOAT4A(1,1,1,1), 0.0f);
	pl.CreateShadowDirection(PointLight::XYZ_ALL);
	PointLight pl2;
	pl2.Init(DirectX::XMFLOAT4A(5, 5, 0, 1), DirectX::XMFLOAT4A(1, 1, 1, 1), 0.0f);
	pl2.CreateShadowDirection(PointLight::XYZ_ALL);
	
	Timer::StopTimer();
	std::cout << Timer::GetDurationInSeconds() << ":s" << std::endl;

	camera.setLookTo(0, 0, 0, 1);

	double pos = 0;

	while (renderingManager.getWindow().isOpen())
	{
		renderingManager.Update();
		renderingManager.ImGuiStartFrame();
		pl.SetPosition(lightPosX, lightPosY, lightPosZ);
		pl.SetColor(lightColorR, lightColorG, lightColorB);
		pl.setFarPlane(farPlane);
		pl.setNearPlane(nearPlane);
		
		pl.SetIntensity(lightIntensity);
		pl.setDropOff(dropoff);
		pl.setPower(powVar);

		pl2.SetPosition(lightPosX1, lightPosY1, lightPosZ1);
		pl2.SetColor(lightColorR, lightColorG, lightColorB);
		pl2.setFarPlane(farPlane);
		pl2.setNearPlane(nearPlane);

		pl2.SetIntensity(lightIntensity);
		pl2.setDropOff(dropoff);
		pl2.setPower(powVar);


		/*
			Test Camera movement
		*/
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
		
		if (InputHandler::isKeyPressed('L'))
			camera.setPosition(pl2.getPosition());

		

		ImGuiTest();
		MoveLight();
		pl.QueueLight();

		//pl2.QueueLight();
		
		modelManager.DrawMeshes();
		
		renderingManager.Flush(camera);
	}
	DX::g_shaderManager.Release();
	renderingManager.Release();

	
	return 0;
}