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

#include "b3SphericalJoint.h"
#include "..\b3Body.h"
#include "..\..\Common\b3Time.h"

b3SphericalJoint::b3SphericalJoint(const b3SphericalJointDef* def) {
	m_bodyA = def->bodyA;
	m_bodyB = def->bodyB;
	m_userData = def->userData;
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;
	m_mA = B3_ZERO;
	m_mB = B3_ZERO;
}

void b3SphericalJoint::InitializeVelocityConstraint(const b3SolverData* data) {
	u32 indexA = m_bodyA->m_islandID;
	u32 indexB = m_bodyB->m_islandID;

	r32 mA = m_bodyA->m_invMass;
	r32 mB = m_bodyB->m_invMass;

	b3Mat33 iA = m_bodyA->m_invWorldInertia;
	b3Mat33 iB = m_bodyB->m_invWorldInertia;

	r32 frequency = data->invdt;
	b3Vec3 xA = data->positions[indexA].x;
	b3Vec3 xB = data->positions[indexB].x;

	b3Transform transformA = m_bodyA->m_transform;
	b3Transform transformB = m_bodyB->m_transform;

	b3Vec3 pA = transformA * m_localAnchorA;
	b3Vec3 pB = transformB * m_localAnchorB;

	b3Vec3 rA = pA - xA;
	b3Vec3 rB = pB - xB;

	b3Mat33 skewA = b3SkewSymmetric(rA);
	b3Mat33 skewB = b3SkewSymmetric(rB);

	b3Mat33 skewAT = b3Transpose(skewA);
	b3Mat33 skewBT = b3Transpose(skewB);

	b3Mat33 massSum;
	massSum.x.x = massSum.y.y = massSum.z.z = mA + mB;

	// Compute (JM(J^T))^-1.
	b3Mat33 A = massSum + (skewA * iA * skewAT) + (skewB * iB * skewBT);	
	b3Mat33 B = b3Inverse(A);

	// Compute position constraint and the velocity bias.
	b3Vec3 C = pB - pA;
	b3Vec3 velocityBias = -frequency * B3_BAUMGARTE * C;
	
	// Setup this joint velocity constraint for solving it (sequentially) later.
	m_indexA = indexA;
	m_indexB = indexB;
	m_mA = mA;
	m_mB = mB;
	m_iA = iA;
	m_iB = iB;
	m_rA = rA;
	m_rB = rB;
	m_invMass = B;
	m_velocityBias = velocityBias;
}

void b3SphericalJoint::WarmStart(const b3SolverData* data) {
	u32 indexA = m_indexA;
	u32 indexB = m_indexB;
	r32 mA = m_mA;
	r32 mB = m_mB;
	b3Mat33 iA = m_iA;
	b3Mat33 iB = m_iB;
	b3Vec3 rA = m_rA;
	b3Vec3 rB = m_rB;

	b3Vec3 impulse = m_accImpulse;

	data->velocities[indexA].v -= mA * impulse;
	data->velocities[indexA].w -= iA * b3Cross(rA, impulse);

	data->velocities[indexB].v += mB * impulse;
	data->velocities[indexB].w += iB * b3Cross(rB, impulse);
}

void b3SphericalJoint::SolveVelocityConstraint(const b3SolverData* data) {
	u32 indexA = m_indexA;
	u32 indexB = m_indexB;

	r32 mA = m_mA;
	r32 mB = m_mB;
	b3Mat33 iA = m_iA;
	b3Mat33 iB = m_iB;
	b3Vec3 rA = m_rA;
	b3Vec3 rB = m_rB;
	b3Mat33 invMass = m_invMass;
	b3Vec3 velocityBias = m_velocityBias;	
	b3Vec3 lastImpulse = m_accImpulse;

	b3Vec3 vA = data->velocities[indexA].v;
	b3Vec3 wA = data->velocities[indexA].w;
	b3Vec3 vB = data->velocities[indexB].v;
	b3Vec3 wB = data->velocities[indexB].w;
	
	// Compute J * u.
	b3Vec3 dCdt = vB + b3Cross(wB, rB) - vA - b3Cross(wA, rA);

	// Compute the new, total, and delta impulses.
	b3Vec3 impulse = invMass * (-dCdt + velocityBias);
	b3Vec3 newImpulse = lastImpulse + impulse;
	// b3Vec3 deltaImpulse = newImpulse - lastImpulse <=> impulse;

	// Keep track of the accumulated impulse for the next iteration.
	lastImpulse = newImpulse;

	// Apply new impulse.
	vA -= mA * impulse;
	wA -= iA * b3Cross(rA, impulse);

	vB += mB * impulse;
	wB += iB * b3Cross(rB, impulse);

	// Copy the new old impulse for the next iteration.
	m_accImpulse = lastImpulse;
	
	// Update velocity vector.
	data->velocities[indexA].v = vA;
	data->velocities[indexA].w = wA;
	data->velocities[indexB].v = vB;
	data->velocities[indexB].w = wB;
}
