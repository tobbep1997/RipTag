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

#ifndef __B3_SHAPE_H__
#define __B3_SHAPE_H__

#include "..\b3Collision.h"

class b3Body;
class b3Shape;
class b3BlockAllocator;

struct b3ShapeDef {
	b3ShapeDef() {
		density = B3_ZERO;
		friction = B3_ZERO;
		restitution = B3_ZERO;
		shape = nullptr;
		userData = nullptr;
		sensor = false;
		local.SetIdentity();
	}

	const b3Shape* shape;
	void* userData;
	bool sensor;
	r32 density;
	r32 friction;
	r32 restitution;
	b3Transform local;
};

struct b3MassData {
	r32 mass;
	b3Mat33 I;
	b3Vec3 center;
};

enum b3ShapeType {
	e_hull,
	e_maxShapes
};

class b3Shape {
public :
	// A shape is created and initialized by the b3World class.
	b3Shape() { }
	virtual ~b3Shape() { }

	virtual b3ShapeType GetType() const = 0;
	virtual void ComputeMass(b3MassData* massData, r32 density) const = 0;
	virtual void ComputeAabb(b3AABB& output, const b3Transform& transform) const = 0;
	virtual bool RayCast(const b3RayCastInput& input, b3RayCastOutput& output, const b3Transform& transform) const = 0;

	// Manipulating a shape transform during the simulation may cause non-physical behaviours.
	const b3Transform& GetTransform() const;
	void SetTransform(const b3Vec3& position, const b3Vec3& axis, r32 radians);

	b3Body* GetBody();
	const b3Body* GetBody() const;

	bool IsSensor() const;
	void SetSensor(bool flag);

	r32 GetDensity() const;
	void SetDensity(r32 density);

	r32 GetRestitution() const;
	void SetRestitution(r32 restitution);

	void* GetUserData() const;
	void SetUserData(void* data);

	b3Shape* GetNext();
	const b3Shape* GetNext() const;
protected :
	friend class b3World;
	friend class b3Body;
	friend class b3ContactGraph;

	r32 m_density;
	r32 m_restitution;
	r32 m_friction;
	i32 broadPhaseID;
	bool m_isSensor;
	void* m_userData;

	b3Body* m_body;
	b3Transform m_local;

	b3Shape* m_next;
};

inline b3Body* b3Shape::GetBody() { 
	return m_body; 
}

inline const b3Body* b3Shape::GetBody() const { 
	return m_body; 
}

inline const b3Transform& b3Shape::GetTransform() const { 
	return m_local; 
}

inline void b3Shape::SetTransform(const b3Vec3& position, const b3Vec3& axis, r32 radians) {
	b3Quaternion q;
	q.Set(axis, radians);
	q.ToRotationMatrix(m_local.rotation);
	m_local.translation = position;
}

inline bool b3Shape::IsSensor() const { 
	return m_isSensor; 
}

inline r32 b3Shape::GetDensity() const { 
	return m_density; 
}

inline void b3Shape::SetDensity(r32 density) { 
	m_density = density; 
}

inline r32 b3Shape::GetRestitution() const { 
	return m_restitution; 
}

inline void b3Shape::SetRestitution(r32 restitution) { 
	m_restitution = restitution; 
}

inline void* b3Shape::GetUserData() const { 
	return m_userData; 
}

inline void b3Shape::SetUserData(void* data) { 
	m_userData = data; 
}

inline b3Shape* b3Shape::GetNext() {
	return m_next;
}

inline const b3Shape* b3Shape::GetNext() const {
	return m_next;
}

#endif
