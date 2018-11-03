#include "EnginePCH.h"
#include "Threading.h"


void Threading::Init()
{
	amountOfThreads = std::thread::hardware_concurrency();
	currentThreadsInUse = 0;
}

unsigned int Threading::GetThreadsInUse()
{
	return currentThreadsInUse;
}

unsigned Threading::GetCPUSupportedThreads()
{
	//This is only meant as a hint
	return amountOfThreads;
}

void Threading::StartThread()
{
	currentThreadsInUse++;
}

void Threading::EndThread()
{
	currentThreadsInUse--;
}
