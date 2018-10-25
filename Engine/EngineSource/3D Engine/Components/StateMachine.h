#pragma once
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include "../Model/Meshes/AnimatedModel.h"

namespace SM
{
#pragma region AnimationMachine
	enum COMPARISON_TYPE
	{
		COMPARISON_EQUAL,
		COMPARISON_NOT_EQUAL,
		COMPARISON_LESS_THAN,
		COMPARISON_GREATER_THAN
	};

	class TransitionBase
	{
	public:
		
		~TransitionBase() {};
		virtual bool Evaluate() const = 0;

	};
	typedef std::unique_ptr<TransitionBase> UniqueTransition;

	template <typename T>
	class TransitionCondition : public TransitionBase
	{
	public:
		TransitionCondition(T* reference, const T value, const COMPARISON_TYPE type);
		~TransitionCondition() {};

		bool Evaluate() const override;
	private:
		T* m_Reference = nullptr;
		T m_Value = 0;
		COMPARISON_TYPE m_Type;
	};

	template <typename T>
	bool SM::TransitionCondition<T>::Evaluate() const
	{
		switch (m_Type)
		{
		case SM::COMPARISON_EQUAL:
			return *m_Reference == m_Value;
			break;
		case SM::COMPARISON_NOT_EQUAL:
			return *m_Reference != m_Value;
			break;
		case SM::COMPARISON_LESS_THAN:
			return *m_Reference < m_Value;
			break;
		case SM::COMPARISON_GREATER_THAN:
			return *m_Reference > m_Value;
			break;
		default:
			return false;
			break;
		}
	}

	template <typename T>
	SM::TransitionCondition<T>::TransitionCondition(T* reference, const T value, const COMPARISON_TYPE type)
		: m_Reference(reference), m_Value(value), m_Type(type)
	{}

	typedef std::vector<std::unique_ptr<TransitionBase>> TransitionVector;
	typedef std::unordered_map<std::string, TransitionVector> TransitionMap;
	
	class AnimationState;

	struct OutState 
	{
		AnimationState* state;
		TransitionVector transitions;

		template <class T>
		void AddTransition(T* ref, const T value, COMPARISON_TYPE type);
	};

	template <class T>
	void SM::OutState::AddTransition(T* ref, const T value, COMPARISON_TYPE type)
	{
		transitions.push_back(std::make_unique<TransitionCondition<T>>(ref, value, type));
	}

	class StateVisitorBase;

	class AnimationState
	{
	public:
		AnimationState(std::string name);
		~AnimationState();

		OutState& AddOutState(AnimationState* outState);
		//Returns true if all transition conditions
		//for the given out state are met; false otherwise.
		std::optional<AnimationState*> EvaluateAllTransitions(std::string key);
		std::string GetName();
		AnimationState(const AnimationState& other) = delete;
		virtual void recieveStateVisitor(StateVisitorBase& visitor) = 0;
		bool operator=(const std::string name) { return m_Name == name; }
	private:
		std::string m_Name = "";

		std::unordered_map<std::string, OutState> m_OutStates;
	};

	class BlendSpace1D;

	class StateVisitorBase{
	public:
		virtual void dispatch(BlendSpace1D& state) = 0;
	};

	class BlendSpace1D : public AnimationState
	{
	public:
		struct BlendSpaceClipData
		{
			Animation::SharedAnimation clip;
			float location;
		};

		BlendSpace1D(std::string name, float* blendSpaceDriver, float min, float max)
			: m_Current(blendSpaceDriver), m_Min(min), m_Max(max), AnimationState(name)
		{}

		//Assumes nodes are sorted from lowest to highest
		void AddBlendNodes(const std::vector<BlendSpaceClipData>& nodes);

		void recieveStateVisitor(StateVisitorBase& visitor) override {
			visitor.dispatch(*this);
		}

		std::pair<Animation::SharedAnimation, Animation::SharedAnimation> CalculateCurrentClips();
	private:
		std::vector<BlendSpaceClipData> m_Clips;
		float m_Min = 0.0f;
		float m_Max = 1.0f;
		float* m_Current = nullptr;
	};

#pragma region "Visitors"
	class StateVisitor : public StateVisitorBase{
	public:
		StateVisitor(Animation::AnimatedModel* model) : m_AnimatedModel(model)
		{}
		virtual void dispatch(BlendSpace1D& state) override {
			auto clips = state.CalculateCurrentClips();
			
			if (!m_AnimatedModel)
				return;
			// #todo
			m_AnimatedModel->SetPlayingClip(clips.first.get(), true);
		}
	private:
		Animation::AnimatedModel* m_AnimatedModel = nullptr;
	};
#pragma endregion "Visitors"

	class AnimationStateMachine
	{
	public:
		AnimationStateMachine(size_t size);
		~AnimationStateMachine();

		//Add a new animation state.
		//Returns a pointer to the new state.
		AnimationState* AddState(std::string name);
		BlendSpace1D* AddBlendSpace1DState(std::string name, float* blendSpaceDriver, float min, float max);

		void SetState(std::string stateName);
		void SetModel(Animation::AnimatedModel* model);
		AnimationState& GetCurrentState();
	private:
		//The animated model to set the clip to when we enter a state
		Animation::AnimatedModel* m_AnimatedModel;

		//The current state
		AnimationState* m_CurrentState = nullptr;

