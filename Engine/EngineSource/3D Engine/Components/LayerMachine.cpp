#include "EnginePCH.h"
#include "LayerMachine.h"


LayerMachine::LayerMachine()
{
}


LayerMachine::~LayerMachine()
{
}

void LayerMachine::PopLayer(LayerState* state)
{
	auto it = std::find(m_ActiveLayers.begin(), m_ActiveLayers.end(), state);
	m_ActiveLayers.erase(it);
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

void LayerState::_getIndexAndProgression()
{
	float actualTime = m_CurrentTime / (1.0 / 24.0);
	int prevIndex = (int)(actualTime);
	float progression = (actualTime)-(float)prevIndex;

	//return values
	//return std::make_pair(static_cast<uint16_t>(prevIndex), progression); //#todo
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
		LayerState::_getIndexAndProgression();
		float weight = 1.0f; //todo
	}
	return std::nullopt;
}
