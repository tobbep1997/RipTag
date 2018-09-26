#pragma once
#pragma warning(disable : 4244)
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <../New_Library/FormatHeader.h>
struct Joint;
struct SkeletonPose;
struct Skeleton;
struct AnimationClip;




namespace Animation
{
	using namespace DirectX;

	struct SRT
	{
		
		DirectX::XMFLOAT4A m_rotationQuaternion;
		DirectX::XMFLOAT4A m_translation;
		DirectX::XMFLOAT4A m_scale;
	};

	struct Joint
	{
		DirectX::XMFLOAT4X4A m_inverseBindPose;
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
	static SRT ConvertToSRT(const MyLibrary::Transform transform);
	
	class AnimatedModel
	{
	private:
		std::vector<DirectX::XMFLOAT4X4A> m_skinningMatrices;
		std::vector<DirectX::XMFLOAT4X4A> m_globalMatrices;

		Skeleton* m_skeleton = nullptr;
		AnimationClip* m_currentClip = nullptr;

		float m_currentTime = 0.0f;
		uint16_t m_currentFrame = 0;
		bool m_isPlaying = false;
		bool m_isLooping = true;

	public:
		AnimatedModel();	
		~AnimatedModel();

		void Update(float deltaTime);
		void SetPlayingClip(AnimationClip* clip, bool isLooping = true);

		void Pause();
		void Play();

	private:
		XMMATRIX _createMatrixFromSRT(const SRT& srt);
		void _computeSkinningMatrices(SkeletonPose* pose);
		void _computeModelMatrices(SkeletonPose* pose);
		XMMATRIX _recursiveMultiplyParents(uint8_t jointIndex, SkeletonPose* pose);
		void _interpolatePose(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight);
	};
}

