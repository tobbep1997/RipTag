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

#ifndef __B3_BLOCK_ALLOCATOR_H__
#define __B3_BLOCK_ALLOCATOR_H__

#include "..\b3Settings.h"

const u32 b3_chunkArrayIncrement = 128;
const u32 b3_chunkSize = b3_chunkArrayIncrement * b3_chunkArrayIncrement;
const u32 b3_maxBlockSize = 640;
const u32 b3_blockSizes = 14;

// References:
// Box2D (b2BlockAllocator.cpp).
// http://www.codeproject.com/Articles/17060/A-Fast-Efficient-Allocator-for-Small-Blocks-of-Mem
// http://www.memorymanagement.org/mmref/alloc.html

class b3BlockAllocator {
public :
	// This block allocator will allocate how many chunks is needed to return memory to the client.
	// The allocation is faster because we map the requested size to free block indices directly.
	// Keeping a list of free blocks we can grab a free block in O(1) time using the
	// requested memory size. Similarly is the dellocation.
	b3BlockAllocator();
	~b3BlockAllocator();

	// Allocate memory.
	void* Allocate(u32 size);

	// Free memory.
	void Free(void* p, u32 size);

	void Clear();
protected :
	struct b3Block {
		b3Block* next;
	};

	struct b3Chunk {
		u32 blockSize;
		b3Block* blocks;
	};
	
	// Tables used to quickly find out which block the allocation request
	// should go to.
	static u32 m_blockSizes[b3_blockSizes];
	static u8 m_blockSizeToFreeBlock[b3_maxBlockSize + 1];
	static bool m_blockSizeTableInitialized;

	b3Chunk* m_chunks;
	u32 m_chunkCount;
	u32 m_chunkCapacity;

	// Keep a singly-linked list of free blocks.
	b3Block* m_freeBlocks[b3_blockSizes];
};

#endif
