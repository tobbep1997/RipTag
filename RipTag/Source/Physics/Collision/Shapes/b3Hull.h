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

#ifndef __B3_HULL_H__
#define __B3_HULL_H__

#include "..\..\Common\Math\b3Math.h"

/*****************************************************
The half-edge structure is an edge-centric mesh.
A half-edge based mesh will increase the performance 
of the SAT and it will facilitate later collision 
detection optimizations.
*****************************************************/

#define NULL_FEATURE (-1)

struct b3Face {
	i32 edge;
};

struct b3HalfEdge {
	i32 prev;
	i32 next;
	i32 twin;
	i32 face;
	i32 origin;
};

struct b3FaceDef {
	u32 vertexCount;
	const u32* vertices;
};

struct b3HullDef {
	b3HullDef() {
		faceCount = 0;
		faces = nullptr;
		vertexCount = 0;
		vertices = nullptr;
	}
	u32 faceCount;
	const b3FaceDef* faces;
	u32 vertexCount;
	const b3Vec3* vertices;
};

struct b3Hull {
	b3Hull();	
	~b3Hull();

	// Initialize the convex hull from an array of faces.
	// The face vertices must be ordered clock-wise (CW).
	void SetFromFaces(const b3HullDef& def);
	// Create the hull planes using Newell's method.
	// @todo Shift vertices to origin.
	void CreateFacesPlanes(const b3HullDef& def);
	// Set the hull as a box given the box extents.
	void SetAsBox(const b3Vec3& scale);
		
	// Validate the half-edge data structure.
	// This function must be called after you have setup the hull 
	// by yourself.
	void Validate() const;
	void Validate(const b3Face* face) const;
	void Validate(const b3HalfEdge* halfEdge) const;

	// Get a supporting vertex of the hull in a given direction.
	b3Vec3 GetSupport(const b3Vec3& direction) const;

	// Get a supporting vertex given a direction an transform.
	b3Vec3 GetSupport(const b3Vec3& direction, const b3Transform& transform) const;

	const b3Vec3& GetVertex(u32 i) const;
	const b3Plane& GetPlane(u32 i) const;
	const b3Face* GetFace(u32 i) const;
	const b3HalfEdge* GetEdge(u32 i) const;

	u32 vertexCount;
	b3Vec3* vertices;

	u32 faceCount;
	b3Face* faces;
	b3Plane* facesPlanes; 

	u32 edgeCount;
	b3HalfEdge* edges;
};

inline const b3Vec3& b3Hull::GetVertex(u32 i) const { 
	return vertices[i]; 
}

inline const b3Plane& b3Hull::GetPlane(u32 i) const { 
	return facesPlanes[i]; 
}

inline const b3Face* b3Hull::GetFace(u32 i) const { 
	return faces + i; 
}

inline const b3HalfEdge* b3Hull::GetEdge(u32 i) const { 
	return edges + i; 
}

#endif
