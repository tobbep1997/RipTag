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

#ifndef __B3_MATH_H__
#define __B3_MATH_H__

#include <math.h>
#include "b3Vec3.h"
#include "b3Mat33.h"
#include "b3Quaternion.h"

template <class T>
inline T b3Min(T a, T b) {
	return a < b ? a : b;
}

template <class T>
inline T b3Max(T a, T b) {
	return a > b ? a : b;
}

template <class T>
inline T b3Clamp(T a, T low, T high) {
	return b3Max(low, b3Min(a, high));
}

inline b3Vec3 b3Min(const b3Vec3& a, const b3Vec3& b) {
	return b3Vec3(b3Min(a.x, b.x), b3Min(a.y, b.y), b3Min(a.z, b.z));
}

inline b3Vec3 b3Max(const b3Vec3& a, const b3Vec3& b) {
	return b3Vec3(b3Max(a.x, b.x), b3Max(a.y, b.y), b3Max(a.z, b.z));
}
// Compute u = (A^T)v.
inline b3Vec3 b3MulT(const b3Mat33& A, const b3Vec3& v) {
	return b3Vec3(b3Dot(A.x, v), b3Dot(A.y, v), b3Dot(A.z, v));
}

// Compute C = (A^T)B.
inline b3Mat33 b3MulT(const b3Mat33& A, const b3Mat33& B) {
	return 
		b3Mat33(
		b3Vec3(b3Dot(A.x, B.x), b3Dot(A.y, B.x), b3Dot(A.z, B.x)),
		b3Vec3(b3Dot(A.x, B.y), b3Dot(A.y, B.y), b3Dot(A.z, B.y)),
		b3Vec3(b3Dot(A.x, B.z), b3Dot(A.y, B.z), b3Dot(A.z, B.z)));
}

// A transform represent a rigid body frame. 
// It has origin and basis.
struct b3Transform {
	void SetIdentity() {
		translation.SetZero();
		rotation.SetIdentity();
	}
	b3Vec3 translation;
	b3Mat33 rotation;
};

// Map v to T.
inline b3Vec3 operator*(const b3Transform& T, const b3Vec3& v) {
	return (T.rotation * v) + T.translation;
}

// Map B to A.
inline b3Transform operator*(const b3Transform& A, const b3Transform& B) {
	b3Transform C;
	C.rotation = A.rotation * B.rotation;
	C.translation = (A.rotation * B.translation) + A.translation;
	return C;
}

// Multiply transform B by the transpose of A.
inline b3Transform b3MulT(const b3Transform& A, const b3Transform& B) {
	b3Transform C;
	C.rotation = b3MulT(A.rotation, B.rotation);
	C.translation = b3MulT(A.rotation, B.translation - A.translation);
	return C;
}

// Multiply the vector v by the transpose of A. Useful for converting
// the vector from world to local coordinates.
inline b3Vec3 b3MulT(const b3Transform& A, const b3Vec3& v) {
	return b3MulT(A.rotation, v - A.translation);
}

struct b3Plane {
	b3Plane() { }

	b3Plane(const b3Vec3& _normal, r32 _offset) :
		normal(_normal),
		offset(_offset) {
	}

	// Assuming a, b, and c are ordered CCW, the 
	// normal will point to the viewer.
	b3Plane(const b3Vec3& a, const b3Vec3& b, const b3Vec3& c) {
		normal = b3Normalize(b3Cross(b - a, c - a));
		offset = b3Dot(normal, a);
	}

	b3Vec3 normal;
	r32 offset;
};

// Compute the (signed) distance from a plane to a point.
inline r32 b3Distance(const b3Plane& plane, const b3Vec3& point) {
	return b3Dot(plane.normal, point) - plane.offset;
}

// Compute the closest point on a plane given the point and the plane.
inline b3Vec3 b3ClosestPoint(const b3Plane& plane, const b3Vec3& point) {
	// The plane must be normalized.
	r32 fraction = b3Distance(plane, point);
	return point - fraction * plane.normal;
}

// Transform a plane by a given frame.
inline b3Plane operator*(const b3Transform& T, const b3Plane& plane) {
	b3Vec3 normal = T.rotation * plane.normal;
	return b3Plane(normal, plane.offset + b3Dot(normal, T.translation));
}

#endif
