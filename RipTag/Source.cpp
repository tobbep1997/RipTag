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
float scaleX = 1;
float scaleY = 1;
float scaleZ = 1;

float posX = 1;
float posY = 1;
float posZ = -6;
void ImGuiTest()
{
#if _DEBUG
	ImGui::Begin("Cube Setting");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("Rotation", &rotSpeed, 0.0f, 0.1f);
	ImGui::SliderFloat("ScaleX", &scaleX, 0.0f, 10.f);
	ImGui::SliderFloat("ScaleY", &scaleY, 0.0f, 10.f);
	ImGui::SliderFloat("ScaleZ", &scaleZ, 0.0f, 10.f);
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
	Model m2(ObjectType::Dynamic);
	m.SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	m.SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	m2.SetVertexShader(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl");
	m2.SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");

	m.setPosition(0, 0, 0);
	m2.setPosition(-1, 0, 0);
	StaticMesh * s = new StaticMesh();
	DynamicMesh * d = new DynamicMesh();
	s->LoadModel("../Assets/sphere.bin");
	d->LoadModel("../Assets/Animationmeshtorus.bin");
	m.SetModel(s);
	m2.SetModel(d);

	m.setPosition(0, -3, 0);
	m.setScale(5, 1, 5);

	PointLight pl;
	pl.Init(DirectX::XMFLOAT4A(0,2,0,1), DirectX::XMFLOAT4A(1,1,1,1), 32132154.0f);

	Timer::StopTimer();
	std::cout << Timer::GetDurationInSeconds() << ":s" << std::endl;

	camera.setLookTo(0, 0, 0, 1);

	while (renderingManager.getWindow().isOpen())
	{
		renderingManager.Update();
		renderingManager.ImGuiStartFrame();
		
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
		pl.QueueLight();
		//camera.setPosition(posX, posY, posZ);

		//m.addRotation(0, rotSpeed, 0);
		//m.setScale(scaleX,scaleY,scaleZ);
		m.Draw();
		m2.Draw();

		
		//std::cout << std::cos(180) << std::endl;
		//camera.setPosition((float)std::cos(pos) * 5.0f, 0, (float)std::sin(pos) * 5.0f);
		//camera.setLookTo(0, 0, 0);
		
		renderingManager.Flush(camera);
	}
	DX::g_shaderManager.Release();
	renderingManager.Release();

	delete s;
	delete d;
	return 0;
}