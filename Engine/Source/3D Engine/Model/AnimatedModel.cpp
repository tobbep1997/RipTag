#include "AnimatedModel.h"
#include "../Extern.h"

#define INVERT_BINDPOSE

Animation::AnimatedModel::AnimatedModel()
{
}

Animation::AnimatedModel::~AnimatedModel()
{
}

// | Computes the frame we're currently on and computes final skinning matrices for gpu skinning |
void Animation::AnimatedModel::Update(float deltaTime)
{
	deltaTime /= 8;
	if (m_isPlaying)
	{
		//increase local time
		m_currentTime += deltaTime;

		//calc time per frame TODO : cache
		float frameTime = static_cast<float>(1.0 / m_currentClip->m_framerate);

		//if we exceeded clips time, set back to 0 ish if we are looping, or stop if we arent
		if (m_currentTime > frameTime * m_currentClip->m_frameCount)
		{
			m_isPlaying
				? m_currentTime -= frameTime * m_currentClip->m_frameCount
				: m_isPlaying != m_isPlaying;
		}

		//calc the actual frame index and progression towards the next frame
		int prevIndex = std::floorf(m_currentClip->m_framerate*m_currentTime);
		float progression = std::fmod(m_currentClip->m_framerate*m_currentTime, 1.0f);

		if (prevIndex + 1 >= m_currentClip->m_frameCount)
		{
			float neg = (1 / 24.0f) * (m_currentClip->m_frameCount - 1);
			m_currentTime = m_currentTime - neg;

			prevIndex = std::floorf(m_currentClip->m_framerate*m_currentTime);
			progression = std::fmod(m_currentClip->m_framerate*m_currentTime, 1.0f);

		}

		// compute skinning matrices
		_computeSkinningMatrices(&m_currentClip->m_skeletonPoses[prevIndex], &m_currentClip->m_skeletonPoses[prevIndex+1], progression);
	}
}

void Animation::AnimatedModel::SetPlayingClip(AnimationClip * clip, bool isLooping)
{
	m_currentClip = clip;
	m_currentTime = 0.0f;
}

void Animation::AnimatedModel::SetSkeleton(Skeleton * skeleton)
{
	m_skeleton = skeleton;

	// make sure the matrix vectors can accommodate each joint matrix 
	m_globalMatrices.resize(skeleton->m_jointCount);
	m_skinningMatrices.resize(skeleton->m_jointCount);
}

void Animation::AnimatedModel::Pause()
{
	m_isPlaying = false;
}

void Animation::AnimatedModel::Play()
{
	m_isPlaying = true;
}

// | Returns a reference to the skinning matrix vector |
std::vector<DirectX::float4x4>& Animation::AnimatedModel::GetSkinningMatrices()
{
	return m_skinningMatrices;
}

DirectX::XMMATRIX Animation::_createMatrixFromSRT(const SRT& srt)
{
	using namespace DirectX;

	XMFLOAT4A fScale = ( srt.m_scale);
	XMFLOAT4A fRotation = srt.m_rotationQuaternion;
	XMFLOAT4A fTranslation = srt.m_translation;
	return XMMatrixAffineTransformation(XMLoadFloat4A(&fScale), { 0.0, 0.0, 0.0, 1.0 }, XMLoadFloat4A(&fRotation), XMLoadFloat4A(&fTranslation));
}

Animation::AnimationClip* Animation::LoadAndCreateAnimation(std::string file, Animation::Skeleton* skeleton)
{
	MyLibrary::Loadera loader;
	auto importedAnimation = loader.readAnimationFile(file);
	return new Animation::AnimationClip(importedAnimation, skeleton);
}

Animation::Skeleton* Animation::LoadAndCreateSkeleton(std::string file)
{
	MyLibrary::Loadera loader;
	auto importedSkeleton = loader.readSkeletonFile(file);
	return new Animation::Skeleton(importedSkeleton);
}

// | Calls _computeModelMatrices() then multiplies the results with the inverse bind pose | 
void Animation::AnimatedModel::_computeSkinningMatrices(SkeletonPose* pose)
{
	using namespace DirectX;

	_computeModelMatrices(pose);

	for (int i = 0; i < m_skeleton->m_jointCount; i++)
	{
		XMFLOAT4X4A global = m_globalMatrices[i];
		XMFLOAT4X4A inverseBindPose = m_skeleton->m_joints[i].m_inverseBindPose;
		XMMATRIX skinningMatrix = XMMatrixMultiply(XMLoadFloat4x4A(&global), XMLoadFloat4x4A(&inverseBindPose));
		DirectX::XMStoreFloat4x4A(&m_skinningMatrices[i], skinningMatrix);
		
		//DEBUG
		//storeMatrix(&m_skinningMatrices[i], XMMatrixIdentity());
	}
}

