#pragma once
#pragma warning(disable : 4244)
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include <../New_Library/FormatHeader.h>

#define MAXJOINT 128
#define BLEND_MATCH_TIME (1<<1)
#define BLEND_FROM_START (1<<2)
#define BLEND_MATCH_NORMALIZED_TIME (1<<3)
#define ANIMATION_SET_KEEP_OTHER (1<<1)
#define ANIMATION_SET_CLEAN (1<<2)

namespace Animation
{
#pragma region AnimatedModelClassStructs
	// Struct containing rotation, translation and scale as XMFLOAT4A, in that order
	struct SRT
	{
		DirectX::XMFLOAT4A m_rotationQuaternion;
		DirectX::XMFLOAT4A m_translation;
		DirectX::XMFLOAT4A m_scale;

		SRT() 
		{
			DirectX::XMStoreFloat4A(&m_rotationQuaternion, DirectX::XMVectorZero());
			DirectX::XMStoreFloat4A(&m_translation, DirectX::XMVectorZero());
			m_scale = { 1.0, 1.0, 1.0, 1.0 };
		};
		SRT(const MyLibrary::Transform& transform);
		SRT(const MyLibrary::DecomposedTransform& transform);
		bool operator==(const SRT& other);
	};

	// Struct containing a joint as XMFLOAT4X4A and a parent index as an int16_t, in that order
	struct Joint
	{
		DirectX::XMFLOAT4X4A m_inverseBindPose;
		int16_t parentIndex;

		Joint() {};
	};

	// Struct containing a joint count as a uint8_t and an array as a unique_ptr, in that order
	struct Skeleton
	{
		uint8_t m_jointCount;
		std::unique_ptr<Joint[]> m_joints;

		Skeleton() {};
		Skeleton(const MyLibrary::Skeleton& skeleton);
	};

	// Struct containing rot, trans, scale as an SRT struct
	struct JointPose
	{
		SRT m_transformation;

		JointPose() {};
		JointPose(const SRT& srt);
	};

	// Struct containing an array of JointPose structs as a unique_ptr
	struct SkeletonPose
	{
		std::unique_ptr<JointPose[]> m_jointPoses;
	};

	// Struct containing all necessary data for an animation for a given skeleton
	struct AnimationClip
	{
		Skeleton* m_skeleton;
		uint16_t m_frameCount;
		std::unique_ptr<SkeletonPose[]> m_skeletonPoses;
		uint8_t m_framerate;

		AnimationClip() {};
		AnimationClip(const MyLibrary::AnimationFromFileStefan& animation, Skeleton* skeleton);
		~AnimationClip();
	};

	struct CombinedClip
	{
		AnimationClip* firstClip = nullptr;
		AnimationClip* secondClip = nullptr;
		float secondWeight = 0.0;
		float secondCurrentTime = 0.0;
	};
#pragma endregion Joint, Skeleton, AnimationClip, ...

#pragma region GlobalAnimationFunctions
	SRT ConvertTransformToSRT(MyLibrary::Transform transform);
	Animation::AnimationClip* ConvertToAnimationClip(MyLibrary::AnimationFromFile* animation, uint8_t jointCount);
	void SetInverseBindPoses(Animation::Skeleton* mainSkeleton, const MyLibrary::Skeleton* importedSkeleton);
	DirectX::XMMATRIX _createMatrixFromSRT(const SRT& srt);
	DirectX::XMMATRIX _createMatrixFromSRT(const MyLibrary::DecomposedTransform& transform);
	Animation::AnimationClip* LoadAndCreateAnimation(std::string file, Animation::Skeleton* skeleton);
	Animation::Skeleton* LoadAndCreateSkeleton(std::string file);
	Animation::JointPose getDifferencePose(JointPose sourcePose, JointPose referencePose);
	Animation::AnimationClip* computeDifferenceClip(Animation::AnimationClip * sourceClip, Animation::AnimationClip * referenceClip);
	bool bakeDifferenceClipOntoClip(Animation::AnimationClip* targetClip, Animation::AnimationClip* differenceClip);

#pragma endregion Conversion stuff, Loaders, ...

	class AnimatedModel
	{
	public:
		AnimatedModel();
	
		~AnimatedModel();

		void Update(float deltaTime);
		void UpdateBlend(float deltaTime);
		void SetPlayingClip(AnimationClip* clip, bool isLooping = true);
		void SetLayeredClip(AnimationClip* clip, float weight, UINT flags = BLEND_MATCH_NORMALIZED_TIME, bool isLooping = true);
		void SetLayeredClipWeight(const float& weight);
		void SetTargetClip(AnimationClip* clip, UINT blendFlags = 0, float blendTime = 1.0f, bool isLooping = true);
		void SetSkeleton(Skeleton* skeleton);
		void SetScrubIndex(unsigned int index);
		void Pause();
		void Play();

		float GetCurrentTimeInClip();
		int GetCurrentFrameIndex();

		const std::vector<DirectX::XMFLOAT4X4A>& GetSkinningMatrices();
	private:
		std::vector<DirectX::XMFLOAT4X4A> m_skinningMatrices;
		std::vector<DirectX::XMFLOAT4X4A> m_globalMatrices;

		Skeleton* m_skeleton = nullptr;
		AnimationClip* m_currentClip = nullptr;
		CombinedClip m_combinedClip;
		AnimationClip* m_targetClip = nullptr;

		float m_currentBlendTime = 0.0;
		float m_targetBlendTime = 0.0;
		float m_targetClipCurrentTime = 0.0;

		float m_currentTime = 0.0f;
		uint16_t m_currentFrame = 0;
		bool m_isPlaying = false;
		bool m_isLooping = true;

		unsigned int m_scrubIndex = 0; // #todo remove

		void _computeSkinningMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight);
		void _computeSkinningMatrices(SkeletonPose* firstPose1, SkeletonPose* secondPose1, float weight1, SkeletonPose* firstPose2, SkeletonPose* secondPose2, float weight2);
		void _computeSkinningMatricesCombined(SkeletonPose* firstPose1, SkeletonPose* secondPose1, float weight1, SkeletonPose* firstPose2, SkeletonPose* secondPose2, float weight2);
		void _computeModelMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight);
		void _computeModelMatrices(SkeletonPose* firstPose1, SkeletonPose* secondPose1, float weight1, SkeletonPose* firstPose2, SkeletonPose* secondPose2, float weight2);
		void _interpolatePose(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight);
		JointPose _interpolateJointPose(JointPose * firstPose, JointPose * secondPose, float weight);
		std::pair<uint16_t, float> _computeIndexAndProgression(float deltaTime, float currentTime, uint16_t frameCount);
		std::pair<uint16_t, float> _computeIndexAndProgression(float deltaTime, float* currentTime, uint16_t frameCount);
		void UpdateCombined(float deltaTime);
		void _computeModelMatricesCombined(SkeletonPose* firstPose1, SkeletonPose* secondPose1, float weight1, SkeletonPose* firstPose2, SkeletonPose* secondPose2, float weight2);
	};

#pragma region AnimationCBufferClass
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
#pragma endregion Class for setting the skinning matrices to GPU

}


