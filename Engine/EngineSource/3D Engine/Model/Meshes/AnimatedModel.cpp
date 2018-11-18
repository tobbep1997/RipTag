#include "EnginePCH.h"
#include "AnimatedModel.h"

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

float constexpr getNewValueInNewRange(float x, float a, float b, float c, float d)
{
	return ((x - a) * ((d - c) / (b - a))) + c;
}

Animation::AnimatedModel::AnimatedModel()
{
	m_skinningMatrices.resize(128);
	m_globalMatrices.resize(128);
	DirectX::XMFLOAT4X4A identityMatrix = {};
	DirectX::XMStoreFloat4x4A(&identityMatrix, DirectX::XMMatrixIdentity());
	std::fill(m_skinningMatrices.begin(), m_skinningMatrices.end(), identityMatrix);
	std::fill(m_globalMatrices.begin(), m_globalMatrices.end(), identityMatrix);
}

Animation::AnimatedModel::~AnimatedModel()
{
}

// Computes the frame we're currently on and computes final skinning matrices for gpu skinning
void Animation::AnimatedModel::Update(float deltaTime)
{
	timeAlreadyUpdatedThisFrame = false;
	m_currentFrameDeltaTime = deltaTime;

	if (!m_isPlaying)
		return;

	if (m_StateMachine)
	{


		m_StateMachine->UpdateCurrentState();
		
		//Blendfactor used to blend between two states.
		float blendFactor = 1.0 - m_StateMachine->UpdateBlendFactor(deltaTime);


		auto finalPoseCurrent = m_StateMachine->GetCurrentState().recieveStateVisitor(*m_Visitor);
		auto pPreviousState = m_StateMachine->GetPreviousState();
		
		//Layer
		if (m_LayerStateMachine)
		{
			auto finalLayerPose = m_LayerStateMachine->GetCurrentState().recieveStateVisitor(*m_LayerVisitor);

			if (pPreviousState)
			{
				auto finalPosePrevious = pPreviousState->recieveStateVisitor(*m_Visitor);
				_computeSkinningMatrices(&finalPosePrevious.value(), &finalPoseCurrent.value(), blendFactor);
				return;
			}
			if (finalPoseCurrent.value().m_jointPoses)
				_computeSkinningMatrices(&finalPoseCurrent.value());

			return;
		}
		else //no layers
		{
			if (pPreviousState)
			{
				auto finalPosePrevious = pPreviousState->recieveStateVisitor(*m_Visitor);
				_computeSkinningMatrices(&finalPosePrevious.value(), &finalPoseCurrent.value(), blendFactor);
				return;
			}
			if (finalPoseCurrent.value().m_jointPoses)
				_computeSkinningMatrices(&finalPoseCurrent.value());
			return;
		}
	}

	if (m_targetClip)
	{
		//UpdateBlend(deltaTime);
	}
	else if ( m_combinedClip.secondClip)
	{
		UpdateCombined(deltaTime);
	}
	else
	{
		/// increase local time
		//done in _computeIndexAndProgression()

		///calc the actual frame index and progression towards the next frame
		auto indexAndProgression = _computeIndexAndProgression(deltaTime, &m_currentTime, m_currentClip->m_frameCount);
		auto prevIndex = indexAndProgression.first;
		auto progression = indexAndProgression.second;

		///if we exceeded clips time, set back to 0 ish if we are looping, or stop if we aren't
		if (prevIndex >= m_currentClip->m_frameCount -1) /// -1 because last frame is only used to interpolate towards
		{
			if (m_isLooping)
			{
				m_currentTime = 0.0 + progression;
	
				prevIndex = std::floorf(m_currentClip->m_framerate / 2* m_currentTime);
				progression = std::fmod(m_currentTime, 1.0 / 24.0);
			}
			else
			{
				m_isPlaying = false; 
			}
		}

		/// compute skinning matrices
		if (m_isPlaying)
			_computeSkinningMatrices(&m_currentClip->m_skeletonPoses[prevIndex], &m_currentClip->m_skeletonPoses[prevIndex + 1], progression);
	}
}

void Animation::AnimatedModel::UpdateBlend(float deltaTime)
{
	m_currentBlendTime += deltaTime;

	if (m_currentBlendTime > m_targetBlendTime) //null target clip if the blend is done
	{
		m_currentClip = m_targetClip;
		m_targetClip = nullptr;
		m_currentBlendTime = 0.0;
		m_targetBlendTime = 0.0;
		m_currentTime = m_targetClipCurrentTime;
		m_targetClipCurrentTime = 0.0;
	}

	if (m_targetClip)
	{
		///calc the actual frame index and progression towards the next frame for both clips
		auto indexAndProgression = _computeIndexAndProgression(deltaTime, &m_currentTime, m_currentClip->m_frameCount);
		uint16_t prevIndex = indexAndProgression.first;
		float progression = indexAndProgression.second;

		auto targetIndexAndProgression = _computeIndexAndProgression(deltaTime, &m_targetClipCurrentTime, m_targetClip->m_frameCount);
		uint16_t targetPrevIndex = targetIndexAndProgression.first;
		float targetProgression = targetIndexAndProgression.second;
		
		/// compute skinning matrices
		if (m_isPlaying)
		{
			_computeSkinningMatrices(
				&m_currentClip->m_skeletonPoses[prevIndex], &m_currentClip->m_skeletonPoses[prevIndex + 1], progression,
				&m_targetClip->m_skeletonPoses[targetPrevIndex], &m_targetClip->m_skeletonPoses[targetPrevIndex + 1], targetProgression);
		}
	}
}

float getSpeedScale(size_t firstFrameCount, size_t secondFrameCount, float weight)
{
	float scale = static_cast<float>(firstFrameCount) / static_cast<float>(secondFrameCount);
	scale += lerp(0.0, 1.0 - scale, weight);
	return scale;
}

