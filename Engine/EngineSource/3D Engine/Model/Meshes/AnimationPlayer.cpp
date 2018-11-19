#include "EnginePCH.h"
#include "AnimationPlayer.h"
#include "../../../Helper/AnimationHelpers.h"
#include "../../Components/LayerMachine.h"

Animation::AnimationPlayer::AnimationPlayer()
{
// 	m_SkinningMatrices.resize(128);
// 	m_GlobalMatrices.resize(128);
/*	DirectX::XMFLOAT4X4A identityMatrix = {};*/
// 	DirectX::XMStoreFloat4x4A(&identityMatrix, DirectX::XMMatrixIdentity());
// 	std::fill(m_SkinningMatrices.begin(), m_SkinningMatrices.end(), identityMatrix);
// 	std::fill(m_GlobalMatrices.begin(), m_GlobalMatrices.end(), identityMatrix);
}

Animation::AnimationPlayer::~AnimationPlayer()
{
}

// Computes the frame we're currently on and computes final skinning matrices for gpu skinning
void Animation::AnimationPlayer::Update(float deltaTime)
{
	m_TimeAlreadyUpdatedThisFrame = false;
	m_currentFrameDeltaTime = deltaTime;

	if (!m_IsPlaying)
		return;

	if (m_StateMachine)
	{


		m_StateMachine->UpdateCurrentState();
		
		//Blendfactor used to blend between two states.
		float blendFactor = 1.0 - m_StateMachine->UpdateBlendFactor(deltaTime);


		auto finalPoseCurrent = m_StateMachine->GetCurrentState().recieveStateVisitor(*m_Visitor);
		auto previousState = m_StateMachine->GetPreviousState();
		
		//Layer
		if (m_LayerStateMachine)
		{
			auto finalLayerPose = m_LayerStateMachine->GetCurrentState().recieveStateVisitor(*m_LayerVisitor);

			if (previousState)
			{
				auto finalPosePrevious = previousState->recieveStateVisitor(*m_Visitor);
				_ComputeSkinningMatrices(&finalPosePrevious.value(), &finalPoseCurrent.value(), blendFactor);
				return;
			}
			if (finalPoseCurrent.value().m_JointPoses)
				_ComputeSkinningMatrices(&finalPoseCurrent.value());

			return;
		}
		else //no layers
		{
			if (previousState)
			{
				auto finalPosePrevious = previousState->recieveStateVisitor(*m_Visitor);
				_ComputeSkinningMatrices(&finalPosePrevious.value(), &finalPoseCurrent.value(), blendFactor);
				return;
			}
			if (finalPoseCurrent.value().m_JointPoses)
				_ComputeSkinningMatrices(&finalPoseCurrent.value());
			return;
		}
	}
	else
	{
		/// increase local time
		//done in _computeIndexAndProgression()

		///calc the actual frame index and progression towards the next frame
		auto indexAndProgression = _ComputeIndexAndProgression(deltaTime, &m_CurrentTime, m_CurrentClip->m_FrameCount);
		auto prevIndex = indexAndProgression.first;
		auto progression = indexAndProgression.second;

		///if we exceeded clips time, set back to 0 ish if we are looping, or stop if we aren't
		if (prevIndex >= m_CurrentClip->m_FrameCount -1) /// -1 because last frame is only used to interpolate towards
		{
			if (m_IsLooping)
			{
				m_CurrentTime = 0.0 + progression;
	
				prevIndex = std::floorf(m_CurrentClip->m_Framerate / 2* m_CurrentTime);
				progression = std::fmod(m_CurrentTime, 1.0 / 24.0);
			}
			else
			{
				m_IsPlaying = false; 
			}
		}

		/// compute skinning matrices
		if (m_IsPlaying)
			_ComputeSkinningMatrices(&m_CurrentClip->m_SkeletonPoses[prevIndex], &m_CurrentClip->m_SkeletonPoses[prevIndex + 1], progression);
	}
}

float getSpeedScale(size_t firstFrameCount, size_t secondFrameCount, float weight)
{
	float scale = static_cast<float>(firstFrameCount) / static_cast<float>(secondFrameCount);
	scale += Animation::lerp(0.0, 1.0 - scale, weight);
	return scale;
}

