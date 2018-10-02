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

#ifndef __B3_SETTINGS_H__
#define __B3_SETTINGS_H__

#include <cmath>
#include <float.h>
#include <assert.h>
#include <new>
#include <memory.h>

typedef signed char	i8;
typedef signed short i16;
typedef signed int i32;
typedef unsigned char u8;
typedef unsigned short u6;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float r32;
typedef double r64;

// You can modify the following parameters as long
// you're right.

#define B3_ZERO static_cast<r32>(0.0)
#define B3_HALF static_cast<r32>(0.5)
#define B3_ONE static_cast<r32>(1.0)
#define B3_TWO static_cast<r32>(2.0)
#define B3_PI static_cast<r32>(3.14159265359)
#define	B3_EPSILON (FLT_EPSILON)

#define	B3_MAX_FLOAT (FLT_MAX)
#define	B3_MAX_LONG (LONG_MAX)

#define B3_WORLD_AABB_EXTENSION static_cast<r32>(0.2)
#define B3_MAX_MANIFOLD_POINTS 8
#define B3_MAX_TANGENT_DIRECTIONS 2

#define B3_LINEAR_SLOP static_cast<r32>(0.005) //5cm
#define B3_ANGULAR_SLOP static_cast<r32>(2.0f / 180.0f * B3_PI)
#define B3_BAUMGARTE static_cast<r32>(0.2)
#define B3_GRAVITY_ACC static_cast<r32>(9.8)

#define B3_ONE_SECOND_MICROSECONDS (1000000ULL) //1s=1000000us
#define B3_ONE_MILISECOND_MICROSECONDS (1000ULL) //1ms=1000us
#define B3_TIME_TO_SLEEP static_cast<r32>(1.0)

#define B3_SLEEP_LINEAR_TOL static_cast<r32>( 0.05 ) //50cm
#define B3_SLEEP_ANGULAR_TOL static_cast<r32>( (2.0 / 180.0) * B3_PI )

#define b3Assert(c) assert(c)

void* b3Alloc(i32 size);
void b3Free(void* memory);

template <class T>
inline void b3Swap(T& a, T& b) {
	T tmp = a;
	a = b;
	b = tmp;
}

inline r32 b3Sqrt(r32 x) {
	return ::sqrt(x);
}

template <class T>
inline T b3Abs(T x) {
	return ::abs(x);
}

#endif
