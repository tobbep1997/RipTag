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

#include "b3DynamicAABBTree.h"

b3DynamicAABBTree::b3DynamicAABBTree() {
	m_root = NULL_NODE;

	// Preallocate 16 nodes.
	m_nodeCapacity = 16;
	m_nodes = (b3Node*) b3Alloc(m_nodeCapacity * sizeof(b3Node));
	::memset(m_nodes, NULL, m_nodeCapacity * sizeof(b3Node));
	m_nodeCount = 0;

	// Link the allocated nodes and make the first node 
	// available the the next allocation.
	AddToFreeList(m_nodeCount);
}

b3DynamicAABBTree::~b3DynamicAABBTree() {
	b3Free(m_nodes);
}

bool b3DynamicAABBTree::TestOverlap(i32 proxy1, i32 proxy2) const {
	return b3TestOverlap(m_nodes[proxy1].aabb, m_nodes[proxy2].aabb);
}

// Return a node from the pool.
i32 b3DynamicAABBTree::AllocateNode() {
	if (m_freeList == NULL_NODE) {
		b3Assert(m_nodeCount == m_nodeCapacity);

		// Duplicate capacity.
		m_nodeCapacity *= 2;

		b3Node* oldNodes = m_nodes;
		m_nodes = (b3Node*) b3Alloc(m_nodeCapacity * sizeof(b3Node));;
		::memcpy(m_nodes, oldNodes, m_nodeCount * sizeof(b3Node));
		b3Free(oldNodes);

		// Link the (allocated) nodes starting from the new 
		// node and make the new nodes available the the next allocation.
		AddToFreeList(m_nodeCount);
	}

	// Grab the free node.
	i32 node = m_freeList;

	m_freeList = m_nodes[node].next;

	m_nodes[node].parent = NULL_NODE;
	m_nodes[node].child1 = NULL_NODE;
	m_nodes[node].child2 = NULL_NODE;
	m_nodes[node].height = 0;
	m_nodes[node].userData = nullptr;

	++m_nodeCount;

	return node;
}

void b3DynamicAABBTree::FreeNode(i32 node) {
	b3Assert(node >= 0 && node < m_nodeCapacity);
	m_nodes[node].next = m_freeList;
	m_nodes[node].height = -1;
	m_freeList = node;
	--m_nodeCount;
}

void b3DynamicAABBTree::AddToFreeList(i32 node) {
	// Starting from the given node, relink the linked list of nodes.
	for (i32 i = node; i < m_nodeCapacity - 1; ++i) {
		m_nodes[i].next = i + 1;
		m_nodes[i].height = -1;
	}

	m_nodes[m_nodeCapacity - 1].next = NULL_NODE;
	m_nodes[m_nodeCapacity - 1].height = -1;

	// Make the node available for the next allocation.
	m_freeList = node;
}

i32 b3DynamicAABBTree::InsertNode(const b3AABB& aabb, void* userData) {
	// Insert into the array.
	i32 node = AllocateNode();

	// Later, if the node bounds has changed then it should be reinserted into the tree.
	// Logically, however, this can be expansive due to the hierarchy reconstruction.
	// Therefore, the original AABB is initially extended and inserted into the tree,
	// so we can check later if the new (original) AABB is inside the old (fat) AABB.
	b3AABB fatAABB = aabb;
	fatAABB.Extend(B3_WORLD_AABB_EXTENSION);
	
	// Set the AABB and client data to the allocated node.
	m_nodes[node].aabb = fatAABB;
	m_nodes[node].userData = userData;
	m_nodes[node].height = 0;

	// (assuming the AABB was set) Insert the allocated node into the tree.
	InsertLeaf(node);

	// Return the node ID.
	return node;
}

void b3DynamicAABBTree::RemoveNode(i32 proxyId) {
	// Remove from the tree.
	RemoveLeaf(proxyId);
	// Remove from the node array and make it available.
	FreeNode(proxyId);
}