Animation::SkeletonPose Animation::AnimationPlayer::UpdateBlendspace1D(SM::BlendSpace1D::Current1DStateData stateData)
{
	if (!stateData.first)
		return Animation::SkeletonPose();

	float speedScale = 1.0f;
	std::pair<uint16_t, float> indexAndProgressionFirst{};
	std::pair<uint16_t, float> indexAndProgressionSecond{};
	if (stateData.second)
	{
		speedScale = getSpeedScale(stateData.second->m_FrameCount, stateData.first->m_FrameCount, stateData.weight);
		assert(speedScale >= 0.0f);
		indexAndProgressionSecond = _ComputeIndexAndProgressionNormalized(m_currentFrameDeltaTime * speedScale, &m_CurrentNormalizedTime, stateData.second->m_FrameCount);
	}
	indexAndProgressionFirst= _ComputeIndexAndProgressionNormalized(m_currentFrameDeltaTime, &m_CurrentNormalizedTime, stateData.first->m_FrameCount);

	auto prevIndexFirst = indexAndProgressionFirst.first;
	auto progressionFirst = indexAndProgressionFirst.second;
	auto prevIndexSecond = indexAndProgressionSecond.first;
	auto progressionSecond = indexAndProgressionSecond.second;


	SkeletonPose finalPose;
	if (!stateData.second)
	{
		finalPose = _BlendSkeletonPoses(&stateData.first->m_SkeletonPoses[prevIndexFirst], &stateData.first->m_SkeletonPoses[prevIndexFirst + 1], progressionFirst, m_Skeleton->m_JointCount);
	}
	else
	{
		auto finalFirst = _BlendSkeletonPoses(&stateData.first->m_SkeletonPoses[prevIndexFirst], &stateData.first->m_SkeletonPoses[prevIndexFirst + 1], progressionFirst, m_Skeleton->m_JointCount);
		auto finalSecond = _BlendSkeletonPoses(&stateData.second->m_SkeletonPoses[prevIndexSecond], &stateData.second->m_SkeletonPoses[prevIndexSecond + 1], progressionSecond, m_Skeleton->m_JointCount);

		finalPose = _BlendSkeletonPoses
		(
			&finalFirst,
			&finalSecond,
			stateData.weight,
			m_Skeleton->m_JointCount
		);
	}
	return std::move(finalPose);
}

Animation::SkeletonPose Animation::AnimationPlayer::UpdateBlendspace2D(SM::BlendSpace2D::Current2DStateData stateData)
{
	if (!stateData.firstTop)
		return Animation::SkeletonPose();

	float speedScale = 1.0f;
	std::pair<uint16_t, float> indexAndProgressionBottom{};
	if (stateData.firstBottom)
	{
		speedScale = getSpeedScale(stateData.firstBottom->m_FrameCount, stateData.firstTop->m_FrameCount, stateData.weightY);		
		indexAndProgressionBottom = _ComputeIndexAndProgressionNormalized(m_currentFrameDeltaTime * speedScale, &m_CurrentNormalizedTime, stateData.firstBottom->m_FrameCount);
	}

	///calc the actual frame index and progression towards the next frame
	auto indexAndProgressionTop = _ComputeIndexAndProgressionNormalized(0.0 /*wont use delta time second time it's called*/, &m_CurrentNormalizedTime, stateData.firstTop->m_FrameCount);

	auto prevIndexTop = indexAndProgressionTop.first;
	auto progressionTop = indexAndProgressionTop.second;
	auto prevIndexBottom = indexAndProgressionBottom.first;
	auto progressionBottom = indexAndProgressionBottom.second;
	SkeletonPose finalPose;


	if (!stateData.secondTop)
	{
		finalPose = _BlendSkeletonPoses(&stateData.firstTop->m_SkeletonPoses[prevIndexTop], &stateData.firstTop->m_SkeletonPoses[prevIndexTop + 1], progressionTop, m_Skeleton->m_JointCount);
		_ComputeSkinningMatrices(&finalPose);
	}
	else if (!stateData.firstBottom)
	{
		auto firstPoseTop = _BlendSkeletonPoses(&stateData.firstTop->m_SkeletonPoses[prevIndexTop], &stateData.firstTop->m_SkeletonPoses[prevIndexTop + 1], progressionTop, m_Skeleton->m_JointCount);
		auto secondPoseTop = _BlendSkeletonPoses(&stateData.secondTop->m_SkeletonPoses[prevIndexTop], &stateData.secondTop->m_SkeletonPoses[prevIndexTop + 1], progressionTop, m_Skeleton->m_JointCount);
		finalPose = _BlendSkeletonPoses(&firstPoseTop, &secondPoseTop, stateData.weightTop, m_Skeleton->m_JointCount);
	}
	else
	{
		auto firstPoseTop = _BlendSkeletonPoses(&stateData.firstTop->m_SkeletonPoses[prevIndexTop], &stateData.firstTop->m_SkeletonPoses[prevIndexTop + 1], progressionTop, m_Skeleton->m_JointCount);
		auto secondPoseTop = _BlendSkeletonPoses(&stateData.secondTop->m_SkeletonPoses[prevIndexTop], &stateData.secondTop->m_SkeletonPoses[prevIndexTop + 1], progressionTop, m_Skeleton->m_JointCount);
		auto firstPoseBottom = _BlendSkeletonPoses(&stateData.firstBottom->m_SkeletonPoses[prevIndexBottom], &stateData.firstBottom->m_SkeletonPoses[prevIndexBottom + 1], progressionBottom, m_Skeleton->m_JointCount);
		auto secondPoseBottom = _BlendSkeletonPoses(&stateData.secondBottom->m_SkeletonPoses[prevIndexBottom], &stateData.secondBottom->m_SkeletonPoses[prevIndexBottom + 1], progressionBottom, m_Skeleton->m_JointCount);
		finalPose = _BlendSkeletonPoses2D
		(
			{ &firstPoseTop, &secondPoseTop, stateData.weightTop },
			{ &firstPoseBottom, &secondPoseBottom, stateData.weightBottom },
			stateData.weightY,
			m_Skeleton->m_JointCount
		);
	}

	return std::move(finalPose);
}