Animation::SkeletonPose Animation::AnimatedModel::UpdateBlendspace1D(SM::BlendSpace1D::Current1DStateData stateData)
{
	if (!stateData.first)
		return Animation::SkeletonPose();

	float speedScale = 1.0f;
	std::pair<uint16_t, float> indexAndProgressionFirst{};
	std::pair<uint16_t, float> indexAndProgressionSecond{};
	if (stateData.second)
	{
		speedScale = getSpeedScale(stateData.second->m_frameCount, stateData.first->m_frameCount, stateData.weight);
		indexAndProgressionFirst = _computeIndexAndProgressionNormalized(m_currentFrameDeltaTime * speedScale, &m_currentNormalizedTime, stateData.second->m_frameCount);
	}
	indexAndProgressionSecond = _computeIndexAndProgressionNormalized(0.0 /*wont use delta time second time it's called*/, &m_currentNormalizedTime, stateData.first->m_frameCount);

	auto prevIndexFirst = indexAndProgressionFirst.first;
	auto progressionFirst= indexAndProgressionFirst.second;
	auto prevIndexSecond = indexAndProgressionSecond.first;
	auto progressionSecond = indexAndProgressionSecond.second;
	SkeletonPose finalPose;
	if (!stateData.second)
	{
		finalPose = _BlendSkeletonPoses(&stateData.first->m_skeletonPoses[prevIndexFirst], &stateData.first->m_skeletonPoses[prevIndexFirst + 1], progressionFirst, m_skeleton->m_jointCount);
	}
	else
	{
		auto finalFirst = _BlendSkeletonPoses(&stateData.first->m_skeletonPoses[prevIndexFirst], &stateData.first->m_skeletonPoses[prevIndexFirst + 1], progressionFirst, m_skeleton->m_jointCount);
		auto finalSecond = _BlendSkeletonPoses(&stateData.second->m_skeletonPoses[prevIndexSecond], &stateData.second->m_skeletonPoses[prevIndexSecond + 1], progressionSecond, m_skeleton->m_jointCount);

		finalPose = _BlendSkeletonPoses
		(
			&finalFirst,
			&finalSecond,
			stateData.weight,
			m_skeleton->m_jointCount
		);
	}
	return std::move(finalPose);
}

Animation::SkeletonPose Animation::AnimatedModel::UpdateBlendspace2D(SM::BlendSpace2D::Current2DStateData stateData)
{
	if (!stateData.firstTop)
		return Animation::SkeletonPose();

	float speedScale = 1.0f;
	std::pair<uint16_t, float> indexAndProgressionBottom{};
	if (stateData.firstBottom)
	{
		speedScale = getSpeedScale(stateData.firstBottom->m_frameCount, stateData.firstTop->m_frameCount, stateData.weightY);		
		indexAndProgressionBottom = _computeIndexAndProgressionNormalized(m_currentFrameDeltaTime * speedScale, &m_currentNormalizedTime, stateData.firstBottom->m_frameCount);
	}

	///calc the actual frame index and progression towards the next frame
	auto indexAndProgressionTop = _computeIndexAndProgressionNormalized(0.0 /*wont use delta time second time it's called*/, &m_currentNormalizedTime, stateData.firstTop->m_frameCount);

	auto prevIndexTop = indexAndProgressionTop.first;
	auto progressionTop = indexAndProgressionTop.second;
	auto prevIndexBottom = indexAndProgressionBottom.first;
	auto progressionBottom = indexAndProgressionBottom.second;
	SkeletonPose finalPose;


	if (!stateData.secondTop)
	{
		finalPose = _BlendSkeletonPoses(&stateData.firstTop->m_skeletonPoses[prevIndexTop], &stateData.firstTop->m_skeletonPoses[prevIndexTop + 1], progressionTop, m_skeleton->m_jointCount);
		_computeSkinningMatrices(&finalPose);
	}
	else if (!stateData.firstBottom)
	{
		auto firstPoseTop = _BlendSkeletonPoses(&stateData.firstTop->m_skeletonPoses[prevIndexTop], &stateData.firstTop->m_skeletonPoses[prevIndexTop + 1], progressionTop, m_skeleton->m_jointCount);
		auto secondPoseTop = _BlendSkeletonPoses(&stateData.secondTop->m_skeletonPoses[prevIndexTop], &stateData.secondTop->m_skeletonPoses[prevIndexTop + 1], progressionTop, m_skeleton->m_jointCount);
		finalPose = _BlendSkeletonPoses(&firstPoseTop, &secondPoseTop, stateData.weightTop, m_skeleton->m_jointCount);
	}
	else
	{
		auto firstPoseTop = _BlendSkeletonPoses(&stateData.firstTop->m_skeletonPoses[prevIndexTop], &stateData.firstTop->m_skeletonPoses[prevIndexTop + 1], progressionTop, m_skeleton->m_jointCount);
		auto secondPoseTop = _BlendSkeletonPoses(&stateData.secondTop->m_skeletonPoses[prevIndexTop], &stateData.secondTop->m_skeletonPoses[prevIndexTop + 1], progressionTop, m_skeleton->m_jointCount);
		auto firstPoseBottom = _BlendSkeletonPoses(&stateData.firstBottom->m_skeletonPoses[prevIndexBottom], &stateData.firstBottom->m_skeletonPoses[prevIndexBottom + 1], progressionBottom, m_skeleton->m_jointCount);
		auto secondPoseBottom = _BlendSkeletonPoses(&stateData.secondBottom->m_skeletonPoses[prevIndexBottom], &stateData.secondBottom->m_skeletonPoses[prevIndexBottom + 1], progressionBottom, m_skeleton->m_jointCount);
		finalPose = _BlendSkeletonPoses2D
		(
			{ &firstPoseTop, &secondPoseTop, stateData.weightTop },
			{ &firstPoseBottom, &secondPoseBottom, stateData.weightBottom },
			stateData.weightY,
			m_skeleton->m_jointCount
		);
	}

	return std::move(finalPose);
}

void Animation::AnimatedModel::SetPlayingClip(AnimationClip* clip, bool isLooping /*= true*/, bool keepCurrentNormalizedTime /*= false*/)
{
	m_currentClip = clip;
	m_currentTime = keepCurrentNormalizedTime
		? m_currentTime
		: 0.0;
}

