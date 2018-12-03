#pragma once

struct Node;
class Grid;
class VisibilityComponent;
class Enemy;

enum AIState
{
	NoState,
	Investigating_Sight,
	Investigating_Sound,
	Investigating_Room,
	High_Alert,
	Suspicious,
	Scanning_Area,
	Patrolling,
	Possessed,
	Disabled,

};

enum AITransitionState
{
	NoTransitionState,
	Alerted,
	InvestigateSound,
	InvestigateSight,
	Observe,
	SearchArea,
	ReturnToPatrol,
	BeingPossessed,
	BeingDisabled,
	ExitingPossess,
	ExitingDisable,
};

class AI
{
public:
	struct SoundLocation
	{
		float percentage = 0.0f;
		DirectX::XMFLOAT3 soundPos = DirectX::XMFLOAT3(0, 0, 0);
	};

	enum KnockOutType
	{
		Stoned,
		Possessed
	};
private:
	Enemy* m_owner = nullptr;
	AudioEngine::SoundDesc m_alerted;

	//AI Behavior constants
	const float SOUND_LEVEL = 0.33f;
	const int SIGHT_LEVEL = 1700;
	const float ALERT_TIME_LIMIT = 0.45f;
	const float SUSPICIOUS_TIME_LIMIT = 3.0f;
	const float SEARCH_ROOM_TIME_LIMIT = 20.0f;
	const float HIGH_ALERT_LIMIT = 3.0f;
	const float CHECK_TORCHES_INTERVALL = 3.0f;
	const float CHECK_TORCHES_RADIUS = 5.0f;

	//stateData
	float m_HighAlertTime = 0.f;
	float m_actTimer = 0.0f;
	float m_searchTimer = 0.0f;
	float m_checkTorchesTimer = 0.0f;
	AITransitionState m_transState = AITransitionState::NoTransitionState;
	AIState m_state = AIState::Patrolling;
	float lastSearchDirX = 0;
	float lastSearchDirY = 0;


	//Pathing
	Grid* m_grid;
	int m_currentPathNode = 0;
	std::vector<Node*> m_path;
	std::vector<Node*> m_alertPath;

	struct lerpVal
	{
		bool newNode = true;
		bool turnState = false;
		bool next = false;
		float timer = 0.0f;
		DirectX::XMFLOAT2 lastDir = { 0.0f,0.0f };
		DirectX::XMFLOAT2 middleTarget = { 0.0f,0.0f };
	};
	
	lerpVal m_lv;

public:
	AI();
	AI(Enemy * owner);
	virtual ~AI();

	void handleStates(const double deltaTime);
	void handleStatesClient(const double deltaTime);

	void SetPathVector(std::vector<Node*>  path);
	Node * GetCurrentPathNode() const;
	int GetCurrentPathNodeGridID() const;
	void SetAlertVector(std::vector<Node*> alertPath);
	
	bool GetPathEmpty() const;
	size_t GetAlertPathSize() const;
	Node * GetAlertDestination() const;

	AIState getAIState() const;
	void setAIState(AIState state);

	AITransitionState getTransitionState() const;
	void setTransitionState(AITransitionState state);

	void setGrid(Grid* grid);

	DirectX::XMFLOAT2 GetDirectionToPlayer(const DirectX::XMFLOAT4A & player, Camera & playerCma);

private:

	//Transistion States
	void _onAlerted();
	void _onInvestigateSound();
	void _onInvestigateSight();
	void _onObserve();
	void _onSearchArea();
	void _onReturnToPatrol();
	void _onBeingPossessed();
	void _onBeingDisabled();
	void _onExitingPossessed();
	void _onExitingDisabled();

	//States
	void _investigatingSight(const double deltaTime);
	void _investigatingSound(const double deltaTime);
	void _investigatingRoom(const double deltaTime);
	void _highAlert(const double deltaTime);
	void _suspicious(const double deltaTime);
	void _scanningArea(const double deltaTime);
	void _patrolling(const double deltaTime);
	void _possessed(const double deltaTime);
	void _disabled(const double deltaTime);

	//Pathing
	void _Move(Node * nextNode, double deltaTime);
	bool _MoveTo(Node * nextNode, double deltaTime);
	bool _MoveToAlert(Node * nextNode, double deltaTime);
	float _getPathNodeRotation(DirectX::XMFLOAT2 first, DirectX::XMFLOAT2 last);

	//Actions
	void _checkTorches(float dt);

};

