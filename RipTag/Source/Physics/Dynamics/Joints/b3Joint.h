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

#ifndef __B3_JOINT_H__
#define __B3_JOINT_H__

#include "..\..\Common\Math\b3Math.h"

class b3Body;
class b3Joint;
struct b3SolverData;

enum b3JointType {
	e_mouseJoint,
	e_sphericalJoint,
	e_revoluteJoint
};

struct b3JointDef {
	b3JointDef() {
		bodyA = nullptr;
		bodyB = nullptr;
		userData = nullptr;
	}
	
	virtual b3JointType GetType() const = 0;
	
	b3Body* bodyA;
	b3Body* bodyB;
	void* userData;
};

// The joint edge for the joint.
struct b3JointEdge {
	b3Joint* joint;
	b3Body* other;
	b3JointEdge* prev;
	b3JointEdge* next;
};

// @note All the data members of the joint structure they will be initialized
// by the joint graph.
class b3Joint {
public :
	// Get the joint type.
	virtual b3JointType GetType() const = 0;

	// Get the first body connected to the joint.
	b3Body* GetBodyA();
	const b3Body* GetBodyA() const;

	// Set the body to be connected to the joint as the first body.
	virtual void SetBodyA(b3Body* bodyA);

	// Get the second body connected to the joint.
	b3Body* GetBodyB();
	const b3Body* GetBodyB() const;
	
	// Set the body to be connected to the joint as the second body.
	virtual void SetBodyB(b3Body* bodyB);

	// Set the user data to be associated with the joint.
	void SetUserData(void* data);

	// Get the user specific data associated with the joint.
	void* GetUserData();
	const void* GetUserData() const;
protected :
	friend class b3Body;
	friend class b3World;
	friend class b3JointGraph;
	friend class b3JointSolver;

	b3Joint() {	}
	virtual ~b3Joint() { }
	
	virtual void InitializeVelocityConstraint(const b3SolverData* data) = 0;
	virtual void WarmStart(const b3SolverData* data) = 0;
	virtual void SolveVelocityConstraint(const b3SolverData* data) = 0;

	b3Body* m_bodyA;
	b3Body* m_bodyB;
	
	// User specific data for the joint.
	void* m_userData;

	// Is the joint on the island?
	bool m_onIsland;

	b3JointEdge m_nodeA;
	b3JointEdge m_nodeB;

	// Pointers to the doubly-linked list of joints.
	b3Joint* m_prev;
	b3Joint* m_next;

	// Temporary data copied from the joint solver.
	// to avoid cache misses.
	// @note Don't duplicate the data when 
	// inheriting from the class.
	u32 m_indexA;
	u32 m_indexB;
	r32 m_mA;
	r32 m_mB;
	b3Mat33 m_iA;
	b3Mat33 m_iB;
};

inline void b3Joint::SetBodyA(b3Body* bodyA) {
	m_bodyA = bodyA;
}

inline b3Body* b3Joint::GetBodyA() {
	return m_bodyA;
}

inline const b3Body* b3Joint::GetBodyA() const {
	return m_bodyA;
}

inline void b3Joint::SetBodyB(b3Body* bodyB) {
	m_bodyB = bodyB;
}

inline b3Body* b3Joint::GetBodyB() {
	return m_bodyB;
}

inline const b3Body* b3Joint::GetBodyB() const {
	return m_bodyB;
}

inline void b3Joint::SetUserData(void* data) {
	m_userData = data;
}

inline void* b3Joint::GetUserData() {
	return m_userData;
}

inline const void* b3Joint::GetUserData() const {
	return m_userData;
}

#endif