void Animation::AnimatedModel::SetLayeredClip(AnimationClip* clip, float weight, UINT flags /*= BLEND_MATCH_NORMALIZED_TIME*/, bool isLooping /*= true*/)
{
	m_combinedClip.secondClip = clip;
	m_combinedClip.secondWeight = weight;

	if (flags & BLEND_MATCH_NORMALIZED_TIME)
	{
		float currentClipNormalizedTime = (m_currentTime / (1.0 / 24.0 * m_combinedClip.firstClip->m_frameCount));
		m_combinedClip.secondCurrentTime = currentClipNormalizedTime * (1.0 / 24.0 * clip->m_frameCount);
	}
	else if (flags & BLEND_FROM_START)
	{
		m_combinedClip.secondCurrentTime = 0.0;
	}
	if (flags & BLEND_MATCH_TIME)
	{
		m_combinedClip.secondCurrentTime = m_currentTime;
	}
}

void Animation::AnimatedModel::SetLayeredClipWeight(const float& weight)
{
	m_combinedClip.secondWeight = weight;
}

void Animation::AnimatedModel::SetTargetClip(AnimationClip* clip, UINT blendFlags /*= 0*/, float blendTime /*= 1.0f*/, bool isLooping /*= true*/)
{
	if (blendFlags & BLEND_MATCH_TIME)
	{
		m_targetClipCurrentTime = m_currentTime;
	}
	else if (blendFlags & BLEND_FROM_START)
	{
		m_targetClipCurrentTime = 0.0;
	}
	else if (blendFlags & BLEND_MATCH_NORMALIZED_TIME)
	{
		float currentClipNormalizedTime = (m_currentTime / (1.0 / 24.0 * m_currentClip->m_frameCount));
		m_targetClipCurrentTime = currentClipNormalizedTime * (1.0 / 24.0 * clip->m_frameCount);
	}

	m_targetClip = clip;
	m_targetBlendTime = blendTime;
}

void Animation::AnimatedModel::SetSkeleton(SharedSkeleton skeleton)
{
	m_skeleton = skeleton;

	// make sure the matrix vectors can accommodate each joint matrix 
	m_globalMatrices.resize(skeleton->m_jointCount);
	m_skinningMatrices.resize(skeleton->m_jointCount);
}

void Animation::AnimatedModel::SetScrubIndex(unsigned int index)
{
	m_scrubIndex = index;
}

void Animation::AnimatedModel::Pause()
{
	m_isPlaying = false;
}

void Animation::AnimatedModel::Play()
{
	m_isPlaying = true;
}

float Animation::AnimatedModel::GetCurrentTimeInClip()
{
	return m_currentTime;
}

int Animation::AnimatedModel::GetCurrentFrameIndex()
{
	return std::floorf(m_currentClip->m_framerate * m_currentTime);
}

std::unique_ptr<SM::AnimationStateMachine>& Animation::AnimatedModel::GetStateMachine()
{
	return m_StateMachine;
}

std::unique_ptr<SM::AnimationStateMachine>& Animation::AnimatedModel::GetLayerStateMachine()
{
	return m_LayerStateMachine;
}

std::unique_ptr<SM::AnimationStateMachine>& Animation::AnimatedModel::InitStateMachine(size_t numStates)
{
	m_StateMachine = std::make_unique<SM::AnimationStateMachine>(numStates);
	m_Visitor = std::make_unique<SM::StateVisitor>(this);
	return m_StateMachine;
}

std::unique_ptr<SM::AnimationStateMachine>& Animation::AnimatedModel::InitLayerStateMachine(size_t numStates)
{
	m_LayerStateMachine = std::make_unique<SM::AnimationStateMachine>(numStates);
	m_LayerVisitor = std::make_unique<SM::LayerVisitor>(this);
	return m_LayerStateMachine;
}

// Returns a reference to the skinning matrix vector
const std::vector<DirectX::XMFLOAT4X4A>& Animation::AnimatedModel::GetSkinningMatrices()
{
	return m_skinningMatrices;
}

float Animation::AnimatedModel::GetCachedDeltaTime()
{
	return m_currentFrameDeltaTime;
}

DirectX::XMMATRIX Animation::_createMatrixFromSRT(const SRT& srt)
{
	using namespace DirectX;

	XMFLOAT4A fScale = srt.m_scale;
	XMFLOAT4A fRotation = srt.m_rotationQuaternion;
	XMFLOAT4A fTranslation = srt.m_translation;				   

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
	auto importedAnimation = loader.readAnimationFile(file, skeleton->m_jointCount);

	return std::make_shared<Animation::AnimationClip>(importedAnimation, skeleton);
}

std::shared_ptr<Animation::Skeleton> Animation::LoadAndCreateSkeleton(std::string file)
{
	ImporterLibrary::CustomFileLoader loader;
	auto importedSkeleton = loader.readSkeletonFile(file);
	return std::make_shared<Animation::Skeleton>(importedSkeleton);
}

Animation::JointPose Animation::getDifferencePose(JointPose sourcePose, JointPose referencePose)
{
	using namespace DirectX;

	XMMATRIX sourceMatrix    = _createMatrixFromSRT(sourcePose.m_transformation);
	XMMATRIX referenceMatrix = _createMatrixFromSRT(referencePose.m_transformation);
	XMMATRIX referenceMatrixInverse = XMMatrixInverse(nullptr, referenceMatrix);
	XMMATRIX differenceMatrix = XMMatrixMultiply(sourceMatrix, referenceMatrixInverse);

	SRT differencePose = {};
	XMVECTOR s, r, t;
	XMMatrixDecompose(&s, &r, &t, differenceMatrix);
	XMStoreFloat4A(&differencePose.m_scale, s);
	XMStoreFloat4A(&differencePose.m_rotationQuaternion, r);
	XMStoreFloat4A(&differencePose.m_translation, t);

	return JointPose(differencePose);
}

Animation::JointPose Animation::getAdditivePose(JointPose targetPose, JointPose differencePose)
{
	using namespace DirectX;

	XMMATRIX targetPoseMatrix = _createMatrixFromSRT(targetPose.m_transformation);
	XMMATRIX differencePoseMatrix = _createMatrixFromSRT(differencePose.m_transformation);
	XMMATRIX additivePoseMatrix = XMMatrixMultiply(targetPoseMatrix, differencePoseMatrix);


	SRT additivePose = {};
	XMVECTOR s, r, t;
	XMMatrixDecompose(&s, &r, &t, additivePoseMatrix);
	XMStoreFloat4A(&additivePose.m_scale, s);
	XMStoreFloat4A(&additivePose.m_rotationQuaternion, r);
	XMStoreFloat4A(&additivePose.m_translation, t);

	return JointPose(additivePose);
}

