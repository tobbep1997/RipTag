#pragma once

namespace Threading
{
	static unsigned int amountOfThreads;
	static unsigned int currentThreadsInUse;

	void Init();

	
	unsigned int GetThreadsInUse();
	unsigned int GetCPUSupportedThreads();

	void StartThread();
	void EndThread();
}