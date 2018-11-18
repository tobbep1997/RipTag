#include "EnginePCH.h"
#include "StateMachine.h"

namespace SM
{
#pragma region "AnimationState"

	AnimationState::AnimationState(std::string name)
		: m_Name(name)
	{}

	std::pair<AnimationState*, float> AnimationState::EvaluateAll()
{

		for (auto& outState : m_OutStates)
		{
			if (std::all_of(outState.second.transitions.begin(), outState.second.transitions.end(), [](const UniqueTransition& elem) {return elem->Evaluate(); }))
				return std::make_pair(outState.second.state, 0.4f);
		}
		return std::make_pair(nullptr, .0f);
	}

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
		SM::OutState* t;
		try
		{
			t = &(m_OutStates.at(key));
		}
		catch (std::out_of_range oor)
		{
			return std::nullopt;
		}

		if (std::all_of(t->transitions.begin(), t->transitions.end(), [](const UniqueTransition& elem) {return elem->Evaluate(); }))
			return { t->state };
		return std::nullopt;
	}

	std::string AnimationState::GetName()
	{
		return m_Name;
	}
#pragma endregion "AnimationState"

#pragma region "StateMachine"
	AnimationStateMachine::AnimationStateMachine(size_t size)
	{
		m_States.reserve(size);
	}

	AnimationStateMachine::~AnimationStateMachine()
	{
		for (auto& i : m_States)
			delete i.second;
	}

	SM::BlendSpace1D* AnimationStateMachine::AddBlendSpace1DState(std::string name, float* blendSpaceDriver, float min, float max)
	{
		BlendSpace1D* state = new BlendSpace1D(name, blendSpaceDriver, min, max);
		m_States.insert(std::make_pair(name, static_cast<AnimationState*>(state)));
		return state;
	}

	SM::BlendSpace1DAdditive* AnimationStateMachine::AddBlendSpace1DAdditiveState(std::string name, float* blendSpaceDriver, float min, float max)
	{
		BlendSpace1DAdditive* state = new BlendSpace1DAdditive(name, blendSpaceDriver, min, max);
		m_States.insert(std::make_pair(name, static_cast<AnimationState*>(state)));
		return state;
	}

	SM::BlendSpace2D* AnimationStateMachine::AddBlendSpace2DState(std::string name, float* blendSpaceDriverX, float* blendSpaceDriverY, float minX, float maxX, float minY, float maxY)
	{
		BlendSpace2D* state = new BlendSpace2D(name, blendSpaceDriverX, blendSpaceDriverY, minX, maxX, minY, maxY);
		m_States.insert(std::make_pair(name, static_cast<AnimationState*>(state)));
		return state;
	}

	SM::LoopState* AnimationStateMachine::AddLoopState(std::string name, Animation::AnimationClip* clip)
	{
		LoopState* state = new LoopState(name);
		state->SetClip(clip);
		m_States.insert(std::make_pair(name, static_cast<AnimationState*>(state)));
		return state;
	}

	SM::PlayOnceState* AnimationStateMachine::AddPlayOnceState(std::string name, Animation::AnimationClip* clip)
	{
		PlayOnceState* state = new PlayOnceState(name);
		state->SetClip(clip);
		m_States.insert(std::make_pair(name, static_cast<AnimationState*>(state)));
		return state;
	}

	void AnimationStateMachine::SetModel(Animation::AnimationPlayer* model)
	{
		m_AnimationPlayer = model;
	}

	void AnimationStateMachine::SetState(std::string stateName)
	{
		m_CurrentState = m_States.at(stateName);
		if (m_AnimationPlayer)
			m_AnimationPlayer->Play();
	}

	void AnimationStateMachine::SetStateIfAllowed(std::string stateName)
	{
		auto stateOpt = m_CurrentState->EvaluateAllTransitions(stateName);
		if (stateOpt.has_value())
			m_CurrentState = stateOpt.value();
	}

	SM::AnimationState& AnimationStateMachine::GetCurrentState()
	{
		return *m_CurrentState;
	}

	AnimationState * AnimationStateMachine::GetPreviousState()
	{
		return m_BlendFromState;
	}

	bool AnimationStateMachine::UpdateCurrentState()
{
		if (!m_CurrentState)
			return false;
		//returns the first state that has all conditions satisfied, if any.
		auto state = m_CurrentState->EvaluateAll();

		if (state.first)
		{
			m_BlendFromState = m_CurrentState;
			m_BlendFromState->LockCurrentValues();
			m_CurrentState = state.first;
			m_RemainingBlendTime = m_TotalBlendTime = (state.second - m_RemainingBlendTime);
		}
		return true;
	}

	float AnimationStateMachine::UpdateBlendFactor(float deltaTime)
	{
		if (!m_BlendFromState)
			return 0.0;

		m_RemainingBlendTime -= deltaTime;
		if (m_RemainingBlendTime < 0.0f)
		{
			m_RemainingBlendTime = 0.0;
			m_TotalBlendTime = 0.0;
			m_BlendFromState = nullptr;
			return 0.0;
		}
		else
		{
			return m_RemainingBlendTime / m_TotalBlendTime;
		}
	}



