#include "EnginePCH.h"
#include "Timer.h"
std::chrono::steady_clock::time_point Timer::startTime;
std::chrono::steady_clock::time_point Timer::stopTime;

Timer::Timer()
{
}

Timer::~Timer()
{
}

Timer& Timer::Instance()
{
	static Timer inst;
	return inst;
}

void Timer::StartTimer()
{
	startTime = std::chrono::high_resolution_clock::now();
}

void Timer::StopTimer()
{
	stopTime = std::chrono::high_resolution_clock::now();
}

float Timer::GetDurationInSeconds()
{
	stopTime = std::chrono::high_resolution_clock::now();
	float dur = std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count();
	dur = dur / 1000;
	return dur;
}