// Returns null if the clips are not compatible
Animation::AnimationClip* Animation::computeDifferenceClip(Animation::AnimationClip* sourceClip, Animation::AnimationClip* referenceClip)
{
#pragma region using stuff
	using namespace Animation;
	using std::make_unique;
#pragma endregion 

	if (sourceClip->m_frameCount != referenceClip->m_frameCount
	   || sourceClip->m_skeleton != referenceClip->m_skeleton)
		return nullptr;

	AnimationClip* differenceClip = new AnimationClip();
	differenceClip->m_frameCount = sourceClip->m_frameCount;
	differenceClip->m_skeleton = sourceClip->m_skeleton;
	differenceClip->m_skeletonPoses = make_unique<SkeletonPose[]>(differenceClip->m_frameCount);

	//Go through each skeleton pose and set new difference pose for each joint
	for (int frame = 0; frame < differenceClip->m_frameCount; frame++) 
	{
		//Init joint pose array for this skeleton pose
		differenceClip->m_skeletonPoses[frame].m_jointPoses = make_unique<JointPose[]>(differenceClip->m_skeleton->m_jointCount);
		
		//Go through each joint and assign the difference pose
		for (int jointPose = 0; jointPose < differenceClip->m_skeleton->m_jointCount; jointPose++)
		{
			auto sourcePose = sourceClip->m_skeletonPoses[frame].m_jointPoses[jointPose];
			auto referencePose = referenceClip->m_skeletonPoses[frame].m_jointPoses[jointPose];
			auto differencePose = getDifferencePose(sourcePose, referencePose);

			differenceClip->m_skeletonPoses[frame].m_jointPoses[jointPose] = differencePose;
		}
	}

	return differenceClip;
}

bool Animation::bakeDifferenceClipOntoClip(Animation::AnimationClip* targetClip, Animation::AnimationClip* differenceClip)
{
#pragma region incompatibility check
	if (targetClip->m_frameCount != differenceClip->m_frameCount // #todo support different clip lengths
		|| targetClip->m_skeleton != differenceClip->m_skeleton)
		return false;
#pragma endregion

	for (int frame = 0; frame < targetClip->m_frameCount; frame++) //for each frame
	{
		for (int jointPose = 0; jointPose < targetClip->m_skeleton->m_jointCount; jointPose++) //for each joint in this frame
		{
			auto targetPose = targetClip->m_skeletonPoses[frame].m_jointPoses[jointPose];
			auto differencePose = differenceClip->m_skeletonPoses[frame].m_jointPoses[jointPose];

			//Concatenate the target pose and difference pose and store in target animation clip
			targetClip->m_skeletonPoses[frame].m_jointPoses[jointPose] = getAdditivePose(targetPose, differencePose);
		}
	}

	return true;
}

Animation::JointPose Animation::AnimatedModel::_BlendJointPoses(JointPose* firstPose, JointPose* secondPose, float blendFactor)
{
	return _interpolateJointPose(firstPose, secondPose, blendFactor);
}

Animation::JointPose getAdditivePose(Animation::JointPose targetPose, Animation::JointPose differencePose)
{
	using namespace DirectX;

	XMMATRIX targetPoseMatrix = _createMatrixFromSRT(targetPose.m_transformation);
	XMMATRIX differencePoseMatrix = _createMatrixFromSRT(differencePose.m_transformation);
	XMMATRIX additivePoseMatrix = XMMatrixMultiply(differencePoseMatrix, targetPoseMatrix);


	Animation::SRT additivePose = {};
	XMVECTOR s, r, t;
	XMMatrixDecompose(&s, &r, &t, additivePoseMatrix);
	XMStoreFloat4A(&additivePose.m_scale, s);
	XMStoreFloat4A(&additivePose.m_rotationQuaternion, r);
	XMStoreFloat4A(&additivePose.m_translation, t);

	return Animation::JointPose(additivePose);
}

Animation::SkeletonPose Animation::AnimatedModel::_BlendSkeletonPoses(SkeletonPose* firstPose, SkeletonPose* secondPose, float blendFactor, size_t jointCount)
{
	using Animation::SkeletonPose;
	SkeletonPose pose(jointCount);

	//LERP each joint pose
	for (int jointPose = 0; jointPose < jointCount; jointPose++)
		pose.m_jointPoses[jointPose] = _BlendJointPoses(&firstPose->m_jointPoses[jointPose], &secondPose->m_jointPoses[jointPose], blendFactor);
	
	//Return the blended pose
	return std::move(pose);
}

//Blend first and second pair, then blend the results
Animation::SkeletonPose Animation::AnimatedModel::_BlendSkeletonPoses2D(SkeletonPosePair firstPair, SkeletonPosePair secondPair, float pairsBlendFactor, size_t jointCount)
{
	auto firstBlendedPose  = _BlendSkeletonPoses(firstPair.first, firstPair.second, firstPair.blendFactor, jointCount);
	auto secondBlendedPose = _BlendSkeletonPoses(secondPair.first, secondPair.second, secondPair.blendFactor, jointCount);
	auto finalBlendedPose  = _BlendSkeletonPoses(&firstBlendedPose, &secondBlendedPose, pairsBlendFactor, jointCount);
	return std::move(finalBlendedPose);
}

void Animation::AnimatedModel::_computeSkinningMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight)
{
	using namespace DirectX;


	_computeModelMatrices(firstPose, secondPose, weight);

	for (int i = 0; i < m_skeleton->m_jointCount; i++)
	{
		const XMFLOAT4X4A& global = m_globalMatrices[i];
		const XMFLOAT4X4A& inverseBindPose = m_skeleton->m_joints[i].m_inverseBindPose;

		XMMATRIX skinningMatrix = XMMatrixMultiply(XMLoadFloat4x4A(&inverseBindPose), XMLoadFloat4x4A(&global)); // #matrixmultiplication

		DirectX::XMStoreFloat4x4A(&m_skinningMatrices[i], skinningMatrix);
	}
}

