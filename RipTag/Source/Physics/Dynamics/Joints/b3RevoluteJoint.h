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

#ifndef __B3_REVOLUTE_JOINT_H__
#define __B3_REVOLUTE_JOINT_H__

#include "b3Joint.h"

/*
* A Revolute (or Hinge) Joint.
* The body B will rotate relative to the body A
* about the z-axis of the specified frame on the local space
* of body A.
* It can be usefull to create interesting physical
* object behaviours such doors, platforms, ragdolls, and others.
* I (Irlan) solve each constraint separately but this is 
* probably not the best way to do it.
* @todo_irlan Use direct enumeration (as in Box2D)?
*/

struct b3RevoluteJointDef : b3JointDef {
	b3RevoluteJointDef() {
		//@todo_irlan Is the atan2 function really returning angles on this range?
		lowLimit = -B3_TWO * B3_PI;
		highLimit = B3_TWO * B3_PI;
	}

	virtual b3JointType GetType() const { return b3JointType::e_revoluteJoint; };
	
	b3Transform localFrameA;
	b3Transform localFrameB;
	r32 lowLimit;
	r32 highLimit;
};

class b3RevoluteJoint : public b3Joint {
public :
	enum b3LimitState {
		e_lowerLimit,
		e_upperLimit,
		e_betweenLimits,
	};

	// Get the joint type.
	virtual b3JointType GetType() const { return b3JointType::e_revoluteJoint; }
	
	// Get the local joint frame on body A.
	const b3Transform& GetLocalFrameA() const;	
	
	// Get the local joint frame on body B.
	const b3Transform& GetLocalFrameB() const;
	
	// Set the local joint frames on each body.
	// The hinge axis will be the z vector of the (world) frame A.
	void SetLocalFrames(const b3Transform& localFrameA, const b3Transform& localFrameB);
	
	// Set the angle limits for rotations about the hinge axis.
	void SetAngleLimits(r32 low, r32 high);
protected :
	friend class b3JointGraph;
	friend class b3JointSolver;

	b3RevoluteJoint(const b3RevoluteJointDef* def);
	
	virtual void InitializeVelocityConstraint(const b3SolverData* data);
	virtual void WarmStart(const b3SolverData* data);
	virtual void SolveVelocityConstraint(const b3SolverData* data);

	// The local joint frames on each body.
	b3Transform m_localFrameA;
	b3Transform m_localFrameB;
	// The angle limits.
	r32 m_low;
	r32 m_high;

	// Relative revolute velocity constraint data.
	b3Vec3 m_u2xw1;
	r32 m_invMass1;
	r32 m_velocityBias1;
	r32 m_accLambda1;

	b3Vec3 m_v2xw1;
	r32 m_invMass2;
	r32 m_velocityBias2;
	r32 m_accLambda2;

	// Revolute joint axis angle limits velocity constraint data.
	b3LimitState m_limitState;
	b3Vec3 m_w1;
	r32 m_velocityBias3;
	r32 m_accLambda3;

	// Point-to-point velocity constraint data.
	b3Mat33 m_invMass4;
	b3Vec3 m_rA;
	b3Vec3 m_rB;
	b3Vec3 m_velocityBias4;
	b3Vec3 m_accImpulse4;
};

inline const b3Transform& b3RevoluteJoint::GetLocalFrameA() const {
	return m_localFrameA;
}

inline const b3Transform& b3RevoluteJoint::GetLocalFrameB() const {
	return m_localFrameB;
}

inline void b3RevoluteJoint::SetLocalFrames(const b3Transform& localFrameA, const b3Transform& localFrameB) {
	m_localFrameA = localFrameA;
	m_localFrameB = localFrameB;
}

inline void b3RevoluteJoint::SetAngleLimits(r32 low, r32 high) {
	b3Assert(low < high);
	m_low = low;
	m_high = high;
}

#endif
