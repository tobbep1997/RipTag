#pragma once
#pragma warning(disable : 4244)
#include <chrono>



namespace Timer
{
	static std::chrono::steady_clock::time_point startTime;
	static std::chrono::steady_clock::time_point stopTime;


	static void StartTimer();
	static void StopTimer();

	static std::chrono::milliseconds GetDuration();
	static float GetDurationInSeconds();
}

void Timer::StartTimer()
{
	startTime =  std::chrono::high_resolution_clock::now();
}

void Timer::StopTimer()
{
	stopTime = std::chrono::high_resolution_clock::now();
}

std::chrono::milliseconds Timer::GetDuration()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime);
}

float Timer::GetDurationInSeconds()
{

	float dur = std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count();
	dur = dur / 1000;
	return dur;
}

