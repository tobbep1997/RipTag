#pragma once
#include <string>
#include <Windows.h>

namespace SettingLoader
{
	void LoadWindowSettings(std::string path);
	void SaveWindowSettings(std::string path);
}