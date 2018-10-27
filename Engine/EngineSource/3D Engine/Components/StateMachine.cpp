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
				data.weightY = (*m_Current_Y - m_Rows.at(rowIndices.first.value()).first) / (m_Rows.at(rowIndices.first.value()).first - m_Rows.at(rowIndices.second.value()).first);
			return data;
		}
		else
			return BlendSpace2D::Current2DStateData();

		//bool firstRowOnly = false;
		//bool firstXOnly = false;
		//auto topRowIt = std::find_if(m_Rows.begin(), m_Rows.end(),
		//	[&](const auto& row) {return row.first >= *m_Current_Y; });
		//assert(topRowIt != std::end(m_Rows));

		//if (topRowIt != std::begin(m_Rows))
		//{
		//	//Process first row only.
		//	// do only first row blend
		//	// (no lerp along Y)
		//	firstRowOnly = true;
		//}

		//auto bottomRowIt = firstRowOnly
		//	? topRowIt
		//	: topRowIt - 1;

		//auto rightTopIt = std::find_if(topRowIt->second.begin(), topRowIt->second.end(),
		//	[&](const auto& data) {return data.locationX >= *m_Current_X; });
		//assert(rightTopIt != std::end(topRowIt->second));

		//if (rightTopIt == std::begin(topRowIt->second))
		//{
		//	// no blend necessary on X, use first
		//	firstXOnly = true;
		//}

		//auto rightBottomIt = std::find_if(bottomRowIt->second.begin(), bottomRowIt->second.end(),
		//	[&](const auto& data) {return data.locationX >= *m_Current_X; });
		//assert(rightBottomIt != std::end(bottomRowIt->second));

		//if (rightBottomIt == std::begin(bottomRowIt->second))
		//{
		//	// no blend necessary on X, use first
		//}
		//auto leftTopIt = rightTopIt - 1;
		//auto leftBottomIt = rightBottomIt - 1;

		//Current2DStateData data;
		//data.firstTop = leftTopIt->clip;
		//data.secondTop = rightTopIt->clip;
		//if (!firstRowOnly)
		//{
		//	data.firstBottom = leftBottomIt->clip;
		//	data.secondBottom = rightBottomIt->clip;
		//	data.weightBottom = (*m_Current_X - leftBottomIt->locationX) / (rightBottomIt->locationX - leftBottomIt->locationX);
		//	data.weightY = (*m_Current_Y - bottomRowIt->first) / (topRowIt->first - bottomRowIt->first);
		//}

		//data.weightTop = (*m_Current_X - leftTopIt->locationX) / (rightTopIt->locationX - leftTopIt->locationX);

		//return data;
	}

	std::tuple<Animation::AnimationClip*, Animation::AnimationClip*, float> BlendSpace2D::GetLeftAndRightClips(size_t rowIndex)
{
		auto rightIt = std::find_if(m_Rows.at(rowIndex).second.begin(), m_Rows.at(rowIndex).second.end(),
			[&](const auto& e) {return *m_Current_X >= e.locationX; });

		if (rightIt != m_Rows.at(rowIndex).second.end())
		{
			if (rightIt == m_Rows.at(rowIndex).second.begin())
			{
				return { rightIt->clip, nullptr, 0.0f };
			}
			else
			{
				auto leftIt = rightIt - 1;
				float weight = (*m_Current_X - leftIt->locationX) / (rightIt->locationX - leftIt->locationX);
				return { leftIt->clip, rightIt->clip, weight };
			}
		}
		else return { nullptr, nullptr, 0.0f };
	}

	std::pair<std::optional<size_t>, std::optional<size_t>> BlendSpace2D::GetTopAndBottomRows()
	{
		auto topRowIt = std::find_if(m_Rows.begin(), m_Rows.end(),
			[&](const auto& e) { return *m_Current_Y >= e.first; });

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

}