void Animation::AnimatedModel::_computeSkinningMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight)
{
	using namespace DirectX;

	_computeModelMatrices(firstPose, secondPose, weight);

	for (int i = 0; i < m_skeleton->m_jointCount; i++)
	{
		XMFLOAT4X4A global = m_globalMatrices[i];
		XMFLOAT4X4A inverseBindPose = m_skeleton->m_joints[i].m_inverseBindPose;
		XMMATRIX skinningMatrix = XMMatrixMultiply(XMLoadFloat4x4A(&global), XMLoadFloat4x4A(&inverseBindPose));
		DirectX::XMStoreFloat4x4A(&m_skinningMatrices[i], skinningMatrix);

		//DEBUG
		//storeMatrix(&m_skinningMatrices[i], XMMatrixIdentity());
	}
}

// | Compute model matrix for each joint using its local pose multiplied with it's parent's final model matrix | 
void Animation::AnimatedModel::_computeModelMatrices(SkeletonPose* pose)
{
	using namespace DirectX;

	DirectX::XMStoreFloat4x4A(&m_globalMatrices[0], Animation::_createMatrixFromSRT(pose->m_jointPoses[0].m_transformation));

	for (int i = 1; i < m_skeleton->m_jointCount; i++) //start at second joint (first is root, already processed)
	{
		int16_t parentIndex = m_skeleton->m_joints[i].parentIndex;
		assert(parentIndex > -1);
		assert(i > parentIndex);
		XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&m_globalMatrices[parentIndex]);
		DirectX::XMStoreFloat4x4A(&m_globalMatrices[i], XMMatrixMultiply(Animation::_createMatrixFromSRT(pose->m_jointPoses[i].m_transformation), parentGlobalMatrix));
	}
}

void Animation::AnimatedModel::_computeModelMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight)
{
	using namespace DirectX;
	auto rootJointPose = _interpolateJointPose(&firstPose->m_jointPoses[0], &secondPose->m_jointPoses[0], weight);
	DirectX::XMStoreFloat4x4A(&m_globalMatrices[0], Animation::_createMatrixFromSRT(rootJointPose.m_transformation));

	for (int i = 1; i < m_skeleton->m_jointCount; i++) //start at second joint (first is root, already processed)
	{
		int16_t parentIndex = m_skeleton->m_joints[i].parentIndex;
		assert(parentIndex > -1);
		assert(i > parentIndex);
		XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&m_globalMatrices[parentIndex]);
		auto jointPose = _interpolateJointPose(&firstPose->m_jointPoses[i], &secondPose->m_jointPoses[i], weight);
		DirectX::XMStoreFloat4x4A(&m_globalMatrices[i], XMMatrixMultiply(Animation::_createMatrixFromSRT(jointPose.m_transformation), parentGlobalMatrix));
	}
}
Animation::SRT Animation::ConvertTransformToSRT(MyLibrary::Transform transform)
{
	using namespace DirectX;
	SRT srt = {};
	XMStoreFloat4A(&srt.m_rotationQuaternion, XMQuaternionRotationRollPitchYaw(transform.transform_rotation[0], transform.transform_rotation[1], transform.transform_rotation[2]));
	srt.m_translation = { transform.transform_position[0], transform.transform_position[1], transform.transform_position[2], 1.0f };
	srt.m_scale = { 1.0, 1.0, 1.0, 1.0f };
	return srt;
}

void Animation::AnimatedModel::_interpolatePose(SkeletonPose * firstPose, SkeletonPose * secondPose, float weight) //1.0 weight means 100% second pose
{
	for (int i = 0; i < m_skeleton->m_jointCount; i++)
	{
		
		DirectX::XMVECTOR firstRotation = DirectX::XMLoadFloat4A(&firstPose->m_jointPoses[i].m_transformation.m_rotationQuaternion);
		DirectX::XMVECTOR secondRotation = DirectX::XMLoadFloat4A(&secondPose->m_jointPoses[i].m_transformation.m_rotationQuaternion);
		DirectX::XMVECTOR firstTranslation = DirectX::XMLoadFloat4A(&firstPose->m_jointPoses[i].m_transformation.m_translation);
		DirectX::XMVECTOR secondTranslation = DirectX::XMLoadFloat4A(&secondPose->m_jointPoses[i].m_transformation.m_translation);
		DirectX::XMVECTOR firstScale = DirectX::XMLoadFloat4A(&firstPose->m_jointPoses[i].m_transformation.m_scale);
		DirectX::XMVECTOR secondScale = DirectX::XMLoadFloat4A(&secondPose->m_jointPoses[i].m_transformation.m_scale);

		DirectX::XMVECTOR newRotation = DirectX::XMQuaternionSlerp(firstRotation, secondRotation, weight);
		DirectX::XMVECTOR newTranslation = DirectX::XMVectorLerp(firstTranslation, secondTranslation, weight);
		DirectX::XMVECTOR newScale = DirectX::XMVectorLerp(firstScale, secondScale, weight);
	}
	//TODO do stuff / return new skeleton pose
}

