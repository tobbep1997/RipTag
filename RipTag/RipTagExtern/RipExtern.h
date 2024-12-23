#pragma once
#include <DirectXMath.h>

class b3World;
class ContactListener;
class RayCastListener;
class ParticleSystem;

namespace RipSounds
{
	extern std::vector<std::string> g_sneakStep;
	extern std::vector<std::string> g_hardStep;
	extern std::vector<std::string> g_armorStepsStone;
	extern std::string				g_leverActivate;
	extern std::string				g_leverDeactivate;
	extern std::string				g_pressurePlateActivate;
	extern std::string				g_pressurePlateDeactivate;
	extern std::string				g_torch;
	extern std::string				g_windAndDrip;
	extern std::string				g_phase;
	extern std::string				g_music1;
	extern std::string				g_grunt;
	extern std::string				g_playAmbientSound;
	extern std::string				g_metalDoorOpening;
	extern std::string				g_metalDoorClosening;
	extern std::string				g_metalDoorClosed;
	extern std::string				g_smokeBomb;
	extern std::string				g_gateClosed;
	extern std::string				g_gateClosening;
	extern std::string				g_gateOpend;
	extern std::string				g_gateOpening;
	extern std::string				g_teleport; 
	extern std::string				g_teleportHit; 
}

namespace RipExtern
{
	extern b3World * g_world;
	extern ParticleSystem   * g_particleSystem;
	extern ContactListener	* g_contactListener;
	extern RayCastListener	* g_rayListener;
	extern bool g_kill;

	static bool BoolReverser(const bool & first)
	{
		if (first)
		{
			return false;
		}
		else
		{
			return true;
		}
	};
}

class XMMATH
{
public:
	static DirectX::XMFLOAT4A add(const DirectX::XMFLOAT4A & a, const DirectX::XMFLOAT4A & b)
	{
		DirectX::XMFLOAT4A re;
		
		DirectX::XMVECTOR A, B, C;
		A = DirectX::XMLoadFloat4A(&a);
		B = DirectX::XMLoadFloat4A(&b);
		C = DirectX::XMVectorAdd(A, B);
		DirectX::XMStoreFloat4A(&re, C);

		return re;
	}
	static DirectX::XMFLOAT4A subtract(const DirectX::XMFLOAT4A & a, const DirectX::XMFLOAT4A & b)
	{
		DirectX::XMFLOAT4A re;

		DirectX::XMVECTOR A, B, C;
		A = DirectX::XMLoadFloat4A(&a);
		B = DirectX::XMLoadFloat4A(&b);
		C = DirectX::XMVectorSubtract(A, B);
		DirectX::XMStoreFloat4A(&re, C);

		return re;
	}
	static DirectX::XMFLOAT4A multiply(const DirectX::XMFLOAT4A & a, const DirectX::XMFLOAT4A & b)
	{
		DirectX::XMFLOAT4A re;

		DirectX::XMVECTOR A, B, C;
		A = DirectX::XMLoadFloat4A(&a);
		B = DirectX::XMLoadFloat4A(&b);
		C = DirectX::XMVectorMultiply(A, B);
		DirectX::XMStoreFloat4A(&re, C);

		return re;
	}
	static DirectX::XMFLOAT4A divide(const DirectX::XMFLOAT4A & a, const DirectX::XMFLOAT4A & b)
	{
		DirectX::XMFLOAT4A re;

		DirectX::XMVECTOR A, B, C;
		A = DirectX::XMLoadFloat4A(&a);
		B = DirectX::XMLoadFloat4A(&b);
		C = DirectX::XMVectorDivide(A, B);
		DirectX::XMStoreFloat4A(&re, C);

		return re;
	}
	static DirectX::XMFLOAT4A scale(const DirectX::XMFLOAT4A & a, float scale)
	{
		DirectX::XMFLOAT4A re;

		DirectX::XMVECTOR A, C;
		A = DirectX::XMLoadFloat4A(&a);
		C = DirectX::XMVectorScale(A, scale);
		DirectX::XMStoreFloat4A(&re, C);

		return re;
	}
};

namespace ConstTimer
{

	#include <Windows.h>
	#include <stdio.h>
	typedef float real32;
	typedef double real64;

	class MTimer
	{
	public:
		enum TIME_UNIT
		{
			SECONDS = 1,
			MILLISECONDS = 1000,
			MICROSECONDS = 1000000
		};
	private:
		LARGE_INTEGER m_StartingTime;
		LARGE_INTEGER m_EndingTime;
		LARGE_INTEGER m_Frequency;
	public:
		void Start()
		{
			QueryPerformanceFrequency(&m_Frequency);
			QueryPerformanceCounter(&m_StartingTime);
		}
		double Stop(TIME_UNIT tu = SECONDS)
		{
			QueryPerformanceCounter(&m_EndingTime);
			LARGE_INTEGER ElapsedMicroseconds;
			ElapsedMicroseconds.QuadPart = m_EndingTime.QuadPart - m_StartingTime.QuadPart;
			double MSPerFrame = ((real64)ElapsedMicroseconds.QuadPart / (real64)m_Frequency.QuadPart);
			m_StartingTime = m_EndingTime;
			return MSPerFrame * tu;
		}
		double GetTime(TIME_UNIT tu = SECONDS)
		{
			QueryPerformanceCounter(&m_EndingTime);
			LARGE_INTEGER ElapsedMicroseconds;
			ElapsedMicroseconds.QuadPart = m_EndingTime.QuadPart - m_StartingTime.QuadPart;
			double MSPerFrame = ((real64)ElapsedMicroseconds.QuadPart / (real64)m_Frequency.QuadPart);
			return MSPerFrame * tu;
		}
	};

	extern MTimer g_blinkTimer;
	extern MTimer g_teleportTimer;
}