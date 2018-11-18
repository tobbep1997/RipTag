#pragma once
#pragma warning(disable : 4244)
#include <string>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <unordered_map>
#include "../../Components/StateMachine.h"
#define MAXJOINT 128
#define BLEND_MATCH_TIME (1<<1)
#define BLEND_FROM_START (1<<2)
#define BLEND_MATCH_NORMALIZED_TIME (1<<3)
#define ANIMATION_SET_KEEP_OTHER (1<<1)
#define ANIMATION_SET_CLEAN (1<<2)

#define ANIMATION_FRAMETIME 0.041666666f
#define ANIMATION_FRAMERATE 24
namespace ImporterLibrary
{
	class CustomFileLoader;
	struct Transform;
	struct DecomposedTransform;
	struct Skeleton;
	struct AnimationFromFileStefan;
	struct AnimationFromFile;
};


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
			: m_rotationQuaternion{0.0, 0.0, 0.0, 0.0}, m_translation{0.0, 0.0, 0.0, 0.0}, m_scale{1.0, 1.0, 1.0, 0.0}
		{
			//auto zeroVector = DirectX::XMVectorZero();
			//DirectX::XMStoreFloat4A(&m_rotationQuaternion, DirectX::XMVectorZero());
			//DirectX::XMStoreFloat4A(&m_translation, DirectX::XMVectorZero());
			//m_scale = { 1.0, 1.0, 1.0, 1.0 };
		};
		SRT(const ImporterLibrary::Transform& transform);
		SRT(const ImporterLibrary::DecomposedTransform& transform);
		SRT(const SRT& other) = default;
		SRT& operator=(const SRT& other) = default;
		SRT(SRT&& other) noexcept: 
			m_rotationQuaternion(std::move(other.m_rotationQuaternion)), 
			m_scale(std::move(other.m_scale)), 
			m_translation(std::move(other.m_translation)){};

		bool operator==(const SRT& other);
	};

	// Struct containing a joint as XMFLOAT4X4A and a parent index as an int16_t, in that order
	struct Joint
	{
		DirectX::XMFLOAT4X4A m_inverseBindPose;
		int16_t parentIndex;

		Joint() : parentIndex(-1)
		{
			using namespace DirectX;

			XMStoreFloat4x4A(&m_inverseBindPose, XMMatrixIdentity());
		};
	};

	// Struct containing a joint count as a uint8_t and an array as a unique_ptr, in that order
	struct Skeleton
	{
		uint8_t m_jointCount;
		std::unique_ptr<Joint[]> m_joints;

		Skeleton() : m_jointCount(0){};
		Skeleton(const ImporterLibrary::Skeleton& skeleton);

		//Move ctor
		Skeleton(Skeleton&& other) noexcept 
			: m_joints(std::move(other.m_joints)), 
		  m_jointCount(std::move(other.m_jointCount)){}
	};

	// Struct containing rot, trans, scale as an SRT struct
	struct JointPose
	{
		SRT m_transformation;

		JointPose() {};
		JointPose(const SRT& srt);
		JointPose(const JointPose& srt) = default;
		JointPose& operator=(const JointPose& srt) = default;
		JointPose(JointPose&& other) = default;
	};

	// Struct containing an array of JointPose structs as a unique_ptr
	struct SkeletonPose
	{
		std::unique_ptr<JointPose[]> m_jointPoses;

		SkeletonPose() = default;
		SkeletonPose(size_t jointCount) 
			: m_jointPoses(std::make_unique<JointPose[]>(jointCount)){}
		SkeletonPose(SkeletonPose&& other) noexcept 
			: m_jointPoses(std::move(other.m_jointPoses)){}
		SkeletonPose& operator=(SkeletonPose&& other) noexcept
		{
			m_jointPoses.swap(other.m_jointPoses);
			return *this; 
		}
	};

	// Struct containing all necessary data for an animation for a given skeleton
	struct AnimationClip
	{
		std::shared_ptr<Skeleton> m_skeleton;
		uint16_t m_frameCount;
		std::unique_ptr<SkeletonPose[]> m_skeletonPoses;
		uint8_t m_framerate;

		AnimationClip() : m_frameCount(0), m_framerate(24){};
		AnimationClip(uint8_t frameCount, uint8_t jointCount)
			: m_frameCount(frameCount), m_framerate(24)
		{
			m_skeletonPoses = std::make_unique<SkeletonPose[]>(frameCount);
			for (int i = 0; i < jointCount; i++)
				m_skeletonPoses[i] = SkeletonPose(jointCount);
		};
		AnimationClip(const ImporterLibrary::AnimationFromFileStefan& animation, std::shared_ptr<Animation::Skeleton> skeleton);
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
	typedef std::shared_ptr<Animation::AnimationClip> SharedAnimation;
	typedef std::unordered_map<std::string, SharedAnimation> ClipCollection;
	typedef std::shared_ptr<ClipCollection> SharedClipCollection;
	typedef std::shared_ptr<Skeleton> SharedSkeleton;
#pragma region GlobalAnimationFunctions
	SRT ConvertTransformToSRT(ImporterLibrary::Transform transform);
	Animation::SharedAnimation ConvertToAnimationClip(ImporterLibrary::AnimationFromFile* animation, uint8_t jointCount);
	void SetInverseBindPoses(Animation::Skeleton* mainSkeleton, const ImporterLibrary::Skeleton* importedSkeleton);
	DirectX::XMMATRIX _createMatrixFromSRT(const SRT& srt);
	DirectX::XMMATRIX _createMatrixFromSRT(const ImporterLibrary::DecomposedTransform& transform);
	Animation::SharedAnimation LoadAndCreateAnimation(std::string file, std::shared_ptr<Skeleton> skeleton);
	SharedSkeleton LoadAndCreateSkeleton(std::string file);
	Animation::JointPose getDifferencePose(JointPose sourcePose, JointPose referencePose);
	Animation::JointPose getAdditivePose(JointPose targetPose, JointPose differencePose);
	Animation::AnimationClip* computeDifferenceClip(Animation::AnimationClip * sourceClip, Animation::AnimationClip * referenceClip);
	bool bakeDifferenceClipOntoClip(Animation::AnimationClip* targetClip, Animation::AnimationClip* differenceClip);

#pragma endregion Conversion stuff, Loaders, ...

	struct SkeletonPosePair
	{
		SkeletonPose* first;
		SkeletonPose* second;
		float blendFactor;
	};

	class AnimatedModel
	{
	public:
		enum ANIMATION_TYPE_STATE
		{
			STATE_1D_BLEND,
			STATE_2D_BLEND,
			STATE_SINGLE_CLIP
		};
		AnimatedModel();
	
		~AnimatedModel();

		void Update(float deltaTime);
		SkeletonPose UpdateBlendspace1D(SM::BlendSpace1D::Current1DStateData stateData);
		SkeletonPose UpdateBlendspace2D(SM::BlendSpace2D::Current2DStateData stateData);
		void SetPlayingClip(AnimationClip* clip, bool isLooping = true, bool keepCurrentNormalizedTime = false);
		void SetSkeleton(SharedSkeleton skeleton);
		void Pause();
		void Play();

		std::unique_ptr<SM::AnimationStateMachine>& GetStateMachine();
		std::unique_ptr<SM::AnimationStateMachine>& GetLayerStateMachine();
		std::unique_ptr<SM::AnimationStateMachine>& InitStateMachine(size_t numStates);
		std::unique_ptr<SM::AnimationStateMachine>& InitLayerStateMachine(size_t numStates);

		const std::vector<DirectX::XMFLOAT4X4A>& GetSkinningMatrices();
		float GetCachedDeltaTime();
	private:
		float m_currentFrameDeltaTime = 0.0f;

		std::unique_ptr<SM::AnimationStateMachine> m_StateMachine;
		std::unique_ptr<SM::AnimationStateMachine> m_LayerStateMachine;
		std::unique_ptr<SM::StateVisitor> m_Visitor;
		std::unique_ptr<SM::LayerVisitor> m_LayerVisitor;

		std::vector<DirectX::XMFLOAT4X4A> m_skinningMatrices;
		std::vector<DirectX::XMFLOAT4X4A> m_globalMatrices;
		
		SharedSkeleton m_skeleton = nullptr;
		AnimationClip* m_currentClip = nullptr;

		float m_currentBlendTime = 0.0;
		float m_targetBlendTime = 0.0;

		float m_currentTime = 0.0f;
		float m_currentNormalizedTime = 0.0f;
		bool timeAlreadyUpdatedThisFrame = false;
		bool m_isPlaying = false;
		bool m_isLooping = true;

	public:
		//-- Helper functions --
		JointPose    _BlendJointPoses(JointPose* firstPose, JointPose* secondPose, float blendFactor);
		SkeletonPose _BlendSkeletonPoses(SkeletonPose* firstPose, SkeletonPose* secondPose, float blendFactor, size_t jointCount);
		SkeletonPose _BlendSkeletonPoses2D(SkeletonPosePair firstPair, SkeletonPosePair secondPair, float pairsBlendFactor, size_t jointCount);
		//----------------------
	private:

		void _computeSkinningMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight);
		void _computeSkinningMatrices(SkeletonPose* pose);
		void _computeModelMatrices(SkeletonPose* pose);
		void _computeModelMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight);
		void _interpolatePose(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight);
		JointPose _interpolateJointPose(JointPose * firstPose, JointPose * secondPose, float weight);
		std::pair<uint16_t, float> _computeIndexAndProgression(float deltaTime, float currentTime, uint16_t frameCount);
		std::pair<uint16_t, float> _computeIndexAndProgression(float deltaTime, float* currentTime, uint16_t frameCount);
		std::optional<std::pair<uint16_t, float>> _computeIndexAndProgressionOnce(float deltaTime, float* currentTime, uint16_t frameCount);
		std::pair<uint16_t, float> _computeIndexAndProgressionNormalized(float deltaTime, float* currentTime, uint16_t frameCount);


	public:
		void UpdateLooping(Animation::AnimationClip* clip);
		void UpdateOnce(Animation::AnimationClip* clip);
	};

	//Stuff
	inline SkeletonPose MakeSkeletonPose(const SkeletonPose& referencePose, uint16_t jointCount)
	{
		SkeletonPose newPose;
		newPose.m_jointPoses = std::make_unique<JointPose[]>(jointCount);
		
		for (int i = 0; i < jointCount; i++)
			newPose.m_jointPoses[i].m_transformation = referencePose.m_jointPoses[i].m_transformation;

		return std::move(newPose);
	}
	//

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


