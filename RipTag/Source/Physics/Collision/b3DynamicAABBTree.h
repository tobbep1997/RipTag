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

#ifndef __B3_DYNAMIC_AABB_TREE_H__
#define __B3_DYNAMIC_AABB_TREE_H__

#include "..\Common\b3Settings.h"
#include "..\Common\Memory\b3ArrayPOD.h"
#include "b3Collision.h"

#define NULL_NODE (-1)

class b3DynamicAABBTree {
public :
	//@todo Balance the tree.
	b3DynamicAABBTree();
	~b3DynamicAABBTree();

	// Insert a node to the tree and return a ID.
	i32 InsertNode(const b3AABB& aabb, void* userData);
	
	// Remove a node from the tree.
	void RemoveNode(i32 proxyId);

	// Update a node AABB.
	// The function will return true if the node AABB has significantly changed.
	bool UpdateNode(i32 proxyId, const b3AABB& aabb);

	// Get the (fat) AABB of a given proxy.
	const b3AABB& GetAABB(i32 proxyId) const;

	// Get the data associated with a given proxy.
	void* GetUserData(i32 proxyId) const;

	// Check if two bounds are overlapping.
	bool TestOverlap(i32 proxy1, i32 proxy2) const;

	// Keep notifying the client callback all AABBs that are overlapping with
	// the given AABB. The client callback must return true if the query 
	// must be stopped or false to continue looking for more overlapping pairs.
	template<class T> 
	void Query(T* callback, const b3AABB& aabb) const;

	// Keep notifying the client callback all AABBs that are overlapping with
	// the given ray. The client callback must return the new intersection fraction 
	// (real). If the fraction == 0 then the query is cancelled immediatly.
	template<class T>
	void RayCast(T* callback, const b3RayCastInput& input) const;

	// Validate a given node.
	void Validate(i32 node) const;
protected :
	friend class b3World;

	struct b3Node {
		bool IsLeaf() const {
			return child1 == NULL_NODE; //or child 2 == NULL_NODE, or height == 0.
		}

		// The (enlarged) AABB of this node.
		b3AABB aabb;
		// The associated user data.
		void* userData;

		union {
			i32 parent;
			i32 next;
		};

		i32 child1;
		i32 child2;

		// leaf = 0, free node = -1
		i32 height;
	};

	// Insert a allocated (leaf) node into the tree.
	void InsertLeaf(i32 node);
	// Remove a allocated node from the tree.
	void RemoveLeaf(i32 node);

	// Rebuild the tree hierarchy starting from the given node.
	void WalkBackNodeAndCombineVolumes(i32 node);
	//@todo i32 Balance(i32 node);

	// Perform a basic surface are heuristic to find the best
	// node that can be merged with a given AABB. It can be used for
	// debugging.
	i32 HeuristicSearch(const b3AABB& leafAABB) const;

	// Peel a node from the free list and insert into the node array. 
	// Allocate a new node if necessary. The function returns the new node index.
	i32 AllocateNode();

	// Free a node (not destroy) from the node pool and add it to the free list.
	void FreeNode(i32 node);

	// Make a node available for the next allocation request.
	void AddToFreeList(i32 node);

	// The root of the tree.
	i32 m_root;

	// The node array.
	b3Node* m_nodes;
	i32 m_nodeCount;
	i32 m_nodeCapacity;
	i32 m_freeList;
};

inline const b3AABB& b3DynamicAABBTree::GetAABB(i32 proxyId) const {
	b3Assert(proxyId < m_nodeCount);
	return m_nodes[proxyId].aabb;
}

inline void* b3DynamicAABBTree::GetUserData(i32 proxyId) const {
	b3Assert(proxyId < m_nodeCount);
	return m_nodes[proxyId].userData;
}

template<class T>
inline void b3DynamicAABBTree::Query(T* callback, const b3AABB& aabb) const {
	// @todo Decrease stack size when balanced.
	b3ArrayPOD<i32, u32, 512> stack;
	stack.PushBack(m_root);

	while (!stack.Empty()) {
		i32 nodeIndex = stack.Back();
		
		stack.PopBack();

		if (nodeIndex == NULL_NODE) {
			continue;
		}

		const b3Node* node = m_nodes + nodeIndex;

		if (b3TestOverlap(node->aabb, aabb)) {
			if (node->IsLeaf()) {
				if (!callback->QueryCallback(nodeIndex)) {
					return;
				}
			}
			else {
				stack.PushBack(node->child1);
				stack.PushBack(node->child2);
			}
		}
	}
}

template<class T>
inline void b3DynamicAABBTree::RayCast(T* callback, const b3RayCastInput& input) const {
	b3Vec3 p1 = input.p1;
	b3Vec3 p2 = input.p2;
	b3Vec3 direction = p2 - p1;
	r32 maxFraction = input.maxFraction;

	r32 dirSqLen = b3LenSq(direction);
	b3Assert(dirSqLen > B3_ZERO);

	direction *= B3_ONE / b3Sqrt(dirSqLen);
	
	b3ArrayPOD<i32, u32, 512> stack;
	stack.PushBack(m_root);

	while (!stack.Empty()) {
		i32 nodeIndex = stack.Back();
		
		stack.PopBack();

		if (nodeIndex == NULL_NODE) {
			continue;
		}

		const b3Node* node = m_nodes + nodeIndex;

		r32 minFraction = B3_ZERO;
		if (node->aabb.RayCast(p1, p2 - p1, maxFraction, minFraction)) {
			if (node->IsLeaf()) {
				b3RayCastInput subInput;
				subInput.p1 = input.p1;
				subInput.p2 = input.p2;
				subInput.maxFraction = maxFraction;

				r32 newMaxFraction = callback->RayCastCallback(subInput, nodeIndex);

				if (newMaxFraction == B3_ZERO) {
					// The client has stopped the ray cast.
					return;
				}
			}
			else {
				stack.PushBack(node->child1);
				stack.PushBack(node->child2);
			}
		}
	}
}

#endif
