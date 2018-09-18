#include "AnimatedModel.h"

#define loadMatrix(x) XMLoadFloat4x4(x)
#define storeMatrix(x,y) XMStoreFloat4x4(x,y)

Animation::AnimatedModel::AnimatedModel()
{
}



Animation::AnimatedModel::~AnimatedModel()
{
}


void Animation::AnimatedModel::Update(float deltaTime)
{
	if (m_isPlaying)
	{
		uint16_t lastFrame = m_currentTime * (1.0 / m_currentClip->m_framerate);
		m_currentTime += deltaTime;
		uint16_t currentFrame = std::floorf(m_currentTime * (1.0 / m_currentClip->m_framerate));

		if (currentFrame != lastFrame)
			_computeSkinningMatrices(&m_currentClip->m_skeletonPoses[currentFrame]);
	}
}

void Animation::AnimatedModel::SetPlayingClip(AnimationClip * clip, bool isLooping)
{
	m_currentClip = clip;
	m_currentTime = 0.0f;
}

void Animation::AnimatedModel::Pause()
{
	m_isPlaying = false;
}

void Animation::AnimatedModel::Play()
{
	m_isPlaying = true;
}

DirectX::XMMATRIX Animation::AnimatedModel::_createMatrixFromSRT(const SRT& srt)
{
	XMFLOAT4A fScale = ( srt.m_scale);
	XMFLOAT4A fRotation = srt.m_rotationQuaternion;
	XMFLOAT4A fTranslation = srt.m_translation;
	return XMMatrixAffineTransformation(XMLoadFloat4A(&fScale), { 0.0, 0.0, 0.0, 1.0 }, XMLoadFloat4A(&fRotation), XMLoadFloat4A(&fTranslation));
}

void Animation::AnimatedModel::_computeSkinningMatrices(SkeletonPose* pose)
{
	_computeModelMatrices(pose);

	for (int i = 0; i < m_skeleton->m_jointCount; i++)
	{
		XMFLOAT4X4 global = m_globalMatrices[i];
		XMFLOAT4X4 inverseBindPose = m_skeleton->m_joints[i].m_inverseBindPose;
		XMMATRIX skinningMatrix = XMMatrixMultiply(loadMatrix(&global), loadMatrix(&inverseBindPose));
		storeMatrix(&m_skinningMatrices[i], skinningMatrix);
	}
}

void Animation::AnimatedModel::_computeModelMatrices(SkeletonPose* pose)
{
	XMStoreFloat4x4A(&m_globalMatrices[0], _createMatrixFromSRT(pose->m_jointPoses[0].m_transformation));

	for (int i = 1; i < m_skeleton->m_jointCount; i++) //start at second joint (first is root, already processed)
	{
		uint8_t parentIndex = m_skeleton->m_joints[i].parentIndex;
		assert(parentIndex > -1);

		XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&m_globalMatrices[parentIndex]);
		XMStoreFloat4x4A(&m_globalMatrices[i], XMMatrixMultiply(_createMatrixFromSRT(pose->m_jointPoses[i].m_transformation), parentGlobalMatrix));
	}
}

DirectX::XMMATRIX Animation::AnimatedModel::recursiveMultiplyParents(uint8_t jointIndex, SkeletonPose* pose)
//TODO remove
{
	XMVECTOR thisRotation = XMLoadFloat4A(&pose->m_jointPoses[jointIndex].m_transformation.m_rotationQuaternion);

	XMFLOAT4A scale = (
		pose->m_jointPoses[jointIndex].m_transformation.m_scale
	);
	XMVECTOR thisScale = XMLoadFloat4A(&scale);
	XMVECTOR thisTranslation = XMLoadFloat4A(&pose->m_jointPoses[jointIndex].m_transformation.m_translation);
	XMMATRIX thisJoint = DirectX::XMMatrixAffineTransformation(thisScale, { 0, 0, 0, 1 }, thisRotation, thisTranslation);
	uint8_t parentIndex = m_skeleton->m_joints[jointIndex].parentIndex;
	if (parentIndex >= 0)
		return XMMatrixMultiply(thisJoint, recursiveMultiplyParents(m_skeleton->m_joints[jointIndex].parentIndex, pose));
	else return thisJoint;
}

Animation::AnimationClip* Animation::ConvertToAnimationClip(MyLibrary::AnimationFromFile* animation, uint8_t jointCount)
{
	using std::vector;

	AnimationClip* clipToReturn = new AnimationClip();
	clipToReturn->m_skeletonPoses = new SkeletonPose[jointCount];
	clipToReturn->m_frameCount = static_cast<uint8_t>(animation->nr_of_keyframes);
	for (int j = 0; j < jointCount; j++)
	{
		for (int k = 0; k < animation->nr_of_keyframes; k++)
		{
			//
			vector<JointPose> jointPoses;
		}
	}
}

Animation::Skeleton * Animation::ConvertToSkeleton(MyLibrary::SkeletonFromFile * skeleton)
{
	return nullptr;
}


