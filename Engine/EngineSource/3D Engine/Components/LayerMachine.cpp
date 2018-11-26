#include "EnginePCH.h"
#include "LayerMachine.h"

LayerMachine::LayerMachine(Animation::Skeleton* skeleton)
	: m_Skeleton(skeleton)
{
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_OFF);

}

LayerMachine::~LayerMachine()
{
	for (auto& layer : m_Layers)
		delete layer.second;
}

void LayerMachine::UpdatePoseWithLayers(Animation::SkeletonPose& mainPose, float deltaTime)
{
	//Get final poses for active layers
	std::vector<Animation::SkeletonPose> finalLayerPoses{};
	for (auto& layer : m_ActiveLayers)
	{
		auto layerPose = layer->UpdateAndGetFinalPose(deltaTime);
		if (layerPose.has_value())
		{
			finalLayerPoses.push_back(std::move(layerPose.value()));
		}
	}

	if (finalLayerPoses.empty())
		return;

	//Create matrix array from layer poses
	std::vector<DirectX::XMMATRIX> layerPosesMultiplied{};
	layerPosesMultiplied.resize(m_Skeleton->m_JointCount);
	{
		//Do first layer
		for (int joint = 0; joint < m_Skeleton->m_JointCount; joint++)
		{
			layerPosesMultiplied[joint] = Animation::AnimationPlayer::_CreateMatrixFromSRT(finalLayerPoses[0].m_JointPoses[joint].m_Transformation);
		}
		
		//Do all other layers
		for (size_t layer = 1; layer < finalLayerPoses.size(); layer++)
		{
			for (size_t joint = 0; joint < m_Skeleton->m_JointCount; joint++)
			{
				auto thisPoseMatrix = Animation::AnimationPlayer::_CreateMatrixFromSRT(finalLayerPoses[layer].m_JointPoses[joint].m_Transformation);
				layerPosesMultiplied[joint] = DirectX::XMMatrixMultiply(thisPoseMatrix, layerPosesMultiplied[joint]);
			}
		}
	}
	
	//Combine layer matrices with the main pose
	for (int joint = 0; joint < m_Skeleton->m_JointCount; joint++)
	{
		mainPose.m_JointPoses[joint] = Animation::AnimationPlayer::_GetAdditivePose(mainPose.m_JointPoses[joint], layerPosesMultiplied[joint]);
	}

}

BasicLayer* LayerMachine::AddBasicLayer(std::string layerName, Animation::AnimationClip* clip, float blendInTime, float blendOutTime)
{
	BasicLayer* state = new BasicLayer(layerName, clip, blendInTime, blendOutTime, this);
	m_Layers.insert(std::make_pair(layerName, static_cast<LayerState*>(state)));
	return state;
}

void LayerMachine::ActivateLayer(std::string layerName)
{
	auto layer = m_Layers.at(layerName);
	layer->Reset();
	layer->SetEndlessLoop();
	m_ActiveLayers.push_back(layer);
}

void LayerMachine::ActivateLayer( std::string layerName, float loopCount)
{
	auto layer = m_Layers.at(layerName);
	layer->SetPlayTime(loopCount);
	layer->PopOnFinish();
	if (!_mildResetIfActive(layer))
	{
		layer->Reset();
		m_ActiveLayers.push_back(layer);
	}
}

Pose1DLayer* LayerMachine::Add1DPoseLayer(std::string layerName, float* driver, float min, float max, std::vector<std::pair<Animation::SkeletonPose*, float>> poses)
{
	auto layer = new Pose1DLayer(layerName, driver, min, max, poses, this);
	m_Layers.insert(std::make_pair(layerName, static_cast<LayerState*>(layer)));
	return layer;
}

void LayerMachine::PopLayer(LayerState* state)
{
	auto it = std::find(m_ActiveLayers.begin(), m_ActiveLayers.end(), state);
	if (it != std::end(m_ActiveLayers)) 
	{
		m_ActiveLayers.erase(it);
		//m_ActiveLayers.erase(it);
	}
}

void LayerMachine::PopLayer(std::string layer)
{
	auto pLayer = m_Layers.at(layer);
	PopLayer(pLayer);
}

uint16_t LayerMachine::GetSkeletonJointCount()
{
	return m_Skeleton->m_JointCount;
}

bool LayerMachine::_mildResetIfActive(LayerState* layer)
{
	auto it = std::find(m_ActiveLayers.begin(), m_ActiveLayers.end(), layer);
	if (it != std::end(m_ActiveLayers))
	{
		(*it)->MildReset();
		return true;
	}
	else return false;
}

LayerState::~LayerState()
{
}

void LayerState::BlendOut()
{
	m_BlendState = BLENDING_OUT;
	m_CurrentBlendTime = m_BlendOutTime;
}

