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
#include "BouncePCH.h"
#include "b3RevoluteJoint.h"

b3RevoluteJoint::b3RevoluteJoint(const b3RevoluteJointDef* def) {
	m_bodyA = def->bodyA;
	m_bodyB = def->bodyB;

	m_mA = B3_ZERO;
	m_mB = B3_ZERO;

	m_localFrameA = def->localFrameA;
	m_localFrameB = def->localFrameB;
	m_low = def->lowLimit;
	m_high = def->highLimit;

	m_invMass1 = B3_ZERO;
	m_velocityBias1 = B3_ZERO;
	m_accLambda1 = B3_ZERO;

	m_invMass2 = B3_ZERO;
	m_velocityBias2 = B3_ZERO;
	m_accLambda2 = B3_ZERO;

	m_limitState = e_betweenLimits;
	m_accLambda3 = B3_ZERO;

	b3Assert(m_low < m_high);
}

void b3RevoluteJoint::InitializeVelocityConstraint(const b3SolverData* data) {
	u32 indexA = m_bodyA->m_islandID;
	u32 indexB = m_bodyB->m_islandID;

	r32 mA = m_bodyA->m_invMass;
	r32 mB = m_bodyB->m_invMass;

	b3Mat33 iA = m_bodyA->m_invWorldInertia;
	b3Mat33 iB = m_bodyB->m_invWorldInertia;

	b3Transform transformA = m_bodyA->m_transform;
	b3Transform transformB = m_bodyB->m_transform;

	b3Transform worldFrameA = transformA * m_localFrameA;
	b3Transform worldFrameB = transformB * m_localFrameB;

	b3Vec3 p1 = worldFrameA.translation;
	b3Vec3 u1 = worldFrameA.rotation.x;
	b3Vec3 v1 = worldFrameA.rotation.y;
	b3Vec3 w1 = worldFrameA.rotation.z;

	b3Vec3 p2 = worldFrameB.translation;
	b3Vec3 u2 = worldFrameB.rotation.x;
	b3Vec3 v2 = worldFrameB.rotation.y;

	r32 frequency = data->invdt;
	b3Vec3 xA = data->positions[indexA].x;
	b3Vec3 xB = data->positions[indexB].x;

	b3Vec3 vA = data->velocities[indexA].v;
	b3Vec3 wA = data->velocities[indexA].w;

	b3Vec3 vB = data->velocities[indexB].v;
	b3Vec3 wB = data->velocities[indexB].w;

	b3Mat33 iAB = iA + iB;

	// Initialize the revolute constraint.
	{
		{
			// The u2 axis must be orthogonal to w1 (orthogonality condition).
			r32 C = b3Dot(u2, w1);
			m_velocityBias1 = -frequency * B3_BAUMGARTE * C;

			b3Vec3 u2xw1 = b3Cross(u2, w1); // The "Jacobian"
			m_invMass1 = B3_ONE / b3Dot(u2xw1, iAB * u2xw1);

			m_u2xw1 = u2xw1;
		}

		{
			// The v2 axis must be orthogonal to w1 (orthogonality condition).
			r32 C = b3Dot(v2, w1);
			m_velocityBias2 = -frequency * B3_BAUMGARTE * C;

			b3Vec3 v2xw1 = b3Cross(v2, w1); // The "Jacobian"
			m_invMass2 = B3_ONE / b3Dot(v2xw1, iAB * v2xw1);

			m_v2xw1 = v2xw1;
		}
	}

	// Initialize the revolute joint axis angle limit position constraint C(w(t)) = lo < theta < hi.
	{
		// Compute the angle between the two reference frames.
		// @note: atan2(sine, cosine) = f(y, x)!	
		r32 cosine = b3Dot(u2, u1);
		r32 sine = b3Dot(u2, v1);
		r32 theta = atan2(sine, cosine);

		// std::cout << "\nCos = " << cosine << " Sin = " << sine << " Theta = " << theta;

		if (theta >= m_low && theta <= m_high) {
			// The position constraint is satisfied (the angle is between the angle limits).
			m_limitState = e_betweenLimits;
			m_accLambda3 = B3_ZERO;
			m_velocityBias3 = B3_ZERO;

		}
		else {
			// The position constraint is violated.
			m_w1 = w1;

			// Compute position constraint error and set velocity bias = -frequency * beta * error.
			if (theta <= m_low) {
				// Lower limit <=> Enforce theta = low.
				// Reset accumulated lambda if the last state
				// is different from the current one.
				if (m_limitState != e_lowerLimit) {
					m_accLambda3 = B3_ZERO;
					m_limitState = e_lowerLimit;
				}

				// Allow some slop.
				if (theta > m_low - B3_ANGULAR_SLOP) {
					r32 C = B3_ZERO;
					m_velocityBias3 = -frequency * B3_BAUMGARTE * C;
				}
				else {
					r32 C = theta - m_low;
					m_velocityBias3 = -frequency * B3_BAUMGARTE * C;
				}
			}
			else if (theta >= m_high) {
				// Reset accumulated lambda if the last state
				// is different from the current one.
				if (m_limitState != e_upperLimit) {
					m_accLambda3 = B3_ZERO;
					m_limitState = e_upperLimit;
				}

				if (theta < m_high + B3_ANGULAR_SLOP) {
					r32 C = B3_ZERO;
					m_velocityBias3 = -frequency * B3_BAUMGARTE * C;
				}
				else {
					r32 C = theta - m_high;
					m_velocityBias3 = -frequency * B3_BAUMGARTE * C;
				}
			}
			else {
				// Is this position constraint satisfied?! No.
				b3Assert(false);
			}
		}
	}

	// Initialize the point-to-point equality constraint.
	{
		// Compute position constraint error and set the velocity bias.
		b3Vec3 C = p2 - p1;
		b3Vec3 velocityBias = -frequency * B3_BAUMGARTE * C;

		b3Vec3 rA = p1 - xA;
		b3Vec3 rB = p2 - xB;

		b3Mat33 skewA = b3SkewSymmetric(rA);
		b3Mat33 skewB = b3SkewSymmetric(rB);

		b3Mat33 skewAT = b3Transpose(skewA);
		b3Mat33 skewBT = b3Transpose(skewB);

		r32 imSum = mA + mB;
		b3Mat33 invMassSum = b3Diagonal(imSum);

		// Compute (JM(J^T))^-1.
		b3Mat33 A = invMassSum + (skewA * iA * skewAT) + (skewB * iB * skewBT);
		b3Mat33 B = b3Inverse(A);

		m_invMass4 = B;
		m_rA = rA;
		m_rB = rB;
		m_velocityBias4 = velocityBias;
	}

	m_indexA = indexA;
	m_indexB = indexB;
	m_mA = mA;
	m_mB = mB;
	m_iA = iA;
	m_iB = iB;
}

