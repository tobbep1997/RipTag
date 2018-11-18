#include "EnginePCH.h"
#include "LayerMachine.h"
#include "../Model/Meshes/AnimationPlayer.h"

LayerMachine::LayerMachine(Animation::Skeleton* skeleton)
	: m_Skeleton(skeleton)
{
}

LayerMachine::~LayerMachine()
{
	for (auto& layer : m_Layers)
		delete layer.second;
}

void LayerMachine::UpdatePoseWithLayers(Animation::SkeletonPose& mainPose, float deltaTime)
{
	if (m_ActiveLayers.empty())
		return;

	std::vector<Animation::SkeletonPose> finalLayerPoses{};
	for (auto& layer : m_ActiveLayers)
	{
		auto layerPose = layer->UpdateAndGetFinalPose(deltaTime);
		if (layerPose.has_value())
			finalLayerPoses.push_back(std::move(layerPose.value()));
	}
	
	//#CONTINUE
}

void LayerMachine::AddBasicLayer
	(std::string layerName, Animation::AnimationClip* clip, float blendInTime, float blendOutTime)
{
	LayerState* state = new BasicLayer(layerName, clip, blendInTime, blendOutTime, this);
	m_Layers.insert(std::make_pair(layerName, state));
}

void LayerMachine::PopLayer(LayerState* state)
{
	auto it = std::find(m_ActiveLayers.begin(), m_ActiveLayers.end(), state);
	m_ActiveLayers.erase(it);
}

uint16_t LayerMachine::GetSkeletonJointCount()
{
	return m_Skeleton->m_JointCount;
}

LayerState::~LayerState()
{
}

void LayerState::BlendOut()
{
	m_BlendState = BLENDING_OUT;
	if (m_CurrentBlendTime == 0.0f)
		m_CurrentBlendTime = m_BlendOutTime;
}

void LayerState::PopOnFinish()
{
	m_IsLooping = false;
}

void LayerState::Reset()
{
	m_CurrentBlendTime = 0.0f;
	m_CurrentTime = 0.0f;
	m_IsPopped = false;
}

bool LayerState::IsPopped() const
{
	return m_IsPopped;
}

std::pair<uint16_t, float> LayerState::_getIndexAndProgression()
{
	m_CurrentTime = std::fmod(m_CurrentTime, (m_ClipLength - 1) / 24.0);

	///calc the actual frame index and progression towards the next frame
	float indexTime = m_CurrentTime / (1.0 / 24.0);
	int prevIndex = (int)(indexTime);
	float progression = (indexTime)-(float)prevIndex;

	//return values
	return std::make_pair(static_cast<uint16_t>(prevIndex), progression);
}

void LayerState::_updateBlend(float deltaTime)
{
	switch (m_BlendState)
	{
	case NONE:
		break;
	case BLENDING_IN:
		m_CurrentBlendTime += deltaTime;

		if (m_CurrentBlendTime > m_BlendOutTime)
		{
			m_BlendState = NONE;
		}

		break;
	case BLENDING_OUT:
		m_CurrentBlendTime -= deltaTime;

		if (m_CurrentBlendTime < 0.0f)
		{
			m_OwnerMachine->PopLayer(this);
			m_IsPopped = true;
		}

		break;
	}
}

void LayerState::_updateTime(float deltaTime)
{
	m_CurrentTime += deltaTime;
	if (m_IsLooping)
		m_CurrentTime = std::fmod(m_CurrentTime, m_ClipLength);
	else
	{
		if (m_CurrentTime >= m_ClipLength)
			m_OwnerMachine->PopLayer(this);
	}
}

void LayerState::_setLength(float length)
{
	m_ClipLength = length;
}

BasicLayer::~BasicLayer()
{
}

std::optional<Animation::SkeletonPose> BasicLayer::UpdateAndGetFinalPose(float deltaTime)
{
	LayerState::_updateTime(deltaTime);
	LayerState::_updateBlend(deltaTime);
	if (!LayerState::IsPopped())
	{
		auto indexAndProgression = LayerState::_getIndexAndProgression();
		float weight = 1.0f; //todo

		return Animation::AnimationPlayer::_BlendSkeletonPoses
			( &m_Clip->m_SkeletonPoses[indexAndProgression.first]
			, &m_Clip->m_SkeletonPoses[indexAndProgression.first + 1]
			, indexAndProgression.second, m_OwnerMachine->GetSkeletonJointCount());
	}
	else return std::nullopt;
}