#pragma endregion "StateMachine"

#pragma region "StateVisitor"

	Animation::SkeletonPose StateVisitor::dispatch(BlendSpace1D& state){
		auto clips = state.CalculateCurrentClips();

		if (!m_AnimationPlayer)
			return Animation::SkeletonPose();

		//if (clips.first)
		//	m_AnimationPlayer->SetPlayingClip(clips.first, true, true);
		//if (clips.second)
		//	m_AnimationPlayer->SetLayeredClip(clips.second, clips.weight, BLEND_MATCH_TIME, true);
		//else
		//	m_AnimationPlayer->SetLayeredClipWeight(0.0);

		return std::move(m_AnimationPlayer->UpdateBlendspace1D(clips));
	}
	Animation::SkeletonPose StateVisitor::dispatch(BlendSpace2D& state) {
		BlendSpace2D::Current2DStateData clips = state.CalculateCurrentClips();
		if (!m_AnimationPlayer)
			return Animation::SkeletonPose();

		return std::move(m_AnimationPlayer->UpdateBlendspace2D(clips));
	}
	Animation::SkeletonPose StateVisitor::dispatch(LoopState & state)
	{
		if (!m_AnimationPlayer)
			return Animation::SkeletonPose();

		m_AnimationPlayer->UpdateLooping(state.GetClip());
	}

	Animation::SkeletonPose StateVisitor::dispatch(AutoTransitionState& state)
	{
		return Animation::SkeletonPose();
	}

	Animation::SkeletonPose StateVisitor::dispatch(PlayOnceState& state)
	{
		if (!m_AnimationPlayer)
			return Animation::SkeletonPose();

		m_AnimationPlayer->UpdateOnce(state.GetClip());
		
		//#todo
		return Animation::SkeletonPose();
	}

#pragma endregion "StateVisitor"

#pragma region "LayerVisitor"

	std::optional<Animation::SkeletonPose> LayerVisitor::dispatch(BlendSpace1DAdditive& state)
	{
		assert(m_AnimationPlayer && "LayerVisitor missing animation player");
		//#todo all layers

		auto clips = state.CalculateCurrent(m_AnimationPlayer->GetCachedDeltaTime());
		float b = clips.weight;

		if (clips.second)
			return m_AnimationPlayer->_BlendSkeletonPoses(&clips.first->m_skeletonPoses[0], &clips.second->m_skeletonPoses[0], clips.weight, clips.first->m_skeleton->m_jointCount);
		else if (clips.first)
			return Animation::MakeSkeletonPose(clips.first->m_skeletonPoses[0], clips.first->m_skeleton->m_jointCount);
		else
			return std::nullopt;

	}

#pragma endregion "LayerVisitor"

#pragma region "BlendSpace1D"

	void BlendSpace1D::AddBlendNodes(const std::vector<BlendSpaceClipData> nodes)
	{
		std::copy(nodes.begin(), nodes.end(), std::back_inserter(m_Clips));
	}

	std::optional<Animation::SkeletonPose> BlendSpace1D::recieveStateVisitor(StateVisitorBase& visitor)
	{
		return visitor.dispatch(*this);
	}

	BlendSpace1D::Current1DStateData BlendSpace1D::CalculateCurrentClips()
	{
		//#todo check and get weight

		//If we have a locked value, use it; else, use the driver value
		const float currentValue = (m_LockedValue.has_value())
			? m_LockedValue.value()
			: *m_Current;

		auto it = std::find_if(m_Clips.begin(), m_Clips.end(),
			[&](const auto& data) {return data.location >= currentValue; });

		if (it != m_Clips.end())
		{
			Current1DStateData data;
			//std::cout << it->location << std::endl;
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
				data.weight = (currentValue - (it - 1)->location) / (it->location - (it - 1)->location);
			}
			return data;
		}
		else if (m_Clips.back().location < currentValue)
		{
			Current1DStateData data;
			data.first = (m_Clips.rbegin() + 1)->clip;
			data.second = m_Clips.rbegin()->clip;
			data.weight = 1.0f;
			return data;
		}
		else return { nullptr, nullptr, 0.0f};
	}

	void BlendSpace1D::LockCurrentValues()
	{
		m_LockedValue = *m_Current;
	}

