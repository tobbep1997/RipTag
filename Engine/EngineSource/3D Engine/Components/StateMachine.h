#pragma once
#include <string>
#include <vector>
#include <functional>
#include <optional>

#pragma region "FwdDec"
namespace Animation
{
	struct AnimationClip;
	class AnimatedModel;
}
#pragma endregion "FwdDec"

namespace SM
{
#pragma region "AnimationMachine"

#pragma region "Transition"
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
#pragma endregion "Transition"

#pragma region "AnimationState"
	class StateVisitorBase;

	enum STATE_TYPE
	{
		LOOPING,
		BLEND_1D,
		BLEND_2D
	};

	class AnimationState
	{
	public:
		AnimationState(std::string name);
		virtual ~AnimationState();

		OutState& AddOutState(AnimationState* outState);
		//Returns true if all transition conditions
		//for the given out state are met; false otherwise.
		std::optional<AnimationState*> EvaluateAllTransitions(std::string key);
		std::string GetName();
		AnimationState(const AnimationState& other) = delete;
		virtual STATE_TYPE recieveStateVisitor(StateVisitorBase& visitor) = 0;
		bool operator=(const std::string name) { return m_Name == name; }
	private:
		std::string m_Name = "";

		std::unordered_map<std::string, OutState> m_OutStates;
	};

	class BlendSpace1D;
	class BlendSpace2D;
	class StateVisitorBase{
	public:
		virtual void dispatch(BlendSpace1D& state) = 0;
		virtual void dispatch(BlendSpace2D& state) = 0;
	};
#pragma endregion "AnimationState"

#pragma region "BlendSpace1D"

	class BlendSpace1D : public AnimationState
	{
	public:
		virtual ~BlendSpace1D() = default;
		struct BlendSpaceClipData
		{
			Animation::AnimationClip* clip;
			float location;
		};
		struct Current1DStateData 
		{
			Animation::AnimationClip* first = nullptr;
			Animation::AnimationClip* second = nullptr;
			float weight = 0.0f;
		};

		BlendSpace1D(std::string name, float* blendSpaceDriver, float min, float max)
			: m_Current(blendSpaceDriver), m_Min(min), m_Max(max), AnimationState(name)
		{}

		//Assumes arguments are sorted from lowest to highest
		void AddBlendNodes(const std::vector<BlendSpaceClipData> nodes);
		
		STATE_TYPE recieveStateVisitor(StateVisitorBase& visitor) override {
			visitor.dispatch(*this);
			return BLEND_1D;
		}

		Current1DStateData CalculateCurrentClips();
	private:
		std::vector<BlendSpaceClipData> m_Clips;
		float m_Min = 0.0f;
		float m_Max = 1.0f;
		float* m_Current = nullptr;
	};
#pragma endregion "BlendSpace1D"

#pragma region "BlendSpace2D"

	class BlendSpace2D : public AnimationState
	{
	public:
		struct BlendSpaceClipData2D
		{
			Animation::AnimationClip* clip;
			float locationX;
		};
		struct Current2DStateData
		{
			Animation::AnimationClip* firstTop = nullptr;
			Animation::AnimationClip* secondTop = nullptr;
			Animation::AnimationClip* firstBottom = nullptr;
			Animation::AnimationClip* secondBottom = nullptr;
			float weightTop = 0.0f;
			float weightBottom = 0.0f;
			float weightY = 0.0f;
		};

		BlendSpace2D(std::string name, float* blendSpaceDriverX, float* blendSpaceDriverY, float minX, float maxX, float minY, float maxY)
			: m_Current_X(blendSpaceDriverX), m_Current_Y(blendSpaceDriverY), m_Min_X(minX), m_Max_X(maxX), m_Min_Y(minY), m_Max_Y(maxY), AnimationState(name)
		{}
		virtual ~BlendSpace2D() = default;
		//Assumes arguments are sorted from lowest to highest
		void AddRow(float y, std::vector<BlendSpaceClipData2D>&& nodes);

		STATE_TYPE recieveStateVisitor(StateVisitorBase& visitor) override {
			visitor.dispatch(static_cast<BlendSpace2D&>(*this));
			return BLEND_2D;
		}

		Current2DStateData CalculateCurrentClips();
	private:
		std::tuple<Animation::AnimationClip*, Animation::AnimationClip*, float> GetLeftAndRightClips(size_t rowIndex);
		std::pair<std::optional<size_t>, std::optional<size_t>> GetTopAndBottomRows();
	private:
		typedef std::vector<BlendSpaceClipData2D> Row;
		std::vector<std::pair<float, Row>> m_Rows;
		float m_Min_X = -1.0f;
		float m_Max_X = 1.0f;
		float m_Min_Y = -1.0f;
		float m_Max_Y = 1.0f;
		float* m_Current_X = nullptr;
		float* m_Current_Y = nullptr;
	};
#pragma endregion "BlendSpace2D"

#pragma region "Visitors"
	class StateVisitor : public StateVisitorBase{
	public:
		StateVisitor(Animation::AnimatedModel* model) : m_AnimatedModel(model)
		{}
		virtual void dispatch(BlendSpace1D& state) override;
		virtual void dispatch(BlendSpace2D& state) override;
	private:
		Animation::AnimatedModel* m_AnimatedModel = nullptr;
	};
#pragma endregion "Visitors"

#pragma region "StateMachine"
	class AnimationStateMachine
	{
	public:
		AnimationStateMachine(size_t size);
		~AnimationStateMachine();

		//Add a new animation state.
		//Returns a pointer to the new state.
		///AnimationState* AddState(std::string name);
		BlendSpace1D* AddBlendSpace1DState
		(std::string name, float* blendSpaceDriver, float min, float max);
		BlendSpace2D* AddBlendSpace2DState
		(std::string name, float* blendSpaceDriverX, float* blendSpaceDriverY, float minX, float maxX, float minY, float maxY);
		
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
#pragma endregion "StateMachine"

#pragma endregion "AnimationMachine"
}
