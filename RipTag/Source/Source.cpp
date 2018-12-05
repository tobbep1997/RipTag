#include "RipTagPCH.h"
#include "Game/Game.h"
#include "EngineSource/Helper/Timer.h"
#include <AudioEngine.h>

#include "EngineSource/Shader/ShaderManager.h"
#include "CheetConsole/CheetParser.h"

//Allocates memory to the console
bool cakeIsALie = true;
CheetParser * parser;
void _alocConsole() {
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	std::string x;
	while (cakeIsALie)
	{
		//system("CLS");
		std::cout << "Enter Command" << std::endl;
		std::cin >> x;
		CheetParser::ParseString(x);
	}
}

void _CrtSetDbg() {
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}


struct SoundSettings {
	float master, effects, ambient, music;
};

SoundSettings _ReadSettingsFromFile()
{
	SoundSettings ss = {100,100,100,100};
	std::string file[1] = { "../Configuration/AudioSettings.ini" };
	std::string names[1] = { "Audio" };
	for (int k = 0; k < 1; k++)
	{
		const int bufferSize = 1024;
		char buffer[bufferSize];

		//Load in Keyboard section
		if (GetPrivateProfileStringA(names[k].c_str(), NULL, NULL, buffer, bufferSize, file[k].c_str()))
		{
			std::vector<std::string> nameList;
			std::istringstream nameStream;
			nameStream.str(std::string(buffer, bufferSize));

			std::string name = "";
			while (std::getline(nameStream, name, '\0'))
			{
				if (name == "")
					break;
				nameList.push_back(name);
			}

			for (size_t i = 0; i < nameList.size(); i++)
			{
				int key = -1;
				key = GetPrivateProfileIntA(names[k].c_str(), nameList[i].c_str(), -1, file[k].c_str());
				if (key != -1)
				{
					if (nameList[i] == "Master")
					{
						ss.master = key;
					}
					else if (nameList[i] == "Effects")
					{
						ss.effects = key;
					}
					else if (nameList[i] == "Ambient")
					{
						ss.ambient = key;
					}
					else if (nameList[i] == "Music")
					{
						ss.music = key;
					}
				}

			}
			//Clear buffer for reuse
			ZeroMemory(buffer, bufferSize);

		}
		else
			std::cout << GetLastError() << std::endl;
	}
	return ss;
}


void GameLoop(Game * game)
{
	DeltaTime dt;
	float deltaTime = 0.0f;
	float deltaNega = 0;
	while (game->isRunning())
	{
		InputHandler::Reset();
		deltaTime = dt.getDeltaTimeInSeconds();
		if (deltaTime > 1.0f)
			deltaTime = 1 / 60.0f;

		//This is to avoid Pollevents from fucking with the game
		game->Clear();

		//Pollevents

		//Draw and update
		game->Update(deltaTime);
		AudioEngine::Update();
		game->Draw();
	}
}

void SingleGameLoop(Game * game)
{
	DeltaTime dt;
	float deltaTime = 0.0f;
	float deltaNega = 0;

	while (game->isRunning())
	{
		InputHandler::Reset();
	
		deltaTime = dt.getDeltaTimeInSeconds();
		if (deltaTime > 1.0f)
			deltaTime = 1 / 60.0f;
		game->PollSingelThread();

		//This is to avoid Pollevents from fucking with the game
		game->Clear();
		///-------------------

		///-------------------

		//Pollevents

		//Draw and update
		game->Update(deltaTime);
		AudioEngine::Update();
		game->Draw();
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	parser = CheetParser::GetInstance();
	bool consoleEnabled = true;
	std::thread console;
	if (consoleEnabled)
	{
		console = std::thread(_alocConsole);
	}
	
#if _DEBUG
	
	_CrtSetDbg();
#endif
#if _RELEASE_DBG
	_alocConsole();
	_CrtSetDbg();
#endif



    AudioEngine::Init();
	SoundSettings ss = _ReadSettingsFromFile();
	AudioEngine::SetMasterVolume(ss.master / 100.0f);
	AudioEngine::SetEffectVolume(ss.effects / 100.0f);
	AudioEngine::SetAmbientVolume(ss.ambient / 100.0f);
	AudioEngine::SetMusicVolume(ss.music / 100.0f);
	srand(time(0));


	Game game;
	game.Init(hInstance);

	SingleGameLoop(&game);
#ifdef _DEBUG
#else
	/*std::thread gameLooop = std::thread(&GameLoop, &game);
	game.PollEvents();
	if (gameLooop.joinable())
		gameLooop.join();*/
#endif

	
	DX::g_shaderManager.Release();
	FontHandler::Release();
	AudioEngine::Release();
	if (consoleEnabled)
	{
		cakeIsALie = false;
		console.join();
	}
	
	return 0;
}