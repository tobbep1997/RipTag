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

#include "b3ContactGraph.h"
#include "b3Contact.h"
#include "..\b3Body.h"
#include "..\b3WorldListeners.h"
#include "..\..\Common\Memory\b3BlockAllocator.h"
#include "..\..\Collision\Shapes\b3Shape.h"
#include "..\..\Collision\b3SAT.h"

b3ContactGraph::b3ContactGraph() {
	m_blockAllocator = nullptr;
	m_contactList = nullptr;
	m_contactCount = 0;
	m_contactListener = nullptr;
}

void b3ContactGraph::AddPair(void* data1, void* data2) {
	b3Shape* shapeA = (b3Shape*)data1;
	b3Shape* shapeB = (b3Shape*)data2;

	b3Body* bodyA = shapeA->m_body;
	b3Body* bodyB = shapeB->m_body;

	if (bodyA == bodyB) {
		// Two shapes of the same body cannot collide.
		return;
	}

	b3ContactEdge* edge = bodyB->m_contactList;
	while (edge) {
		if (edge->other == bodyA) {
			b3Shape* sA = edge->contact->m_shapeA;
			b3Shape* sB = edge->contact->m_shapeB;

			if (sA == shapeA && sB == shapeB) {
				// A contact already exists.
				return;
			}
		}
		edge = edge->next;
	}

	if (bodyA->m_type == e_staticBody && bodyB->m_type == e_staticBody) {
		// At least one dynamic body enters in contact.
		return;
	}

	// Allocate the new contact.
	b3Contact* c = (b3Contact*)m_blockAllocator->Allocate(sizeof(b3Contact));
	c->m_flags = 0;
	c->m_shapeA = shapeA;
	c->m_shapeB = shapeB;
	c->m_friction = b3MixFriction(shapeA->m_friction, shapeB->m_friction);
	c->m_restitution = b3MixRestitution(shapeA->m_restitution, shapeB->m_restitution);
	c->m_manifold.pointCount = 0;

	// Add the contact to the contact graph contact list.
	c->m_prev = nullptr;
	c->m_next = m_contactList;
	if (m_contactList) {
		m_contactList->m_prev = c;
	}
	m_contactList = c;

	// Add the contact to body A's contact contact list.
	c->m_nodeA.contact = c;
	c->m_nodeA.other = bodyB;
	c->m_nodeA.prev = nullptr;
	c->m_nodeA.next = bodyA->m_contactList;
	if (bodyA->m_contactList) {
		bodyA->m_contactList->prev = &c->m_nodeA;
	}
	bodyA->m_contactList = &c->m_nodeA;

	// Add the contact to body B's contact contact list.
	c->m_nodeB.contact = c;
	c->m_nodeB.other = bodyA;
	c->m_nodeB.prev = nullptr;
	c->m_nodeB.next = bodyB->m_contactList;
	if (bodyB->m_contactList) {
		bodyB->m_contactList->prev = &c->m_nodeB;
	}
	bodyB->m_contactList = &c->m_nodeB;

	// Awake the bodies if both are not sensors.
	if (!shapeA->IsSensor() && !shapeB->IsSensor()) {
		bodyA->SetAwake(true);
		bodyB->SetAwake(true);
	}

	++m_contactCount;
}

void b3ContactGraph::DestroyContact(b3Contact* c) {
	// Report to the contact listener that the contact will be destroyed.
	if (m_contactListener) {
		if (c->IsTouching()) {
			m_contactListener->EndContact(c);
		}
	}

	b3Body* bodyA = c->m_shapeA->m_body;
	b3Body* bodyB = c->m_shapeB->m_body;

	// Remove the contact from the contact graph contact list.
	if (c->m_prev) {
		c->m_prev->m_next = c->m_next;
	}

	if (c->m_next) {
		c->m_next->m_prev = c->m_prev;
	}

	if (c == m_contactList) {
		m_contactList = c->m_next;
	}

	// Remove the contact from body A's contact list.
	if (c->m_nodeA.prev) {
		c->m_nodeA.prev->next = c->m_nodeA.next;
	}

	if (c->m_nodeA.next) {
		c->m_nodeA.next->prev = c->m_nodeA.prev;
	}

	if (&c->m_nodeA == bodyA->m_contactList) {
		bodyA->m_contactList = c->m_nodeA.next;
	}

	// Remove the contact from body B's contact list.
	if (c->m_nodeB.prev) {
		c->m_nodeB.prev->next = c->m_nodeB.next;
	}

	if (c->m_nodeB.next) {
		c->m_nodeB.next->prev = c->m_nodeB.prev;
	}

	if (&c->m_nodeB == bodyB->m_contactList) {
		bodyB->m_contactList = c->m_nodeB.next;
	}

	// Remove the contact from the heap.
	c->~b3Contact();
	m_blockAllocator->Free(c, sizeof(b3Contact));
	--m_contactCount;
}

void b3ContactGraph::FindNewContacts() {
	// Find potentially colliding shapes.
	m_broadPhase.FindOverlappingPairs(this);
}

