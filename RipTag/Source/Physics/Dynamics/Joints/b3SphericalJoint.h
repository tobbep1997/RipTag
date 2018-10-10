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

#ifndef __B3_SPHERICAL_JOINT_H__
#define __B3_SPHERICAL_JOINT_H__

#include "b3Joint.h"

/*
* A ball-in-socket (or spherical) joint removes the relative velocity
* between two anchor points. Set these anchor points on 
* the local space of each body.
*/

struct b3SphericalJointDef : b3JointDef {
	b3SphericalJointDef() { }

	virtual b3JointType GetType() const { return b3JointType::e_sphericalJoint; };

	b3Vec3 localAnchorA;
	b3Vec3 localAnchorB;
};

class b3SphericalJoint : public b3Joint {
public :
	// Get the joint type.
	virtual b3JointType GetType() const { return b3JointType::e_sphericalJoint; }
	
	// Get the local anchor point in the local space of the first body.
	const b3Vec3& GetLocalAnchorA() const;

	// Get the local anchor point in the local space of the second body.
	const b3Vec3& GetLocalAnchorB() const;
protected :
	friend class b3JointGraph;
	friend class b3JointSolver;

	b3SphericalJoint(const b3SphericalJointDef* def);

	virtual void InitializeVelocityConstraint(const b3SolverData* data);
	virtual void WarmStart(const b3SolverData* data);
	virtual void SolveVelocityConstraint(const b3SolverData* data);

	// The two anchor points on the local space of each body.
	b3Vec3 m_localAnchorA;
	b3Vec3 m_localAnchorB;

	// Temporary velocity constraint data for the joint solver (per joint).
	b3Vec3 m_rA;
	b3Vec3 m_rB;
	b3Mat33 m_invMass;
	b3Vec3 m_velocityBias;
	b3Vec3 m_accImpulse;
};

inline const b3Vec3& b3SphericalJoint::GetLocalAnchorA() const {
	return m_localAnchorA;
}

inline const b3Vec3& b3SphericalJoint::GetLocalAnchorB() const {
	return m_localAnchorB;
}

#endif