void Animation::AnimationPlayer::SetPlayingClip(AnimationClip* clip, bool isLooping /*= true*/, bool keepCurrentNormalizedTime /*= false*/)
{
	m_CurrentClip = clip;
	m_CurrentTime = keepCurrentNormalizedTime
		? m_CurrentTime
		: 0.0;
}

void Animation::AnimationPlayer::SetSkeleton(SharedSkeleton skeleton)
{
	m_Skeleton = skeleton;

	// make sure the matrix vectors can accommodate each joint matrix 
	m_GlobalMatrices.resize(skeleton->m_JointCount);
	m_SkinningMatrices.resize(skeleton->m_JointCount);
}

void Animation::AnimationPlayer::Pause()
{
	m_IsPlaying = false;
}

void Animation::AnimationPlayer::Play()
{
	m_IsPlaying = true;
}

std::unique_ptr<LayerMachine>& Animation::AnimationPlayer::GetLayerMachine()
{
	return m_LayerMachine;
}

std::unique_ptr<SM::AnimationStateMachine>& Animation::AnimationPlayer::GetStateMachine()
{
	return m_StateMachine;
}

std::unique_ptr<SM::AnimationStateMachine>& Animation::AnimationPlayer::GetLayerStateMachine()
{
	return m_LayerStateMachine;
}

std::unique_ptr<SM::AnimationStateMachine>& Animation::AnimationPlayer::InitStateMachine(size_t numStates)
{
	m_StateMachine = std::make_unique<SM::AnimationStateMachine>(numStates);
	m_Visitor = std::make_unique<SM::StateVisitor>(this);
	return m_StateMachine;
}

std::unique_ptr<SM::AnimationStateMachine>& Animation::AnimationPlayer::InitLayerStateMachine(size_t numStates)
{
	m_LayerStateMachine = std::make_unique<SM::AnimationStateMachine>(numStates);
	m_LayerVisitor = std::make_unique<SM::LayerVisitor>(this);
	return m_LayerStateMachine;
}

// Returns a reference to the skinning matrix vector
const std::vector<DirectX::XMFLOAT4X4A>& Animation::AnimationPlayer::GetSkinningMatrices()
{
	return m_SkinningMatrices;
}

float Animation::AnimationPlayer::GetCachedDeltaTime()
{
	return m_currentFrameDeltaTime;
}

std::vector<DirectX::XMMATRIX> Animation::AnimationPlayer::_CombinePoses(std::vector<Animation::SkeletonPose>&& poses)
{
	{
		
	}
	return std::vector<DirectX::XMMATRIX>{};
}

std::unique_ptr<LayerMachine>& Animation::AnimationPlayer::InitLayerMachine(Animation::Skeleton* skeleton)
{
	m_LayerMachine = std::make_unique<LayerMachine>(skeleton);
	return m_LayerMachine;
}
DirectX::XMMATRIX Animation::AnimationPlayer::_CreateMatrixFromSRT(const SRT& srt)
{
	using namespace DirectX;

	XMFLOAT4A fScale = srt.m_Scale;
	XMFLOAT4A fRotation = srt.m_RotationQuaternion;
	XMFLOAT4A fTranslation = srt.m_Translation;				   

	auto translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat4A(&fTranslation));
	auto rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4A(&fRotation));
	auto scaleMatrix = XMMatrixScalingFromVector(XMLoadFloat4A(&fScale));
	
	return XMMatrixAffineTransformation(XMLoadFloat4A(&fScale), { 0.0, 0.0, 0.0, 1.0 }, XMLoadFloat4A(&fRotation), XMLoadFloat4A(&fTranslation));
}

DirectX::XMMATRIX Animation::_createMatrixFromSRT(const ImporterLibrary::DecomposedTransform& transform)
{
	using namespace DirectX;

	XMFLOAT4A fScale = {transform.scale.x, transform.scale.y, transform.scale.z, transform.scale.w};
	XMFLOAT4A fRotation = { transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w };
	XMFLOAT4A fTranslation = { transform.translation.x, transform.translation.y, transform.translation.z, transform.translation.w };

	auto t = XMMatrixTranslationFromVector(XMLoadFloat4A(&fTranslation));
	auto r = XMMatrixRotationQuaternion(XMLoadFloat4A(&fRotation));
	auto s = XMMatrixScalingFromVector(XMLoadFloat4A(&fTranslation));

	return XMMatrixAffineTransformation(XMLoadFloat4A(&fScale), { 0.0f, 0.0f, 0.0f, 1.0f }, XMLoadFloat4A(&fRotation), XMLoadFloat4A(&fTranslation));
}

Animation::SharedAnimation Animation::LoadAndCreateAnimation(std::string file, std::shared_ptr<Skeleton> skeleton)
{
	ImporterLibrary::CustomFileLoader loader;
	auto importedAnimation = loader.readAnimationFile(file, skeleton->m_JointCount);

	return std::make_shared<Animation::AnimationClip>(importedAnimation, skeleton);
}

