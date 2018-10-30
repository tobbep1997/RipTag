#include "StateMachine.h"
#include <assert.h>
#include <algorithm>
#include <iostream>
#include "../Model/Meshes/AnimatedModel.h"
#include <math.h>
namespace SM
{
#pragma region "AnimationState"

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
#pragma endregion "AnimationState"

#pragma region "StateMachine"
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

	SM::BlendSpace2D* AnimationStateMachine::AddBlendSpace2DState(std::string name, float* blendSpaceDriverX, float* blendSpaceDriverY, float minX, float maxX, float minY, float maxY)
	{
		BlendSpace2D* state = new BlendSpace2D(name, blendSpaceDriverX, blendSpaceDriverY, minX, maxX, minY, maxY);
		m_States.push_back(static_cast<AnimationState*>(state));
		return state;
	}

	void AnimationStateMachine::SetModel(Animation::AnimatedModel* model)
	{
		m_AnimatedModel = model;
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
#pragma endregion "StateMachine"

#pragma region "StateVisitor"

	void StateVisitor::dispatch(BlendSpace1D& state){
		auto clips = state.CalculateCurrentClips();

		if (!m_AnimatedModel)
			return;
		if (clips.first)
			m_AnimatedModel->SetPlayingClip(clips.first, true, true);
		if (clips.second)
			m_AnimatedModel->SetLayeredClip(clips.second, clips.weight, BLEND_MATCH_TIME, true);
		else
			m_AnimatedModel->SetLayeredClipWeight(0.0);
	}
	void StateVisitor::dispatch(BlendSpace2D& state) {
		auto clips = state.CalculateCurrentClips();
		if (!m_AnimatedModel)
			return;

		m_AnimatedModel->UpdateBlendspace2D(clips);


		// #todo
	}
#pragma endregion "StateVisitor"

#pragma region "BlendSpace1D"

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
#pragma endregion "BlendSpace1D"

#pragma region "BlendSpace2D"

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
}