void b3ContactGraph::UpdateContacts() {
	// Update all contact constraints and its states.
	b3Contact* c = m_contactList;
	while (c) {
		const b3Shape* shapeA = c->m_shapeA;
		const b3Shape* shapeB = c->m_shapeB;

		b3Body* bodyA = c->m_shapeA->m_body;
		b3Body* bodyB = c->m_shapeB->m_body;

		if (bodyA == bodyB) {
			b3Contact* quack = c;
			c = c->m_next;
			DestroyContact(quack);
			continue;
		}

		bool activeA = bodyA->IsAwake() && (bodyA->m_type != e_staticBody);
		bool activeB = bodyB->IsAwake() && (bodyB->m_type != e_staticBody);
		if (!activeA && !activeB) {
			c = c->m_next;
			continue;
		}

		// Destroy the contact if is definately persistenting.
		if (!m_broadPhase.TestOverlap(shapeA->broadPhaseID, shapeB->broadPhaseID)) {
			b3Contact* quack = c;
			c = c->m_next;
			DestroyContact(quack);
			continue;
		}

		bool wasTouching = c->IsTouching();
		bool isTouching = false;
		bool isSensorContact = shapeA->IsSensor() || shapeB->IsSensor();

		if (isSensorContact) {
			// Simply, a sensor is active if its bounds is touching with another other shape's bounds.
			isTouching = m_broadPhase.TestOverlap(shapeA->broadPhaseID, shapeB->broadPhaseID);
			c->m_manifold.pointCount = 0;
		}
		else {
			// For the time being, Bounce can compute the closest distance between convex objects only.
			typedef void(*b3DistanceQuery) (b3Manifold&, const b3Transform&, const b3Shape*, const b3Transform&, const b3Shape*);

			// Simply, register the collision routines here.
			static b3DistanceQuery queryMatrix[e_maxShapes][e_maxShapes] = {
				{ &b3HullHullShapeContact },
			};

			b3ShapeType typeA = shapeA->GetType();
			b3ShapeType typeB = shapeB->GetType();

			b3Assert(typeA <= typeB);
			b3DistanceQuery Query = queryMatrix[typeA][typeB];
			b3Assert(Query);

			// Copy the old manifold so we can compare the new contact points with it.
			b3Manifold oldManifold = c->m_manifold;

			// Compute the a new contact manifold.
			c->m_manifold.pointCount = 0;
			Query(c->m_manifold, bodyA->m_transform * shapeA->m_local, shapeA, bodyB->m_transform * shapeB->m_local, shapeB);

			isTouching = c->m_manifold.pointCount > 0;

			// Look up the contact cache for identical contact points.
			b3Manifold* newManifold = &c->m_manifold;

			b3Vec3 normal = newManifold->normal;

			b3Vec3 xA = bodyA->m_worldCenter;
			b3Vec3 vA = bodyA->m_linearVelocity;
			b3Vec3 wA = bodyA->m_angularVelocity;

			b3Vec3 xB = bodyB->m_worldCenter;
			b3Vec3 vB = bodyB->m_linearVelocity;
			b3Vec3 wB = bodyB->m_angularVelocity;

			for (u32 i = 0; i < newManifold->pointCount; ++i) {
				b3ContactPoint* p2 = newManifold->points + i;

				b3Vec3 position = p2->position;
				b3Vec3 tangent1;
				b3Vec3 tangent2;

				p2->normalImpulse = B3_ZERO;
				p2->tangentImpulse[0] = B3_ZERO;
				p2->tangentImpulse[1] = B3_ZERO;
				p2->warmStarted = false;

				// Compute the (two) new tangent directions.
				b3Vec3 rA = position - xA;
				b3Vec3 rB = position - xB;

				b3Vec3 dv = vB + b3Cross(wB, rB) - vA - b3Cross(wA, rA);

				tangent1 = dv - b3Dot(dv, normal) * normal;
				r32 tangentMag = b3Dot(tangent1, tangent1);
				if (tangentMag > B3_EPSILON) {
					tangent1 *= B3_ONE / b3Sqrt(tangentMag);
					tangent2 = b3Cross(tangent1, normal);
				}
				else {
					b3ComputeBasis(normal, &tangent1, &tangent2);
				}

				p2->tangents[0] = tangent1;
				p2->tangents[1] = tangent2;

				// Initialize new contact point with the old contact point solution if they're identical.
				for (u32 j = 0; j < oldManifold.pointCount; ++j) {
					b3ContactPoint* p1 = oldManifold.points + j;
					if (p1->id.key == p2->id.key) {
						// Copy normal impulse.
						p2->warmStarted = true;
						p2->normalImpulse = p1->normalImpulse;
						// Project old friction solutions into the new tangential directions.
						b3Vec3 oldFrictionSolution = p1->tangentImpulse[0] * p1->tangents[0] + p1->tangentImpulse[1] * p1->tangents[1];
						p2->tangentImpulse[0] = b3Dot(oldFrictionSolution, p2->tangents[0]);
						p2->tangentImpulse[1] = b3Dot(oldFrictionSolution, p2->tangents[1]);
						break;
					}
				}
			}

			// If the contact has begun then awake the bodies.
			if (isTouching != wasTouching) {
				bodyA->SetAwake(true);
				bodyB->SetAwake(true);
			}
		}

		// Mark the contact as touching.
		if (isTouching) {
			c->m_flags |= b3Contact::e_touchingFlag;
		}
		else {
			c->m_flags &= ~b3Contact::e_touchingFlag;
		}

		// Notify the contact listener the contact state.
		if (m_contactListener) {
			if (!wasTouching && isTouching) {
				m_contactListener->BeginContact(c);
			}
			if (wasTouching && !isTouching) {
				m_contactListener->EndContact(c);
			}
			if (!isSensorContact && isTouching) {
				m_contactListener->Persisting(c);
			}
		}
		// Go to the next contact.
		c = c->m_next;
	}
}