void Animation::AnimatedModel::_computeSkinningMatrices(SkeletonPose * pose)
{
	using namespace DirectX;
	_computeModelMatrices(pose);

	for (int i = 0; i < m_skeleton->m_jointCount; i++)
	{
		const XMFLOAT4X4A& global = m_globalMatrices[i];
		const XMFLOAT4X4A& inverseBindPose = m_skeleton->m_joints[i].m_inverseBindPose;

		XMMATRIX skinningMatrix = XMMatrixMultiply(XMLoadFloat4x4A(&inverseBindPose), XMLoadFloat4x4A(&global)); // #matrixmultiplication

		DirectX::XMStoreFloat4x4A(&m_skinningMatrices[i], skinningMatrix);
	}
}

// #clipblend
void Animation::AnimatedModel::_computeSkinningMatrices(SkeletonPose* firstPose1, SkeletonPose* secondPose1, float weight1, SkeletonPose* firstPose2, SkeletonPose* secondPose2, float weight2)
{
	using namespace DirectX;

	_computeModelMatrices(firstPose1, secondPose1, weight1, firstPose2, secondPose2, weight2);

	for (int i = 0; i < m_skeleton->m_jointCount; i++)
	{
		const XMFLOAT4X4A& global = m_globalMatrices[i];
		const XMFLOAT4X4A& inverseBindPose = m_skeleton->m_joints[i].m_inverseBindPose;

		XMMATRIX skinningMatrix = XMMatrixMultiply(XMLoadFloat4x4A(&inverseBindPose), XMLoadFloat4x4A(&global)); // #matrixmultiplication

		DirectX::XMStoreFloat4x4A(&m_skinningMatrices[i], skinningMatrix);
	}
}

// #clipblend
void Animation::AnimatedModel::_computeSkinningMatricesCombined(SkeletonPose* firstPose1, SkeletonPose* secondPose1, float weight1, SkeletonPose* firstPose2, SkeletonPose* secondPose2, float weight2)
{
	using namespace DirectX;

	_computeModelMatricesCombined(firstPose1, secondPose1, weight1, firstPose2, secondPose2, weight2);

	for (int i = 0; i < m_skeleton->m_jointCount; i++)
	{
		const XMFLOAT4X4A& global = m_globalMatrices[i];
		const XMFLOAT4X4A& inverseBindPose = m_skeleton->m_joints[i].m_inverseBindPose;

		XMMATRIX skinningMatrix = XMMatrixMultiply(XMLoadFloat4x4A(&inverseBindPose), XMLoadFloat4x4A(&global)); // #matrixmultiplication

		DirectX::XMStoreFloat4x4A(&m_skinningMatrices[i], skinningMatrix);
	}
}

void Animation::AnimatedModel::_computeModelMatrices(SkeletonPose * pose)
{
	using namespace DirectX;

	//Check if we have layers
	std::optional<Animation::SkeletonPose> layerPose = std::nullopt;
	if (m_LayerStateMachine)
		layerPose = m_LayerStateMachine->GetCurrentState().recieveStateVisitor(*m_LayerVisitor);

	if (layerPose.has_value())
		XMStoreFloat4x4A(&m_globalMatrices[0], Animation::_createMatrixFromSRT(getAdditivePose(pose->m_jointPoses[0].m_transformation, layerPose.value().m_jointPoses[0].m_transformation).m_transformation));
	else
		XMStoreFloat4x4A(&m_globalMatrices[0], Animation::_createMatrixFromSRT(pose->m_jointPoses[0].m_transformation));

	for (int i = 1; i < m_skeleton->m_jointCount; i++)
	{
		const int16_t parentIndex = m_skeleton->m_joints[i].parentIndex;
		const XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&m_globalMatrices[parentIndex]);

		if (layerPose.has_value())
			DirectX::XMStoreFloat4x4A(&m_globalMatrices[i], XMMatrixMultiply(Animation::_createMatrixFromSRT(getAdditivePose(pose->m_jointPoses[i].m_transformation, layerPose.value().m_jointPoses[i]).m_transformation), parentGlobalMatrix)); // #matrixmultiplication
		else
			XMStoreFloat4x4A(&m_globalMatrices[i], XMMatrixMultiply(Animation::_createMatrixFromSRT(pose->m_jointPoses[i].m_transformation), parentGlobalMatrix));
	}
}

// #modelmatrix 
void Animation::AnimatedModel::_computeModelMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight)
{
	using namespace DirectX;

	//Check if we have layers
	std::optional<Animation::SkeletonPose> layerPose = std::nullopt;
	if (m_LayerStateMachine)
		layerPose = m_LayerStateMachine->GetCurrentState().recieveStateVisitor(*m_LayerVisitor);


	auto rootJointPose = _interpolateJointPose(&firstPose->m_jointPoses[0], &secondPose->m_jointPoses[0], weight);
	if (layerPose.has_value())
		rootJointPose = getAdditivePose(rootJointPose, layerPose.value().m_jointPoses[0]);

	DirectX::XMStoreFloat4x4A(&m_globalMatrices[0], Animation::_createMatrixFromSRT(rootJointPose.m_transformation));
	



	for (int i = 1; i < m_skeleton->m_jointCount; i++) //start at second joint (first is root, already processed)
	{
		const int16_t parentIndex = m_skeleton->m_joints[i].parentIndex;
		const XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&m_globalMatrices[parentIndex]);
		auto jointPose = _interpolateJointPose(&firstPose->m_jointPoses[i], &secondPose->m_jointPoses[i], weight);
		if (layerPose.has_value())
			jointPose = getAdditivePose(jointPose, layerPose.value().m_jointPoses[i]);
		DirectX::XMStoreFloat4x4A(&m_globalMatrices[i], XMMatrixMultiply(Animation::_createMatrixFromSRT(jointPose.m_transformation), parentGlobalMatrix)); // #matrixmultiplication
	}
}

