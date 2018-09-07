#include <Windows.h>
#include "Source/3D Engine/RenderingManager.h"
#include "Source/Shader/ShaderManager.h"
#include "Source/3D Engine/Model/Model.h"
#pragma comment(lib, "../x64/Debug/New_Library.lib")

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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
#if _DEBUG
	_alocConsole();
#endif

	RenderingManager renderingManager;

	renderingManager.Init(hInstance);

	Model m(ObjectType::Static);
	Model m2;
	m.SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	m.SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	m2.SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	m2.SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");

	m.setPosition(1, 0, 0);
	m2.setPosition(-1, 0, 0);
	StaticMesh * s = new StaticMesh();
	s->SET_DEFAULT();
	m.SetModel(s);
	m2.SetModel(s);

	while (renderingManager.getWindow().isOpen())
	{
		renderingManager.Update();
		m.Draw();
		m2.Draw();
		renderingManager.Flush();
	}
	DX::g_shaderManager.Release();
	renderingManager.Release();

	delete s;
	return 0;
}