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

#ifndef __B3_DRAW_H__
#define __B3_DRAW_H__

#include "Math\b3Math.h"
#include "..\Collision\b3Aabb.h"

struct b3Color {
	b3Color(r32 r, r32 g, r32 b) :
		R(r), 
		G(g), 
		B(b) {
	}
	r32 R, G, B;
};

// Implement this interface and call it using b3World::Draw(...) to debug the physics world.
class b3Draw {
public :
	// Use these bit flags to tell the world what needs to be rendered.
	enum b3DrawFlags {
		e_centerOfMassesFlag = 0x0001,
		e_contactsFlag = 0x0002,
		e_broadPhaseFlag = 0x0004
	};

	virtual void DrawPoint(const b3Vec3& position, const b3Color& color) const = 0;
	virtual void DrawLine(const b3Vec3& a, const b3Vec3& b, const b3Color& color) const = 0;
	virtual void DrawAABB(const b3AABB& aabb, const b3Color& color) const = 0;
};

#endif
