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

#include "b3Island.h"
#include "b3Body.h"
#include "Joints\b3Joint.h"
#include "Joints\b3JointSolver.h"
#include "Contacts\b3Contact.h"
#include "Contacts\b3ContactSolver.h"
#include "..\Common\Memory\b3StackAllocator.h"
#include "..\Common\b3Time.h"

b3Island::b3Island(const b3IslandDef& def) {
	allocator = def.allocator;
	dt = def.dt;
	allowSleep = def.allowSleep;
	velocityIterations = def.velocityIterations;
	bodyCapacity = def.bodyCapacity;
	contactCapacity = def.contactCapacity;
	jointCapacity = def.jointCapacity;

	bodies = (b3Body**)allocator->Allocate(bodyCapacity * sizeof(b3Body*));
	velocities = (b3Velocity*)allocator->Allocate(bodyCapacity * sizeof(b3Velocity));
	positions = (b3Position*)allocator->Allocate(bodyCapacity * sizeof(b3Position));
	contacts = (b3Contact**)allocator->Allocate(contactCapacity * sizeof(b3Contact*));
	joints = (b3Joint**)allocator->Allocate(jointCapacity * sizeof(b3Joint*));

	bodyCount = 0;
	contactCount = 0;
	jointCount = 0;
}

b3Island::~b3Island() {
	// @note Reverse order of construction.
	allocator->Free(joints);
	allocator->Free(contacts);
	allocator->Free(positions);
	allocator->Free(velocities);
	allocator->Free(bodies);
}

void b3Island::Reset() {
	bodyCount = 0;
	contactCount = 0;
	jointCount = 0;
}

void b3Island::Add(b3Body* b) {
	b3Assert(bodyCount < bodyCapacity);
	b->m_islandID = bodyCount;
	bodies[bodyCount] = b;
	++bodyCount;
}

void b3Island::Add(b3Contact* c) {
	b3Assert(contactCount < contactCapacity);
	contacts[contactCount] = c;
	++contactCount;
}

void b3Island::Add(b3Joint* j) {
	b3Assert(jointCount < jointCapacity);
	joints[jointCount] = j;
	++jointCount;
}

void b3Island::Solve(const b3Vec3& gravityDir) {
	r32 h = dt;
	b3Vec3 gravityForce = B3_GRAVITY_ACC * gravityDir;

	// Integrate velocities.
	for (u32 i = 0; i < bodyCount; ++i) {
		b3Body* b = bodies[i];

		b3Vec3 v = b->m_linearVelocity;
		b3Vec3 w = b->m_angularVelocity;
		b3Vec3 x = b->m_worldCenter;
		b3Quaternion q = b->m_orientation;

		if (b->m_type == e_dynamicBody) {
			// Use semi-implitic Euler.
			b3Vec3 force = b->m_gravityScale * gravityForce + b->m_force;
			v += (h * b->m_invMass) * force;
			w += h * (b->m_invWorldInertia * b->m_torque);

			// References: Box2D.
			// Apply damping.
			// ODE: dv/dt + c * v = 0
			// Solution: v(t) = v0 * exp(-c * t)
			// Time step: v(t + dt) = v0 * exp(-c * (t + dt)) = v0 * exp(-c * t) * exp(-c * dt) = v * exp(-c * dt)
			// v2 = exp(-c * dt) * v1
			// Pade approximation:
			// v2 = v1 * 1 / (1 + c * dt)
			v *= B3_ONE / (B3_ONE + h * r32(0.1));
			w *= B3_ONE / (B3_ONE + h * r32(0.1));
		}

		velocities[i].v = v;
		velocities[i].w = w;
		positions[i].x = x;
		positions[i].q = q;
	}

	b3JointSolverDef jointSolverDef;
	jointSolverDef.dt = h;
	jointSolverDef.joints = joints;
	jointSolverDef.count = jointCount;
	jointSolverDef.positions = positions;
	jointSolverDef.velocities = velocities;

	b3JointSolver jointSolver(&jointSolverDef);
	jointSolver.InitializeVelocityConstraints();

	b3ContactSolverDef contactSolverDef;
	contactSolverDef.dt = h;
	contactSolverDef.contacts = contacts;
	contactSolverDef.count = contactCount;
	contactSolverDef.positions = positions;
	contactSolverDef.velocities = velocities;
	contactSolverDef.allocator = allocator;

	b3ContactSolver contactSolver(&contactSolverDef);
	contactSolver.InitializeVelocityConstraints();
	
	jointSolver.WarmStart();
	contactSolver.WarmStart();
	
	// Solve velocity constraints.
	for (u32 i = 0; i < velocityIterations; ++i) {
		jointSolver.SolveVelocityConstraints();
		contactSolver.SolveVelocityConstraints();
	}

	contactSolver.StoreImpulses();

	for (u32 i = 0; i < bodyCount; ++i) {
		b3Body* b = bodies[i];
		if (b->m_type == e_staticBody) {
			continue;
		}

		b3Vec3 x = positions[i].x;
		b3Quaternion q1 = positions[i].q;
		b3Vec3 v = velocities[i].v;
		b3Vec3 w = velocities[i].w;
		
		x += h * v;
		
		b3Quaternion q2 = Integrate(q1, w, h);

		positions[i].x = x;
		positions[i].q = q2;
		velocities[i].v = v;
		velocities[i].w = w;
	}

	for (u32 i = 0; i < bodyCount; ++i) {
		b3Body* b = bodies[i];
		if (b->m_type == e_staticBody) {
			continue;
		}

		b->m_worldCenter = positions[i].x;
		b->m_orientation = positions[i].q;
		b->m_linearVelocity = velocities[i].v;
		b->m_angularVelocity = velocities[i].w;
	}

	if (allowSleep) {
		r32 minSleepTime = B3_MAX_FLOAT;
		for (u32 i = 0; i < bodyCount; ++i) {
			b3Body* b = bodies[i];
			if (b->m_type == e_staticBody) {
				continue;
			}

			// Compute the linear and angular speed of the body.
			const r32 sqrLinVel = b3LenSq(b->m_linearVelocity);
			const r32 sqrAngVel = b3LenSq(b->m_angularVelocity);

			if (sqrLinVel > B3_SLEEP_LINEAR_TOL || sqrAngVel > B3_SLEEP_ANGULAR_TOL) {
				minSleepTime = B3_ZERO;
				b->m_sleepTime = B3_ZERO;
			}
			else {
				b->m_sleepTime += h;
				minSleepTime = b3Min(minSleepTime, b->m_sleepTime);
			}
		}

		// Put the island to sleep so long as the minimum found sleep time
		// is below the threshold. 
		if (minSleepTime >= B3_TIME_TO_SLEEP) {
			for (u32 i = 0; i < bodyCount; ++i) {
				bodies[i]->SetAwake(false);
			}
		}
	}
}