#pragma endregion "BlendSpace1D"

#pragma region "BlendSpace2D"

	void BlendSpace2D::AddRow(float y, std::vector<BlendSpaceClipData2D>&& nodes)
	{
#ifdef _DEBUG
		auto it = std::find_if(m_Rows.begin(), m_Rows.end(), [&y](const auto& e) {return e.first == y; });
		assert(it == m_Rows.end());
#endif
		m_Rows.emplace_back(y, std::move(nodes));
	}

	std::optional<Animation::SkeletonPose> BlendSpace2D::recieveStateVisitor(StateVisitorBase & visitor)
	{
		return std::move(visitor.dispatch(static_cast<BlendSpace2D&>(*this)));
	}

	BlendSpace2D::Current2DStateData BlendSpace2D::CalculateCurrentClips()
	{ //#todo check end()
		using Animation::AnimationClip;
		using std::tuple;
		
		auto rowIndices = GetTopAndBottomRows();
		if (rowIndices.first.has_value())
		{
			tuple<AnimationClip*, AnimationClip*, float> clipsAndWeightFirst = { nullptr, nullptr, 0.0f };
			tuple<AnimationClip*, AnimationClip*, float> clipsAndWeightSecond = { nullptr, nullptr, 0.0f };

			clipsAndWeightFirst = GetLeftAndRightClips(rowIndices.first.value());

			if (rowIndices.second.has_value())
			{
				clipsAndWeightSecond = GetLeftAndRightClips(rowIndices.second.value());
			}

			Current2DStateData data;
			data.firstTop = std::get<0>(clipsAndWeightFirst);
			data.secondTop = std::get<1>(clipsAndWeightFirst);
			data.weightTop = std::get<2>(clipsAndWeightFirst);

			data.firstBottom = std::get<0>(clipsAndWeightSecond);
			data.secondBottom = std::get<1>(clipsAndWeightSecond);
			data.weightBottom = std::get<2>(clipsAndWeightSecond);

			if (rowIndices.second.has_value())
				data.weightY = (*m_Current_Y - m_Rows.at(rowIndices.first.value()).first) / (m_Rows.at(rowIndices.second.value()).first - m_Rows.at(rowIndices.first.value()).first);
			return data;
		}
		else
			return BlendSpace2D::Current2DStateData();
	}

	void BlendSpace2D::LockCurrentValues()
	{
		m_LockedX = *m_Current_X;
		m_LockedY = *m_Current_Y;
	}

	std::tuple<Animation::AnimationClip*, Animation::AnimationClip*, float> BlendSpace2D::GetLeftAndRightClips(size_t rowIndex)
{
		auto rightIt = std::find_if(m_Rows.at(rowIndex).second.begin(), m_Rows.at(rowIndex).second.end(),
			[&](const auto& e) {return e.locationX >= *m_Current_X; });

		if (rightIt != m_Rows.at(rowIndex).second.end())
		{
			if (rightIt == m_Rows.at(rowIndex).second.begin())
			{
				return { rightIt->clip, nullptr, 0.0f }; //use first clip without weight if we're at begin or 
			}
			else
			{
				auto leftIt = rightIt - 1;
				float weight = (*m_Current_X - leftIt->locationX) / (rightIt->locationX - leftIt->locationX);
				return { leftIt->clip, rightIt->clip, weight };
			}
		}
		else return { (m_Rows.at(rowIndex).second.end()-1)->clip, nullptr, 0.0f }; //use last clip if we exceed positive limit
	}

	std::pair<std::optional<size_t>, std::optional<size_t>> BlendSpace2D::GetTopAndBottomRows()
	{
		auto topRowIt = std::find_if(m_Rows.begin(), m_Rows.end(),
			[&](const auto& e) { return e.first >= *m_Current_Y; });

		if (topRowIt != std::end(m_Rows))
		{
			if (topRowIt == std::begin(m_Rows))
			{
				return std::make_pair(std::optional<size_t>{0}, std::nullopt);
			}
			else
			{
				size_t topRowIndex = static_cast<size_t>(std::distance(std::begin(m_Rows), topRowIt));
				assert(topRowIndex > 0);
				return std::make_pair(std::optional<size_t>{topRowIndex - 1}, std::optional<size_t>{topRowIndex});
			}
		}
		else return std::make_pair(std::nullopt, std::nullopt);
	}
