#include "EnginePCH.h"
#include "SettingsLoader.h"

void SettingLoader::LoadWindowSettings(WindowContext & wind,std::string path)
{
	const int bufferSize = 1024;
	char buffer[bufferSize];

	//Load in Keyboard section
	if (GetPrivateProfileStringA("Engine", NULL, NULL, buffer, bufferSize, path.c_str()))
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
			key = GetPrivateProfileIntA("Engine", nameList[i].c_str(), -1, path.c_str());
			if (key != -1)
			{
				if (nameList[i] == "fullscreen")
				{
					wind.fullscreen = key;
				}
				else if (nameList[i] == "width")
				{
					wind.clientWidth = (UINT)key;
				}
				else if (nameList[i] == "height")
				{
					wind.clientHeight = (UINT)key;
				}
			}

		}
		//Clear buffer for reuse
		ZeroMemory(buffer, bufferSize);

	}
	else
		std::cout << GetLastError() << std::endl;



	/*std::ifstream file(path);

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
	}*/

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
