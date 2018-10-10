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

#ifndef __B3_POLYHEDRON_H__
#define __B3_POLYHEDRON_H__

#include "b3Shape.h"

struct b3Hull;

class b3Polyhedron : public b3Shape {
public :
	b3Polyhedron();
	~b3Polyhedron();

	virtual b3ShapeType GetType() const { return e_hull; };

	virtual void ComputeMass(b3MassData* massData, r32 density) const;
	virtual void ComputeAabb(b3AABB& output, const b3Transform& transform) const;
	virtual bool RayCast(const b3RayCastInput& input, b3RayCastOutput& output, const b3Transform& transform) const;

	//@warning A hull can be shared therefore a pointer to it is kept instead of an instance.
	const b3Hull* GetHull() const;	
	
	void SetHull(const b3Hull* hull);
protected :
	const b3Hull* m_hull;
};

inline const b3Hull* b3Polyhedron::GetHull() const { return m_hull; }

inline void b3Polyhedron::SetHull(const b3Hull* hull) {
	m_hull = hull;
}

#endif