Animation::JointPose Animation::AnimatedModel::_interpolateJointPose(JointPose * firstPose, JointPose * secondPose, float weight)//1.0 weight means 100% second pose
{
	using namespace DirectX;

	XMVECTOR firstRotation = XMLoadFloat4A(&firstPose->m_transformation.m_rotationQuaternion);
	XMVECTOR secondRotation = XMLoadFloat4A(&secondPose->m_transformation.m_rotationQuaternion);
	XMVECTOR firstTranslation = XMLoadFloat4A(&firstPose->m_transformation.m_translation);
	XMVECTOR secondTranslation = XMLoadFloat4A(&secondPose->m_transformation.m_translation);
	XMVECTOR firstScale = XMLoadFloat4A(&firstPose->m_transformation.m_scale);
	XMVECTOR secondScale = XMLoadFloat4A(&secondPose->m_transformation.m_scale);

	DirectX::XMVECTOR newRotation = XMQuaternionSlerp(firstRotation, secondRotation, weight);
	DirectX::XMVECTOR newTranslation = XMVectorLerp(firstTranslation, secondTranslation, weight);
	DirectX::XMVECTOR newScale = XMVectorLerp(firstScale, secondScale, weight);

	SRT srt = {};
	XMStoreFloat4A(&srt.m_rotationQuaternion, newRotation);
	XMStoreFloat4A(&srt.m_scale, newScale);
	XMStoreFloat4A(&srt.m_translation, newTranslation);

	return JointPose(srt);
}

Animation::AnimationClip* Animation::ConvertToAnimationClip(MyLibrary::AnimationFromFile* animation, uint8_t jointCount)
{
	uint32_t keyCount = animation->nr_of_keyframes;

	AnimationClip* clipToReturn = new AnimationClip();
	clipToReturn->m_frameCount = static_cast<uint16_t>(keyCount);
	clipToReturn->m_skeletonPoses = std::make_unique<SkeletonPose[]>(clipToReturn->m_frameCount);

	// Review
	//Init joint poses for skeleton poses
	for (int i = 0; i < clipToReturn->m_frameCount; i++)
	{
		clipToReturn->m_skeletonPoses[i].m_jointPoses = std::make_unique<JointPose[]>(jointCount);
	}

	for (int j = 0; j < jointCount; j++)
	{
		//for each key
		for (unsigned int k = 0; k < keyCount; k++)
		{
			// Review
			Animation::SRT trans = ConvertTransformToSRT(animation->keyframe_transformations[j * animation->nr_of_keyframes + k]);
			clipToReturn->m_skeletonPoses[k].m_jointPoses[j].m_transformation = trans;
		}
	}
	clipToReturn->m_framerate = 24;
	return clipToReturn;
}

Animation::Skeleton * Animation::ConvertToSkeleton(MyLibrary::SkeletonFromFile * skeleton)
{
	Animation::Skeleton* skeletonToReturn = new Animation::Skeleton();
	skeletonToReturn->m_jointCount = skeleton->skeleton_nrOfJoints;
	skeletonToReturn->m_joints = std::make_unique<Animation::Joint[]>(skeletonToReturn->m_jointCount);

	for (int i = 0; i < skeletonToReturn->m_jointCount; i++)
	{
		skeletonToReturn->m_joints[i].parentIndex = skeleton->skeleton_joints[i].parentIndex;
	}
	skeletonToReturn->m_joints[0].parentIndex = -1;
	Animation::SetInverseBindPoses(skeletonToReturn, skeleton);
	return skeletonToReturn;
}

