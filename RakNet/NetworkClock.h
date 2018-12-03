#pragma once
#include <chrono>

namespace Network
{
	class ChronoClock
	{
	public:
		ChronoClock()
		{
			this->m_isRunning = false;
			this->m_value = 0;
		}
		~ChronoClock(){}

		double restart()
		{
			double elapsed = 0.0;
			elapsed = this->getElapsedTime();
			this->m_value = 0.0;
			this->start();
		}

		void start()
		{
			this->m_start = std::chrono::steady_clock::now();
			if (!this->m_isRunning)
				this->m_isRunning = true;
		}

		void pause()
		{
			if (this->m_isRunning)
			{
				this->m_value += this->getElapsedTime();
				this->m_isRunning = false;
			}
		}

		void toggle()
		{
			if (!this->m_isRunning)
				this->start();
			else
				this->pause();
		}

		double getElapsedTime()
		{
			if (this->m_isRunning)
			{
				this->m_current = std::chrono::steady_clock::now();
				std::chrono::duration<double, std::milli> duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(this->m_current - this->m_start);
				return (duration.count() + this->m_value);
			}
			else
				return this->m_value;
		}

		bool isRunning() { return this->m_isRunning; }

	private:
		std::chrono::steady_clock::time_point m_start;
		std::chrono::steady_clock::time_point m_current;

		bool m_isRunning;
		double m_value;
	};
}
