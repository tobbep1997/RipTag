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

#ifndef B3_JOINT_SOLVER_H
#define B3_JOINT_SOLVER_H

#include "b3Joint.h"
#include "..\..\Common\b3Time.h"

struct b3Position;
struct b3Velocity;

struct b3JointVelocityConstraint {
	r32 imA;
	r32 imB;
	b3Mat33 iA;
	b3Mat33 iB;
	u32 indexA;
	u32 indexB;
};

struct b3JointSolverDef {
	r32 dt;
	b3Joint** joints;
	u32 count;
	b3Position* positions;
	b3Velocity* velocities;
};
// This solver Baumgarte method for constraint stabilization.
// It is rapidly executed, but it takes some time to completely solve a constraint.
// Each joint computes its own solver data due to the diversity of the 
// joint constraints.

class b3JointSolver {
public :
	b3JointSolver(const b3JointSolverDef* def);

	void InitializeVelocityConstraints();
	void WarmStart();
	void SolveVelocityConstraints();
protected :
	b3SolverData m_solverData;
	b3Joint** m_joints;
	u32 m_count;
};

#endif
