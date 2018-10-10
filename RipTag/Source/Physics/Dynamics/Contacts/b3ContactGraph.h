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

#ifndef __B3_CONTACT_GRAPH_H__
#define __B3_CONTACT_GRAPH_H__

#include "..\..\Collision\b3BroadPhase.h"

class b3BlockAllocator;
class b3Contact;
class b3ContactListener;

class b3ContactGraph {
public :
	b3ContactGraph();

	// The broad-phase will notify us if ther is a potential shape pair shapes colliding.
	void AddPair(void* data1, void* data2);
	// The broad-phase will contact us if ther is a potential shape pair shapes colliding.
	void DestroyContact(b3Contact* c);
	
	// Get the potential colliding shape pairs (broad-phase).
	void FindNewContacts();
	// Get the actual colliding shapes (narrow-phase).
	void UpdateContacts();
protected :
	friend class b3World;
	friend class b3Body;

	b3BlockAllocator* m_blockAllocator;
	b3BroadPhase m_broadPhase;
	b3Contact* m_contactList;
	u32 m_contactCount;
	b3ContactListener* m_contactListener;
};

#endif
