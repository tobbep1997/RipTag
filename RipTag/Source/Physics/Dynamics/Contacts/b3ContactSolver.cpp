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

#include "b3ContactSolver.h"
#include "b3Contact.h"
#include "..\..\Collision\Shapes\b3Shape.h"
#include "..\..\Dynamics\b3Body.h"
#include "..\..\Common\Memory\b3StackAllocator.h"
#include "..\..\Common\b3Time.h"

b3ContactSolver::b3ContactSolver(const b3ContactSolverDef* def) {
	m_allocator = def->allocator;
	m_contacts = def->contacts;
	m_count = def->count;
	m_invDt = def->dt > B3_ZERO ? B3_ONE / def->dt : B3_ZERO;
	m_positions = def->positions;
	m_velocities = def->velocities;

	m_velocityConstraints = (b3ContactVelocityConstraint*)m_allocator->Allocate(m_count * sizeof(b3ContactVelocityConstraint));

	for (u32 i = 0; i < m_count; ++i) {
		b3Contact* c = m_contacts[i];
		b3ContactVelocityConstraint* vc = m_velocityConstraints + i;
		
		b3Body* bodyA = c->m_shapeA->GetBody();
		b3Body* bodyB = c->m_shapeB->GetBody();

		u32 pointCount = c->m_manifold.pointCount;

		b3Assert(pointCount > 0);

		vc->normal = c->m_manifold.normal;
		vc->invMassA = bodyA->m_invMass;
		vc->invMassB = bodyB->m_invMass;
		vc->invIA = bodyA->m_invWorldInertia;
		vc->invIB = bodyB->m_invWorldInertia;
		vc->friction = c->m_friction;
		vc->restitution = c->m_restitution;
		vc->indexA = bodyA->m_islandID;
		vc->indexB = bodyB->m_islandID;
		vc->pointCount = pointCount;

		for (u32 j = 0; j < pointCount; ++j) {
			b3ContactPoint* cp = c->m_manifold.points + j;
			b3VelocityConstraintPoint* vcp = vc->points + j;

			// Setup warm start.
			vcp->normalImpulse = cp->normalImpulse;
			vcp->tangentImpulse[0] = cp->tangentImpulse[0];
			vcp->tangentImpulse[1] = cp->tangentImpulse[1];
			vcp->tangents[0] = cp->tangents[0];
			vcp->tangents[1] = cp->tangents[1];
		}
	}
}

b3ContactSolver::~b3ContactSolver() {
	m_allocator->Free(m_velocityConstraints);
}

void b3ContactSolver::InitializeVelocityConstraints() {
	for (u32 i = 0; i < m_count; ++i) {
		b3Contact* c = m_contacts[i];
		b3ContactVelocityConstraint* vc = m_velocityConstraints + i;
		
		r32 mA = vc->invMassA;
		r32 mB = vc->invMassB;
		b3Mat33 iA = vc->invIA;
		b3Mat33 iB = vc->invIB;
		u32 indexA = vc->indexA;
		u32 indexB = vc->indexB;
		u32 pointCount = c->m_manifold.pointCount;

		b3Vec3 vA = m_velocities[indexA].v;
		b3Vec3 wA = m_velocities[indexA].w;
		b3Vec3 xA = m_positions[indexA].x;
		b3Quaternion qA = m_positions[indexA].q;

		b3Vec3 vB = m_velocities[indexB].v;
		b3Vec3 wB = m_velocities[indexB].w;
		b3Vec3 xB = m_positions[indexB].x;
		b3Quaternion qB = m_positions[indexB].q;

		for (u32 j = 0; j < pointCount; ++j) {
			b3ContactPoint* cp = c->m_manifold.points + j;
			b3VelocityConstraintPoint* vcp = vc->points + j;

			vcp->rA = cp->position - xA;
			vcp->rB = cp->position - xB;

			// Compute tangent mass.
			b3Vec3 rt1A = b3Cross(vcp->rA, vcp->tangents[0]);
			b3Vec3 rt1B = b3Cross(vcp->rB, vcp->tangents[0]);

			b3Vec3 rt2A = b3Cross(vcp->rA, vcp->tangents[1]);
			b3Vec3 rt2B = b3Cross(vcp->rB, vcp->tangents[1]);

			r32 kTangent1 = mA + mB + b3Dot(rt1A, iA * rt1A) + b3Dot(rt1B, iB * rt1B);
			r32 kTangent2 = mA + mB + b3Dot(rt2A, iA * rt2A) + b3Dot(rt2B, iB * rt2B);

			vcp->tangentMass[0] = B3_ONE / kTangent1;
			vcp->tangentMass[1] = B3_ONE / kTangent2;

			// Compute normal mass.
			b3Vec3 rnA = b3Cross(vcp->rA, vc->normal);
			b3Vec3 rnB = b3Cross(vcp->rB, vc->normal);

			r32 kNormal = mA + mB + b3Dot(rnA, iA * rnA) + b3Dot(rnB, iB * rnB);
			vcp->normalMass = B3_ONE / kNormal;

			r32 C = b3Min(B3_ZERO, c->m_manifold.distances[j] + B3_LINEAR_SLOP);
			vcp->velocityBias = -m_invDt * B3_BAUMGARTE * C;

			// Add restitution in the velocity constraint.			
			r32 vn = b3Dot(vB + b3Cross(wB, vcp->rB) - vA - b3Cross(wA, vcp->rA), vc->normal);
			if (vn < -B3_ONE) {
				vcp->velocityBias += -(c->m_restitution) * vn;
			}
		}
	}
}