bool b3DynamicAABBTree::UpdateNode(i32 proxyId, const b3AABB& aabb) {
	b3Assert(m_root != NULL_NODE);
	b3Assert(m_nodes[proxyId].IsLeaf());

	// The new node should be reinserted into the tree.
	// Logically, however, this can be expansive.
	// Therefore, the original AABB is initially extended and inserted into the tree,
	// so we can check (here) if the new (original) AABB is inside the old AABB.
	if (m_nodes[proxyId].aabb.ContainsAabb(aabb)) {
		// Do nothing if the new AABB is contained in the old AABB.
		return false;
	}

	// Extend the new (original) AABB.
	b3AABB fatAabb = aabb;
	fatAabb.Extend(B3_WORLD_AABB_EXTENSION);
	
	// Remove from the tree.
	RemoveLeaf(proxyId);

	// Since nodes aren't destroyed, simply update the new node AABB.
	m_nodes[proxyId].aabb = fatAabb;
	
	// Insert the (fat) AABB into the tree.
	InsertLeaf(proxyId);

	// Return true if the new AABB was reinserted.
	return true;
}

i32 b3DynamicAABBTree::HeuristicSearch(const b3AABB& leafAABB) const {
	// References: Box2D (b2DynamicTree.cpp).
	// To find a good branch node, the manhattan distance could be used as heuristic. 
	// However, the current propagated node and the leaf node volume are incompletely considerable.
	// Therefore, an approximation of the surface are heuristic (SAH) is used.

	i32 index = m_root;
	while (!m_nodes[index].IsLeaf()) {
		r32 branchArea = m_nodes[index].aabb.SurfaceArea();

		// Minumum cost of pushing the leaf down the tree.
		b3AABB combinedAABB = Combine(leafAABB, m_nodes[index].aabb);
		r32 combinedArea = combinedAABB.SurfaceArea();

		// Cost for creating a new parent node.
		r32 branchCost = B3_TWO * combinedArea;

		r32 inheritanceCost = B3_TWO * (combinedArea - branchArea);

		// The branch node child nodes cost.
		i32 child1 = m_nodes[index].child1;
		i32 child2 = m_nodes[index].child2;

		// Cost of descending onto child1.
		r32 childCost1 = B3_ZERO;
		if (m_nodes[child1].IsLeaf()) {
			b3AABB aabb = Combine(leafAABB, m_nodes[child1].aabb);
			childCost1 = aabb.SurfaceArea();
		}
		else {
			b3AABB aabb = Combine(leafAABB, m_nodes[child1].aabb);
			r32 oldArea = m_nodes[child1].aabb.SurfaceArea();
			r32 newArea = aabb.SurfaceArea();
			childCost1 = (newArea - oldArea) + inheritanceCost;
		}

		// Cost of descending onto child1.
		r32 childCost2 = B3_ZERO;
		if (m_nodes[child2].IsLeaf()) {
			b3AABB aabb = Combine(leafAABB, m_nodes[child2].aabb);
			childCost2 = aabb.SurfaceArea();
		}
		else {
			b3AABB aabb = Combine(leafAABB, m_nodes[child2].aabb);
			r32 oldArea = m_nodes[child2].aabb.SurfaceArea();
			r32 newArea = aabb.SurfaceArea();
			childCost2 = (newArea - oldArea) + inheritanceCost;
		}

		// Choose the node that has the minimum cost.
		if (branchCost < childCost1 && branchCost < childCost2) {
			// The current branch node is the best node and it will be used.
			break;
		}

		// Visit the node that has the minimum cost.
		index = childCost1 < childCost2 ? child1 : child2;
	}
	return index;
}

