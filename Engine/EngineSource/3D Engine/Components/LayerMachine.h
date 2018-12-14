#pragma once

#pragma region "FwdDec"
class LayerState;
class BasicLayer;
class PoseLayer;
class Pose1DLayer;
namespace Animation
{
	struct Skeleton;
	struct SkeletonPose;
	struct AnimationClip;
}
#pragma endregion "FwdDec"

#pragma region "Machine"
class LayerMachine
{
public:
	LayerMachine(Animation::Skeleton* skeleton);
	~LayerMachine();

	void UpdatePoseWithLayers(Animation::SkeletonPose& mainAnimationPose, float deltaTime);

	BasicLayer* AddBasicLayer(std::string layerName, Animation::AnimationClip* clip, float blendInTime, float blendOutTime);
	void ActivateLayer(std::string layerName);
	void ActivateLayer(std::string layerName, float loopCount);
	void Add1DLayer(std::string layerName, float* driver, std::vector<std::pair<float, Animation::AnimationClip*>> nodes);
	Pose1DLayer* Add1DPoseLayer(std::string layerName, float* driver, float min, float max, std::vector<std::pair<Animation::SkeletonPose*, float>> poses);
	void PopLayer(LayerState*);
	void PopLayer(std::string layer);
	void BlendOutLayer(std::string layer);
	uint16_t GetSkeletonJointCount();
private:
	std::unordered_map<std::string, LayerState*> m_Layers;
	std::vector<LayerState*> m_ActiveLayers;
	Animation::Skeleton* m_Skeleton{ nullptr };

	bool _mildResetIfActive(LayerState* layer);
public:
	void ActivateLayerIfInactive(std::string layer);
	void PopAll();
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

	virtual ~LayerState();

	void BlendOut();
	void BlendIn();
	void PopOnFinish();
	void SetEndlessLoop();
	void SetPlayTime(float loopCount);
	void Reset();
	void MildReset();

	std::string GetName() { return m_Name; }

	virtual std::optional<Animation::SkeletonPose> UpdateAndGetFinalPose(float deltaTime) = 0;
	bool IsPopped() const;
	std::pair<uint16_t, float> _getIndexAndProgression();
	LayerState::BLEND_STATE GetState();
private:
	bool m_IsPopped = false;

	std::string m_Name = "";
	BLEND_STATE m_BlendState = NONE;

	float m_CurrentTime = 0.0f;
	float m_ClipLength = 0.0f;
	bool m_IsEndlesslyLooping = true;
	
	float m_PlayTime = 0.0f;

protected:
	//Blend members
	float m_CurrentBlendTime = 0.0f;
	float m_BlendInTime = 0.0f;
	float m_BlendOutTime = 0.0f;

	LayerMachine* m_OwnerMachine = nullptr;

	void _updateBlend(float deltaTime);
	void _updateTime(float deltaTime);
	void _setLength(float length);
};

class BasicLayer : public LayerState
{
public:
	BasicLayer(std::string name, Animation::AnimationClip* clip, float blendInTime, float blendOutTime, LayerMachine* owner);

	virtual ~BasicLayer();

	virtual std::optional<Animation::SkeletonPose> UpdateAndGetFinalPose(float deltaTime) override;

	void MakeDriven(float* driver, float min, float max, bool affectsSpeed = true);

	void UseFirstPoseOnly(bool use = true);
private:
	Animation::AnimationClip* m_Clip;

	float* m_Driver            { nullptr };
	float m_CurrentDriverWeight{ 1.0f };
	float m_DriverMin          { 0.0f };
	float m_DriverMax          { 0.0f };
	bool m_DriverAffectsSpeed  { true };
	bool m_FirstPoseOnly       { false };

	void _updateDriverWeight();
};

class PoseLayer : public LayerState
{
public:
	PoseLayer(std::string name, Animation::SkeletonPose* pose, LayerMachine* owner);
	virtual ~PoseLayer();

	virtual std::optional<Animation::SkeletonPose> UpdateAndGetFinalPose(float deltaTime) override;

	void MakeDriven(float* driver, float min, float max);
private:
	Animation::SkeletonPose* m_Pose{};

	float* m_Driver{ nullptr };
	float m_CurrentDriverWeight{ 1.0f };
	float m_DriverMin{ 0.0f };
	float m_DriverMax{ 0.0f };

	void _updateDriverWeight();
};


class Pose1DLayer : public LayerState
{
public:
	struct PosesAndWeight 
	{
		Animation::SkeletonPose* firstPose{ nullptr };
		Animation::SkeletonPose* secondPose{ nullptr };
		float weight = 0.0f;
	};
	Pose1DLayer(
		std::string name,
		float* driver,
		float min,
		float max,
		std::vector<std::pair<Animation::SkeletonPose*, float>> poses,
		LayerMachine* owner);

	virtual ~Pose1DLayer();

	void UseSmoothDriver(bool use = true);

	virtual std::optional<Animation::SkeletonPose> UpdateAndGetFinalPose(float deltaTime) override;

private:
	std::vector<std::pair<Animation::SkeletonPose*, float>> m_Poses{};

	float* m_Driver{ nullptr };
	float m_LastDriver{ 0.0f };
	float m_DriverMin{ 0.0f };
	float m_DriverMax{ 0.0f };
	bool m_UseSmoothDriver{ true };
	PosesAndWeight GetPosesAndWeight(float deltaTime);
};
#pragma endregion "States"
