#include <Windows.h>
#include "Source/3D Engine/RenderingManager.h"
#include "Source/Shader/ShaderManager.h"
#include "Source/3D Engine/Model/Model.h"
#include "Source/Light/PointLight.h"
#pragma comment(lib, "New_Library.lib")
 
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
float rotSpeed = 0.001f;
float scaleX = 0;
float scaleY = 0;
float scaleZ = 0;

float posX = 1;
float posY = 1;
float posZ = -6;

float lightPosX = 0, lightPosY = 5, lightPosZ = 0;
float lightColorR = 1, lightColorG = 1, lightColor, lightColorB = 1;
float lightIntensity = 1;
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

void CameraTest()
{
#if _DEBUG
	ImGui::Begin("Camera Settings");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("posX", &posX, -20.0f, 20.f);
	ImGui::SliderFloat("posY", &posY, -20.0f, 20.f);
	ImGui::SliderFloat("posZ", &posZ, -20.0f, 20.f);
	ImGui::End();
#endif
}

void MoveLight() {
#if _DEBUG
	ImGui::Begin("Light pos");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("posX", &lightPosX, -50.0f, 50.f);
	ImGui::SliderFloat("posY", &lightPosY, -50.0f, 50.f);
	ImGui::SliderFloat("posZ", &lightPosZ, -50.0f, 50.f);

	ImGui::SliderFloat("R", &lightColorR, 0.0f, 1.0f);
	ImGui::SliderFloat("G", &lightColorG, 0.0f, 1.0f);
	ImGui::SliderFloat("B", &lightColorB, 0.0f, 1.0f);

	ImGui::SliderFloat("Intensity", &lightIntensity, 0.0f, 1.0f);

	ImGui::End();
#endif
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
#if _DEBUG
	_alocConsole();
#endif

	Timer::StartTimer();

	RenderingManager renderingManager;

	

	renderingManager.Init(hInstance);
	
	//std::chrono::

	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f/9.0f);
	camera.setPosition(0, 0, -6);

	Model m(ObjectType::Static);
	Model m2;
	m.SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	m.SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	m2.SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	m2.SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");

	m.setPosition(0, 0, 0);
	m2.setPosition(-1, 0, 0);
	StaticMesh * s = new StaticMesh();
	StaticMesh * d = new StaticMesh();
	s->LoadModel("../Assets/kub.bin");
	d->LoadModel("../Assets/sphere.bin");
	m.SetModel(s);
	m2.SetModel(d);

	m.setPosition(0, -3, 0);
	m.setScale(20, 1, 20);

	PointLight pl;
	pl.Init(DirectX::XMFLOAT4A(0,5,0,1), DirectX::XMFLOAT4A(1,1,1,1), 0.0f);
	
	

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
		pl.SetIntensity(lightIntensity);

		/*
			Test Camera movement
		*/
		if (GetAsyncKeyState(int('W')))
			camera.Move(0.0f, 0.0f, 0.01f);
		else if (GetAsyncKeyState(int('S')))
			camera.Move(0.0f, 0.0f, -0.01f);
		if (GetAsyncKeyState(int('A')))
			camera.Move(-0.01f, 0.0f, 0.0f);
		else if (GetAsyncKeyState(int('D')))
			camera.Move(0.01f, 0.0f, 0.0f);
		if (GetAsyncKeyState(VK_SPACE))
			camera.Move(0.0f, 0.01f, 0.0f);
		else if (GetAsyncKeyState(VK_SHIFT))
			camera.Move(0.0f, -0.01f, 0.0f);

		/*
			Test Camera rotation
		*/
		if (GetAsyncKeyState(VK_UP))
			camera.Rotate(0.005f, 0.0f, 0.0f);
		else if (GetAsyncKeyState(VK_DOWN))
			camera.Rotate(-0.005f, 0.0f, 0.0f);
		if (GetAsyncKeyState(VK_LEFT))
			camera.Rotate(0.0f, -0.005f, 0.0f);
		else if (GetAsyncKeyState(VK_RIGHT))
			camera.Rotate(0.0f, 0.005f, 0.0f);
		if (GetAsyncKeyState(VK_ESCAPE))
			renderingManager.getWindow().Close();

		if (GetAsyncKeyState(VK_BACK))
			camera.setLookTo(0, 0, 0, 1);


		ImGuiTest();
		//CameraTest();
		MoveLight();
		pos += Timer::GetDurationInSeconds() * 0.03;
		pl.SetPosition((float)std::cos(pos) * 5.0f, 5, (float)std::sin(pos) * 5.0f);
		pl.SetColor((float)std::cos(pos), (float)std::sin(pos), (float)std::tan(pos));
		pl.SetIntensity(1.0 - std::abs((float)std::sin(pos) * 0.1f));
		pl.QueueLight();
		//camera.setPosition(posX, posY, posZ);
		m2.setPosition(scaleX, scaleY, scaleZ);
		//m.addRotation(0, rotSpeed, 0);
		//m.setScale(scaleX,scaleY,scaleZ);
		m.Draw();
		m2.Draw();


		
		//std::cout << std::cos(180) << std::endl;
		//camera.setLookTo(0, 0, 0);
		
		renderingManager.Flush(camera);
	}
	DX::g_shaderManager.Release();
	renderingManager.Release();

	delete s;
	delete d;
	return 0;
}