void b3DynamicAABBTree::InsertLeaf(i32 leaf) {
	if (m_root == NULL_NODE) {
		// If this tree root node is empty then just set the leaf
		// node to it.
		m_root = leaf;
		m_nodes[m_root].parent = NULL_NODE;
		return;
	}

	// Get the inserted leaf AABB.
	b3AABB leafAabb = m_nodes[leaf].aabb;
	
	// Search for the best branch node of this tree starting from the tree root node.
	i32 sibling = HeuristicSearch(leafAabb);

	i32 oldParent = m_nodes[sibling].parent;
	
	// Create and setup new parent. 
	i32 newParent = AllocateNode();	
	m_nodes[newParent].parent = oldParent;
	m_nodes[newParent].child1 = sibling;
	m_nodes[sibling].parent = newParent;	
	m_nodes[newParent].child2 = leaf;
	m_nodes[leaf].parent = newParent;	
	m_nodes[newParent].userData = nullptr;
	m_nodes[newParent].aabb = Combine(leafAabb, m_nodes[sibling].aabb);
	m_nodes[newParent].height = m_nodes[sibling].height + 1;

	if (oldParent != NULL_NODE) {
		// The sibling was not the root.
		// Find which child node of the old parent is the sibling
		// and link the new parent to it.
		if (m_nodes[oldParent].child1 == sibling) {
			m_nodes[oldParent].child1 = newParent;
		}
		else {
			m_nodes[oldParent].child2 = newParent;
		}
	}
	else {
		// If the sibling was the root then the root becomes the created
		// node.
		m_root = newParent;
	}

	// If we have ancestor nodes then adjust its AABBs.
	WalkBackNodeAndCombineVolumes(newParent);
}

void b3DynamicAABBTree::RemoveLeaf(i32 leaf) {
	if (leaf == m_root) {
		m_root = NULL_NODE;
		return;
	}

	i32 parent = m_nodes[leaf].parent;
	i32 grandParent = m_nodes[parent].parent;
	i32 sibling;
	if (m_nodes[parent].child1 == leaf) {
		sibling = m_nodes[parent].child2;
	}
	else {
		sibling = m_nodes[parent].child1;
	}

	if (grandParent != NULL_NODE) {
		if (m_nodes[grandParent].child1 == parent) {
			m_nodes[grandParent].child1 = sibling;
		}
		else {
			m_nodes[grandParent].child2 = sibling;
		}
		m_nodes[sibling].parent = grandParent;
		
		// Remove parent node.
		FreeNode(parent);

		// If we have ancestor then nodes adjust its AABBs.
		WalkBackNodeAndCombineVolumes(grandParent);
	}
	else {
		m_root = sibling;
		m_nodes[sibling].parent = NULL_NODE;		
		// Remove parent node.
		FreeNode(parent);
	}
}

void b3DynamicAABBTree::WalkBackNodeAndCombineVolumes(i32 node) {
	while (node != NULL_NODE) {
		//@todo node = Balance(node);

		i32 child1 = m_nodes[node].child1;
		i32 child2 = m_nodes[node].child2;

		b3Assert(child1 != NULL_NODE);
		b3Assert(child2 != NULL_NODE);

		m_nodes[node].height = 1 + b3Max(m_nodes[child1].height, m_nodes[child2].height);
		m_nodes[node].aabb = Combine(m_nodes[child1].aabb, m_nodes[child2].aabb);

		node = m_nodes[node].parent;
	}
}

void b3DynamicAABBTree::Validate(i32 nodeID) const {
	if (nodeID == NULL_NODE) {
		return;
	}

	// The root node has no parent.
	if (nodeID == m_root) {
		b3Assert(m_nodes[nodeID].parent == NULL_NODE);
	}

	const b3Node* node = m_nodes + nodeID;

	i32 child1 = node->child1;
	i32 child2 = node->child2;

	if (node->IsLeaf()) {
		// Leaf nodes has no children and its height is zero.
		b3Assert(child1 == NULL_NODE);
		b3Assert(child2 == NULL_NODE);
		b3Assert(node->height == 0);
	}
	else {
		b3Assert(0 <= child1 && child1 < m_nodeCapacity);
		b3Assert(0 <= child2 && child2 < m_nodeCapacity);

		// The parent of its children is its parent (really?!).

		b3Assert(m_nodes[child1].parent == nodeID);
		b3Assert(m_nodes[child2].parent == nodeID);

		// Walk down the tree.
		Validate(child1);
		Validate(child2);
	}
}
