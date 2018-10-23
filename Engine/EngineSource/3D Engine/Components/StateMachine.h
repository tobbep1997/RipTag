#pragma once
#include <string>
#include <vector>
namespace SMachine
{
	enum MACHINE_STATE
	{
		MACHINE_IS_TRANSITIONING,
		MACHINE_IS_IN_STATE
	};
	template <class StateData, class TransitionData>
	class State
	{
	public:
		State(std::string name, std::vector<std::pair<State*, TransitionData>> outStates);
		State(std::string name, StateData stateData);
		~State();
		std::string GetName();
		bool HasOutState(std::string state);
		bool operator==(const std::string& name);
		StateData& GetStateData();
		TransitionData* GetTransitionData(std::string outState);
		void SetStateData(StateData data);
		void AddOutState(std::pair<State*, TransitionData> outState);

	private:
		std::string m_StateName;
		std::vector<std::pair<State*, TransitionData>> m_OutStates;
		StateData m_StateData;
	};

	template <class StateData, class TransitionData>
	void SMachine::State<StateData, TransitionData>::AddOutState(std::pair<State*, TransitionData> outState)
	{
		m_OutStates.push_back(outState);
	}

	template <class StateData, class TransitionData>
	class StateMachine
	{
	public:
		StateMachine(size_t maxStates);
		~StateMachine();

		std::string GetCurrentState();
		TransitionData* TryGoTo(std::string state);
		void AddState(std::string name, StateData stateData);
		void AddOutStates(std::string state, std::vector<std::pair<std::string, TransitionData>> outStates);
	private:
		SMachine::State<StateData, TransitionData>* m_CurrentState = nullptr;
		std::vector<SMachine::State<StateData, TransitionData>> m_States;
	};

	template <class StateData, class TransitionData>
	void SMachine::StateMachine<StateData, TransitionData>::AddOutStates(std::string state, std::vector<std::pair<std::string, TransitionData>> outStates)
	{
		auto iter = std::find(std::begin(m_States), std::end(m_States), state);
		if (iter != std::end(m_States))
		{
			for (auto& p : outStates)
			{
				auto outStateIter = std::find(std::begin(m_States), std::end(m_States), p.first);

				if (outStateIter != std::end(m_States))
					(*iter).AddOutState(std::make_pair(&*outStateIter, p.second));
			}
		}
	}

	template <class StateData, class TransitionData>
	StateMachine<StateData, TransitionData>::StateMachine(size_t maxStates)
	{
		m_States.reserve(maxStates);
	}

	template <class StateData, class TransitionData>
	State<StateData, TransitionData>::~State()
	{}

	template <class StateData, class TransitionData>
	StateMachine<StateData, TransitionData>::~StateMachine()
	{
	}

	template <class StateData, class TransitionData>
	std::string StateMachine<StateData, TransitionData>::GetCurrentState()
	{
		return m_CurrentState->GetName();
	}

	// Returns the related TransitionData towards passed state if 
	// go to is successful, nullptr otherwise
	template <class StateData, class TransitionData>
	TransitionData* StateMachine<StateData, TransitionData>::TryGoTo(std::string state)
	{
		auto iter = std::find(m_States.begin(), m_States.end(), state);
		if (iter != m_States.end())
		{
			auto transitionData = m_CurrentState->GetTransitionData(state);
			m_CurrentState = &(*iter);
			return transitionData;
		}
		else return nullptr;
	}

	template <class StateData, class TransitionData>
	SMachine::State<StateData, TransitionData>::State(std::string name, std::vector<std::pair<State*, TransitionData>> outStates) : m_StateName(name)
	{
		std::copy(outStates.begin(), outStates.end(), std::back_inserter(m_OutStates));
	}

	template <class StateData, class TransitionData>
	SMachine::State<StateData, TransitionData>::State(std::string name, StateData stateData) : m_StateName(name), m_StateData(stateData)
	{}

	// Gets the associated StateData for this state
	template <class StateData, class TransitionData>
	StateData& State<StateData, TransitionData>::GetStateData()
	{
		return m_StateData;
	}

	template <class StateData, class TransitionData>
	TransitionData* State<StateData, TransitionData>::GetTransitionData(std::string outState)
	{
		auto iter = std::find_if(m_OutStates.begin(), m_OutStates.end(),
			[&outState](std::pair<SMachine::State<StateData, TransitionData>*, TransitionData>& element) { return *(element.first) == outState; });

		if (iter != m_OutStates.end())
			return &((*iter).second);
		else return nullptr;
	}

	template<class StateData, class TransitionData>
	void State<StateData, TransitionData>::SetStateData(StateData data)
	{
		m_StateData = data;
	}

	template <class StateData, class TransitionData>
	std::string State<StateData, TransitionData>::GetName()
	{
		return m_StateName;
	}

	template <class StateData, class TransitionData>
	bool State<StateData, TransitionData>::HasOutState(std::string state)
	{
		if (std::find(std::begin(m_OutStates), std::end(m_OutStates), state) != std::end(m_OutStates))
			return true;
		else return false;
	}

	template <class StateData, class TransitionData>
	bool State<StateData, TransitionData>::operator==(const std::string& name)
	{
		return m_StateName == name;
	}

	template <class StateData, class TransitionData>
	void StateMachine<StateData, TransitionData>::AddState(std::string name, StateData stateData)
	{
		m_States.push_back(SMachine::State<StateData, TransitionData>(name, stateData));
		if (m_States.size() == 1)
			m_CurrentState = &m_States[0];
	}

}