void Animation::SetInverseBindPoses(Animation::Skeleton* mainSkeleton, const MyLibrary::SkeletonFromFile* importedSkeleton)
{
	using namespace DirectX;
	DirectX::XMStoreFloat4x4A(&mainSkeleton->m_joints[0].m_inverseBindPose, _createMatrixFromSRT(ConvertTransformToSRT(importedSkeleton->skeleton_joints[0].joint_transform)));

	for (int i = 1; i < mainSkeleton->m_jointCount; i++)
	{
		int16_t parentIndex = mainSkeleton->m_joints[i].parentIndex;
		assert(i > parentIndex);
#ifdef INVERT_BINDPOSE
		DirectX::XMStoreFloat4x4A
		(&mainSkeleton->m_joints[i].m_inverseBindPose, XMMatrixInverse(nullptr, XMMatrixMultiply(_createMatrixFromSRT(ConvertTransformToSRT(importedSkeleton->skeleton_joints[i].joint_transform)), XMLoadFloat4x4A(&mainSkeleton->m_joints[parentIndex].m_inverseBindPose))));
#else
		DirectX::XMStoreFloat4x4A
		(&mainSkeleton->m_joints[i].m_inverseBindPose, XMMatrixMultiply(_createMatrixFromSRT(ConvertTransformToSRT(importedSkeleton->skeleton_joints[i].joint_transform)), XMLoadFloat4x4A(&mainSkeleton->m_joints[parentIndex].m_inverseBindPose)));
#endif
		DirectX::XMStoreFloat4x4A
		(&mainSkeleton->m_joints[i].m_inverseBindPose, XMMatrixMultiply(_createMatrixFromSRT(ConvertTransformToSRT(importedSkeleton->skeleton_joints[i].joint_transform)), XMLoadFloat4x4A(&mainSkeleton->m_joints[parentIndex].m_inverseBindPose)));

	}
}

Animation::AnimationCBuffer::AnimationCBuffer()
{
	SetAnimationCBuffer();
}

Animation::AnimationCBuffer::~AnimationCBuffer()
{

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
	memcpy(m_AnimationValues.skinnedMatrix, &buffer->skinnedMatrix, sizeof(sizeof(float) * 16 * MAXJOINT));
	DX::g_deviceContext->Map(m_AnimationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_dataPtr);
	memcpy(m_dataPtr.pData, &m_AnimationValues, sizeof(AnimationBuffer));
	DX::g_deviceContext->Unmap(m_AnimationBuffer, 0);
}

void Animation::AnimationCBuffer::UpdateBuffer(PVOID64 data, size_t dataSize)
{
	DX::g_deviceContext->Map(m_AnimationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_dataPtr);
	memcpy(m_dataPtr.pData, data, dataSize);
	DX::g_deviceContext->Unmap(m_AnimationBuffer, 0);
}

void Animation::AnimationCBuffer::SetToShader()
{
	DX::g_deviceContext->VSSetConstantBuffers(2, 1, &m_AnimationBuffer);
}

Animation::SRT::SRT(const MyLibrary::Transform& transform)
{
	using namespace DirectX;
	XMStoreFloat4A(&m_rotationQuaternion, XMQuaternionRotationRollPitchYaw(transform.transform_rotation[0], transform.transform_rotation[1], transform.transform_rotation[2]));
	m_translation = { transform.transform_position[0], transform.transform_position[1], transform.transform_position[2], 1.0f };
	m_scale = { 1.0, 1.0, 1.0, 1.0f };
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

Animation::Skeleton::Skeleton(const MyLibrary::SkeletonFromFile& skeleton)
{
	m_jointCount = skeleton.skeleton_nrOfJoints;
	m_joints = std::make_unique<Animation::Joint[]>(m_jointCount);

	for (int i = 0; i < m_jointCount; i++)
	{
		m_joints[i].parentIndex = skeleton.skeleton_joints[i].parentIndex;
	}
	m_joints[0].parentIndex = -1; // Root does not have a real parent index
	Animation::SetInverseBindPoses(this, &skeleton);
}

Animation::JointPose::JointPose(const SRT& srt)
{
	m_transformation = srt;
}

Animation::AnimationClip::AnimationClip(const MyLibrary::AnimationFromFile& animation, Skeleton* skeleton)
{
	m_skeleton = skeleton;
	m_framerate = 24; //TODO
	uint32_t keyCount = animation.nr_of_keyframes;
	m_frameCount = static_cast<uint16_t>(keyCount);
	m_skeletonPoses = std::make_unique<SkeletonPose[]>(m_frameCount);

	//Init joint poses for skeleton poses
	for (int i = 0; i < m_frameCount; i++)
	{
		m_skeletonPoses[i].m_jointPoses = std::make_unique<JointPose[]>(m_skeleton->m_jointCount);
	}

	for (int j = 0; j < m_skeleton->m_jointCount; j++)
	{
		//for each key
		for (int k = 0; k < keyCount; k++)
		{
			// Review
			SRT srt = SRT(animation.keyframe_transformations[j * animation.nr_of_keyframes + k]);
			m_skeletonPoses[k].m_jointPoses[j].m_transformation = srt;
		}
	}
}

Animation::AnimationClip::~AnimationClip()
{
}
