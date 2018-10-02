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

#include "b3Body.h"
#include "b3World.h"
#include "..\Collision\Shapes\b3Polyhedron.h"
#include "Contacts\b3Contact.h"
#include "Joints\b3Joint.h"

b3Body::b3Body(const b3BodyDef& def, b3World* world) {
	m_world = world;
	m_type = def.type;
	m_flags = 0;
	if (def.awake) {
		m_flags |= e_awakeFlag;
	}

	if (m_type == e_dynamicBody) {
		// Dynamic bodies has positive mass.
		m_mass = B3_ONE;
		m_invMass = B3_ONE;
	}
	else {
		m_mass = B3_ZERO;
		m_invMass = B3_ZERO;
	}

	m_userData = def.userData;
	m_worldCenter = def.position;
	m_orientation = def.orientation;
	m_linearVelocity = def.linearVelocity;
	m_angularVelocity = def.angularVelocity;
	m_gravityScale = def.gravityScale;

	// Calculate the world transform.
	m_localCenter.SetZero();
	m_transform.translation = m_worldCenter;
	m_orientation.ToRotationMatrix(m_transform.rotation);
	m_force.SetZero();
	m_torque.SetZero();
	m_invLocalInertia.SetZero();
	m_invWorldInertia.SetZero();
	
	m_islandID = -1;
	m_shapeList = nullptr;
	m_shapeCount = 0;
	m_contactList = nullptr;
	m_jointList = nullptr;
	m_sleepTime = B3_ZERO;
}

b3Shape* b3Body::CreateShape(const b3ShapeDef& def) {
	b3Assert(def.shape);

	// The shape definition shape will be cloned.
	b3Shape* shape = nullptr;
	switch (def.shape->GetType()) {
	case e_hull: {
		// Grab pointer to the specific memory.
		b3Polyhedron* poly1 = (b3Polyhedron*)def.shape;
		void* mem = m_world->m_blockAllocator.Allocate(sizeof(b3Polyhedron));
		b3Polyhedron* poly2 = new (mem)b3Polyhedron();
		// Clone the polyhedra.
		*poly2 = *poly1;
		shape = poly2;
		break;
	}
	default: {
		b3Assert(false);
		break;
	}
	}

	b3Assert(shape);

	// Setup the shape with the definition.
	shape->m_body = this;
	shape->m_userData = def.userData;
	shape->m_local = def.local;
	shape->m_density = def.density;
	shape->m_friction = def.friction;
	shape->m_restitution = def.restitution;
	shape->m_isSensor = def.sensor;

	// Link the new shape into the singly-linked list of shapes of this body.
	shape->m_next = m_shapeList;
	m_shapeList = shape;
	++m_shapeCount;

	// Since a new shape was added the new inertial properties of 
	// this body needs to be recomputed (mass, inertial, local and world center of mass).
	if (shape->m_density > B3_ZERO) {
		ResetMassData();
	}

	// Compute the world AABB of the new shape and assign a broad-phase proxy to it.
	b3AABB aabb;
	shape->ComputeAabb(aabb, m_transform * shape->m_local);
	shape->broadPhaseID = m_world->m_contactGraph.m_broadPhase.CreateProxy(aabb, shape);

	// Tell the world that a new shape was added to initially verify new contacts.
	m_world->m_flags |= b3World::e_bodyAddedFlag;

	return shape;
}

void b3Body::DestroyContacts() {
	// Quick function to remove all contacts associated with this body.
	b3ContactEdge* ce = m_contactList;
	while (ce) {
		b3ContactEdge* ce0 = ce;
		ce = ce->next;
		m_world->m_contactGraph.DestroyContact(ce0->contact);
	}
	m_contactList = nullptr;
}

void b3Body::DestroyJoints() {
	// Quick function to remove all joints connected with this body.
	b3JointEdge* je = m_jointList;
	while (je) {
		b3JointEdge* je0 = je;
		je = je->next;
		m_world->m_jointGraph.DestroyJoint(je0->joint);
	}
	m_jointList = nullptr;
}

void b3Body::DestroyContacts(const b3Shape* shape) {
	// Quick function to remove all contacts associated with a shape.
	b3ContactEdge* edge = m_contactList;
	while (edge) {
		b3Contact* c = edge->contact;
		edge = edge->next;
		if (shape == c->m_shapeA || shape == c->m_shapeB) {
			// Destroy the contact and remove it from this body contact list.
			m_world->m_contactGraph.DestroyContact(c);
		}
	}
}

