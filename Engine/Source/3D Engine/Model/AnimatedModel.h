#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <FormatHeader.h>
#include "FormatHeader.h"

struct Joint;
struct SkeletonPose;
struct Skeleton;
struct AnimationClip;
class SkinnedMesh;


#define DX DirectX
#define float4x4 XMFLOAT4X4A
#define float4 XMFLOAT4A

namespace Animation
{
	using namespace DirectX;

	struct SRT
	{
		float4 m_rotationQuaternion;
		float4 m_translation;
		float4 m_scale;
	};

	struct Joint
	{
		float4x4 m_inverseBindPose;
		uint8_t parentIndex;
	};

	struct Skeleton
	{
		uint8_t m_jointCount;
		Joint* m_joints;
	};

	struct JointPose
	{
		SRT m_transformation;
	};

	struct SkeletonPose
	{
		Skeleton* m_skeleton;
		JointPose* m_jointPoses;
	};

	struct AnimationClip
	{
		Skeleton* m_skeleton;
		uint16_t m_frameCount;
		SkeletonPose* m_skeletonPoses;
		uint8_t m_framerate;
	};

	static AnimationClip* ConvertToAnimationClip(MyLibrary::AnimationFromFile* animation, uint8_t jointCount);
	static Skeleton* ConvertToSkeleton(MyLibrary::SkeletonFromFile* skeleton);
	static XMMATRIX _createMatrixFromSRT(const SRT& srt);

	static SRT _convertToSRT(const MyLibrary::Transform transform);
	
	class AnimatedModel
	{
	public:
		AnimatedModel();
		AnimatedModel(SkinnedMesh* mesh);
		~AnimatedModel();

		void Init(SkinnedMesh* mesh);

		void Update(float deltaTime);
		void SetPlayingClip(AnimationClip* clip, bool isLooping = true);

		void Pause();
		void Play();

	private:
		std::vector<DX::float4x4> m_skinningMatrices;
		std::vector<DX::float4x4> m_globalMatrices;

		SkinnedMesh* m_mesh = nullptr;
		Skeleton* m_skeleton = nullptr;
		AnimationClip* m_currentClip = nullptr;

		float m_currentTime = 0.0f;
		uint16_t m_currentFrame = 0;
		bool m_isPlaying = false;
		bool m_isLooping = true;

		void _computeSkinningMatrices(SkeletonPose* pose);
		void _computeModelMatrices(SkeletonPose* pose);
		XMMATRIX recursiveMultiplyParents(uint8_t jointIndex, SkeletonPose* pose);
		void _interpolatePose(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight);
	};
}


