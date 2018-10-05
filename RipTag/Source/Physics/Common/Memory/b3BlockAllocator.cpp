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

#include "b3BlockAllocator.h"

bool b3BlockAllocator::m_blockSizeTableInitialized = false;

u32 b3BlockAllocator::m_blockSizes[b3_blockSizes] = {
	16,		// 0
	32,		// 1
	64,		// 2
	96,		// 3
	128,	// 4
	160,	// 5
	192,	// 6
	224,	// 7
	256,	// 8
	320,	// 9
	384,	// 10
	448,	// 11
	512,	// 12
	640		// 13
};

u8 b3BlockAllocator::m_blockSizeToFreeBlock[b3_maxBlockSize + 1];

b3BlockAllocator::b3BlockAllocator() {
	b3Assert(b3_chunkSize >= b3_maxBlockSize);
	b3Assert(b3_blockSizes < UCHAR_MAX);

	// Map block size ranges to block indices.
	if (!m_blockSizeTableInitialized) {
		u32 j = 0;
		for (u32 i = 0; i < b3_maxBlockSize + 1; ++i) {				
			m_blockSizeToFreeBlock[i] = (u8)j;
			b3Assert(j < b3_blockSizes);
			if (i == m_blockSizes[j]) {
				++j;
			}
		}
		m_blockSizeTableInitialized = true;
	}

	// Initially, allocate sixteen memory chunks.
	m_chunkCapacity = 32 * b3_chunkArrayIncrement;
	m_chunks = (b3Chunk*)b3Alloc(m_chunkCapacity * sizeof(b3Chunk));
	memset(m_chunks, 0, m_chunkCapacity * sizeof(b3Chunk));
	m_chunkCount = 0;

	memset(m_freeBlocks, 0, sizeof(m_freeBlocks));
}

b3BlockAllocator::~b3BlockAllocator() {
	for (u32 i = 0; i < m_chunkCount; ++i) {
		::b3Free(m_chunks[i].blocks);
	}
	::b3Free(m_chunks);
}

void* b3BlockAllocator::Allocate(u32 size) {
	if (size == 0) {
		return nullptr;
	}

	if (size > b3_maxBlockSize) {
		// Use the general purpose allocator (malloc).
		// Posteriorly, the memory will be freeded (free) if its size (passed) is larger than b3_maxBlockSize.
		return b3Alloc(size);
	}

	// Retain the block index that will be used.
	u32 blockIndex = m_blockSizeToFreeBlock[size];
	
	b3Assert(blockIndex < b3_blockSizes);

	// We have the block index for the size.

	if (m_freeBlocks[blockIndex]) {
		// If there is a free block then use it.
		b3Block* block = m_freeBlocks[blockIndex];
		// Adjust the singly-linked list of free blocks so the next block is available for the next allocation.
		m_freeBlocks[blockIndex] = block->next;
		// Return the block to the client.
		return block;
	}
	else {
		// If there is no free block available then a new one is allocated.
		// Link the singly-linked list of the new block.
		b3Block* block = (b3Block*)b3Alloc(b3_chunkSize);
#ifdef _DEBUG
		memset(block, 0xcd, b3_chunkSize);
#endif
		u32 blockSize = m_blockSizes[blockIndex];
		u32 blockCount = b3_chunkSize / blockSize;

		b3Assert(blockCount * blockSize <= b3_chunkSize);

		for (u32 i = 0; i < blockCount - 1; ++i) {
			b3Block* current = (b3Block*)((u8*)block + i * blockSize);
			current->next = (b3Block*)((u8*)block + (i + 1) * blockSize);
		}
		b3Block* last = (b3Block*)((u8*)block + (blockCount - 1) * blockSize);
		last->next = nullptr;

		// If there is no chunk available then a new one is allocated.
		if (m_chunkCount == m_chunkCapacity) {
			// ...then rebuild the chunk array.
			b3Chunk* oldChunks = m_chunks;
			// Increase capacity.
			m_chunkCapacity += b3_chunkArrayIncrement;
			// Allocate new chunk.
			m_chunks = (b3Chunk*)b3Alloc(m_chunkCapacity * sizeof(b3Chunk));
			memcpy(m_chunks, oldChunks, m_chunkCount * sizeof(b3Chunk));
			memset(m_chunks + m_chunkCount, 0, b3_chunkArrayIncrement * sizeof(b3Chunk));
			// Free old chunks.
			b3Free(oldChunks);
		}

		// Add new chunk and set the new block to it.
		b3Chunk* chunk = m_chunks + m_chunkCount;
		chunk->blockSize = blockSize;
		chunk->blocks = block;
		++m_chunkCount;

		// Make the free block of the current chunk available for the next allocation.
		m_freeBlocks[blockIndex] = chunk->blocks->next;

		return chunk->blocks;
	}
}

void b3BlockAllocator::Free(void* p, u32 size) {
	if (size == 0) {
		return;
	}

	if (size > b3_maxBlockSize) {
		// "...the memory will be freeded (free) if its size (passed) is larger than b3_maxBlockSize".
		::b3Free(p);
		return;
	}

	u32 blockIndex = m_blockSizeToFreeBlock[size];
	b3Assert(blockIndex >= 0 && blockIndex < b3_blockSizes);

#ifdef _DEBUG
	// Verify the memory address and size is valid.
	u32 blockSize = m_blockSizes[blockIndex];
	bool found = false;
	for (u32 i = 0; i < m_chunkCount; ++i) {
		b3Chunk* chunk = m_chunks + i;
		if (chunk->blockSize != blockSize) {
			b3Assert((u8*)p + blockSize <= (u8*)chunk->blocks || (u8*)chunk->blocks + b3_chunkSize <= (u8*)p);
		}
		else {
			if ((u8*)chunk->blocks <= (u8*)p && (u8*)p + blockSize <= (u8*)chunk->blocks + b3_chunkSize) {
				found = true;
			}
		}
	}

	b3Assert(found);

	memset(p, 0xfd, blockSize);
#endif
	// Adjust the block list and put the block back to the free list.
	b3Block* block = (b3Block*)p;
	block->next = m_freeBlocks[blockIndex];
	m_freeBlocks[blockIndex] = block;
}

void b3BlockAllocator::Clear() {
	for (u32 i = 0; i < m_chunkCount; ++i) {
		::b3Free(m_chunks[i].blocks);
	}

	m_chunkCount = 0;
	memset(m_chunks, 0, m_chunkCapacity * sizeof(b3Chunk));
	memset(m_freeBlocks, 0, sizeof(m_freeBlocks));
}
