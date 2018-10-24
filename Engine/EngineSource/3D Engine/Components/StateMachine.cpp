#include "StateMachine.h"
#include <assert.h>
#include <algorithm>
namespace SM
{

	AnimationState::AnimationState(std::string name, Animation::SharedAnimation clip)
		: m_Name(name), m_AnimationClip(clip)
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

	AnimationStateMachine::AnimationStateMachine(size_t size)
	{
		m_States.reserve(size);
	}

	AnimationStateMachine::~AnimationStateMachine()
	{
		for (auto& i : m_States)
			delete i;
	}

	AnimationState* AnimationStateMachine::AddState(std::string name, Animation::SharedAnimation clip)
	{
		auto p = new AnimationState(name, clip);
		m_States.push_back(p);
		return p;
	}

}