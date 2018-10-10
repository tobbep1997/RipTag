#pragma once
#pragma warning(disable : 4244)
#include <chrono>

class Timer
{
private:
	static std::chrono::steady_clock::time_point startTime;
	static std::chrono::steady_clock::time_point stopTime;

	Timer();
	~Timer();
public:

	static Timer& Instance();

	static void StartTimer();
	static void StopTimer();

	static float GetDurationInSeconds();
};

//namespace Timer
//{
//	static std::chrono::steady_clock::time_point startTime;
//	static std::chrono::steady_clock::time_point stopTime;
//
//
//	static void StartTimer();
//	static void StopTimer();
//
//	static std::chrono::milliseconds GetDuration();
//	static float GetDurationInSeconds();
//}
//
//void Timer::StartTimer()
//{
//	startTime =  std::chrono::high_resolution_clock::now();
//}
//
//void Timer::StopTimer()


