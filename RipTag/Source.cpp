#include <Windows.h>
#include "Source/3D Engine/RenderingManager.h"
#include "Source/Shader/ShaderManager.h"

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

	Shaders::ShaderManager sm;

	ID3D11VertexShader * vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	vs = sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	sm.UnloadShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	sm.UnloadShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	sm.UnloadShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	sm.UnloadShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	sm.UnloadShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	sm.UnloadShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	sm.UnloadShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	sm.UnloadShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	sm.UnloadShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	sm.UnloadShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	sm.UnloadShader(L"../Engine/Source/Shader/VertexShader.hlsl");

	while (renderingManager.GetWindow().isOpen())
	{
		renderingManager.Update();
	}
	
	sm.Release();
	renderingManager.Release();
	return 0;
}