// #clipblend
void Animation::AnimatedModel::_computeModelMatrices(SkeletonPose* firstPose1, SkeletonPose* secondPose1, float weight1, SkeletonPose* firstPose2, SkeletonPose* secondPose2, float weight2)
{
	using namespace DirectX;
	float clipBlendWeight = m_currentBlendTime / m_targetBlendTime;

	auto rootJointPose1 = _interpolateJointPose(&firstPose1->m_jointPoses[0], &secondPose1->m_jointPoses[0], weight1);
	auto rootJointPose2 = _interpolateJointPose(&firstPose2->m_jointPoses[0], &secondPose2->m_jointPoses[0], weight2);
	auto finalRootJointPose = _interpolateJointPose(&rootJointPose1, &rootJointPose2, clipBlendWeight);

	DirectX::XMStoreFloat4x4A(&m_globalMatrices[0], Animation::_createMatrixFromSRT(finalRootJointPose.m_transformation));

	for (int i = 1; i < m_skeleton->m_jointCount; i++) //start at second joint (first is root, already processed)
	{
		const int16_t parentIndex = m_skeleton->m_joints[i].parentIndex;
		const XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&m_globalMatrices[parentIndex]);

		auto jointPose1 = _interpolateJointPose(&firstPose1->m_jointPoses[i], &secondPose1->m_jointPoses[i], weight1);
		auto jointPose2 = _interpolateJointPose(&firstPose2->m_jointPoses[i], &secondPose2->m_jointPoses[i], weight2);
		auto finalJointPose = _interpolateJointPose(&jointPose1, &jointPose2, clipBlendWeight);

		XMStoreFloat4x4A(&m_globalMatrices[i], XMMatrixMultiply(Animation::_createMatrixFromSRT(finalJointPose.m_transformation), parentGlobalMatrix)); // #matrixmultiplication
	}
}

// #convert Transform conversion
Animation::SRT Animation::ConvertTransformToSRT(ImporterLibrary::Transform transform)
{
	using namespace DirectX;
	SRT srt = {};
	float pitch = DirectX::XMConvertToRadians(transform.transform_rotation[0]);
	float yaw = DirectX::XMConvertToRadians(transform.transform_rotation[1]);
	float roll = DirectX::XMConvertToRadians(transform.transform_rotation[2]);
	XMStoreFloat4A(&srt.m_rotationQuaternion, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));

	srt.m_rotationQuaternion.x *= -1;
	srt.m_rotationQuaternion.y *= -1;

	srt.m_translation = { transform.transform_position[0], transform.transform_position[1], transform.transform_position[2], 1.0f };
	srt.m_scale = { 1.0, 1.0, 1.0, 1.0f };
	return srt;
}

// #interpolate
void Animation::AnimatedModel::_interpolatePose(SkeletonPose * firstPose, SkeletonPose * secondPose, float weight)
{
	for (int i = 0; i < m_skeleton->m_jointCount; i++)
	{		
		DirectX::XMVECTOR firstRotation     = DirectX::XMLoadFloat4A(&firstPose->m_jointPoses[i].m_transformation.m_rotationQuaternion);
		DirectX::XMVECTOR secondRotation    = DirectX::XMLoadFloat4A(&secondPose->m_jointPoses[i].m_transformation.m_rotationQuaternion);
		DirectX::XMVECTOR firstTranslation  = DirectX::XMLoadFloat4A(&firstPose->m_jointPoses[i].m_transformation.m_translation);

		DirectX::XMVECTOR secondTranslation = DirectX::XMLoadFloat4A(&secondPose->m_jointPoses[i].m_transformation.m_translation);
		DirectX::XMVECTOR firstScale        = DirectX::XMLoadFloat4A(&firstPose->m_jointPoses[i].m_transformation.m_scale);
		DirectX::XMVECTOR secondScale       = DirectX::XMLoadFloat4A(&secondPose->m_jointPoses[i].m_transformation.m_scale);

		DirectX::XMVECTOR newRotation    = DirectX::XMQuaternionSlerp(firstRotation, secondRotation, weight);
		DirectX::XMVECTOR newTranslation = DirectX::XMVectorLerp(firstTranslation, secondTranslation, weight);
		DirectX::XMVECTOR newScale       = DirectX::XMVectorLerp(firstScale, secondScale, weight);
	}
	//TODO do stuff / return new skeleton pose
}

// #interpolate
Animation::JointPose Animation::AnimatedModel::_interpolateJointPose(JointPose * firstPose, JointPose * secondPose, float weight)//1.0 weight means 100% second pose
{
	using namespace DirectX;

	XMVECTOR firstRotation     = XMLoadFloat4A(&firstPose->m_transformation.m_rotationQuaternion);
	XMVECTOR secondRotation    = XMLoadFloat4A(&secondPose->m_transformation.m_rotationQuaternion);
	XMVECTOR firstTranslation  = XMLoadFloat4A(&firstPose->m_transformation.m_translation);
	XMVECTOR secondTranslation = XMLoadFloat4A(&secondPose->m_transformation.m_translation);
	XMVECTOR firstScale        = XMLoadFloat4A(&firstPose->m_transformation.m_scale);
	XMVECTOR secondScale       = XMLoadFloat4A(&secondPose->m_transformation.m_scale);

	DirectX::XMVECTOR newRotation    = XMQuaternionSlerp(firstRotation, secondRotation, weight);
	DirectX::XMVECTOR newTranslation = XMVectorLerp(firstTranslation, secondTranslation, weight);
	DirectX::XMVECTOR newScale       = XMVectorLerp(firstScale, secondScale, weight);

	SRT srt = {};
	XMStoreFloat4A(&srt.m_rotationQuaternion, newRotation);
	XMStoreFloat4A(&srt.m_scale, newScale);
	XMStoreFloat4A(&srt.m_translation, newTranslation);

	return JointPose(srt);
}

std::pair<uint16_t, float> Animation::AnimatedModel::_computeIndexAndProgression(float deltaTime, float currentTime, uint16_t frameCount)
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

std::pair<uint16_t, float> Animation::AnimatedModel::_computeIndexAndProgression(float deltaTime, float* currentTime, uint16_t frameCount)
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
	return std::move(std::make_pair(static_cast<uint16_t>(prevIndex), progression));
}

std::optional<std::pair<uint16_t, float>> Animation::AnimatedModel::_computeIndexAndProgressionOnce(float deltaTime, float* currentTime, uint16_t frameCount)
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

