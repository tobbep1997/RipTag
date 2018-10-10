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

#ifndef __B3_ARRAY_H__
#define __B3_ARRAY_H__

#include "..\b3Settings.h"

// A non plain-old-data array.
// It can be used as a stack.
// @note The array doesn't uses Bounce allocators.
template <typename T, typename IT, IT N = 1>
class b3Array {
public :
	b3Array() {
		// Initially point to the pre-allocated memory. 
		m_elementCapacity = N;
		m_elements = m_freeElements;
		m_elementCount = 0;
	}

	~b3Array() {
		if (m_elements != m_freeElements) {
			// If the free elements were lost (have gone to full capacity).
			// Then free the general purpose allocated memory.
			for (IT i = 0; i < m_elementCount; ++i) {
				T* t = m_elements + i;
				t->~T();
			}

			b3Free(m_elements);
			m_elements = nullptr;
		}
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
		// O(n) in our guy.
		T* oldElements = m_elements;
		m_elementCapacity += size;
		m_elements = (T*)b3Alloc(m_elementCapacity * sizeof(T));

		// Allocate new elements.
		//@note Free elements aren't initialized.
		for (IT i = 0; i < size; ++i) {
			T* mem = m_elements + i;
			new (mem) T();
		}

		// Copy elements using the user-defined assignment operator.
		for (IT i = 0; i < m_elementCount; ++i) {
			T* mem = m_elements + i;
			*mem = oldElements[i];
		}
		
		if (oldElements != m_freeElements) {
			b3Free(oldElements);
		}

		m_elementCount = size;
	}

	void PushBack(const T& ele) {
		T* oldElements;
		if (m_elementCount == m_elementCapacity) {
			// Duplicate capacity.
			// O(n) in our guy.
			oldElements = m_elements;
			m_elementCapacity *= 2;
			m_elements = (T*)b3Alloc(m_elementCapacity * sizeof(T));

			// Reallocate elements.
			//@note Free elements aren't initialized.
			for (IT i = 0; i < m_elementCount; ++i) {
				T* mem = m_elements + i;
				new (mem) T();
				 // Dereference and copy it using the user-defined assignment operator.
				*mem = oldElements[i];
			}

			if (oldElements != m_freeElements) {
				b3Free(oldElements);
			}
		}

		T* mem = m_elements + m_elementCount;
		if (oldElements != m_freeElements) {
			// Build element.		
			new (mem) T();
		}
		// Copy it.
		*mem = ele;
		++m_elementCount;
	}

	void PopBack() {
		b3Assert(m_elementCount > 0);
		T* mem = m_elements + (m_elementCount - 1);
		mem->~T();
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
