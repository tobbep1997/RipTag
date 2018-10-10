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

#include "b3JointGraph.h"
#include "..\b3Body.h"
#include "..\..\Common\Memory\b3BlockAllocator.h"
#include "b3MouseJoint.h"
#include "b3SphericalJoint.h"
#include "b3RevoluteJoint.h"

b3JointGraph::b3JointGraph() {
	m_blockAllocator = nullptr;
	m_jointList = nullptr;
	m_jointCount = 0;
}

b3Joint* b3JointGraph::CreateJoint(const b3JointDef* def) {
	b3Body* bodyA = def->bodyA;
	b3Body* bodyB = def->bodyB;

	// Allocate the new joint.
	b3Joint* j = AllocateJoint(def);
	j->m_onIsland = false;

	// Add the joint to the joint graph joint list.
	j->m_prev = nullptr;
	j->m_next = m_jointList;
	if (m_jointList) {
		// If there is a joint then adjust the joint pointer.
		m_jointList->m_prev = j;
	}
	m_jointList = j;

	// Add the joint to body A's joint contact list.
	j->m_nodeA.joint = j;
	j->m_nodeA.other = bodyB;
	j->m_nodeA.prev = nullptr;
	j->m_nodeA.next = bodyA->m_jointList;
	if (bodyA->m_jointList) {
		bodyA->m_jointList->prev = &j->m_nodeA;
	}
	bodyA->m_jointList = &j->m_nodeA;

	// Add the joint to body B's joint joint list.
	j->m_nodeB.joint = j;
	j->m_nodeB.other = bodyA;
	j->m_nodeB.prev = nullptr;
	j->m_nodeB.next = bodyB->m_jointList;
	if (bodyB->m_jointList) {
		bodyB->m_jointList->prev = &j->m_nodeB;
	}
	bodyB->m_jointList = &j->m_nodeB;

	++m_jointCount;

	return j;
}

void b3JointGraph::DestroyJoint(b3Joint* j) {
	b3Body* bodyA = j->m_bodyA;
	b3Body* bodyB = j->m_bodyB;

	// Remove the joint from the joint graph joint list.
	if (j->m_prev) {
		j->m_prev->m_next = j->m_next;
	}

	if (j->m_next) {
		j->m_next->m_prev = j->m_prev;
	}

	if (j == m_jointList) {
		m_jointList = j->m_next;
	}

	// Remove the joint from body A's joint list.
	if (j->m_nodeA.prev) {
		j->m_nodeA.prev->next = j->m_nodeA.next;
	}

	if (j->m_nodeA.next) {
		j->m_nodeA.next->prev = j->m_nodeA.prev;
	}

	if (&j->m_nodeA == bodyA->m_jointList) {
		bodyA->m_jointList = j->m_nodeA.next;
	}

	// Remove the joint from body B's joint list.
	if (j->m_nodeB.prev) {
		j->m_nodeB.prev->next = j->m_nodeB.next;
	}

	if (j->m_nodeB.next) {
		j->m_nodeB.next->prev = j->m_nodeB.prev;
	}

	if (&j->m_nodeB == bodyB->m_jointList) {
		bodyB->m_jointList = j->m_nodeB.next;
	}

	// Remove the joint from the memory.
	FreeJoint(j);
	
	--m_jointCount;
}

b3Joint* b3JointGraph::AllocateJoint(const b3JointDef* def) const {
	b3Joint* joint = nullptr;
	b3JointType type = def->GetType();
	switch (type) {
	case b3JointType::e_mouseJoint: {
		void* memory = m_blockAllocator->Allocate(sizeof(b3MouseJoint));
		joint = new (memory) b3MouseJoint((b3MouseJointDef*)def);
		break;
	}
	case b3JointType::e_sphericalJoint: {
		void* memory = m_blockAllocator->Allocate(sizeof(b3SphericalJoint));
		joint = new (memory) b3SphericalJoint((b3SphericalJointDef*)def);
		break;
	}
	case b3JointType::e_revoluteJoint: {
		void* memory = m_blockAllocator->Allocate(sizeof(b3RevoluteJoint));
		joint = new (memory) b3RevoluteJoint((b3RevoluteJointDef*)def);
		break;
	}
	default: {
		b3Assert(false);
		break;
	}
	}
	return joint;
}

void b3JointGraph::FreeJoint(b3Joint* joint) const {
	b3Assert(joint);

	b3JointType type = joint->GetType();
	switch (type) {
	case b3JointType::e_mouseJoint: {
		b3MouseJoint* memory = (b3MouseJoint*)joint;
		memory->~b3MouseJoint();
		m_blockAllocator->Free(memory, sizeof(b3MouseJoint));
		break;
	}
	case b3JointType::e_sphericalJoint: {
		b3SphericalJoint* memory = (b3SphericalJoint*)joint;
		memory->~b3SphericalJoint();
		m_blockAllocator->Free(memory, sizeof(b3SphericalJoint));
		break;
	}
	case b3JointType::e_revoluteJoint: {
		b3RevoluteJoint* memory = (b3RevoluteJoint*)joint;
		memory->~b3RevoluteJoint();
		m_blockAllocator->Free(memory, sizeof(b3RevoluteJoint));
		break;
	}
	default: {
		b3Assert(false);
		break;
	}
	}
}