std::shared_ptr<Animation::Skeleton> Animation::LoadAndCreateSkeleton(std::string file)
{
	ImporterLibrary::CustomFileLoader loader;
	auto importedSkeleton = loader.readSkeletonFile(file);
	return std::make_shared<Animation::Skeleton>(importedSkeleton);
}

Animation::JointPose Animation::getAdditivePose(JointPose targetPose, JointPose differencePose)
{
	using namespace DirectX;
	using Animation::AnimationPlayer;

	XMMATRIX targetPoseMatrix = AnimationPlayer::_CreateMatrixFromSRT(targetPose.m_Transformation);
	XMMATRIX differencePoseMatrix = AnimationPlayer::_CreateMatrixFromSRT(differencePose.m_Transformation);
	XMMATRIX additivePoseMatrix = XMMatrixMultiply(targetPoseMatrix, differencePoseMatrix);


	SRT additivePose = {};
	XMVECTOR s, r, t;
	XMMatrixDecompose(&s, &r, &t, additivePoseMatrix);
	XMStoreFloat4A(&additivePose.m_Scale, s);
	XMStoreFloat4A(&additivePose.m_RotationQuaternion, r);
	XMStoreFloat4A(&additivePose.m_Translation, t);

	return JointPose(additivePose);
}


Animation::JointPose Animation::AnimationPlayer::_GetAdditivePose(Animation::JointPose targetPose, DirectX::XMMATRIX differencePose)
{
	using namespace DirectX;

	XMMATRIX targetPoseMatrix = _CreateMatrixFromSRT(targetPose.m_Transformation);
	XMMATRIX differencePoseMatrix = differencePose;
	XMMATRIX additivePoseMatrix = XMMatrixMultiply(differencePoseMatrix, targetPoseMatrix);


	Animation::SRT additivePose = {};
	XMVECTOR s, r, t;
	XMMatrixDecompose(&s, &r, &t, additivePoseMatrix);
	XMStoreFloat4A(&additivePose.m_Scale, s);
	XMStoreFloat4A(&additivePose.m_RotationQuaternion, r);
	XMStoreFloat4A(&additivePose.m_Translation, t);

	return Animation::JointPose(additivePose);
}

Animation::JointPose Animation::AnimationPlayer::_BlendJointPoses(JointPose* firstPose, JointPose* secondPose, float blendFactor)
{
	using namespace DirectX;

	XMVECTOR firstRotation = XMLoadFloat4A(&firstPose->m_Transformation.m_RotationQuaternion);
	XMVECTOR secondRotation = XMLoadFloat4A(&secondPose->m_Transformation.m_RotationQuaternion);
	XMVECTOR firstTranslation = XMLoadFloat4A(&firstPose->m_Transformation.m_Translation);
	XMVECTOR secondTranslation = XMLoadFloat4A(&secondPose->m_Transformation.m_Translation);
	XMVECTOR firstScale = XMLoadFloat4A(&firstPose->m_Transformation.m_Scale);
	XMVECTOR secondScale = XMLoadFloat4A(&secondPose->m_Transformation.m_Scale);

	DirectX::XMVECTOR newRotation = XMQuaternionSlerp(firstRotation, secondRotation, blendFactor);
	DirectX::XMVECTOR newTranslation = XMVectorLerp(firstTranslation, secondTranslation, blendFactor);
	DirectX::XMVECTOR newScale = XMVectorLerp(firstScale, secondScale, blendFactor);

	SRT srt = {};
	XMStoreFloat4A(&srt.m_RotationQuaternion, newRotation);
	XMStoreFloat4A(&srt.m_Scale, newScale);
	XMStoreFloat4A(&srt.m_Translation, newTranslation);

	return JointPose(srt);
}

Animation::SkeletonPose Animation::AnimationPlayer::_BlendSkeletonPoses(SkeletonPose* firstPose, SkeletonPose* secondPose, float blendFactor, size_t jointCount)
{
	using Animation::SkeletonPose;
	SkeletonPose pose(jointCount);

	//LERP each joint pose
	for (int jointPose = 0; jointPose < jointCount; jointPose++)
		pose.m_JointPoses[jointPose] = _BlendJointPoses(&firstPose->m_JointPoses[jointPose], &secondPose->m_JointPoses[jointPose], blendFactor);
	
	//Return the blended pose
	return std::move(pose);
}

//Blend first and second pair, then blend the results
Animation::SkeletonPose Animation::AnimationPlayer::_BlendSkeletonPoses2D(SkeletonPosePair firstPair, SkeletonPosePair secondPair, float pairsBlendFactor, size_t jointCount)
{
	auto firstBlendedPose  = _BlendSkeletonPoses(firstPair.first, firstPair.second, firstPair.blendFactor, jointCount);
	auto secondBlendedPose = _BlendSkeletonPoses(secondPair.first, secondPair.second, secondPair.blendFactor, jointCount);
	auto finalBlendedPose  = _BlendSkeletonPoses(&firstBlendedPose, &secondBlendedPose, pairsBlendFactor, jointCount);
	return std::move(finalBlendedPose);
}

