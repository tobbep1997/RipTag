#pragma once
#include <vector>

class Enemy;
class Player;
class Grid;

class EnemyHandler
{
private:
	const float SOUND_LEVEL = 0.33f;
	const int SIGHT_LEVEL = 1700;
	const float ALERT_TIME_LIMIT = 0.8f;
	const float SUSPICIOUS_TIME_LIMIT = 2.0f;
	const float HIGH_ALERT_LIMIT = 3.0f;

private:
	std::vector<Enemy*> m_guards;
	Player * m_player;
	Grid * m_grid;

public:
	EnemyHandler();
	~EnemyHandler();

	// Add guard function rather than init for guards (?)
	void Init(std::vector<Enemy*> enemies, Player * player, Grid * grid);
	
	void Update(float deltaTime);
	void HandlePacket(unsigned char id, unsigned char * data);

private:
	void _registerThisInstanceToNetwork();

	int _getPlayerVisibility(Enemy * guard);
	void _alert(Enemy * guard, bool followSound = false);
	void _investigating(Enemy * guard);
	void _investigateSound(Enemy * guard);
	void _patrolling(Enemy * guard);
	void _highAlert(Enemy * guard, const double & dt);
	void _suspicious(Enemy * guard, const double & dt);

};