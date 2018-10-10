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

#include "b3MouseJoint.h"
#include "..\b3Body.h"
#include "..\..\Common\b3Time.h"

b3MouseJoint::b3MouseJoint(const b3MouseJointDef* def) {
	m_bodyA = def->bodyA;
	m_bodyB = def->bodyB;
	m_userData = def->userData;

	m_worldAnchorA = def->worldAnchorA;
	m_localAnchorB = def->localAnchorB;
	
	m_mB = B3_ZERO;
}

void b3MouseJoint::InitializeVelocityConstraint(const b3SolverData* data) {
	u32 indexB = m_bodyB->m_islandID;
	r32 mB = m_bodyB->m_invMass;
	b3Mat33 iB = m_bodyB->m_invWorldInertia;
	b3Transform transformB = m_bodyB->m_transform;	
	b3Vec3 xB = data->positions[indexB].x;
	r32 frequency = data->invdt;
	
	// Compute position constraint and velocity constraint bias.
	b3Vec3 pA = m_worldAnchorA;
	b3Vec3 pB = transformB * m_localAnchorB;
	
	// @note Every constraint points to body.
	b3Vec3 C = pB - pA;
	b3Vec3 velocityBias = -frequency * B3_BAUMGARTE * C;
	
	// Compute effective mass (J * M * J^T)^1.
	b3Vec3 rB = pB - xB;

	/*
	MATLAB (simplify(exp)):
	J*M*(J^T) =

	[ mB - conj(rz)*(Iyz*ry - Iyy*rz) + conj(ry)*(Izz*ry - Izy*rz),     conj(rz)*(Ixz*ry - Ixy*rz) - conj(rx)*(Izz*ry - Izy*rz),     conj(rx)*(Iyz*ry - Iyy*rz) - conj(ry)*(Ixz*ry - Ixy*rz)]
	[     conj(rz)*(Iyz*rx - Iyx*rz) - conj(ry)*(Izz*rx - Izx*rz), mB - conj(rz)*(Ixz*rx - Ixx*rz) + conj(rx)*(Izz*rx - Izx*rz),     conj(ry)*(Ixz*rx - Ixx*rz) - conj(rx)*(Iyz*rx - Iyx*rz)]
	[     conj(ry)*(Izy*rx - Izx*ry) - conj(rz)*(Iyy*rx - Iyx*ry),     conj(rz)*(Ixy*rx - Ixx*ry) - conj(rx)*(Izy*rx - Izx*ry), mB - conj(ry)*(Ixy*rx - Ixx*ry) + conj(rx)*(Iyy*rx - Iyx*ry)]
	*/

	b3Mat33 A;
	
	A.x.x = mB - rB.z * (iB.y.z * rB.y - iB.y.y * rB.z) + rB.y * (iB.z.z * rB.y - iB.z.y * rB.z);
	A.y.x = rB.z * (iB.y.z * rB.x - iB.y.x * rB.z) - rB.y * (iB.z.z * rB.x - iB.z.x * rB.z);
	A.z.x = rB.y * (iB.z.y * rB.x - iB.z.x * rB.y) - rB.z * (iB.y.y * rB.x - iB.y.x * rB.y);

	A.x.y = rB.z * (iB.x.z * rB.y - iB.x.y * rB.z) - rB.x * (iB.z.z * rB.y - iB.z.y * rB.z);
	A.y.y = mB - rB.z * (iB.x.z * rB.x - iB.x.x * rB.z) + rB.x * (iB.z.z * rB.x - iB.z.x * rB.z);
	A.z.y = rB.z * (iB.x.y * rB.x - iB.x.x * rB.y) - rB.x * (iB.z.y * rB.x - iB.z.x * rB.y);

	A.x.z = rB.x * (iB.y.z * rB.y - iB.y.y * rB.z) - rB.y * (iB.x.z * rB.y - iB.x.y * rB.z);
	A.y.z = rB.y * (iB.x.z * rB.x - iB.x.x * rB.z) - rB.x * (iB.y.z * rB.x - iB.y.x * rB.z);
	A.z.z = mB - rB.y * (iB.x.y * rB.x - iB.x.x * rB.y) + rB.x * (iB.y.y * rB.x - iB.y.x * rB.y);

	b3Mat33 B = b3Inverse(A);	
	
	// Setup the computed data for solving (sequentially) 
	// this joint velocity constraint later.
	m_indexB = indexB;
	m_mB = mB;
	m_iB = iB;
	m_invMass = B;
	m_rB = rB;
	m_velocityBias = velocityBias;
}

void b3MouseJoint::WarmStart(const b3SolverData* data) {
	i32 indexB = m_indexB;
	r32 mB = m_mB;
	b3Mat33 iB = m_iB;

	// Warm Start
	b3Vec3 impulse = m_accImpulse;
	data->velocities[indexB].v += mB * impulse;
	data->velocities[indexB].w += iB * impulse;
}

void b3MouseJoint::SolveVelocityConstraint(const b3SolverData* data) {
	i32 indexB = m_indexB;
	r32 mB = m_mB;
	b3Mat33 iB = m_iB;
	b3Mat33 invMass = m_invMass;
	b3Vec3 rB = m_rB;
	b3Vec3 velocityBias = m_velocityBias;
	b3Vec3 lastImpulse = m_accImpulse;

	b3Vec3 vB = data->velocities[indexB].v;
	b3Vec3 wB = data->velocities[indexB].w;
	
	// Compute J * u.
	b3Vec3 dCdt = vB + b3Cross(wB, rB);

	// Compute the delta impulses for this iteration.
	b3Vec3 impulse = invMass * (-dCdt + velocityBias);
	b3Vec3 newImpulse = lastImpulse + impulse;
	// b3Vec3 deltaImpulse = newImpulse - lastImpulse = impulse;

	// Keep track of the accumulated impulse for the next iteration.
	lastImpulse = newImpulse;
	m_accImpulse = lastImpulse;

	vB += mB * impulse;
	wB += iB * b3Cross(rB, impulse);
	
	data->velocities[indexB].v = vB;
	data->velocities[indexB].w = wB;
}