void Animation::AnimationPlayer::_ComputeSkinningMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight)
{
	using namespace DirectX;


	_ComputeModelMatrices(firstPose, secondPose, weight);

	for (int i = 0; i < m_Skeleton->m_JointCount; i++)
	{
		const XMFLOAT4X4A& global = m_GlobalMatrices[i];
		const XMFLOAT4X4A& inverseBindPose = m_Skeleton->m_Joints[i].m_InverseBindPose;

		XMMATRIX skinningMatrix = XMMatrixMultiply(XMLoadFloat4x4A(&inverseBindPose), XMLoadFloat4x4A(&global)); // #matrixmultiplication

		DirectX::XMStoreFloat4x4A(&m_SkinningMatrices[i], skinningMatrix);
	}
}

void Animation::AnimationPlayer::_ComputeSkinningMatrices(SkeletonPose * pose)
{
	if (m_LayerMachine)
		m_LayerMachine->UpdatePoseWithLayers(*pose, m_currentFrameDeltaTime);

	using namespace DirectX;
	_ComputeModelMatrices(pose);

	for (int i = 0; i < m_Skeleton->m_JointCount; i++)
	{
		const XMFLOAT4X4A& global = m_GlobalMatrices[i];
		const XMFLOAT4X4A& inverseBindPose = m_Skeleton->m_Joints[i].m_InverseBindPose;

		XMMATRIX skinningMatrix = XMMatrixMultiply(XMLoadFloat4x4A(&inverseBindPose), XMLoadFloat4x4A(&global)); // #matrixmultiplication

		DirectX::XMStoreFloat4x4A(&m_SkinningMatrices[i], skinningMatrix);
	}
}

// #modelmatrix 
void Animation::AnimationPlayer::_ComputeModelMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight)
{
	using namespace DirectX;
	using Animation::AnimationPlayer;

	//Check if we have layers
	std::optional<Animation::SkeletonPose> layerPose = std::nullopt;
	if (m_LayerStateMachine)
		layerPose = m_LayerStateMachine->GetCurrentState().recieveStateVisitor(*m_LayerVisitor);


	auto rootJointPose = _BlendJointPoses(&firstPose->m_JointPoses[0], &secondPose->m_JointPoses[0], weight);
	if (layerPose.has_value())
		rootJointPose = getAdditivePose(rootJointPose, layerPose.value().m_JointPoses[0]);

	DirectX::XMStoreFloat4x4A(&m_GlobalMatrices[0], AnimationPlayer::_CreateMatrixFromSRT(rootJointPose.m_Transformation));
	



	for (int i = 1; i < m_Skeleton->m_JointCount; i++) //start at second joint (first is root, already processed)
	{
		const int16_t parentIndex = m_Skeleton->m_Joints[i].m_ParentIndex;
		const XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&m_GlobalMatrices[parentIndex]);
		auto jointPose = _BlendJointPoses(&firstPose->m_JointPoses[i], &secondPose->m_JointPoses[i], weight);
		if (layerPose.has_value())
			jointPose = getAdditivePose(jointPose, layerPose.value().m_JointPoses[i]);
		DirectX::XMStoreFloat4x4A(&m_GlobalMatrices[i], XMMatrixMultiply(AnimationPlayer::_CreateMatrixFromSRT(jointPose.m_Transformation), parentGlobalMatrix)); // #matrixmultiplication
	}
}

void Animation::AnimationPlayer::_ComputeModelMatrices(SkeletonPose * pose)
{
	using namespace DirectX;
	using Animation::AnimationPlayer;

	//Check if we have layers
	std::optional<Animation::SkeletonPose> layerPose = std::nullopt;
	if (m_LayerStateMachine)
		layerPose = m_LayerStateMachine->GetCurrentState().recieveStateVisitor(*m_LayerVisitor);

	if (layerPose.has_value())
		XMStoreFloat4x4A(&m_GlobalMatrices[0], AnimationPlayer::_CreateMatrixFromSRT(getAdditivePose(pose->m_JointPoses[0].m_Transformation, layerPose.value().m_JointPoses[0].m_Transformation).m_Transformation));
	else
		XMStoreFloat4x4A(&m_GlobalMatrices[0], AnimationPlayer::_CreateMatrixFromSRT(pose->m_JointPoses[0].m_Transformation));

	for (int i = 1; i < m_Skeleton->m_JointCount; i++)
	{
		const int16_t parentIndex = m_Skeleton->m_Joints[i].m_ParentIndex;
		const XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&m_GlobalMatrices[parentIndex]);

		if (layerPose.has_value())
			DirectX::XMStoreFloat4x4A(&m_GlobalMatrices[i], XMMatrixMultiply(AnimationPlayer::_CreateMatrixFromSRT(getAdditivePose(pose->m_JointPoses[i].m_Transformation, layerPose.value().m_JointPoses[i]).m_Transformation), parentGlobalMatrix)); // #matrixmultiplication
		else
			XMStoreFloat4x4A(&m_GlobalMatrices[i], XMMatrixMultiply(AnimationPlayer::_CreateMatrixFromSRT(pose->m_JointPoses[i].m_Transformation), parentGlobalMatrix));
	}
}