std::pair<uint16_t, float> Animation::AnimatedModel::_computeIndexAndProgressionNormalized(float deltaTime, float* currentTime, uint16_t frameCount)
{
	if (!timeAlreadyUpdatedThisFrame)
	{
		deltaTime /= (frameCount / ANIMATION_FRAMERATE);
		*currentTime += deltaTime;
		timeAlreadyUpdatedThisFrame = true;
	}

	frameCount -= 1;

	float properTime = std::fmod(*currentTime, 1.0f);
	*currentTime = properTime;
	///calc the actual frame index and progression towards the next frame
	//float actualTime = properTime / (1.0 / 24.0);
	float prevIndexFloat = (properTime * frameCount) ;
	int prevIndexInt = (int)prevIndexFloat;
	float lol = properTime * frameCount;
	float progression = prevIndexFloat - (float)prevIndexInt;


	//return values
	return std::move(std::make_pair(static_cast<uint16_t>(prevIndexInt), progression));
}

void Animation::AnimatedModel::UpdateCombined(float deltaTime)
{
	if (m_targetClip) // #todo
	{

	}
	else
	{
		///calc the actual frame index and progression towards the next frame for both clips
		auto indexAndProgression = _computeIndexAndProgression(deltaTime, &m_currentTime, m_currentClip->m_frameCount);
		uint16_t prevIndex = indexAndProgression.first;
		float progression = indexAndProgression.second;

		auto targetIndexAndProgression = _computeIndexAndProgression(deltaTime, &m_combinedClip.secondCurrentTime, m_combinedClip.secondClip->m_frameCount);
		uint16_t targetPrevIndex = targetIndexAndProgression.first;
		float targetProgression = targetIndexAndProgression.second;

		/// compute skinning matrices
		if (m_isPlaying)
		{
			_computeSkinningMatricesCombined(
				&m_currentClip->m_skeletonPoses[prevIndex], &m_currentClip->m_skeletonPoses[prevIndex + 1], progression,
				&m_combinedClip.secondClip->m_skeletonPoses[targetPrevIndex], &m_combinedClip.secondClip->m_skeletonPoses[targetPrevIndex + 1], targetProgression);
		}
	}
}

void Animation::AnimatedModel::_computeModelMatricesCombined(SkeletonPose* firstPose1, SkeletonPose* secondPose1, float weight1, SkeletonPose* firstPose2, SkeletonPose* secondPose2, float weight2)
{
	using namespace DirectX;
	float clipBlendWeight = m_combinedClip.secondWeight;

	auto rootJointPose1 = _interpolateJointPose(&firstPose1->m_jointPoses[0], &secondPose1->m_jointPoses[0], weight1);
	auto rootJointPose2 = _interpolateJointPose(&firstPose2->m_jointPoses[0], &secondPose2->m_jointPoses[0], weight2);
	auto finalRootJointPose = _interpolateJointPose(&rootJointPose1, &rootJointPose2, clipBlendWeight);

	DirectX::XMStoreFloat4x4A(&m_globalMatrices[0], Animation::_createMatrixFromSRT(finalRootJointPose.m_transformation));

	for (int i = 1; i < m_skeleton->m_jointCount; i++) //start at second joint (first is root, already processed)
	{
		const int16_t parentIndex = m_skeleton->m_joints[i].parentIndex;
		const XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&m_globalMatrices[parentIndex]);

		auto jointPose1 = _interpolateJointPose(&firstPose1->m_jointPoses[i], &secondPose1->m_jointPoses[i], weight1);
		auto jointPose2 = _interpolateJointPose(&firstPose2->m_jointPoses[i], &secondPose2->m_jointPoses[i], weight2);
		auto finalJointPose = _interpolateJointPose(&jointPose1, &jointPose2, clipBlendWeight);

		XMStoreFloat4x4A(&m_globalMatrices[i], XMMatrixMultiply(Animation::_createMatrixFromSRT(finalJointPose.m_transformation), parentGlobalMatrix)); // #matrixmultiplication
	}
}

void Animation::AnimatedModel::UpdateLooping(Animation::AnimationClip* clip)
{
	if (clip != m_currentClip)
		this->SetPlayingClip(clip, true, false);

	{
		/// increase local time
		//done in _computeIndexAndProgression()

		///calc the actual frame index and progression towards the next frame
		auto indexAndProgression = _computeIndexAndProgression(m_currentFrameDeltaTime, &m_currentTime, m_currentClip->m_frameCount);
		auto prevIndex = indexAndProgression.first;
		auto progression = indexAndProgression.second;

		///if we exceeded clips time, set back to 0 ish if we are looping, or stop if we aren't
		if (prevIndex >= m_currentClip->m_frameCount - 1) /// -1 because last frame is only used to interpolate towards
		{
			assert(1 == 0);
			if (m_isLooping)
			{
				m_currentTime = 0.0 + progression;

				prevIndex = std::floorf(m_currentClip->m_framerate / 2 * m_currentTime);
				progression = std::fmod(m_currentTime, 1.0 / 24.0);
			}
			else
			{
				m_isPlaying = false;
			}
		}

		/// compute skinning matrices
		if (m_isPlaying)
			_computeSkinningMatrices(&m_currentClip->m_skeletonPoses[prevIndex], &m_currentClip->m_skeletonPoses[prevIndex + 1], progression);
	}
}

void Animation::AnimatedModel::UpdateOnce(Animation::AnimationClip* clip)
{
	if (clip != m_currentClip)
		this->SetPlayingClip(clip, false, false);

	if (m_isPlaying)
	{
		/// increase local time
		//done in _computeIndexAndProgression()

		///calc the actual frame index and progression towards the next frame
		auto indexAndProgression = _computeIndexAndProgressionOnce(m_currentFrameDeltaTime, &m_currentTime, m_currentClip->m_frameCount);

		uint16_t prevIndex = 0;
		float progression = 0.0f;
		if (indexAndProgression.has_value())
		{
			prevIndex = indexAndProgression.value().first;
			progression = indexAndProgression.value().second;
		}
		else
		{
			m_isPlaying = false;
			prevIndex = m_currentClip->m_frameCount - 1;
			_computeSkinningMatrices(&m_currentClip->m_skeletonPoses[prevIndex]);
			return;
		}
		///if we exceeded clips time, set back to 0 ish if we are looping, or stop if we aren't
		if (prevIndex >= m_currentClip->m_frameCount - 1) /// -1 because last frame is only used to interpolate towards
		{
			assert(1 == 0);
			if (m_isLooping)
			{
				m_currentTime = 0.0 + progression;

				prevIndex = std::floorf(m_currentClip->m_framerate / 2 * m_currentTime);
				progression = std::fmod(m_currentTime, 1.0 / 24.0);
			}
			else
			{
				m_isPlaying = false;
			}
		}

		/// compute skinning matrices
		if (m_isPlaying)
			_computeSkinningMatrices(&m_currentClip->m_skeletonPoses[prevIndex], &m_currentClip->m_skeletonPoses[prevIndex + 1], progression);
	}
}

