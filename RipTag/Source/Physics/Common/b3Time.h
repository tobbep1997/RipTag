/*
* Copyright (c) 2015-2015 Irlan Robson http://www.irlans.wordpress.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef __B3_TIME_H__
#define __B3_TIME_H__

#include "Math\b3Math.h"
#include "b3Settings.h"

struct b3Velocity {
	b3Vec3 v;
	b3Vec3 w;
};

struct b3Position {
	b3Vec3 x;
	b3Quaternion q;
};

struct b3SolverData {
	r32 invdt;
	b3Position* positions;
	b3Velocity* velocities;
};

// This is used to define the simulation configuration.
struct b3TimeStep {
	r32 dt;
	u32 velocityIterations;
	bool sleeping;
};

// Call b3World::GetProfile() to inspect the time
// spend for running a single simulation module.
struct b3StepProfile {
	r64 broadPhaseTime;
	r64 narrowPhaseTime;
	r64 solverTime;
	r64 totalTime;
};

// Convert microseconds to seconds.
inline r32 MicrosToSecs(r32 us) {
	return (B3_ONE / r32(B3_ONE_SECOND_MICROSECONDS)) * us;
}

// Convert microseconds to miliseconds.
inline r32 MicrosToMilisecs(r32 us) {
	return (B3_ONE / r32(B3_ONE_MILISECOND_MICROSECONDS)) * us;
}

// A (very) precise time class.
class b3Time {
public :
	b3Time();
	
	static u64 GetRealTime();

	// Set B3_ONE_SECOND_MICROSECONDS to use as a normal timer.
	void SetResolution(u64 _ui64Resolution);
	
	u64 GetCurMicros() const;
	u64 GetDeltaMicros() const;
	r32 GetCurSecs() const;
	r32 GetDeltaSecs() const;
	r32 GetDeltaMilisecs() const;

	void Update();
	void UpdateBy(u64 _ui64Delta);
protected :
	u64 m_frequency;
	u64 m_lastRealTime;
	u64 m_curTime;
	u64 m_lastTime;
	u64 m_curMicros;
	u64 m_deltaMicros;
};

inline void b3Time::SetResolution(u64 _ui64Resolution) {
	m_frequency = _ui64Resolution;
}

inline u64 b3Time::GetCurMicros() const { 
	return m_curMicros; 
}

inline u64 b3Time::GetDeltaMicros() const { 
	return m_deltaMicros; 
}

inline r32 b3Time::GetCurSecs() const { 
	return MicrosToSecs(r32(m_curMicros)); 
}

inline r32 b3Time::GetDeltaSecs() const { 
	return MicrosToSecs(r32(m_deltaMicros)); 
}

inline r32 b3Time::GetDeltaMilisecs() const { 
	return MicrosToMilisecs(r32(m_deltaMicros)); 
}

#endif