Animation::SRT Animation::ConvertTransformToSRT(ImporterLibrary::Transform transform)
{
	using namespace DirectX;
	SRT srt = {};
	float pitch = DirectX::XMConvertToRadians(transform.transform_rotation[0]);
	float yaw = DirectX::XMConvertToRadians(transform.transform_rotation[1]);
	float roll = DirectX::XMConvertToRadians(transform.transform_rotation[2]);
	XMStoreFloat4A(&srt.m_RotationQuaternion, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));

	srt.m_RotationQuaternion.x *= -1;
	srt.m_RotationQuaternion.y *= -1;

	srt.m_Translation = { transform.transform_position[0], transform.transform_position[1], transform.transform_position[2], 1.0f };
	srt.m_Scale = { 1.0, 1.0, 1.0, 1.0f };
	return srt;
}

std::pair<uint16_t, float> Animation::AnimationPlayer::_ComputeIndexAndProgression(float deltaTime, float currentTime, uint16_t frameCount)
{
	currentTime += deltaTime;
	frameCount -= 1;

	float properTime = std::fmod(currentTime, frameCount / 24.0);

	///calc the actual frame index and progression towards the next frame
	float actualTime = properTime / (1.0 / 24.0);
	int prevIndex = (int)(actualTime);
	float progression = (actualTime) - (float)prevIndex;

	//return values
	return std::make_pair(static_cast<uint16_t>(prevIndex), progression);
}

std::pair<uint16_t, float> Animation::AnimationPlayer::_ComputeIndexAndProgression(float deltaTime, float* currentTime, uint16_t frameCount)
{
	*currentTime += deltaTime;
	frameCount -= 1;

	float properTime = std::fmod(*currentTime, frameCount / 24.0);
	*currentTime = properTime;
	///calc the actual frame index and progression towards the next frame
	float actualTime = properTime / (1.0 / 24.0);
	int prevIndex = (int)(actualTime);
	float progression = (actualTime)-(float)prevIndex;

	//return values
	return std::make_pair(static_cast<uint16_t>(prevIndex), progression);
}

std::optional<std::pair<uint16_t, float>> Animation::AnimationPlayer::_ComputeIndexAndProgressionOnce(float deltaTime, float* currentTime, uint16_t frameCount)
{
	*currentTime += deltaTime;
	frameCount -= 1;

	if (*currentTime >= (frameCount / 24.0f))
		return std::nullopt;

	///calc the actual frame index and progression towards the next frame
	float actualTime = *currentTime / (1.0 / 24.0);
	int prevIndex = (int)(actualTime);
	float progression = (actualTime)-(float)prevIndex;

	//return values
	return std::make_pair(static_cast<uint16_t>(prevIndex), progression);
}

std::pair<uint16_t, float> Animation::AnimationPlayer::_ComputeIndexAndProgressionNormalized(float deltaTime, float* currentTime, uint16_t frameCount)
{
	if (!m_TimeAlreadyUpdatedThisFrame)
	{
		deltaTime /= (frameCount / ANIMATION_FRAMERATE);
		*currentTime += deltaTime;
		m_TimeAlreadyUpdatedThisFrame = true;
	}

	frameCount -= 1;

	float properTime = std::fmod(*currentTime, 1.0f);
	*currentTime = properTime;
	///calc the actual frame index and progression towards the next frame
	float prevIndexFloat = (properTime * frameCount) ;
	int prevIndexInt = (int)prevIndexFloat;
	float lol = properTime * frameCount;
	float progression = prevIndexFloat - (float)prevIndexInt;


	return std::move(std::make_pair(static_cast<uint16_t>(prevIndexInt), progression));
}

void Animation::AnimationPlayer::UpdateLooping(Animation::AnimationClip* clip)
{
	if (clip != m_CurrentClip)
		this->SetPlayingClip(clip, true, false);

	{
		/// increase local time
		//done in _computeIndexAndProgression()

		///calc the actual frame index and progression towards the next frame
		auto indexAndProgression = _ComputeIndexAndProgression(m_currentFrameDeltaTime, &m_CurrentTime, m_CurrentClip->m_FrameCount);
		auto prevIndex = indexAndProgression.first;
		auto progression = indexAndProgression.second;

		///if we exceeded clips time, set back to 0 ish if we are looping, or stop if we aren't
		if (prevIndex >= m_CurrentClip->m_FrameCount - 1) /// -1 because last frame is only used to interpolate towards
		{
			//assert(1 == 0);
			if (m_IsLooping)
			{
				m_CurrentTime = 0.0 + progression;

				prevIndex = std::floorf(m_CurrentClip->m_Framerate / 2 * m_CurrentTime);
				progression = std::fmod(m_CurrentTime, 1.0 / 24.0);
			}
			else
			{
				m_IsPlaying = false;
			}
		}

		/// compute skinning matrices
		if (m_IsPlaying)
			_ComputeSkinningMatrices(&m_CurrentClip->m_SkeletonPoses[prevIndex], &m_CurrentClip->m_SkeletonPoses[prevIndex + 1], progression);
	}
}

