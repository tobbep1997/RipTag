#pragma once
#include <vector>

namespace Animation
{
	struct AnimationClip;
	struct SkeletonPose;
	struct SRT;
};

class AnimationDebugHelper
{
public:
	AnimationDebugHelper();
	~AnimationDebugHelper();

	static float foo;
	static float bar;
};

std::vector<Animation::SRT> GetVectorFromSkeletonPoseDebug(Animation::SkeletonPose* pose, uint16_t jointCount);