Animation::SharedAnimation Animation::ConvertToAnimationClip(ImporterLibrary::AnimationFromFile* animation, uint8_t jointCount)
{
	uint32_t keyCount = animation->nr_of_keyframes;

	Animation::SharedAnimation clipToReturn = std::make_shared<Animation::AnimationClip>();
	clipToReturn->m_frameCount = static_cast<uint16_t>(keyCount);
	clipToReturn->m_skeletonPoses = std::make_unique<SkeletonPose[]>(clipToReturn->m_frameCount);

	//Init joint poses for skeleton poses
	std::for_each(clipToReturn->m_skeletonPoses.get(), clipToReturn->m_skeletonPoses.get() + clipToReturn->m_frameCount,
		[&](auto& e) { e.m_jointPoses = std::make_unique<JointPose[]>(jointCount); });

	for (int j = 0; j < jointCount; j++)
	{
		//for each key
		for (unsigned int k = 0; k < keyCount; k++)
		{
			Animation::SRT trans = ConvertTransformToSRT(animation->keyframe_transformations[j * animation->nr_of_keyframes + k]);
			clipToReturn->m_skeletonPoses[k].m_jointPoses[j].m_transformation = trans;
		}
	}
	clipToReturn->m_framerate = 24;
	return clipToReturn;
}

void Animation::SetInverseBindPoses(Animation::Skeleton* mainSkeleton, const ImporterLibrary::Skeleton* importedSkeleton)
{
	using namespace DirectX;

	std::vector<XMFLOAT4X4A> vec;
	std::vector <ImporterLibrary::Transform > vec2;
	DirectX::XMStoreFloat4x4A(&mainSkeleton->m_joints[0].m_inverseBindPose, _createMatrixFromSRT(importedSkeleton->joints[0].jointInverseBindPoseTransform));

	for (int i = 1; i < mainSkeleton->m_jointCount; i++)
	{
		const int16_t parentIndex = mainSkeleton->m_joints[i].parentIndex;

		DirectX::XMStoreFloat4x4A
		(&mainSkeleton->m_joints[i].m_inverseBindPose, _createMatrixFromSRT(importedSkeleton->joints[i].jointInverseBindPoseTransform));
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
	XMStoreFloat4A(&m_rotationQuaternion, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));

	m_rotationQuaternion.x *= -1;
	m_rotationQuaternion.y *= -1;

	m_translation = { transform.transform_position[0], transform.transform_position[1], transform.transform_position[2], 1.0f };
	m_scale = { 1.0, 1.0, 1.0, 1.0f };
}

Animation::SRT::SRT(const ImporterLibrary::DecomposedTransform& transform)
{
	m_rotationQuaternion.x = transform.rotation.x;
	m_rotationQuaternion.y = transform.rotation.y;
	m_rotationQuaternion.z = transform.rotation.z;
	m_rotationQuaternion.w = transform.rotation.w;

	m_translation.x = transform.translation.x;
	m_translation.y = transform.translation.y;
	m_translation.z = transform.translation.z;
	m_translation.w = transform.translation.w;

	m_scale.x = transform.scale.x;
	m_scale.y = transform.scale.y;
	m_scale.z = transform.scale.z;
	m_scale.w = transform.scale.w;
}

bool Animation::SRT::operator==(const SRT& other)
{
	return (m_rotationQuaternion.x == other.m_rotationQuaternion.x
		&& m_rotationQuaternion.y == other.m_rotationQuaternion.y
		&& m_rotationQuaternion.z == other.m_rotationQuaternion.z
		&& m_rotationQuaternion.w == other.m_rotationQuaternion.w
		&& m_scale.x == other.m_scale.x
		&& m_scale.z == other.m_scale.z
		&& m_scale.y == other.m_scale.y
		&& m_scale.w == other.m_scale.w
		&& m_translation.x == other.m_translation.x
		&& m_translation.z == other.m_translation.z
		&& m_translation.y == other.m_translation.y
		&& m_translation.w == other.m_translation.w
		);
}

Animation::Skeleton::Skeleton(const ImporterLibrary::Skeleton& skeleton)
{
	m_jointCount = static_cast<uint8_t>(skeleton.joints.size());
	m_joints = std::make_unique<Animation::Joint[]>(m_jointCount);

	for (int i = 0; i < m_jointCount; i++)
	{
		m_joints[i].parentIndex = skeleton.joints[i].parentIndex;
	}
	m_joints[0].parentIndex = -1; // Root does not have a real parent index
	Animation::SetInverseBindPoses(this, &skeleton);
}

Animation::JointPose::JointPose(const SRT& srt)
{
	m_transformation = srt;
}

Animation::AnimationClip::AnimationClip(const ImporterLibrary::AnimationFromFileStefan& animation, std::shared_ptr<Skeleton> skeleton)
{
	auto keyCount   = animation.nr_of_keyframes;
	m_skeleton      = skeleton;
	m_framerate     = 24;
	m_frameCount    = static_cast<uint16_t>(keyCount);
	m_skeletonPoses = std::make_unique<SkeletonPose[]>(m_frameCount);

	//Init joint poses for skeleton poses
	for (int i = 0; i < m_frameCount; i++)
	{
		m_skeletonPoses[i].m_jointPoses = std::make_unique<JointPose[]>(m_skeleton->m_jointCount);
	}

	
	for (uint32_t k = 0; k < keyCount; k++) //for each key
	{
		for (uint8_t j = 0; j < m_skeleton->m_jointCount; j++) //and each joint
		{
			//init the transformation from the raw keyframe array
			auto srt = SRT(animation.keyframe_transformations[k * skeleton->m_jointCount + j]);
			m_skeletonPoses[k].m_jointPoses[j].m_transformation = srt;
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