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

#ifndef __B3_STACK_ALLOCATOR_H__
#define __B3_STACK_ALLOCATOR_H__

#include "..\b3Settings.h"

// Initially allocate 10 MB. 
// Increase as you want.
const u32 b3_maxStackSize = 10 * 1024 * 1024;

class b3StackAllocator {
public :
	// This is a general purpose stack allocator.
	// We can create how many entries if we want
	// as long the memory size stays below b3_maxStackSize.
	// If there is no stack memory left then we 
	// allocate more using the b3Malloc.
	b3StackAllocator();
	~b3StackAllocator();

	// Push.
	void* Allocate(u32 size);
	/// Pop.
	void Free(void* p);
protected :
	struct b3Entry {
		u32 size;
		u8* data;
		bool general;
	};
	
	b3Entry* m_entries;
	u32 m_entryCount;
	u32 m_entryCapacity;

	u8 m_memory[b3_maxStackSize];
	u32 m_allocatedSize;
};

#endif