void Animation::AnimationPlayer::UpdateOnce(Animation::AnimationClip* clip)
{
	if (clip != m_CurrentClip)
		this->SetPlayingClip(clip, false, false);

	if (m_IsPlaying)
	{
		/// increase local time
		//done in _computeIndexAndProgression()

		///calc the actual frame index and progression towards the next frame
		auto indexAndProgression = _ComputeIndexAndProgressionOnce(m_currentFrameDeltaTime, &m_CurrentTime, m_CurrentClip->m_FrameCount);

		uint16_t prevIndex = 0;
		float progression = 0.0f;
		if (indexAndProgression.has_value())
		{
			prevIndex = indexAndProgression.value().first;
			progression = indexAndProgression.value().second;
		}
		else
		{
			m_IsPlaying = false;
			prevIndex = m_CurrentClip->m_FrameCount - 1;
			_ComputeSkinningMatrices(&m_CurrentClip->m_SkeletonPoses[prevIndex]);
			return;
		}
		///if we exceeded clips time, set back to 0 ish if we are looping, or stop if we aren't
		if (prevIndex >= m_CurrentClip->m_FrameCount - 1) /// -1 because last frame is only used to interpolate towards
		{
			assert(1 == 0);
			if (m_IsLooping)
			{
				m_CurrentTime = 0.0 + progression;

				prevIndex = std::floorf(m_CurrentClip->m_Framerate / 2 * m_CurrentTime);
				progression = std::fmod(m_CurrentTime, 1.0 / 24.0);
			}
			else
			{
				m_IsPlaying = false;
			}
		}

		/// compute skinning matrices
		if (m_IsPlaying)
			_ComputeSkinningMatrices(&m_CurrentClip->m_SkeletonPoses[prevIndex], &m_CurrentClip->m_SkeletonPoses[prevIndex + 1], progression);
	}
}

Animation::SharedAnimation Animation::ConvertToAnimationClip(ImporterLibrary::AnimationFromFile* animation, uint8_t jointCount)
{
	uint32_t keyCount = animation->nr_of_keyframes;

	Animation::SharedAnimation clipToReturn = std::make_shared<Animation::AnimationClip>();
	clipToReturn->m_FrameCount = static_cast<uint16_t>(keyCount);
	clipToReturn->m_SkeletonPoses = std::make_unique<SkeletonPose[]>(clipToReturn->m_FrameCount);

	//Init joint poses for skeleton poses
	std::for_each(clipToReturn->m_SkeletonPoses.get(), clipToReturn->m_SkeletonPoses.get() + clipToReturn->m_FrameCount,
		[&](auto& e) { e.m_JointPoses = std::make_unique<JointPose[]>(jointCount); });

	for (int j = 0; j < jointCount; j++)
	{
		//for each key
		for (unsigned int k = 0; k < keyCount; k++)
		{
			Animation::SRT trans = ConvertTransformToSRT(animation->keyframe_transformations[j * animation->nr_of_keyframes + k]);
			clipToReturn->m_SkeletonPoses[k].m_JointPoses[j].m_Transformation = trans;
		}
	}
	clipToReturn->m_Framerate = 24;
	return clipToReturn;
}

void Animation::SetInverseBindPoses(Animation::Skeleton* mainSkeleton, const ImporterLibrary::Skeleton* importedSkeleton)
{
	using namespace DirectX;

	std::vector<XMFLOAT4X4A> vec;
	std::vector <ImporterLibrary::Transform > vec2;
	DirectX::XMStoreFloat4x4A(&mainSkeleton->m_Joints[0].m_InverseBindPose, AnimationPlayer::_CreateMatrixFromSRT(importedSkeleton->joints[0].jointInverseBindPoseTransform));

	for (int i = 1; i < mainSkeleton->m_JointCount; i++)
	{
		const int16_t parentIndex = mainSkeleton->m_Joints[i].m_ParentIndex;

		DirectX::XMStoreFloat4x4A
		(&mainSkeleton->m_Joints[i].m_InverseBindPose, AnimationPlayer::_CreateMatrixFromSRT(importedSkeleton->joints[i].jointInverseBindPoseTransform));
	}
}



void Animation::AnimationCBuffer::UpdateBuffer(PVOID64 data, size_t dataSize)
{
	DX::g_deviceContext->Map(m_AnimationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_dataPtr);
	memcpy(m_dataPtr.pData, data, dataSize);
	DX::g_deviceContext->Unmap(m_AnimationBuffer, 0);
}

void Animation::AnimationCBuffer::SetToShader()
{
	DX::g_deviceContext->VSSetConstantBuffers(4, 1, &m_AnimationBuffer);
}

Animation::SRT::SRT(const ImporterLibrary::Transform& transform)
{
	using namespace DirectX;
	
	float pitch = DirectX::XMConvertToRadians(transform.transform_rotation[0]);
	float yaw = DirectX::XMConvertToRadians(transform.transform_rotation[1]);
	float roll = DirectX::XMConvertToRadians(transform.transform_rotation[2]);
	XMStoreFloat4A(&m_RotationQuaternion, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));

	m_RotationQuaternion.x *= -1;
	m_RotationQuaternion.y *= -1;

	m_Translation = { transform.transform_position[0], transform.transform_position[1], transform.transform_position[2], 1.0f };
	m_Scale = { 1.0, 1.0, 1.0, 1.0f };
}