void b3ContactSolver::WarmStart() {
	// Warm start.
	for (u32 i = 0; i < m_count; ++i) {
		b3ContactVelocityConstraint* vc = m_velocityConstraints + i;

		b3Vec3 normal = vc->normal;
		r32 mA = vc->invMassA;
		r32 mB = vc->invMassB;
		b3Mat33 iA = vc->invIA;
		b3Mat33 iB = vc->invIB;
		u32 indexA = vc->indexA;
		u32 indexB = vc->indexB;
		u32 pointCount = vc->pointCount;

		b3Vec3 vA = m_velocities[indexA].v;
		b3Vec3 wA = m_velocities[indexA].w;
		b3Vec3 vB = m_velocities[indexB].v;
		b3Vec3 wB = m_velocities[indexB].w;

		for (u32 j = 0; j < pointCount; ++j) {
			// Project old solutions into the new transposed Jacobians.
			b3VelocityConstraintPoint* vcp = vc->points + j;

			r32 lastNormalLambda = vcp->normalImpulse;
			r32 lastTangentLambda1 = vcp->tangentImpulse[0];
			r32 lastTangentLambda2 = vcp->tangentImpulse[1];

			b3Vec3 normalImpulse = lastNormalLambda * normal;
			b3Vec3 tangentImpulse1 = lastTangentLambda1 * vcp->tangents[0];
			b3Vec3 tangentImpulse2 = lastTangentLambda2 * vcp->tangents[1];

			b3Vec3 impulse = normalImpulse + tangentImpulse1 + tangentImpulse2;
			
			vA -= mA * impulse;
			wA -= iA * b3Cross(vcp->rA, impulse);
			
			vB += mB * impulse;
			wB += iB * b3Cross(vcp->rB, impulse);
		}

		m_velocities[indexA].v = vA;
		m_velocities[indexA].w = wA;
		m_velocities[indexB].v = vB;
		m_velocities[indexB].w = wB;
	}
}

void b3ContactSolver::SolveVelocityConstraints() {
	for (u32 i = 0; i < m_count; ++i) {
		b3ContactVelocityConstraint* vc = m_velocityConstraints + i;

		b3Vec3 normal = vc->normal;
		r32 mA = vc->invMassA;
		r32 mB = vc->invMassB;
		b3Mat33 iA = vc->invIA;
		b3Mat33 iB = vc->invIB;
		u32 indexA = vc->indexA;
		u32 indexB = vc->indexB;
		u32 pointCount = vc->pointCount;

		b3Vec3 vA = m_velocities[indexA].v;
		b3Vec3 wA = m_velocities[indexA].w;
		b3Vec3 vB = m_velocities[indexB].v;
		b3Vec3 wB = m_velocities[indexB].w;

		for (u32 j = 0; j < pointCount; ++j) {
			b3VelocityConstraintPoint* vcp = vc->points + j;
			{
				// Compute J * u.
				b3Vec3 dv = vB + b3Cross(wB, vcp->rB) - vA - b3Cross(wA, vcp->rA);
				r32 dCdt = b3Dot(dv, normal);

				// Compute new lambda values.
				r32 lambda = vcp->normalMass * (-dCdt + vcp->velocityBias);				
				r32 newLambda = b3Max(vcp->normalImpulse + lambda, B3_ZERO);
				r32 deltaLambda = newLambda - vcp->normalImpulse;
				
				vcp->normalImpulse = newLambda;

				b3Vec3 deltaImpulse = deltaLambda * normal;

				vA -= mA * deltaImpulse;
				wA -= iA * b3Cross(vcp->rA, deltaImpulse);

				vB += mB * deltaImpulse;
				wB += iB * b3Cross(vcp->rB, deltaImpulse);
			}

			for (u32 k = 0; k < 2; ++k) {
				// Compute tangential impulse.
				r32 hi = vc->friction * vcp->normalImpulse;
				r32 lo = -hi;

				// Compute J * u.
				b3Vec3 dv = vB + b3Cross(wB, vcp->rB) - vA - b3Cross(wA, vcp->rA);
				r32 dCdt = b3Dot(dv, vcp->tangents[k]);

				// Compute new lambda values.
				r32 lambda = vcp->tangentMass[k] * -dCdt;
				r32 newLambda = b3Clamp(vcp->tangentImpulse[k] + lambda, lo, hi);
				r32 deltaLambda = newLambda - vcp->tangentImpulse[k];
				
				vcp->tangentImpulse[k] = newLambda;

				b3Vec3 deltaImpulse = deltaLambda * vcp->tangents[k];

				vA -= mA * deltaImpulse;
				wA -= iA * b3Cross(vcp->rA, deltaImpulse);

				vB += mB * deltaImpulse;
				wB += iB * b3Cross(vcp->rB, deltaImpulse);
			}
		}

		m_velocities[indexA].v = vA;
		m_velocities[indexA].w = wA;
		m_velocities[indexB].v = vB;
		m_velocities[indexB].w = wB;
	}
}

void b3ContactSolver::StoreImpulses() {
	for (u32 i = 0; i < m_count; ++i) {
		b3Contact* c = m_contacts[i];
		b3ContactVelocityConstraint* vc = m_velocityConstraints + i;

		for (u32 j = 0; j < vc->pointCount; ++j) {
			b3VelocityConstraintPoint* vcp = vc->points + j;
			b3ContactPoint* cp = c->m_manifold.points + j;
			cp->normalImpulse = vcp->normalImpulse;
			cp->tangentImpulse[0] = vcp->tangentImpulse[0];
			cp->tangentImpulse[1] = vcp->tangentImpulse[1];
		}
	}
}
