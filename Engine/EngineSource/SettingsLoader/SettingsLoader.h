#pragma once
#include <string>
#include <Windows.h>

struct WindowContext;

namespace SettingLoader
{
	void LoadWindowSettings(WindowContext & wind, std::string path = "../Configuration/defultEngineSettings.ini");
	void SaveWindowSettings(std::string path = "../Configuration/defultEngineSettings.ini");
}