Animation::SRT::SRT(const ImporterLibrary::DecomposedTransform& transform)
{
	m_RotationQuaternion.x = transform.rotation.x;
	m_RotationQuaternion.y = transform.rotation.y;
	m_RotationQuaternion.z = transform.rotation.z;
	m_RotationQuaternion.w = transform.rotation.w;

	m_Translation.x = transform.translation.x;
	m_Translation.y = transform.translation.y;
	m_Translation.z = transform.translation.z;
	m_Translation.w = transform.translation.w;

	m_Scale.x = transform.scale.x;
	m_Scale.y = transform.scale.y;
	m_Scale.z = transform.scale.z;
	m_Scale.w = transform.scale.w;
}

bool Animation::SRT::operator==(const SRT& other)
{
	return (m_RotationQuaternion.x == other.m_RotationQuaternion.x
		&& m_RotationQuaternion.y == other.m_RotationQuaternion.y
		&& m_RotationQuaternion.z == other.m_RotationQuaternion.z
		&& m_RotationQuaternion.w == other.m_RotationQuaternion.w
		&& m_Scale.x == other.m_Scale.x
		&& m_Scale.z == other.m_Scale.z
		&& m_Scale.y == other.m_Scale.y
		&& m_Scale.w == other.m_Scale.w
		&& m_Translation.x == other.m_Translation.x
		&& m_Translation.z == other.m_Translation.z
		&& m_Translation.y == other.m_Translation.y
		&& m_Translation.w == other.m_Translation.w
		);
}

Animation::Skeleton::Skeleton(const ImporterLibrary::Skeleton& skeleton)
{
	m_JointCount = static_cast<uint8_t>(skeleton.joints.size());
	m_Joints = std::make_unique<Animation::Joint[]>(m_JointCount);

	for (int i = 0; i < m_JointCount; i++)
	{
		m_Joints[i].m_ParentIndex = skeleton.joints[i].parentIndex;
	}
	m_Joints[0].m_ParentIndex = -1; // Root does not have a real parent index
	Animation::SetInverseBindPoses(this, &skeleton);
}

Animation::JointPose::JointPose(const SRT& srt)
{
	m_Transformation = srt;
}

Animation::AnimationClip::AnimationClip(const ImporterLibrary::AnimationFromFileStefan& animation, std::shared_ptr<Skeleton> skeleton)
{
	auto keyCount   = animation.nr_of_keyframes;
	m_Skeleton      = skeleton;
	m_Framerate     = 24;
	m_FrameCount    = static_cast<uint16_t>(keyCount);
	m_SkeletonPoses = std::make_unique<SkeletonPose[]>(m_FrameCount);

	//Init joint poses for skeleton poses
	for (int i = 0; i < m_FrameCount; i++)
	{
		m_SkeletonPoses[i].m_JointPoses = std::make_unique<JointPose[]>(m_Skeleton->m_JointCount);
	}

	
	for (uint32_t k = 0; k < keyCount; k++) //for each key
	{
		for (uint8_t j = 0; j < m_Skeleton->m_JointCount; j++) //and each joint
		{
			//init the transformation from the raw keyframe array
			auto srt = SRT(animation.keyframe_transformations[k * skeleton->m_JointCount + j]);
			m_SkeletonPoses[k].m_JointPoses[j].m_Transformation = srt;
		}
	}
}

Animation::AnimationClip::~AnimationClip()
{
}

#pragma region AnimationCBufferClass
Animation::AnimationCBuffer::AnimationCBuffer() 
{
}

Animation::AnimationCBuffer::~AnimationCBuffer()
{
	DX::SafeRelease(m_AnimationBuffer);
}

void Animation::AnimationCBuffer::SetAnimationCBuffer()
{
	D3D11_BUFFER_DESC AnimationBufferDesc;
	AnimationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	AnimationBufferDesc.ByteWidth = (sizeof(float) * 16 * MAXJOINT);
	AnimationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	AnimationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	AnimationBufferDesc.MiscFlags = 0;
	AnimationBufferDesc.StructureByteStride = 0;

	// check if the creation failed for any reason
	HRESULT hr = 0;
	hr = DX::g_device->CreateBuffer(&AnimationBufferDesc, nullptr, &m_AnimationBuffer);
	if (FAILED(hr))
	{
		// handle the error, could be fatal or a warning...
		exit(-1);
	}
}

void Animation::AnimationCBuffer::UpdateBuffer(AnimationBuffer *buffer)
{
	memcpy(m_AnimationValues.skinnedMatrix, &buffer->skinnedMatrix, (sizeof(float) * 16 * MAXJOINT));
	DX::g_deviceContext->Map(m_AnimationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_dataPtr);
	memcpy(m_dataPtr.pData, &m_AnimationValues, sizeof(AnimationBuffer));
	DX::g_deviceContext->Unmap(m_AnimationBuffer, 0);
}
#pragma endregion AnimationCBufferClass