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

DirectX::XMMATRIX Animation::AnimatedModel::_createMatrixFromSRT(const SRT& srt)
{
	XMFLOAT4A fScale = { srt.m_scale, srt.m_scale, srt.m_scale, 1.0 };
	XMFLOAT4A fRotation = srt.m_rotationQuaternion;
	XMFLOAT4A fTranslation = srt.m_translation;
	return XMMatrixAffineTransformation(XMLoadFloat4A(&fScale), { 0.0, 0.0, 0.0, 1.0 }, XMLoadFloat4A(&fRotation), XMLoadFloat4A(&fTranslation));
}

void Animation::AnimatedModel::_computeSkinningMatrices()
{
	//no
}

void Animation::AnimatedModel::_computeModelMatrices(SkeletonPose* pose)
{ //TODO

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
