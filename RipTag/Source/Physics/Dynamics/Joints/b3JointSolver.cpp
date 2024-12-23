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
#include "BouncePCH.h"
#include "b3JointSolver.h"

b3JointSolver::b3JointSolver(const b3JointSolverDef* def) {
	m_count = def->count;
	m_joints = def->joints;

	m_solverData.invdt = def->dt > B3_ZERO ? B3_ONE / def->dt : B3_ZERO;
	m_solverData.positions = def->positions;
	m_solverData.velocities = def->velocities;
}

void b3JointSolver::InitializeVelocityConstraints() {
	for (u32 i = 0; i < m_count; ++i) {
		b3Joint* j = m_joints[i];
		j->InitializeVelocityConstraint(&m_solverData);
	}
}

void b3JointSolver::WarmStart() {
	for (u32 i = 0; i < m_count; ++i) {
		b3Joint* j = m_joints[i];
		j->WarmStart(&m_solverData);
	}
}

void b3JointSolver::SolveVelocityConstraints() {
	for (u32 i = 0; i < m_count; ++i) {
		b3Joint* j = m_joints[i];
		j->SolveVelocityConstraint(&m_solverData);
	}
}
