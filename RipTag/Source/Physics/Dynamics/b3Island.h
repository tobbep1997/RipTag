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

#ifndef __B3_ISLAND_H__
#define __B3_ISLAND_H__

#include "..\Common\Math\b3Math.h"

class b3StackAllocator;
class b3Contact;
class b3Joint;
class b3Body;
struct b3Velocity;
struct b3Position;
struct b3TimeStep;

struct b3IslandDef {
	r32 dt;
	bool allowSleep;
	u32 velocityIterations;
	b3StackAllocator* allocator;
	u32 bodyCapacity;
	u32 contactCapacity;
	u32 jointCapacity;
};

class b3Island {
public :
	b3Island(const b3IslandDef& def);
	~b3Island();

	void Reset();
	void Add(b3Body* b);
	void Add(b3Contact* c);
	void Add(b3Joint* j);
	void Solve(const b3Vec3& gravity);
protected :
	friend class b3World;

	bool allowSleep;
	r32 dt;
	u32 velocityIterations;

	b3Body** bodies;
	u32 bodyCount;
	u32 bodyCapacity;

	b3Contact** contacts;
	u32 contactCount;	
	u32 contactCapacity;

	b3Joint** joints;
	u32 jointCount;
	u32 jointCapacity;
	
	b3StackAllocator* allocator;
	b3Position* positions;
	b3Velocity* velocities;
};

#endif