void LayerState::PopOnFinish()
{
	m_IsEndlesslyLooping = false;
}

void LayerState::SetEndlessLoop()
{
	m_IsEndlesslyLooping = true;
}

void LayerState::Reset()
{
	m_CurrentBlendTime = 0.0f;
	m_CurrentTime = 0.0f;
	m_BlendState = NONE;
	m_IsPopped = false;
}

void LayerState::MildReset()
{
	m_IsPopped = false;
}

bool LayerState::IsPopped() const
{
	return m_IsPopped;
}

std::pair<uint16_t, float> LayerState::_getIndexAndProgression()
{
	m_CurrentTime = std::fmod(m_CurrentTime, (m_ClipLength));

	///calc the actual frame index and progression towards the next frame
	float indexTime = m_CurrentTime / (1.0 / 24.0);
	int prevIndex = (int)(indexTime);
	float progression = (indexTime)-(float)prevIndex;

	//return values
	return std::make_pair(static_cast<uint16_t>(prevIndex), progression);
}

LayerState::BLEND_STATE LayerState::GetState()
{
	return m_BlendState;
}

void LayerState::_updateBlend(float deltaTime)
{
	switch (m_BlendState)
	{
	case NONE:
		break;
	case BLENDING_IN:
	{
		m_CurrentBlendTime += deltaTime;

		if (m_CurrentBlendTime > m_BlendOutTime)
		{
			m_BlendState = NONE;
		}

		break;
	}
	case BLENDING_OUT:
	{
		m_CurrentBlendTime -= deltaTime;

		if (m_CurrentBlendTime <= 0.0f)
		{
			m_OwnerMachine->PopLayer(this);
			m_IsPopped = true;
		}

		break;
	}

	}
}

void LayerState::_updateTime(float deltaTime)
{
	m_CurrentTime += deltaTime;

	if (!m_IsEndlesslyLooping)
	{
		m_PlayTime -= deltaTime;

		if (m_PlayTime <= m_BlendOutTime 
		 && m_BlendState != BLENDING_OUT)
		{
			BlendOut();
		}
	}
	else
	{	
		m_CurrentTime = std::fmod(m_CurrentTime, m_ClipLength);
	}
}

void LayerState::_setLength(float length)
{
	m_ClipLength = length;
}

void LayerState::SetPlayTime(float loopCount)
{
	m_PlayTime = loopCount * (m_ClipLength);
}

BasicLayer::BasicLayer(std::string name, Animation::AnimationClip* clip, float blendInTime, float blendOutTime, LayerMachine* owner)
	: LayerState(name, blendInTime, blendOutTime, owner), m_Clip(clip)
{
	LayerState::_setLength((m_Clip->m_FrameCount-1) / 24.0f);
}

BasicLayer::~BasicLayer()
{
}

std::optional<Animation::SkeletonPose> BasicLayer::UpdateAndGetFinalPose(float deltaTime)
{
	_updateDriverWeight();
	deltaTime *= m_CurrentDriverWeight;

	//if (m_Muted)
	//	m_CurrentDriverWeight = 0.0f;

	LayerState::_updateTime(deltaTime);
	LayerState::_updateBlend(deltaTime);
	if (!LayerState::IsPopped())
	{
		auto indexAndProgression = LayerState::_getIndexAndProgression();
		float weight = 1.0f; //todo
		auto currentState = LayerState::GetState();
		if (currentState != NONE)
		{
			if (currentState == BLENDING_OUT)
			{
				weight = m_CurrentBlendTime / m_BlendOutTime;
			}
		}
		auto pose = Animation::AnimationPlayer::_BlendSkeletonPoses
			( &m_Clip->m_SkeletonPoses[indexAndProgression.first]
			, &m_Clip->m_SkeletonPoses[indexAndProgression.first + 1]
			, indexAndProgression.second, m_OwnerMachine->GetSkeletonJointCount());

		weight *= m_CurrentDriverWeight;
		if (weight >= 0.0f || weight < 0.9999f)
		{
			Animation::AnimationPlayer::_ScalePose(&pose, weight, m_OwnerMachine->GetSkeletonJointCount());
		}

		return pose;
	}
	else return std::nullopt;
}

void BasicLayer::MakeDriven(float * driver, float min, float max, bool affectsSpeed /*= true*/)
{
	m_Driver = driver;
	m_DriverMin = min;
	m_DriverMax = max;
	m_DriverAffectsSpeed = affectsSpeed;
}

void BasicLayer::_updateDriverWeight()
{
	m_CurrentDriverWeight = m_Driver
		? (*m_Driver - m_DriverMin) / (m_DriverMax - m_DriverMin)
		: 1.0f;

	m_CurrentDriverWeight = std::clamp(m_CurrentDriverWeight, 0.0f, 1.0f);
}