#pragma endregion "BlendSpace2D"

#pragma region "AutoTransState"
	AutoTransitionState::AutoTransitionState(std::string name)
		: AnimationState(name)
	{
	}

	AutoTransitionState::~AutoTransitionState()
	{

	}

	std::optional<Animation::SkeletonPose> AutoTransitionState::recieveStateVisitor(StateVisitorBase & visitor)
	{
		return std::move(visitor.dispatch(*this));
	}

#pragma endregion "AutoTransState"

#pragma region "PlayOnceState"

	PlayOnceState::PlayOnceState(std::string name)
		: AnimationState(name)
	{
	}

	void PlayOnceState::SetClip(Animation::AnimationClip* clip)
	{
		m_Clip = clip;
	}

	Animation::AnimationClip * PlayOnceState::GetClip()
	{
		return m_Clip;
	}

	std::optional<Animation::SkeletonPose> PlayOnceState::recieveStateVisitor(StateVisitorBase& visitor)
	{
		auto pose = visitor.dispatch(*this);
		return pose;
	}

#pragma endregion "PlayOnceState"

#pragma region "LoopState"
	LoopState::LoopState(std::string name) 
		: AnimationState(name)
	{
	}

	LoopState::~LoopState()
	{

	}

	void LoopState::SetClip(Animation::AnimationClip * clip)
	{
		m_Clip = clip;
	}

	Animation::AnimationClip* LoopState::GetClip()
	{
		return m_Clip;
	}

	std::optional<Animation::SkeletonPose> LoopState::recieveStateVisitor(StateVisitorBase & visitor)
	{
		return std::move(visitor.dispatch(*this));
	}

#pragma endregion "LoopState"

	void BlendSpace1DAdditive::AddBlendNodes(const std::vector<BlendSpaceLayerData> nodes)
	{
		std::copy(nodes.begin(), nodes.end(), std::back_inserter(m_Layers));
	}

	void BlendSpace1DAdditive::LockCurrentValues()
	{

	}

	std::optional<Animation::SkeletonPose> BlendSpace1DAdditive::recieveStateVisitor(StateVisitorBase & visitor)
	{
		return visitor.dispatch(*this);
	}

	SM::BlendSpace1D::Current1DStateData BlendSpace1DAdditive::CalculateCurrent(float deltaTime)
	{
		//#todo check and get weight

		const float currentValue = *m_Current;

		auto it = std::find_if(m_Layers.begin(), m_Layers.end(),
			[&](const auto& data) {return data.location >= currentValue; });

		if (it != m_Layers.end())
		{
			BlendSpace1D::Current1DStateData data;
			//std::cout << it->location << std::endl;
			it == m_Layers.begin()
				? std::make_pair(it->clip, nullptr)
				: std::make_pair((it - 1)->clip, it->clip);

			if (it == m_Layers.begin())
			{
				data.first = it->clip;
			}
			else
			{
				data.first = (it - 1)->clip;
				data.second = it->clip;
				data.weight = (currentValue - (it - 1)->location) / (it->location - (it - 1)->location);
			}
			return data;
		}
		else if (m_Layers.back().location < currentValue)
		{
			BlendSpace1D::Current1DStateData data;
			data.first = (m_Layers.rbegin() + 1)->clip;
			data.second = m_Layers.rbegin()->clip;
			data.weight = 1.0f;
			return data;
		}
		else return { nullptr, nullptr, 0.0f };
	}

	Animation::SkeletonPose StateVisitorBase::dispatch(BlendSpace1D& state)
	{
		return Animation::SkeletonPose();
	}

	Animation::SkeletonPose StateVisitorBase::dispatch(BlendSpace2D& state)
	{
		return Animation::SkeletonPose();
	}

	Animation::SkeletonPose StateVisitorBase::dispatch(LoopState& state)
	{
		return Animation::SkeletonPose();
	}

	Animation::SkeletonPose StateVisitorBase::dispatch(AutoTransitionState& state)
	{
		return Animation::SkeletonPose();
	}

	Animation::SkeletonPose StateVisitorBase::dispatch(PlayOnceState& state)
	{
		return Animation::SkeletonPose();
	}

	std::optional<Animation::SkeletonPose> StateVisitorBase::dispatch(BlendSpace1DAdditive& state)
	{
		return std::nullopt;
	}
}