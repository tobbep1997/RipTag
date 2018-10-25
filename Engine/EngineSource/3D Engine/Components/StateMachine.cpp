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

	AnimationState* AnimationStateMachine::AddState(std::string name)
	{
		//auto p = new AnimationState(name, clip);
		//m_States.push_back(p);
		return nullptr;
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

	SM::AnimationState& AnimationStateMachine::GetCurrentState()
	{
		return *m_CurrentState;
	}

	void BlendSpace1D::AddBlendNodes(const std::vector<BlendSpaceClipData>& nodes)
	{
		std::copy(nodes.begin(), nodes.end(), std::back_inserter(m_Clips));
	}

	std::pair<Animation::SharedAnimation, Animation::SharedAnimation> BlendSpace1D::CalculateCurrentClips()
	{
		//#todo check and get weight
		auto it = std::find_if(m_Clips.begin(), m_Clips.end(),
			[&](const auto& data) {return data.location >= *m_Current; });

		if (it != m_Clips.end())
		{
			std::cout << it->location << std::endl;
			return it == m_Clips.begin()
				? std::make_pair(it->clip, nullptr)
				: std::make_pair((it - 1)->clip, it->clip);
		}
		else return { nullptr, nullptr };
	}
}