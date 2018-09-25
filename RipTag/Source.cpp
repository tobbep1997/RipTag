#include <Windows.h>
#include "Source/3D Engine/RenderingManager.h"
#include "Source/Shader/ShaderManager.h"
#include "Source/3D Engine/Model/Model.h"
#include "Source/3D Engine/Model/Texture.h"
#include "Source/Light/PointLight.h"
#include "Source/3D Engine/Model/ModelManager.h"
#include "Source/3D Engine/Model/AnimatedModel.h"
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
	modelManager.m_staticMesh[0]->setPosition(10.0, 0.0, 0.0);
	modelManager.addDynamicMesh("../Assets/pCube5_ANIMATION_Mesh.bin");
	modelManager.m_dynamicMesh[0]->setPosition({ 3.0, -2.0, 0.0, 1.0 });
	modelManager.m_dynamicMesh[0]->setScale(0.6, 0.6, 0.6);
	//modelManager.staticMesh[0]->setPosition(0, 0, 0);

	
	
		Animation::AnimationCBuffer animCB;
		animCB.SetAnimationCBuffer();

		auto skeleton = Animation::LoadAndCreateSkeleton("../Assets/joint1_Skeleton.bin");
		auto animation = Animation::LoadAndCreateAnimation("../Assets/ANIMATION_ANIMATION.bin", skeleton);

		std::vector<Animation::SRT> srts;
		for (int i = 0; i < skeleton->m_jointCount; i++)
		{
			for (int k = 0; k < animation->m_frameCount; k++)
			{
				srts.push_back(animation->m_skeletonPoses[k].m_jointPoses[i].m_transformation);
			}
		}


		modelManager.m_dynamicMesh[0]->getAnimatedModel()->SetPlayingClip(animation);
		modelManager.m_dynamicMesh[0]->getAnimatedModel()->SetSkeleton(skeleton);
		modelManager.m_dynamicMesh[0]->getAnimatedModel()->Play();

		


		//std::vector<Animation::SRT> srts;
		//{
		//	for (int i = 0; i < animation->m_frameCount; i++)
		//		srts.push_back(animation->m_skeletonPoses[i].m_jointPoses[0].m_transformation);
		//}


	

	PointLight pl;
	pl.Init(DirectX::XMFLOAT4A(0,5,0,1), DirectX::XMFLOAT4A(1,1,1,1), 0.0f);
	//pl.CreateShadowDirection(PointLight::XYZ_ALL);
	pl.CreateShadowDirection(std::vector<PointLight::ShadowDir>({ PointLight::ShadowDir::Y_NEGATIVE, PointLight::ShadowDir::X_NEGATIVE}));
	PointLight pl2;
	std::vector<PointLight> point;

	for (int i = 0; i < 1; i++)
	{
		point.push_back(PointLight());
	}
	//srand(time(0));
	for (int i = 0; i < point.size(); i++)
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

		static float speed = 0.01;
#ifdef _DEBUG
		ImGui::Begin("Animation");
		ImGui::SliderFloat("Speed", &speed, 0.0, 0.5);
		ImGui::End();
#endif
		modelManager.m_dynamicMesh[0]->getAnimatedModel()->Update(speed);


		pl.setPosition(lightPosX, lightPosY, lightPosZ);
		pl.setColor(lightColorR, lightColorG, lightColorB);
		pl.setFarPlane(farPlane);
		pl.setNearPlane(nearPlane);
		
		pl.setIntensity(lightIntensity);
		pl.setDropOff(dropoff);
		pl.setPower(powVar);

		pl2.setPosition(lightPosX1, lightPosY1, lightPosZ1);
		pl2.setColor(lightColorR, lightColorG, lightColorB);
		pl2.setFarPlane(farPlane);
		pl2.setNearPlane(nearPlane);

		pl2.setIntensity(lightIntensity);
		pl2.setDropOff(dropoff);
		pl2.setPower(powVar);


		/*
			Test Camera movement
		*/
		if (InputHandler::isKeyPressed('W'))
			camera.Translate(0.0f, 0.0f, 0.01f);
		else if (InputHandler::isKeyPressed('S'))
			camera.Translate(0.0f, 0.0f, -0.01f);
		if (InputHandler::isKeyPressed('A'))
			camera.Translate(-0.01f, 0.0f, 0.0f);
		else if (InputHandler::isKeyPressed('D'))
			camera.Translate(0.01f, 0.0f, 0.0f);
		if (InputHandler::isKeyPressed(InputHandler::SPACEBAR))
			camera.Translate(0.0f, 0.01f, 0.0f);
		else if (InputHandler::isKeyPressed(InputHandler::Shift))
			camera.Translate(0.0f, -0.01f, 0.0f);

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
		for (int i = 0; i < point.size(); i++)
		{
			//point[i].setPosition((rand() % 20) - 10, 5, (rand() % 20) - 10);
			point[i].QueueLight();
		}
		
		modelManager.DrawMeshes();
		
		renderingManager.Flush(camera);
		renderingManager.Clear();
		
	}
	DX::g_shaderManager.Release();
	renderingManager.Release();

	delete animation;
	delete skeleton;

	
	return 0;
}