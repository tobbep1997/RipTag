#pragma once

#pragma region "FwdDec"
class LayerState;
#pragma endregion "FwdDec"

#pragma region "Machine"
class LayerMachine
{
public:
	LayerMachine();
	~LayerMachine();

	void AddLayer(std::string layerName);
	void PopLayer(LayerState*);
private:
	std::vector<LayerState*> m_ActiveLayers;
	std::unordered_map<std::string, LayerState*> m_Layers;
};
#pragma endregion "Machine"

#pragma region "States"

class LayerState
{
public:
	enum BLEND_STATE
	{
		NONE,
		BLENDING_IN,
		BLENDING_OUT
	};

	LayerState(std::string name, float blendInTime, float blendOutTime, LayerMachine* owner)
		: m_Name(name), m_BlendState(BLENDING_IN), m_OwnerMachine(owner)
		, m_BlendInTime(blendInTime), m_BlendOutTime(blendOutTime)
	{}

	~LayerState();

	void BlendOut();
	void PopOnFinish();
	void Reset();

	virtual std::optional<Animation::SkeletonPose> UpdateAndGetFinalPose(float deltaTime) = 0;
	bool IsPopped() const;
	void _getIndexAndProgression();
private:
	bool m_IsPopped = false;

	std::string m_Name = "";
	BLEND_STATE m_BlendState = NONE;
	LayerMachine* m_OwnerMachine = nullptr;

	float m_CurrentTime = 0.0f;
	float m_ClipLength = 0.0f;
	bool m_IsLooping = true;

	//Blend members
	float m_CurrentBlendTime = 0.0f;
	float m_BlendInTime = 0.0f;
	float m_BlendOutTime = 0.0f;

protected:
	void _updateBlend(float deltaTime);
	void _updateTime(float deltaTime);
	void _setLength(float length);
};

class BasicLayer : public LayerState
{
public:
	BasicLayer(std::string name, Animation::AnimationClip* clip, float blendInTime, float blendOutTime, LayerMachine* owner)
		: LayerState(name, blendInTime, blendOutTime, owner), m_Clip(clip)
	{
		LayerState::_setLength(m_Clip->m_FrameCount / 24.0f);
	}

	~BasicLayer();

	virtual std::optional<Animation::SkeletonPose> UpdateAndGetFinalPose(float deltaTime) override;

private:
	Animation::AnimationClip* m_Clip;
};

#pragma endregion "States"
