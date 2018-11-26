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

#ifndef __B3_WORLD_H__
#define __B3_WORLD_H__

#include "..\Common\Memory\b3StackAllocator.h"
#include "..\Common\Memory\b3BlockAllocator.h"
#include "..\Common\b3Time.h"
#include "Contacts\b3ContactGraph.h"
#include "Joints\b3JointGraph.h"

struct b3BodyDef;
class b3Body;
class b3QueryListener;
class b3RayCastListener;
class b3ContactListener;
class b3Draw;

class b3World {
public :
	// Use a physics world to create/destroy (rigid) bodies, execute ray cast and AABB queries.
	b3World();
	~b3World();

	// Allocates a new rigid body and insert it into the world.
	b3Body* CreateBody(const b3BodyDef& def);
	
	// Remove a rigid body from the world and deallocate it from the memory.
	void DestroyBody(b3Body* body);
	
	// Allocate a new joint and insert it into the world.
	b3Joint* CreateJoint(const b3JointDef& def);

	// Remove a joint from the world and deallocate it from the memory.
	void DestroyJoint(b3Joint* joint);

	// Set the gravity direction (usually y-down (0, -1, 0)).
	void SetGravityDirection(const b3Vec3& direction);

	// The contact listener passed will be notified when two body shapes begin/stays/ends
	// touching with each other.
	void SetContactListener(b3ContactListener* listener);

	// The query listener will be notified when two shape AABBs are overlapping.
	// If the listener returns false then the query is stopped immediately.
	// Otherwise, it continues searching for new overlapping shape AABBs.
	void QueryAABB(b3QueryListener* listener, const b3AABB& aabb) const;
	
	// The ray cast listener will be notified when a ray intersects a world shape.
	// The ray cast output is the intercepted shape, the intersection point, normal, and fraction.
	void RayCast(b3RayCastListener* listener, const b3Vec3& p1, const b3Vec3& p2) const;

	// Call the function below to simulate a physics frame.
	// The function parameter is the time step definition, which must contain 
	// the simulation frequency (dt), the number of contact solver iterations, and 
	// a flag that tells if the bodies should sleep when they're not moving significantly.
	void Step(const b3TimeStep& step);

	// Return the time spend to execute each simulation module of last physics step.
	const b3StepProfile& GetStepProfile() const;
	
	// How many bodies are on the world?
	u32 GetBodyCount() const;

	// How many joints are on the world?
	u32 GetJointCount() const;

	// How many contacts are created on the world?
	u32 GetContactCount() const;

	// Call the function below to debug the physics world.
	// The user must implement the b3Draw interface.
	void Draw(const b3Draw* draw, u32 flags) const;
	const b3Body* getBodyList() const;
protected :
	enum b3WorldFlags {
		e_bodyAddedFlag = 0x0001,
		e_clearForcesFlag = 0x0002,
	};

	friend class b3Body;

	void ClearForces();
	void Solve(const b3TimeStep& step);
	
	b3StackAllocator m_stackAllocator;
	b3BlockAllocator m_blockAllocator;
	b3ContactGraph m_contactGraph;
	b3JointGraph m_jointGraph;
	u32 m_flags;
	b3Vec3 m_gravityDir;
	b3Body* m_bodyList;
	u32 m_bodyCount;
	b3StepProfile m_profile;
};

inline const b3StepProfile& b3World::GetStepProfile() const {
	return m_profile;
}

inline void b3World::SetGravityDirection(const b3Vec3& direction) {
	m_gravityDir = direction;
}

inline void b3World::SetContactListener(b3ContactListener* listener) {
	m_contactGraph.m_contactListener = listener;
}

inline u32 b3World::GetBodyCount() const {
	return m_bodyCount;
}

inline u32 b3World::GetJointCount() const {
	return m_jointGraph.m_jointCount;
}

inline u32 b3World::GetContactCount() const {
	return m_contactGraph.m_contactCount;
}

inline const b3Body* b3World::getBodyList() const {
	return m_bodyList;
}

#endif