void b3RevoluteJoint::WarmStart(const b3SolverData* data) {
	u32 indexA = m_indexA;
	u32 indexB = m_indexB;

	r32 mA = m_mA;
	r32 mB = m_mB;
	b3Mat33 iA = m_iA;
	b3Mat33 iB = m_iB;
	b3Vec3 rA = m_rA;
	b3Vec3 rB = m_rB;

	b3Vec3 vA = data->velocities[indexA].v;
	b3Vec3 wA = data->velocities[indexA].w;
	b3Vec3 vB = data->velocities[indexB].v;
	b3Vec3 wB = data->velocities[indexB].w;

	{
		// Warm Start (revolute joint + angle limits).

		// The lagrange multipliers must be
		// stored because they will be projected on the 
		// new J^T this frame (and not on the last J^T).
		b3Vec3 impulse1 = m_accLambda1 * m_u2xw1;
		b3Vec3 impulse2 = m_accLambda2 * m_v2xw1;

		b3Vec3 impulse = impulse1 + impulse2;

		if (m_limitState != e_betweenLimits) {
			b3Vec3 impulse3 = m_accLambda3 * m_w1;
			impulse += impulse3;
		}

		wA -= iA * impulse;
		wB += iB * impulse;
	}

	{
		// Warm Start (point-to-point).
		b3Vec3 impulse = m_accImpulse4;

		vA -= mA * impulse;
		wA -= iA * b3Cross(rA, impulse);

		vB += mB * impulse;
		wB += iB * b3Cross(rB, impulse);
	}

	data->velocities[indexA].v = vA;
	data->velocities[indexA].w = wA;
	data->velocities[indexB].v = vB;
	data->velocities[indexB].w = wB;
}