		//The states of this machine
		std::vector<AnimationState*> m_States;
	};

#pragma endregion AnimationMachine
//#pragma region TemplateStateMachine
//	enum MACHINE_STATE
//	{
//		MACHINE_IS_TRANSITIONING,
//		MACHINE_IS_IN_STATE
//	};
//
//	template <class StateData, class TransitionData>
//	class State
//	{
//	public:
//		State(std::string name, std::vector<std::pair<State*, TransitionData>> outStates);
//		State(std::string name, StateData stateData);
//		~State();
//		std::string GetName();
//		bool HasOutState(std::string state);
//		bool operator==(const std::string& name);
//		StateData& GetStateData();
//		TransitionData* GetTransitionData(std::string outState);
//		void SetStateData(StateData data);
//		void AddOutState(std::pair<State*, TransitionData> outState);
//
//	private:
//		std::string m_StateName;
//		std::vector<std::pair<State*, TransitionData>> m_OutStates;
//		StateData m_StateData;
//	};
//
//	template <class StateData, class TransitionData>
//	void SM::State<StateData, TransitionData>::AddOutState(std::pair<State*, TransitionData> outState)
//	{
//		m_OutStates.push_back(outState);
//	}
//
//	template <class StateData, class TransitionData>
//	class StateMachine
//	{
//	public:
//		StateMachine(size_t maxStates);
//		~StateMachine();
//
//		std::string GetCurrentState();
//		std::optional<std::pair<const StateData*, const TransitionData*>> TryGoTo(std::string state);
//		void AddState(std::string name, StateData stateData);
//		void AddOutStates(std::string state, std::vector<std::pair<std::string, TransitionData>> outStates);
//	private:
//		SM::State<StateData, TransitionData>* m_CurrentState = nullptr;
//		std::vector<SM::State<StateData, TransitionData>> m_States;
//	};
//
//	template <class StateData, class TransitionData>
//	void SM::StateMachine<StateData, TransitionData>::AddOutStates(std::string state, std::vector<std::pair<std::string, TransitionData>> outStates)
//	{
//		auto iter = std::find(std::begin(m_States), std::end(m_States), state);
//		if (iter != std::end(m_States))
//		{
//			for (auto& p : outStates)
//			{
//				auto outStateIter = std::find(std::begin(m_States), std::end(m_States), p.first);
//
//				if (outStateIter != std::end(m_States))
//					(*iter).AddOutState(std::make_pair(&*outStateIter, p.second));
//			}
//		}
//	}
//
//	template <class StateData, class TransitionData>
//	StateMachine<StateData, TransitionData>::StateMachine(size_t maxStates)
//	{
//		m_States.reserve(maxStates);
//	}
//
//	template <class StateData, class TransitionData>
//	State<StateData, TransitionData>::~State()
//	{}
//
//	template <class StateData, class TransitionData>
//	StateMachine<StateData, TransitionData>::~StateMachine()
//	{
//	}
//
//	template <class StateData, class TransitionData>
//	std::string StateMachine<StateData, TransitionData>::GetCurrentState()
//	{
//		return m_CurrentState->GetName();
//	}
//
//	// Returns the related TransitionData towards passed state if 
//	// go to is successful, nullptr otherwise
//	template <class StateData, class TransitionData>
//	std::optional < std::pair<const StateData*, const TransitionData*>> StateMachine<StateData, TransitionData>::TryGoTo(std::string state)
//	{
//		if (m_CurrentState->HasOutState(state))
//		{
//			auto transitionData = m_CurrentState->GetTransitionData(state);
//			m_CurrentState = &(*std::find(m_States.begin(), m_States.end(), state));
//			return std::make_pair(&m_CurrentState->GetStateData(), transitionData);
//		}
//		else return {};
//	}
//
//	template <class StateData, class TransitionData>
//	SM::State<StateData, TransitionData>::State(std::string name, std::vector<std::pair<State*, TransitionData>> outStates) : m_StateName(name)
//	{
//		std::copy(outStates.begin(), outStates.end(), std::back_inserter(m_OutStates));
//	}
//
//	template <class StateData, class TransitionData>
//	SM::State<StateData, TransitionData>::State(std::string name, StateData stateData) : m_StateName(name), m_StateData(stateData)
//	{}
//
//	// Gets the associated StateData for this state
//	template <class StateData, class TransitionData>
//	StateData& State<StateData, TransitionData>::GetStateData()
//	{
//		return m_StateData;
//	}
//
//	template <class StateData, class TransitionData>
//	TransitionData* State<StateData, TransitionData>::GetTransitionData(std::string outState)
//	{
//		auto iter = std::find_if(m_OutStates.begin(), m_OutStates.end(),
//			[&outState](std::pair<SM::State<StateData, TransitionData>*, TransitionData>& element) { return *(element.first) == outState; });
//
//		if (iter != m_OutStates.end())
//			return &((*iter).second);
//		else return nullptr;
//	}
//
//	template<class StateData, class TransitionData>
//	void State<StateData, TransitionData>::SetStateData(StateData data)
//	{
//		m_StateData = data;
//	}
//
//	template <class StateData, class TransitionData>
//	std::string State<StateData, TransitionData>::GetName()
//	{
//		return m_StateName;
//	}
//
//	template <class StateData, class TransitionData>
//	bool State<StateData, TransitionData>::HasOutState(std::string state)
//	{
//		auto iter = std::find_if(m_OutStates.begin(), m_OutStates.end(),
//			[&state](std::pair<SM::State<StateData, TransitionData>*, TransitionData>& element) { return *(element.first) == state; });
//		if (iter != m_OutStates.end())
//			return true;
//		else return false;
//	}
//
//	template <class StateData, class TransitionData>
//	bool State<StateData, TransitionData>::operator==(const std::string& name)
//	{
//		return m_StateName == name;
//	}
//
//	template <class StateData, class TransitionData>
//	void StateMachine<StateData, TransitionData>::AddState(std::string name, StateData stateData)
//	{
//		m_States.push_back(SM::State<StateData, TransitionData>(name, stateData));
//		if (m_States.size() == 1)
//			m_CurrentState = &m_States[0];
//	}
//#pragma endregion TemplateStateMachine
//
}
