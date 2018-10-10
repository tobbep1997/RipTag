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

#ifndef __B3_SAT_H__
#define __B3_SAT_H__

#include "..\Collision\b3Collision.h"
#include "..\Common\Memory\b3ArrayPOD.h"

class b3Shape;
struct b3Hull;

struct b3ClipVertex {
	b3Vec3 position;
	b3FeaturePair featurePair;
};

struct b3ClipPlane {
	b3Plane plane;
	u8 edgeId;
};

typedef b3ArrayPOD<b3ClipVertex, u32> b3ClipPolygon;

typedef b3ArrayPOD<b3ClipPlane, u32> b3ClipPlanes;

struct b3FaceQuery {
	i32 index;
	r32 distance;
};

struct b3EdgeQuery {
	i32 index1;
	i32 index2;
	r32 distance;
};

// SAT collision detection entry point.
void b3HullHullShapeContact(b3Manifold& output, const b3Transform& transform1, const b3Shape* shape1, const b3Transform& transform2, const b3Shape* shape2);
void b3HullHullContact(b3Manifold& output, const b3Transform& transform1, const b3Hull* hull1, const b3Transform& transform2, const b3Hull* hull2);

#endif
