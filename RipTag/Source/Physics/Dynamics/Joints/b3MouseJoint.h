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

#ifndef __B3_MOUSE_JOINT_H__
#define __B3_MOUSE_JOINT_H__

#include "b3Joint.h"

/*
* A mouse joint is helpfull when the user wants
* to control a body using the mouse world space position
* after having performed a ray cast on it.
* This is more accurately than applying a force on the object proportional 
* to the distance between the ray cast intersection point 
* and the mouse position and using spring damper factors.
* The constraint equation is the same as the ball-in-socket (or spherical) joint
* but the first body has infinite mass. 
* Therefore we keep stored the local point on body B and the world point of the mouse (body A).
*/

struct b3MouseJointDef : b3JointDef {
	b3MouseJointDef() { }
	virtual b3JointType GetType() const { return b3JointType::e_mouseJoint; };
	
	b3Vec3 worldAnchorA;
	b3Vec3 localAnchorB;
};

class b3MouseJoint : public b3Joint {
public : 
	// Get the joint type.
	virtual b3JointType GetType() const { return b3JointType::e_mouseJoint; };
	
	// Get the world space anchor point on the first body (usually the mouse world space position).
	const b3Vec3& GetWorldAnchorA() const;
	
	// Set the world space anchor position on the first body.
	void SetWorldAnchorA(const b3Vec3& v);
	
	// Get the local space anchor point on the second body (usually the ray cast intersection).
	const b3Vec3& GetLocalAnchorB() const;

	// Set the mouse position on the space of the second body (usually the ray cast intersection).
	void SetLocalAnchorB(const b3Vec3& v);
protected:
	friend class b3JointGraph;
	friend class b3JointSolver;

	b3MouseJoint(const b3MouseJointDef* def);

	virtual void InitializeVelocityConstraint(const b3SolverData* data);
	virtual void WarmStart(const b3SolverData* data);
	virtual void SolveVelocityConstraint(const b3SolverData* data);

	// The two anchor points on each body.
	// The first body has infinite mass. Therefore,
	// we store the world space anchor point.
	b3Vec3 m_worldAnchorA;
	b3Vec3 m_localAnchorB;

	// Temporary velocity constraint data for the joint solver.
	u32 m_indexB;
	r32 m_mB;
	b3Mat33 m_iB;
	b3Mat33 m_invMass;
	b3Vec3 m_rB;
	b3Vec3 m_velocityBias;
	b3Vec3 m_accImpulse;
};

inline const b3Vec3& b3MouseJoint::GetWorldAnchorA() const {
	return m_worldAnchorA;
}

inline const b3Vec3& b3MouseJoint::GetLocalAnchorB() const {
	return m_localAnchorB;
}

inline void b3MouseJoint::SetWorldAnchorA(const b3Vec3& v) {
	m_worldAnchorA = v;
}

inline void b3MouseJoint::SetLocalAnchorB(const b3Vec3& v) {
	m_localAnchorB = v;
}

#endif
