#pragma once
#pragma warning(disable : 4244)
#include <string>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <unordered_map>
#include "../../Components/StateMachine.h"
class LayerMachine;
class Drawable;

#define MAXJOINT 128
#define BLEND_MATCH_TIME (1<<1)
#define BLEND_FROM_START (1<<2)
#define BLEND_MATCH_NORMALIZED_TIME (1<<3)
#define ANIMATION_SET_KEEP_OTHER (1<<1)
#define ANIMATION_SET_CLEAN (1<<2)

#define ANIMATION_FRAMETIME 0.041666666f
#define ANIMATION_FRAMERATE 24.0f

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
#pragma region AnimationPlayerClassStructs
	// Struct containing rotation, translation and scale as XMFLOAT4A, in that order
	struct SRT
	{
		DirectX::XMFLOAT4A m_RotationQuaternion;
		DirectX::XMFLOAT4A m_Translation;
		DirectX::XMFLOAT4A m_Scale;

		SRT()
			: m_RotationQuaternion{0.0, 0.0, 0.0, 0.0}, m_Translation{0.0, 0.0, 0.0, 0.0}, m_Scale{1.0, 1.0, 1.0, 0.0}
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
			m_RotationQuaternion(std::move(other.m_RotationQuaternion)), 
			m_Scale(std::move(other.m_Scale)), 
			m_Translation(std::move(other.m_Translation)){};

		bool operator==(const SRT& other);
	};

	// Struct containing a joint as XMFLOAT4X4A and a parent index as an int16_t, in that order
	struct Joint
	{
		DirectX::XMFLOAT4X4A m_InverseBindPose;
		int16_t m_ParentIndex;

		Joint() : m_ParentIndex(-1)
		{
			using namespace DirectX;

			XMStoreFloat4x4A(&m_InverseBindPose, XMMatrixIdentity());
		};
	};

	// Struct containing a joint count as a uint8_t and an array as a unique_ptr, in that order
	struct Skeleton
	{
		uint8_t m_JointCount;
		std::unique_ptr<Joint[]> m_Joints;

		Skeleton() : m_JointCount(0){};
		Skeleton(const ImporterLibrary::Skeleton& skeleton);

		//Move ctor
		Skeleton(Skeleton&& other) noexcept 
			: m_Joints(std::move(other.m_Joints)), 
		  m_JointCount(std::move(other.m_JointCount)){}
	};

	// Struct containing rot, trans, scale as an SRT struct
	struct JointPose
	{
		SRT m_Transformation;

		JointPose() {};
		JointPose(const SRT& srt);
		JointPose(const JointPose& srt) = default;
		JointPose& operator=(const JointPose& srt) = default;
		JointPose(JointPose&& other) = default;
	};

	// Struct containing an array of JointPose structs as a unique_ptr
	struct SkeletonPose
	{
		std::unique_ptr<JointPose[]> m_JointPoses;

		SkeletonPose() = default;
		SkeletonPose(size_t jointCount) 
			: m_JointPoses(std::make_unique<JointPose[]>(jointCount)){}
		SkeletonPose(SkeletonPose&& other) noexcept 
			: m_JointPoses(std::move(other.m_JointPoses)){}
		SkeletonPose& operator=(SkeletonPose&& other) noexcept
		{
			m_JointPoses.swap(other.m_JointPoses);
			return *this; 
		}
	};

	// Struct containing all necessary data for an animation for a given skeleton
	struct AnimationClip
	{
		std::shared_ptr<Skeleton> m_Skeleton;
		uint16_t m_FrameCount;
		std::unique_ptr<SkeletonPose[]> m_SkeletonPoses;
		uint8_t m_Framerate;

		AnimationClip() : m_FrameCount(0), m_Framerate(24){};
		AnimationClip(uint8_t frameCount, uint8_t jointCount)
			: m_FrameCount(frameCount), m_Framerate(24)
		{
			m_SkeletonPoses = std::make_unique<SkeletonPose[]>(frameCount);
			for (int i = 0; i < jointCount; i++)
				m_SkeletonPoses[i] = SkeletonPose(jointCount);
		};
		AnimationClip(const ImporterLibrary::AnimationFromFileStefan& animation, std::shared_ptr<Animation::Skeleton> skeleton);
		~AnimationClip();
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
	DirectX::XMMATRIX _createMatrixFromSRT(const ImporterLibrary::DecomposedTransform& transform);
	Animation::SharedAnimation LoadAndCreateAnimation(std::string file, std::shared_ptr<Skeleton> skeleton);
	SharedSkeleton LoadAndCreateSkeleton(std::string file);
	Animation::JointPose getAdditivePose(JointPose targetPose, JointPose differencePose);

#pragma endregion Conversion stuff, Loaders, ...

	struct SkeletonPosePair
	{
		SkeletonPose* first;
		SkeletonPose* second;
		float blendFactor;
	};

	class AnimationPlayer
	{
	public:
		AnimationPlayer(Drawable* owner);
		~AnimationPlayer();

		void Update(float deltaTime);
		SkeletonPose UpdateBlendspace1D(SM::BlendSpace1D::Current1DStateData stateData);
		SkeletonPose UpdateBlendspace2D(SM::BlendSpace2D::Current2DStateData stateData);
		void UpdateWithPose(Animation::SkeletonPose* pose);
		void SetPlayingClip(AnimationClip* clip, bool isLooping = true, bool keepCurrentNormalizedTime = false);
		void SetSkeleton(SharedSkeleton skeleton);
		uint16_t GetSkeletonJointCount();
		void Pause();
		void Play();
		void Reset();

		float GetTimeLeft(bool useNormalizedTime);

		///Joint parenting
		DirectX::XMMATRIX GetModelMatrixForJoint(uint16_t jointIndex);
		DirectX::XMVECTOR GetPositionForJoint(uint16_t jointIndex);
		DirectX::XMVECTOR GetOrientationForJoint(uint16_t jointIndex);
		std::pair<DirectX::XMVECTOR, DirectX::XMVECTOR> GetLocalPositionAndOrientationOfJoint(uint16_t jointIndex);
		std::pair<DirectX::XMVECTOR, DirectX::XMVECTOR> GetWorldPositionAndOrientationOfJoint(uint16_t jointIndex);
		///---------------

		std::unique_ptr<LayerMachine>& GetLayerMachine();
		std::unique_ptr<SM::AnimationStateMachine>& GetStateMachine();
		std::unique_ptr<SM::AnimationStateMachine>& GetLayerStateMachine();
		std::unique_ptr<SM::AnimationStateMachine>& InitStateMachine(size_t numStates);
		std::unique_ptr<SM::AnimationStateMachine>& InitLayerStateMachine(size_t numStates);

		const std::vector<DirectX::XMFLOAT4X4A>& GetSkinningMatrices(); 
		float GetCachedDeltaTime();
		std::unique_ptr<LayerMachine>& InitLayerMachine(Animation::Skeleton* skeleton);
	private:
		Drawable* m_Owner{ nullptr };


		std::unique_ptr<LayerMachine> m_LayerMachine{};
		std::unique_ptr<SM::AnimationStateMachine> m_StateMachine;
		std::unique_ptr<SM::AnimationStateMachine> m_LayerStateMachine;
		std::unique_ptr<SM::StateVisitor> m_Visitor;
		std::unique_ptr<SM::LayerVisitor> m_LayerVisitor;

		std::vector<DirectX::XMFLOAT4X4A> m_SkinningMatrices;
		std::vector<DirectX::XMFLOAT4X4A> m_GlobalMatrices;
		
		SharedSkeleton m_Skeleton = nullptr;
		AnimationClip* m_CurrentClip = nullptr;

		float m_currentFrameDeltaTime = 0.0f;
		float m_CurrentTime = 0.0f;
		float m_CurrentNormalizedTime = 0.0f;
		bool m_TimeAlreadyUpdatedThisFrame = false;
		bool m_IsPlaying = false;
		bool m_IsLooping = true;

	public:
		//-- Helper functions --
		static DirectX::XMMATRIX _CreateMatrixFromSRT(const SRT& srt);

		static void         _ScalePose(Animation::SkeletonPose* pose, float scale, uint16_t jointCount);
		static JointPose    _ScalePose(Animation::JointPose& pose, float scale);
		static JointPose    _GetAdditivePose(Animation::JointPose targetPose, DirectX::XMMATRIX differencePose);
		static JointPose    _BlendJointPoses(JointPose* firstPose, JointPose* secondPose, float blendFactor);
		static SkeletonPose _BlendSkeletonPoses(SkeletonPose* firstPose, SkeletonPose* secondPose, float blendFactor, size_t jointCount);
		static SkeletonPose _BlendSkeletonPoses2D(SkeletonPosePair firstPair, SkeletonPosePair secondPair, float pairsBlendFactor, size_t jointCount);
		//----------------------
	private:

		void _ComputeSkinningMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight);
		void _ComputeSkinningMatrices(SkeletonPose* pose);
		void _ComputeModelMatrices(SkeletonPose* pose);
		void _ComputeModelMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight);
		std::pair<uint16_t, float> _ComputeIndexAndProgression(float deltaTime, float currentTime, uint16_t frameCount);
		std::pair<uint16_t, float> _ComputeIndexAndProgression(float deltaTime, float* currentTime, uint16_t frameCount);
		std::optional<std::pair<uint16_t, float>> _ComputeIndexAndProgressionOnce(float deltaTime, float* currentTime, uint16_t frameCount);
		std::pair<uint16_t, float> _ComputeIndexAndProgressionNormalized(float deltaTime, float* currentTime, uint16_t frameCount);


	public:
		Animation::SkeletonPose UpdateLooping(Animation::AnimationClip* clip);
		std::optional<Animation::SkeletonPose> UpdateOnce(Animation::AnimationClip* clip);
	};

	//Stuff
	inline SkeletonPose MakeSkeletonPose(const SkeletonPose& referencePose, uint16_t jointCount)
	{
		SkeletonPose newPose;
		newPose.m_JointPoses = std::make_unique<JointPose[]>(jointCount);
		
		for (int j = 0; j < jointCount; j++)
			newPose.m_JointPoses[j].m_Transformation = referencePose.m_JointPoses[j].m_Transformation;

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


