#include "EnginePCH.h"

float AnimationDebugHelper::foo = 0.0f;
float AnimationDebugHelper::bar = 0.0f;

AnimationDebugHelper::AnimationDebugHelper()
{
}
AnimationDebugHelper::~AnimationDebugHelper()
{
}

std::vector<Animation::SRT> GetVectorFromSkeletonPoseDebug(Animation::SkeletonPose* pose, uint16_t jointCount)
{
	std::vector<Animation::SRT> v;
	v.resize(jointCount);

	for (int joint = 0; joint < jointCount; joint++)
	{
		v[joint] = pose->m_JointPoses[joint].m_Transformation;
	}
	return v;
}
