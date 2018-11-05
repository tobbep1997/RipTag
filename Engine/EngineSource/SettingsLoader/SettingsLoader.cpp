#include "EnginePCH.h"
#include "SettingsLoader.h"

void SettingLoader::LoadWindowSettings(WindowContext & wind,std::string path)
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
						wind.fullscreen = true;
					}
					else
					{
						wind.fullscreen = false;
					}
				}
				else if(type == "width")
				{
					float t = 0;
					sscanf_s(line.c_str(), " %*s %f", &t);
					wind.clientWidth = (UINT)t;
				}
				else if(type == "height")
				{
					float t = 0;
					sscanf_s(line.c_str(), " %*s %f", &t);
					wind.clientHeight = (UINT)t;
				}
			}
		}

	}
	else
	{
		OutputDebugString(L"FAILED TO OPEN WINDOW SETTINGS\n");
	}

}

void SettingLoader::SaveWindowSettings(std::string path)
{
	std::ofstream file(path);

	
	if (file.is_open())
	{

		//file << "temp";
	}
	else
	{
		OutputDebugString(L"FAILED TO OPEN WINDOW SETTINGS\n");
		return;
	}

	file.close();
}
