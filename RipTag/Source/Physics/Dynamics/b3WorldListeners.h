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

#ifndef __B3_WORLD_LISTENERS_H__
#define __B3_WORLD_LISTENERS_H__

#include "..\Common\Math\b3Math.h"

class b3Shape;
class b3Contact;

class b3QueryListener {
public :
	virtual ~b3QueryListener() {}
	virtual bool ReportShape(b3Shape* shape) = 0;
};

class b3RayCastListener {
public:	
	// The user must return the new ray cast fraction.
	// If fraction == zero then the ray cast query will be canceled.
	virtual ~b3RayCastListener() { }
	virtual r32 ReportShape(b3Shape* shape, const b3Vec3& point, const b3Vec3& normal, r32 fraction) = 0;
};

class b3ContactListener {
public:
	// Inherit from this class and set it in the world to listen for collision events.	
	// Call the functions below to inspect when a shape start/end colliding with another shape.
	/// @warning You cannot create/destroy Bounc3 objects inside these callbacks.
	virtual void BeginContact(b3Contact* contact) = 0;
	virtual void EndContact(b3Contact* contact) = 0;
	virtual void Persisting(b3Contact* contact) = 0;
};

#endif
