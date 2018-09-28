#pragma once
#include <chrono>

class DeltaTime
{
	
private: 
	std::chrono::time_point <std::chrono::steady_clock> preTime;
	std::chrono::time_point <std::chrono::steady_clock> currentTime;
public:
	DeltaTime();
	~DeltaTime();

	void Init();
	

	float getDeltaTimeInSeconds();
};

