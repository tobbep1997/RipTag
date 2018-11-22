#pragma once
#include <string>
#include <Windows.h>

struct WindowContext;

namespace SettingLoader
{
	extern WindowContext * g_windowContext;
	void LoadWindowSettings(WindowContext & wind, std::string path = "../Configuration/defultEngineSettings.ini");
	void SaveWindowSettings(std::string path = "../Configuration/defultEngineSettings.ini");
}
