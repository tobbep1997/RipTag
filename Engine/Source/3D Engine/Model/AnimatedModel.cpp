#include "AnimatedModel.h"



Animation::AnimatedModel::AnimatedModel()
{
}

Animation::AnimatedModel::AnimatedModel(SkinnedMesh * mesh)
{
	//m_skeleton = mesh->GetSkeleton();
}

Animation::AnimatedModel::~AnimatedModel()
{
}

void Animation::AnimatedModel::Init(SkinnedMesh * mesh)
{
	if (mesh)
		m_mesh = mesh;
}

void Animation::AnimatedModel::Update(float deltaTime)
{
	if (m_isPlaying)
	{

	}
}

void Animation::AnimatedModel::SetPlayingClip(AnimationClip * clip)
{
	m_currentClip = clip;
}

void Animation::AnimatedModel::Pause()
{
	m_isPlaying = false;
}

void Animation::AnimatedModel::Play()
{
	m_isPlaying = true;
}

void Animation::AnimatedModel::_computeSkinningMatrices()
{
	//no
}

void Animation::AnimatedModel::_computeModelMatrix(Joint * joint)
{
	//no
}

DirectX::XMMATRIX Animation::AnimatedModel::recursiveMultiplyParents(uint8_t jointIndex, SkeletonPose* pose)
//TODO quadruple check
{
	XMVECTOR thisRotation = XMLoadFloat4A(&pose->m_jointPoses[jointIndex].m_transformation.m_rotationQuaternion);

	XMFLOAT4A scale = XMFLOAT4A
	(
		pose->m_jointPoses[jointIndex].m_transformation.m_scale,
		pose->m_jointPoses[jointIndex].m_transformation.m_scale,
		pose->m_jointPoses[jointIndex].m_transformation.m_scale,
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
