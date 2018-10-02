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

#include "b3StackAllocator.h"

b3StackAllocator::b3StackAllocator() {
	// Initially allocate 32 entries.
	m_entryCapacity = 32;
	m_entries = (b3Entry*)b3Alloc(m_entryCapacity * sizeof(b3Entry));
	m_entryCount = 0;
	m_allocatedSize = 0;
}

b3StackAllocator::~b3StackAllocator() {
	b3Assert(m_allocatedSize == 0);
	b3Assert(m_entryCount == 0);
	b3Free(m_entries);
}

void* b3StackAllocator::Allocate(u32 size) {
	if (m_entryCount == m_entryCapacity) {
		// Then duplicate capacity if needed.
		b3Entry* oldEntries = m_entries;
		m_entryCapacity *= 2;
		m_entries = (b3Entry*)b3Alloc(m_entryCapacity * sizeof(b3Entry));
		::memcpy(m_entries, oldEntries, m_entryCount * sizeof(b3Entry));
		b3Free(oldEntries);
	}

	b3Entry* entry = m_entries + m_entryCount;
	entry->size = size;
	if (m_allocatedSize + size > b3_maxStackSize) {
		// Pass to the general-purpose allocator.
		entry->data = (u8*) b3Alloc(size);
		entry->general = true;
	}
	else {
		// Use the this stack memory.
		entry->data = m_memory + m_allocatedSize;
		entry->general = false;
		m_allocatedSize += size;
	}
	
	++m_entryCount;

	return entry->data;
}

void b3StackAllocator::Free(void* p) {
	b3Assert(m_entryCount > 0);
	b3Entry* entry = m_entries + m_entryCount - 1;
	b3Assert(entry->data == p);
	if (entry->general) {
		b3Free(p);
	}
	else {
		m_allocatedSize -= entry->size;
	}
	--m_entryCount;
}