void b3RevoluteJoint::SolveVelocityConstraint(const b3SolverData* data) {
	u32 indexA = m_indexA;
	u32 indexB = m_indexB;

	r32 mA = m_mA;
	r32 mB = m_mB;

	b3Mat33 iA = m_iA;
	b3Mat33 iB = m_iB;

	b3Vec3 vA = data->velocities[indexA].v;
	b3Vec3 wA = data->velocities[indexA].w;
	b3Vec3 vB = data->velocities[indexB].v;
	b3Vec3 wB = data->velocities[indexB].w;

	/*
	* @todo_irlan
	* Solve the point-to-point constraint and the orthogonal
	* conditions as a 3x3 linear system.
	* Solve the angle limits as a scalar constraint (already implemented).
	*/

	// Solve revolute joint (orthogonal conditions) equality constraints.
	{
		b3Vec3 u2xw1 = m_u2xw1;
		r32 invMass = m_invMass1;
		r32 velocityBias = m_velocityBias1;
		r32 lambda0 = m_accLambda1;

		// Compute J * u.
		b3Vec3 dw = wB - wA;
		r32 dCdt = b3Dot(u2xw1, dw);

		// Compute the new, total, and delta lambdas.
		r32 lambda = invMass * (-dCdt + velocityBias);
		r32 newLambda = lambda0 + lambda;

		// Set lambda for the next iteration and for warm starting.
		lambda0 = newLambda;
		m_accLambda1 = lambda0;

		// Project the delta lambda onto the Jacobian transpose (to get the delta impulse).
		b3Vec3 impulse = lambda * u2xw1;

		// Apply the delta impulses.
		wA -= iA * impulse;
		wB += iB * impulse;
	}

	{
		b3Vec3 v2xw1 = m_v2xw1;
		r32 invMass = m_invMass2;
		r32 velocityBias = m_velocityBias2;
		r32 lambda0 = m_accLambda2;

		// Compute J * u.
		b3Vec3 dw = wB - wA;
		r32 dCdt = b3Dot(v2xw1, dw);

		// Compute the new, total, and delta lambdas.
		r32 lambda = invMass * (-dCdt + velocityBias);
		r32 newLambda = lambda0 + lambda;

		// Set lambda for the next iteration and for warm starting.
		lambda0 = newLambda;
		m_accLambda2 = lambda0;

		// Project the delta lambda onto the Jacobian transpose (to get the delta impulse).
		b3Vec3 impulse = lambda * v2xw1;

		// Apply the delta impulses.
		wA -= iA * impulse;
		wB += iB * impulse;
	}

	// Solve revolute angle limits (inequality constraints).
	if (m_limitState != e_betweenLimits) {
		b3Vec3 w1 = m_w1;
		r32 velocityBias = m_velocityBias3;
		r32 lambda0 = m_accLambda3;

		// Compute J * u.
		b3Vec3 dw = wB - wA;
		r32 dCdt = b3Dot(dw, w1);

		// Compute the new, total, and delta lambdas for
		// this (inequality) constraint.
		r32 lambda = -dCdt + velocityBias;
		r32 newLambda = m_limitState == e_lowerLimit ? b3Max(lambda0 + lambda, B3_ZERO) : b3Min(lambda0 + lambda, B3_ZERO);
		r32 deltaLambda = newLambda - lambda0;
		// Set lambda for the next iteration and for warm starting.
		lambda0 = newLambda;
		m_accLambda3 = lambda0;

		// Project to the Jacobian (transposed).
		b3Vec3 deltaImpulse = deltaLambda * w1;

		wA -= iA * deltaImpulse;
		wB += iB * deltaImpulse;
	}

	{
		// Solve point-to-point (linear constraint).
		b3Mat33 invMass = m_invMass4;
		b3Vec3 rA = m_rA;
		b3Vec3 rB = m_rB;
		b3Vec3 velocityBias = m_velocityBias4;
		b3Vec3 lastImpulse = m_accImpulse4;

		// Compute J * u.
		b3Vec3 dCdt = vB + b3Cross(wB, rB) - vA - b3Cross(wA, rA);

		// Compute the new impulses.
		b3Vec3 impulse = invMass * (-dCdt + velocityBias);
		b3Vec3 newImpulse = lastImpulse + impulse;

		// Keep track of the accumulated impulse for the next iteration.
		lastImpulse = newImpulse;

		// Apply delta lambdas.
		vA -= mA * impulse;
		wA -= iA * b3Cross(rA, impulse);

		vB += mB * impulse;
		wB += iB * b3Cross(rB, impulse);

		// Copy the new old impulse for the next iteration.
		m_accImpulse4 = lastImpulse;
	}

	// Inject the new velocities back to the velocity vector.
	data->velocities[indexA].v = vA;
	data->velocities[indexA].w = wA;
	data->velocities[indexB].v = vB;
	data->velocities[indexB].w = wB;
}