PoseLayer::PoseLayer(std::string name, Animation::SkeletonPose* pose, LayerMachine* owner)
	: LayerState(name, 0.0f, 0.0f, owner)
{
	m_Pose = new Animation::SkeletonPose();
	*m_Pose = Animation::MakeSkeletonPose(*pose, m_OwnerMachine->GetSkeletonJointCount());
}

PoseLayer::~PoseLayer()
{
	if (m_Pose)
		delete m_Pose;
}

std::optional<Animation::SkeletonPose> PoseLayer::UpdateAndGetFinalPose(float deltaTime)
{
	_updateDriverWeight();

	if (!LayerState::IsPopped())
	{
		if (m_CurrentDriverWeight >= 0.0f || m_CurrentDriverWeight < 0.9999f)
		{
			auto pose = Animation::MakeSkeletonPose(*m_Pose, m_OwnerMachine->GetSkeletonJointCount());
			Animation::AnimationPlayer::_ScalePose(&pose, m_CurrentDriverWeight, m_OwnerMachine->GetSkeletonJointCount());
			return std::move(pose);
		}
		else return Animation::MakeSkeletonPose(m_CurrentDriverWeight, m_OwnerMachine->GetSkeletonJointCount());
	}

	return std::nullopt;
}

void PoseLayer::MakeDriven(float* driver, float min, float max)
{
	m_Driver = driver;
	m_DriverMin = min;
	m_DriverMax = max;
}

void PoseLayer::_updateDriverWeight()
{
	m_CurrentDriverWeight = m_Driver
		? (*m_Driver - m_DriverMin) / (m_DriverMax - m_DriverMin)
		: 1.0f;

	m_CurrentDriverWeight = std::clamp(m_CurrentDriverWeight, 0.0f, 1.0f);
}

Pose1DLayer::Pose1DLayer(std::string name, float * driver, float min, float max, std::vector<std::pair<Animation::SkeletonPose*, float>> poses, LayerMachine * owner)
	: LayerState(name, 0.0f, 0.0f, owner), m_Driver(driver), m_DriverMin(min), m_DriverMax(max)
{
	for (auto& pose : poses)
	{
		std::pair<Animation::SkeletonPose*, float> pair{};
		pair.first = new Animation::SkeletonPose;
		*pair.first = Animation::MakeSkeletonPose(*pose.first, m_OwnerMachine->GetSkeletonJointCount());
		pair.second = pose.second;
		m_Poses.push_back(pair);
	}
}

Pose1DLayer::~Pose1DLayer()
{
	for (auto& pair : m_Poses)
	{
		if (pair.first)
		{
			delete pair.first;
		}
	}
}

std::optional<Animation::SkeletonPose> Pose1DLayer::UpdateAndGetFinalPose(float deltaTime)
{
	auto posesAndWeight = GetPosesAndWeight(deltaTime);

	if (posesAndWeight.firstPose)
		return Animation::AnimationPlayer::_BlendSkeletonPoses(
			posesAndWeight.firstPose,
			posesAndWeight.secondPose,
			posesAndWeight.weight,
			m_OwnerMachine->GetSkeletonJointCount());

	else return Animation::MakeSkeletonPose(
		*posesAndWeight.secondPose,
		m_OwnerMachine->GetSkeletonJointCount());
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

Pose1DLayer::PosesAndWeight Pose1DLayer::GetPosesAndWeight(float deltaTime)
{
	float driverDelta = std::clamp(std::fabs(m_LastDriver - *m_Driver), .1f, 5.f);
	float smoothDriver = lerp(m_LastDriver, *m_Driver, driverDelta * deltaTime);
	m_LastDriver = smoothDriver;

	auto secondPoseIt = std::find_if(m_Poses.begin(), m_Poses.end(),
		[&](const auto& pair) { return pair.second >= smoothDriver; });

	if (secondPoseIt == std::end(m_Poses))
		return { nullptr, (m_Poses.end() - 1)->first, 1.0f };

	else if (secondPoseIt == std::begin(m_Poses))
		return { nullptr, m_Poses.begin()->first, 1.0f };

	else
	{
		auto firstPoseIt = secondPoseIt - 1;
		const float firstFloat = firstPoseIt->second;
		const float secondFloat = secondPoseIt->second;

		assert(firstFloat <= smoothDriver && secondFloat >= smoothDriver);

		float weight = (smoothDriver - firstFloat) / (secondFloat - firstFloat);
		m_LastDriver = smoothDriver;

		return { firstPoseIt->first, secondPoseIt->first, weight };
	}
}
