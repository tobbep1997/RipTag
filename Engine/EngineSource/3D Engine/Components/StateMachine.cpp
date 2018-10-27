#include "StateMachine.h"
#include <assert.h>
#include <algorithm>
#include <iostream>
namespace SM
{

	AnimationState::AnimationState(std::string name)
		: m_Name(name)
	{}

	AnimationState::~AnimationState()
	{}

	SM::OutState& AnimationState::AddOutState(AnimationState* outState)
	{
		OutState outstate;
		outstate.state = outState;

		m_OutStates.insert({ outState->m_Name, std::move(outstate) });
		return m_OutStates.at(outState->m_Name);
	}

	std::optional<AnimationState*> AnimationState::EvaluateAllTransitions(std::string key)
{
		auto& t = m_OutStates.at(key);
		if (std::all_of(t.transitions.begin(), t.transitions.end(), [](const UniqueTransition& elem) {return elem->Evaluate(); }))
			return { t.state };
		else return {};
		return {};
	}

	std::string AnimationState::GetName()
	{
		return m_Name;
	}

	AnimationStateMachine::AnimationStateMachine(size_t size)
	{
		m_States.reserve(size);
	}

	AnimationStateMachine::~AnimationStateMachine()
	{
		for (auto& i : m_States)
			delete i;
	}

	SM::BlendSpace1D* AnimationStateMachine::AddBlendSpace1DState(std::string name, float* blendSpaceDriver, float min, float max)
	{
		BlendSpace1D* state = new BlendSpace1D(name, blendSpaceDriver, min, max);
		m_States.push_back(static_cast<AnimationState*>(state));
		return state;
	}

	void AnimationStateMachine::SetState(std::string stateName)
	{
		auto it = std::find_if(m_States.begin(), m_States.end(), 
			[&](const auto& p) { return p->GetName() == stateName; });
		if (it != m_States.end())
			m_CurrentState = *it;
	}

	void AnimationStateMachine::SetModel(Animation::AnimatedModel* model)
	{
		m_AnimatedModel = model;
	}

	SM::AnimationState& AnimationStateMachine::GetCurrentState()
	{
		return *m_CurrentState;
	}

	void BlendSpace1D::AddBlendNodes(const std::vector<BlendSpaceClipData> nodes)
	{
		std::copy(nodes.begin(), nodes.end(), std::back_inserter(m_Clips));
	}

	BlendSpace1D::Current1DStateData BlendSpace1D::CalculateCurrentClips()
	{
		//#todo check and get weight
		auto it = std::find_if(m_Clips.begin(), m_Clips.end(),
			[&](const auto& data) {return data.location >= *m_Current; });

		if (it != m_Clips.end())
		{
			Current1DStateData data;
			std::cout << it->location << std::endl;
			it == m_Clips.begin()
				? std::make_pair(it->clip, nullptr)
				: std::make_pair((it - 1)->clip, it->clip);

			if (it == m_Clips.begin())
			{
				data.first = it->clip;
			}
			else
			{
				data.first = (it-1)->clip;
				data.second = it->clip;
				data.weight = (*m_Current - (it - 1)->location) / (it->location - (it - 1)->location);
			}
			return data;
		}
		else if (m_Clips.back().location < *m_Current)
		{
			Current1DStateData data;
			data.first = (m_Clips.rbegin() + 1)->clip;
			data.second = m_Clips.rbegin()->clip;
			data.weight = 1.0f;
			return data;
		}
		else return { nullptr, nullptr, 0.0f};
	}

	void BlendSpace2D::AddRow(float y, std::vector<BlendSpaceClipData2D>&& nodes)
	{
#ifdef _DEBUG
		auto it = std::find_if(m_Rows.begin(), m_Rows.end(), [&y](const auto& e) {return e.first == y; });
		assert(it == m_Rows.end());
#endif
		m_Rows.push_back(std::make_pair(y, std::move(nodes)));
	}

	BlendSpace2D::Current2DStateData BlendSpace2D::CalculateCurrentClips()
	{ //#todo check end()
		auto topRowIt = std::find_if(m_Rows.begin(), m_Rows.end(),
			[&](const auto& row) {return row.first >= *m_Current_Y; });

		if (topRowIt == std::begin(m_Rows))
		{
			// do only first row blend
			// (no lerp along Y)
		}

		auto bottomRowIt = topRowIt - 1;

		auto rightTopIt = std::find_if(topRowIt->second.begin(), topRowIt->second.end(),
			[&](const auto& data) {return data.locationX >= *m_Current_X; });

		if (rightTopIt == std::begin(topRowIt->second))
		{
			// no blend necessary on X, use first
		}

		auto leftTopIt = rightTopIt - 1;

		return Current2DStateData();
	}
}