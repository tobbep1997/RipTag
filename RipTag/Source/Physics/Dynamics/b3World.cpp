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
#include "b3World.h"


b3World::b3World() {
	m_contactGraph.m_blockAllocator = &m_blockAllocator;
	m_jointGraph.m_blockAllocator = &m_blockAllocator;
	m_flags = 0;
	m_flags |= e_clearForcesFlag;
	m_bodyList = nullptr;
	m_bodyCount = 0;
	m_gravityDir.Set(B3_ZERO, -B3_ONE, B3_ZERO);
	m_profile.broadPhaseTime = B3_ZERO;
	m_profile.narrowPhaseTime = B3_ZERO;
	m_profile.solverTime = B3_ZERO;
	m_profile.totalTime = B3_ZERO;
}

b3World::~b3World() {
	// Destroy the shapes that were allocate using b3Alloc.
	b3Body* b = m_bodyList;
	while (b) {
		b->DestroyShapes();
		b->DestroyJoints();
		b = b->m_next;
	}
}

b3Body* b3World::CreateBody(const b3BodyDef& def) {
	// Allocate memory for the body.
	void* mem = m_blockAllocator.Allocate(sizeof(b3Body));
	b3Body* b = new(mem) b3Body(def, this);

	// Link the body to this doubly-linked list of rigid bodies.
	b->m_prev = nullptr;
	b->m_next = m_bodyList;
	if (m_bodyList) {
		m_bodyList->m_prev = b;
	}
	m_bodyList = b;

	++m_bodyCount;

	return b;
}

void b3World::DestroyBody(b3Body* b) {
	b3Assert(m_bodyCount > 0);

	// Remove the contacts, joints, and shapes from the body.
	b->DestroyContacts();
	b->DestroyJoints();
	b->DestroyShapes();

	// Remove from the world's body list.
	if (b->m_prev) {
		b->m_prev->m_next = b->m_next;
	}
	if (b->m_next) {
		b->m_next->m_prev = b->m_prev;
	}
	if (b == m_bodyList) {
		m_bodyList = b->m_next;
	}

	--m_bodyCount;

	// Free memory.
	b->~b3Body();
	m_blockAllocator.Free(b, sizeof(b3Body));
	b = nullptr;
}

b3Joint* b3World::CreateJoint(const b3JointDef& def) {
	return m_jointGraph.CreateJoint(&def);
}

void b3World::DestroyJoint(b3Joint* joint) {
	m_jointGraph.DestroyJoint(joint);
}

