#pragma once
#include <string>
#include <Windows.h>
#include "../Window/window.h"

namespace SettingLoader
{
	void LoadWindowSettings(WindowContext & wind, std::string path = "defultEngineSettings.ini");
	void SaveWindowSettings(std::string path = "defultEngineSettings.ini");
}
