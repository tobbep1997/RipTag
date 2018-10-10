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

#include "b3BroadPhase.h"

b3BroadPhase::b3BroadPhase() {
	m_moveBufferCapacity = 16;
	m_moveBuffer = (i32*)::b3Alloc(m_moveBufferCapacity * sizeof(i32));
	::memset(m_moveBuffer, NULL, m_moveBufferCapacity * sizeof(i32));
	m_moveBufferCount = 0;

	m_pairBufferCapacity = 16;
	m_pairBuffer = (b3Pair*)::b3Alloc(m_pairBufferCapacity * sizeof(b3Pair));
	::memset(m_pairBuffer, NULL, m_pairBufferCapacity * sizeof(b3Pair));
	m_pairBufferCount = 0;
}

b3BroadPhase::~b3BroadPhase() {
	b3Free(m_moveBuffer);
	b3Free(m_pairBuffer);
}

void b3BroadPhase::BufferMove(i32 proxyId) {
	// The proxy has been moved. Add it to the buffer of moved proxies.
	// Check capacity.
	if (m_moveBufferCount == m_moveBufferCapacity) {
		// Duplicate capacity.
		m_moveBufferCapacity *= 2;

		i32* oldMoveBuffer = m_moveBuffer;
		m_moveBuffer = (i32*)::b3Alloc(m_moveBufferCapacity * sizeof(i32));
		::memcpy(m_moveBuffer, oldMoveBuffer, m_moveBufferCount * sizeof(i32));
		::b3Free(oldMoveBuffer);
	}

	// Add to move buffer.
	m_moveBuffer[m_moveBufferCount] = proxyId;
	++m_moveBufferCount;
}

bool b3BroadPhase::TestOverlap(i32 proxy1, i32 proxy2) const {
	return m_dynamicAabbTree.TestOverlap(proxy1, proxy2);
}

i32 b3BroadPhase::CreateProxy(const b3AABB& aabb, void* userData) {
	return m_dynamicAabbTree.InsertNode(aabb, userData);
}

void b3BroadPhase::DestroyProxy(i32 proxyId) {
	return m_dynamicAabbTree.RemoveNode(proxyId);
}

void b3BroadPhase::UpdateProxy(i32 proxyId, const b3AABB& aabb) {
	if (m_dynamicAabbTree.UpdateNode(proxyId, aabb)) {
		// If the proxy AABB changed significantly then the proxy has moved.
		BufferMove(proxyId);
	}
}

bool b3BroadPhase::QueryCallback(i32 proxyId) {
	if (proxyId == m_queryProxyId) {
		// The proxy can't overlap with itself.
		return true;
	}

	// Check capacity.
	if (m_pairBufferCount == m_pairBufferCapacity) {
		// Duplicate capacity.
		m_pairBufferCapacity *= 2;
		
		b3Pair* oldPairBuffer = m_pairBuffer;
		m_pairBuffer = (b3Pair*)::b3Alloc(m_pairBufferCapacity * sizeof(b3Pair));
		::memcpy(m_pairBuffer, oldPairBuffer, m_pairBufferCount * sizeof(b3Pair));
		::b3Free(oldPairBuffer);
	}

	// Add overlapping pair to the pair buffer.
	m_pairBuffer[m_pairBufferCount].proxy1 = b3Min(proxyId, m_queryProxyId);
	m_pairBuffer[m_pairBufferCount].proxy2 = b3Max(proxyId, m_queryProxyId);
	++m_pairBufferCount;

	// Keep looking for overlapping pairs.
	return true;
}
