#include "AnimatedModel.h"

#define loadMatrix(x) XMLoadFloat4x4(x)
#define storeMatrix(x,y) XMStoreFloat4x4(x,y)

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

DirectX::XMMATRIX Animation::_createMatrixFromSRT(const SRT& srt)
{
	XMFLOAT4A fScale = ( srt.m_scale);
	XMFLOAT4A fRotation = srt.m_rotationQuaternion;
	XMFLOAT4A fTranslation = srt.m_translation;
	return XMMatrixAffineTransformation(XMLoadFloat4A(&fScale), { 0.0, 0.0, 0.0, 1.0 }, XMLoadFloat4A(&fRotation), XMLoadFloat4A(&fTranslation));
}

Animation::SRT Animation::_convertToSRT(const MyLibrary::Transform transform)
{
	SRT srt = {};

	//TODO CHECK 
	XMStoreFloat4A(&srt.m_rotationQuaternion, XMQuaternionRotationRollPitchYaw(transform.transform_rotation[0], transform.transform_rotation[1], transform.transform_rotation[2]));
	srt.m_scale = { transform.transform_scale[0], transform.transform_scale[1], transform.transform_scale[2], 1.0 };
	srt.m_translation = { transform.transform_position[0], transform.transform_position[1], transform.transform_position[2], 1.0 };

	return srt;
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
	DirectX::XMStoreFloat4x4A(&m_globalMatrices[0], _createMatrixFromSRT(pose->m_jointPoses[0].m_transformation));

	for (int i = 1; i < m_skeleton->m_jointCount; i++) //start at second joint (first is root, already processed)
	{
		uint8_t parentIndex = m_skeleton->m_joints[i].parentIndex;
		assert(parentIndex > -1);

		XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&m_globalMatrices[parentIndex]);
		DirectX::XMStoreFloat4x4A(&m_globalMatrices[i], XMMatrixMultiply(_createMatrixFromSRT(pose->m_jointPoses[i].m_transformation), parentGlobalMatrix));
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

	uint32_t keyCount = animation->nr_of_keyframes;

	AnimationClip* clipToReturn = new AnimationClip();
	clipToReturn->m_skeletonPoses = new SkeletonPose[jointCount];
	clipToReturn->m_frameCount = static_cast<uint16_t>(animation->nr_of_keyframes);

	//for each joint
	for (int j = 0; j < jointCount; j++)
	{
		//for each key
		for (int k = 0; k < keyCount; k++)
		{
			//create jointPose for this key
			JointPose pose = {};
			pose.m_transformation = Animation::_convertToSRT(animation->keyframe_transformations[j * keyCount + k]);

			clipToReturn->m_skeletonPoses[k].m_jointPoses[j] = pose;
		}
	}
	return clipToReturn;
}

Animation::Skeleton * Animation::ConvertToSkeleton(MyLibrary::SkeletonFromFile * skeleton)
{
	Skeleton* SkeletonToReturn = new Skeleton();
	float4x4 tempGlobalMatrix;
	SRT temp;
	SRT globalTemp;

	XMStoreFloat4(&globalTemp.m_rotationQuaternion,  XMQuaternionRotationRollPitchYaw( skeleton->skeleton_joints[0].joint_transform.transform_rotation[0], skeleton->skeleton_joints[0].joint_transform.transform_rotation[1], temp.m_rotationQuaternion.z = skeleton->skeleton_joints[0].joint_transform.transform_rotation[2]));
	globalTemp.m_scale = { skeleton->skeleton_joints[0].joint_transform.transform_scale[0], skeleton->skeleton_joints[0].joint_transform.transform_scale[1], skeleton->skeleton_joints[0].joint_transform.transform_scale[2], 1.0f};
	globalTemp.m_translation = { skeleton->skeleton_joints[0].joint_transform.transform_position[0], skeleton->skeleton_joints[0].joint_transform.transform_position[1], skeleton->skeleton_joints[0].joint_transform.transform_position[2], 1.0f };
	
	DirectX::XMStoreFloat4x4A(&tempGlobalMatrix, _createMatrixFromSRT(globalTemp));

	SkeletonToReturn->m_jointCount = skeleton->skeleton_nrOfJoints;
	SkeletonToReturn->m_joints->parentIndex = skeleton->skeleton_joints->parentIndex;

	for (int i = 1; i < skeleton->skeleton_nrOfJoints; i++) //start at second joint (first is root, already processed)
	{
		uint8_t parentIndex = skeleton->skeleton_joints[i].parentIndex;
		assert(parentIndex > -1);

		XMStoreFloat4(&temp.m_rotationQuaternion, XMQuaternionRotationRollPitchYaw(skeleton->skeleton_joints[0].joint_transform.transform_rotation[0], skeleton->skeleton_joints[0].joint_transform.transform_rotation[1], temp.m_rotationQuaternion.z = skeleton->skeleton_joints[0].joint_transform.transform_rotation[2]));
		temp.m_scale = { skeleton->skeleton_joints[0].joint_transform.transform_scale[0], skeleton->skeleton_joints[0].joint_transform.transform_scale[1], skeleton->skeleton_joints[0].joint_transform.transform_scale[2], 1.0f };
		temp.m_translation = { skeleton->skeleton_joints[0].joint_transform.transform_position[0], skeleton->skeleton_joints[0].joint_transform.transform_position[1], skeleton->skeleton_joints[0].joint_transform.transform_position[2], 1.0f };

		XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&SkeletonToReturn->m_joints[parentIndex].m_inverseBindPose);
		DirectX::XMStoreFloat4x4A(&SkeletonToReturn->m_joints[i].m_inverseBindPose, XMMatrixMultiply(_createMatrixFromSRT(temp), parentGlobalMatrix));
	}
	return SkeletonToReturn;
}


