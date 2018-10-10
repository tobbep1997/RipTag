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

#ifndef __B3_ARRAY_POD_H__
#define __B3_ARRAY_POD_H__

#include "..\b3Settings.h"

// A plain-old-data (POD) array for bytes.
// It is faster than b3Array
// and it can be used as a stack.
// @note The array doesn't uses Bounce allocators.
template <typename T, typename IT, IT N = 1>
class b3ArrayPOD {
public :
	b3ArrayPOD() {
		// Initially point to the pre-allocated memory. 
		m_elementCapacity = N;
		m_elements = m_freeElements;
		m_elementCount = 0;
	}

	~b3ArrayPOD() {
		if (m_elements != m_freeElements) {
			// If the free elements were lost (have gone to full capacity).
			// Then free the general purpose allocated memory.
			b3Free(m_elements);
			m_elements = nullptr;
		}
	}

	b3ArrayPOD<T, IT, N>& operator=(const b3ArrayPOD<T, IT, N>& other) {
		Resize(other.Size());
		::memcpy(m_elements, other.m_elements, other.m_elementCount * sizeof(T));
		return (*this);
	}

	void Resize(IT size) {
		// Array is already resized.
		if (m_elementCount == size) {
			return;
		}

		// Check if we could use the free list as the memory source.
		if (size <= m_elementCapacity) {
			m_elementCount = size;
			return;
		}

		// Increase capacity.
		// O(n) on our guy.
		T* oldElements = m_elements;
		m_elementCapacity += size;
		m_elements = (T*)b3Alloc(m_elementCapacity * sizeof(T));
		::memcpy(m_elements, oldElements, m_elementCount * sizeof(T));
		m_elementCount = size;
		if (oldElements != m_freeElements) {
			b3Free(oldElements);
		}
	}

	void PushBack(const T& ele) {
		if (m_elementCount == m_elementCapacity) {
			// Duplicate capacity.
			// Free elements are lost.
			T* oldElements = m_elements;
			m_elementCapacity *= 2;
			m_elements = (T*)b3Alloc(m_elementCapacity * sizeof(T));
			::memcpy(m_elements, oldElements, m_elementCount * sizeof(T));

			if (oldElements != m_freeElements) {
				b3Free(oldElements);
			}
		}

		m_elements[m_elementCount] = ele;
		++m_elementCount;
	}

	void PopBack() {
		b3Assert(m_elementCount > 0);
		--m_elementCount;
	}

	T& Back() {
		return m_elements[m_elementCount - 1];
	}

	const T& Back() const {
		return m_elements[m_elementCount - 1];
	}

	T& operator[](IT i) {
		b3Assert(i < m_elementCount);
		return m_elements[i];
	}

	const T& operator[](IT i) const {
		b3Assert(i < m_elementCount);
		return m_elements[i];
	}

	IT Size() const {
		return m_elementCount;
	}

	bool Empty() const {
		return m_elementCount == 0;
	}
protected:
	IT m_elementCapacity;
	T m_freeElements[N];
	T* m_elements;
	IT m_elementCount;
};

#endif
