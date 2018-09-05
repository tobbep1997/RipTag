#include "SettingsLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

void SettingLoader::LoadWindowSettings(std::string path)
{
	std::ifstream file(path);

	if (file.is_open())
	{
		std::string line;

		if (line[0] != '#' || line != "")
		{
			while (std::getline(file, line))
			{
				std::transform(line.begin(), line.end(), line.begin(), ::tolower);

				std::istringstream stream(line);
				std::string type;

				stream >> type;

				if (type == "fullscreen")
				{
					float t = 0;
					sscanf_s(line.c_str(), " %*s %f", &t);
					if (t == 1)
					{
						//Set True
					}
					else
					{
						//Set false
					}
				}
			}
		}

	}
	else
	{
		OutputDebugString(L"FAILED TO OPEN WINDOW SETTINGS\n");
	}

}
