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

#ifndef __B3_BROAD_PHASE_H__
#define __B3_BROAD_PHASE_H__

#include <algorithm>
#include "b3Collision.h"
#include "b3DynamicAABBTree.h"

class b3BroadPhase {
public :
	// The broad-phase collision detection facade.
	// We could make it as an interface, but this will make things
	// slightly complex and definately unnecessary.
	b3BroadPhase();
	~b3BroadPhase();

	// Create a broad-phase proxy and return an proxy ID.
	i32 CreateProxy(const b3AABB& aabb, void* userData);
	
	// Destroy an existing proxy.
	void DestroyProxy(i32 proxyId);

	// Update an existing proxy with the a new AABB.
	void UpdateProxy(i32 proxyId, const b3AABB& aabb);
	
	// Get the user data attached to a proxy.
	void* GetUserData(i32 proxyId) const;

	// Test if two (AABB) proxies are overlapping.
	bool TestOverlap(i32 proxy1, i32 proxy2) const;
	
	// Notify the callback client the AABBs that are overlapping the passed AABB.
	template<class T>
	void Query(T* callback, const b3AABB& aabb) const;
	
	// Notify the callback client the AABBs that are overlapping the 
	// passed ray and compute intersection information.
	template<class T>
	void RayCast(T* callback, const b3RayCastInput& input) const;
protected :
	friend class b3World;
	friend class b3ContactGraph;
	friend class b3DynamicAABBTree;

	// Add an proxy to the list of moved proxies.
	// Only moved proxies will be used as AABB query reference object.
	void BufferMove(i32 proxyId);

	// m_queryProxyId is the current proxy being queried for 
	// overlapping witha another proxies and is used to avoid a proxy overlap with itself.
	// The QueryCallback is the client callback used to add a overlapping pair
	// to the overlapping pair buffer.
	i32 m_queryProxyId;
	bool QueryCallback(i32 proxyId);
	
	// Report to the client callback the unique AABB pairs that are overlapping.
	template<class T>
	void FindOverlappingPairs(T* callback);

	b3DynamicAABBTree m_dynamicAabbTree;

	// Keep a buffer of the objects that has moved in a step.
	i32* m_moveBuffer;
	u32 m_moveBufferCount;
	u32 m_moveBufferCapacity;

	// The buffer holding the (unique) overlapping  (AABB) pairs.
	b3Pair* m_pairBuffer;
	u32 m_pairBufferCapacity;
	u32 m_pairBufferCount;
};

inline void* b3BroadPhase::GetUserData(i32 proxyId) const {
	return m_dynamicAabbTree.GetUserData(proxyId);
}

template<class T>
inline void b3BroadPhase::Query(T* callback, const b3AABB& aabb) const {
	return m_dynamicAabbTree.Query(callback, aabb);
}

template<class T>
inline void b3BroadPhase::RayCast(T* callback, const b3RayCastInput& input) const {
	return m_dynamicAabbTree.RayCast(callback, input);
}

inline bool operator<(const b3Pair& pair1, const b3Pair& pair2) {
	if (pair1.proxy1 < pair2.proxy1) {
		return true;
	}

	if (pair1.proxy1 == pair2.proxy1) {
		return pair1.proxy2 < pair2.proxy2;
	}

	return false;
}

template<class T>
inline void b3BroadPhase::FindOverlappingPairs(T* callback) {
	// Reset the overlapping pairs buffer count for the current step.
	m_pairBufferCount = 0;

	// Notifying this class with QueryCallback(), get the (duplicated) overlapping pair buffer.
	for (u32 i = 0; i < m_moveBufferCount; ++i) {
		// Keep the current queried proxy ID to avoid overlapping with itself.
		m_queryProxyId = m_moveBuffer[i];
		if (m_queryProxyId == NULL_NODE) {
			continue;
		}
		const b3AABB& aabb = m_dynamicAabbTree.GetAABB(m_queryProxyId);
		m_dynamicAabbTree.Query(this, aabb);
	}

	// Reset the move buffer for the next step.
	m_moveBufferCount = 0;

	// Sort the (duplicated) overlapping pair buffer to prune duplicated pairs.
	std::sort(m_pairBuffer, m_pairBuffer + m_pairBufferCount);

	// Skip duplicated overlapping pairs.
	u32 index = 0;
	b3Assert(index == 0);
	while (index < m_pairBufferCount) {
		const b3Pair* primaryPair = m_pairBuffer + index;

		// Notify an unique overlapping pair to the client callback.
		callback->AddPair(m_dynamicAabbTree.GetUserData(primaryPair->proxy1), m_dynamicAabbTree.GetUserData(primaryPair->proxy2));

		// Skip all duplicated pairs until an unique pair is found.
		++index;
		while (index < m_pairBufferCount) {
			const b3Pair* secondaryPair = m_pairBuffer + index;
			if (secondaryPair->proxy1 != primaryPair->proxy1 || secondaryPair->proxy2 != primaryPair->proxy2) {
				break;
			}
			++index;
		}
	}
}

#endif