void b3World::Solve(const b3TimeStep& step) {
	// Clear all the island flags for the current step.
	for (b3Body* b = m_bodyList; b; b = b->m_next) {
		b->m_flags &= ~b3Body::e_islandFlag;
	}
	
	for (b3Contact* c = m_contactGraph.m_contactList; c; c = c->m_next) {
		c->m_flags &= ~b3Contact::e_islandFlag;
	}
	
	for (b3Joint* j = m_jointGraph.m_jointList; j; j = j->m_next) {
		j->m_onIsland = false;
	}

	// Build and simulate awake islands.
	
	b3IslandDef islandDef;
	islandDef.dt = step.dt;
	islandDef.allowSleep = step.sleeping;
	islandDef.allocator = &m_stackAllocator;
	islandDef.bodyCapacity = m_bodyCount;
	islandDef.contactCapacity = m_contactGraph.m_contactCount;
	islandDef.jointCapacity = m_jointGraph.m_jointCount;
	islandDef.velocityIterations = step.velocityIterations;

	b3Island island(islandDef);

	u32 stackSize = m_bodyCount;
	b3Body** stack = (b3Body**)m_stackAllocator.Allocate(stackSize * sizeof(b3Body*));
	for (b3Body* seed = m_bodyList; seed; seed = seed->m_next) {
		// Skip seeds that are on a island.
		if (seed->m_flags & b3Body::e_islandFlag) {
			continue;
		}
		// This seed must be awake.
		if (!seed->IsAwake()) {
			continue;
		}
		// The seed must be dynamic or kinematic.
		if (seed->m_type == e_staticBody) {
			continue;
		}

		// Reset island and stack.
		island.Reset();
		u32 stackCount = 0;
		stack[stackCount++] = seed;
		seed->m_flags |= b3Body::e_islandFlag;

		// Do a DFS on the contact graph.
		while (stackCount > 0) {
			// Pop a body from the stack.
			b3Body* b = stack[--stackCount];
			// Add this body to the island.
			island.Add(b);

			// This body must be awake.
			b->SetAwake(true);

			// Don't propagate islands across static bodies to keep the island small.
			if (b->m_type == e_staticBody){
				continue;
			}

			// Get all contacts associated with this body.
			for (b3ContactEdge* ce = b->m_contactList; ce; ce = ce->next) {
				b3Contact* contact = ce->contact;

				// Skip the contact if is already on the island.
				if (contact->m_flags & b3Contact::e_islandFlag) {
					continue;
				}

				// Skip contact if the bodies associated with it aren't touching.
				if (!contact->IsTouching()) {
					continue;
				}

				// Skip sensors.
				bool sensorA = contact->m_shapeA->m_isSensor;
				bool sensorB = contact->m_shapeB->m_isSensor;
				if (sensorA || sensorB) {
					continue;
				}

				// Add contact onto the island and mark it.
				island.Add(contact);
				contact->m_flags |= b3Contact::e_islandFlag;

				b3Body* other = ce->other;

				// Skip next propagation if the other body is already on the island.
				if (other->m_flags & b3Body::e_islandFlag) {
					continue;
				}

				// Push the other body onto the stack and mark it.
				b3Assert(stackCount < stackSize);
				stack[stackCount++] = other;
				other->m_flags |= b3Body::e_islandFlag;
			}

			// Get all joints connected with the body.
			for (b3JointEdge* je = b->m_jointList; je; je = je->next) {
				b3Joint* joint = je->joint;

				// Skip the joint is already on the island.
				if (joint->m_onIsland) {
					continue;
				}

				// Add joint to the island and flag it.
				island.Add(joint);
				joint->m_onIsland = true;

				b3Body* other = je->other;

				// Skip next propagation if the other body is already on the island.
				if (other->m_flags & b3Body::e_islandFlag) {
					continue;
				}

				// Push the other body onto the stack and mark it.
				b3Assert(stackCount < stackSize);
				stack[stackCount++] = other;
				other->m_flags |= b3Body::e_islandFlag;
			}
		}

		// Integrate velocities, solve velocity constraints, integrate positions.
		island.Solve(m_gravityDir);

		// Cleanup island flags.
		for (u32 i = 0; i < island.bodyCount; ++i) {
			// Allow static bodies to participate in other islands.
			b3Body* b = island.bodies[i];
			if (b->m_type == e_staticBody) {
				b->m_flags &= ~b3Body::e_islandFlag;
			}
		}
	}

	m_stackAllocator.Free(stack);

	{
		b3Time time;

		for (b3Body* b = m_bodyList; b; b = b->m_next) {
			if (b->m_type == e_staticBody) {
				continue;
			}

			// Compute transforms, shape AABBs, and world inertia tensor.
			b->SynchronizeTransform();
			b->SynchronizeShapes();
		}

		m_contactGraph.FindNewContacts();
		time.Update();
		m_profile.broadPhaseTime = time.GetDeltaMilisecs();
	}
}

void b3World::ClearForces() {
	for (b3Body* b = m_bodyList; b; b = b->m_next) {
		b->m_force.SetZero();
		b->m_torque.SetZero();
	}
}

void b3World::Step(const b3TimeStep& step) {
	b3Time stepTime;

	if (m_flags & e_bodyAddedFlag) {
		// If new shapes were added we need to find new contacts.
		m_contactGraph.FindNewContacts();
		m_flags &= ~b3World::e_bodyAddedFlag;
	}

	{
		b3Time time;
		// Update contact constraints. Destroy ones if they aren't intersecting.
		m_contactGraph.UpdateContacts();
		time.Update();
		m_profile.narrowPhaseTime = time.GetDeltaMilisecs();
	}
	{
		b3Time time;
		// Solve system's EDOs and MLCPs.
		Solve(step);
		time.Update();
		m_profile.solverTime = time.GetDeltaMilisecs();
	}
	
	if (m_flags & e_clearForcesFlag) {
		ClearForces();
	}

	stepTime.Update();
	m_profile.totalTime = stepTime.GetDeltaMilisecs();
}