void b3Body::DestroyShape(b3Shape* shape) {
	// Remove (and destroy) the given shape from this body.
#ifdef _DEBUG
	// Make sure the shape belongs to this body.
	b3Assert(shape->m_body == this);
	b3Assert(m_shapeCount > 0);

	b3Shape** node = &m_shapeList;
	bool found = false;
	while (*node) {
		if (*node == shape) {
			*node = shape->m_next;
			found = true;
			break;
		}
		node = &(*node)->m_next;
	}

	b3Assert(found);
#endif
	// Destroy any contacts associated with the shape.
	DestroyContacts(shape);

	// Destroy the broad-phase proxy associated with the shape.
	b3BlockAllocator* allocator = &m_world->m_blockAllocator;
	m_world->m_contactGraph.m_broadPhase.DestroyProxy(shape->broadPhaseID);

	shape->m_body = nullptr;
	shape->m_next = nullptr;

	// Deallocate shape from the heap.
	switch (shape->GetType()) {
	case e_hull: {
		b3Polyhedron* poly = (b3Polyhedron*)shape;
		poly->~b3Polyhedron();
		allocator->Free(poly, sizeof(b3Polyhedron));
		break;
	}
	default: {
		b3Assert(false);
		break;
	}
	}

	--m_shapeCount;

	// Since a shape was removed, the new inertial properties of 
	// this body needs to be recomputed (mass, inertial, local and world center of mass).
	ResetMassData();
}

void b3Body::DestroyShapes() {
	// Destroy all shapes that belong to this body.
	// Make sure all shapes belong to this body.
	b3Shape* s = m_shapeList;
	while (s) {
		b3Shape* shape = s;
		s = shape->m_next;

		b3Assert(shape->m_body == this);
		b3Assert(m_shapeCount > 0);

		b3Shape** node = &m_shapeList;
		bool found = false;
		while (*node) {
			if (*node == shape) {
				*node = shape->m_next;
				found = true;
				break;
			}
			node = &(*node)->m_next;
		}

		b3Assert(found);

		// Destroy all contacts associated with the shape.
		DestroyContacts(shape);
		// Destroy the shape broad-phase proxy.
		b3BlockAllocator* allocator = &m_world->m_blockAllocator;
		m_world->m_contactGraph.m_broadPhase.DestroyProxy(shape->broadPhaseID);

		shape->m_body = nullptr;
		shape->m_next = nullptr;

		// Deallocate the shape.
		switch (shape->GetType()) {
		case e_hull : {
			b3Polyhedron* poly = (b3Polyhedron*)shape;
			poly->~b3Polyhedron();
			allocator->Free(poly, sizeof(b3Polyhedron));
			break;
		}
		default : {
			b3Assert(false);
			break;
		}
		}
	}
	// Reset list of shapes.
	m_shapeList = nullptr;
	m_shapeCount = 0;
}

void b3Body::SynchronizeTransform() {
	// Compute the world transform of this body using
	// the world center of mass and the orientation quaternion.
	m_orientation.Normalize();
	m_orientation.ToRotationMatrix(m_transform.rotation);
	m_transform.translation = m_worldCenter - m_transform.rotation * m_localCenter;

	// Recompute world inverse inertia tensor.
	m_invWorldInertia = m_transform.rotation * m_invLocalInertia * b3Transpose(m_transform.rotation);
}

void b3Body::SynchronizeShapes() {
	// Sweep the shapes attached to this body and compute its new AABBs.
	for (b3Shape* s = m_shapeList; s; s = s->m_next) {
		b3AABB aabb;
		s->ComputeAabb(aabb, m_transform * s->m_local);
		// Update the proxy AABB with the new (transformed) AABB.
		m_world->m_contactGraph.m_broadPhase.UpdateProxy(s->broadPhaseID, aabb);
	}
}

void b3Body::ResetMassData() {
	// Set mass and inertia tensor to zero.
	m_mass = B3_ZERO;
	m_invMass = B3_ZERO;
	m_invLocalInertia.SetZero();
	m_invWorldInertia.SetZero();

	if (m_type == b3BodyType::e_staticBody) {
		return;
	}

	// Compute the local center of mass of this body based on the body shape centroids.
	b3Vec3 localCenter;
	localCenter.SetZero();
	b3Mat33 localInertia;
	localInertia.SetZero();
	for (b3Shape* s = m_shapeList; s; s = s->m_next) {
		if (s->m_density == B3_ZERO) {
			continue;
		}
		b3MassData massData;
		s->ComputeMass(&massData, s->m_density);
		m_mass += massData.mass;
		localCenter += massData.mass * massData.center;
		localInertia += massData.I;
	}

	if (m_mass > B3_ZERO) {
		m_invMass = B3_ONE / m_mass;
		localCenter *= m_invMass;
		m_invLocalInertia = b3Inverse(localInertia);
		m_invWorldInertia = m_transform.rotation * m_invLocalInertia * b3Transpose(m_transform.rotation);
	}
	else {
		// Dynamic bodies has positive mass.
		m_mass = B3_ONE;
		m_invMass = B3_ONE;
	}

	// Update this body world center of mass position and velocity.
	m_localCenter = localCenter;
	b3Vec3 oldWorldCenter = m_worldCenter;
	m_worldCenter = m_transform * m_localCenter;
	m_linearVelocity += b3Cross(m_angularVelocity, m_worldCenter - oldWorldCenter);
}
