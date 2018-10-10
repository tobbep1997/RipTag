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

#include "b3Hull.h"
#include "..\..\Common\Memory\b3Array.h"
#include "..\..\Common\Memory\b3ArrayPOD.h"
#include <map>

b3Hull::b3Hull() :
	vertexCount(0),
	vertices(nullptr),
	faceCount(0),
	faces(nullptr),
	facesPlanes(nullptr),
	edgeCount(0),
	edges(nullptr) {
}

b3Hull::~b3Hull() {
	b3Free(vertices);
	b3Free(faces);
	b3Free(facesPlanes);
	b3Free(edges);
}

b3Vec3 b3Hull::GetSupport(const b3Vec3& direction) const {
	u32 index = 0;
	r32 max = b3Dot(direction, vertices[index]);
	for (u32 i = 1; i < vertexCount; ++i) {
		r32 dot = b3Dot(direction, vertices[i]);
		if (dot > max) {
			index = i;
			max = dot;
		}
	}
	return vertices[index];
}

b3Vec3 b3Hull::GetSupport(const b3Vec3& direction, const b3Transform& transform) const {
	u32 index = 0;
	r32 max = b3Dot(direction, transform * vertices[index]);
	for (u32 i = 1; i < vertexCount; ++i) {
		r32 dot = b3Dot(direction, transform * vertices[i]);
		if (dot > max) {
			index = i;
			max = dot;
		}
	}
	return transform * vertices[index];
}

b3Vec3 Newell(const b3Vec3& a, const b3Vec3& b) {
	return b3Vec3(
		(a.y - b.y) * (a.z + b.z),
		(a.z - b.z) * (a.x + b.x),
		(a.x - b.x) * (a.y + b.y));
}

void b3Hull::CreateFacesPlanes(const b3HullDef& def) {
	b3Assert(!facesPlanes);
	b3Assert(def.faceCount > 0);
	facesPlanes = (b3Plane*)b3Alloc(def.faceCount * sizeof(b3Plane));

	for (u32 i = 0; i < def.faceCount; ++i) {
		const b3FaceDef& face = def.faces[i];
		u32 vertCount = face.vertexCount;
		b3Assert(vertCount >= 3);
		const u32* indices = face.vertices;

		b3Vec3 normal;
		normal.SetZero();
		b3Vec3 centroid;
		centroid.SetZero();
		for (u32 j = 0; j < vertCount; ++j) {
			u32 i1 = indices[j];
			u32 i2 = j + 1 < vertCount ? indices[j + 1] : indices[0];

			b3Vec3 v1 = def.vertices[i1];
			b3Vec3 v2 = def.vertices[i2];

			normal += Newell(v1, v2);
			centroid += v1;
		}

		b3Vec3 newell = b3Normalize(normal);
		facesPlanes[i].normal = newell;
		facesPlanes[i].offset = b3Dot(centroid, newell) / r32(vertCount);
	}
}

void b3Hull::SetFromFaces(const b3HullDef& def) {
	b3Assert(def.faceCount);
	b3Assert(def.vertexCount);

	vertexCount = def.vertexCount;
	vertices = (b3Vec3*)b3Alloc(def.vertexCount * sizeof(b3Vec3));
	::memcpy(vertices, def.vertices, def.vertexCount * sizeof(b3Vec3));

	faceCount = def.faceCount;
	faces = (b3Face*)b3Alloc(def.faceCount * sizeof(b3Face));
	for (u32 k = 0; k < def.faceCount; ++k) {
		b3Face* f = faces + k;
		f->edge = NULL_FEATURE;
	}
	
	CreateFacesPlanes(def);
	
	typedef std::pair<u32, u32> b3Edge;
	typedef std::map<b3Edge, u32> b3HalfEdgeMap;

	b3HalfEdgeMap edgeMap;

	// Loop through all faces.
	for (u32 i = 0; i < def.faceCount; ++i) {
		const b3FaceDef& face = def.faces[i];
		u32 vertCount = face.vertexCount;
		b3Assert(vertCount >= 3);
		const u32* vertices = face.vertices;

		b3ArrayPOD<u32, u32> faceHalfEdges;

		// Loop through all face edges.
		for (u32 j = 0; j < vertCount; ++j) {
			u32 v1 = vertices[j];
			u32 v2 = j + 1 < vertCount ? vertices[j + 1] : vertices[0];
			
			b3HalfEdgeMap::const_iterator iter12 = edgeMap.find(b3Edge(v1, v2));
			b3HalfEdgeMap::const_iterator iter21 = edgeMap.find(b3Edge(v2, v1));
			bool edgeFound12 = iter12 != edgeMap.end();
			bool edgeFound21 = iter21 != edgeMap.end();
			b3Assert(edgeFound12 == edgeFound21);
			if (edgeFound12) {
				// The edge is shared by two faces.
				u32 e12 = iter12->second;

				b3HalfEdge& edge12 = edges[e12];
				b3HalfEdge& edge21 = edges[edge12.twin];
				
				// Link adjacent face to edge.
				if (edge12.face == NULL_FEATURE) {
					edge12.face = i;
				}
				else {
					// Two shared edges can't have the same vertices in the same order.
					b3Assert(false);
				}
				
				if (faces[i].edge == NULL_FEATURE) {
					faces[i].edge = e12;
				}
				
				faceHalfEdges.PushBack(e12);
			}
			else {
				// Allocate a new edge.
				b3HalfEdge* oldHalfEdges = edges;
				edges = (b3HalfEdge*)::b3Alloc((edgeCount + 2) * sizeof(b3HalfEdge));
				::memcpy(edges, oldHalfEdges, edgeCount * sizeof(b3HalfEdge));
				::b3Free(oldHalfEdges);

				// The next edge of the current half edge in the array is the twin edge.
				u32 e12 = edgeCount++;
				u32 e21 = edgeCount++;

				if (faces[i].edge == NULL_FEATURE) {
					faces[i].edge = e12;
				}

				faceHalfEdges.PushBack(e12);

				edges[e12].prev = NULL_FEATURE;
				edges[e12].next = NULL_FEATURE;
				edges[e12].twin = e21;
				edges[e12].face = i;
				edges[e12].origin = v1;

				edges[e21].prev = NULL_FEATURE;
				edges[e21].next = NULL_FEATURE;
				edges[e21].twin = e12;
				edges[e21].face = NULL_FEATURE;
				edges[e21].origin = v2;

				// Add edges to map.
				edgeMap[b3Edge(v1, v2)] = e12;
				edgeMap[b3Edge(v2, v1)] = e21;
			}
		}

		// Link the half-edges of the current face.
		for (u32 j = 0; j < faceHalfEdges.Size(); ++j) {
			u32 e1 = faceHalfEdges[j];
			u32 e2 = j + 1 < faceHalfEdges.Size() ? faceHalfEdges[j + 1] : faceHalfEdges[0];

			edges[e1].next = e2;
			edges[e2].prev = e1;
		}
	}
	
	Validate();
}

