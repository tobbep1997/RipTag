#include "AnimatedModel.h"
#include "../../Extern.h"

Animation::AnimatedModel::AnimatedModel()
{
}

Animation::AnimatedModel::~AnimatedModel()
{
}

// Computes the frame we're currently on and computes final skinning matrices for gpu skinning
void Animation::AnimatedModel::Update(float deltaTime)
{
	deltaTime /= 16.0; // #todo deltatime
	if (m_isPlaying)
	{
		/// increase local time
		m_currentTime += deltaTime;

		///calc the actual frame index and progression towards the next frame
		int prevIndex = std::floorf(m_currentClip->m_framerate*m_currentTime);
		float progression = std::fmod(m_currentClip->m_framerate*m_currentTime, 1.0f);

		///if we exceeded clips time, set back to 0 ish if we are looping, or stop if we aren't
		if (prevIndex == m_currentClip->m_frameCount - 1) /// -1 because last frame is only used to interpolate towards
		{
			if (m_isLooping)
			{
				m_currentTime = 0.0 + progression;
	
				prevIndex = std::floorf(m_currentClip->m_framerate*m_currentTime);
				progression = std::fmod(m_currentClip->m_framerate*m_currentTime, 1.0f);
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
	else //scrub
	{
		m_scrubIndex + 1 >= m_currentClip->m_frameCount
			? _computeSkinningMatrices(&m_currentClip->m_skeletonPoses[0], &m_currentClip->m_skeletonPoses[1], 0.0)
			: _computeSkinningMatrices(&m_currentClip->m_skeletonPoses[m_scrubIndex], &m_currentClip->m_skeletonPoses[m_scrubIndex + 1], 0.0);
	}

}

void Animation::AnimatedModel::SetPlayingClip(AnimationClip * clip, bool isLooping)
{
	m_currentClip = clip;
	m_currentTime = 0.0f;
}

// #todo BlendIntoClip()

void Animation::AnimatedModel::SetSkeleton(Skeleton * skeleton)
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

// Returns a reference to the skinning matrix vector
const std::vector<DirectX::XMFLOAT4X4A>& Animation::AnimatedModel::GetSkinningMatrices()
{
	return m_skinningMatrices;
}

DirectX::XMMATRIX Animation::_createMatrixFromSRT(const SRT& srt)
{
	using namespace DirectX;

	XMFLOAT4A fScale = ( srt.m_scale);
	XMFLOAT4A fRotation = srt.m_rotationQuaternion;
	XMFLOAT4A fTranslation = srt.m_translation;				   

	auto translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat4A(&fTranslation));
	auto rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4A(&fRotation));
	auto scaleMatrix = XMMatrixScalingFromVector(XMLoadFloat4A(&fScale));
	
	//return XMMatrixMultiply(rotationMatrix, translationMatrix); // #todo test/make sure we dont need this
	return XMMatrixAffineTransformation(XMLoadFloat4A(&fScale), { 0.0, 0.0, 0.0, 1.0 }, XMLoadFloat4A(&fRotation), XMLoadFloat4A(&fTranslation));
}

DirectX::XMMATRIX Animation::_createMatrixFromSRT(const MyLibrary::DecomposedTransform& transform)
{
	using namespace DirectX;

	XMFLOAT4A fScale = {transform.scale.x, transform.scale.y, transform.scale.z, transform.scale.w};
	XMFLOAT4A fRotation = { transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w };
	XMFLOAT4A fTranslation = { transform.translation.x, transform.translation.y, transform.translation.z, transform.translation.w };

	auto t = XMMatrixTranslationFromVector(XMLoadFloat4A(&fTranslation));
	auto r = XMMatrixRotationQuaternion(XMLoadFloat4A(&fRotation));
	auto s = XMMatrixScalingFromVector(XMLoadFloat4A(&fTranslation));

	// #NEWCHECK
	//return XMMatrixMultiply(t, r);

	return XMMatrixAffineTransformation(XMLoadFloat4A(&fScale), { 0.0f, 0.0f, 0.0f, 1.0f }, XMLoadFloat4A(&fRotation), XMLoadFloat4A(&fTranslation));
}

// #todo rename
Animation::AnimationClip* Animation::LoadAndCreateAnimation(std::string file, Skeleton* skeleton)
{
	MyLibrary::Loadera loader;
	auto importedAnimation = loader.readAnimationFileStefan(file, skeleton->m_jointCount);

	std::vector<MyLibrary::DecomposedTransform> transforms;
	for (int i = 0; i < importedAnimation.nr_of_keyframes * skeleton->m_jointCount; i++)
	{
		transforms.push_back(importedAnimation.keyframe_transformations[i]);
	}

	return new Animation::AnimationClip(importedAnimation, skeleton);
}

// #todo rename
Animation::Skeleton* Animation::LoadAndCreateSkeleton(std::string file)
{
	MyLibrary::Loadera loader;
	auto importedSkeleton = loader.readSkeletonFileStefan(file);
	return new Animation::Skeleton(importedSkeleton);
}

// #skinningmatrix
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

// #modelmatrix
void Animation::AnimatedModel::_computeModelMatrices(SkeletonPose* firstPose, SkeletonPose* secondPose, float weight)
{
	using namespace DirectX;
	auto rootJointPose = _interpolateJointPose(&firstPose->m_jointPoses[0], &secondPose->m_jointPoses[0], weight);
	DirectX::XMStoreFloat4x4A(&m_globalMatrices[0], Animation::_createMatrixFromSRT(rootJointPose.m_transformation));

	for (int i = 1; i < m_skeleton->m_jointCount; i++) //start at second joint (first is root, already processed)
	{
		const int16_t parentIndex = m_skeleton->m_joints[i].parentIndex;
		const XMMATRIX parentGlobalMatrix = XMLoadFloat4x4A(&m_globalMatrices[parentIndex]);
		auto jointPose = _interpolateJointPose(&firstPose->m_jointPoses[i], &secondPose->m_jointPoses[i], weight);

		DirectX::XMStoreFloat4x4A(&m_globalMatrices[i], XMMatrixMultiply(Animation::_createMatrixFromSRT(jointPose.m_transformation), parentGlobalMatrix)); // #matrixmultiplication
	}
}

// #convert Transform conversion
Animation::SRT Animation::ConvertTransformToSRT(MyLibrary::Transform transform)
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

// #convert #animationclip AnimationClip conversion
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

// #bindpose
void Animation::SetInverseBindPoses(Animation::Skeleton* mainSkeleton, const MyLibrary::Skeleton* importedSkeleton)
{
	using namespace DirectX;

	std::vector<XMFLOAT4X4A> vec;
	std::vector <MyLibrary::Transform > vec2;
	DirectX::XMStoreFloat4x4A(&mainSkeleton->m_joints[0].m_inverseBindPose, _createMatrixFromSRT(importedSkeleton->joints[0].jointInverseBindPoseTransform));

	for (int i = 1; i < mainSkeleton->m_jointCount; i++)
	{
		const int16_t parentIndex = mainSkeleton->m_joints[i].parentIndex;
		assert(i > parentIndex);

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
	DX::g_deviceContext->VSSetConstantBuffers(2, 1, &m_AnimationBuffer);
}

Animation::SRT::SRT(const MyLibrary::Transform& transform)
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

Animation::SRT::SRT(const MyLibrary::DecomposedTransform& transform)
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

Animation::Skeleton::Skeleton(const MyLibrary::Skeleton& skeleton)
{
	m_jointCount = skeleton.joints.size();
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

Animation::AnimationClip::AnimationClip(const MyLibrary::AnimationFromFileStefan& animation, Skeleton* skeleton)
{
	m_skeleton = skeleton;
	m_framerate = 24; //TODO maybe...
	uint32_t keyCount = animation.nr_of_keyframes;
	m_frameCount = static_cast<uint16_t>(keyCount);
	m_skeletonPoses = std::make_unique<SkeletonPose[]>(m_frameCount);

	//Init joint poses for skeleton poses
	for (int i = 0; i < m_frameCount; i++)
	{
		m_skeletonPoses[i].m_jointPoses = std::make_unique<JointPose[]>(m_skeleton->m_jointCount);
	}

	//for each key
	for (int k = 0; k < keyCount; k++)
	{
		for (int j = 0; j < m_skeleton->m_jointCount; j++)
		{
			// Review
			SRT srt = SRT(animation.keyframe_transformations[k * skeleton->m_jointCount + j]);
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
#pragma endregion AnimationCBufferClass