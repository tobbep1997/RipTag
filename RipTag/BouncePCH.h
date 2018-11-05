#pragma once

#include <map>
#include <Windows.h>

#include "Source/Physics/Collision/b3SAT.h"
#include "Source/Physics/Collision/Shapes/b3Hull.h"
#include "Source/Physics/Collision/Shapes/b3Shape.h"
#include "Source/Physics/Collision/Shapes/b3Polyhedron.h"


#include "Source/Physics/Common/b3Time.h"
#include "Source/Physics/Common/b3Draw.h"
#include "Source/Physics/Common/Memory/b3Array.h"
#include "Source/Physics/Common/Memory/b3ArrayPOD.h"
#include "Source/Physics/Common/Memory/b3BlockAllocator.h"
#include "Source/Physics/Common/Memory/b3StackAllocator.h"

#include "Source/Physics/Dynamics/b3Body.h"
#include "Source/Physics/Dynamics/b3World.h"
#include "Source/Physics/Dynamics/b3Island.h"
#include "Source/Physics/Dynamics/b3WorldListeners.h"
#include "Source/Physics/Dynamics/Contacts/b3Contact.h"
#include "Source/Physics/Dynamics/Contacts/b3ContactSolver.h"
#include "Source/Physics/Dynamics/Joints/b3Joint.h"
#include "Source/Physics/Dynamics/Joints/b3MouseJoint.h"
#include "Source/Physics/Dynamics/Joints/b3JointSolver.h"
#include "Source/Physics/Dynamics/Joints/b3SphericalJoint.h"
#include "Source/Physics/Dynamics/Joints/b3RevoluteJoint.h"