void b3Hull::SetAsBox(const b3Vec3& scale) {
	b3Vec3 cubeVertices[8] = {
		b3Vec3(B3_ONE, B3_ONE, -B3_ONE),
		b3Vec3(-B3_ONE, B3_ONE, -B3_ONE),
		b3Vec3(-B3_ONE, -B3_ONE, -B3_ONE),
		b3Vec3(B3_ONE, -B3_ONE, -B3_ONE),
		b3Vec3(B3_ONE, B3_ONE, B3_ONE),
		b3Vec3(-B3_ONE, B3_ONE, B3_ONE),
		b3Vec3(-B3_ONE, -B3_ONE, B3_ONE),
		b3Vec3(B3_ONE, -B3_ONE, B3_ONE),
	};

	// CW
	u32 left[4] = { 1, 2, 6, 5 };
	u32 right[4] = { 4, 7, 3, 0 };
	u32 down[4] = { 3, 7, 6, 2 };
	u32 up[4] = { 0, 1, 5, 4 };
	u32 back[4] = { 4, 5, 6, 7 };
	u32 front[4] = { 0, 3, 2, 1 };

	for (u32 i = 0; i < 8; ++i) {
		cubeVertices[i].x *= scale.x;
		cubeVertices[i].y *= scale.y;
		cubeVertices[i].z *= scale.z;
	}

	b3HullDef hullDef;
	hullDef.vertexCount = 8;
	hullDef.vertices = &cubeVertices[0];
	
	hullDef.faceCount = 6;
	b3FaceDef boxFaces[6] = {
		{ 4, &left[0] },
		{ 4, &right[0] },
		{ 4, &down[0] },
		{ 4, &up[0] },
		{ 4, &back[0] },
		{ 4, &front[0] },
	};

	hullDef.faces = boxFaces;

	SetFromFaces(hullDef);
}

void b3Hull::Validate() const {
	b3Assert(faceCount > 0);
	b3Assert(edgeCount > 0);
	// Recursively validate this hull faces.
	for (u32 i = 0; i < faceCount; ++i) {
		Validate(faces + i);
	}
}

void b3Hull::Validate(const b3Face* face) const {
	b3Assert(faceCount > 0);
	b3Assert(edgeCount > 0);
	b3Assert(face->edge != NULL_FEATURE);
	// Recursively validate the face edges.
	Validate(edges + face->edge);
}

void b3Hull::Validate(const b3HalfEdge* edge) const {
	b3Assert(faceCount > 0);
	b3Assert(edgeCount > 0);
	b3Assert(edge->twin != NULL_FEATURE);
	
	//Grab twin edge of the current edge.
	const b3HalfEdge* twin = edges + edge->twin;

	// Grab edge and edge index into the array.
	i32 edgeIndex = (i32)(edge - edges);

	// The twin of the edge twin must be the edge itself.
	b3Assert(twin->twin == edgeIndex);

	// The two edges must be close by one index.
	b3Assert(b3Abs(edge->twin - edgeIndex) == 1);
	
	// The twin of the edge twin must be the edge.
	b3Assert(edges[edge->prev].next == edgeIndex);
	
	b3Assert(edge->origin != twin->origin);

	// Loop over the edges of this edge.
	// Incorrect assignments can lead to infinite loops.
	u32 count = 0;
	const b3HalfEdge* start = edge;
	do {
		const b3HalfEdge* next = edges + edge->next;
		const b3HalfEdge* twin = edges + next->twin;
		edge = twin;
		b3Assert(count < edgeCount);
		++count;
	} while (edge != start);
}
