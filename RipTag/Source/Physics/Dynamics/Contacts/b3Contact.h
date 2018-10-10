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

#ifndef __B3_CONTACT_H__
#define __B3_CONTACT_H__

#include "..\..\Collision\b3Collision.h"

class b3Contact;
class b3Shape;
class b3Body;

// The idea is to allow anything to bounce off an inelastic surface (i.e. e == 0).
inline r32 b3MixRestitution(r32 e1, r32 e2)  {
	return b3Max(e1, e2);
}

// The idea is to drive the restitution to zero. 
inline r32 b3MixFriction(r32 u1, r32 u2) {
	return b3Sqrt(u1 * u2);
}

// The contact edge for the contact graph.
struct b3ContactEdge {
	b3Contact* contact;
	b3Body* other;
	b3ContactEdge* prev;
	b3ContactEdge* next;
};

class b3Contact {
public :
	b3Shape* GetShapeA();
	const b3Shape* GetShapeA() const;

	b3Shape* GetShapeB();
	const b3Shape* GetShapeB() const;

	bool IsTouching() const;
protected :
	enum b3ContactFlags {
		e_touchingFlag = 0x0001,
		e_islandFlag = 0x0002,
	};

	friend class b3ContactGraph;
	friend class b3ContactSolver;
	friend class b3World;
	friend class b3Body;

	b3Contact() { }
	~b3Contact() { }

	b3Shape* m_shapeA;
	b3Shape* m_shapeB;
	r32 m_friction;
	r32 m_restitution;
	u32 m_flags;
	b3Manifold m_manifold;

	b3ContactEdge m_nodeA;
	b3ContactEdge m_nodeB;

	b3Contact* m_prev;
	b3Contact* m_next;
};

inline bool b3Contact::IsTouching() const {
	return (m_flags & e_touchingFlag) != 0;
}

inline b3Shape* b3Contact::GetShapeA() {
	return m_shapeA;
}

inline const b3Shape* b3Contact::GetShapeA() const {
	return m_shapeA;
}

inline b3Shape* b3Contact::GetShapeB() {
	return m_shapeB;
}

inline const b3Shape* b3Contact::GetShapeB() const {
	return m_shapeB;
}

#endif
