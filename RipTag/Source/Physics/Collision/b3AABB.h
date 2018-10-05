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

#ifndef __B3_AABB_H__
#define __B3_AABB_H__

#include "..\Common\Math\b3Math.h"

struct b3AABB {
	// Compute this AABB from a list of points.
	void ComputeAabbFromPointArray(const b3Vec3* verts, u32 vertCount) {
		min = max = verts[0];		
		for (u32 i = 1; i < vertCount; ++i) {
			min = b3Min(min, verts[i]);
			max = b3Max(max, verts[i]);
		}
	}
	
	// Compute this AABB from a list of points and a transform.
	void ComputeAabbFromPointArray(const b3Vec3* verts, u32 vertCount, const b3Transform& transform) {
		min = max = transform * verts[0];
		for (u32 i = 1; i < vertCount; ++i) {
			b3Vec3 v = transform * verts[i];
			min = b3Min(min, v);
			max = b3Max(max, v);
		}
	}

	// Extend this AABB bounds by value.
	void Extend(r32 value) {
		min.x -= value;
		min.y -= value;
		min.z -= value;
		
		max.x += value;
		max.y += value;
		max.z += value;
	}

	// Get the this AABB center.
	b3Vec3 GetCenter() const {
		return  B3_HALF * (min + max);
	}

	r32 Width() const {
		return max.x - min.x;
	}

	r32 Height() const {
		return max.y - min.y;
	}

	r32 Depth() const {
		return max.z - min.z;
	}

	r32 Volume() const {
		return Width() * Height() * Depth();
	}

	r32 SurfaceArea() const {
		return B3_TWO * ( Width() * Depth() + Width() * Height() + Depth() * Height() );
	}

	// Test if a point is inside the AABB.
	bool ContainsPoint(const b3Vec3& point) const {
		return min.x <= point.x && point.x <= max.x &&
			min.y <= point.y && point.y <= max.y &&
			min.z <= point.z && point.z <= max.z;
	}

	// Test if a AABB is inside this AABB.
	bool ContainsAabb(const b3AABB& aabb) const {
		return ContainsPoint(aabb.min) && ContainsPoint(aabb.max);
	}
	
	// Intersect ray R(t) = p + t*d against this AABB.
	bool RayCast(const b3Vec3& p, const b3Vec3& d, r32 tmax, r32& tmin) const {
		// Christer Ericson, Real-Time Collision Detection (2005), p. 180.
		// Correction: http://realtimecollisiondetection.net/books/rtcd/errata/

		// Test all three slabs.
		for (u32 i = 0; i < 3; i++) {
			if (b3Abs(d[i]) < B3_EPSILON) {
				// The segment is parallel to slab. No hit if origin not within slab.
				if (p[i] < min[i] || p[i] > max[i]) {
					return false; 
				}
			}
			else {
				// Compute intersection t value of segment with near and far plane of slab.
				r32 ood = B3_ONE / d[i];
				r32 t1 = ood * (min[i] - p[i]);
				r32 t2 = ood * (max[i] - p[i]);

				// Make t1 be intersection with near plane, t2 with far plane.
				if (t1 > t2) {
					b3Swap(t1, t2);
				}

				// Compute the intersection of slab intersection intervals
				tmin = b3Max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
				tmax = b3Min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;

				// Exit with no collision as soon as slab intersection becomes empty.
				if (tmin > tmax) { 
					return false; 
				}
			}
		}

		return true;
	}

	b3Vec3 min;
	b3Vec3 max;
};

// Compute an AABB from two AABBs.
inline b3AABB Combine(const b3AABB& aabb1, const b3AABB& aabb2) {
	b3AABB aabb;
	aabb.min = b3Min(aabb1.min, aabb2.min);
	aabb.max = b3Max(aabb1.max, aabb2.max);
	return aabb;
}

#endif
