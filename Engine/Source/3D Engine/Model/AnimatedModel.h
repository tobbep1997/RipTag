#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include <../New_Library/FormatHeader.h>
//struct Joint;
//struct SkeletonPose;
//struct Skeleton;
//struct AnimationClip;




#define float4x4 XMFLOAT4X4A
#define float4 XMFLOAT4A
#define MAXJOINT 128

namespace Animation
{

	struct SRT
	{
		DirectX::float4 m_rotationQuaternion;
		DirectX::float4 m_translation;
		DirectX::float4 m_scale;

		SRT() {};
		SRT(const MyLibrary::Transform& transform)
		{
			using namespace DirectX;
			XMStoreFloat4A(&m_rotationQuaternion, XMQuaternionRotationRollPitchYaw(transform.transform_rotation[0], transform.transform_rotation[1], transform.transform_rotation[2]));
			m_translation = { transform.transform_position[0], transform.transform_position[1], transform.transform_position[2], 1.0f };
			m_scale = { 1.0, 1.0, 1.0, 1.0f };
		}
	};

	struct Joint
	{
		DirectX::float4x4 m_inverseBindPose;
		int16_t parentIndex;

		Joint() {};
		Joint(const MyLibrary::Joint& joint) 
		{

		};
	};

	struct Skeleton
	{
		uint8_t m_jointCount;
		Joint* m_joints;

		Skeleton() {};
		Skeleton(const MyLibrary::SkeletonFromFile& skeleton)
		{

		}
	};

	struct JointPose
	{
		SRT m_transformation;

		JointPose() {};
		JointPose(const SRT& srt)
		{
			m_transformation = srt;
		}
	};

	struct SkeletonPose
	{
		JointPose* m_jointPoses;
	};

	struct AnimationClip
	{
		Skeleton* m_skeleton;
		uint16_t m_frameCount;
		SkeletonPose* m_skeletonPoses;
		uint8_t m_framerate;

		AnimationClip() {};
		AnimationClip(const MyLibrary::AnimationFromFile& animation)
		{

		}
	};
	SRT ConvertTransformToSRT(MyLibrary::Transform transform);
	Animation::AnimationClip* ConvertToAnimationClip(MyLibrary::AnimationFromFile* animation, uint8_t jointCount);
	Skeleton* ConvertToSkeleton     (MyLibrary::SkeletonFromFile* skeleton);
	static void SetInverseBindPoses(Animation::Skeleton* mainSkeleton, MyLibrary::SkeletonFromFile* importedSkeleton);
	DirectX::XMMATRIX _createMatrixFromSRT(const SRT& srt);
	class AnimatedModel
	{
	public:
		AnimatedModel();
	
		~AnimatedModel();



		void Update(float deltaTime);
		void SetPlayingClip(AnimationClip* clip, bool isLooping = true);
		void SetSkeleton(Skeleton* skeleton);

		void Pause();
		void Play();

		std::vector<DirectX::float4x4>& GetSkinningMatrices();
	private:
		std::vector<DirectX::float4x4> m_skinningMatrices;
		std::vector<DirectX::float4x4> m_globalMatrices;


		Skeleton* m_skeleton = nullptr;
		AnimationClip* m_currentClip = nullptr;

		float m_currentTime = 0.0f;
		uint16_t m_currentFrame = 0;
		bool m_isPlaying = false;
		bool m_isLooping = true;

		void _computeSkinningMatrices(SkeletonPose* pose);
		void _computeModelMatrices(SkeletonPose* pose);
		void _interpolatePose(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight);
	};

	class AnimationCBuffer
	{
		struct AnimationBuffer
		{
			DirectX::XMFLOAT4X4 skinnedMatrix[MAXJOINT];
		};

		private:
			ID3D11Buffer* m_AnimationBuffer = nullptr;
			D3D11_MAPPED_SUBRESOURCE m_dataPtr;
			AnimationBuffer m_AnimationValues;

		public:
			AnimationCBuffer();
			~AnimationCBuffer();
			void SetAnimationCBuffer();
			void UpdateBuffer(AnimationBuffer *buffer);
			void UpdateBuffer(PVOID64 data, size_t dataSize);
			void SetToShader();
	};
}


