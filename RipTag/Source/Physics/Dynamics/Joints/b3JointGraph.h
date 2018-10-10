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

#ifndef __B3_JOINT_GRAPH_H__
#define __B3_JOINT_GRAPH_H__

#include "..\..\Common\b3Settings.h"

struct b3JointDef;
class b3Joint;
class b3BlockAllocator;

// The joint graph structure.
// Keep it separated from the world because
// is more easy to debug. 
class b3JointGraph {
public :
	b3JointGraph();

	// Allocate a joint on the heap and add it to the graph.
	// The user must cast to the specific class.
	b3Joint* CreateJoint(const b3JointDef* def);
	
	// Remove the given joint from the graph and
	// deallocate it from the heap.
	void DestroyJoint(b3Joint* j);
protected :
	friend class b3World;
	friend class b3Body;

	// Request a joint from the heap.
	b3Joint* AllocateJoint(const b3JointDef* def) const;
	
	// Deallocate a joint from the heap.
	void FreeJoint(b3Joint* j) const;

	b3Joint* m_jointList;
	u32 m_jointCount;
	mutable b3BlockAllocator* m_blockAllocator;
};

#endif