struct b3QueryCallback {
	bool QueryCallback(i32 proxyID) {
		b3Shape* shape = (b3Shape*)broadPhase->GetUserData(proxyID);
		return listener->ReportShape(shape);
	}
	b3QueryListener* listener;
	const b3BroadPhase* broadPhase;
};

void b3World::QueryAABB(b3QueryListener* listener, const b3AABB& aabb) const {
	b3QueryCallback callback;
	callback.broadPhase = &m_contactGraph.m_broadPhase;
	callback.listener = listener;
	m_contactGraph.m_broadPhase.Query(&callback, aabb);
}

struct b3RayCastCallback {
	r32 RayCastCallback(const b3RayCastInput& input, i32 proxyID) {
		b3Shape* shape = (b3Shape*)broadPhase->GetUserData(proxyID);

		b3Transform transform = shape->GetBody()->GetTransform() * shape->GetTransform();

		b3RayCastOutput output;
		bool hit = shape->RayCast(input, output, transform);
		if (hit) {
			r32 fraction = output.fraction;
			b3Vec3 point = (B3_ONE - fraction) * input.p1 + fraction * input.p2;
			b3Vec3 normal = output.normal;

			// Report the intersection and get the new ray cast fraction.
			return listener->ReportShape(shape, point, normal, fraction);
		}

		// Continue from where we stopped.
		return input.maxFraction;
	}

	b3RayCastListener* listener;
	const b3BroadPhase* broadPhase;
};

void b3World::RayCast(b3RayCastListener* listener, const b3Vec3& p1, const b3Vec3& p2) const {
	b3RayCastInput input;
	input.p1 = p1;
	input.p2 = p2;
	input.maxFraction = B3_MAX_FLOAT;// Use B3_MAX_FLOAT for a ray.

	b3RayCastCallback callback;
	callback.listener = listener;
	callback.broadPhase = &m_contactGraph.m_broadPhase;

	m_contactGraph.m_broadPhase.RayCast(&callback, input);
}

void b3World::Draw(const b3Draw* draw, u32 flags) const {
	b3Assert(draw);

	if (flags & b3Draw::e_centerOfMassesFlag) {
		for (const b3Body* b = 0; b; b = b->GetNext()) {
			b3Color color(0.0f, 1.0f, 0.0f);

			const b3Transform& transform = b->GetTransform();

			draw->DrawPoint(transform.translation, color);

			for (const b3Shape* s = b->GetShapeList(); s; s = s->GetNext()) {
				const b3Transform world = transform * s->GetTransform();
				draw->DrawPoint(world.translation, color);
			}
		}
	}

	if (flags & b3Draw::e_broadPhaseFlag) {
		b3Color color1(0.0f, 1.0f, 0.0f);
		b3Color color2(1.0f, 0.0f, 0.0f);

		const b3DynamicAABBTree* tree = &m_contactGraph.m_broadPhase.m_dynamicAabbTree;

		b3ArrayPOD<i32, u32, 512> stack;
		stack.PushBack(tree->m_root);

		while (!stack.Empty()) {
			i32 nodeIndex = stack.Back();
			
			stack.PopBack();

			if (nodeIndex == NULL_NODE) {
				continue;
			}

			const b3DynamicAABBTree::b3Node* node = tree->m_nodes + nodeIndex;

			if (node->IsLeaf()) {
				draw->DrawAABB(node->aabb, color1);
			}
			else {
				draw->DrawAABB(node->aabb, color2);

				stack.PushBack(node->child1);
				stack.PushBack(node->child2);
			}
		}
	}

	if (flags & b3Draw::e_contactsFlag) {
		for (const b3Contact* c = m_contactGraph.m_contactList; c; c = c->m_next) {
			const b3Manifold* m = &c->m_manifold;

			b3Color color1(0.0f, 1.0f, 0.0f);
			b3Color color2(1.0f, 0.0f, 0.0f);

			for (u32 i = 0; i < m->pointCount; ++i) {
				const b3ContactPoint* p = &m->points[i];

				draw->DrawPoint(p->position, p->warmStarted ? color1 : color2);
				draw->DrawLine(p->position, p->position + m->normal, color1);
				draw->DrawLine(p->position, p->position + p->tangents[0], color1);
				draw->DrawLine(p->position, p->position + p->tangents[1], color1);
			}
		}

	}
}
